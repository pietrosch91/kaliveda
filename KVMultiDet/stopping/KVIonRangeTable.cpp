//Created by KVClassFactory on Thu Feb  3 10:04:41 2011
//Author: frankland,,,,

#include "KVIonRangeTable.h"
#include "KVIonRangeTableMaterial.h"
#include <TPluginManager.h>
#include <TError.h>
#include "TGeoManager.h"
#include "KVEvent.h"

#define FIND_MAT_AND_EXEC(method,defval) \
    KVIonRangeTableMaterial* M = GetMaterial(mat); \
    if(M) return M->method; \
    return defval

ClassImp(KVIonRangeTable)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIonRangeTable</h2>
<h4>Abstract base class for calculation of range & energy loss of charged particles in matter</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIonRangeTable::KVIonRangeTable(const Char_t* name, const Char_t* title)
   : KVBase(name,title)
{
   // Default constructor
}

KVIonRangeTable::~KVIonRangeTable()
{
   // Destructor
}

KVIonRangeTable* KVIonRangeTable::GetRangeTable(const Char_t* name)
{
   // Generates an instance of the KVIonRangeTable plugin class corresponding to given name.

   TPluginHandler *ph;
   //check and load plugin library
   if (!(ph = LoadPlugin("KVIonRangeTable", name))) {
      ::Error("KVIonRangeTable::GetRangeTable", "No plugin for KVIonRangeTable with name=%s found in .kvrootrc", name);
      return 0;
   }
   KVIonRangeTable *irt = (KVIonRangeTable *) ph->ExecPlugin(0);
   return irt;
}

//________________________________________________________________________________//

Double_t KVIonRangeTable::GetAtomicMass(const Char_t* material)
{
   // Return atomic mass of a material in the range table.
   // "material" can be either the type or the name of the material.
   // Prints a warning and returns 0 if material is unknown.

   KVIonRangeTableMaterial* M = GetMaterial(material);
   if (!M) {
      Warning("GetAtomicMass", "Material %s is unknown. Returned mass = 0.", material);
      return 0.0;
   }
   return M->GetMass();
}

//________________________________________________________________________________//

Double_t KVIonRangeTable::GetZ(const Char_t* material)
{
   // Return atomic number of a material in the range table.
   // "material" can be either the type or the name of the material.
   // Prints a warning and returns 0 if material is unknown.

   KVIonRangeTableMaterial* M = GetMaterial(material);
   if (!M) {
      Warning("GetZ", "Material %s is unknown. Returned Z = 0.", material);
      return 0.0;
   }
   return M->GetZ();
}

//________________________________________________________________________________//

Bool_t KVIonRangeTable::IsMaterialKnown(const Char_t* material)
{
   // Returns kTRUE if material of given name or type is in range table.
   KVIonRangeTableMaterial* M = GetMaterial(material);
   return (M != 0x0);
}

//________________________________________________________________________________//

Bool_t KVIonRangeTable::IsMaterialGas(const Char_t* mat)
{
   // Returns kTRUE if material of given name or type is gaseous.
   FIND_MAT_AND_EXEC(IsGas(),kFALSE);
}

//________________________________________________________________________________//

const Char_t* KVIonRangeTable::GetMaterialName(const Char_t* mat)
{
   // Return name of material of given type or name if it is in range tables
   FIND_MAT_AND_EXEC(GetName(),"");
}

//________________________________________________________________________________//

Double_t KVIonRangeTable::GetDensity(const Char_t* mat)
{
   // Return density of material (g/cm**3) of given type or name if it is in range tables
   FIND_MAT_AND_EXEC(GetDensity(),0.0);
}

//________________________________________________________________________________//

void KVIonRangeTable::SetTemperatureAndPressure(const Char_t*material, Double_t temperature, Double_t pressure)
{
   // Set temperature (in degrees celsius) and pressure (in torr) for a given
   // material. This has no effect except for gaseous materials, for which T & P
   // determine the density (in g/cm**3).

   KVIonRangeTableMaterial* M = GetMaterial(material);
   if (M) M->SetTemperatureAndPressure(temperature, pressure);
}

//________________________________________________________________________________//

//________________________________________________________________________________//

Double_t KVIonRangeTable::GetRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat, Double_t , Double_t)
{
   // Returns range (in g/cm**2) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetRangeOfIon(Z, A, E, Amat),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetLinearRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat, Double_t T, Double_t P)
{
   // Returns linear range (in cm) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearRangeOfIon(Z, A, E, Amat, T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat, Double_t , Double_t )
{
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness r (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetDeltaEOfIon(Z, A, E, r, Amat),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetLinearDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat, Double_t T, Double_t P)
{
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness d (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearDeltaEOfIon(Z, A, E, d, Amat, T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat, Double_t , Double_t )
{
   // Returns residual energy (in MeV) of ion (Z,A) with incident energy E (MeV) after thickness r (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetEResOfIon(Z, A, E, r, Amat),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetLinearEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat, Double_t T, Double_t P)
{
   // Returns residual energy (in MeV) of ion (Z,A) with incident energy E (MeV) after thickness d (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearEResOfIon(Z, A, E, d, Amat, T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetEIncFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculates incident energy (in MeV) of an ion (Z,A) with residual energy Eres (MeV) after thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   FIND_MAT_AND_EXEC(GetEIncFromEResOfIon(Z, A, Eres, e, isoAmat),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetLinearEIncFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e,
                                                       Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) with residual energy Eres (MeV) after thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetLinearEIncFromEResOfIon(Z, A, Eres, e, isoAmat, T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetEIncFromDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t DeltaE, Double_t e, enum KVIonRangeTable::SolType type, Double_t isoAmat, Double_t , Double_t )
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   FIND_MAT_AND_EXEC(GetEIncFromDeltaEOfIon(Z, A, DeltaE, e, type, isoAmat),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetLinearEIncFromDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t deltaE, Double_t e, enum KVIonRangeTable::SolType type,
                                                         Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetLinearEIncFromDeltaEOfIon(Z, A, deltaE, e, type, isoAmat,T,P),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetDeltaEFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   FIND_MAT_AND_EXEC(GetDeltaEFromEResOfIon(Z, A, Eres, e, isoAmat),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetLinearDeltaEFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetLinearDeltaEFromEResOfIon(Z, A, Eres, e, isoAmat,T,P),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetPunchThroughEnergy(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculate incident energy (in MeV) for ion (Z,A) for which the range is equal to the
   // given thickness e (in g/cm**2). At this energy the residual energy of the ion is (just) zero,
   // for all energies above this energy the residual energy is > 0.
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetPunchThroughEnergy(Z, A, e, isoAmat),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetLinearPunchThroughEnergy(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculate incident energy (in MeV) for ion (Z,A) for which the range is equal to the
   // given thickness e (in cm). At this energy the residual energy of the ion is (just) zero,
   // for all energies above this energy the residual energy is > 0.
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetLinearPunchThroughEnergy(Z, A, e, isoAmat,T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVIonRangeTable::GetMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculate maximum energy loss (in MeV) of ion (Z,A) in given thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material.
   FIND_MAT_AND_EXEC(GetMaxDeltaEOfIon(Z, A, e, isoAmat),0.0);   
}

Double_t KVIonRangeTable::GetEIncOfMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculate incident energy (in MeV) corresponding to maximum energy loss of ion (Z,A)
   // in given thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material.
   
   FIND_MAT_AND_EXEC(GetEIncOfMaxDeltaEOfIon(Z, A, e, isoAmat),0.0);   
}

Double_t KVIonRangeTable::GetLinearMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculate maximum energy loss (in MeV) of ion (Z,A) in given thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearMaxDeltaEOfIon(Z, A, e, isoAmat, T, P),0.0);   
}

Double_t KVIonRangeTable::GetLinearEIncOfMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculate incident energy (in MeV) corresponding to maximum energy loss of ion (Z,A)
   // in given thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearEIncOfMaxDeltaEOfIon(Z, A, e, isoAmat, T, P),0.0);   
}

Double_t KVIonRangeTable::GetEmaxValid(const Char_t* mat, Int_t Z, Int_t A)
{
   // Returns maximum energy (in MeV) for which range table is valid
   // for given material and incident ion (Z,A)
   
   FIND_MAT_AND_EXEC(GetEmaxValid(Z, A),0.0);   
}

//______________________________________________________________________________________//

TGeoMaterial* KVIonRangeTable::GetTGeoMaterial(const Char_t* mat)
{
   // Return pointer to TGeoMaterial corresponding to this material,
   // for use in ROOT geometries, VMC, etc.
   
   FIND_MAT_AND_EXEC(GetTGeoMaterial(),0x0);   
}

void KVIonRangeTable::Print(Option_t*) const
{
    printf("%s::%s\n%s\n", ClassName(), GetName(), GetTitle());
}

void KVIonRangeTable::DetectEvent(TGeoManager* TheGeometry, KVEvent* TheEvent, TVector3 *TheOrigin)
{
    // Given a valid ROOT geometry TheGeometry, we propagate the particles of TheEvent
    // (starting from optional position TheOrigin; if not given, we assume particles
    // are produced at (0,0,0) in the world coordinates) and, every time a particle
    // traverses a volume made of a TGeoMaterial with a name corresponding to a material
    // known by this range table, we calculate the energy loss of the particle,
    // and store it in the particle's list KVParticle::fParameters in the form
    //   "DE_[volume name]" = [energy lost in volume]

    static Bool_t printit = kFALSE; /* debug */

    KVNucleus* part;
    while( (part = TheEvent->GetNextParticle()) ){

        // Define point of origin of particles
        if(TheOrigin) TheGeometry->SetCurrentPoint(TheOrigin->X(),TheOrigin->Y(),TheOrigin->Z());
        else TheGeometry->SetCurrentPoint(0., 0., 0.);

        // unit vector in direction of particle's momentum
        TVector3 v = part->GetMomentum().Unit();
        // use particle's momentum direction
        TheGeometry->SetCurrentDirection(v.x(), v.y(), v.z());
        TheGeometry->FindNode();

        TGeoVolume* lastVol = TheGeometry->GetCurrentVolume();
        // move along trajectory until we hit a new volume
        TheGeometry->FindNextBoundaryAndStep();
        Double_t step = TheGeometry->GetStep();
        TGeoVolume* newVol = TheGeometry->GetCurrentVolume();

        Double_t e = part->GetKE(), de = 0;

        // track particle until we leave the geometry
        while (!TheGeometry->IsOutside()) {

            if(printit){
                std::cout << "   NOW IN VOLUME : " << lastVol->GetName() << std::endl;
                std::cout << "will travel " << step << " cm in this material :";
                std::cout << lastVol->GetMaterial()->GetTitle() << std::endl;
            }

            de = 0;
            // calculate energy losses in known materials
            if (IsMaterialKnown(lastVol->GetMaterial()->GetTitle())) {
                de = GetLinearDeltaEOfIon(
                            lastVol->GetMaterial()->GetTitle(),
                            part->GetZ(), part->GetA(), e, step, 0.,
                            lastVol->GetMaterial()->GetTemperature(),
                            lastVol->GetMaterial()->GetPressure());
                if (printit) std::cout << "and lose " << de << "MeV" << std::endl;
                e -= de;
                if(e<=1.e-3) e=0.;
                //set flag to say that particle has been slowed down
                part->SetIsDetected();
                //If this is the first absorber that the particle crosses, we set a "reminder" of its
                //initial energy
                if (!part->GetPInitial()) part->SetE0();
                part->GetParameters()->SetValue(Form("DE_%s",lastVol->GetName()), de);
            }
            lastVol = newVol;
            // stop when particle is stopped
            if (e <= 1.e-3) break;
            // move on to next volume crossed by trajectory
            TheGeometry->FindNextBoundaryAndStep();
            step = TheGeometry->GetStep();
            newVol = TheGeometry->GetCurrentVolume();
        }
        // set final energy of particle
        part->SetEnergy(e);
    }
}
   
