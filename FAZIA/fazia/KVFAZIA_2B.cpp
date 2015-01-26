//Created by KVClassFactory on Thu Jan 22 16:02:02 2015
//Author: ,,,

#include "KVFAZIA_2B.h"
#include "FAZIABlock.h"
#include "INDRAGeometryBuilder.h"
#include "KVMaterial.h"
#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"
#include "KVGeoImport.h"
#include "TROOT.h"
#include "KVUnits.h"
#include "TMath.h"
#include "KVIDTelescope.h"

#include "KVGroup.h"
#include "KVDetectorEvent.h"
#include "KVSignal.h"

ClassImp(KVFAZIA_2B)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIA_2B</h2>
<h4>FAZIA set-up with two blocks used in LNS 2014 commissioning</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVFAZIA_2B::KVFAZIA_2B()
{
   // Default constructor
   SetName("FAZIA-2B");
   SetTitle("FAZIA commissioning set-up");

   fFGeoType = gEnv->GetValue("FAZIA-2B.GeoType", "2B");
   fFDist    = gEnv->GetValue("FAZIA-2B.DistanceFAZIA", 100.0);
   fFThetaMin = gEnv->GetValue("FAZIA-2B.ThetaMinCompactGeo", 0.5);
	gMultiDetArray = this;
}

KVFAZIA_2B::~KVFAZIA_2B()
{
   // Destructor
}

void KVFAZIA_2B::Build(Int_t run)
{
   // Build the combined INDRA & FAZIA arrays

   
   if (!gGeoManager) {
       new TGeoManager("FAZIA", Form("FAZIA geometry for dataset %s", gDataSet->GetName()));

       TGeoMaterial*matVacuum = gGeoManager->GetMaterial("Vacuum");
       if(!matVacuum) {
           matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
           matVacuum->SetTitle("Vacuum");
       }
       TGeoMedium*Vacuum = gGeoManager->GetMedium("Vacuum");
       if(!Vacuum) Vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
       TGeoVolume *top = gGeoManager->MakeBox("WORLD", Vacuum,  500, 500, 500);
       gGeoManager->SetTopVolume(top);
   }
   
   if (fFGeoType == "2B") BuildFAZIA();
   else {
      Fatal("Build", "Unknown geometry type: %s. Perhaps you should implement it?",
            fFGeoType.Data());
   }
   
   KVGeoImport imp(gGeoManager, KVMaterial::GetRangeTable(), this,kTRUE);
   imp.SetDetectorPlugin(ClassName());
   imp.SetNameCorrespondanceList("FAZIA.names");
   
   // the following parameters are optimized for a 12-block compact
	// geometry placed at 80cm with rings 1-5 of INDRA removed.
	// make sure that the expected number of detectors get imported!
   imp.ImportGeometry(0.25, 1, 2., 0, 14);
   
}

void KVFAZIA_2B::GetDetectedEvent(KVDetectorEvent* detev, TClonesArray* signals)
{
    // First step in event reconstruction based on current status of detectors in array.
    // Fills the given KVDetectorEvent with the list of all groups which have fired.
    // i.e. loop over all groups of the array and test whether KVGroup::Fired() returns true or false.
    //
    // If the list of fired acquisition parameters 'fired_params' is given, then we use this list
    // to find, first, the associated fired detectors, then, the associated groups. This is possible when
    // reading raw data using an object derived from KVRawDataReader:
    //
    //     KVRawDataReader *run = ... ; // base pointer to object used to read data
    //     if( run->GetNextEvent() )  // read an event and test that all is well
    //      {
    //           KVSeqCollection* fired = run->GetFiredDataParameters(); // get fired acquisition parameters
    //           gMultiDetArray->GetDetectorEvent(detev, fired); // build list of fired groups
    //       }
    //
    // The KVDetectorEvent object can then be used by KVReconstructedEvent::ReconstructEvent
    // in order to generate a list of particles (KVReconstructedNucleus).
    //
    // Call method detev->Clear() before reading another event in order to reset all of the hit groups
    // (including all detectors etc.) and emptying the list.

    if (signals){
        // list of fired acquisition parameters given
        TIter next_par(signals);
        
        KVSignal* par = 0;
        KVDetector* det = 0;
        KVGroup* grp = 0;
        while ( (par = (KVSignal*)next_par()) ){ 
        		par->DeduceFromName();
            par->ComputeGlobals();
            if ( (det = GetDetector( par->GetDetectorName() )) )
            {
            	printf("%s\n",par->GetDetectorName());
               //if ( (grp = det->GetGroup()) && grp->GetParents()->Contains( this) ) {
               if ( (grp = det->GetGroup())  && !detev->FindObject(grp) ) {
               	detev->AddGroup(grp);
            	}
            }
        }
        /*
        code de KVMultiDetArray::GetDetectorEvent
        while ( (par = (KVSignal*)next_par()) ){
            if ( (det = par->GetDetector()) ){
                if ( (grp = det->GetGroup()) && grp->GetParents()->Contains( this) ) detev->AddGroup(grp);
            }
        }
        */
    }
    else
    {
        KVMultiDetArray::GetDetectorEvent(detev,0);
    }
    
}

void KVFAZIA_2B::GetIDTelescopes(KVDetector *de, KVDetector *e, TCollection *idtels)
{
    // Add some ID telescopes for FAZIA array

    KVMultiDetArray::GetIDTelescopes(de,e,idtels);
}

void KVFAZIA_2B::BuildFAZIA()
{
   Info("BuildFAZIA", "Compact geometry, %f cm from target, theta-min=%f deg.",
        fFDist, fFThetaMin);

   TGeoVolume* top = gGeoManager->GetTopVolume();

   Double_t distance_block_cible = fFDist * KVUnits::cm;
   Double_t thick_si1 = 300 * KVUnits::um;
   TGeoTranslation trans;
   trans.SetDz(distance_block_cible + thick_si1 / 2.);

   FAZIABlock* block = new FAZIABlock;
	
   Int_t block_starting_number=0;
   
   TGeoRotation rot1, rot2;
   TGeoHMatrix h;
   TGeoHMatrix* ph = 0;
   Double_t theta = 0;
   Double_t phi = 0;

   Double_t theta_min = fFThetaMin;//smallest lab polar angle in degrees
   Double_t centre_hole = 2.*tan(theta_min * TMath::DegToRad()) * distance_block_cible;
   Double_t dx = (block->GetTotalSideWithBlindage()) / 2.;

   /*BLOCK 1*/
   TVector3 cent_b1(dx - centre_hole / 2, -(dx + centre_hole) + centre_hole / 2, distance_block_cible); //position centre of block 1
   theta = cent_b1.Theta() * TMath::RadToDeg();
   phi = cent_b1.Phi() * TMath::RadToDeg();
   rot2.SetAngles(phi + 90., theta, 0.);
   rot1.SetAngles(-1.*phi, 0., 0.);
   h = rot2 * trans * rot1;
   ph = new TGeoHMatrix(h);
   top->AddNode(block, block_starting_number++, ph);
	/*BLOCK 2*/
   TVector3 cent_b2(dx + centre_hole - centre_hole / 2, dx - centre_hole + centre_hole / 2, distance_block_cible); //position centre of block 2
   theta = cent_b2.Theta() * TMath::RadToDeg();
   phi = cent_b2.Phi() * TMath::RadToDeg();
   rot2.SetAngles(phi + 90., theta, 0.);
   rot1.SetAngles(-1.*phi, 0., 0.);
   h = rot2 * trans * rot1;
   ph = new TGeoHMatrix(h);
   top->AddNode(block, block_starting_number++, ph);
   /*BLOCK 3*/
   TVector3 cent_b3(-dx + centre_hole - centre_hole / 2, dx + centre_hole / 2, distance_block_cible); //position centre of block 3
   theta = cent_b3.Theta() * TMath::RadToDeg();
   phi = cent_b3.Phi() * TMath::RadToDeg();
   rot2.SetAngles(phi + 90., theta, 0.);
   rot1.SetAngles(-1.*phi, 0., 0.);
   h = rot2 * trans * rot1;
   ph = new TGeoHMatrix(h);
   top->AddNode(block, block_starting_number++, ph);
   /*BLOCK 4*/
   TVector3 cent_b4(-dx - centre_hole / 2, -dx + centre_hole / 2, distance_block_cible); //position centre of block 4
   theta = cent_b4.Theta() * TMath::RadToDeg();
   phi = cent_b4.Phi() * TMath::RadToDeg();
   rot2.SetAngles(phi + 90., theta, 0.);
   rot1.SetAngles(-1.*phi, 0., 0.);
   h = rot2 * trans * rot1;
   ph = new TGeoHMatrix(h);
   top->AddNode(block, block_starting_number++, ph);

	gGeoManager->CloseGeometry();
   gGeoManager->DefaultColors();

}
