/***************************************************************************
                          KVIDSiCsI5.cpp  -  description
                             -------------------
    begin                : Fri Feb 20 2004
    copyright            : (C) 2004 by J.D. Frankland
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

#include "KVIDSiCsI5.h"
#include "KVCsI.h"
#include "KVChIo.h"
#include "KVSilicon.h"
#include "KVINDRAReconNuc.h"
#include "Riostream.h"
#include "KVDetector.h"
#include "KVMultiDetArray.h"
#include "KVDataSet.h"
#include "KVTGIDZA.h"
#include "TObjString.h"


ClassImp(KVIDSiCsI5)
/////////////////////////////////////////////////////////////////////////////////////
//KVIDSiCsI5
//
//Identification in Si(GG/PG)-CsI(lumiere) matrices of INDRA 5th campaign.
//(Mathieu Pichon & Josquin Marie)
//
//For each telescope, at least 2 grids were used :
//      GG      identification using Si canal GG
//      PG1     identification using Si canal PG
//For certain telescopes, a second PG grid (PG2) is available, used for
//large Z identification.
//Each of the three grids has an associated maximum and minimum Z (GetZmax, GetZmin)
//corresponding to the lines drawn by hand used for the fit.
//Z identification is considered possible between max(Zmin-0.5, 0.5) and min(Zmax+0.5, 100.5)
//(Zmax is taken equal to 100 when using the highest Z grid of the telescope (i.e. either
//PG1 or PG2). 
//In addition certain telescopes have both Z and A identification for the SiGG grid.
//In this case KVIDSiCsI5::HasMassID=kTRUE.
//
//All three grids were fitted using the polynomials tassangot_Z and tassangot_A
//defined in KVTGIDFunctions.
//
//ID SUB-CODES:
//-------------
// 0  KVIDSiCsI5::k_OK_GG            "ok in GG",
// 1  KVIDSiCsI5::k_noTGID_GG        "no Z identification available - last grid tried GG",
// 2  KVIDSiCsI5::k_OutOfIDRange_GG  "point outside of range of possible Z identification in GG",
// 3  KVIDSiCsI5::k_OK_PG1           "ok in PG1",
// 4  KVIDSiCsI5::k_noTGID_PG1       "no Z identification available - last grid tried PG1",
// 5  KVIDSiCsI5::k_OutOfIDRange_PG1 "point outside of range of possible Z identification in PG1",
// 6  KVIDSiCsI5::k_OK_PG2           "ok in PG2",
// 7  KVIDSiCsI5::k_noTGID_PG2       "no Z identification available - last grid tried PG2",
// 8  KVIDSiCsI5::k_OutOfIDRange_PG2 "point outside of range of possible Z identification in PG2",
// 9  KVIDSiCsI5::kZOK_AnoTGID       "Z ok, A identification tried but not available",
//10  KVIDSiCsI5::kZOK_AOutOfIDRange "Z ok, A identification tried but point to identify outside of identification range of function",
//11  KVIDSiCsI5::kZOK_A_ZtooSmall   "Z ok, A identification tried but IdentA called with Z<1",
//12  KVIDSiCsI5::kZOK_A_ZtooLarge   "Z ok, A identification tried but IdentA called with Z larger than max Z defined for KVTGIDZA isotopic identification object"
/////////////////////////////////////////////////////////////////////////////////////
Double_t KVIDSiCsI5::IdentifyZ(Double_t & funLTG)
{
   //Identify Z using the following method:
   //      - first try GG matrix
   //      if this doesn't work, or if the Z found is greater than the maximum
   //      Z for GG....
   //      - try PG1 matrix
   //      if this doesn't work, or if the Z found is greater than the maximum
   //      Z for PG1....
   //      - try PG2 matrix (if it exists)

   Double_t Z = -1.;

   if(((KVSilicon *) GetDetector(1))->GetGG() < 4090){
      //try GG identification if SIGG is not saturated
      //the condition is applied directly to the raw channel number instead of GetIDMapY()
      //so that it is correct even if GetIDMapY rescales for the change in gain
      Z = IdentZ(this, funLTG, "GG", "GG");
      fWhichGrid = k_GG;
   }
      
   if (Z < 0.) {

      Z = IdentZ(this, funLTG, "PG1", "PG");
      fWhichGrid = k_PG1;

   } else {
      //successful GG identification
      return Z;
   }

   if (!HasPG2()) {
      //no 2nd PG fit - this is the end
      return Z;
   }

   if (Z < 0.) {

      Z = IdentZ(this, funLTG, "PG2", "PG");
      fWhichGrid = k_PG2;

   }

   return Z;
}

//______________________________________________________________________________
Bool_t KVIDSiCsI5::Identify(KVReconstructedNucleus * part)
{
   //Identification of particles using Si(300)-CsI matrices for 5th campaign.
   //First of all, Z identification is attempted with KVIDSiCsI5::IdentZ.
   //If successful, if this telescope has mass identification capabilities
   //(HasMassID()=kTRUE), then if the identified Z is not too large for the
   //GG grid Z&A identification, we identify the mass of the particle.
   //In this case, a further check is made that the resulting nucleus is not
   //too bizarre, i.e. it must be in the KVNucleus mass table (KVNucleus::IsKnown = kTRUE).
   //If not, or if mass identification is not possible, the mass of the nucleus
   //is deduced from Z using the default mass formula of class KVNucleus.

   Double_t funLTG_Z = -1;
   Double_t funLTG_A = -1;
   Double_t mass = -1;
   Int_t ia = -1;
   Int_t iz = -1;

   Double_t Z = IdentifyZ(funLTG_Z);
	KVINDRAReconNuc* irnuc = (KVINDRAReconNuc*)part;
	
	irnuc->SetAMeasured(kFALSE);
	irnuc->SetZMeasured(kFALSE);

   //use KVTGIDManager::GetStatus value for IdentZ as identification subcode
   Int_t Zstatus = (GetStatus() + 3 * fWhichGrid);
   SetIDSubCode(irnuc->GetCodes().GetSubCodes(),Zstatus);

   if (GetStatus() != KVTGIDManager::kStatus_OK)
      return kFALSE;            // no ID

   iz = TMath::Nint(Z);

   //is mass identification a possibility ?
   if (HasMassID() && Zstatus == k_OK_GG) {     //only in GG...  

      mass = IdentA(this, funLTG_A, "GG", "GG", iz);

      if (GetStatus() != KVTGIDManager::kStatus_OK)     //mass ID not good ?
      {
         //only Z identified
         part->SetZ(iz);
         irnuc->SetRealZ((Float_t) Z);
         //subcode says "Z ok but A failed because..."
         SetIDSubCode(irnuc->GetCodes().GetSubCodes(),
                      (k_OutOfIDRange_PG2 + GetStatus()));
			irnuc->SetZMeasured(kTRUE);
         irnuc->SetRealA(0);
      }
		else                    //both Z and A successful ?
      {
         ia = TMath::Nint(mass);
         part->SetZ(iz);
         part->SetA(ia);
         //full A and Z identification
         irnuc->SetRealZ((Float_t) Z);
         irnuc->SetRealA((Float_t) mass);
         irnuc->SetAMeasured(kTRUE);
			irnuc->SetZMeasured(kTRUE);
      }
   } else {
      //only Z identified
      //ID subcode remains 'Zstatus'
      part->SetZ(iz);
      irnuc->SetRealZ((Float_t) Z);
		irnuc->SetZMeasured(kTRUE);
      irnuc->SetRealA(0);
   }

   // set general ID code
   irnuc->SetIDCode( kIDCode3 );
   return kTRUE;
}

//__________________________________________________________________________//

void KVIDSiCsI5::Print(Option_t * opt) const
{
   cout << "KVIDSiCsI5 : " << GetName() << endl;
   if (GetListOfIDFunctions().GetSize() > 0) {
      if (HasMassID()) {
         cout << "  A and Z identification" << endl;
      } else {
         cout << "  Z identification only" << endl;
      }
      cout << "Identification objects :" << endl;
      GetListOfIDFunctions().Print();
   }
}

//__________________________________________________________________________//

Double_t KVIDSiCsI5::GetIDMapX(Option_t * opt)
{
   //Calculates current X coordinate for identification.
   //It is the CsI detector's total light output calculated from current values of 'R' and 'L'
   //raw data without pedestal correction (because identification maps were drawn without
   //correcting).
   //'opt' has no effect.
	
   Double_t rapide = (Double_t)fCsI->GetR() + fCsIRPedestal;
   Double_t lente = (Double_t)fCsI->GetL() + fCsILPedestal;
   Double_t h = (Double_t)fCsI->GetLumiereTotale(rapide,lente);
   return h;
}

//____________________________________________________________________________________

void KVIDSiCsI5::Initialize()
{   
   // Initialisation of telescope before identification.
   // This method MUST be called once before any identification is attempted.
   // IsReadyForID() will return kTRUE if KVTGID objects are associated
   // to this telescope for the current run.
   
	fSi = (KVSilicon*)GetDetector(1);
	fSiPGPedestal = fSi->GetPedestal("PG");
	fSiGGPedestal = fSi->GetPedestal("GG");
	fSiGain = fSi->GetGain();
	fCsI = (KVCsI*)GetDetector(2);
	fCsIRPedestal = fCsI->GetPedestal("R");
	fCsILPedestal = fCsI->GetPedestal("L");
   if( GetListOfIDFunctions().GetEntries() ){
      SetBit(kReadyForID);
   }
   else
      ResetBit(kReadyForID);
}

//__________________________________________________________________________//

Double_t KVIDSiCsI5::GetIDMapY(Option_t * opt)
{
   //Calculates current Y coordinate for identification.
   //It is the silicon detector's current grand gain (if opt="GG") or petit gain (opt != "GG")
   //coder data, without pedestal correction.
   //We include a "correction" for the gain of the Silicon amplifier:
   //this was set to 1.0 during the runs for which the identification grids were drawn;
   //when it increases to 1.41 we simply scale the data down by the same factor.
	
   Double_t si, si_ped;
   if (!strcmp(opt, "GG")) {
      si = (Double_t)fSi->GetGG();
		si_ped = fSiGGPedestal;
   } else {
      si = (Double_t)fSi->GetPG();
		si_ped = fSiPGPedestal;
   }
   //gain "correction"
   if (fSiGain > 1.1) si = (si - si_ped) / fSiGain + si_ped;
   return si;
}


//____________________________________________________________________________________

const Char_t *KVIDSiCsI5::GetIDSubCodeString(KVIDSubCode & concat) const
{
   //Returns string explaining subcodes for this telescope

   static TString messages[] = {
      "ok in GG",
      "no Z identification available - last grid tried GG",
      "point outside of range of possible Z identification in GG",
      "ok in PG1",
      "no Z identification available - last grid tried PG1",
      "point outside of range of possible Z identification in PG1",
      "ok in PG2",
      "no Z identification available - last grid tried PG2",
      "point outside of range of possible Z identification in PG2",
      "Z ok, A identification tried but not available",
      "Z ok, A identification tried but point to identify outside of identification range of function",
      "Z ok, A identification tried but IdentA called with Z<1",
      "Z ok, A identification tried but IdentA called with Z larger than max Z defined for KVTGIDZA isotopic identification object"
   };
   Int_t code = GetIDSubCode(concat);
   if (code == -1)
      return SubCodeNotSet();
   return messages[code];
}

//____________________________________________________________________________________

Bool_t KVIDSiCsI5::SetIdentificationParameters(const KVMultiDetArray* MDA)
{
   //Initialise the identification parameters (grids, etc.) of ALL identification telescopes of this
   //kind (label) in the multidetector array. Therefore this method need only be called once, and not
   //called for each telescope. The kind/label (returned by GetLabel) of the telescope corresponds
   //to the URI used to find the plugin class in $KVROOT/KVFiles/.kvrootrc.
//Parameters are read from the file with name given by the environment variable
//INDRA_camp5.IdentificationParameterFile.SI-CSI:       [filename]
   
   TString filename = gDataSet->GetDataSetEnv( Form("IdentificationParameterFile.%s",GetLabel()) );
   if( filename == "" ){
      Warning("SetIdentificationParameters",
            "No filename defined. Should be given by %s.IdentificationParameterFile.%s",
            gDataSet->GetName(), GetLabel());
      return kFALSE;
   }
   TString path;
   if (!SearchKVFile(filename.Data(), path, gDataSet->GetName())){
      Error("SetIdentificationParameters",
            "File %s not found. Should be in $KVROOT/KVFiles/%s",
            filename.Data(), gDataSet->GetName());
      return kFALSE;
   }
   Info("SetIdentificationParameters", "Using file %s", path.Data());
   ifstream datfile;
   datfile.open(path.Data());
   KVString aline;
   int zmin, zmax;
   char type[5], name[20];
   KVIDSiCsI5 *idt = 0;

   aline.ReadLine(datfile);
   while (datfile.good()) {

      if (!aline.BeginsWith('#')) {     //skip comments in header

         if (aline.BeginsWith("+")) {

            aline.Remove(0, 2);
            sscanf(aline.Data(), "%s", name);
            idt = (KVIDSiCsI5 *) MDA->GetIDTelescope(name);

            aline.ReadLine(datfile);
            Bool_t _Aident = kFALSE;
            if (aline.BeginsWith("MASS_ID")) {
               idt->SetHasMassID();
               _Aident = kTRUE;
               aline.Remove(0, 8);
            }
            sscanf(aline.Data(), "%s", type);

            if (!strcmp(type, "PG2")){
               idt->SetHasPG2();
				}

            aline.ReadLine(datfile);
            sscanf(aline.Data(), "ZMIN=%d  ZMAX=%d", &zmin, &zmax);

            //create new Tassan-Got ID object for telescope
            KVTGID *_tgid = 0, *_tgid2 = 0;
            if (_Aident) {
               //When mass ID is possible, have to create two objects: one
               //for Z ident and the other for A ident
               //A-identification functional. Set name of parameter "Z"
               _tgid =
                   new KVTGIDZA(idt->
                                GetTGIDName(idt->GetName(), "A", type),
                                "tassangot_A", 0.1, 100., 13, 11, 12);
               _tgid->SetParName(10, "Z");
               _tgid2 =
                   new KVTGIDZ(idt->GetTGIDName(idt->GetName(), "Z", type),
                               "pichon_Z", 0.1, 100., 12, 10, 11);
            } else {
               //Z-identification functional.
               _tgid =
                   new KVTGIDZ(idt->GetTGIDName(idt->GetName(), "Z", type),
                               "pichon_Z", 0.1, 100., 12, 10, 11);
            }

            _tgid->SetIDmin((Double_t) zmin);
            _tgid->SetIDmax((Double_t) zmax);
            if (_Aident) {
               _tgid2->SetIDmin((Double_t) zmin);
               _tgid2->SetIDmax((Double_t) zmax);
            }
            //read line with parameters on it from file
            aline.ReadLine(datfile);
            TObjArray *par_arr = aline.Tokenize(" ");   //split up into 1 string per parameter
            for (int i = 0; i < par_arr->GetEntries(); i++) {
               //read Double_t value in string
               KVString kvs(((TObjString *) (*par_arr)[i])->String());
               Double_t param = kvs.Atof();
               //set parameter of ID functional
               _tgid->SetParameter(i, param);
               if (_Aident)
                  _tgid2->SetParameter(i, param);
            }
            delete par_arr;
            //add identification object to telescope's ID manager
            idt->AddTGID(_tgid);
            if (_Aident)
               idt->AddTGID(_tgid2);
         }
      }
      aline.ReadLine(datfile);
   }                            //while( datfile.good()
   datfile.close();
   return kTRUE;
}

//____________________________________________________________________________________

void KVIDSiCsI5::RemoveIdentificationParameters()
{
   //Delete any KVTGID objects associated with this telescope
   RemoveAllTGID();
   SetHasMassID(kFALSE);
   SetHasPG2(kFALSE);
}
