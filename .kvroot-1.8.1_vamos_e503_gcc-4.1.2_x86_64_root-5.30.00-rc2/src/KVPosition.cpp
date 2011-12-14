/***************************************************************************
$Id: KVPosition.cpp,v 1.21 2008/12/17 13:01:26 franklan Exp $
                          kvposition.cpp  -  description
                             -------------------
    begin                : Sun May 19 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVPosition.h"
#include "TRandom3.h"
#include "TRotation.h"
#include "Riostream.h"
#include "TMath.h"

ClassImp(KVPosition);

////////////////////////////////////////////////////////////////////////////////
//KVPosition
//
//Base class used for positioning telescopes in a multidetector array.
//
//Each KVPosition object represents an element of solid angle, with a domain in
//polar angle (theta) and azimuthal angle (phi). All methods necessary to
//inquire about relative positions of such elements (overlaps, inclusion, etc.)
//are provided.
//
//The coordinate system has the target position for its centre. The beam
//direction defines the positive z axis, with respect to which polar angles are
//measured. The +ve x-axis is taken to be vertical, and azimuthal angles
//increase when going clockwise from the +ve x-axis, looking towards the +ve
//z direction:
//
//               phi=0
//    (x) beam    |
//                | 
//                |  
//                +------- phi=90
//
//All angles are in degrees.
//
//Polar angles (theta) vary between 0 and 180 degrees
//Azimuthal angles (phi) vary between 0 and 359.999... degrees.
//
//An element is defined by theta-min/max and phi-min/max:
//Theta-min is the polar angle of the edge closest the beam (+ve z) axis
//Theta-max is the polar angle of the edge furthest from the beam (+ve z) axis
//Phi-min is the phi angle of the edge which is the most anticlockwise when
//  looking as in the figure shown above
//Phi-max is the phi angle of the edge which is the most clockwise when
//  looking as in the figure shown above
//
//The above definitions mean that phi-min is not necessarily smaller than
//phi-max: we redefine "smaller" for phi angles to mean "more anticlockwise than"
//or "less clockwise than". Some examples:
// an element with phi-min = 20 and phi-max = 40 has an azimuthal width of 20
// degrees;
// an element with phi-min = 40 and phi-max = 20 has an azimuthal width of 340
// degrees;
// an element with phi-min = 350 and phi-max = 10 has an azimuthal width of
// 20 degrees.
//
//Examples of use of KVPosition objects:
//______________________________________
//
//Generating a random unit vector within a given angular range
//
//  KVPosition pos;                 |
//  pos.SetPolarMinMax(20, 30);         |  --  or:  KVPosition pos(20, 30, 0, 90);
//  pos.SetAzimuthalMinMax(0, 90);  |
//  TVector3 dir1 = pos.GetDirection();//unit vector in direction theta=25, phi=45
//  TVector3 dir2 = pos.GetRandomDirection();//isotropic direction within angular limits of pos
//  TVector3 dir3 = pos.GetRandomDirection("random");//random direction
//

void KVPosition::init()
{
   //default initialiser

   fTheta = fPhi = -1.0;
   fTheta_min = fTheta_max = fPhi_min = fPhi_max = fDistance = 0.;
}

KVPosition::KVPosition()
{
   init();
}

KVPosition::KVPosition(Float_t thmin, Float_t thmax, Float_t phmin,
                       Float_t phmax, Float_t dist)
{
   init();
   SetPolarMinMax(thmin, thmax);
   SetAzimuthalMinMax(phmin, phmax);
   SetDistance(dist);
};

//_____________________________________________________________________________
KVPosition::KVPosition(const KVPosition & obj)
{
   //copy ctor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   obj.Copy(*this);
#else
   ((KVPosition &) obj).Copy(*this);
#endif
}

//_____________________________________________________________________________
void KVPosition::SetPolarAngle(Float_t th)
{
   //Sets the polar angle corresponding to the centre of this telescope/solid angle element/etc.
   //If the polar width has been set already (KVPosition::SetPolarWidth),
   //the limits theta-min and theta-max are calculated.

   fTheta = th;
   if (fTheta_min == -180.) {   // polar width of detector already set, calculate theta_min
      // and theta_max
      fTheta_min = fTheta - .5 * (fTheta_max + 180.);
      fTheta_max = 2. * fTheta - fTheta_min;
   }
}

//___________________________________________________________________________
void KVPosition::SetAzimuthalAngle(Float_t ph)
{
   //Sets the azimuthal angle corresponding to the centre of this telescope/solid angle element/etc.
   //If the azimuthal width has been set already (KVPosition::SetAzimuthalWidth),
   //the limits phi-min and phi-max are calculated.

   fPhi = ph;
   if (fPhi_min == -180.) {     // azimuthal width of detector already set, calculate phi_min
      // and phi_max
      fPhi_min = fPhi - .5 * (fPhi_max + 180.);
      fPhi_max = 2. * fPhi - fPhi_min;
      //make sure phimin and phimax are between 0 and 360
      if (fPhi_min < 0.)
         fPhi_min += 360.;
      if (fPhi_max >= 360.)
         fPhi_max -= 360.;
   }
}

//___________________________________________________________________________
void KVPosition::SetPolarWidth(Float_t pw)
{
   //Set theta_min and theta_max from width (in degrees).
   //If theta is already known, use to set theta_min and theta_max.
   //If not, keep relative values (negative) of theta_min and theta_max,
   //to be used when theta is set 

   if (fTheta > -1.0) {
      fTheta_min = fTheta - .5 * pw;
      fTheta_max = fTheta + .5 * pw;
   } else {
      fTheta_min = -180.;
      fTheta_max = fTheta_min + pw;
   }
}

//___________________________________________________________________________
void KVPosition::SetPolarMinMax(Float_t min, Float_t max)
{
   //Set min and max polar angles and calculate (mean) theta

   fTheta_min = min;
   fTheta_max = max;
   fTheta = .5 * (min + max);
}

//___________________________________________________________________________
void KVPosition::SetAzimuthalWidth(Float_t aw)
{
   //Set phi_min and phi_max from width (in degrees)
   //  If phi is already known, use to set phi_min and phi_max
   //  If not, keep relative values (negative) of phi_min and phi_max,
   //  to be used when phi is set 

   if (fPhi > -1.0) {
      fPhi_min = fPhi - .5 * aw;
      if (fPhi_min < 0.)
         fPhi_min += 360.;
      fPhi_max = fPhi + .5 * aw;
      if (fPhi_max >= 360.)
         fPhi_max -= 360.;
   } else {
      fPhi_min = -180.;
      fPhi_max = fPhi_min + aw;
   }
}

//___________________________________________________________________________
void KVPosition::SetAzimuthalMinMax(Float_t min, Float_t max)
{
   //Set min and max azimuthal angles and calculate (mean) phi

   fPhi_min = min;
   fPhi_max = max;
   if (min > max)
      max += 360;
   fPhi = .5 * (min + max);
   if (fPhi >= 360.)
      fPhi -= 360.;
}

//___________________________________________________________________________
TVector3 KVPosition::GetRandomDirection(Option_t * t)
{
   //Set theta and phi to random values between the max and min limits defining the
   //solid angle element, return a unit vector in this direction.
   //Depending on the optional option string, the direction is either drawn at
   //"random" among the corresponding angles, or "isotropic".
   //By default, the direction is "isotropic".

   Double_t  dtor = TMath::DegToRad();
   Double_t th,ph;
	GetRandomAngles(th,ph,t);
	TVector3 dummy;
	dummy.SetMagThetaPhi(1.0,th*dtor,ph*dtor); // a unit vector
	
	return dummy;
}
//___________________________________________________________________________
void KVPosition::GetRandomAngles(Double_t &th,Double_t &ph, Option_t * t)
{
   //Set theta and phi to random values between the max and min limits defining the
   //solid angle element.
   //Depending on the optional option string, the direction is either drawn at
   //"random" among the corresponding angles, or "isotropic".
   //By default, the direction is "isotropic".
	
	Float_t dtor = TMath::DegToRad();
   Float_t phmin = fPhi_min;     //phimin in radians
   Float_t phmax = fPhi_max;     //phimax in radians
   if (phmax < phmin)
      phmax += 360;
   
	Float_t dphi = phmax - phmin;
   Float_t thmin, thmax, dtheta;

	if (!strcmp(t, "random")) {
      thmin = fTheta_min * dtor;
      thmax = fTheta_max * dtor;
      dtheta = thmax - thmin;
      th = gRandom->Uniform(dtheta) + thmin;
   } else {
      thmin = TMath::Cos(fTheta_min * dtor);
      thmax = TMath::Cos(fTheta_max * dtor);
      dtheta = thmin - thmax;
      th = TMath::ACos(gRandom->Uniform(dtheta) + thmax);
   }
   ph = gRandom->Uniform(dphi) + phmin;
   if (ph > 360)
      ph -= 360;
  	th/=dtor;
	
}

//___________________________________________________________________________
Bool_t KVPosition::IsInPhiRange(const Float_t phi)
{
   //kTRUE if given angle phi is within the azimuthal range of this solid
   //angle element

   Float_t phimintest = fPhi_min;
   Float_t phimaxtest = fPhi_max;
   if (phimintest > phimaxtest) {
      phimaxtest += 360.;
   }
   if (phi >= phimintest && phi <= phimaxtest) {
      return kTRUE;
   }
   if ((phi + 360.) >= phimintest && (phi + 360.) <= phimaxtest) {
      return kTRUE;
   }
   return kFALSE;
}

//___________________________________________________________________________
Bool_t KVPosition::IsInPolarRange(const Float_t theta)
{
   //kTRUE if given angle theta is within the polar range of this solid angle element

   if (theta >= fTheta_min && theta <= fTheta_max)
      return kTRUE;
   else
      return kFALSE;
}

//_____________________________________________________________________________________
Bool_t KVPosition::IsSmallerThan(KVPosition * pos)
{
   //kTRUE if "this" is entirely contained within "pos"

   return (pos->IsInPolarRange(GetThetaMin())
           && pos->IsInPolarRange(GetThetaMax())
           && pos->IsInPhiRange(GetPhiMin())
           && pos->IsInPhiRange(GetPhiMax()));
}

//___________________________________________________________________________
Bool_t KVPosition::IsAlignedWith(KVPosition * pos)
{
   //kTRUE if one of the two solid angle elements is completely contained within the other.

   return (IsSmallerThan(pos) || pos->IsSmallerThan(this));
}

//___________________________________________________________________________
Bool_t KVPosition::IsOverlappingWith(KVPosition * other)
{
   //kTRUE if there is at least partial overlap between two solid angle elements

   return (
             //overlapping corners - case 1
             (IsInPolarRange(other->GetThetaMin())
              || IsInPolarRange(other->GetThetaMax()))
             && (IsInPhiRange(other->GetPhiMin())
                 || IsInPhiRange(other->GetPhiMax()))
       ) || (
               //overlapping corners - case 2
               (other->IsInPolarRange(GetThetaMin())
                || other->IsInPolarRange(GetThetaMax()))
               && (other->IsInPhiRange(GetPhiMin())
                   || other->IsInPhiRange(GetPhiMax()))
       ) || (
               //case where this is phi-contained by the other, but the other is theta-contained by this
               (other->IsInPolarRange(GetThetaMin())
                || other->IsInPolarRange(GetThetaMax()))
               && (IsInPhiRange(other->GetPhiMin())
                   || IsInPhiRange(other->GetPhiMax()))
       ) || (
               //case where this is phi-contained by the other, but the other is theta-contained by this
               (IsInPolarRange(other->GetThetaMin())
                || IsInPolarRange(other->GetThetaMax()))
               && (other->IsInPhiRange(GetPhiMin())
                   || other->IsInPhiRange(GetPhiMax()))
       );
}

//___________________________________________________________________________
Bool_t KVPosition::IsAzimuthallyWiderThan(KVPosition * pos)
{
   //kTRUE if "this" has larger azimuthal width than "pos".
   //Takes care of cases where the solid angle straddles 0 degrees
   if (GetAzimuthalWidth() > pos->GetAzimuthalWidth())
      return kTRUE;
   return kFALSE;
}

//___________________________________________________________________________
TVector3 KVPosition::GetDirection()
{
   //Returns a unit vector corresponding to the direction of fTheta, fPhi
   //i.e. the centre of the solid angle element.

   TVector3 tmp(1.0);
   tmp.SetTheta(fTheta * TMath::Pi() / 180.);
   tmp.SetPhi(fPhi * TMath::Pi() / 180.);
   return tmp;
}

//___________________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVPosition::Copy(TObject & obj) const
#else
void KVPosition::Copy(TObject & obj)
#endif
{
   //Copy this to obj
   KVBase::Copy(obj);
   ((KVPosition &) obj).SetPolarMinMax(GetThetaMin(), GetThetaMax());
   ((KVPosition &) obj).SetAzimuthalMinMax(GetPhiMin(), GetPhiMax());
   ((KVPosition &) obj).SetDistance(GetDistance());
}

//___________________________________________________________________________

void KVPosition::GetCornerCoordinates(TVector3 * corners, Double_t depth)
{
   // Fill the array (TVector3 corner[4]) with the coordinates of the 4 'corners' of the solid angle element.
	//
	// These 'corners' are the points of intersection between the plane defined by the normal
	// to the centre of the solid angle (direction: theta,phi), at a distance fDistance [cm] from the
	// origin, and the four lines starting at the origin with directions (thetamin,phimin),
	// (thetamax,phimin), (thetamax,phimax), (thetamin,phimax).
	//
	// If optional argument 'depth' [cm] is given, the coordinates are calculated for the plane
	// situated at distance (fDistance+depth) from the origin.
   //
   // The order of the 4 corners is as follows:
   //       corners[3] : theta-min, phi-min
   //       corners[2] : theta-max, phi-min
   //       corners[1] : theta-max, phi-max
   //       corners[0] : theta-min, phi-max
	//
	// Coordinates are in CENTIMETRES
	
	// calculate unit vector normal to solid angle
	Double_t pthR = GetTheta()*TMath::DegToRad();
	Double_t pphR = GetPhi()*TMath::DegToRad();
	TVector3 normal_to_plane(sin(pthR)*cos(pphR), sin(pthR)*sin(pphR), cos(pthR));
	
	// the four directions/lines
	corners[3].SetMagThetaPhi(1.0, TMath::DegToRad()*GetThetaMin(), TMath::DegToRad()*GetPhiMin() );
	corners[2].SetMagThetaPhi(1.0, TMath::DegToRad()*GetThetaMax(), TMath::DegToRad()*GetPhiMin() );
	corners[1].SetMagThetaPhi(1.0, TMath::DegToRad()*GetThetaMax(), TMath::DegToRad()*GetPhiMax() );
	corners[0].SetMagThetaPhi(1.0, TMath::DegToRad()*GetThetaMin(), TMath::DegToRad()*GetPhiMax() );
	
	// calculate intersection points
	for(register int i=0; i<4; i++){
		Double_t t = corners[i] * normal_to_plane;
		if( t <= 0.0 ) corners[i].SetXYZ(0,0,0);
		else corners[i] *= (fDistance/10. + depth)/t;
	}
}

//___________________________________________________________________________

void KVPosition::GetCornerCoordinatesInOwnFrame(TVector3 * corners, Double_t depth)
{
	// Like GetCornerCoordinates(), except that the coordinates correspond to a reference frame
	// in which the +ve z-axis goes through the centre of the solid angle

	GetCornerCoordinates(corners,depth);
	TRotation rot_to_frame;
	rot_to_frame.SetYEulerAngles( -GetPhi()*TMath::DegToRad(), -GetTheta()*TMath::DegToRad(), 0. );
	TVector3 displZ(0,0,fDistance/10.+depth);
	for(register int i=0;i<4;i++){
		corners[i] = rot_to_frame*corners[i] - displZ;
	}
}

//___________________________________________________________________________
Double_t KVPosition::GetSolidAngle(void)
{
   //return values of the solid angle (in msr) seen by the geometric ensemble
	
	return (-1.*cos(GetThetaMax()*TMath::DegToRad())+cos(GetThetaMin()*TMath::DegToRad()))*(GetAzimuthalWidth()*TMath::DegToRad())*1.e3;
	
}

//_________________________________________________________________________________________
Float_t KVPosition::GetAzimuthalWidth(Float_t phmin, Float_t phmax) const
{
   //Calculate azimuthal width taking phi-min as the most anticlockwise point of the
   //element and phi-max the most clockwise.
   //If no arguments are given, width calculated for this object
   //Otherwise, width calculated for given phi-min and phi-max

   if (phmin == -1.)
      phmin = GetPhiMin();
   if (phmax == -1.)
      phmax = GetPhiMax();
   if (phmin < phmax)
      return (phmax - phmin);
   else
      return (phmax - phmin + 360.);
}

//__________________________________________________________________________________

void KVPosition::GetWidthsFromDimension(Float_t lin_dim)
{
   //Calculate azimuthal and polar widths for a square element placed at a
   //given distance from the origin with linear dimension 'lin_dim' (in mm).
   //SetDistance, SetTheta and SetPhi must already have been called.

   if (GetDistance() <= 0.0) {
      Error("GetWidthsFromDimension", "Distance not set");
      return;
   }
   Float_t d__th =
       2. * TMath::RadToDeg() * TMath::ATan(lin_dim /
                                            (2. * GetDistance()));
   Float_t d__ph =
       TMath::RadToDeg() * lin_dim / (GetDistance() * GetSinTheta());
   SetPolarWidth(d__th);
   SetAzimuthalWidth(d__ph);
}
