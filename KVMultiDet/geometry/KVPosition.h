/***************************************************************************
                          kvposition.h  -  description
                             -------------------
    begin                : Sun May 19 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVPosition.h,v 1.20 2009/01/19 14:36:57 ebonnet Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVPOSITION_H
#define KVPOSITION_H
#include "TMath.h"
#include "TVector3.h"
#include "KVBase.h"
#include "KVParticle.h"

class KVPosition {
 private:
   Float_t fTheta;              // polar angle in degrees with respect to beam axis, corresponds to centre of telescope
   Float_t fPhi;                // azimuthal angle in degrees with respect to 12 o'clock (=0 deg.), corresponds to centre of telescope 
   Float_t fTheta_min;          // polar angle in degrees corresponding to edge of telescope closest to beam axis 
   Float_t fTheta_max;          // polar angle in degrees of the edge furthest from the beam axis 
   Float_t fPhi_min;            // azimuthal angle in degrees corresponding to most anticlockwise edge of telescope 
   Float_t fPhi_max;            // azimuthal angle in degrees corresponding to most clockwise edge of telescope
   Float_t fDistance;           // distance in mm from centre of solid angle element to coordinate system origin (target)

 public:
    KVPosition();
    KVPosition(Float_t thmin, Float_t thmax, Float_t phmin, Float_t phmax,
               Float_t dist = 0.0);
   void init();
    virtual ~ KVPosition() {
   };
   void SetAzimuthalAngle(Float_t ph);
   void SetPolarAngle(Float_t th);
   void SetPolarWidth(Float_t pw);
   void SetPolarMinMax(Float_t min, Float_t max);
   void SetAzimuthalWidth(Float_t aw);
   void SetAzimuthalMinMax(Float_t min, Float_t max);
   void SetPhiMinMax(Float_t min, Float_t max) {
      SetAzimuthalMinMax(min, max);
   };
   TVector3 GetRandomDirection(Option_t * t = "isotropic");
   TRotation GetRandomIsotropicRotation();
	void GetRandomAngles(Double_t &th, Double_t &ph, Option_t * t = "isotropic");
   TVector3 GetDirection();

   Bool_t IsInPolarRange(const Float_t theta);
   Bool_t IsInPhiRange(const Float_t phi);
   Bool_t IsSmallerThan(KVPosition * pos);
   Bool_t IsAlignedWith(KVPosition * pos);
   Bool_t IsOverlappingWith(KVPosition * pos);
   Bool_t IsAzimuthallyWiderThan(KVPosition * pos);

   inline Float_t GetPhiMin() const {
      return fPhi_min;
   };
   inline Float_t GetPhiMax() const {
      return fPhi_max;
   };
   inline Float_t GetThetaMin() const {
      return fTheta_min;
   };
   inline Float_t GetThetaMax() const {
      return fTheta_max;
   };
   Float_t GetAzimuthalWidth(Float_t phmin = -1., Float_t phimax =
                             -1.) const;
   Float_t GetTheta() const {
      return fTheta;
   };
   Float_t GetSinTheta() const {
      return TMath::Sin(fTheta * TMath::DegToRad());
   };
   Float_t GetCosTheta() const {
      return TMath::Cos(fTheta * TMath::DegToRad());
   };
   Float_t GetPhi() {
      if (fPhi >= 360.0)
         fPhi -= 360.;
      return fPhi;
   };
   void SetTheta(Float_t t) {
      SetPolarAngle(t);
   };
   void SetPhi(Float_t p) {
      SetAzimuthalAngle(p);
   };
   void SetDistance(Float_t d) {
      fDistance = d;
   };
   Float_t GetDistance(void) const {
      return fDistance;
   };
   void GetCornerCoordinates(TVector3 *, Double_t /*depth*/=0);
	void GetCornerCoordinatesInOwnFrame(TVector3 *, Double_t /*depth*/=0);
   void GetWidthsFromDimension(Float_t lin_dim);
	
	virtual Double_t GetSolidAngle(void);
	
   ClassDef(KVPosition, 1)      //Class for positioning elements of multidetector array
};

#endif
