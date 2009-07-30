/***************************************************************************
                          KVIDSi150CsI.cpp  -  description
                             -------------------
    begin                : Fri Jul 18 2003
    copyright            : (C) 2003 by John Frankland
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

#include "KVIDSi150CsI.h"
#include "KVNucleus.h"
#include "KVCsI.h"
#include "KVSilicon.h"
#include "KVINDRAReconNuc.h"
#include "KVINDRACodes.h"
#include "Riostream.h"
#include "KVIDZAGrid.h"
#include "KVMultiDetArray.h"
#include "KVDataSet.h"
#include "KVTGIDZA.h"

ClassImp(KVIDSi150CsI)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KVIDTelescope specific to Si(150u)-CsI telescopes of 5th campaign.
//Implements Z & A identification using P. Pawlowski's calibrations.
//
// Identification subcodes:
//
//       0   KVIDSi150CsI::kOK_GG                      "Z&A identification OK in SIGG-CSI"
//       1   KVIDSi150CsI::kOK_PG                      "Z&A identification OK in SIPG-CSI"
//       2   KVIDSi150CsI::kZOK             "Z identification OK, Z is > than max Z for which we have isotopic identification"
//       3   KVIDSi150CsI::kZOK_AOutOfIDRange      "Z id OK, mass ID attempted but point to identify outside of identification range of KVTGIDZA"
//       4   KVIDSi150CsI::kZOK_A_ZtooSmall         "Z id ok, mass ID attempted but Z passed to IdentA too small (<1)"
//       5   KVIDSi150CsI::kZOK_A_ZtooLarge         "Z id ok, mass ID attempted but Z passed to IdentA larger than max Z defined by KVTGIDZA  "    
//       6   KVIDSi150CsI::kZOK_badA    "Z identification OK. A not in KVNucleus mass table for this Z, ignored (A calculated from Z)"
//       7   KVIDSi150CsI::kZOK_bigZ    "Z identification OK. Z found greater than last complete Z-line in the SIPG-CSI matrix"
//       8   KVIDSi150CsI::kZOK_badFit    "Z identification OK. Mass-fit is bad for measured CsI total light (A calculated from Z)"
//       9   KVIDSi150CsI::kZmin_coder             "Module with coder saturation. SiPG>max canal for which ID possible. Z given is a minimum"
//     10   KVIDSi150CsI::kNoID             "No Z identification. Bad news"

Double_t KVIDSi150CsI::IdentifyZ(Double_t & funLTG)
{
   //Identify Z using the following method:
   //      - first try PG matrix
   //      if this doesn't work
   //      - try GG matrix

   Double_t Z = -1.;

   fWhichGrid = k_PG;
   
   Z = IdentZ(this, funLTG, "PG", "PG");

   if (Z < 0.) {

      fWhichGrid = k_GG;
   
      Z = IdentZ(this, funLTG, "GG", "GG");

   }
   
   return Z;
}

//______________________________________________________________________________________________

Double_t KVIDSi150CsI::IdentifyA(Int_t z, Double_t & funLTG)
{
   //Identify A using functional defined for given Z
   //If Z identified in GG (fWhichGrid = k_GG), use GG functional
   //If Z identified in PG (fWhichGrid = k_PG), use PG functional

   Double_t A = -1.;

   if ( fWhichGrid == k_GG )
   { A = IdentA(this, funLTG, "GG", "GG", z);}
   else
   {  A = IdentA(this, funLTG, "PG", "PG", z);}
   
   return A;
}

//______________________________________________________________________________________________

void KVIDSi150CsI::Print(Option_t * opt) const
{
   cout << "  KVIDSi150CsI : " << GetName() << endl << endl;
   if (GetListOfIDFunctions().GetSize() > 0) {
      if (HasMassID()) {
         cout << "  A and Z identification" << endl;
      } else {
         cout << "  Z identification only" << endl;
      }
      cout << "Identification objects :" << endl;
      GetListOfIDFunctions().Print();
      cout << "  Mass correction parameters: " << endl;
      for (int z = 0; z < ZMAX_IDT + 1; z++) {
         cout << " Z= " << z << endl;
         for (int i = 0; i < 2; i++) {
            cout << "    i=" << i << "  par[" << i << "] = " << fCorr[z][i]
                << endl;
         }
      }
   }
}

//_________________________________________________________________________________________________//

Bool_t KVIDSi150CsI::Identify(KVReconstructedNucleus * part)
{
   //Identify nucleus if possible
   //First Z identification is performed using either GG or PG (see IdentifyZ).
   //A identification is performed if Z<=8
   //if the resulting nucleus is not in the KVNucleus mass table,
   //we use the default (calculated) A for the identified Z and the particle is not
   //labelled as being isotopically resolved.

   //Z-identification
   Double_t funLTG_Z = -1;
   Double_t mass = -1;
   Double_t funLTG_A = -1;
   Int_t ia = -1;
   Int_t iz = -1;
   KVINDRAReconNuc* INDpart = (KVINDRAReconNuc *) part;
	part->SetZMeasured(kFALSE);
	part->SetAMeasured(kFALSE);

   Double_t Z = IdentifyZ(funLTG_Z);

   if (GetStatus() != KVTGIDManager::kStatus_OK){
      //no Z identification possible ?
      TString lastfit("GG");
      if(fWhichGrid==k_PG) lastfit="PG";
//       Error("Identify", "No Z identification possible: (last fit tried %s) SIGG = %f SIPG = %f  CSI-lumiere = %f",
//             lastfit.Data(), GetIDMapY("GG"), GetIDMapY("PG"), GetIDMapX());
      SetIDSubCode(INDpart->GetCodes().GetSubCodes(), kNoID);
      return kFALSE; 
   }
   
   SetIDSubCode(INDpart->GetCodes().GetSubCodes(), fWhichGrid);//=0 (GG) or 1 (PG)
	part->SetZMeasured(kTRUE);

   iz = TMath::Nint(Z);

   //mass identification - only for 1 <= Z <= ZMAX_IDT
   if (iz <= ZMAX_IDT) {

      mass = IdentifyA(iz, funLTG_A);

      if (GetStatus() == KVTGIDManager::kStatus_OK) {   //mass ID good

         //further mass correction by P. Pawlowski
         mass = fCorr[iz][0] * mass + fCorr[iz][1];
         ia = TMath::Nint(mass);

         // check mass fit is good for current Z & CsI total light output
         if( GetIDMapX() < GetMinLumiere(iz) ){
            // mass ID not good, Z ok
            SetIDSubCode(INDpart->GetCodes().GetSubCodes(), kZOK_badFit );
            //only Z identified
            part->SetZ(iz);
            INDpart->SetRealZ((Float_t) Z);
            INDpart->SetRealA(0);
         }
         //check isotope in mass table
         else if( !part->IsKnown(iz,ia) ){ 
            // mass ID not good, Z ok
            SetIDSubCode(INDpart->GetCodes().GetSubCodes(),kZOK_badA );
            //only Z identified
            part->SetZ(iz);
            INDpart->SetRealZ((Float_t) Z);
            INDpart->SetRealA(0);
         }
         else
         {
            //full A and Z identification
            part->SetZ(iz);
            part->SetA(ia);
            INDpart->SetRealZ((Float_t) Z);
            INDpart->SetRealA((Float_t) mass);
				part->SetAMeasured(kTRUE);
         }
      } else {                  // A identification failed
                                    //only Z identified
         part->SetZ(iz);
         INDpart->SetRealZ((Float_t) Z);
         //subcode says "Z ok but A failed because..."
         SetIDSubCode(INDpart->GetCodes().GetSubCodes(), kZOK + GetStatus() - kStatus_noTGID);
         INDpart->SetRealA(0);
      }
   } else {
      //only Z identified
      part->SetZ(iz);
      INDpart->SetRealZ((Float_t) Z);
      INDpart->SetRealA(0);
      //subcode says "Z ok, but larger than max Z for which we have isotopic identification"
      SetIDSubCode(INDpart->GetCodes().GetSubCodes(), kZOK);
   }
   
   // set general ID code
   INDpart->SetIDCode( kIDCode3 );
   
   // set warning if Z > last complete line in PG
   if( fWhichGrid == k_PG && iz>GetTGID(GetName(),"Z","PG")->GetIDmax())
      SetIDSubCode(INDpart->GetCodes().GetSubCodes(), kZOK_bigZ);
   
   // check  SIPG is not in coder saturation zone
   if( GetIDMapY("PG") > fMaxSiPG ){
      // Z is a minimum value. No isotopic identification.
      part->SetZ(iz);
      INDpart->SetRealA(0);
      SetIDSubCode(INDpart->GetCodes().GetSubCodes(), kZmin_coder);
		part->SetAMeasured(kFALSE);
      // set general ID code
      INDpart->SetIDCode( kIDCode5 );
   }
   
   return kTRUE;
}

//__________________________________________________________________________//

Double_t KVIDSi150CsI::GetIDMapX(Option_t * opt)
{
   //Calculates current X coordinate for identification.
   //It is the CsI detector's total light output calculated from current values of 'R' and 'L'
   //'opt' has no effect.
   Double_t h = (Double_t) ((KVCsI *) GetDetector(2))->GetLumiereTotale();
   return h;
}

//__________________________________________________________________________//

Double_t KVIDSi150CsI::GetIDMapY(Option_t * opt)
{
   //Calculates current Y coordinate for identification.
   //It is the silicon detector's current grand gain (if opt="GG") or petit gain (opt = "PG")
   //coder data, without pedestal correction.
   Double_t si = (Double_t) GetDetector(1)->GetACQParam(opt)->GetCoderData();
   return si;
}

//___________________________________________________________________________________________//

KVIDGrid *KVIDSi150CsI::GetTGIDGrid(const Char_t * tgid_name,
                                    Double_t xmax, Double_t xmin,
                                    Int_t ID_min, Int_t ID_max,
                                    Int_t npoints, Bool_t logscale)
{
   return KVTGIDManager::GetTGIDGrid(tgid_name, xmax, xmin, ID_min, ID_max,
                                     npoints, logscale);
}

//___________________________________________________________________________________________//

KVIDGrid *KVIDSi150CsI::GetTGIDGrid(const Char_t * idt_name,
                                    const Char_t * id_type,
                                    const Char_t * grid_type,
                                    Double_t xmax, Double_t xmin,
                                    Int_t ID_min, Int_t ID_max,
                                    Int_t npoints, Bool_t logscale)
{
   //Redefinition of KVTGIDManager::GetTGIDGrid in order to be able to visualise lines corresponding
   //to Tassan-Got functionals for A-identification : there is a functional for each Z, with 'type' = "A(Z=1)" etc. !!
   //Therefore this method constructs a single KVIDZAGrid when id_type = "A" with lines showing
   //the different isotopes.
   //REMEMBER: user's responsibility to delete the grid after use

   if (!strcmp(id_type, "A")) {
      //For "ZA" grids we have to loop over the functionals for Z=1 to Z=ZMAX_IDT
      KVIDZAGrid *grid = new KVIDZAGrid;        //make new grid
      //for default Zmin & Zmax we use those of the corresponding Z functional
      Int_t zmin = (Int_t)GetTGID(idt_name,"Z",grid_type)->GetIDmin();
      Int_t zmax = (Int_t)GetTGID(idt_name,"Z",grid_type)->GetIDmax();
      Int_t Z = TMath::Max(ID_min, zmin);
      zmax = (ID_max ? ID_max : zmax);
      Int_t Iso_Zmax = TMath::Min(ZMAX_IDT, zmax);
      while (Z <= Iso_Zmax) {
         KVTGID *_tgid = GetTGID(idt_name, Form("A(Z=%d)", Z), grid_type);
         if (_tgid) {
            _tgid->AddLineToGrid(grid, Z, npoints, xmin, xmax, logscale);
         }
         Z++;
      }
      //for any remaining lines we use the "Z" functional for "GG" or "PG"
      if (zmax > ZMAX_IDT) {
         KVTGID *_tgid = GetTGID(idt_name, "Z", grid_type);
         while (Z <= zmax) {
            _tgid->AddLineToGrid(grid, Z, npoints, xmin, xmax, logscale);
            Z++;
         }
      }
      return grid;
   }
   //if we did not ask for the "A" grid we let the standard method handle it
   return KVTGIDManager::
       GetTGIDGrid(GetTGIDName(idt_name, id_type, grid_type), xmax, xmin,
                   ID_min, ID_max, npoints, logscale);
}

//___________________________________________________________________________________________//

Double_t KVIDSi150CsI::IdentA(KVIDTelescope * idt, Double_t & funLTG,
                              const Char_t * grid_type, const Char_t * signal_type, Int_t Z)
{
   //Get A identification (we already know Z) for ID telescope idt using grid "grid_type"
   //funLTG is the ABSOLUTE value of the Tassan-Got function for the Z and the calculated
   //A i.e. in an ideal world it should equal zero. This gives an idea of the
   //quality of the identification.
   // funLTG = 0 -> identification parfaite
   //See Status code via GetStatus

   SetStatus(KVTGIDManager::kStatus_OK);
   funLTG = -1.;
   if (Z < 1) {
      SetStatus(KVTGIDManager::kStatus_ZtooSmall);
      return -1.;
   }
   //get A identification object from KVTGIDManager
   KVTGID *_tgid = GetTGID(idt->GetName(), Form("A(Z=%d)", Z), grid_type);
   //if no identification object exists, return -1
   if (!_tgid) {
      SetStatus(KVTGIDManager::kStatus_noTGID);
      return (-1.0);
   }
   //Initialise parameters of A identification function
   _tgid->SetParameter("X", idt->GetIDMapX(signal_type));
   _tgid->SetParameter("Y", idt->GetIDMapY(signal_type));
   _tgid->SetParameter("Z", Z);

   Double_t Afound = -1., Amin, Amax;
   Amin = 1. * Z - 0.5;
   Amax = 5. * Z;

   Afound = _tgid->GetIdentification(Amin, Amax, funLTG);

   if (_tgid->GetStatus() == KVTGID::kStatus_NotBetween_IDMin_IDMax)
      SetStatus(KVTGIDManager::kStatus_OutOfIDRange);

   return Afound;
}

//___________________________________________________________________________________________//

const Char_t *KVIDSi150CsI::GetIDSubCodeString(KVIDSubCode & concat) const
{
   //Returns string explaining subcodes for this telescope

   static TString messages[] = {
 "Z&A identification OK in SIGG-CSI",
"Z&A identification OK in SIPG-CSI",
 "Z identification OK, Z is > than max Z for which we have isotopic identification",
"Z id OK, mass ID attempted but point to identify outside of identification range of KVTGIDZA",
"Z id ok, mass ID attempted but Z passed to IdentA too small (<1)",
"Z id ok, mass ID attempted but Z passed to IdentA larger than max Z defined by KVTGIDZA  " ,   
 "Z identification OK. A not in KVNucleus mass table for this Z, ignored (A calculated from Z)",
 "Z identification OK. Z found greater than last complete Z-line in the SIPG-CSI matrix",
"Z identification OK. Mass-fit is bad for measured CsI total light (A calculated from Z)",
 "Module with coder saturation. SiPG>max canal for which ID possible. Z given is a minimum",
 "No Z identification. Bad news"
   };
   Int_t code = GetIDSubCode(concat);
   if (code == -1)
      return SubCodeNotSet();
   return messages[code];
}

//____________________________________________________________________________________

Bool_t KVIDSi150CsI::SetIdentificationParameters(const KVMultiDetArray* MDA)
{
   //Initialise the identification parameters (grids, etc.) of ALL identification telescopes of this
   //kind (label) in the multidetector array. Therefore this method need only be called once, and not
   //called for each telescope. The kind/label (returned by GetLabel) of the telescope corresponds
   //to the URI used to find the plugin class in $KVROOT/KVFiles/.kvrootrc.
   //
   //Parameters are read from the file with name given by the environment variable
   //INDRA_camp5.IdentificationParameterFile.SI150-CSI:       [filename]
   //
   //We also read the file given by
   //INDRA_camp5.IdentificationParameterFile.SI150-CSI.SIPGLimits:       [filename]
   //which gives, for certain telescopes, the maximum value of SIPG for which identification
   //is correct. Above this value, the high gain used for the Si150 caused the coder
   //to "double-back" on itself, making the data unusable.
   
   
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
   //read grids from file
   Info("SetIdentificationParameters", "Using file %s", path.Data());
   ifstream datfile;
   datfile.open(path.Data());
   
   KVString aline;
   int cou, mod, z;
   cou = mod = z = 0;
   KVIDSi150CsI *idt = 0;

   aline.ReadLine(datfile);
   while (datfile.good()) {

      if (!aline.BeginsWith('#')) {     //skip comments in header

         if (aline.BeginsWith("Ring")) {
            sscanf(aline.Data(), "Ring  %d", &cou);
         } else if (aline.BeginsWith("Module")) {
            sscanf(aline.Data(), "Module  %d", &mod);
            Char_t nom_tel[50];
            sprintf(nom_tel, "SI_CSI_%02d%02d", cou, mod);
            idt = (KVIDSi150CsI *) MDA->GetIDTelescope(nom_tel);
            if (!idt)
               Error("SetIdentificationParameters",
                     "ID telescope for ring=%d mod=%d not found", cou,
                     mod);
         } else if (aline.BeginsWith("Z=")) {
            sscanf(aline.Data(), " Z= %d", &z);
            Double_t param;
            KVTGID *_tgid_pg = 0, *_tgid_gg = 0;
            // need to create two id functionals for this Z - PG et GG
            //note if Z>0 it is an A-ident functional : mass ID for given Z
            if (z > 0) {
               /* petit gain */
               _tgid_pg =
                   new KVTGIDZA(idt->
                                GetTGIDName(idt->GetName(),
                                            Form("A(Z=%d)", z), "PG"),
                                "pawlowski_A", 0.1, 100., 14, 12, 13);
               _tgid_pg->SetParName(11, "Z");
               _tgid_pg->SetIDmin((Double_t) z);
               _tgid_pg->SetIDmax((Double_t) z);
               /* grand gain */
               _tgid_gg =
                   new KVTGIDZA(idt->
                                GetTGIDName(idt->GetName(),
                                            Form("A(Z=%d)", z), "GG"),
                                "pawlowski_A", 0.1, 100., 14, 12, 13);
               _tgid_gg->SetParName(11, "Z");
               _tgid_gg->SetIDmin((Double_t) z);
               _tgid_gg->SetIDmax((Double_t) z);
            } else {
               _tgid_pg =
                   new KVTGIDZ(idt->GetTGIDName(idt->GetName(), "Z", "PG"),
                               "pawlowski_Z", 0.1, 100., 13, 11, 12);
               int min,max;
               datfile >> min >> max;
               _tgid_pg->SetIDmin(min);
               _tgid_pg->SetIDmax(max);
               _tgid_gg =
                   new KVTGIDZ(idt->GetTGIDName(idt->GetName(), "Z", "GG"),
                               "pawlowski_Z", 0.1, 100., 13, 11, 12);
               datfile >> min >> max;
               _tgid_gg->SetIDmin(min);
               _tgid_gg->SetIDmax(max);
            }
            //check that parameters are not all equal to zero by summing them
            Double_t sum_par = 0.;
            for (int i = 0; i < 11; i++) {
               datfile >> param;
               sum_par += param;
               _tgid_pg->SetParameter(i, param);
            }
            if (sum_par == 0.0) {
               //params all = 0
               delete _tgid_pg;
               _tgid_pg = 0;
            }
            sum_par = 0.0;
            for (int i = 0; i < 11; i++) {
               datfile >> param;
               sum_par += param;
               _tgid_gg->SetParameter(i, param);
            }
            if (sum_par == 0.0) {
               //params all = 0
               delete _tgid_gg;
               _tgid_gg = 0;
            }
            for (int i = 0; i < 2; i++) {
               datfile >> param;
               idt->SetCorrParam(z, i, param);
            }

            if (_tgid_pg)
               idt->AddTGID(_tgid_pg);
            if (_tgid_gg)
               idt->AddTGID(_tgid_gg);
            if (_tgid_pg || _tgid_gg)
               idt->SetHasMassID();
         }
      }
      aline.ReadLine(datfile);
   }                            //while( datfile.good()
   datfile.close();
   
   //////////////////////////////////////////////////////////////////////
   //READ sipg LIMITS ABOVE WHICH CODER SATURATED DURING EXPERIMENT
   //
   filename = gDataSet->GetDataSetEnv( Form("IdentificationParameterFile.%s.SIPGLimits",GetLabel()) );
   if( filename == "" ){
      Warning("SetIdentificationParameters",
            "No filename defined. Should be given by %s.IdentificationParameterFile.%s.SIPGLimits",
            gDataSet->GetName(), GetLabel());
      return kFALSE;
   }
   if (!SearchKVFile(filename.Data(), path, gDataSet->GetName())){
      Error("SetIdentificationParameters",
            "File %s not found. Should be in $KVROOT/KVFiles/%s",
            filename.Data(), gDataSet->GetName());
      return kFALSE;
   }
   Info("SetIdentificationParameters", "Using file %s", path.Data());
   // read file with a TEnv
   TEnv sipglimits;
   if( sipglimits.ReadFile( path.Data(), kEnvLocal ) != 0 ){
      Error("SetIdentificationParameters", "TEnv::ReadFile != 0, cannot read %s file", filename.Data());
   }
   // get list of all similar telescopes
   KVCollection* si150csi = MDA->GetListOfIDTelescopes()->GetSubListWithLabel( GetLabel() );
   TIter next(si150csi); KVIDSi150CsI* tt=0;
   while( (tt = (KVIDSi150CsI*)next()) ){
      if( sipglimits.Defined( Form("SIPG_Max.%s", tt->GetName()) ) ){
         tt->SetMaxCanalSiPG( sipglimits.GetValue( Form("SIPG_Max.%s", tt->GetName()), 50.0 ) );
      }
   }
   
   
   //////////////////////////////////////////////////////////////////////
   //READ MINIMUM VALUE OF CSI TOTAL LIGHT FOR ISOTOPIC IDENTIFICATION
   //TO BE VALID FOR EACH Z
   //
   filename = gDataSet->GetDataSetEnv( Form("IdentificationParameterFile.%s.CsILightMin",GetLabel()) );
   if( filename == "" ){
      Warning("SetIdentificationParameters",
            "No filename defined. Should be given by %s.IdentificationParameterFile.%s.CsILightMin",
            gDataSet->GetName(), GetLabel());
      return kFALSE;
   }
   if (!SearchKVFile(filename.Data(), path, gDataSet->GetName())){
      Error("SetIdentificationParameters",
            "File %s not found. Should be in $KVROOT/KVFiles/%s",
            filename.Data(), gDataSet->GetName());
      return kFALSE;
   }
   Info("SetIdentificationParameters", "Using file %s", path.Data());
   // read file with a TEnv
   TEnv lumlimits;
   if( lumlimits.ReadFile( path.Data(), kEnvLocal ) != 0 ){
      Error("SetIdentificationParameters", "TEnv::ReadFile != 0, cannot read %s file", filename.Data());
   }
   // get list of all similar telescopes
   next.Reset(); tt=0;
   while( (tt = (KVIDSi150CsI*)next()) ){
      for(int z=1; z<=ZMAX_IDT; z++){
         tt->SetMinLumiere( z, lumlimits.GetValue( Form("Lum_min_iso.%s.Z=%d", tt->GetName(), z), 0 ));
      }
   }
   delete si150csi;
   return kTRUE;
}

//____________________________________________________________________________________

void KVIDSi150CsI::RemoveIdentificationParameters()
{
  //Delete any KVTGID objects associated with this telescope
  //Reset fMaxSIPG to 5000
  RemoveAllTGID();
  SetHasMassID(kFALSE);
}

//____________________________________________________________________________________

void KVIDSi150CsI::Initialize()
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
