//Created by KVClassFactory on Wed Feb  1 11:09:41 2012
//Author: John Frankland,,,

#ifndef __INDRAGEOMETRYBUILDER_H
#define __INDRAGEOMETRYBUILDER_H

#include "KVBase.h"
#include "TVector3.h"
#include "KVMaterial.h"
#include "TGeoVolume.h"
#include "KVNumberList.h"
#include "KVNameValueList.h"

class TGeoManager;

class INDRAGeometryBuilder : public KVBase {
   TString fDetName;//!name of detector
   TVector3 fInnerFront[4];//!coords of inner front face
   TVector3 fOuterFront[4];//!coords of outer front face
   TVector3 fFrameFront[4];//!coords of outer front face
   KVList* fLayers;//!list of materials making up layers of current detector
   Int_t fActiveLayer;//!index of active layer of current detector
   KVMaterial fFrameMat;//!material of outer frame
   Double_t fTotalThickness;//!sum of thicknesses of layers of current detector
   TVector3 fInnerCentre;//!centre of inner face
   TVector3 fOuterCentre;//!centre of outer face
   TVector3 fFrameCentre;//!centre of frame
   Int_t Ndets;//!number of detectors in ring
   Double_t thetaMin, thetaMax, deltaPhi, phi0; //!theoretical geometry
   TGeoVolume* fFrameVolume;//!geo volume representing frame
   TGeoVolume* fDetVolume;//!geo volume representing frame
   Int_t fInnerPads, fOuterPads;
   Int_t fInnerRing, fOuterRing;
   Int_t fInnerDmod, fOuterDmod;
   Int_t fInnerModmin, fOuterModmin;
   Int_t fModmax;
   Double_t fRingCentreDistance;

public:
   INDRAGeometryBuilder();
   INDRAGeometryBuilder(const INDRAGeometryBuilder&) ;
   virtual ~INDRAGeometryBuilder();
   void Copy(TObject&) const;

   void ReadDetCAO(const Char_t* detname, Int_t ring);
   void CalculateBackPlaneCoordinates(TVector3* frontcoords, TVector3 centre, Double_t depth, TVector3* backcoords);
   void CalculateCentre(TVector3* corners, TVector3& centre);
   void CalculateCornersInPlane(TVector3* plane, Double_t thetamin,
                                Double_t thetamax, Double_t phimin, Double_t phimax, TVector3* corners);

   void Print(Option_t* = "") const;

   void MakeFrame();
   void MakeDetector(const Char_t* det, int ring, int mod, TVector3* som, TVector3 cen);
   void PlaceFrame(Double_t phi);
   void PlaceDetector();
   void MakeRing(const Char_t* det, int ring);
   void ReflectPad(TVector3* orig, Double_t phicentre, TVector3* newpad);
   void CloseAndDraw();

   TGeoManager* Build(Bool_t withTarget=kTRUE);
   void BuildTarget();
   void Build(KVNumberList& rings, KVNameValueList& detectors);
   void TransformToOwnFrame(TVector3* orig, TVector3& centre, TVector3* ownframe);

   ClassDef(INDRAGeometryBuilder, 0) //Build INDRA geometry from Huguet CAO infos
};

#endif
