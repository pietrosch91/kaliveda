/***************************************************************************
                          KVIDChIoCsI5.cpp  -  description
                             -------------------
    begin                : 17/5/2004
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

#include "KVIDChIoCsI5.h"
#include "KVCsI.h"
#include "KVChIo.h"
#include "KVINDRAReconNuc.h"
#include "Riostream.h"
#include "KVDetector.h"
#include "KVDataSet.h"
#include "KVTGIDZA.h"
#include "TObjString.h"

ClassImp(KVIDChIoCsI5)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//KVIDChIoCsI5
//
//Identification in ChIo-CsI matrices of INDRA 5th campaign
//
//ID SUB-CODES:
//-------------
// 0  KVIDChIoCsI5::k_OK_GG            "ok in GG",
// 1  KVIDChIoCsI5::k_noTGID_GG        "no Z identification available - last grid tried GG",
// 2  KVIDChIoCsI5::k_OutOfIDRange_GG  "point outside of range of possible Z identification in GG",
// 3  KVIDChIoCsI5::k_OK_PG1           "ok in PG1",
// 4  KVIDChIoCsI5::k_noTGID_PG1       "no Z identification available - last grid tried PG1",
// 5  KVIDChIoCsI5::k_OutOfIDRange_PG1 "point outside of range of possible Z identification in PG1"

Double_t KVIDChIoCsI5::IdentifyZ(Double_t & funLTG)
{
   //Identify Z using the following method:
   //      - first try GG matrix
   //      if this doesn't work, or if the Z found is greater than the maximum
   //      Z for GG....
   //      - try PG1 matrix
   //      if this doesn't work, or if the Z found is greater than the maximum
   //      Z for PG1....

   Double_t Z;

   Z = IdentZ(this, funLTG, "GG", "GG");
   fWhichGrid = k_GG;

   if ( GetStatus() == kStatus_OutOfIDRange ) { // GG exists, but point is too high for id

      Z = IdentZ(this, funLTG, "PG1", "PG");
      fWhichGrid = k_PG1;

   } else {
      // successful GG identification, or no GG identification exists
      return Z;
   }

   return Z;
}

//___________________________________________________________________________________________//
Bool_t KVIDChIoCsI5::Identify(KVReconstructedNucleus * part)
{
   //Identification of particles using ChIo-CsI matrices for 5th campaign

   Double_t funLTG_Z = -1;
   Int_t iz = -1;

   Double_t Z = IdentifyZ(funLTG_Z);

   //use KVTGIDManager::GetStatus value for IdentZ as identification subcode
   Int_t Zstatus = (GetStatus() + 3 * fWhichGrid);
   SetIDSubCode(((KVINDRAReconNuc *) part)->GetCodes().GetSubCodes(),
                Zstatus);

   if (GetStatus() != KVTGIDManager::kStatus_OK)
      return kFALSE;            // no ID

   iz = TMath::Nint(Z);
   if (iz < 1)
      iz = 1;

   part->SetZ(iz);
   ((KVINDRAReconNuc *) part)->SetRealZ((Float_t) Z);
   //need to remove isotope resolve flag and real A set by CsI
   ((KVINDRAReconNuc *) part)->SetAMeasured(kFALSE);
   ((KVINDRAReconNuc *) part)->SetRealA(0);
   // set general ID code
   ((KVINDRAReconNuc *) part)->SetIDCode( kIDCode4 );

   return kTRUE;
}

//__________________________________________________________________________//

void KVIDChIoCsI5::Print(Option_t * opt) const
{
   cout << "KVIDChIoCsI5 : " << GetName() << endl;
   if (GetListOfIDFunctions().GetSize() > 0) {
      cout << "  Z identification only" << endl;
      cout << "Identification objects :" << endl;
      GetListOfIDFunctions().Print();
   }
}

//__________________________________________________________________________//

Double_t KVIDChIoCsI5::GetIDMapX(Option_t * opt)
{
   //Calculates current X coordinate for identification.
   //It is the CsI detector's total light output calculated from current values of 'R' and 'L'
   //raw data without pedestal correction (because identification maps were drawn without
   //correcting).
   //'opt' has no effect.
   Double_t rapide =
       (Double_t) (((KVCsI *) GetDetector(2))->GetR() +
                   GetDetector(2)->GetPedestal("R"));
   Double_t lente =
       (Double_t) (((KVCsI *) GetDetector(2))->GetL() +
                   GetDetector(2)->GetPedestal("L"));
   Double_t h =
       (Double_t) ((KVCsI *) GetDetector(2))->GetLumiereTotale(rapide,
                                                               lente);
   return h;
}

//__________________________________________________________________________//

Double_t KVIDChIoCsI5::GetIDMapY(Option_t * opt)
{
   //Calculates current Y coordinate for identification.
   //It is the ionisation chamber's current grand gain (if opt="GG") or petit gain (opt != "GG")
   //coder data, without pedestal correction.
   Double_t si;
   if (!strcmp(opt, "GG")) {
      si = (Double_t) ((KVChIo *) GetDetector(1))->GetGG();
   } else {
      si = (Double_t) ((KVChIo *) GetDetector(1))->GetPG();
   }
   return si;
}

//__________________________________________________________________________//

const Char_t *KVIDChIoCsI5::GetIDSubCodeString(KVIDSubCode & concat) const
{
   //Returns string explaining subcodes for particles identified with this telescope

   static TString messages[] = {
      "ok in GG",
      "no Z identification available - last grid tried GG",
      "point outside of range of possible Z identification in GG",
      "ok in PG1",
      "no Z identification available - last grid tried PG1",
      "point outside of range of possible Z identification in PG1"
   };
   Int_t code = GetIDSubCode(concat);
   if (code == -1)
      return SubCodeNotSet();
   return messages[code];
}

//__________________________________________________________________________//

Bool_t KVIDChIoCsI5::SetIdentificationParameters(const KVMultiDetArray* MDA)
{
   //Initialise the identification parameters (grids, etc.) of ALL identification telescopes of this
   //kind (label) in the multidetector array. Therefore this method need only be called once, and not
   //called for each telescope. The kind/label (returned by GetLabel) of the telescope corresponds
   //to the URI used to find the plugin class in $KVROOT/KVFiles/.kvrootrc.
//The parameters are read from the file with name given by the environment variable
//INDRA_camp5.IdentificationParameterFile.CI-CSI:       [filename]
   
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
   datfile.open( path.Data() );   
   if (!datfile.good()) {
      Error("SetIdentificationParameters",
            "Cannot open file %s",
            filename.Data());
      return kFALSE;
   }

   KVString aline;
   int zmin, zmax;
   char type[5], name[20];
   KVIDChIoCsI5 *idt = 0;

   aline.ReadLine(datfile);
   while (datfile.good()) {

      if (!aline.BeginsWith('#')) {     //skip comments in header

         if (aline.BeginsWith("+")) {

            aline.Remove(0, 2);
            sscanf(aline.Data(), "%s", name);
            idt = (KVIDChIoCsI5 *) MDA->GetIDTelescope(name);

            aline.ReadLine(datfile);
            Bool_t _Aident = kFALSE;
            if (aline.BeginsWith("MASS_ID")) {
               idt->SetHasMassID();
               _Aident = kTRUE;
               aline.Remove(0, 8);
            }
            sscanf(aline.Data(), "%s", type);

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

void KVIDChIoCsI5::RemoveIdentificationParameters()
{
   //Delete any KVTGID objects associated with this telescope
   RemoveAllTGID();
   SetHasMassID(kFALSE);
}

//____________________________________________________________________________________

void KVIDChIoCsI5::Initialize()
{   
   // Initialisation of telescope before identification.
   // This method MUST be called once before any identification is attempted.
   // IsReadyForID() will return kTRUE if KVTGID objects are associated
   // to this telescope for the current run.
   
   if( GetListOfIDFunctions().GetEntries() ){
      SetBit(kReadyForID);
   }
   else
      ResetBit(kReadyForID);
}
