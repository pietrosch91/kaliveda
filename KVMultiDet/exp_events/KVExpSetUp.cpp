//Created by KVClassFactory on Thu Feb 25 14:59:59 2016
//Author: bonnet,,,

#include "KVExpSetUp.h"
#include "KVDataSetManager.h"
#include "TClass.h"

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

   KVGeoNavigator* nav = 0;
   KVMultiDetArray* tmp = 0;
   lmultidetarrayclasses =
      gDataSetManager->GetDataSet(fDataSet)->GetDataSetEnv("DataSet.ExpSetUp.ClassList", IsA()->GetName());
   lmultidetarrayclasses.Begin(" ");
   while (!lmultidetarrayclasses.End()) {
      KVString sname = lmultidetarrayclasses.Next();
      Info("Build", "Build %s %s\n", gDataSet->GetName(), sname.Data());
      tmp = MakeMultiDetector(gDataSet->GetName(), -1, sname.Data());
      if (tmp) {
         if (nav) {
            Info("Build", "KVGeoNavigator already set ...");
            tmp->SetNavigator(nav);
         }
         // make sure array is using ROOT geometry
         tmp->CheckROOTGeometry();

         fMDAList.Add(tmp);
         fDetectors.AddAll((KVUniqueNameList*)tmp->GetDetectors());
         fStructures.AddAll((KVUniqueNameList*)tmp->GetStructures());
         fIDTelescopes->AddAll(tmp->GetListOfIDTelescopes());

         nav = tmp->GetNavigator();
      } else {
         Error("Build", "NULL pointer returned by MakeMultiDetector");
      }
   }

   gGeoManager->DefaultColors();
   gGeoManager->CloseGeometry();
   SetNavigator(nav);
   SetGeometry(gGeoManager);

   gMultiDetArray = this;
   SetBit(kIsBuilt);
}
