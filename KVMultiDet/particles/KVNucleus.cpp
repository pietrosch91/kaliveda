/***************************************************************************
$Id: KVNucleus.cpp,v 1.48 2009/04/02 09:32:55 ebonnet Exp $
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVNucleus.h"
#include "KVString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "Riostream.h"
#include "TSystem.h"
#include "TEnv.h"
#include "KVParticleCondition.h"
#include "Riostream.h"
#include "TMethodCall.h"
#include "KVNumberList.h"
#include "TPluginManager.h"

//Atomic mass unit in MeV
//Reference: 2002 CODATA recommended values Reviews of Modern Physics 77, 1-107 (2005)
Double_t KVNucleus::kAMU = 9.31494043e02;

ClassImp(KVNucleus);

////////////////////////////////////////////////////////////////////////////
//Description of atomic nuclei
//
//Examples of use:
//________________
//
//Create an alpha particle, and find its binding energy, mass, and mass excess:
//
// KVNucleus n(2,4); // create nucleus with Z=2 and A=4
// cout << "Binding energy of alpha = " << n.GetBindingEnergy() << " MeV" << endl;
// cout << "Mass = " << n.GetMass() << " = 4 * " << KVNucleus::kAMU << " + " << n.GetMassExcess() << endl;
//
//Using the same object we can also obtain binding energies and mass excesses
//for all other nuclei:
//
//  n.GetBindingEnergy(1,2);//binding energy of deuteron
//  n.GetMassExcess(0,1);//mass excess of neutron
//
//NUCLEAR MASSES
//-----------------------
//We can also create nuclei by only specifying the atomic number Z.
//In this case the mass number A is calculated from Z:
//
//    KVNucleus nuc(82);//create Pb nucleus
//    nuc.GetA();//by default this is the A corresponding to beta-stability, i.e. 208
//    nuc.SetMassFormula(KVNucleus::kVedaMass);//change the mass formula used for this nucleus
//    nuc.GetA();//now the value is that calculated according to the Veda formula, 202
//    nuc.SetMassFormula(KVNucleus::kEALMass);//Evaporation Attractor Line from R.J. Charity
//    nuc.GetA();//gives 186
//    nuc.SetMassFormula(KVNucleus::kBetaMass);//restore the default mass formula
//
//Z and A can be specified separately:
//
//   KVNucleus a;//no A or Z specified
//   a.SetZ(10);//at this moment the nucleus' mass number is 20 (beta-stability)
//   a.SetA(24);//now this represents a 24Ne nucleus
//
//However, be careful not to use SetZ() AFTER SetA(), because the mass number
//is always automatically calculated from Z after a call to SetZ().
//
//The value of the atomic mass unit, u, is given by KVNucleus::kAMU or KVNucleus::u()
//It is 931.494 043 x 10**6 eV, as per the 2002 CODATA recommended values (Reviews of Modern Physics 77, 1-107 (2005)).
//
//  Nuclear Arithmetic & Calorimetry
//  ----------------------------------------
//The '+', '-' and '=' operators have been redefined for the KVNucleus class.
//One can therefore perform "nuclear arithmetic".
//Example:
//
//  KVNucleus c = a + b; //'a' and 'b' are also KVNucleus objects
//
//The Z, A, momentum and excitation energy of 'c' are calculated from the
//appropriate conservation laws. The mass excesses of the 3 nuclei are obviously
//taken into consideration.
//If 'a' and 'b' are projectile and target, 'c' is the compound nucleus after
//fusion.
//In order to perform calorimetry (calculation of source characteristics from
//daughter nuclei) one need only sum all nuclei associated with the source.
//The resulting nucleus is the source nucleus with its Z, A, momentum and
//excitation energy.
//
//The subtraction operator allows to perform energy balance for a binary
//splitting of a nucleus.
//Example:
//
//      KVNucleus d = c - b;
//
//In this case, the resulting nucleus 'd' should be identical to 'a' in the first
//example. One could also imagine
//
//      KVNucleus e = c - alpha;
//
//where 'alpha' is a KVNucleus alpha-particle, for which we specify the
//momentum after emission. The resulting nucleus 'e' is the residue of the
//fusion compound after evaporation of an alpha particle.
//
//The operators '+=' and '-=' also exist. 'a+=b' means 'a = a + b' etc.
//
//Mass Table
//==========
//Different mass tables can be implemented using classes derived from
//KVMassTable. The mass table to be used is defined by environment variable
//
//  KVNucleus.MassTable:        MyMassTable
//
//where 'MyMassTable' must be defined in terms of a KVMassTable plugin:
//
//+Plugin.KVMassTable: MyMassTable  MyMassTable  MyMassTable.cpp+  "MyMassTable()"
////////////////////////////////////////////////////////////////////////////

UInt_t KVNucleus::fNb_nuc = 0;
KVMassTable *KVNucleus::fMassTable=0;

#define MAXZ_ELEMENT_SYMBOL 109
Char_t KVNucleus::fElements[][3] = {
   "n", "p", "d", "t", "He", "Li", "Be", "B", "C", "N", "O",
   "F", "Ne", "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",
   "Sc",
   "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As",
   "Se", "Br",
   "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag",
   "Cd",
   "In", "Sn", "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",
   "Pm",
   "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta",
   "W",
   "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn",
   "Fr",
   "Ra", "Ac", "Th", "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es",
   "Fm", "Md",
   "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt"
};

const Char_t *KVNucleus::GetSymbol(Option_t* opt) const
{
   // Returns symbol of isotope corresponding to this nucleus,
   // i.e. "238U", "12C", "3He" etc.
   // Neutrons are represented by "n".
   // Isotopes of hydrogen are represented by "p", "d", "t".
   // In order to have just the symbol of the chemical element
   // (e.g. "Pt", "Zn", "Fe"), call with opt="EL".

	static TString name;
   Int_t a = GetA();
   Int_t z = GetZ();
   Bool_t Mpfx = strcmp(opt,"EL");// kTRUE if mass prefix required
   if (z == 0) {
		if( a==1 ) 
			name = "n";
		else{
			if(Mpfx) name.Form("%dn",a);
         else name = "n";
      }
   }
	else if(z == 1) {
		if( a<4 ) {
			name = fElements[a];
		}
		else
		{
         if(Mpfx) name.Form("%dH",a);
         else name = "H";
		}
	}
	else if (GetZ() <= MAXZ_ELEMENT_SYMBOL) {
		if(Mpfx) name.Form("%d%s",a,fElements[z+2]);
      else name = fElements[z+2];
   }
	else
		name="";
   return name.Data();
}

void KVNucleus::Set(const Char_t * isotope)
{
   //Set nucleus' Z & A using chemical symbol e.g. Set("12C") or Set("233U") etc.
   Int_t A;
   Char_t name[5];
   TString tmp(isotope);
   if (tmp.BeginsWith("nat"))
      tmp.Remove(0, 3);
   if (sscanf(tmp.Data(), "%d%s", &A, name) == 2) {
      //name given in form "208Pb"
      SetZFromSymbol(name);
      SetA(A);
   } else if (sscanf(tmp.Data(), "%s", name) == 1) {
      //name given in form "Pb"
      SetZFromSymbol(name);
   }
}

Int_t KVNucleus::GetZFromSymbol(const Char_t * sym)
{
   //Returns Z of nucleus with given symbol i.e. "C" => Z=6, "U" => Z=92
   //if unknown, returns -1
   if (!strcmp(sym, "H"))
      return 1;
   for (register int i = 0; i < MAXZ_ELEMENT_SYMBOL + 2; i++) {
      if (!strcmp(sym, fElements[i])) {
         if (i == 0)
            return 0;
         else if (i < 4)
            return 1;
         else
            return i - 2;
      }
   }
   cout << "KVNucleus::GetZFromSymbol : " << sym << " is unknown" << endl;
   return -1;
}

void KVNucleus::SetZFromSymbol(const Char_t * sym)
{
   //Set Z of nucleus with given symbol i.e. "C" => Z=6, "U" => Z=92
   //For "p", "d", "t" we set also the mass A
   if (!strcmp(sym, "H")) {
      SetZ(1);
      return;
   }
   for (register int i = 0; i < MAXZ_ELEMENT_SYMBOL + 2; i++) {
      if (!strcmp(sym, fElements[i])) {
         if (i == 0)
            SetZ(0);
         else if (i < 4) {
            SetZ(1);
            SetA(i);
         } else
            SetZ(i - 2);
         return;
      }
   }
   Error("SetZFromSymbol", "%s is unknown", sym);
}

//_________________________________________________________________________________

void KVNucleus::init()
{
   // Default intialisations
   // The mass formula is taken from environment
   // variable KVNucleus.DefaultMassFormula (if not defined, we
   // use kBetaMass, i.e. the formula for the valley of beta-stability).
	// First nucleus created will cause current mass table to be initialised
   
   fZ = fA = 0;
   fExx = 0;
   if (!fNb_nuc){
      KVBase::InitEnvironment(); // initialise environment i.e. read .kvrootrc
      InitMassTable();
   }
   fMassFormula = (Int_t)gEnv->GetValue("KVNucleus.DefaultMassFormula", kBetaMass);
   fNb_nuc++;
   fExx = 0.;
}

KVNucleus::KVNucleus()
{
   //
   //Default constructor.
   //
   init();
}

KVNucleus::KVNucleus(const KVNucleus & obj)
{
   //copy ctor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   obj.Copy(*this);
#else
   ((KVNucleus &) obj).Copy(*this);
#endif
}

//___________________________________________________________________________________________
void KVNucleus::Clear(Option_t * opt)
{
   //Reset nucleus' properties: set excitation energy to zero,
   //as well as A and Z. For other properties, see KVParticle::Clear
   KVParticle::Clear(opt);
   ResetBit(kIsHeavy);
   fZ = fA = 0;
   fExx = 0.;
}

//___________________________________________________________________________________________
KVNucleus::KVNucleus(Int_t z, Int_t a)
{
   //Create a nucleus with atomic number Z.
   //If the mass number A is not given, A is calculated using the
   //parametrisation determined by the value of fMassFormula (see KVNucleus::GetAFromZ).

   init();
   fZ = (UChar_t) z;
   if (z != 0 && a == 0) {
      SetA(GetAFromZ(z, fMassFormula));
   } else {
      SetA(a);
   }
}

//___________________________________________________________________________________________
KVNucleus::KVNucleus(const Char_t * symbol)
{
   //Create a nucleus defined by symbol e.g. "12C", "34Mg", "42Si" etc. etc.

   init();
   Set(symbol);
}

//___________________________________________________________________________________________
KVNucleus::KVNucleus(Int_t z, Float_t t, TVector3 & p)
{

//Create nucleus with given Z, kinetic energy t and direction p
//(p is a unit vector in the desired direction. See KVPosition for methods
//for generating such vectors).
//The mass number A is calculated from Z. See KVNucleus::GetAFromZ.
//
   init();
   fZ = (UChar_t) z;
   SetA(GetAFromZ(z, fMassFormula));
   SetMomentum(t, p);
}

//___________________________________________________________________________________________
KVNucleus::KVNucleus(Int_t z, Int_t a, TVector3 p)
{
//
//Create nucleus with given Z, A, and 3-momentum p
//
   init();
   fZ = (UChar_t) z;
   SetA(a);
   SetMomentum(&p);
}

//___________________________________________________________________________________________
KVNucleus::~KVNucleus()
{
   fNb_nuc--;
   if (fNb_nuc < 0)
      cout << "fNb_nuc = " << fNb_nuc << endl;
   fZ = fA = 0;
   fExx = 0.;
}

//___________________________________________________________________________________________

Double_t KVNucleus::GetRealAFromZ(Double_t Z, Char_t mt)
{
   //Calculate nuclear mass number from the element's atomic number Z.
   //This value is not rounded off, we just return the result of one of the following formulae:
//
//mt = KVNucleus::kVedaMass
//__________________________
//Veda - A calculated using the formula
//      fA = (1.867*fZ+.016*fZ*fZ-1.07E-4*fZ*fZ*fZ);
//      This corresponds to the amass.f subroutine of the old INDRA Veda
//      calibration programme. This formula was supposed to represent
//      the Z-dependence of isotope masses in the beta-stability valley,
//      but is in fact a rather poor approximation, especially for large Z.
//
//mt = KVNucleus::kBetaMass
//_________________________
//Beta (default) - An improved parametrisation of the beta-stability valley,
//   correct even for heavy nuclei up to 238U. The formula is the result
//   of a fit to 8 stable nuclear masses from Ne20 up to U238. 
//   fA = (.2875 + 1.7622 *Z + .013879 * Z  * Z - .000054875 * Z * Z * Z);
//
//mt = KVNucleus::kEALMass
//________________________
//EAL - parametrisation of the Evaporation Attractor Line (residue corridor)
//    due to R.J. Charity (PRC 58(1998)1073) (eq 2)
//    fA = (2.072*Z + 2.32E-03 * Z*Z) ;
//
//mt = KVNucleus::kEALResMass
//________________________
//EALRes - R.J. Charity ---- improvement of EAL parametrisation for 
// Heavy Residue (QP for instance) (PRC 58(1998)1073) (eq 7)
//    fA = (2.045*Z + 3.57E-03 * Z*Z) ;
//
//mt = any other value: A=2*Z

   Double_t A;
   switch (mt) {

   case kVedaMass:
      A = (1.867 * Z + .016 * TMath::Power(Z, 2.) -
           1.07E-4 * TMath::Power(Z, 3.));
      break;

   case kBetaMass:
      A = (.2875 + 1.7622 * Z + .013879 * TMath::Power(Z, 2.) -
           .000054875 * TMath::Power(Z, 3.));
      break;

   case kEALMass:
      A = (2.072 * Z + 2.32E-03 * TMath::Power(Z, 2.));
      break;

   case kEALResMass:
      A = (2.045 * Z + 3.57E-03 * TMath::Power(Z, 2.));
      break;

   default:
      A = 2. * Z;
   }

   return A;
}

//___________________________________________________________________________________________
Int_t KVNucleus::GetAFromZ(Double_t Z, Char_t mt)
{
//Calculate nuclear mass number from the element's atomic number Z.
//Used by default to set fA and fMass if fA not given.
//For light nuclei (Z<6) the values are given (not calculated) and
//correspond to: p, alpha, 7Li, 9Be, 11B.
//For heavier nuclei, several prescriptions are available
//by giving one of the following values to argument mt:
//
//mt = KVNucleus::kVedaMass
//__________________________
//Veda - A calculated using the formula
//      fA = (Int_t)(1.867*fZ+.016*fZ*fZ-1.07E-4*fZ*fZ*fZ) + 1;
//      This corresponds to the amass.f subroutine of the old INDRA Veda
//      calibration programme. This formula was supposed to represent
//      the Z-dependence of isotope masses in the beta-stability valley,
//      but is in fact a rather poor approximation, especially for large Z.
//
//mt = KVNucleus::kBetaMass
//_________________________
//Beta (default) - An improved parametrisation of the beta-stability valley,
//   correct even for heavy nuclei up to 238U. The formula is the result
//   of a fit to 8 stable nuclear masses from Ne20 up to U238. From carbon-12 onwards,
//   the mass is calculated using
//   fA = (Int_t) (.2875 + 1.7622 *Z + .013879 * Z  * Z - .000054875 * Z * Z * Z) + 1;
//
//mt = KVNucleus::kEALMass
//________________________
//EAL - parametrisation of the Evaporation Attractor Line (residue corridor)
//    due to R.J. Charity (PRC 58(1998)1073).
//    fA = (Int_t)(2.072*Z + 2.32E-03 * Z*Z) + 1; (eq 2)
//
//mt = KVNucleus::kEALResMass
//________________________
//EALRes - R.J. Charity ---- improvement of EAL parametrisation for 
// Heavy Residues (QP for instance) (PRC 58(1998)1073) (eq 7)
//    fA = (Int_t)(2.045*Z + 3.57E-03 * Z*Z) + 1 ;
//
//mt = any other value: A=2*Z

   Int_t A = 0;
   Int_t z = (Int_t) Z;
   switch (z) {                 // masses for lightest nuclei
   case 1:
      A = 1;
      break;
   case 2:
      A = 4;
      break;
   case 3:
      A = 7;
      break;
   case 4:
      A = 9;
      break;
   case 5:
      A = 11;
      break;
   default:
      A = (Int_t) KVNucleus::GetRealAFromZ(Z, mt) + 1;
   }
   return A;
}

//___________________________________________________________________________________________
void KVNucleus::SetA(Int_t a)
{
   //Set mass number
   //Be careful not to call SetZ() after SetA(), as SetZ() will
   //reset the mass number according to one of the available
   //parametrisations of A as a function of Z.
   //
   //For A>255 the kIsHeavy flag is set. Then fA will equal A-255,
   //and GetA will return fA+255.
   //If A<=255 the flag is reset.

   if (a > 255) {
      fA = (UChar_t) (a - 255);
      SetBit(kIsHeavy);
   } else {
      fA = (UChar_t) a;
      ResetBit(kIsHeavy);
   }
   SetMass(kAMU * a + GetMassExcess());
}

//___________________________________________________________________________________________
void KVNucleus::SetZ(Int_t z, Char_t mt)
{
//Set atomic number
//The mass number fA is automatically calculated and set using GetAFromZ().
//The optional EMassType argument allows to change the default parametrisation
//used for calculating A from Z.
   fZ = (UChar_t) z;
   if (mt > -1)
      fMassFormula = mt;
   SetA(GetAFromZ(z, fMassFormula));
}

//___________________________________________________________________________________________
void KVNucleus::Print(Option_t * t) const
{
// Display nucleus parameters
   cout << "KVNucleus Z=" << GetZ() << " A=" << GetA() << " KE=" <<
       GetKE();
   cout << " E*=" << GetExcitEnergy();
   cout << " Theta=" << GetTheta() << " Phi=" << GetPhi() << endl;
}

//___________________________________________________________________________________________
Int_t KVNucleus::GetZ() const
{
   return (Int_t) fZ;
}

//___________________________________________________________________________________________
Int_t KVNucleus::GetA() const
{
   //Returns mass number (A) of nucleus.
   //
   //The actual member variable (fA) is a UChar_t and so limited to values 0-255.
   //In case nuclei with larger A are needed (for example in calculations of 2-body
   //scattering, a temporary nucleus corresponding to the sum of the entrance channel
   //nuclei is used in order to find the outgoing target-like from the outgoing
   //projectile-like) the flag "kIsHeavy" is set and GetA returns the value (fA+255).
   //For this reason you should always use GetA and not fA.

   if (TestBit(kIsHeavy))
      return ((Int_t) fA + 255);
   return (Int_t) fA;
}

//_______________________________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVNucleus::Copy(TObject & obj) const
#else
void KVNucleus::Copy(TObject & obj)
#endif
{
   //Copy this KVNucleus into the KVNucleus object referenced by "obj"
   KVParticle::Copy(obj);
   ((KVNucleus &) obj).SetZ(GetZ());
   ((KVNucleus &) obj).SetMassFormula(fMassFormula);
   ((KVNucleus &) obj).SetA(((KVNucleus *) this)->GetA());
   ((KVNucleus &) obj).SetExcitEnergy(((KVNucleus *) this)->
                                      GetExcitEnergy());
}

//________________________________________________________________________________________

void KVNucleus::InitMassTable()
{
   //PRIVATE method - called by CTOR
	//Initialize current mass table.
	//Different mass tables can be implemented using classes derived from
	//KVMassTable. The mass table to be used is defined by environment variable
	//
	//  KVNucleus.MassTable:        MyMassTable
	//
	//where 'MyMassTable' must be defined in terms of a KVMassTable plugin:
	//
	//+Plugin.KVMassTable: MyMassTable  MyMassTable  MyMassTable.cpp+  "MyMassTable()"

	//instanciate mass table plugin
   TPluginHandler *ph;
   if (!(ph = KVBase::LoadPlugin("KVMassTable", gEnv->GetValue("KVNucleus.MassTable",""))))
	{
		Error("InitMassTable", "Cannot find plugin for KVNucleus.MassTable: %s",
				gEnv->GetValue("KVNucleus.MassTable",""));
		return;
	}

   //execute constructor/macro for mass table
   fMassTable = (KVMassTable *) ph->ExecPlugin(0);
		
	//initialise mass table
	fMassTable->Initialize();
}

//________________________________________________________________________________________

Double_t KVNucleus::GetMassExcess(Int_t z, Int_t a)
{
	//Returns mass excess value in MeV for this nucleus.
	//If optional arguments (z,a) are given we return the mass excess for the
	//required nucleus.
	//If the nucleus is not included in the mass table, an extrapolated value
	//using KVNucleus::LiquidDrop_BrackGuet is returned.

   if (z == -1)
      z = GetZ();
   if (a == -1)
      a = GetA();
	if(fMassTable->IsKnown(z,a)) return (fMassTable->GetMassExcess(z,a)/1000.);
   return (LiquidDrop_BrackGuet(a, z) - a * kAMU);
}

//________________________________________________________________________________________

Bool_t KVNucleus::IsKnown(int z, int a)
{
   //Returns kTRUE if this nucleus or (z,a) is a known nucleus
   //i.e. is included in the mass table.

   if (z == -1)
      z = GetZ();
   if (a == -1)
      a = GetA();
	return fMassTable->IsKnown(z,a);
}

//________________________________________________________________________________________

Double_t KVNucleus::GetBindingEnergy(Int_t z, Int_t a)
{
//Returns binding energy in MeV for this nucleus.
//The convention is : binding energy is positive if nucleus is bound.
//If optional arguments (z,a) are given we return the binding energy for the
//required nucleus.
//If the nucleus is not included in the mass table, an extrapolated value
//using KVNucleus::LiquidDrop_BrackGuet is returned.

   if (z == -1)
      z = GetZ();
   if (a == -1)
      a = GetA();

   return a ==
       0 ? 0. : (z * GetMassExcess(1, 1) + (a - z) * GetMassExcess(0, 1) -
                 GetMassExcess(z, a));
}

//________________________________________________________________________________________

Double_t KVNucleus::GetBindingEnergyPerNucleon(Int_t z, Int_t a)
{
//Returns binding energy in MeV/A for this nucleus.

   if (z == -1)
      z = GetZ();
   if (a == -1)
      a = GetA();
	
	if (a==0) return 0;
   return GetBindingEnergy(z,a)/a;
}
//________________________________________________________________________________________

Double_t KVNucleus::GetEnergyPerNucleon()
{
   //
   //Returns kinetic energy of nucleus per nucleon (in MeV/nucleon, donc)
   //
   return GetA()? GetEnergy() / GetA() : GetEnergy();
}

//________________________________________________________________________________________

Double_t KVNucleus::GetAMeV()
{
   //
   //Returns kinetic energy of nucleus per nucleon (in MeV/nucleon, donc)
   //
   return GetEnergyPerNucleon();
}

//________________________________________________________________________________________

KVNumberList KVNucleus::GetKnownARange(Int_t zz)
{

	if (zz==-1) zz=GetZ();	
	KVNumberList nla; nla.SetMinMax(zz,4*zz);
	KVNumberList nlb;
	nla.Begin();
	while (!nla.End()){
		Int_t aa = nla.Next();
		if (IsKnown(zz,aa)) nlb.Add(aa);
	}
	return nlb;
}

//________________________________________________________________________________________

Int_t KVNucleus::GetAWithMaxBindingEnergy(Int_t zz){

if (zz==-1) zz=GetZ();	
KVNumberList nla = GetKnownARange(zz);
nla.Begin();
Double_t emax=0;
Int_t amax=0;
while (!nla.End()){
	Int_t aa = nla.Next();
	if (GetBindingEnergyPerNucleon(zz,aa)>emax) {
		emax = GetBindingEnergyPerNucleon(zz,aa);
		amax = aa;
	}
}
return amax;

}
//___________________________________________________________________________//

void KVNucleus::AddGroup_Withcondition(const Char_t* groupname,KVParticleCondition* pc)
{
	// implementation of AddGroup(const Char_t* groupname,KVParticleCondition* pc)
	// Can be overriden in child classes [unlike
	// KVParticle::AddGroup(const Char_t* groupname,KVParticleCondition* pc), which cannot]
	
	if (pc){
		//pc->SetParticleClassName(this->IsA()->GetName());
		if (pc->Test(this)) AddGroup_Sanscondition(groupname);	
	}
}

//________________________________________________________________________________________

KVNucleus & KVNucleus::operator=(const KVNucleus & rhs)
{
   //KVNucleus assignment operator.
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   rhs.Copy(*this);
#else
   ((KVNucleus &) rhs).Copy(*this);
#endif
   return *this;
}

//________________________________________________________________________________________

KVNucleus KVNucleus::operator+(const KVNucleus & rhs)
{
   //KVNucleus addition operator.
   //Add two nuclei together to form a compound nucleus whose momentum and
   //excitation energy are calculated from energy and momentum conservation.

   KVNucleus & lhs = *this;
   Int_t ztot = lhs.GetZ() + rhs.GetZ();
   Int_t atot = lhs.GetA() + ((KVNucleus &) rhs).GetA();
   Double_t extot = lhs.GetExcitEnergy() + rhs.GetExcitEnergy();
   Double_t etot = lhs.E() + rhs.E();
   TVector3 ptot = lhs.GetMomentum() + rhs.GetMomentum();
   TVector3 Vcm = (KVParticle::C()/etot)*ptot;

   KVNucleus temp(ztot, atot);  //mass of nucleus includes mass excess
   temp.SetVelocity(Vcm);
   //"excitation energy" of resulting nucleus is given by bilan energetique
   Double_t estar = extot + lhs.E() + rhs.E() - temp.E();
   temp.SetExcitEnergy(estar);

   return temp;
}

//________________________________________________________________________________________

KVNucleus KVNucleus::operator-(const KVNucleus & rhs)
{
   //KVNucleus subtraction operator.
   //If the LHS is a compound nucleus and the RHS an emitted nucleus
   //(which may or may not be excited) then the result of the subtraction
   //is the residual nucleus, with recoil and residual excitation calculated
   //by conservation laws.

   KVNucleus & lhs = *this;
   Int_t zres = lhs.GetZ() - rhs.GetZ();
   Int_t ares = lhs.GetA() - ((KVNucleus &) rhs).GetA();
   Double_t exres = lhs.GetExcitEnergy() - rhs.GetExcitEnergy();
   TVector3 pres = lhs.GetMomentum() - rhs.GetMomentum();

   if (zres < 0 || ares < 0) {
      Warning("operator-(const KVNucleus &rhs)",
              "Cannot subtract nuclei, resulting Z=%d A=%d", zres, ares);
      KVNucleus temp;
      temp.SetZ(0);
      temp.SetA(0);
      temp.SetExcitEnergy(0.0);
      temp.SetEnergy(0.0);
      return temp;
   } else {
      KVNucleus temp(zres, ares);       //mass of nucleus includes mass excess
      temp.SetMomentum(pres);
      //"excitation energy" of residual nucleus is given by bilan energetique
      Double_t estar = exres + lhs.E() - (rhs.E() + temp.E());
      temp.SetExcitEnergy(estar);
      return temp;
   }
}

//________________________________________________________________________________________

KVNucleus & KVNucleus::operator+=(const KVNucleus & rhs)
{
   //KVNucleus addition and assignment operator.

   KVNucleus temp = (*this) + rhs;
   (*this) = temp;
   return *this;
}

//________________________________________________________________________________________

KVNucleus & KVNucleus::operator-=(const KVNucleus & rhs)
{
   //KVNucleus subtraction and assignment operator.

   KVNucleus temp = (*this) - rhs;
   (*this) = temp;
   return *this;
}

//________________________________________________________________________________________

Double_t KVNucleus::LiquidDrop_BrackGuet(UInt_t aa, UInt_t zz)
{
   //Liquid drop mass formula used for nuclei not in mass table (extrapolation).
   //Parameters are from Brack and Guet (copied from Simon code)

   Double_t A = (Double_t) aa;
   Double_t Z = (Double_t) zz;
   Double_t AVOL = 15.776;
   Double_t ASUR = -17.22;
   Double_t AC = -10.24;
   Double_t AZER = 8.;
   Double_t XJJ = -30.03;
   Double_t QQ = -35.4;
   Double_t C1 = -.737;
   Double_t C2 = 1.28;

   Double_t XNEU = A - Z;
   Double_t SI = (XNEU - Z) / A;
   Double_t X13 = TMath::Power(A, 1. / 3.);
   Double_t EE1 = C1 * Z * Z / X13;
   Double_t EE2 = C2 * Z * Z / A;
   Double_t AUX = 1. + (9. * XJJ / 4. / QQ / X13);
   Double_t EE3 = XJJ * A * SI * SI / AUX;
   Double_t EE4 =
       AVOL * A + ASUR * TMath::Power(A, 2. / 3.) + AC * X13 + AZER;
   Double_t TOTA = EE1 + EE2 + EE3 + EE4;
   return (939.55 * XNEU + 938.77 * Z - TOTA);
}

//________________________________________________________________________________________

void KVNucleus::SetExcitEnergy(Double_t e)
{
   //Set excitation energy. 

   fExx = e;
}

//_______________________________________________________________________________________

Int_t KVNucleus::Compare(const TObject * obj) const
{
   //For sorting lists of nuclei according to their Z
   //Largest Z appears first in list

   if (GetZ() > ((KVNucleus *) obj)->GetZ())
      return -1;
   else if (GetZ() < ((KVNucleus *) obj)->GetZ())
      return 1;
   else
      return 0;
}
//_______________________________________________________________________________________

TH2F* KVNucleus::GetKnownNucleiChart(KVString method)
{
	//Draw nuclei chart of tabulated nuclei and tagged as known in KaliVeda
	//The 2D histogram (AvsZ) has to be deleted by the user
	//Each content cell correponds to the method passed in argument of nucleus in MeV
	// Method Pattern has to be Double_t Method() or Double_t Method(obs = default value) in KVNucleus.h
TH2F* chart = new TH2F("nuclei_known_charts",method.Data(),
					121,-0.5,120.5,
					351,-0.5,350.5);
chart->SetXTitle("Atomic Number");
chart->SetYTitle("Mass Number");					

TMethodCall *mt = new TMethodCall();
mt->InitWithPrototype(this->IsA(),Form("%s",method.Data()),"");
if (! mt->IsValid()) { delete mt; return 0; }
delete mt;
KVNucleus* ntemp = new KVNucleus();
for (Int_t zz=0;zz<120;zz+=1){
	for (Int_t aa=0;aa<350;aa+=1){
		if (this->IsKnown(zz,aa)){
			mt = new TMethodCall();
			mt->InitWithPrototype(ntemp->IsA(),Form("%s",method.Data()),"");
			if (mt->ReturnType()==TMethodCall::kDouble){
				ntemp->SetZ(zz); ntemp->SetA(aa);
				Double_t ret; mt->Execute(ntemp,"",ret);
				chart->Fill(zz,aa,ret);
			}
			delete mt;
		}
	}
}
delete ntemp;
return chart;

}

//_______________________________________________________________________________________

Double_t KVNucleus::u(void)
{
   //Atomic mass unit in MeV
   //Reference: 2002 CODATA recommended values Reviews of Modern Physics 77, 1-107 (2005)
   return kAMU;
};
