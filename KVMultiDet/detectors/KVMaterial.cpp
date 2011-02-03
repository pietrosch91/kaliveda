/***************************************************************************
$Id: KVMaterial.cpp,v 1.55 2009/04/01 09:28:02 franklan Exp $
                          kvmaterial.cpp  -  description
                             -------------------
    begin                : Thu May 16 2002
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

#include "Riostream.h"
#include "KVMaterial.h"
#include "KVTelescope.h"
#include "KVNucleus.h"
#include "TMath.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TEnv.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoManager.h"

ClassImp(KVMaterial);

///////////////////////////////////////////////////////////////////////////////
//                       KVMaterial
//
//The KVMaterial class encapsulates the routines of R. Dayras et al for calculating
//the energy lost by charged particles (nuclei) in different absorbers,
//formerly known as the "vedaloss" routines.
//
//To create an absorber, use the ctor specifying a type of material and optionally
//the thickness of the absorber.
//     KVMaterial mat1("Si",150.0); //150 microns of silicon
//     KVMaterial mat2("Au",0.8); //800 micrograms/cm**2 of gold
//The available materials are defined in the $KVROOT/KVFiles/kvloss.data file. In order to get
//a list of the currently defined materials, use KVMaterial::GetListOfMaterials().
//
//Depending on the type of material, the default units of "thickness" are different:
//     fType = Si, Myl - thickness in micrometres
//     fType = NE102, CsI - thickness in centimetres
//     fType = C3F8, CF4 - pressure in mbar
//     fType = Au, Sn, U, Nb, Ni, C, Ta - thickness in mg/cm2
//
//By default, the naturally-occuring atomic mass is used for the elements. To use
//some rarer isotope, use the SetMass() method with the appropriate mass (not
//valid for compounds).
//
///////////////////////////////////////////////////////////////////////////////

Char_t KVMaterial::kUnits[][10] = {
   "mbar",
   "microns",
   "mg/cm2",
   "cm",
   "Torr"
};

KVIonRangeTable* KVMaterial::fIonRangeTable = 0x0;

//___________________________________________________________________________________
void KVMaterial::init()
{
   // Default initialisations.
   // No properties are set for the material (except default gas temperature: 19°C)
   // Default range table is generated if not already done.
   // By default it is the VEDALOSS table implemented in KVedaLoss.
   // You can change this by changing the value of environment variable KVMaterial.IonRangeTable.
   
   fELoss = 0;
   SetName("");
   SetTitle("");
   ELoss = ERes = 0;
   fNormToMat.SetXYZ(0,0,1);
   fAmasr = 0;
   fPressure = 0;
   fTemp = 19.0;
   // create default range table singleton if not already done
   if(!fIonRangeTable) {
      fIonRangeTable = KVIonRangeTable::GetRangeTable( gEnv->GetValue("KVMaterial.IonRangeTable", "VEDALOSS") );
   }
}

//
KVMaterial::KVMaterial()
{
   //default ctor
   init();
}

//__________________________________________________________________________________
KVMaterial::KVMaterial(const Char_t * type, const Double_t thick)
{
   //Initialise absorber with given type of material and thickness (default = 0.0)
   init();
   SetMaterial(type);
   SetThickness(thick);
}

//
KVMaterial::KVMaterial(const KVMaterial & obj)
{
   //Copy ctor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   obj.Copy(*this);
#else
   ((KVMaterial &) obj).Copy(*this);
#endif
}

//___________________________________________________________________________________
void KVMaterial::SetMaterial(const Char_t * mat_type)
{
   //Intialise material of a given type.
   //The material must exist in the currently used range tables (fIonRangeTable).
   //For materials which are elements of the periodic table you can specify
   //the isotope such as "64Ni", "13C", "natSn", etc. etc.

   init();
   //are we dealing with an isotope ?
   Char_t type[10];
   Int_t iso_mass = 0;
   if (sscanf(mat_type, "nat%s", type) != 1) {
      if (sscanf(mat_type, "%d%s", &iso_mass, type) != 2) {
         strcpy(type, mat_type);
      }
   }
   if(iso_mass) SetMass( iso_mass );
   SetType(type);
   if(!fIonRangeTable->IsMaterialKnown(type))
      Warning("SetMaterial", 
      "Called for material %s which is unknown in current range table %s. Energy loss & range calculations impossible.",
      type, fIonRangeTable->GetName());
   else {
      SetType( fIonRangeTable->GetMaterialName(type) );
      SetName(mat_type);
    }
}

//___________________________________________________________________________________
KVMaterial::~KVMaterial()
{
   //Destructor
}

void KVMaterial::SetMass(Double_t a)
{
   //Set the atomic mass of the material - use if you want to change the default naturally
   //occuring mass for some rarer isotope.
   
   if (GetActiveLayer()) {
      GetActiveLayer()->SetMass(a);
      return;
   }
   fAmasr = a;
}

//___________________________________________________________________________________
Double_t KVMaterial::GetMass() const
{
   //Returns atomic mass of material. Will be isotopic mass if set.
   
   if (GetActiveLayer())
      return GetActiveLayer()->GetMass();
   return (fAmasr ? fAmasr : fIonRangeTable->GetAtomicMass(GetType()));
}

//___________________________________________________________________________________
Bool_t KVMaterial::IsIsotopic() const
{
   //Returns kTRUE if a specific isotope has been chosen for the material
   //using SetMass().
   //e.g. for "119Sn" this method returns kTRUE
   //e.g. for "natSn" this method returns kFALSE
   if (GetActiveLayer())
      return GetActiveLayer()->IsIsotopic();
   return (fAmasr!=0);
}

//___________________________________________________________________________________
Bool_t KVMaterial::IsNat() const
{
   //Returns kFALSE if a specific isotope has been chosen for the material
   //using SetMass().
   //e.g. for "119Sn" this method returns kFALSE
   //e.g. for "natSn" this method returns kTRUE
   if (GetActiveLayer())
      return GetActiveLayer()->IsNat();
   return (!IsIsotopic());
}

//___________________________________________________________________________________
Double_t KVMaterial::GetZ() const
{
   //Returns atomic number of material.
   if (GetActiveLayer())
      return GetActiveLayer()->GetZ();
   return fIonRangeTable->GetZ(GetType());
}

//___________________________________________________________________________________

Double_t KVMaterial::GetDensity() const
{
   //Returns density of material in g/cm**3.
   //For a gas, density is calculated from current pressure & temperature according to ideal gas law

   if (GetActiveLayer())
      return GetActiveLayer()->GetDensity();
   if(fIonRangeTable->IsMaterialGas(GetType())){
      Double_t P = fPressure;
      if(fUnits == kMBAR) P*=0.750061683; // convert to Torr
      return fIonRangeTable->GetGasDensity(GetType(), fTemp, fPressure);
   }
   return fIonRangeTable->GetDensity(GetType());
}

//___________________________________________________________________________________

void KVMaterial::SetPressure(Double_t p)
{
   // Set the pressure of a gaseous material.

   if (GetActiveLayer()) {
      GetActiveLayer()->SetPressure(p);
      return;
   }
   fPressure = p;
}


//___________________________________________________________________________________

Double_t KVMaterial::GetPressure() const
{
   // Returns the pressure of a gas.
   //If the material is not a gas, value is zero.
   //The units depend on fUnits

   if (GetActiveLayer())
      return GetActiveLayer()->GetPressure();
   return fPressure;
}

//___________________________________________________________________________________

void KVMaterial::SetTemperature(Double_t t)
{
   //Set temperature of material.
   //The units are: degrees celsius

   if (GetActiveLayer()) {
      GetActiveLayer()->SetTemperature(t);
      return;
   }

   fTemp = t;
}


//___________________________________________________________________________________

Double_t KVMaterial::GetTemperature() const
{
   //Returns temperature of material.
   //The units are: degrees celsius

   if (GetActiveLayer())
      return GetActiveLayer()->GetTemperature();
   return fTemp;
}

//___________________________________________________________________________________

void KVMaterial::SetUnits(UInt_t u)
{
   //Set units for 'thickness' of material.
   //WARNING: changing the default units will alter how the 'thickness'
   //is used in energy loss calculations!
   //WARNING2: we do not perform a conversion of any existing value
   //for the 'thickness' of the material, the existing value will be used
   //with the new units

   if (GetActiveLayer()) {
      GetActiveLayer()->SetUnits(u);
      return;
   }

   fUnits = u;
}


//___________________________________________________________________________________

UInt_t KVMaterial::GetUnits() const
{
   //Returns units for 'thickness' of material.
   //Can be one of KVMaterial::kMBAR, KVMaterial::kMGCM2, KVMaterial::kTORR,
   //KVMaterial::kMICRON, KVMaterial::kCM.

   if (GetActiveLayer())
      return GetActiveLayer()->GetUnits();
   return fUnits;
}

//___________________________________________________________________________________

void KVMaterial::SetThickness(Double_t t)
{
   //Set the thickness of the material.
   //
   //The meaning and units of "thickness" depend on the material.
   //For a gas it is the depth of the gas cell in millimetres.
   //For silicon and mylar it is the thickness in micrometres.
   //For CsI and plastic scintillators it is the length/thickness in centimetres.
   //For metallic elements (such as for targets) it is the areal density
   //in milligrams per cm^2.

   if (GetActiveLayer()) {
      GetActiveLayer()->SetThickness(t);
      return;
   }
   fThick = t;
}

//___________________________________________________________________________________

Double_t KVMaterial::GetThickness() const
{
   //Returns the "thickness" of the material - units depend on the material type
   //For a gas it is the depth of the gas cell in millimetres.
   //For silicon and mylar it is the thickness in micrometres.
   //For CsI and plastic scintillators it is the length/thickness in centimetres.
   //For metallic elements (such as for targets) it is the areal density
   //in milligrams per cm^2.
	
   if (GetActiveLayer())
      return GetActiveLayer()->GetThickness();
   return fThick;
}

//___________________________________________________________________________________

Double_t KVMaterial::GetThicknessInCM() const
{
   //Returns the thickness of the material in CENTIMETRES
	
   if (GetActiveLayer())
      return GetActiveLayer()->GetThicknessInCM();
	Double_t t = (Double_t)fThick;
   if (fUnits == kMBAR || fUnits == kTORR)
      return (t/10.);
	if(fUnits == kMICRONS)
		return (t/10000.);
	else if(fUnits == kMGCM2)
		return (t/fIonRangeTable->GetDensity(GetType()));
   return fThick;
}

//___________________________________________________________________________________

Double_t KVMaterial::GetEffectiveThickness(TVector3 & norm,
                                           TVector3 & direction)
{
   //Calculate effective thickness of absorber as 'seen' in 'direction', taking into
   //account the arbitrary orientation of the 'norm' normal to the material's surface

   TVector3 n = norm.Unit();
   TVector3 d = direction.Unit();
   //absolute value of scalar product, in case direction is opposite to normal
   Double_t prod = TMath::Abs(n * d);
   return GetThickness() / TMath::Max(prod, 1.e-03);
}

//___________________________________________________________________________________
void KVMaterial::Print(Option_t * option) const
{
   //Show information on this material
   cout << "KVMaterial: " << GetName() << " (" << GetType() << ")" << endl;
   if (fUnits == kMBAR || fUnits == kTORR)
      cout << " Thickness " << KVMaterial::
          GetThickness() << " mm, pressure " << GetPressure() << " " <<
          GetThicknessUnits() << endl;
   else
      cout << " Thickness " << KVMaterial::
          GetThickness() << " " << GetThicknessUnits() << endl;
   cout << "-----------------------------------------------" << endl;
   cout << " Z = " << GetZ() << " atomic mass = " << GetMass() << endl;
   cout << " Density = " << GetDensity() << endl;
   cout << "-----------------------------------------------" << endl;
}

//_______________________________________________________________________________________
Double_t ELossSaclay(Double_t * x, Double_t * par)
{
   //Calculates energy loss (DE) as a function of incident energy x[0]
   //Parameters:
   // par[0] - par[14] : Saclay range tables parameters
   //      par[0] = Z of nucleus
   // par[15] = A of nucleus
   // par[16] = minimum incident energy in MeV for valid calculation
   // par[17] = maximum incident energy in MeV for valid calculation
   // par[18] = ratio of isotopic to natural mass of material, log(fAmasr/fAmat)

   //energy loss = incident energy - residual energy
   return x[0] - EResSaclay(x, par);
}

//_______________________________________________________________________________________
Double_t EResSaclay(Double_t * x, Double_t * par)
{
   //Calculates residual energy after passage through material
   //as a function of incident energy x[0]
   //Parameters:
   // par[0] - par[14] : Saclay range tables parameters
   //      par[0] = Z of nucleus
   // par[15] = A of nucleus
   // par[16] = minimum incident energy in MeV for valid calculation
   // par[17] = maximum incident energy in MeV for valid calculation
   // par[18] = ratio of isotopic to natural mass of material, log(fAmasr/fAmat)

   //check incident energy is valid
   //incident energy less than minimum valid energy - residual energy=0 (particle stops)
   //if (x[0] < par[16])
   //   return 0.0;
   //incident energy greater than maximum valid energy - residual E = incident E
   //if (x[0] > par[17])
   //   return x[0];

   // set up polynomial
   Double_t x1 = TMath::Log(0.1);
   Double_t x2 = TMath::Log(0.2);
   Double_t ran = 0.0;
   for (register int j = 2; j < 7; j++)
      ran += par[j + 1] * TMath::Power(x2, (Double_t) (j - 1));
   ran += par[2];
   Double_t y2 = ran;
   ran = 0.0;
   for (register int jj = 2; jj < 7; jj++)
      ran += par[jj + 1] * TMath::Power(x1, (Double_t) (jj - 1));
   ran += par[2];
   Double_t y1 = ran;
   Double_t adm = (y2 - y1) / (x2 - x1);
   Double_t adn = (y1 - adm * x1);
   Double_t arm = y1;
   // calculate energy loss
   Double_t eps = x[0] / par[15];       //energy in MeV/nucleon
   Double_t dleps = TMath::Log(eps);
   Double_t riso = TMath::Log(par[15] / par[1]) + par[18];

   if (eps < 0.1)
      ran = adm * dleps + adn;
   else {
      ran = 0.0;
      for (register int j = 2; j < 7; j++)
         ran += par[j + 1] * TMath::Power(dleps, (Double_t) (j - 1));
      ran += par[2];
   }
   ran += riso;

   Double_t range = TMath::Exp(ran);
   range -= par[14];
   if (range <= 0.005) {        // particle stopped in material, gives up all energy
      return 0.0;               //residual energy = 0
   }

   Double_t ranx = TMath::Log(range);
   Double_t ranx1 = ranx - riso;
   Double_t depsx;
   if (ranx1 < arm)
      depsx = (ranx1 - adn) / adm;
   else {
      depsx = 0.0;
      for (register int j = 2; j < 7; j++)
         depsx += par[j + 7] * TMath::Power(ranx1, (Double_t) (j - 1));
      depsx += par[8];
   }

   const Double_t PERC = 0.02;
   
   Double_t eps1 = depsx + TMath::Log(1 - PERC);
   Double_t eps2 = depsx + TMath::Log(1 + PERC);
   Double_t rap = TMath::Log((1 + PERC) / (1 - PERC));

   Double_t rn1 = 0.0;
   if (TMath::Exp(eps1) < 0.1)
      rn1 = adm * eps1 + adn;
   else {
      for (register int j = 1; j < 7; j++)
         rn1 += par[j + 1] * TMath::Power(eps1, (Double_t) (j - 1));
   }
   Double_t rn2 = 0.0;
   if (TMath::Exp(eps2) < 0.1)
      rn2 = adm * eps2 + adn;
   else {
      for (register int j = 1; j < 7; j++)
         rn2 += par[j + 1] * TMath::Power(eps2, (Double_t) (j - 1));
   }

   Double_t epres = eps1 + (rap / (rn2 - rn1)) * (ranx1 - rn1);
   epres = TMath::Exp(epres);
   Double_t eres = par[15] * epres;
   //make sure residual energy is <= incident energy
   //following original code, the minimum energy loss is 0.005 MeV

   Double_t eloss = x[0] - eres;
   if (eloss < 0.005 && eres > eloss)
      eres = x[0];
   return eres;
}

//_______________________________________________________________________________________//

void KVMaterial::GetELossParams(Int_t Z, Int_t A, Double_t * par)
{
   // get coefficients associated with this nucleus Z 
   if(Z<1 || Z>ZMAX_VEDALOSS) return;
     
   for (int i = 0; i < 14; i++) {

      par[i] = fCoeff[(Z - 1)][i];

   }
   fAmasr = (fAmasr ? fAmasr : fAmat);
   // calculate "thickness"
   //for isotopic materials, the density used in the conversion
   //from microns or cm to mg/cm2 is modified according to the
   //chosen isotopic mass, fAmasr
   switch (fUnits) {
   case kMICRONS:
      par[14] = 0.1 * fThick * fDens * (fAmasr / fAmat);
      break;
   case kMGCM2:
      par[14] = fThick;
      break;
   case kCM:
      par[14] = fThick * fDens * 1000. * (fAmasr / fAmat);
      break;
      //gases: calculate density*thickness of cell
   case kMBAR:
   case kTORR:
      par[14] = fGasThick * GetDensity();
      break;
   }
   par[15] = A;
   par[16] = GetEminVedaloss(Z) * A;
   par[17] = GetEmaxVedaloss(Z) * A;
   if( fAmasr == fAmat ) par[18] = 0.;
   else par[18] = TMath::Log(fAmasr / fAmat);
}

//______________________________________________________________________________________________//
void KVMaterial::SetELossParams(Int_t Z, Int_t A)
{
   //Initialise energy loss coefficients for this material and a given incident nucleus (Z,A)
   static Double_t par[19];
   GetELossParams(Z, A, par);
   if (!ELoss) {
      //search in list of functions for one corresponding to this material
      //the name of the required function is ELoss
      ELoss = (TF1 *) gROOT->GetListOfFunctions()->FindObject("ELoss");
      if (!ELoss)
         ELoss = new TF1("ELoss", ELossSaclay, 0.1, 5000., 19);
   }
   ELoss->SetParameters(par);
}

//______________________________________________________________________________________________//
void KVMaterial::SetEResParams(Int_t Z, Int_t A)
{
   //Initialise coefficients for residual energy function for this material and a given incident nucleus (Z,A)

   static Double_t par[19];
   GetELossParams(Z, A, par);
   if (!ERes) {
      //search in list of functions for one corresponding to this material
      //the name of the required function is ERes
      ERes = (TF1 *) gROOT->GetListOfFunctions()->FindObject("ERes");
      if (!ERes)
         ERes = new TF1("ERes", EResSaclay, 0.1, 5000., 19);
   }
   ERes->SetParameters(par);
}

//______________________________________________________________________________________//

Double_t KVMaterial::GetELostByParticle(KVNucleus * kvn, TVector3 * norm)
{
   //*** Saclay VEDALOSS library: translation of functions PARAM, GET_PARAM and ELOSS.
   //
   //Method to simulate passage of a charged particle (ion) through a given thickness of
   //material. The energy loss of the particle in the material is returned by the method.
   //The thickness is given in cm (for CsI crystals, NE102), in mbar (for C3F8), in micrometres
   //(for Silicon, Mylar), or mg/cm2 (all others).
   //
   //If the optional argument 'norm' is given, it is supposed to be a vector
   //normal to the material, oriented from the origin towards the material.
   //In this case the effective thickness of the material 'seen' by the particle
   //depending on its direction of motion is used for the calculation.


   if (norm) {
      Double_t thick = GetThickness();
      TVector3 p = kvn->GetMomentum();
      Double_t e = GetEffectiveThickness((*norm), p);
#ifdef DBG_TRGT
      cout << "Thickness used in energy loss calculation=" << e << endl;
#endif
      SetThickness(e);
      Double_t E_loss = GetDeltaE(kvn->GetZ(), kvn->GetA(), kvn->GetKE());
      SetThickness(thick);
      return E_loss;
   }
   Double_t E_loss = GetDeltaE(kvn->GetZ(), kvn->GetA(), kvn->GetKE());
   return E_loss;
}

//______________________________________________________________________________________//

Double_t KVMaterial::GetParticleEIncFromERes(KVNucleus * kvn, TVector3 * norm)
{
   //Calculate the energy of particle 'kvn' before its passage through the absorber,
   //based on the current kinetic energy, Z & A of nucleus 'kvn'.
   //
   //If the optional argument 'norm' is given, it is supposed to be a vector
   //normal to the material, oriented from the origin towards the material.
   //In this case the effective thickness of the material 'seen' by the particle
   //depending on its direction of motion is used for the calculation.

   if (norm) {
      Double_t thick = GetThickness();
      TVector3 p = kvn->GetMomentum();
      Double_t e = GetEffectiveThickness((*norm), p);
      SetThickness(e);
      Double_t E_inc = GetIncidentEnergyFromERes(kvn->GetZ(), kvn->GetA(), kvn->GetKE());
      SetThickness(thick);
      return E_inc;
   }
   Double_t E_inc = GetIncidentEnergyFromERes(kvn->GetZ(), kvn->GetA(), kvn->GetKE());
   return E_inc;
}

//______________________________________________________________________________________//

Double_t KVMaterial::GetDeltaE(Int_t Z, Int_t A, Double_t Einc)
{
   // Calculate energy loss in absorber for incident nucleus (Z,A)
   // with kinetic energy Einc (MeV)
   // For a KVDetector, the energy loss of the ACTIVE layer only is returned.

   if(Z<1 || Z>ZMAX_VEDALOSS) return 0.;
   
   SetELossParams(Z, A);

   Double_t E_loss = GetELossFunction()->Eval(Einc);

   return E_loss;
}

//______________________________________________________________________________________//

Double_t KVMaterial::GetDeltaEFromERes(Int_t Z, Int_t A, Double_t Eres)
{
   // Calculate energy loss in absorber for nucleus (Z,A)
   // having a residual kinetic energy Eres (MeV) after the absorber
   // For a KVDetector, the energy loss of the ACTIVE layer only is returned.

   if(Z<1 || Z>ZMAX_VEDALOSS) return 0.;
   
   Double_t EINC = -1.;
   SetEResParams(Z, A);
   EINC = GetEResFunction()->GetX(Eres, GetEminVedaloss(Z) * A,
                               GetEmaxVedaloss(Z) * A);
   
   // Calculate energy loss in absorber for incident nucleus (Z,A)
   // with kinetic energy Einc (MeV)
   SetELossParams(Z, A);
   Double_t E_loss = GetELossFunction()->Eval(EINC);

   return E_loss;
}

//______________________________________________________________________________________//

Double_t KVMaterial::GetEResFromDeltaE(Int_t Z, Int_t A, Double_t dE,
                                       enum SolType type)
{
   // Calculate residual kinetic energy Eres (MeV) after the absorber from
   // energy loss in absorber for nucleus (Z,A). If dE is not given, the energy
   // loss in this absorber is used.
   // For a KVDetector, dE = energy loss in the ACTIVE layer, not the total
   // energy lost in crossing the detector (i.e. the incident energy of the
   // particle is NOT given by dE + Eres: due to losses in the inactive layers
   // of the detector, Einc > dE + Eres).
   //
   //By default the solution corresponding to the highest incident energy is returned
   //This is the solution found for Einc greater than the maximum of the dE(Einc) curve.
   //If you want the low energy solution (i.e. below Bragg peak for gas detector)
   //set SolType = KVMaterial::kEmin.
   //
   //If the given energy loss in the absorber is greater than the maximum theoretical dE
   //(dE > GetBraggDE) then we return the residual energy corresponding to the
   //maximum dE.
   
   Double_t EINC = GetIncidentEnergy(Z,A,dE,type);
   SetEResParams(Z, A);
   return GetEResFunction()->Eval(EINC);
}

//__________________________________________________________________________________________

Double_t KVMaterial::GetIncidentEnergy(Int_t Z, Int_t A, Double_t delta_e,
                                       enum SolType type)
{
   //Calculate incident energy of nucleus (Z,A) corresponding to the energy loss
   //in this absorber. If delta_e is not given, the energy loss in this absorber is used.
   //
   //By default the solution corresponding to the highest incident energy is returned
   //This is the solution found for Einc greater than the maximum of the dE(Einc) curve.
   //If you want the low energy solution (i.e. below Bragg peak for gas detector)
   //set SolType = KVMaterial::kEmin.
   //
   //If the given energy loss in the absorber is greater than the maximum theoretical dE
   //(dE > GetBraggDE) then we return the incident energy corresponding to the maximum,
   //GetBraggE.

   if(Z<1 || Z>ZMAX_VEDALOSS) return 0.;
   
   Double_t EINC = -1.;
   Double_t DE = (delta_e > 0 ? delta_e : GetEnergyLoss());

   SetELossParams(Z, A);
   //energy loss in the absorber is greater than the maximum theoretical dE ?
   Double_t braggDE = GetELossFunction()->GetMaximum(GetEminVedaloss(Z) * A,
                                         GetEmaxVedaloss(Z) * A);
   if (DE > braggDE)
      return GetELossFunction()->GetMaximumX(GetEminVedaloss(Z) * A,
                                          GetEmaxVedaloss(Z) * A);

   // Emax = GetBraggE(Z,A)
   Double_t Emax = GetELossFunction()->GetMaximumX(GetEminVedaloss(Z) * A,
                                          GetEmaxVedaloss(Z) * A);
   switch (type) {
      case kEmin:
         EINC = GetELossFunction()->GetX(DE, GetEminVedaloss(Z) * A, Emax);
         break;

      case kEmax:
         EINC = GetELossFunction()->GetX(DE, Emax, GetEmaxVedaloss(Z) * A);
         break;
   }
   
   return EINC;
}

//______________________________________________________________________________________//

Double_t KVMaterial::GetERes(Int_t Z, Int_t A, Double_t Einc)
{
   // Calculate residual energy after absorber for incident nucleus (Z,A)
   // with kinetic energy Einc (MeV)

   if(Z<1 || Z>ZMAX_VEDALOSS) return 0.;
   
   SetEResParams(Z, A);

   Double_t E_res = GetEResFunction()->Eval(Einc);

   return E_res;
}

//___________________________________________________________________________________________________

void KVMaterial::DetectParticle(KVNucleus * kvp, TVector3 * norm)
{
   //The energy loss of a charged particle traversing the absorber is calculated,
   //and the particle is slowed down.
   //
   //If the optional argument 'norm' is given, it is supposed to be a unit vector
   //normal to the material, oriented from the origin towards the material.
   //In this case the effective thickness of the material 'seen' by the particle
   //depending on its direction of motion is used for the calculation.

   //set flag to say that particle has been slowed down
   kvp->SetIsDetected();
   //If this is the first absorber that the particle crosses, we set a "reminder" of its
   //initial energy
   if (!kvp->GetPInitial())
      kvp->SetE0();

#ifdef DBG_TRGT
   cout << "detectparticle in material " << GetType() << " of thickness "
       << GetThickness() << endl;
#endif
   Double_t el = GetELostByParticle(kvp, norm);
   // set particle residual energy
   Double_t Eres = kvp->GetKE() - el;
   kvp->SetKE(Eres);
   // add to total of energy losses in absorber
   fELoss += el;
}

//__________________________________________________________________________________________

void KVMaterial::Clear(Option_t * opt)
{
   //Reset absorber - set energy lost by particles to zero
   fELoss = 0.0;
}

//__________________________________________________________________________________________

const Char_t *KVMaterial::GetThicknessUnits() const
{
   //Returns a string with the name of the units used to measure "thickness"
   //for this material type
   if (GetActiveLayer())
      return GetActiveLayer()->GetThicknessUnits();
   if (fUnits < 10)
      return kUnits[GetUnits()];
   else
      return "(undefined)";
}

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVMaterial::Copy(TObject & obj) const
#else
void KVMaterial::Copy(TObject & obj)
#endif
{
   //Copy this to obj
   KVBase::Copy(obj);
   ((KVMaterial &) obj).SetMaterial(GetType());
   ((KVMaterial &) obj).SetMass(GetMass());
   ((KVMaterial &) obj).SetThickness(GetThickness());
   ((KVMaterial &) obj).SetPressure(GetPressure());
   ((KVMaterial &) obj).SetTemperature(GetTemperature());
}

//__________________________________________________________________________________________

Double_t KVMaterial::GetIncidentEnergyFromERes(Int_t Z, Int_t A,
                                               Double_t Eres)
{
   //Get incident energy from residual energy
   if(Z<1 || Z>ZMAX_VEDALOSS) return 0.;
   
   Double_t EINC = -1.;
   SetEResParams(Z, A);
   EINC =
       GetEResFunction()->GetX(Eres, GetEminVedaloss(Z) * A,
                               GetEmaxVedaloss(Z) * A);
   return EINC;
}

//__________________________________________________________________________________________

Double_t KVMaterial::GetBraggE(Int_t Z, Int_t A)
{
   //Incident energy for which the DE(E) curve has a maximum
   if(Z<1 || Z>ZMAX_VEDALOSS) return 0.;
   
   SetELossParams(Z, A);
   return GetELossFunction()->GetMaximumX(GetEminVedaloss(Z) * A,
                                          GetEmaxVedaloss(Z) * A);
}

//__________________________________________________________________________________________

Double_t KVMaterial::GetBraggDE(Int_t Z, Int_t A)
{
   //The maximum energy loss of this particle
   //(corresponding to incident energy GetBraggE(Z,A))
   if(Z<1 || Z>ZMAX_VEDALOSS) return 0.;
   
   SetELossParams(Z, A);
   return GetELossFunction()->GetMaximum(GetEminVedaloss(Z) * A,
                                         GetEmaxVedaloss(Z) * A);
}

//__________________________________________________________________________________________

TGeoMedium* KVMaterial::GetGeoMedium(const Char_t* med_name)
{
	// By default, return pointer to TGeoMedium corresponding to this KVMaterial.
	// If argument "med_name" is given and corresponds to the name of an already existing
	// medium, we return a pointer to this medium, or 0x0 if it does not exist.
	// med_name = "Vacuum" is a special case: if the "Vacuum" does not exist, we create it.
	//
	// Instance of geometry manager class TGeoManager must be created before calling this
	// method, otherwise 0x0 will be returned.
	// If the required TGeoMedium is not already available in the TGeoManager, we create
	// a new TGeoMedium corresponding to the properties of this KVMaterial.
	// The name of the TGeoMedium (and associated TGeoMaterial) is the name of the KVMaterial.
	
	if( !gGeoManager ) return NULL;
	
	if( strcmp(med_name,"") ){
		TGeoMedium* gmed = gGeoManager->GetMedium( med_name );
		if( gmed ) return gmed;
		else if( !strcmp(med_name, "Vacuum") ){
			// create material
			TGeoMaterial *gmat = new TGeoMaterial("Vacuum",0,0,0 );
			gmed = new TGeoMedium( "Vacuum", 0, gmat );
			return gmed;
		}
		return NULL;
	}
	
	// if object is a KVDetector, we return medium corresponding to the active layer
   if (GetActiveLayer()) return GetActiveLayer()->GetGeoMedium();
	
	TGeoMedium* gmed = gGeoManager->GetMedium( GetName() );
	
	if( gmed ) return gmed;
		
	TGeoMaterial *gmat = gGeoManager->GetMaterial( GetName() );
	
	if( !gmat ){
		// create material
		gmat = new TGeoMaterial( GetName(), GetMass(), GetZ(), GetDensity() );
		gmat->SetPressure( GetPressure() );
		gmat->SetTemperature( GetTemperature() );
	}
	
	// create medium
	static Int_t numed = 1; // static counter variable used to number media
	gmed = new TGeoMedium( GetName(), numed, gmat );
	numed+=1;
	
	return gmed;
}


//______________________________________________________________________________

void KVMaterial::Streamer(TBuffer &R__b)
{
   // Stream an object of class KVMaterial.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      R__b.ReadClassBuffer(KVMaterial::Class(),this,R__v,R__s,R__c);
      if (R__v < 5){
			// correct densities of solids written with class version < 5
			// this is to fix launchpad bug#446163
			// it affects materials with fUnits = kMICRONS or kCM
			// if the density we read is less than that we find
			// for a new instanciation of the same type of material,
			// we use the current value.
			if(fUnits==kMICRONS || fUnits==kCM){
				KVMaterial tmp(GetType());
				if(GetDensity() < tmp.GetDensity()) SetDensity(tmp.GetDensity());
			}
		}
   } else {
      R__b.WriteClassBuffer(KVMaterial::Class(),this);
   }
}
