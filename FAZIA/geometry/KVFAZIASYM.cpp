//Created by KVClassFactory on Tue Jan 27 11:38:09 2015
//Author: ,,,

#include "KVFAZIASYM.h"
#include "KVUnits.h"
#include "KVFAZIABlock.h"

ClassImp(KVFAZIASYM)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIASYM</h2>
<h4>Description of the FAZIA set up</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVFAZIASYM::KVFAZIASYM()
{
   // Default constructor
}

KVFAZIASYM::~KVFAZIASYM()
{
   // Destructor
}

void KVFAZIASYM::GetGeometryParameters()
{

   fNblocks = 4;
   fFDist = 80.0;
   fFThetaMin = 2.1;
}


void KVFAZIASYM::BuildFAZIA()
{
   Info("BuildFAZIA", "Compact geometry, %f cm from target",
        fFDist);

   TGeoVolume* top = gGeoManager->GetTopVolume();

   Double_t distance_block_cible = fFDist * KVUnits::cm;
   Double_t thick_si1 = 300 * KVUnits::um;
   TGeoTranslation trans;
   trans.SetDz(distance_block_cible + thick_si1 / 2.);

   KVFAZIABlock* block = new KVFAZIABlock;

   TGeoRotation rot1, rot2;
   TGeoHMatrix h;
   TGeoHMatrix* ph = 0;
   Double_t theta = 0;
   Double_t phi = 0;

   Double_t theta_min = fFThetaMin;//smallest lab polar angle in degrees
   Double_t centre_hole = 2.*tan(theta_min * TMath::DegToRad()) * distance_block_cible;
   Double_t dx = (block->GetTotalSideWithBlindage()) / 2.;

   /*
   Double_t arc = block->GetTotalSideWithBlindage() / (distance_block_cible + thick_si1 / 2.);
   arc /= 2;
   arc *= TMath::RadToDeg();
   */
   fNblocks = 4;
   //theta theorique 5.2
   //dphi trouve par tatonnement ...
   /*
   Double_t dphi = 20;
   dphi -= 5;

   phi = -1 * (dphi + 90);
   */
   TVector3 centre;
   for (Int_t bb = 0; bb < fNblocks; bb += 1) {

      if (bb == 1)        centre.SetXYZ(-1 * (dx - centre_hole / 2), -dx - centre_hole / 2, distance_block_cible);
      else if (bb == 2)   centre.SetXYZ(-1 * (dx + centre_hole / 2), dx - centre_hole / 2, distance_block_cible);
      else if (bb == 3)   centre.SetXYZ(-1 * (-dx + centre_hole / 2), dx + centre_hole / 2, distance_block_cible);
      else if (bb == 0)   centre.SetXYZ(-1 * (-dx - centre_hole / 2), -dx + centre_hole / 2, distance_block_cible);
      else {
         Warning("BuildFAZIA", "Block position definition is done only for %d blocks", fNblocks);
      }
      theta = centre.Theta() * TMath::RadToDeg();
      phi = centre.Phi() * TMath::RadToDeg();
      printf("BLK #%d => theta=%1.2lf - phi=%1.2lf\n", bb, theta, phi);

      /*
            phi += 90;
            theta = 5.2;
            printf("BLK #%d => theta=%1.2lf - phi=%1.2lf\n", bb, theta, phi);
      */
      rot2.SetAngles(phi + 90., theta, 0.);
      rot1.SetAngles(-1.*phi, 0., 0.);
      h = rot2 * trans * rot1;
      ph = new TGeoHMatrix(h);
      top->AddNode(block, bb, ph);
   }

   gGeoManager->CloseGeometry();
   gGeoManager->DefaultColors();

}
