//Created by KVClassFactory on Thu Feb 25 14:59:59 2016
//Author: bonnet,,,

#include "KVExpSetUp.h"
#include "KVDataSetManager.h"
#include "TClass.h"
#include "KVReconstructedNucleus.h"

#include <KVGroup.h>
#include <KVRangeTableGeoNavigator.h>

#ifdef WITH_MFM
#include "KVMFMDataFileReader.h"
#endif

ClassImp(KVExpSetUp)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVExpSetUp</h2>
<h4>Describe an experimental set-up made of several KVMultiDetArray objects</h4>
<!-- */
// --> END_HTML
// All detector & structure names in individual arrays must be unique!
// The group numbering has to be modified (in Build()) to make them unique.
////////////////////////////////////////////////////////////////////////////////

void KVExpSetUp::init()
{
   fBuildTarget = kFALSE;
   fCloseGeometryNow = kFALSE;
   //modification of the owner mode compare to Mother classes
   fStructures.SetOwner(kFALSE);
   fIDTelescopes->SetOwner(kFALSE);
}


KVExpSetUp::KVExpSetUp()
{
   // Default constructor
   init();
}

KVExpSetUp::~KVExpSetUp()
{
   // Destructor
   fCloseGeometryNow = kTRUE;
}


void KVExpSetUp::Build(Int_t run)
{
   // Build the combined arrays

   // default ROOT geometry for all arrays
   gEnv->SetValue("KVMultiDetArray.ROOTGeometry", "yes");

   CreateGeoManager();

   Info("Build", "navigator=%p", GetNavigator());

   KVRangeTableGeoNavigator* gnl = new KVRangeTableGeoNavigator(gGeoManager, KVMaterial::GetRangeTable());
   SetNavigator(gnl);

   KVMultiDetArray* tmp(nullptr);
   Int_t group_offset = 0;// for renumbering groups
   lmultidetarrayclasses =
      gDataSetManager->GetDataSet(fDataSet)->GetDataSetEnv("DataSet.ExpSetUp.ClassList", IsA()->GetName());
   lmultidetarrayclasses.Begin(" ");
   while (!lmultidetarrayclasses.End()) {
      KVString sname = lmultidetarrayclasses.Next();
      Info("Build", "Build %s %s\n", gDataSet->GetName(), sname.Data());
      gMultiDetArray = nullptr; //otherwise MakeMultiDetector will delete any previously built array
      tmp = MakeMultiDetector(gDataSet->GetName(), run, sname.Data());
      if (tmp) {
         fMDAList.Add(tmp);
      }
      else {
         Error("Build", "NULL pointer returned by MakeMultiDetector");
      }
   }

   gGeoManager->DefaultColors();
   gGeoManager->CloseGeometry();

   TIter nxt_mda(&fMDAList);
   while ((tmp = (KVMultiDetArray*)nxt_mda())) {
      tmp->PerformClosedROOTGeometryOperations(run);
      unique_ptr<KVSeqCollection> groups(tmp->GetStructureTypeList("GROUP"));
      if (group_offset) {
         // renumber all groups to keep unique names/numbers
         TIter next(groups.get());
         KVGroup* group;
         while ((group = (KVGroup*)next())) {
            Int_t group_number = group->GetNumber();
            group->SetNumber(group_number + group_offset);
         }
      }
      else
         group_offset += groups->GetEntries();

      fDetectors.AddAll((KVUniqueNameList*)tmp->GetDetectors());
      fStructures.AddAll((KVUniqueNameList*)tmp->GetStructures());
      fIDTelescopes->AddAll(tmp->GetListOfIDTelescopes());

      // retrieve correspondance list node path<->detector
      gnl->AbsorbDetectorPaths(tmp->GetNavigator());
   }

   SetGeometry(gGeoManager);

   gMultiDetArray = this;
   fCurrentRun = (run > 0 ? run : 0);
   SetBit(kIsBuilt);
}

void KVExpSetUp::SetParameters(UShort_t n)
{
   // Call SetParameters for each multidetector, each can use a different KVUpDater
   fCurrentRun = n;
   fMDAList.Execute("SetParameters", Form("%ud", n));
}

void KVExpSetUp::FillDetectorList(KVReconstructedNucleus* rnuc, KVHashList* DetList, const KVString& DetNames)
{
   // Call FillDetectorList for each array in turn, until DetList gets filled
   // This is because some arrays may override the KVMultiDetArray::FillDetectorList method

   TIter next_array(&fMDAList);
   KVMultiDetArray* mda;
   while ((mda = (KVMultiDetArray*)next_array())) {
      mda->FillDetectorList(rnuc, DetList, DetNames);
      if (!DetList->IsEmpty()) break;
   }
}

void KVExpSetUp::AcceptParticleForAnalysis(KVReconstructedNucleus* NUC) const
{
   // Overrides KVMultiDetArray method
   // We use the "ARRAY" parameter of the reconstructed particle (if set)
   // to know which array of the setup detected it.

   if (NUC->GetParameters()->HasStringParameter("ARRAY")) {
      KVMultiDetArray* whichArray = GetArray(NUC->GetParameters()->GetStringValue("ARRAY"));
      if (whichArray) {
         whichArray->AcceptParticleForAnalysis(NUC);
      }
      else
         Warning("AcceptParticleForAnalysis", "ReconstructedNucleus has ARRAY=%s but no KVMultiDetArray with this name in set-up",
                 NUC->GetParameters()->GetStringValue("ARRAY"));
   }
   else {
      Warning("AcceptParticleForAnalysis", "ReconstructedNucleus has no ARRAY parameter: cannot determine correct KVMultiDetArray");
   }
}

void KVExpSetUp::GetDetectorEvent(KVDetectorEvent* detev, const TSeqCollection* fired_params)
{
   // Override KVMultiDetArray method
   // Call each array in turn and add fired groups to the KVDetectorEvent

   TIter next_array(&fMDAList);
   KVMultiDetArray* mda;
   while ((mda = (KVMultiDetArray*)next_array())) {
      mda->GetDetectorEvent(detev, fired_params);
   }
}

KVGroupReconstructor* KVExpSetUp::GetReconstructorForGroup(const KVGroup* g) const
{
   // Override KVMultiDetArray method
   // Call each array in turn to get reconstructor for group
   TIter next_array(&fMDAList);
   KVMultiDetArray* mda;
   KVGroupReconstructor* gr(nullptr);
   while ((mda = (KVMultiDetArray*)next_array())) {
      if ((gr = mda->GetReconstructorForGroup(g))) break;
   }
   return gr;
}

#ifdef WITH_MFM
Bool_t KVExpSetUp::handle_raw_data_event_mfmframe(const MFMCommonFrame& mfmframe)
{
   // Handle single (not merged) MFM frames of raw data. It is assumed
   // that each frame type corresponds to a different detector array.
   // Therefore as soon as one of them treats the data in the frame,
   // we return kTRUE.

   TIter next_array(&fMDAList);
   KVMultiDetArray* mda;
   while ((mda = (KVMultiDetArray*)next_array())) {
      if (mda->handle_raw_data_event_mfmframe(mfmframe)) {
         Info("handle_raw_data_event_mfmframe", "Handled frame for %s", mda->GetName());
         return kTRUE;
      }
   }
   return kFALSE;
}
#endif
