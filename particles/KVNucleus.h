/***************************************************************************
$Id: KVNucleus.h,v 1.37 2009/03/03 13:36:00 franklan Exp $
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVNUCLEUS_H
#define KVNUCLEUS_H

#define MAX_Z_MASS_TABLE 108
#define MAX_A_MASS_TABLE 263

#include "TVector3.h"
#include "TEnv.h"
#include "KVParticle.h"
#include "KVParticleCondition.h"
#include "TLorentzRotation.h"
#include "TH2F.h"
#include "KVString.h"
#include "KVDataSet.h"

class KVNucleus:public KVParticle {

 private:
   UChar_t fA;                  //nuclear mass number
   UChar_t fZ;                  //nuclear charge number (atomic number)
   UChar_t fMassFormula;        //mass formula for calculating A from Z
   static Double_t fMassExcess[MAX_Z_MASS_TABLE][MAX_A_MASS_TABLE];     //!table of mass excesses in MeV for known nuclei
   static UInt_t fNb_nuc;       //!counts number of existing KVNucleus objects
   static Char_t fElements[][3];        //!symbols of chemical elements
   void ReadMassTable();
   Double_t fExx;               //excitation energy in MeV

   enum {
      kIsHeavy = BIT(17)        //flag when mass of nucleus is > 255
   };

 protected:
	virtual void AddGroup_Withcondition(const Char_t* groupname, KVParticleCondition*);
		 
 public:
   enum {                       //determines how to calculate mass from Z
      kBetaMass,
      kVedaMass,
      kEALMass,
		kEALResMass,
		kEPAXMass
   };

   static Double_t kAMU;        //atomic mass unit in MeV
   static Double_t u(void);

   inline void SetMassFormula(UChar_t mt);
   inline Int_t GetMassFormula() const { return (Int_t)fMassFormula;};
   
   void init();
    KVNucleus();
    KVNucleus(const KVNucleus &);
   virtual void Clear(Option_t * opt = "");
    KVNucleus(Int_t z, Int_t a = 0);
    KVNucleus(Int_t z, Float_t t, TVector3 & p);
    KVNucleus(Int_t z, Int_t a, TVector3 p);
    KVNucleus(const Char_t *);

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif

   Bool_t IsSortable() const {
      return kTRUE;
   };
   Int_t Compare(const TObject * obj) const;

    virtual ~ KVNucleus();
   static Int_t GetAFromZ(Double_t, Char_t mt);
   static Double_t GetRealAFromZ(Double_t, Char_t mt);
   const Char_t *GetSymbol() const;

   static Int_t GetZFromSymbol(const Char_t *);
   void SetZFromSymbol(const Char_t *);
   void Set(const Char_t *);

   void SetZ(Int_t z, Char_t mt = -1);
   void SetA(Int_t a);
   virtual void Print(Option_t * t = "") const;
   Int_t GetZ() const;
   Int_t GetA() const;
   Int_t GetN() const { return GetA()-GetZ(); };
   Double_t GetAsurZ() const {return Double_t(GetA())/GetZ(); };
   Double_t GetNsurZ() const {return Double_t(GetN())/GetZ(); };
	Double_t GetChargeAsymetry() const { return Double_t(GetA()-GetZ())/GetZ(); }
	UChar_t GetMassFormula() const { return fMassFormula; }
   Double_t GetEnergyPerNucleon();
   Double_t GetAMeV();
   Double_t GetMassExcess(Int_t z = -1, Int_t a = -1);
   Double_t GetBindingEnergy(Int_t z = -1, Int_t a = -1);
   static Double_t LiquidDrop_BrackGuet(UInt_t A, UInt_t Z);
		
   Bool_t IsKnown(int z = -1, int a = -1);

   void SetExcitEnergy(Double_t e);
   Double_t GetExcitEnergy() const {
      return fExx;
   };
	
    KVNucleus & operator=(const KVNucleus & rhs);
   KVNucleus operator+(const KVNucleus & rhs);
   KVNucleus operator-(const KVNucleus & rhs);
    KVNucleus & operator+=(const KVNucleus & rhs);
    KVNucleus & operator-=(const KVNucleus & rhs);
	
	TH2F* GetKnownNucleiChart(KVString method="GetBindingEnergy");
	
    ClassDef(KVNucleus, 5)      //Class describing atomic nuclei
};

inline void KVNucleus::SetMassFormula(UChar_t mt)
{
   //Set mass formula used for calculating A from Z for this nucleus
   fMassFormula = mt;
   SetA(GetAFromZ(GetZ(), fMassFormula));       //recalculate A and mass
}

#endif
