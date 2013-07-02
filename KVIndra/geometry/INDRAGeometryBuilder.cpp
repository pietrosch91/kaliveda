//Created by KVClassFactory on Wed Feb  1 11:09:41 2012
//Author: John Frankland,,,

#include "INDRAGeometryBuilder.h"
#include "TEnv.h"
#include "TMath.h"
#include "TString.h"
#include "TGeoMedium.h"
#include "TRotation.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TGeoCompositeShape.h"
#include "TGeoEltu.h"

#include "KVINDRA.h"
#include "KVDataSet.h"

using namespace std;

ClassImp(INDRAGeometryBuilder)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>INDRAGeometryBuilder</h2>
<h4>Build INDRA geometry from Huguet CAO infos</h4>
<!-- */
// --> END_HTML
// N.B. - the Ring 1 Si-CsI detectors (>=INDRA_camp4) 
//        are described using the same geometry as for the
//        1st campaign phoswich detectors (no CAO data available)
////////////////////////////////////////////////////////////////////////////////

TGeoTranslation* INDRAGeometryBuilder::CorrectCoordinates(Double_t *coo)
{
    // calculate offset in X and Y
    // correct coordinates for offset
    // return offset TGeoTranslation
    Double_t offX,offY;
    offX=offY=0;
    for(int i=0;i<8;i++){
        offX+=coo[2*i];
        offY+=coo[2*i+1];
    }
    offX/=8.;offY/=8.;
    for(int i=0;i<8;i++){
        coo[2*i]-=offX;
        coo[2*i+1]-=offY;
    }
    return new TGeoTranslation(offX,offY,0);
}

INDRAGeometryBuilder::INDRAGeometryBuilder()
{
   // Default constructor
}

//________________________________________________________________

INDRAGeometryBuilder::INDRAGeometryBuilder(const INDRAGeometryBuilder& obj)  : KVBase()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

INDRAGeometryBuilder::~INDRAGeometryBuilder()
{
   // Destructor
}

//________________________________________________________________

void INDRAGeometryBuilder::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable INDRAGeometryBuilder::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   TObject::Copy(obj);
   //INDRAGeometryBuilder& CastedObj = (INDRAGeometryBuilder&)obj;
}

void INDRAGeometryBuilder::CalculateBackPlaneCoordinates(TVector3* frontcoords, TVector3 normal, Double_t depth, TVector3* backcoords)
{
   // Given an array of ncoords points frontcoords[ncoords] which lie in a plane
   // calculate the corresponding points on a parallel plane which is
   // at a distance from the origin which is greater by depth
   // (distance plane-origin = length of normal to plane passing through origin)
   // 'normal' is vector point of intersection plane-normal to origin

   Double_t planeDist = normal.Mag();//distance original plane-origin
   for (int i = 0; i < 4; i++) {
      Double_t rap = (planeDist + depth) / planeDist;
      backcoords[i] = rap * frontcoords[i];
   }
}
//________________________________________________________________

void INDRAGeometryBuilder::ReadDetCAO(const Char_t* detname, Int_t ring)
{
   // Read infos in CAO file for this detector.
   // We look for a TEnv file with name "detname.cao"

   TEnv infos;
   TString path = Form("%s.cao", detname);
   SearchKVFile(path.Data(),path,"data");
   infos.ReadFile(path,kEnvAll);

   Double_t unit_converter = 0.1;// distances in file given in mm: convert to cm

   fInnerFront[0].SetX(infos.GetValue("INNER.A.X", 0.0)*unit_converter);
   fInnerFront[0].SetY(infos.GetValue("INNER.A.Y", 0.0)*unit_converter);
   fInnerFront[0].SetZ(infos.GetValue("INNER.A.Z", 0.0)*unit_converter);
   fInnerFront[3].SetX(infos.GetValue("INNER.B.X", 0.0)*unit_converter);
   fInnerFront[3].SetY(infos.GetValue("INNER.B.Y", 0.0)*unit_converter);
   fInnerFront[3].SetZ(infos.GetValue("INNER.B.Z", 0.0)*unit_converter);
   fInnerFront[2].SetX(infos.GetValue("INNER.C.X", 0.0)*unit_converter);
   fInnerFront[2].SetY(infos.GetValue("INNER.C.Y", 0.0)*unit_converter);
   fInnerFront[2].SetZ(infos.GetValue("INNER.C.Z", 0.0)*unit_converter);
   fInnerFront[1].SetX(infos.GetValue("INNER.D.X", 0.0)*unit_converter);
   fInnerFront[1].SetY(infos.GetValue("INNER.D.Y", 0.0)*unit_converter);
   fInnerFront[1].SetZ(infos.GetValue("INNER.D.Z", 0.0)*unit_converter);

   // To get back to INDRA phi-coordinates, all points need to be rotated
   // by -150 degrees around the Z-axis
   for (int i = 0; i <= 3; i++) {
      fInnerFront[i].RotateZ(-150.*TMath::DegToRad());
   }
   CalculateCentre(fInnerFront, fInnerCentre);

   fOuterFront[0].SetX(infos.GetValue("OUTER.A.X", 0.0)*unit_converter);
   fOuterFront[0].SetY(infos.GetValue("OUTER.A.Y", 0.0)*unit_converter);
   fOuterFront[0].SetZ(infos.GetValue("OUTER.A.Z", 0.0)*unit_converter);
   fOuterFront[3].SetX(infos.GetValue("OUTER.B.X", 0.0)*unit_converter);
   fOuterFront[3].SetY(infos.GetValue("OUTER.B.Y", 0.0)*unit_converter);
   fOuterFront[3].SetZ(infos.GetValue("OUTER.B.Z", 0.0)*unit_converter);
   fOuterFront[2].SetX(infos.GetValue("OUTER.C.X", 0.0)*unit_converter);
   fOuterFront[2].SetY(infos.GetValue("OUTER.C.Y", 0.0)*unit_converter);
   fOuterFront[2].SetZ(infos.GetValue("OUTER.C.Z", 0.0)*unit_converter);
   fOuterFront[1].SetX(infos.GetValue("OUTER.D.X", 0.0)*unit_converter);
   fOuterFront[1].SetY(infos.GetValue("OUTER.D.Y", 0.0)*unit_converter);
   fOuterFront[1].SetZ(infos.GetValue("OUTER.D.Z", 0.0)*unit_converter);

   // To get back to INDRA phi-coordinates, all points need to be rotated
   // by -150 degrees around the Z-axis
   for (int i = 0; i <= 3; i++) {
      fOuterFront[i].RotateZ(-150.*TMath::DegToRad());
   }
   CalculateCentre(fOuterFront, fOuterCentre);

   phi0 = infos.GetValue("PHIZERO", 30.0);
   thetaMin = infos.GetValue("THETAMIN", 0.0);
   thetaMax = infos.GetValue("THETAMAX", 0.0);
   Ndets = infos.GetValue("NDETS", 1);
   deltaPhi = 360. / Ndets;

   // calculate theoretical geometry:
   // points of intersection with the plane defined by real Huguet geometry
   // and the 4 vectors to theoretical thetamin/max phimin/max
   Double_t Ph0 = fInnerCentre.Phi() * TMath::RadToDeg();
   CalculateCornersInPlane(fInnerFront, thetaMin, thetaMax,
                           Ph0 - deltaPhi / 2.,
                           Ph0 + deltaPhi / 2.,
                           fFrameFront);
   // calculate centre of frame = direction of detector
   CalculateCentre(fFrameFront, fFrameCentre);
   fFrameMat.SetMaterial(infos.GetValue("FRAME.MATERIAL", "Al"));

   fInnerPads = infos.GetValue("INNER.PADS", 1);
   fOuterPads = infos.GetValue("OUTER.PADS", 0);
   fInnerRing = infos.GetValue("INNER.RING", ring);
   fOuterRing = infos.GetValue("OUTER.RING", 0);
   fInnerDmod = infos.GetValue("INNER.DMOD", 1);
   fOuterDmod = infos.GetValue("OUTER.DMOD", 1);
   fInnerModmin = infos.GetValue("INNER.MODMIN", 1);
   fOuterModmin = infos.GetValue("OUTER.MODMIN", 1);
   fModmax = infos.GetValue("MODMAX", 24);
}

//________________________________________________________________

void INDRAGeometryBuilder::Print(Option_t*) const
{
   printf("Detector name : %s\n", fDetName.Data());
   printf("Total thickness : %f cm\n", fTotalThickness);
   printf("Frame centre:\n");
   fFrameCentre.Print();
   printf("Inner centre:\n");
   fInnerCentre.Print();
   printf("Outer centre: \n");
   fOuterCentre.Print();
   printf("Coordinates of outer pads :\n");
   for (int i = 0; i < 4; i++) fOuterFront[i].Print();
   printf("Coordinates of inner pads :\n");
   for (int i = 0; i < 4; i++) fInnerFront[i].Print();
   printf("Material of outer frame : %s\n", fFrameMat.GetTitle());
}

//________________________________________________________________

void INDRAGeometryBuilder::MakeFrame()
{
   // Create the TGeoVolume corresponding to the outer casing
   // (dead zone) of the detector

   TVector3 fFrameBack[4];
   TVector3 fFrameBackCentre;
   CalculateBackPlaneCoordinates(fFrameFront, fFrameCentre, fTotalThickness, fFrameBack);
   CalculateCentre(fFrameBack, fFrameBackCentre);

   TVector3 corners[8]; // 8 vertices of the volume
   TransformToOwnFrame(fFrameFront, fFrameCentre, corners);
   TransformToOwnFrame(fFrameBack, fFrameBackCentre, &corners[4]);

   Double_t vertices[16];
   for (register int i = 0; i < 8; i++) {
      vertices[2 * i] = corners[i].X();
      vertices[2 * i + 1] = corners[i].Y();
   }

   Double_t dz = 0.99*fTotalThickness / 2.;
   TString vol_name;
   vol_name.Form("%s_FRAME", fDetName.Data());
   fFrameVolume = gGeoManager->MakeVolumeAssembly(vol_name);
   vol_name.Form("DEADZONE_%s", fDetName.Data());
   TGeoMedium* med = fFrameMat.GetGeoMedium();
   TGeoTranslation* offset = CorrectCoordinates(vertices);
   TGeoVolume* frame = gGeoManager->MakeArb8(vol_name.Data(), med, dz, vertices);
   frame->SetLineColor(med->GetMaterial()->GetDefaultColor());
   fFrameVolume->AddNode(frame,1,offset);
//   fFrameVolume->SetVisContainers();
}

//________________________________________________________________

void INDRAGeometryBuilder::TransformToOwnFrame(TVector3* orig, TVector3& centre, TVector3* ownframe)
{
   // takes the four points (TVector3) in the array and transforms them
   // to a reference frame in which the +ve z-axis goes through their centre

   TRotation rot_to_frame;
   rot_to_frame.SetYEulerAngles(-centre.Phi(), -centre.Theta(), 0.);
   TVector3 displZ(0, 0, centre.Mag());
   for (register int i = 0; i < 4; i++) {
      ownframe[i] = rot_to_frame * orig[i] - displZ;
   }
}


//________________________________________________________________

void INDRAGeometryBuilder::PlaceFrame(Double_t phi)
{
   // position frame (dead zone) volume in geometry

   Double_t theta = fFrameCentre.Theta() * TMath::RadToDeg();
   TGeoRotation rot1, rot2;
   rot2.SetAngles(phi + 90., theta, 0.);
   rot1.SetAngles(-90., 0., 0.);
   // distance to frame centre
   Double_t trans_z = fFrameCentre.Mag() + fTotalThickness / 2.;

   TGeoTranslation tran(0, 0, trans_z);
   TGeoHMatrix h = rot2 * tran * rot1;
   TGeoHMatrix *ph = new TGeoHMatrix(h);

   // add detector volume to geometry
   gGeoManager->GetTopVolume()->AddNode(fFrameVolume, 1, ph);
}

//________________________________________________________________

void INDRAGeometryBuilder::PlaceDetector()
{
   // position detector inside frame

   fFrameVolume->AddNode(fDetVolume, 1, fDetectorPosition);
   fFrameVolume->GetNode(Form("DET_%s_1", fDetName.Data()))->SetName(Form("DET_%s", fDetName.Data()));
}
//________________________________________________________________

//void INDRAGeometryBuilder::MakeRing(TGeoVolume* MOTHER, const Char_t* det, int ring)
void INDRAGeometryBuilder::MakeRing(const Char_t* det, int ring)
{
   if (!strcmp(det, "PHOS"))
      fDetName.Form("%s_01", det);
   else
      fDetName.Form("%s_%02d01", det, ring);
   ReadDetCAO(fDetName, ring);
   //Print();
   Double_t phi = phi0;
   Int_t innerMod = fInnerModmin;
   Int_t outerMod = fOuterModmin;
   int i = 1;
   int ModStep = fModmax / Ndets;

   cout << fDetName.Data()  << "  " << fInnerCentre.Mag() << "  " <<
        0.5 * (fInnerFront[0].Theta() + fInnerFront[3].Theta())*TMath::RadToDeg() << "  " <<
        0.5 * (fInnerFront[1].Theta() + fInnerFront[2].Theta())*TMath::RadToDeg() << "  " <<
        0.5 * (fInnerFront[2].Phi() + fInnerFront[3].Phi())*TMath::RadToDeg() << "  " <<
        0.5 * (fInnerFront[0].Phi() + fInnerFront[1].Phi())*TMath::RadToDeg() << endl;

   for (; i <= Ndets; i++) {

      if (!strcmp(det, "PHOS"))
         fDetName.Form("%s_%02d", det, ModStep * (i - 1) + 1);
      else
         fDetName.Form("%s_%02d%02d", det, ring, ModStep * (i - 1) + 1);

      // check detector is present
      KVDetector* thisdetector = gIndra->GetDetector(fDetName.Data());
      bool skipDetector = kFALSE;
      if (!thisdetector || !thisdetector->IsPresent()) {
         Info("MakeRing", "%s is absent", fDetName.Data());
         skipDetector = kTRUE;
      }

      if (!skipDetector) {
         fLayers = thisdetector->GetListOfAbsorbers();
         fActiveLayer = 1 + thisdetector->GetListOfAbsorbers()->IndexOf(thisdetector->GetActiveLayer());
         fTotalThickness = thisdetector->GetTotalThicknessInCM();
      }
      /* build and add frame */
      if (!skipDetector) MakeFrame();
      // fRingCentreDistance = fFrameCentre.
      // make pads in inner ring

      if (!skipDetector) {
         MakeDetector(det, fInnerRing, innerMod, fInnerFront, fInnerCentre);
         PlaceDetector();
      }
      innerMod += fInnerDmod;

      if (fInnerPads == 3) {
         if (!skipDetector) {
            MakeDetector(det, fInnerRing, innerMod, fOuterFront, fOuterCentre);
            PlaceDetector();
         }
         innerMod += fInnerDmod;
         if (!skipDetector) {
            TVector3 reflex[4];
            ReflectPad(fInnerFront, fFrameCentre.Phi(), reflex);
            TVector3 refcent;
            CalculateCentre(reflex, refcent);
            MakeDetector(det, fInnerRing, innerMod, reflex, refcent);
            PlaceDetector();
         }
         innerMod += fInnerDmod;
      } else {
         if (fInnerPads == 2) {
            if (!skipDetector) {
               TVector3 reflex[4];
               ReflectPad(fInnerFront, fFrameCentre.Phi(), reflex);
               TVector3 refcent;
               CalculateCentre(reflex, refcent);
               MakeDetector(det, fInnerRing, innerMod, reflex, refcent);
               PlaceDetector();
            }
            innerMod += fInnerDmod;
         }

         if (fOuterPads) {
            // make pads in outer ring
            if (!skipDetector) {
               MakeDetector(det, fOuterRing, outerMod, fOuterFront, fOuterCentre);
               PlaceDetector();
            }
            outerMod += fOuterDmod;

            if (fOuterPads == 2) {
               if (!skipDetector) {
                  TVector3 reflex[4];
                  ReflectPad(fOuterFront, fFrameCentre.Phi(), reflex);
                  TVector3 refcent;
                  CalculateCentre(reflex, refcent);
                  MakeDetector(det, fOuterRing, outerMod, reflex, refcent);
                  PlaceDetector();
               }
               outerMod += fOuterDmod;
            }
         }
      }
      if (!skipDetector) PlaceFrame(phi);
      phi += deltaPhi;
   }

}

//________________________________________________________________

void INDRAGeometryBuilder::MakeDetector(const Char_t* det, int ring, int mod, TVector3* som, TVector3 cen)
{

   // make volume corresponding to the actual detector

   if (!strcmp(det, "PHOS"))
      fDetName.Form("%s_%02d", det, mod);
   else
      fDetName.Form("%s_%02d%02d", det, ring, mod);

   KVDetector* detWeMake = gMultiDetArray->GetDetector(fDetName);
   
   TVector3 corners[8]; // 8 vertices of the volume
   Double_t vertices[16];

   Bool_t multi_layer = fLayers->GetSize() > 1;

   if (multi_layer) {
      fDetVolume = gGeoManager->MakeVolumeAssembly(Form("DET_%s", fDetName.Data()));
   }
   TVector3 frontPlane[4], backPlane[4], frontCentre, backCentre;
   // front plane of first absorber is front plane of detector
   for (int i = 0; i < 4; i++) frontPlane[i] = som[i];
   frontCentre = cen;



   /**** BUILD & ADD LAYERS ****/
   TIter next(fLayers);
   KVMaterial *abs;
   Double_t depth_in_det = 0.;
   Int_t no_abs = 1;

   while ((abs = (KVMaterial*)next())) {
      // get medium for absorber
      TGeoMedium* med = abs->GetGeoMedium();
      Double_t thick = abs->GetThickness();
      
      if(thick==0.0) {no_abs++; continue;} // ignore zero thickness layers

      // calculate coordinates of back plane
      CalculateBackPlaneCoordinates(frontPlane, frontCentre, thick, backPlane);
      // calculate centre of back plane
      CalculateCentre(backPlane, backCentre);
      // calculate corners of volume
      TransformToOwnFrame(frontPlane, fFrameCentre, corners);
      TransformToOwnFrame(backPlane, fFrameCentre, &corners[4]);
      for (register int i = 0; i < 8; i++) {
         vertices[2 * i] = corners[i].X();
         vertices[2 * i + 1] = corners[i].Y();
      }

      Double_t dz = thick / 2.;
      TString vol_name;
      if(multi_layer){
        if (no_abs == fActiveLayer) vol_name = Form("ACTIVE_%s_%d_%s",fDetName.Data(), no_abs, abs->GetType());
        else vol_name = Form("%s_%d_%s", fDetName.Data(), no_abs, abs->GetType());
      }
      else
          vol_name = Form("DET_%s", fDetName.Data());
      fDetectorPosition = CorrectCoordinates(vertices);
      TGeoVolume *vol =
         gGeoManager->MakeArb8(vol_name.Data(), med, dz, vertices);
      vol->SetLineColor(med->GetMaterial()->GetDefaultColor());
      if (multi_layer) {
         /*** position absorber in mother ***/
         Double_t trans_z = -fTotalThickness / 2. + depth_in_det + dz; // (note: reference is CENTRE of absorber)
         TGeoTranslation *tr = new TGeoTranslation(0., 0., trans_z);
         fDetVolume->AddNode(vol, 1, tr);
      } else {
         // single absorber: mother is absorber is detector is mother is ...
         fDetVolume = vol;
      }
      // set reference to volume in absorber
      //detWeMake->GetAbsorber(no_abs-1)->SetAbsGeoVolume(vol);
      
      depth_in_det += thick;
      no_abs++;
      
      // front of next absorber is back of current absorber
      for (int i = 0; i < 4; i++) frontPlane[i] = backPlane[i];
      frontCentre = backCentre;
   }
}

void INDRAGeometryBuilder::CalculateCentre(TVector3* corners, TVector3& inter)
{
   // TVector3 corners[4] array of corner coordinates in usual order:
   //       corners[3] : theta-min, phi-min
   //       corners[2] : theta-max, phi-min
   //       corners[1] : theta-max, phi-max
   //       corners[0] : theta-min, phi-max
   //
   // Calculate the intersection of the normal to the plane passing through the origin.

   TVector3 a10 = corners[1] - corners[0];
   TVector3 a21 = corners[2] - corners[1];
   TVector3 normal = a10.Cross(a21);
   Double_t d = (corners[3] * normal) / normal.Mag2();
   inter = normal * d;
}


void INDRAGeometryBuilder::CalculateCornersInPlane(TVector3* plane, Double_t thetamin,
                                                   Double_t thetamax, Double_t phimin, Double_t phimax, TVector3* corners)
{
   // fill TVector3 corners[4] array in usual order:
   //       corners[3] : theta-min, phi-min
   //       corners[2] : theta-max, phi-min
   //       corners[1] : theta-max, phi-max
   //       corners[0] : theta-min, phi-max
   // theta, phi in degrees
   // These points lie in the plane defined by the equivalent TVEctor3 plane[4] corners

   TVector3 a10 = plane[1] - plane[0];
   TVector3 a21 = plane[2] - plane[1];
   TVector3 normal = a10.Cross(a21);
   Double_t d0 = plane[3] * normal;

   TVector3 l;
   l.SetMagThetaPhi(1., thetamin * TMath::DegToRad(), phimin * TMath::DegToRad());
   corners[3] = l * (d0 / (l * normal));
   l.SetMagThetaPhi(1., thetamax * TMath::DegToRad(), phimin * TMath::DegToRad());
   corners[2] = l * (d0 / (l * normal));
   l.SetMagThetaPhi(1., thetamax * TMath::DegToRad(), phimax * TMath::DegToRad());
   corners[1] = l * (d0 / (l * normal));
   l.SetMagThetaPhi(1., thetamin * TMath::DegToRad(), phimax * TMath::DegToRad());
   corners[0] = l * (d0 / (l * normal));
}

void INDRAGeometryBuilder::CloseAndDraw()
{
   gGeoManager->CloseGeometry();
   //gGeoManager->GetTopVolume()->SetVisContainers();
   gGeoManager->DefaultColors();
   gGeoManager->GetTopVolume()->Draw("ogl");
}

void INDRAGeometryBuilder::ReflectPad(TVector3* orig, Double_t phicentre, TVector3* newpad)
{
   for (int i = 0; i < 4; i++) {
      newpad[3 - i].SetMagThetaPhi(orig[i].Mag(), orig[i].Theta(), 2.*phicentre - orig[i].Phi());
   }
}

void INDRAGeometryBuilder::BuildTarget()
{
   //TO DO: multi-layer targets + target with angle to beam

   // target holder
   KVMaterial target_holder_mat("Al");
   new TGeoBBox("TARGET_FRAME", 3., 3., 0.1 / 2.);
   new TGeoEltu("TARGET_HOLE", 2., 2., 0.1 / 2.);
   TGeoCompositeShape* cs = new TGeoCompositeShape("TARGET_FRAME", "TARGET_FRAME - TARGET_HOLE");
   TGeoVolume* target_frame = new TGeoVolume("TARGET_FRAME", cs, target_holder_mat.GetGeoMedium());
   gGeoManager->GetTopVolume()->AddNode(target_frame, 1);

   // add target
   // only single-layer target treated correctly
   KVTarget* T = gIndra->GetTarget();
   if (T) {
      KVMaterial* targMat = (KVMaterial*)T->GetLayers()->First();
      TGeoVolume* target = gGeoManager->MakeEltu("TARGET", targMat->GetGeoMedium(), 2., 2., targMat->GetThickness() / 2.);
      gGeoManager->GetTopVolume()->AddNode(target, 1);
   }
}

TGeoManager* INDRAGeometryBuilder::Build(Bool_t withTarget, Bool_t closeGeometry)
{
   if (!gIndra) {
      Error("Build", "You must build the geometry with gDataSet->BuildMultiDetector() before calling this method");
      return 0x0;
   }
   if (gGeoManager) delete gGeoManager;

   TGeoManager *geom = new TGeoManager("INDRA", Form("INDRA geometry for dataset %s", gDataSet->GetName()));
   TGeoMaterial*matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
   matVacuum->SetTitle("Vacuum");
   TGeoMedium*Vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
   TGeoVolume *top = geom->MakeBox("WORLD", Vacuum,  500, 500, 500);
   geom->SetTopVolume(top);

   if(gIndra->GetDetector("PHOS_01")) MakeRing("PHOS", 1);
   else {
      MakeRing("SI",1);
      MakeRing("CSI",1);
   }
   for (int ring = 2; ring <= 16; ring += 2) {
      MakeRing("CI", ring);
      if (ring > 1 && ring < 10)MakeRing("SI", ring);
      MakeRing("CSI", ring);
      if (ring == 12) {
         MakeRing("CI", ring + 1);
         MakeRing("CSI", ring + 1);
      }
   }
   for(int ring = 10; ring <= 17; ring++){
      MakeEtalon(ring);
   }
   if(withTarget) BuildTarget();
   if(closeGeometry) gGeoManager->CloseGeometry();
   gGeoManager->DefaultColors();
   return gGeoManager;
}
void INDRAGeometryBuilder::Build(KVNumberList& rings, KVNameValueList& detectors)
{
   // Build geometry, using only the ring numbers in the KVNumberList, and
   // only the detectors which are in the KVNameValueList:
   //
   // KVNameValueList dets;
   // dets.SetValue("CI",1);
   // dets.SetValue("SI",1);
   // igb.Build("2-9", dets); // use INDRAGeometryBuilder object 'igb' to build
   //                         // ChIo and Silicon detectors of rings 2-9
   //
   // Possible detector types are "CI", "SI", "CSI", "PHOS"
   // If you want to see the target in the previous example, use dets.SetValue("TARGET",1)
    //
    // Call with ring+100 to build etalon telescopes

   if (!gIndra) {
      Error("Build", "You must build the geometry with gDataSet->BuildMultiDetector() before calling this method");
      return;
   }
   if (gGeoManager) delete gGeoManager;

   TGeoManager *geom = new TGeoManager("INDRA", Form("INDRA geometry for dataset %s", gDataSet->GetName()));
   TGeoMaterial*matVacuum = new TGeoMaterial("Vacuum", 0, 0, 0);
   matVacuum->SetTitle("Vacuum");
   TGeoMedium*Vacuum = new TGeoMedium("Vacuum", 1, matVacuum);
   TGeoVolume *top = geom->MakeBox("WORLD", Vacuum,  500, 500, 500);
   geom->SetTopVolume(top);

   if (rings.Contains(1)){
      if(detectors.HasParameter("PHOS")) MakeRing("PHOS", 1);
      if(detectors.HasParameter("SI"))MakeRing("SI", 1);
      if(detectors.HasParameter("CSI"))MakeRing("CSI", 1);
   }
   for (int ring = 2; ring <= 16; ring += 2) {
      if (rings.Contains(ring) && detectors.HasParameter("CI")) MakeRing("CI", ring);
      if (ring > 1 && ring < 10) {
         if (rings.Contains(ring) && detectors.HasParameter("SI"))MakeRing("SI", ring);
      }
      if (rings.Contains(ring) && detectors.HasParameter("CSI"))MakeRing("CSI", ring);
      if (ring == 12) {
         if (rings.Contains(ring + 1) && detectors.HasParameter("CI"))MakeRing("CI", ring + 1);
         if (rings.Contains(ring + 1) && detectors.HasParameter("CSI"))MakeRing("CSI", ring + 1);
      }
   }
   for(int ring = 10; ring <= 17; ring++){
      if(rings.Contains(ring+100)) MakeEtalon(ring);
   }
   if(rings.Contains(18))
   if (detectors.HasParameter("TARGET"))BuildTarget();
   CloseAndDraw();
}

void INDRAGeometryBuilder::MakeEtalon(int RING)
{
   // Build and add etalon telescope for ring
Double_t theta[] = {51.075000,63.340000,79.435000,100.685000,118.235000,133.905000,149.790000,166.435000};
Double_t phi[] = {37.500000,37.500000,37.500000,90.000000,78.750000,78.750000,90.000000,90.000000};
Double_t dist[] = {17.4005,17.4005,17.4005,16.7005,16.7005,16.7005,17.4005,17.4005,};
KVMaterial mat_si("Si");
KVMaterial mat_li("Li");
KVMaterial mat_al("Al");
TGeoMedium *Silicon = mat_si.GetGeoMedium();
TGeoMedium *Lithium = mat_li.GetGeoMedium();
TGeoMedium *Alu = mat_al.GetGeoMedium();

Double_t sili_diameter_total = 2.54;
Double_t holder_thickness = 0.05;
Double_t holder_length = 1.0;
Double_t sili_diameter_active = 2.36;
Double_t sili_silicon_thickness = 0.22;
Double_t sili_lithium_thickness = 0.0044;
Double_t si75_thickness = 0.008;
Double_t si75_diameter_active = 2.2;

TGeoVolumeAssembly* etalon = new TGeoVolumeAssembly("STRUCT_TELESCOPE");

TGeoVolume* holder = gGeoManager->MakeTube("DEADZONE_HOLDER", Alu, 
      (sili_diameter_total)/2., (sili_diameter_total+2*holder_thickness)/2., holder_length/2.);
holder->SetLineColor(holder->GetMaterial()->GetDefaultColor());
etalon->AddNode(holder, 1);

Double_t w = sili_silicon_thickness + sili_lithium_thickness;
TGeoVolume* sili_dz = gGeoManager->MakeTube("DEADZONE_SILI", Alu, 
      sili_diameter_active/2., (sili_diameter_total)/2., w/2.);
sili_dz->SetLineColor(sili_dz->GetMaterial()->GetDefaultColor());

etalon->AddNode(sili_dz, 1, new TGeoTranslation(0,0,holder_length/4.));

TGeoVolumeAssembly* sili = new TGeoVolumeAssembly("DET_SILI");      
TGeoVolume* sili_si = gGeoManager->MakeTube("ACTIVE_SILI", Silicon, 
      0., (sili_diameter_active)/2., (sili_silicon_thickness)/2.);
TGeoVolume* sili_li = gGeoManager->MakeTube("SILI_LI", Lithium, 
      0., (sili_diameter_active)/2., (sili_lithium_thickness)/2.);
sili_si->SetLineColor(sili_si->GetMaterial()->GetDefaultColor());
sili_li->SetLineColor(sili_li->GetMaterial()->GetDefaultColor());
sili->AddNode(sili_si, 1, new TGeoTranslation(0,0,-(w/2.-sili_silicon_thickness/2.)));
sili->AddNode(sili_li, 1, new TGeoTranslation(0,0,w/2.-sili_lithium_thickness/2.));
etalon->AddNode(sili, 1, new TGeoTranslation(0,0,holder_length/4.));
etalon->GetNode("DET_SILI_1")->SetName("DET_SILI");

TGeoVolume* si75 = gGeoManager->MakeTube("DET_SI75", Silicon, 
      0., (si75_diameter_active)/2., (si75_thickness)/2.);
TGeoVolume* si75_dz = gGeoManager->MakeTube("DEADZONE_SI75", Alu, 
      (si75_diameter_active)/2., (sili_diameter_total)/2., (si75_thickness)/2.);

etalon->AddNode(si75_dz, 1);
etalon->AddNode(si75, 1);
etalon->GetNode("DET_SI75_1")->SetName("DET_SI75");

TGeoTranslation trans;
trans.SetDz(dist[RING-10]);
TGeoRotation rot1,rot2;
TGeoHMatrix h;
TGeoHMatrix* ph=0;

   rot2.SetAngles(phi[RING-10]+90., theta[RING-10], 0.);
   rot1.SetAngles(-1.*phi[RING-10], 0., 0.);
   if(RING==13){
      TGeoTranslation p(4.5,0,0);
      h = p * rot2 * trans * rot1;
   }
   else
      h = rot2 * trans * rot1;
   ph = new TGeoHMatrix(h);
   gGeoManager->GetTopVolume()->AddNode(etalon,RING,ph);
}   
