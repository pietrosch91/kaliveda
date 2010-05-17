/***************************************************************************
$Id: KVSilicon.cpp,v 1.55 2009/04/15 09:49:19 ebonnet Exp $
                          kvsilicon.cpp  -  description
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
#include "KVSilicon.h"
#include "KVChIo.h"
#include "KVTelescope.h"
#include "KVGroup.h"
#include "KVINDRA.h"
//for GetRunCalibrationPeak
#include "KVINDRADB.h"
#include "KVDBParameterSet.h"
#include "KVDBAlphaPeak.h"
#include "KVDBElasticPeak.h"
#include "KVDBRun.h"
#include "KVNucleus.h"
#include "TROOT.h"

#define MAX_CANAL_GG 4000

ClassImp(KVSilicon)

//////////////////////////////////////////////////////////////////
//KVSilicon
//
//Used to describe Silicon detectors of the INDRA array.
//In order to create a detector, use the KVSilicon::KVSilicon(Float_t thick)
//constructor with "thick" the thickness in microns.
//
//Type of detector : "SI"

void KVSilicon::init()
{
   //initialise non-persistent pointers
   fChIo=0;
   fChVoltGG=0;
   fChVoltPG=0;
   fVoltE=0;
   fPHD=0;
   fZminPHD=10;
   fSegment = 1;
   fPGtoGG_0 = 0;
   fPGtoGG_1 = 15;
}

//______________________________________________________

KVSilicon::KVSilicon()
{
   //Default ctor
   //This ctor (which in turn calls the KVDetector default ctor) must exist in
   //order for Cloning of detectors to work (as used in KVTelescope::AddDetector).
   //Do not replace this ctor by giving default arguments to KVSilicon(Float_t).
   //
   init();
}

//______________________________________________________________________________
KVSilicon::KVSilicon(Float_t thick):KVINDRADetector("Si", thick)
{
   // constructor for silicon detector
	//Type of detector: "SI"
   SetType("SI");
   init();
}

//____________________________________________________________________________________________

KVSilicon::~KVSilicon()
{
}

//____________________________________________________________________________________________
Int_t KVSilicon::GetCanalPGFromVolts(Float_t volts)
{
   //Return raw PG channel number corresponding to a given detector signal in volts
   //
   //Any change in the coder pedestal between the current run and the effective pedestal
   //of the channel-volt calibration (GetCanal(V=0)) is automatically corrected for.
   //
   //Returns -1 if PG <-> Volts calibration is not available


      if (!fChVoltPG || !fChVoltPG->GetStatus())
         return -1;
      Int_t chan = (Int_t) (fChVoltPG->Invert(volts));
      //correct for pedestal drift
      chan = chan + (Int_t) (GetPedestal("PG") - fChVoltPG->Invert(0));
      return chan;

}

//____________________________________________________________________________________________
Int_t KVSilicon::GetCanalGGFromVolts(Float_t volts)
{
   //Return raw GG channel number corresponding to a given detector signal in volts
   //
   //Any change in the coder pedestal between the current run and the effective pedestal
   //of the channel-volt calibration (GetCanal(V=0)) is automatically corrected for.
   //
   //Returns -1 if PG <-> Volts calibration is not available

      if (!fChVoltGG || !fChVoltGG->GetStatus())
         return -1;
      Int_t chan = (Int_t) (fChVoltGG->Invert(volts));
      //correct for pedestal drift
      chan = chan + (Int_t) (GetPedestal("GG") - fChVoltGG->Invert(0));
      return chan;
}

//____________________________________________________________________________________________

KVChIo *KVSilicon::GetChIo() const
{
   //Return pointer to ChIo corresponding to the detector
   //i.e. the ChIo placed in front of the silicon
   return (fChIo ? fChIo : const_cast<KVSilicon*>(this)->FindChIo());
}

//____________________________________________________________________________________________

KVChIo *KVSilicon::FindChIo()
{
   //PRIVATE METHOD
   //Used when GetChIo is called the first time to retrieve the
   //pointer to the ChIo associated to this silicon
   if (GetTelescope()) {
      KVGroup *kvgr = GetTelescope()->GetGroup();
      if (kvgr) {
         KVList *dets = kvgr->GetDetectors();
         TIter next_det(dets);
         KVDetector *dd;
         while ((dd = (KVDetector *) next_det())) {
            if (dd->InheritsFrom("KVChIo"))
               fChIo = (KVChIo *) dd;
         }
      }
   } else
      fChIo=0;
   return fChIo;
}

//____________________________________________________________________________________________

void KVSilicon::SetACQParams()
{
   //Setup acquistion parameters for this Silicon.
   //Do not call before ChIo name has been set.

   AddACQParamType("GG");
   AddACQParamType("PG");
   AddACQParamType("T");

}

//____________________________________________________________________________________________

void KVSilicon::SetCalibrators()
{
   //Set up calibrators for this detector. Call once name has been set.
   KVCalibrator* c=new KVChannelVolt("GG", this);
   if(!AddCalibrator(c)) delete c;
   c=new KVChannelVolt("PG", this);
   if(!AddCalibrator(c)) delete c;
   c=new KVVoltEnergy(this);
   if(!AddCalibrator(c)) delete c;
   c=new KVPulseHeightDefect(this);
   if(!AddCalibrator(c)) delete c;
   fVoltE = (KVVoltEnergy *) GetCalibrator("Volt-Energy");
   fChVoltPG  =  (KVChannelVolt *) GetCalibrator("Channel-Volt PG");
   fChVoltGG  =  (KVChannelVolt *) GetCalibrator("Channel-Volt GG");
   fPHD  =  (KVPulseHeightDefect *) GetCalibrator("Pulse Height Defect");
   fZminPHD = (Int_t)gDataSet->GetDataSetEnv("KVSilicon.ZminForPHDCorrection", 10.);
}


//__________________________________________________________________________________________

Float_t KVSilicon::GetGGfromPG(Float_t PG)
{
   //Calculate GG from PG when GG is saturated (>4095).
   //If PG is not given as argument, the current value of the detector's PG ACQParam is read
   //The GG value returned includes the current pedestal:
   //      GG = pied_GG + alpha + beta * (PG - pied_PG)
   //alpha, beta coefficients are obtained by fitting (GG-pied) vs. (PG-pied) for data.
   if (PG < 0)
      PG = (Float_t) GetPG();
   Float_t GG =
       GetPedestal("GG") + fPGtoGG_0 + fPGtoGG_1 * (PG -
                                                    GetPedestal("PG"));
   return GG;
}

//__________________________________________________________________________________________

Float_t KVSilicon::GetPGfromGG(Float_t GG)
{
   //Calculate PG from GG
   //If GG is not given as argument, the current value of the detector's GG ACQParam is read
   //The PG value returned includes the current pedestal:
   //      PG = (1 / beta)*( GG - pied_GG - alpha ) + pied_PG
   //alpha, beta coefficients were obtained by fitting (GG-pied) vs. (PG-pied) for data.
   if (GG < 0)
      GG = (Float_t) GetGG();
   Float_t PG = (1./fPGtoGG_1)*(GG - GetPedestal("GG") - fPGtoGG_0) + GetPedestal("PG");
   return PG;
}

//__________________________________________________________________________________________

Double_t KVSilicon::GetPHD(Double_t Einc, UInt_t Z)
{
   //Calculate Pulse Height Defect in MeV for a given incident energy Einc(MeV) and Z.
   //The formula of Moulton is used (see class KVPulseHeightDefect).
   //Returns 0 if Z <= ZminForPHDCorrection,
   //which is defined by the following variables in .kvrootrc :
   //
   // (for a given dataset)   [dataset].KVSilicon.ZminForPHDCorrection:
   // (for any dataset)         KVSilicon.ZminForPHDCorrection:
   //
   //Returns 0 if PHD is not defined.

   if(!fPHD || !fPHD->GetStatus()) return 0;
   fPHD->SetZ(Z);
   return fPHD->Compute(Einc);
}

//____________________________________________________________________________________________

Double_t KVSilicon::GetCorrectedEnergy(UInt_t z, UInt_t a, Double_t e, Bool_t trn)
{
   //Returns total energy lost by particle in silicon detector corrected for PHD.
   //
   //If "e" (measured/apparent energy loss in detector) not given, current value
   //measured in detector is used. If PHD for detector has not been set, no correction
   //is performed
   //See GetPHD() and class KVPulseHeightDefect.
   //
   //transmission=kTRUE (default): particle does not stop in the detector
   //transmission=kFALSE:                 particle stops in the detector

   if (e < 0.) e = GetEnergy();
   if( e <= 0 ) return 0;
   // calculate incident energy from measured energy loss in detector
   enum KVMaterial::SolType solution = KVMaterial::kEmax;
   if(!trn) solution = KVMaterial::kEmin;
   Double_t EINC = GetIncidentEnergy(z, a, e, solution);
   //incident energy - residual energy = total real energy loss
   return (EINC - GetERes(z, a, EINC));
}

//__________________________________________________________________________________________
Double_t KVSilicon::GetCalibratedEnergy()
{
   //Calculate energy in MeV from coder values.
   //Returns 0 if calibration not ready or detector not fired
   //(we require that at least one acquisition parameter have a value
   //greater than the current pedestal value)

   if (IsCalibrated() && Fired("Pany"))
      return (fVoltE->Compute( GetVolts() ));
   return 0;
}

//____________________________________________________________________________________________

Double_t KVSilicon::GetVoltsFromCanalPG(Double_t chan)
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
Double_t KVSilicon::GetVoltsFromCanalGG(Double_t chan)
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

//____________________________________________________________________________________________

Double_t KVSilicon::GetVolts()
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

//_______________________________________________________________________________

Double_t KVSilicon::GetVoltsFromEnergy(Double_t e)
{
   //Inverts calibration, i.e. calculates volts for a given energy loss (in MeV)

   if (fVoltE->GetStatus()){
      return (fVoltE->Invert( e ));
   }
   return 0;
}

//____________________________________________________________________________________________

Double_t KVSilicon::GetEnergy()
{
   //Redefinition of KVDetector::GetEnergy().
   //If energy lost in active layer is already set (e.g. by calculation of energy loss
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

void KVSilicon::Streamer(TBuffer &R__b)
{
   // Stream an object of class KVSilicon.
   // We set the pointers to the calibrator objects

   if (R__b.IsReading()) {
      KVSilicon::Class()->ReadBuffer(R__b, this);
      fVoltE = (KVVoltEnergy *) GetCalibrator("Volt-Energy");
      fChVoltPG  =  (KVChannelVolt *) GetCalibrator("Channel-Volt PG");
      fChVoltGG  =  (KVChannelVolt *) GetCalibrator("Channel-Volt GG");
      fPHD  =  (KVPulseHeightDefect *) GetCalibrator("Pulse Height Defect");
   } else {
      KVSilicon::Class()->WriteBuffer(R__b, this);
   }
}

//______________________________________________________________________________

void KVSilicon::SetMoultonPHDParameters(Double_t a_1, Double_t a_2, Double_t b_1, Double_t b_2)
{
   //Sets parameters of Moulton formula used to calculate PHD for particles
   //stopping in this detector. The parameters are as in the following:
   //
   // log_10(PHD) = b(Z) + a(Z)*log_10(E)
   //
   //  with  a(Z) = a_1*(Z**2/1000) + a_2
   //          b(Z) = b_1*(100/Z) + b_2
   //            E = incident energy of particle
   //
   //See class KVPulseHeightDefect

   if(fPHD){
      fPHD->SetParameters(a_1, a_2, b_1, b_2, (Double_t)fZminPHD);
      fPHD->SetStatus(kTRUE);
   }
}

//______________________________________________________________________________

void KVSilicon::SetELossParams(Int_t Z, Int_t A)
{
   //Initialise energy loss coefficients for this detector and a given incident nucleus (Z,A)
   //We redefine the KVDetector::SetELossParams method in order to include the
   //pulse height defect (if defined) for the silicon detector in the calculation of
   //the energy loss in the active (silicon) layer.

   //PHD not defined ? ignore
   if(!fPHD || !fPHD->GetStatus()){
      KVDetector::SetELossParams(Z,A);
      return;
   }

   //do we need to set up the ELoss function ?
   //only if it has never been done for PHD before
   Int_t npar_siphd = 19+6;      //number of params for eloss function = 19 params for energy loss + 6 for PHD (Z,a_1,a_2,b_1,b_2,Zmin)

   if( npar_loss !=  npar_siphd ){
      npar_loss = npar_siphd;
      if( par_loss ) delete [] par_loss; //delete previous parameter array
      par_loss = new Double_t[npar_loss];
      //find/create function
      //search in list of functions for one corresponding to this detector
      //the name of the required function is ELoss_SiPHD
      ELoss =
          (TF1 *) gROOT->GetListOfFunctions()->FindObject("ELoss_SiPHD");
      if (!ELoss)
         ELoss = new TF1("ELoss_SiPHD", ELossSiPHD, 0.1, 5000., npar_loss);
   }

   //fill parameter array
  ((KVMaterial*)fAbsorbers->At(0))->GetELossParams(Z, A, par_loss);
  par_loss[19] = Z;
  for (register int i = 0; i < 5; i++) par_loss[i+20] = fPHD->GetParameter(i);

   //set parameters of energy loss function
   ELoss->SetParameters(par_loss);
}

//_____________________________________________________________________________________//

Double_t ELossSiPHD(Double_t * x, Double_t * par)
{
   //Calculates measured/apparent energy loss in silicon detector, taking into account PHD
   //Parameters par[0] to par [18] are energy loss parameters for Silicon
   //Parameter par[19] is Z of nucleus
   //Parameters par[20] to par[24] are Moulton PHD parameters (see KVPulseHeightDefect
   //source file for function & parameter definitions).
   //Argument x[0] is incident energy in MeV

   //measured/apparent dE = real dE - PHD
   return (ELossSaclay(x, par) - PHDMoulton(x, &par[19]));
}


//______________________________________________________________________________

Short_t KVSilicon::GetCalcACQParam(KVACQParam* ACQ) const
{
   // Calculates & returns value of given acquisition parameter corresponding to the
   // current value of fEcalc, i.e. the calculated residual energy loss in the detector
   // after subtraction of calculated energy losses corresponding to each identified
   // particle crossing this detector.
   // Returns -1 if fEcalc = 0 or if detector is not calibrated

   if(!IsCalibrated() || GetECalc()==0) return -1;
   Double_t volts = const_cast<KVSilicon*>(this)->GetVoltsFromEnergy( GetECalc() );
   if(ACQ->IsType("PG")) return (Short_t)const_cast<KVSilicon*>(this)->GetCanalPGFromVolts(volts);
   else if(ACQ->IsType("GG")) return (Short_t)const_cast<KVSilicon*>(this)->GetCanalGGFromVolts(volts);
   return -1;
}

//__________________________________________________________________________________________

ClassImp(KVSi75)
//_________________________________________
//Etalon telescope detector Si75
//(actually an 80 micron thick silicon detector)
//Type of detector: "SI75"
//Array naming convention: "SI75_RR" with RR=ring number

KVSi75::KVSi75():KVSilicon()
{
   //Default ctor
   //For cloning
}

KVSi75::KVSi75(Float_t thick):KVSilicon(thick)
{
   //Default ctor
   //80 micron silicon detector with type "SI75"
   SetType("SI75");
   SetLabel("SI75");
}

KVSi75::~KVSi75()
{
   //default dtor
}

const Char_t *KVSi75::GetArrayName()
{
   //Redefinition of KVDetector method.
   //Name given as SI75_xx with xx=Ring number

   fFName = Form("SI75_%02d", GetRingNumber());
   return fFName.Data();
}

//__________________________________________________________________________________________

ClassImp(KVSiLi)
//_________________________________________
//Etalon telescope detector Si(Li)
//Nominal thickness: 2mm active layer + 40um dead layer (both silicon)
//Type of detector: "SILI"
//Array naming convention: "SILI_RR" with RR=ring number

KVSiLi::KVSiLi():KVSilicon()
{
   //Default ctor
   //For cloning
}

KVSiLi::KVSiLi(Float_t thick):KVSilicon(thick)
{
   //Default ctor
   // first layer (active) : 2mm silicon (nominal)
   // second layer (dead) : 40um silicon (nominal)
   AddAbsorber( new KVMaterial("Si", 40.0) );
   SetType("SILI");
   SetLabel("SILI");
}

KVSiLi::~KVSiLi()
{
   //default dtor
}

const Char_t *KVSiLi::GetArrayName()
{
   //Redefinition of KVDetector method.
   //Name given as SILI_xx with xx=Ring number

   fFName = Form("SILI_%02d", GetRingNumber());
   return fFName.Data();
}
