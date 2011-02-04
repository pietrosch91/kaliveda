/***************************************************************************
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

KVIonRangeTable* KVMaterial::fIonRangeTable = 0x0;

//___________________________________________________________________________________
void KVMaterial::init()
{
   // Default initialisations.
   // No properties are set for the material (except standard temperature (19°C) and pressure (1 atm))
   // Default range table is generated if not already done.
   // By default it is the VEDALOSS table implemented in KVedaLoss.
   // You can change this by changing the value of environment variable KVMaterial.IonRangeTable.
   
   fELoss = 0;
   SetName("");
   SetTitle("");
   fNormToMat.SetXYZ(0,0,1);
   fAmasr = 0;
   fPressure = 1. * Units::atm;
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
   // Create material with given type and linear thickness in cm.
   
   init();
   SetMaterial(type);
   SetThickness(thick);
}

KVMaterial::KVMaterial(Double_t area_density, const Char_t * type)
{
   // Create material with given area density in g/cm**2 and given type
   
   init();
   SetMaterial(type);
   SetAreaDensity(area_density);
}

KVMaterial::KVMaterial(const Char_t * gas, const Double_t thick, const Double_t pressure, const Double_t temperature)
{
   // Create gaseous material with given type, linear thickness in cm, pressure in Torr,
   // and temperature in degrees C (default value 19°C).
   //
   // Examples
   // 15 cm of CF4 gas at 1 atm and 19°C :   KVMaterial("CF4", 15., 1.*Units::atm)
   // 50 mm of C3F8 at 30 mbar and 25°C :  KVMaterial("C3F8", 50.*Units::mm, 30.*Units::mbar, 25.)
      
   init();
   SetMaterial(gas);
   fPressure = pressure;
   fTemp = temperature;
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
   // Returns density of material in g/cm**3.
   // For a gas, density is calculated from current pressure & temperature according to ideal gas law

   if (GetActiveLayer())
      return GetActiveLayer()->GetDensity();
   fIonRangeTable->SetTemperatureAndPressure(GetType(), fTemp, fPressure);
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

void KVMaterial::SetThickness(Double_t t)
{
   // Set the linear thickness of the material in cm or use one of the
   // Units constants:
   //       SetThickness( 30.*Units::um );  set thickness to 30 microns

   if (GetActiveLayer()) {
      GetActiveLayer()->SetThickness(t);
      return;
   }
   // calculate area density
   fThick = t * GetDensity();
}

//___________________________________________________________________________________

Double_t KVMaterial::GetThickness() const
{
   // Returns the linear thickness of the material in cm.
   // Use Units to change units:
   //      mat.GetThickness()/Units::um ;   in microns
	
   if (GetActiveLayer())
      return GetActiveLayer()->GetThickness();
   return fThick/GetDensity();
}

//___________________________________________________________________________________

Double_t KVMaterial::GetEffectiveThickness(TVector3 & norm,
                                           TVector3 & direction)
{
   // Calculate effective thickness of absorber as 'seen' in 'direction', taking into
   // account the arbitrary orientation of the 'norm' normal to the material's surface

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
   if (fIonRangeTable->IsMaterialGas(GetType()))
      cout << " Pressure " << GetPressure() << " torr" << endl;
   cout << " Thickness " << KVMaterial::GetThickness() << " cm" << endl;
   cout << "-----------------------------------------------" << endl;
   cout << " Z = " << GetZ() << " atomic mass = " << GetMass() << endl;
   cout << " Density = " << GetDensity() << endl;
   cout << "-----------------------------------------------" << endl;
}

//______________________________________________________________________________________//

Double_t KVMaterial::GetELostByParticle(KVNucleus * kvn, TVector3 * norm)
{
   // Method to simulate passage of an ion through a given thickness of material.
   // The energy loss of the particle (in MeV) in the material is returned by the method.
   //
   // If the optional argument 'norm' is given, it is supposed to be a vector
   // normal to the material, oriented from the origin towards the material.
   // In this case the effective thickness of the material 'seen' by the particle
   // depending on its direction of motion is used for the calculation.


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

   if(Z<1) return 0.;
   Double_t E_loss =
      fIonRangeTable->GetDeltaEOfIon(GetType(), Z, A, Einc, fThick, fAmasr, fTemp, fPressure);

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

   if(Z<1) return 0.;
   
   Double_t E_res =
      fIonRangeTable->GetEResOfIon(GetType(), Z, A, Einc, fThick, fAmasr, fTemp, fPressure);

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
   ((KVMaterial &) obj).SetPressure(GetPressure());
   ((KVMaterial &) obj).SetTemperature(GetTemperature());
   ((KVMaterial &) obj).SetThickness(GetThickness());
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
