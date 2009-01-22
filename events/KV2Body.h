/***************************************************************************
                          KV2Body.h  -  description
                             -------------------
    begin                : 28/11/2003
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KV2Body.h,v 1.4 2009/01/22 13:56:13 ebonnet Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KV2BODY_H
#define KV2BODY_H

#include "TObject.h"
#include "TObjArray.h"
#include "TVector3.h"
#include "KVNucleus.h"

class KV2Body:public TObject {

   TObjArray fNuclei;           //nuclei involved in calculation
   Double_t fEDiss;             //dissipated energy, 0 means elastic scattering

   TVector3 VCM;                //velocity of centre of mass
   Double_t BCM;                //beta of centre of mass
   Double_t WLT;                //total lab energy
   Double_t WCT;                //total cm energy
   Double_t WC3, WC4, K3, K4;
   Double_t VC[5];              //cm velocities
   Double_t EC[5];              //cm energies
   Double_t TETA3MAX;
   Double_t TETA4MAX;
   Bool_t fDeleteTarget;
   Bool_t fDeleteProj;
   Bool_t fDeleteN4;

   void Set4thNucleus();

 public:

   void init();
    KV2Body();
    KV2Body(KVNucleus * proj, KVNucleus * cib, KVNucleus * proj_out =
            0, Double_t Ediss = 0.0);
    virtual ~ KV2Body();

   void CalculateKinematics();

   static Double_t GetVelocity(Double_t mass, Double_t E);

   void SetProjectile(KVNucleus *);
   void SetProjectile(Int_t z, Int_t a = 0);
   void SetTarget(KVNucleus *);
   void SetTarget(Int_t z, Int_t a = 0);
   void SetOutgoing(KVNucleus * proj_out);
   void SetOutgoing(Int_t inuc, KVNucleus * nuc);

   void SetExcitEnergy(Double_t ex) {
      fEDiss = ex;
   };
   Double_t GetExcitEnergy() const {
      return fEDiss;
   }
   void SetEDiss(Double_t ex) {
      SetExcitEnergy(ex);
   };
   Double_t GetEDiss() const {
      return GetExcitEnergy();
   }
   KVNucleus *GetNucleus(Int_t i) const;

   Double_t GetQReaction() const;
   Double_t GetQGroundStates() const;
   Double_t GetLabGrazingAngle(Int_t i = 1) const;

   Double_t GetCMEnergy() const;
   Double_t GetCMEnergy(Int_t i) const;

   Double_t GetMaxAngleLab(Int_t i) const;

   TVector3 GetCMVelocity() const;
   Double_t GetCMVelocity(Int_t i) const;
   Double_t GetCMGamma() const {
      Double_t gamma =
          1.0 - TMath::Power(GetCMVelocity().Mag() / KVParticle::C(), 2.);
      if (gamma > 0.)
          gamma = 1. / TMath::Sqrt(gamma);
      else {
         Warning("GetCMGamma", "1 - (beta)**2 = %f ... strange!", gamma);
         gamma = 0.0;
      }
      return gamma;
   };

   Double_t GetELabProj(Double_t ThetaLab_Proj) const;
   Double_t GetVLabProj(Double_t ThetaLab_Proj) const;
   Double_t GetThetaCMProj(Double_t ThetaLab_Proj) const;
   Double_t GetThetaLabTarget(Double_t ThetaLab_Proj) const;
   Double_t GetThetaCMTarget(Double_t ThetaLab_Proj) const;
   Double_t GetELabTarget(Double_t ThetaLab_Proj) const;
   Double_t GetXSecRuthLab(Double_t ThetaLab_Proj) const;
   Double_t GetXSecRuthCM(Double_t ThetaLab_Proj) const;
	
	Double_t GetIntegratedXSecRuthLab(Float_t th1,Float_t th2,Float_t phi1=-1,Float_t phi2=-1) const;
   
	void Print(Option_t * opt = "") const;

   ClassDef(KV2Body, 0)         //Relativistic binary kinematical calculation
};

#endif
