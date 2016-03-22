//Created by KVClassFactory on Thu Feb 25 14:59:59 2016
//Author: bonnet,,,

#include "KVExpSetUp.h"
#include "KVDataSetManager.h"
#include "TClass.h"

#include <KVRangeTableGeoNavigator.h>

ClassImp(KVExpSetUp)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVExpSetUp</h2>
<h4>Describe an experimental set-up made of several KVMultiDetArray objects</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVExpSetUp::init()
{
   fCloseGeometryNow = kFALSE;
   fBuildTarget = kFALSE;

   //modification of the owner mode compare to Mother classes
   fStructures.SetOwner(kFALSE);
   fIDTelescopes->SetOwner(kFALSE);

   gMultiDetArray = this;
}


KVExpSetUp::KVExpSetUp()
{
   // Default constructor
   init();
}

KVExpSetUp::~KVExpSetUp()
{
   // Destructor

}


void KVExpSetUp::Build(Int_t)
{
   // Build the combined arrays
   CreateGeoManager();

   Info("Build", "navigator=%p", GetNavigator());

   KVRangeTableGeoNavigator* gnl = new KVRangeTableGeoNavigator(gGeoManager, KVMaterial::GetRangeTable());
   SetNavigator(gnl);

   //KVGeoNavigator* nav = 0;
   KVMultiDetArray* tmp = 0;
   lmultidetarrayclasses =
      gDataSetManager->GetDataSet(fDataSet)->GetDataSetEnv("DataSet.ExpSetUp.ClassList", IsA()->GetName());
   lmultidetarrayclasses.Begin(" ");
   while (!lmultidetarrayclasses.End()) {
      KVString sname = lmultidetarrayclasses.Next();
      Info("Build", "Build %s %s\n", gDataSet->GetName(), sname.Data());
      tmp = MakeMultiDetector(gDataSet->GetName(), -1, sname.Data());
      if (tmp) {
         // make sure array is using ROOT geometry
         tmp->CheckROOTGeometry();

         fMDAList.Add(tmp);
         fDetectors.AddAll((KVUniqueNameList*)tmp->GetDetectors());
         fStructures.AddAll((KVUniqueNameList*)tmp->GetStructures());
         fIDTelescopes->AddAll(tmp->GetListOfIDTelescopes());

         // retrieve correspondance list node path<->detector
         gnl->AbsorbDetectorPaths(tmp->GetNavigator());

      } else {
         Error("Build", "NULL pointer returned by MakeMultiDetector");
      }
   }

   gGeoManager->DefaultColors();
   gGeoManager->CloseGeometry();

   SetGeometry(gGeoManager);

   gMultiDetArray = this;
   SetBit(kIsBuilt);
}
