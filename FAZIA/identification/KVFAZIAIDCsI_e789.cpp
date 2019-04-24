//Created by KVClassFactory on Tue Sep  8 16:00:28 2015
//Author: ,,,

#include "KVFAZIAIDCsI_e789.h"
#include "KVIDGCsI.h"
#include "KVIdentificationResult.h"
#include "KVFAZIADetector.h"
#include "KVDataSet.h"

ClassImp(KVFAZIAIDCsI_e789)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIAIDCsI_e789</h2>
<h4>id telescope to manage FAZIA CsI identification</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVFAZIAIDCsI_e789::KVFAZIAIDCsI_e789()
{
   // Default constructor
   SetType("CsI");
   CsIGrid = 0;
   fCsI = 0;
   /* in principle all CsI R-L telescopes can identify mass & charge */
   SetHasMassID(kTRUE);
}

KVFAZIAIDCsI_e789::~KVFAZIAIDCsI_e789()
{
   // Destructor
}

Bool_t KVFAZIAIDCsI_e789::Identify(KVIdentificationResult* IDR, Double_t x, Double_t y)
{
   //Particle identification and code setting using identification grid KVIDGCsI* fGrid.

   IDR->SetIDType(GetType());
   IDR->IDattempted = kTRUE;

   //perform identification
   Double_t csir = (y < 0. ? GetIDMapY() : y);
   Double_t csil = (x < 0. ? GetIDMapX() : x);

   CsIGrid->Identify(csil, csir, IDR);

   // set general ID code
   IDR->IDcode = GetIDCode();

   // general ID code for gammas
   if (IDR->IDquality == KVIDGCsI::kICODE10)
      IDR->IDcode = 0;

   return kTRUE;

}
//____________________________________________________________________________________

Double_t KVFAZIAIDCsI_e789::GetIDMapX(Option_t*)
{
   //X-coordinate for CsI identification map :
   // computed slow componment
   // of the charge signal
   return fCsI->GetQ3FPGAEnergy();
}

//____________________________________________________________________________________

Double_t KVFAZIAIDCsI_e789::GetIDMapY(Option_t*)
{
   //Y-coordinate for CsI identification map :
   // computed fast componment
   // of the charge signal
   return fCsI->GetQ3FastFPGAEnergy() / fCsI->GetQ3FPGAEnergy();
}
//____________________________________________________________________________________


void KVFAZIAIDCsI_e789::Initialize()
{
   // Initialisation of telescope before identification.
   // This method MUST be called once before any identification is attempted.
   // Initialisation of grid is performed here.
   // IsReadyForID() will return kTRUE if a grid is associated to this telescope for the current run.

   CsIGrid = (KVIDGCsI*) GetIDGrid();
   fCsI = (KVFAZIADetector*)GetDetector(1);
   if (CsIGrid) {
      CsIGrid->Initialize();
      SetBit(kReadyForID);
   }
   else {
      ResetBit(kReadyForID);
   }
   if (!gDataSet->HasCalibIdentInfos()) SetBit(kReadyForID);
}
