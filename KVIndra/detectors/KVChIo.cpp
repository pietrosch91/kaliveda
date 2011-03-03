/***************************************************************************
$Id: KVChIo.cpp,v 1.55 2008/02/21 10:14:38 franklan Exp $
                          kvchio.cpp  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "Riostream.h"
#include "KVChIo.h"
#include "KVINDRA.h"
//for GetRunCalibrationPeaks
#include "KVINDRADB.h"
#include "KVDBPeak.h"
#include "KVDBAlphaPeak.h"
#include "KVDBElasticPeak.h"
#include "KVDBRun.h"

#define MAX_CANAL_GG 4000

ClassImp(KVChIo);
//_______________________________________________________________________
//KVChIo
//
//Child class of KVDetector, specifically describing the
//ionisation chamber detectors of the INDRA multidetector array.
//These consist of:
//      a 2.5 micron mylar window
//      5 cm of C3F8 (pressure varies) ---> active layer
//      a 2.5 micron mylar window
//
//Type of detector : "CI"

//______________________________________________________________________________

void KVChIo::init()
{
   //initialise non-persistent pointers
   fChVoltGG=0;
   fChVoltPG=0;
   fVoltE=0;
   fSegment = 0;
   fPGtoGG_0 = 0;
   fPGtoGG_1 = 15;
}

KVChIo::KVChIo()
{
   //Default ctor
   //This ctor (which in turn calls the KVDetector default ctor) must exist in
   //order for Cloning of detectors to work (as used in KVTelescope::AddDetector).
   //Do not replace this ctor by giving a default value for the first argument of KVChIo(Float_t,Float_t).
   //
   init();
}

//______________________________________________________________________________
KVChIo::KVChIo(Float_t pressure, Float_t thick):KVINDRADetector("Myl", 2.5*Units::um)
{
   // Make an INDRA ChIo: 2.5micron mylar windows enclosing 'thick' cm of C3F8,
   // give gas pressure in mbar
   // By default 'thick'=5cm
   // The type of these detectors is "CI"

   //gas layer
   KVMaterial *mat = new KVMaterial("C3F8", thick, pressure*Units::mbar);
   AddAbsorber(mat);
   SetActiveLayer(mat);         //gas is the active layer
   // mylar for second window
   mat = new KVMaterial("Myl", 2.5*Units::um);
   AddAbsorber(mat);

   SetType("CI");
   init();
}

//____________________________________________________________________________________________

KVChIo::~KVChIo()
{
}

//____________________________________________________________________________________________
Int_t KVChIo::GetCanalPGFromVolts(Float_t volts)
{
   //Return raw PG channel number corresponding to a given detector signal in volts
   //
   //Any change in the coder pedestal between the current run and the effective pedestal
   //of the channel-volt calibration (GetCanal(V=0)) is automatically corrected for.
   //
   //Returns -1 if PG <-> Volts calibration is not available


      if (!fChVoltPG || !fChVoltPG->GetStatus())
         return -1;
      Int_t chan = TMath::Nint(fChVoltPG->Invert(volts) + GetPedestal("PG") - fChVoltPG->Invert(0));
      return chan;

}

//____________________________________________________________________________________________
Int_t KVChIo::GetCanalGGFromVolts(Float_t volts)
{
   //Return raw GG channel number corresponding to a given detector signal in volts
   //
   //Any change in the coder pedestal between the current run and the effective pedestal
   //of the channel-volt calibration (GetCanal(V=0)) is automatically corrected for.
   //
   //Returns -1 if PG <-> Volts calibration is not available

      if (!fChVoltGG || !fChVoltGG->GetStatus())
         return -1;
      Int_t chan = TMath::Nint(fChVoltGG->Invert(volts) + GetPedestal("GG") - fChVoltGG->Invert(0));
      return chan;
}


//___________________________________________________________________________________________

void KVChIo::SetACQParams()
{
   //Setup acquistion parameters for this ChIo.
   //Do not call before ChIo name has been set.

   AddACQParamType("GG");
   AddACQParamType("PG");
   AddACQParamType("T");

}

//____________________________________________________________________________________________

void KVChIo::SetCalibrators()
{
   //Set up calibrators for this detector. Call once name has been set.
   KVCalibrator*c=new KVChannelVolt("GG", this);
   if(!AddCalibrator(c)) delete c;
   c=new KVChannelVolt("PG", this);
   if(!AddCalibrator(c)) delete c;
   c=new KVVoltEnergy(this);
   if(!AddCalibrator(c)) delete c;
   fVoltE = (KVVoltEnergy *) GetCalibrator("Volt-Energy");
   fChVoltPG  =  (KVChannelVolt *) GetCalibrator("Channel-Volt PG");
   fChVoltGG  =  (KVChannelVolt *) GetCalibrator("Channel-Volt GG");
}

//__________________________________________________________________________________________________________________________

Float_t KVChIo::GetGGfromPG(Float_t PG)
{
   //Calculate GG from PG when GG is saturated.
   //If PG is not given as argument, the current value of the detector's PG ACQParam is read
   //The GG value returned includes the current pedestal:
   //      GG = pied_GG + alpha + beta * (PG - pied_PG)
   //alpha, beta coefficients were obtained by fitting (GG-pied) vs. (PG-pied) for data.
   if (PG < 0)
      PG = (Float_t) GetPG();
   Float_t GG =
       GetPedestal("GG") + fPGtoGG_0 + fPGtoGG_1 * (PG -
                                                    GetPedestal("PG"));
   return GG;
}

//__________________________________________________________________________________________________________________________

Double_t KVChIo::GetELossMylar(UInt_t z, UInt_t a, Double_t egas, Bool_t stopped)
{
   //Based on energy loss in gas, calculates sum of energy losses in mylar windows
   //from energy loss tables.
   //If argument 'egas' not given, KVChIo::GetEnergy() is used.
   //if stopped=kTRUE, we give the correction for a particle which stops in the detector
   //(by default we assume the particle continues after the detector)
   //
   //If the dE energy loss in the gas is > maximum theoretical dE, GetMaxDeltaE(z,a),
   //this calculation is not valid. The mylar energy loss is calculated for a dE
   //corresponding to dE = GetMaxDeltaE(z,a), and then we scale up according
   //to: dE_Mylar = dE_Mylar_max * (dE_measured / dE_max).
   //
   //If the calculated energy loss in the mylar is <0 (i.e. if calculated incident energy
   //of particle is > (dE in gas + residual energy)), we return 0.

   egas = ((egas < 0.) ? GetEnergy() : egas);
   if (egas <= 0.)
      return 0.0;               //check measured (calibrated) energy in gas is reasonable (>0)

   Bool_t maxDE = kFALSE;

   //egas > max possible ?
   Double_t de_measured = 0.;
   if (egas > GetMaxDeltaE(z, a)) {
      de_measured = egas;
      egas = GetMaxDeltaE(z, a);
      maxDE = kTRUE;
//      if(de_measured-egas>2.0)
//         Warning("GetELossMylar","%s: Measured de_ChIo (%f) is greater than maximum for Z=%d (%f)",
//            GetName(), de_measured, (int)z, egas);
   }
   enum KVIonRangeTable::SolType solution = KVIonRangeTable::kEmax;
   if(stopped) solution = KVIonRangeTable::kEmin;
   //calculate incident energy
   Double_t e_inc = GetIncidentEnergy(z, a, egas, solution);

   //calculate residual energy
   Double_t e_res = GetERes(z, a, e_inc);

   //calculate mylar energy
   Double_t emylar = e_inc - e_res - egas;

   emylar = ((emylar < 0.) ? 0. : emylar);

   if (maxDE){
      emylar *= (de_measured/egas);
   }

   return emylar;
}

//__________________________________________________________________________________________________________________________

Double_t KVChIo::GetCorrectedEnergy(UInt_t z, UInt_t a, Double_t egas, Bool_t transmission)
{
   //Redefinition of KVDetector method.
   //Based on energy loss in gas, calculates correction for mylar windows
   //from energy loss tables. Returns total energy loss in mylar+gas+mylar
   //If argument 'egas' not given, KVChIo::GetEnergy() is used
   //If transmission=kFALSE we give the correction for a particle stopping in the
   //detector (i.e. we calculate the incident energy for a particle with dE<dE_max)
   //By default transmission=kTRUE & we assume the particle continues after the
   //detector.
   //
   //If the dE energy loss in the gas is > maximum theoretical dE (GetMaxDeltaE)
   //this calculation is not valid. The mylar energy loss is calculated for a dE
   //corresponding to dE = GetMaxDeltaE(z,a), and then we scale up according
   //to: dE_Mylar = dE_Mylar_max * (dE_measured / dE_max)

   egas = ((egas < 0.) ? GetEnergy() : egas);
   if( egas <=0 ) return 0;
   Double_t emyl = GetELossMylar(z, a, egas, !transmission);
   return (egas + emyl);
}

//_______________________________________________________________________________

Double_t KVChIo::GetCalibratedEnergy()
{
   //Calculate energy in MeV from coder values.
   //Returns 0 if calibration not ready or detector not fired
   //(we require that at least one acquisition parameter have a value
   //greater than the current pedestal value)

   if (IsCalibrated() && Fired("Pany")){
      return (fVoltE->Compute( GetVolts() ));
   }
   return 0;
}

//_______________________________________________________________________________

Double_t KVChIo::GetVoltsFromEnergy(Double_t e)
{
   //Inverts calibration, i.e. calculates volts for a given energy loss (in MeV)

   if (fVoltE->GetStatus()){
      return (fVoltE->Invert( e ));
   }
   return 0;
}

//____________________________________________________________________________________________

Double_t KVChIo::GetVoltsFromCanalPG(Double_t chan)
{
   //Return calibrated detector signal in Volts calculated from PG channel number.
   //If "chan" is not given, the value of the "PG" acquisition parameter read from
   //data for this detector is used to calculate the signal.
   //If the PG parameter is not present (=-1) or no calib we return 0.
   //Any change in the coder pedestal between the current run and the effective pedestal
   //of the channel-volt calibration (GetCanal(V=0)) is automatically corrected for.

      if (!fChVoltPG || !fChVoltPG->GetStatus())
         return 0;

      if (!chan) {
         chan = GetPG();
      }
      if (chan < -0.5)
         return 0.;          //PG parameter absent
      //correct for pedestal drift
      chan = chan - (Double_t) GetPedestal("PG") + fChVoltPG->Invert(0.);
      return (fChVoltPG->Compute(chan));
}

//____________________________________________________________________________________________
Double_t KVChIo::GetVoltsFromCanalGG(Double_t chan)
{
   //Return calibrated detector signal in Volts calculated from GG channel number.
   //If "chan" is not given, the value of the "GG" acquisition parameter read from
   //data for this detector is used to calculate the signal.
   //If the GG parameter is not present (=-1) or no calib we return 0.
   //Any change in the coder pedestal between the current run and the effective pedestal
   //of the channel-volt calibration (GetCanal(V=0)) is automatically corrected for.

      if (!fChVoltGG || !fChVoltGG->GetStatus())
         return 0;

      if (!chan) {
         chan = GetGG();
      }
      if (chan < -0.5)
         return 0.;          //GG parameter absent
      //correct for pedestal drift
      chan = chan - (Double_t) GetPedestal("GG") + fChVoltGG->Invert(0);
      return (fChVoltGG->Compute(chan));

}

Double_t KVChIo::GetVolts()
{
   //Returns Volts for this detector calculated from current PG coder values.
   //We only use PG, as the two channel-volt calibrations do not coincide and so the passage
   //from GG to PG produces a discontinuity (unless only GG calibration is available, then
   //we convert PG to GG and use the GG calibration)
   //Returns 0 if no calibration available

      if (fChVoltPG && fChVoltPG->GetStatus()) {
         return GetVoltsFromCanalPG();
      }
      else if (fChVoltGG && fChVoltGG->GetStatus()) {
         return GetVoltsFromCanalGG(GetGGfromPG());
      }

   return 0;
}


//____________________________________________________________________________________________

Double_t KVChIo::GetEnergy()
{
   //Redefinition of KVDetector::GetEnergy().
   //If energy lost in active (gas) layer is already set (e.g. by calculation of energy loss
   //of charged particles), return its value.
   //If not, we calculate it and set it using the values read from coders (if fired)
   //and the channel-volts/volts-energy calibrations, if present
   //
   //Returns 0 if (i) no calibration present (ii) calibration present but no data in acquisition parameters

   //fELoss already set, return its value
   Double_t ELoss = KVDetector::GetEnergy();
   if( ELoss > 0 ) return KVDetector::GetEnergy();
   ELoss = GetCalibratedEnergy();
   if( ELoss < 0 ) ELoss = 0;
   SetEnergy(ELoss);
   return ELoss;
}

//______________________________________________________________________________

void KVChIo::Streamer(TBuffer &R__b)
{
   // Stream an object of class KVChIo.
   // We set the pointers to the calibrator objects

   if (R__b.IsReading()) {
      KVChIo::Class()->ReadBuffer(R__b, this);
      fVoltE = (KVVoltEnergy *) GetCalibrator("Volt-Energy");
      fChVoltPG  =  (KVChannelVolt *) GetCalibrator("Channel-Volt PG");
      fChVoltGG  =  (KVChannelVolt *) GetCalibrator("Channel-Volt GG");
   } else {
      KVChIo::Class()->WriteBuffer(R__b, this);
   }
}

//______________________________________________________________________________

Short_t KVChIo::GetCalcACQParam(KVACQParam* ACQ, Double_t ECalc) const
{
   // Calculates & returns value of given acquisition parameter corresponding to
   // given calculated energy loss in the detector
   // Returns -1 if detector is not calibrated

   if(!IsCalibrated()) return -1;
   Double_t volts = const_cast<KVChIo*>(this)->GetVoltsFromEnergy( ECalc );
   if(ACQ->IsType("PG")) return (Short_t)const_cast<KVChIo*>(this)->GetCanalPGFromVolts(volts);
   else if(ACQ->IsType("GG")) return (Short_t)const_cast<KVChIo*>(this)->GetCanalGGFromVolts(volts);
   return -1;
}
