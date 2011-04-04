/***************************************************************************
                          kvparticle.h  -  description
                             -------------------
    begin                : Sun May 19 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVParticle.h,v 1.41 2008/05/21 13:19:56 ebonnet Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVPARTICLE_H
#define KVPARTICLE_H

#include "TVector3.h"
#include "TLorentzVector.h"
#include "KVBase.h"
#include "TRef.h"
#include "TMath.h"
#include "KVList.h"

class KVEvent;
class KVList;
class KVParticleCondition;

class KVParticle:public TLorentzVector {

   TString fName;            	//!non-persistent name field - Is useful
   TString fFrameName;			//!non-persistent frame name field, sets when calling SetFrame method
	KVList *fBoosted;				//!list of momenta of the particle in different Lorentz-boosted frames
   									 
   static Double_t kSpeedOfLight;       //speed of light in cm/ns

 protected:

   TVector3 * fE0;             //the momentum of the particle before it is slowed/stopped by an absorber
	virtual void AddGroup_Withcondition(const Char_t*, KVParticleCondition*)
	{
		// Dummy implementation of AddGroup(const Char_t* groupname, KVParticleCondition*)
		// Does nothing. Real implementation is in KVNucleus::AddGroup_Withcondition.
		printf("DUUUUUUUUUUUUUMYYYYYYY do nothing\n");
	};
	virtual void AddGroup_Sanscondition(const Char_t* groupname, const Char_t* from="");
	void SetGroupNames(const Char_t* groupname) { fName=groupname; }
	const Char_t* GetGroupNames() const { return fName; }
	
	public:
	
	Bool_t HasFrame(const Char_t* frame);
	Int_t GetNumberOfDefinedFrames(void) {  
		if (fBoosted) return fBoosted->GetEntries();
		else return 0;
		//	return (fBoosted ? fBoosted->GetEntries() : 0); 
	}
	
   enum {
      kIsOK = BIT(14),          //acceptation/rejection flag
      kIsOKSet = BIT(15),       //flag to indicate flag is set
      kIsDetected = BIT(16)     //flag set when particle is slowed by some KVMaterial
   };

   static Double_t C();

    KVParticle();
    KVParticle(Double_t m, TVector3 & p);
    KVParticle(Double_t m, Double_t px, Double_t py, Double_t pz);
    KVParticle(const KVParticle &);
    virtual ~ KVParticle();
   void init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif
   virtual void Clear(Option_t * opt = "");

   void SetMass(Double_t m) {
      SetXYZM(Px(), Py(), Pz(), m);
   };
   Double_t GetMass() const {
      return M();
   };
   void SetMomentum(const TVector3 & v) {
      SetXYZM(v(0), v(1), v(2), M());
   };
   void SetMomentum(const TVector3 * v) {
      SetXYZM((*v) (0), (*v) (1), (*v) (2), M());
   };
   void SetMomentum(Double_t px, Double_t py, Double_t pz, Option_t * opt =
                    "cart");
   void SetMomentum(Double_t T, TVector3 dir);
   void SetRandomMomentum(Double_t T, Double_t thmin, Double_t thmax,
                          Double_t phmin, Double_t phmax,
                          Option_t * opt = "isotropic");
   virtual void Print(Option_t * t = "") const;
   void SetKE(Double_t ecin);
   void SetEnergy(Double_t e) {
      SetKE(e);
   };
   void SetVelocity(const TVector3&);
   TVector3 GetMomentum() const {
      return Vect();
   };
   Double_t GetKE() const {
      return (E() - M());
   };
   Double_t GetEnergy() const {
      return GetKE();
   };
   Double_t GetTransverseEnergy() const {
      Double_t etran = TMath::Sin(Theta());
       etran = TMath::Power(etran, 2.0);
       etran *= GetKE();
       return etran;
   };
   Double_t GetEtran() const {
      return GetTransverseEnergy();
   };
   Double_t GetRTransverseEnergy() const {
      Double_t etran = Mt()-GetMass();
       return etran;
   };
   Double_t GetREtran() const {
      return GetRTransverseEnergy();
   };
   Double_t GetE() const {
      return GetKE();
   };
   TVector3 GetVelocity() const;
   TVector3 GetV() const {
      return GetVelocity();
   };
   Double_t GetVpar() const {
      return GetV().z();
   };
   Double_t GetVperp() const;
   Double_t GetTheta() const {
      return TMath::RadToDeg() * Theta();
   };
   Double_t GetPhi() const {
      Double_t phi = TMath::RadToDeg() * Phi();
       return (phi < 0 ? 360. + phi : phi);
   };
   void SetTheta(Double_t theta) {
      TLorentzVector::SetTheta(TMath::DegToRad() * theta);
   }
   void SetPhi(Double_t phi) {
      TLorentzVector::SetPhi(TMath::DegToRad() * phi);
   };

   virtual Bool_t IsOK();
   void SetIsOK(Bool_t flag = kTRUE);
   void ResetIsOK() {
      ResetBit(kIsOKSet);
   };
	
	KVList* GetListOfFrames(void) {return fBoosted;}
	
   void SetE0(TVector3 * e = 0) {
      if (!fE0)
         fE0 = new TVector3;
      if (!e) {
         *fE0 = GetMomentum();
      } else {
         *fE0 = *e;
      }
   };
   TVector3 *GetPInitial() const {
      return fE0;
   };
   void SetIsDetected() {
      SetBit(kIsDetected);
   };
   Bool_t IsDetected() {
      return TestBit(kIsDetected);
   }
   KVParticle & operator=(const KVParticle & rhs);

   void ResetEnergy();

   const Char_t *GetName() const;
  	void SetName(const Char_t * nom);
	
	void AddGroup(const Char_t* groupname,const Char_t* from="");
	void AddGroup(const Char_t* groupname, KVParticleCondition*);
	
	Bool_t BelongsToGroup(const Char_t* groupname) const;
	void RemoveGroup(const Char_t* groupname);
	void RemoveAllGroups();
	void ListGroups(void) const;
	
	void SetFrame(const Char_t * frame, const TVector3 & boost, Bool_t beta =
                 kFALSE);
   void SetFrame(const Char_t * frame, TLorentzRotation & rot);
   void SetFrame(const Char_t * frame, TRotation & rot);
   void SetFrame(const Char_t * frame, const TVector3 & boost, TRotation & rot,
                 Bool_t beta = kFALSE);
	
	void SetFrame(const Char_t * newframe, const Char_t * oldframe,
                 const TVector3 & boost, Bool_t beta = kFALSE);
   void SetFrame(const Char_t * newframe, const Char_t * oldframe,
                 TLorentzRotation & rot);
   void SetFrame(const Char_t * newframe, const Char_t * oldframe,
                 TRotation & rot);
   void SetFrame(const Char_t * newframe, const Char_t * oldframe,
                 const TVector3 & boost, TRotation & rot, Bool_t beta = kFALSE);
	
	KVParticle *GetFrame(const Char_t* frame);
	
	const Char_t *GetFrameName(void) const { return fFrameName; }
	void SetFrameName(const Char_t* framename) {fFrameName=framename;} 
	
   ClassDef(KVParticle, 7)      //General base class for all massive particles
};

#endif
