/*
$Id: KVIDSiLiCsI_camp5.cpp,v 1.1 2009/03/17 08:51:57 franklan Exp $
$Revision: 1.1 $
$Date: 2009/03/17 08:51:57 $
*/

//Created by KVClassFactory on Mon Mar 16 09:50:33 2009
//Author: John Frankland,,,

#include "KVIDSiLiCsI_camp5.h"
#include "KVIDGridManager.h"
#include "KVINDRA.h"
#include "KVINDRAReconNuc.h"

ClassImp(KVIDSiLiCsI_camp5)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDSiLiCsI_camp5</h2>
<h4>SiLi-CsI identification for INDRA_camp5 dataset</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDSiLiCsI_camp5::KVIDSiLiCsI_camp5()
{
   // Default constructor
}

KVIDSiLiCsI_camp5::~KVIDSiLiCsI_camp5()
{
   // Destructor
}

Bool_t KVIDSiLiCsI_camp5::SetIdentificationParameters(const KVMultiDetArray* MDA)
{
   // Initialise the identification parameters (grids, etc.) of ALL identification telescopes of this
   // kind (label) in the multidetector array. Therefore this method need only be called once, and not
   // called for each telescope. The kind/label (returned by GetLabel) of the telescope corresponds
   // to the URI used to find the plugin class in $KVROOT/KVFiles/.kvrootrc.
   //
   // Parameters are read from the files with names given by the environment variables:
   // INDRA_camp5.IdentificationParameterFile.SILI-CSI.GG:       [filename]
   // INDRA_camp5.IdentificationParameterFile.SILI-CSI.PG:       [filename]

      
   // ****************************** GRAND GAIN ********************************//
   TString filename = gDataSet->GetDataSetEnv( Form("IdentificationParameterFile.%s.GG",GetLabel()) );
   if( filename == "" ){
      Warning("SetIdentificationParameters",
            "No filename defined for GG Z/A grids. Should be given by %s.IdentificationParameterFile.%s.GG",
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
   Info("SetIdentificationParameters", "Reading GG Z/A grids from file : %s", path.Data());
   gIDGridManager->ReadAsciiFile(path.Data());
   
   // ****************************** PETIT GAIN ********************************//
   filename = gDataSet->GetDataSetEnv( Form("IdentificationParameterFile.%s.PG",GetLabel()) );
   if( filename == "" ){
      Warning("SetIdentificationParameters",
            "No filename defined for PG Z/A grids. Should be given by %s.IdentificationParameterFile.%s.PG",
            gDataSet->GetName(), GetLabel());
      return kFALSE;
   }
   if (!SearchKVFile(filename.Data(), path, gDataSet->GetName())){
      Error("SetIdentificationParameters",
            "File %s not found. Should be in $KVROOT/KVFiles/%s",
            filename.Data(), gDataSet->GetName());
      return kFALSE;
   }
   
   //read grids from file
   Info("SetIdentificationParameters", "Reading PG Z/A grids from file : %s", path.Data());
   gIDGridManager->ReadAsciiFile(path.Data());
   
   return kTRUE;
}


//___________________________________________________________________________________________//

Bool_t KVIDSiLiCsI_camp5::SetIDGrid(KVIDGraph *grid)
{
   // Called by KVIDGridManager::FindGrid in order to set grids for telescope.
   // We check ring & module number, "ID Type" (='SILI-CSI'), and run number.
   // NB. if this method returns kTRUE, KVIDGridManager::FindGrid stops searching
   // for grids for this telescope. Therefore, we never return kTRUE as there can be
   // several grids for each telescope.
   
   //check identification type
   if( strcmp(grid->GetParameters()->GetStringValue("ID Type"), "SILI-CSI") ) return kFALSE;

	if( !grid->HandlesIDTelescope(this) )   
      return kFALSE;
   
   //get run number from INDRA, if it exists (should do!), otherwise accept
   if (gIndra) {
      Int_t run = gIndra->GetCurrentRunNumber();
      if (!grid->GetRuns().Contains(run)) return kFALSE;
   }
   
   //the grid is accepted
   fIDGrids->Add(grid);
      
   return kFALSE; // make gIDGridManager keep searching!
}


//___________________________________________________________________________________________

void KVIDSiLiCsI_camp5::Initialize()
{
   // Initialize telescope for current run.
   // Pointers to grids for run are set, and if there is at least 1 (GG) grid,
   // we set fCanIdentify = kTRUE
   // "Natural" line widths are calculated for KVIDZAGrids.
   
   fGGgrid = (KVIDZAGrid*)GetIDGrid("GG");
   if( fGGgrid ){
      SetBit(kReadyForID);
      fGGgrid->Initialize();
   }
   fPGgrid = (KVIDZAGrid*)GetIDGrid("PG");
   if( fPGgrid ) fPGgrid->Initialize();
}

//___________________________________________________________________________________________

Double_t KVIDSiLiCsI_camp5::GetIDMapX(Option_t * opt)
{
   //X-coordinate for SiLi-CsI identification map is raw "R" coder value for CsI detector
   return (Double_t) GetDetector(2)->GetACQData("R");
}

//____________________________________________________________________________________

Double_t KVIDSiLiCsI_camp5::GetIDMapY(Option_t * opt)
{
   //Y-coordinate for SiLi-CsI identification map is raw "GG" or "PG" coder value,
   //depending on 'opt' string.
   return (Double_t) GetDetector(1)->GetACQData(opt);
}

//________________________________________________________________________________________//

Bool_t KVIDSiLiCsI_camp5::Identify(KVReconstructedNucleus * nuc)
{
   //Particle identification and code setting using identification grids

      KVINDRAReconNuc *irnuc = (KVINDRAReconNuc *) nuc;
      
      //perform identification in SiLi(GG) - CsI(R) map
      
      Double_t sili = GetIDMapY("GG");
      Double_t csir = GetIDMapX();
      
      KVIDGrid* theIdentifyingGrid = 0;
      
      fGGgrid->Identify(csir, sili, irnuc);
      
      if( fGGgrid->GetQualityCode() > 5 && fPGgrid ){ // any code > 5 means we have to try PG grid (if there is one)
         
         // try Z & A identification in SiLi(PG)-CsI(R) map
         sili = GetIDMapY("PG");
         fPGgrid->Identify(sili, csir, irnuc);
         
         if( fPGgrid->GetQualityCode() > 5 ){
            
               // complete and utter failure
               //set subcode 8 in particle
               SetIDSubCode(irnuc->GetCodes().GetSubCodes(), KVIDZAGrid::kICODE8);
					
         }
         else
         {
            theIdentifyingGrid = (KVIDGrid*)fPGgrid;
         }
      }
      else
      {
         theIdentifyingGrid =(KVIDGrid*)fGGgrid;
      }

      //set subcode in particle
      SetIDSubCode(irnuc->GetCodes().GetSubCodes(), theIdentifyingGrid->GetQualityCode());

      //ID totally unsuccessful if ICode=8
      if (theIdentifyingGrid->GetQualityCode() == KVIDZAGrid::kICODE8)
         return kFALSE;

      //ID should be attempted in preceding telescope if ICode=6 or 7
      if (theIdentifyingGrid->GetQualityCode() == KVIDZAGrid::kICODE6
          || theIdentifyingGrid->GetQualityCode() == KVIDZAGrid::kICODE7)
         return kFALSE;

      return kTRUE;
}


