//Created by KVClassFactory on Fri Feb 26 14:22:01 2016
//Author: bonnet,,,

#include "KVFAZIETO.h"
#include "KVUnits.h"
#include "KVFAZIABlock.h"

#include <KVDataSet.h>

ClassImp(KVFAZIETO)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIETO</h2>
<h4>description of the FAZIA-12B demonstrator</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVFAZIETO::KVFAZIETO()
   : KVFAZIA()
{
   // Default constructor
   SetTitle(ClassName());
}

KVFAZIETO::~KVFAZIETO()
{
   // Destructor
}

void KVFAZIETO::GetGeometryParameters()
{
   //defined configuration of the blocks
   fNblocks = KVBase::GetDataSetEnv(fDataSet, "FAZIETO.NBlocks", 12.0);
   fFDist = KVBase::GetDataSetEnv(fDataSet, "FAZIETO.DistanceZ", 100.0);
   fFThetaMin = KVBase::GetDataSetEnv(fDataSet, "FAZIETO.ThetaMinCompactGeo", 1.4);
   // if distance FAZIA-target is not the nominal distance of 100cm, telescopes
   // will not be aligned with the target position at (0,0,0).
   // Therefore we use the theoretical origin for FAZIA in its real position,
   // which is (0,0,fFDist-100.0)
   SetGeometryImportParameters(.25, 1, 1, 0, 13, 360,
                               KVBase::GetDataSetEnv(fDataSet, "FAZIETO.DistanceX", 0.0),
                               KVBase::GetDataSetEnv(fDataSet, "FAZIETO.DistanceY", 0.0),
                               fFDist - 100.0);
}


//________________________________________________________________

void KVFAZIETO::BuildFAZIA()
{
   // Build the FAZIA demonstrator geometry.
   //
   // Distance from target and minimum polar angle between innermost blocks
   // can be changed for different datasets using:
   //
   // [dataset].FAZIETO.DistanceZ:  95.4
   // [dataset].FAZIETO.DistanceX:  1.
   // [dataset].FAZIETO.DistanceY:  0.
   // [dataset].FAZIETO.ThetaMinCompactGeo: 2.3
   // [dataset].FAZIETO.InterBlockPadding: 2.3
   //
   // Number of blocks in demonstrator, their numbers, and positions can be modified for
   // different datasets using the following environment variables:
   //
   // [dataset].FAZIETO.NBlocks:  3
   // [dataset].FAZIETO.BlockNumbers: 0,1,4
   // [dataset].FAZIETO.BlockPositions: 11,9,5
   //
   // This will build a 3 block demonstrator with blocks numbered 0, 1, and 4
   // which will be placed at the given block positions in the standard
   // 12-block geometry (where blocks are numbered clockwise starting from
   // the innermost block placed at roughly 12 o'clock)

   Info("BuildFAZIA", "Compact geometry, %f cm from target, theta-min=%f deg.",
        fFDist, fFThetaMin);

   TGeoVolume* top = gGeoManager->GetTopVolume();

   Double_t distance_block_cible = fFDist * KVUnits::cm;
   Double_t nominal_distance_block_cible = 100.0 * KVUnits::cm;
   Double_t thick_si1 = 300 * KVUnits::um;
   TGeoTranslation trans;
   trans.SetDz(nominal_distance_block_cible + thick_si1 / 2.);

   KVFAZIABlock* block = new KVFAZIABlock;

   TGeoRotation rot1, rot2;
   TGeoTranslation final_trans;
   final_trans.SetDz(distance_block_cible - nominal_distance_block_cible);
   if (KVBase::GetDataSetEnv(fDataSet, "FAZIETO.DistanceX", 0.0) != 0.) final_trans.SetDx(KVBase::GetDataSetEnv(fDataSet, "FAZIETO.DistanceX", 0.0));
   if (KVBase::GetDataSetEnv(fDataSet, "FAZIETO.DistanceY", 0.0) != 0.) final_trans.SetDy(KVBase::GetDataSetEnv(fDataSet, "FAZIETO.DistanceY", 0.0));
   TGeoHMatrix h;
   TGeoHMatrix* ph = 0;
   Double_t theta = 0;
   Double_t phi = 0;

   Double_t theta_min = fFThetaMin;//smallest lab polar angle in degrees
   Double_t centre_hole = 2.*tan(theta_min * TMath::DegToRad()) * nominal_distance_block_cible;
   Double_t dx = (block->GetTotalSideWithBlindage() + KVBase::GetDataSetEnv(fDataSet, "FAZIETO.InterBlockPadding", 0.0)) / 2.;

   TVector3 centre;
   // block numbering and positions
   KVString block_numbers = KVBase::GetDataSetEnv(fDataSet, "FAZIETO.BlockNumbers", "0,1,2,3,4,5,6,7,8,9,10,11");
   block_numbers.Begin(",");
   KVString block_positions = KVBase::GetDataSetEnv(fDataSet, "FAZIETO.BlockPositions", "0,1,2,3,4,5,6,7,8,9,10,11");
   block_positions.Begin(",");

   for (Int_t i = 0; i < fNblocks; i += 1) {

      Int_t block_number = block_numbers.Next().Atoi();
      Int_t bb = block_positions.Next().Atoi();

      //for FAZIASYM ordering
      if (bb == 3)      centre.SetXYZ(-1 * (dx     - centre_hole / 2), 1 * (-dx    - centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 2) centre.SetXYZ(-1 * (dx     + centre_hole / 2), 1 * (dx     - centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 1) centre.SetXYZ(-1 * (-dx    + centre_hole / 2), 1 * (dx     + centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 0) centre.SetXYZ(-1 * (-dx    - centre_hole / 2), 1 * (-dx    + centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 10) centre.SetXYZ(-1 * (3 * dx   - centre_hole / 2),  1 * (-dx   - centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 11) centre.SetXYZ(-1 * (dx     - centre_hole / 2),  1 * (-3 * dx - centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 9) centre.SetXYZ(-1 * (3 * dx   + centre_hole / 2),  1 * (dx    - centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 8) centre.SetXYZ(-1 * (dx     + centre_hole / 2),  1 * (3 * dx  - centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 7) centre.SetXYZ(-1 * (-dx    + centre_hole / 2),  1 * (3 * dx  + centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 6) centre.SetXYZ(-1 * (-3 * dx  + centre_hole / 2),  1 * (dx    + centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 5)centre.SetXYZ(-1 * (-3 * dx  - centre_hole / 2), 1 * (-dx    + centre_hole / 2),  nominal_distance_block_cible);
      else if (bb == 4)centre.SetXYZ(-1 * (-dx    - centre_hole / 2),  1 * (-3 * dx + centre_hole / 2),  nominal_distance_block_cible);


      else {
         Warning("BuildFAZIA", "Block position definition is done only for %d blocks", fNblocks);
      }
      theta = centre.Theta() * TMath::RadToDeg();
      phi = centre.Phi() * TMath::RadToDeg();
      printf("BLK #%d => (theoretical) theta=%1.2lf - phi=%1.2lf\n", block_number, theta, phi);
      TVector3 offset(KVBase::GetDataSetEnv(fDataSet, "FAZIETO.DistanceX", 0.0),
                      KVBase::GetDataSetEnv(fDataSet, "FAZIETO.DistanceY", 0.0),
                      distance_block_cible - nominal_distance_block_cible);
      TVector3 real_centre = offset + centre;
      fBlocCentreTheta[block_number] = real_centre.Theta() * TMath::RadToDeg();
      fBlocCentrePhi[block_number] = real_centre.Phi() * TMath::RadToDeg();
      rot2.SetAngles(phi + 90., theta, 0.);
      rot1.SetAngles(-1.*phi, 0., 0.);
      if (!final_trans.IsIdentity())
         h = final_trans * rot2 * trans * rot1;
      else
         h = rot2 * trans * rot1;
      ph = new TGeoHMatrix(h);
      top->AddNode(block, block_number, ph);
   }

}

