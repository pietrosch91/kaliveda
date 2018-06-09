//Created by KVClassFactory on Sat Jun  9 20:07:03 2018
//Author: John Frankland,,,

#include "KVDataPatch_Camp2MassID.h"
#include "KVINDRAReconNuc.h"
ClassImp(KVDataPatch_Camp2MassID)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVDataPatch_Camp2MassID</h2>
<h4>Patch to correct 2nd campaign A-identification status</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVDataPatch_Camp2MassID::KVDataPatch_Camp2MassID()
   : KVDataPatch()
{
   // Default constructor
}

//____________________________________________________________________________//

KVDataPatch_Camp2MassID::~KVDataPatch_Camp2MassID()
{
   // Destructor
}

void KVDataPatch_Camp2MassID::ApplyToParticle(KVNucleus* n)
{
   KVINDRAReconNuc* irn = dynamic_cast<KVINDRAReconNuc*>(n);
   if (irn->IsAMeasured() && !irn->GetCodes().TestIDCode(kIDCode_CsI)) {
      irn->SetAMeasured(kFALSE);
      irn->SetMassFormula(KVNucleus::kVedaMass);
      irn->SetZ(irn->GetZ());
   }
}

void KVDataPatch_Camp2MassID::PrintPatchInfo() const
{
   std::cout << "Correct bad attribution of isotopic resolution & mass in 2nd campaign data" << std::endl;
   std::cout << "Only light (Z<5) particles were identified in mass." << std::endl;
   std::cout << "The 1st campaign convention of giving negative A values to particles whose mass" << std::endl;
   std::cout << "was only calculated from Z (using the VEDA mass formula) was not respected for this" << std::endl;
   std::cout << "campaign: as a result, when initially imported into KaliVeda, everything was labelled" << std::endl;
   std::cout << "as being isotopically identified (KVReconstructedNucleus::IsAMeasured() returns kTRUE)." << std::endl;
   std::cout << "In addition, calculated masses seem not to have been applied coherently, with each element" << std::endl;
   std::cout << "appearing with two masses, separated by 2 mass units...[\"Curiouser and curiouser\", said Alice]" << std::endl;
}

//____________________________________________________________________________//

