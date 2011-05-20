//Created by KVClassFactory on Wed Feb  2 15:49:27 2011
//Author: frankland,,,,

#include "KVedaLoss.h"
#include "KVedaLossMaterial.h"
#include <TString.h>
#include <TSystem.h>
#include <TEnv.h>
#include "TGeoMaterial.h"

#define FIND_MAT_AND_EXEC(method,defval) \
    KVedaLossMaterial* M = GetMaterial(mat); \
    if(M) return M->method; \
    return defval

ClassImp(KVedaLoss)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVedaLoss</h2>
<h4>C++ implementation of VEDALOSS stopping power calculation</h4>
<h3>Credits</h3>
From the original documentation of vedaloss.f:<br>
<pre>
 vedaloss.doc       MAJ 5/9/97   
 -------------
 Routines ecrites par E. de Filippo          
 Mise a jour :      19/12/96 par O. LOPEZ pour UNIX sur ANASTASIE.
 Mise a jour :      22/05/97 par J-L. Charvet (voir routine DELTA)
 Mise a jour :      27/10/97 par R. Dayras
 Mise a jour :      23/03/00 par R. Dayras (ajout Nobium)
</pre>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVHashList* KVedaLoss::fMaterials = 0x0;

KVedaLoss::KVedaLoss()
{
   // Default constructor
   SetName("VEDALOSS");
   SetTitle("Calculation of range and energy loss of charged particles in matter using VEDALOSS range tables");
   if (!CheckMaterialsList()) {
      Error("KVedaLoss", "Problem reading range tables. Do not use.");
   }
}

KVedaLoss::~KVedaLoss()
{
   // Destructor
}

Bool_t KVedaLoss::init_materials() const
{
   // PRIVATE method - called to initialize fMaterials list of all known materials
   // properties, read from file given by TEnv variable KVedaLoss.RangeTables

   Info("init_materials", "Initialising KVedaLoss...");
   printf("\n");
   printf("\t*************************************************************************\n");
   printf("\t*                VEDALOSS STOPPING POWER & RANGE TABLES                 *\n");
   printf("\t*                                                                       *\n");
   int mat_count = 0;
   fMaterials = new KVHashList;
   fMaterials->SetName("VEDALOSS materials list");
   fMaterials->SetOwner();

   TString DataFilePath = gEnv->GetValue("KVedaLoss.RangeTables", "");
   gSystem->ExpandPathName(DataFilePath);

   Char_t name[25], gtype[25], state[10];
   Float_t Amat = 0.;
   Float_t Dens = 0.;
   Float_t MoleWt = 0.;
   Float_t Temp = 19.;
   Float_t Zmat = 0.;

   FILE *fp;
   if (!(fp = fopen(DataFilePath.Data(), "r"))) {
      Error("init_materials()", "Range tables file %s cannot be opened", DataFilePath.Data());
      return kFALSE;
   } else {
      char line[132];
      while (fgets(line, 132, fp)) {    // read lines from file

         switch (line[0]) {

            case '/':             // ignore comment lines
               break;

            case '+':             // header lines

               if (sscanf(line, "+ %s %s %s %f %f %f %f %f",
                          gtype, name, state, &Dens, &Zmat, &Amat,
                          &MoleWt, &Temp)
                   != 8) {
                  Error("init_materials()", "Problem reading file %s", DataFilePath.Data());
                  fclose(fp);
                  return kFALSE;
               }
//found a new material
               KVedaLossMaterial *tmp_mat = new KVedaLossMaterial(name, gtype, state, Dens,
                                                                  Zmat, Amat, MoleWt);
               fMaterials->Add(tmp_mat);
               if (!tmp_mat->ReadRangeTable(fp)) return kFALSE;
               ++mat_count;
               Double_t rho = 0.;
               if(tmp_mat->IsGas()) tmp_mat->SetTemperatureAndPressure(19., 1.*KVUnits::atm);
               rho = tmp_mat->GetDensity();
               printf("\t*  %2d.  %-7s %-18s  Z=%2d A=%5.1f  rho=%6.3f g/cm**3    *\n",
                           mat_count, tmp_mat->GetType(), tmp_mat->GetName(),
                           (int)tmp_mat->GetZ(), tmp_mat->GetMass(),
                           rho);
               break;
         }
      }
      fclose(fp);
   }
   printf("\t*                                                                       *\n");
   printf("\t*     TF1::Range::Npx = %4d            TF1::EnergyLoss::Npx = %4d     *\n",
   	gEnv->GetValue("KVedaLoss.Range.Npx",100), gEnv->GetValue("KVedaLoss.EnergyLoss.Npx",100));
   printf("\t*                      TF1::ResidualEnergy::Npx = %4d                  *\n",
   		gEnv->GetValue("KVedaLoss.ResidualEnergy.Npx",100));
   printf("\t*                                                                       *\n");
   printf("\t*                       INITALISATION COMPLETE                          *\n");
   printf("\t*************************************************************************\n");
   return kTRUE;
}

//________________________________________________________________________________//

Double_t KVedaLoss::GetAtomicMass(const Char_t* material)
{
   // Return atomic mass of a material in the range table.
   // "material" can be either the type or the name of the material.
   // Prints a warning and returns 0 if material is unknown.

   KVedaLossMaterial* M = GetMaterial(material);
   if (!M) {
      Warning("GetAtomicMass", "Material %s is unknown. Returned mass = 0.", material);
      return 0.0;
   }
   return M->GetMass();
}

//________________________________________________________________________________//

Double_t KVedaLoss::GetZ(const Char_t* material)
{
   // Return atomic number of a material in the range table.
   // "material" can be either the type or the name of the material.
   // Prints a warning and returns 0 if material is unknown.

   KVedaLossMaterial* M = GetMaterial(material);
   if (!M) {
      Warning("GetZ", "Material %s is unknown. Returned Z = 0.", material);
      return 0.0;
   }
   return M->GetZ();
}

//________________________________________________________________________________//

Bool_t KVedaLoss::IsMaterialKnown(const Char_t* material)
{
   // Returns kTRUE if material of given name or type is in range table.
   KVedaLossMaterial* M = GetMaterial(material);
   return (M != 0x0);
}

//________________________________________________________________________________//

Bool_t KVedaLoss::IsMaterialGas(const Char_t* mat)
{
   // Returns kTRUE if material of given name or type is gaseous.
   FIND_MAT_AND_EXEC(IsGas(),kFALSE);
}

//________________________________________________________________________________//

KVedaLossMaterial* KVedaLoss::GetMaterial(const Char_t* material)
{
   // Returns pointer to material of given name or type.
   KVedaLossMaterial* M = (KVedaLossMaterial*)fMaterials->FindObject(material);
   if (!M) {
      M = (KVedaLossMaterial*)fMaterials->FindObjectByType(material);
   }
   return M;
}

//________________________________________________________________________________//

const Char_t* KVedaLoss::GetMaterialName(const Char_t* mat)
{
   // Return name of material of given type or name if it is in range tables
   FIND_MAT_AND_EXEC(GetName(),"");
}

//________________________________________________________________________________//

Double_t KVedaLoss::GetDensity(const Char_t* mat)
{
   // Return density of material (g/cm**3) of given type or name if it is in range tables
   FIND_MAT_AND_EXEC(GetDensity(),0.0);
}

//________________________________________________________________________________//

void KVedaLoss::SetTemperatureAndPressure(const Char_t*material, Double_t temperature, Double_t pressure)
{
   // Set temperature (in degrees celsius) and pressure (in torr) for a given
   // material. This has no effect except for gaseous materials, for which T & P
   // determine the density (in g/cm**3).

   KVedaLossMaterial* M = GetMaterial(material);
   if (M) M->SetTemperatureAndPressure(temperature, pressure);
}

//________________________________________________________________________________//

void KVedaLoss::Print(Option_t*) const
{
   printf("KVedaLoss::%s\n%s\n", GetName(), GetTitle());
   printf("\nEnergy loss & range tables loaded for %d materials:\n\n", fMaterials->GetEntries());
   fMaterials->ls();
}

//________________________________________________________________________________//

Double_t KVedaLoss::GetRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat, Double_t , Double_t)
{
   // Returns range (in g/cm**2) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetRangeOfIon(Z, A, E, Amat),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetLinearRangeOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E,
      Double_t Amat, Double_t T, Double_t P)
{
   // Returns linear range (in cm) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearRangeOfIon(Z, A, E, Amat, T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat, Double_t , Double_t )
{
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness r (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetDeltaEOfIon(Z, A, E, r, Amat),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetLinearDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat, Double_t T, Double_t P)
{
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness d (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearDeltaEOfIon(Z, A, E, d, Amat, T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t r,
      Double_t Amat, Double_t , Double_t )
{
   // Returns residual energy (in MeV) of ion (Z,A) with incident energy E (MeV) after thickness r (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetEResOfIon(Z, A, E, r, Amat),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetLinearEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t E, Double_t d,
      Double_t Amat, Double_t T, Double_t P)
{
   // Returns residual energy (in MeV) of ion (Z,A) with incident energy E (MeV) after thickness d (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearEResOfIon(Z, A, E, d, Amat, T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetEIncFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculates incident energy (in MeV) of an ion (Z,A) with residual energy Eres (MeV) after thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   FIND_MAT_AND_EXEC(GetEIncFromEResOfIon(Z, A, Eres, e, isoAmat),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetLinearEIncFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e,
                                                       Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) with residual energy Eres (MeV) after thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetLinearEIncFromEResOfIon(Z, A, Eres, e, isoAmat, T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetEIncFromDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t DeltaE, Double_t e, enum KVIonRangeTable::SolType type, Double_t isoAmat, Double_t , Double_t )
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   FIND_MAT_AND_EXEC(GetEIncFromDeltaEOfIon(Z, A, DeltaE, e, type, isoAmat),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetLinearEIncFromDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t deltaE, Double_t e, enum KVIonRangeTable::SolType type,
                                                         Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetLinearEIncFromDeltaEOfIon(Z, A, deltaE, e, type, isoAmat,T,P),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetDeltaEFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   FIND_MAT_AND_EXEC(GetDeltaEFromEResOfIon(Z, A, Eres, e, isoAmat),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetLinearDeltaEFromEResOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetLinearDeltaEFromEResOfIon(Z, A, Eres, e, isoAmat,T,P),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetPunchThroughEnergy(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculate incident energy (in MeV) for ion (Z,A) for which the range is equal to the
   // given thickness e (in g/cm**2). At this energy the residual energy of the ion is (just) zero,
   // for all energies above this energy the residual energy is > 0.
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetPunchThroughEnergy(Z, A, e, isoAmat),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetLinearPunchThroughEnergy(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculate incident energy (in MeV) for ion (Z,A) for which the range is equal to the
   // given thickness e (in cm). At this energy the residual energy of the ion is (just) zero,
   // for all energies above this energy the residual energy is > 0.
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   FIND_MAT_AND_EXEC(GetLinearPunchThroughEnergy(Z, A, e, isoAmat,T, P),0.0);
}
//________________________________________________________________________________//

Double_t KVedaLoss::GetMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculate maximum energy loss (in MeV) of ion (Z,A) in given thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material.
   FIND_MAT_AND_EXEC(GetMaxDeltaEOfIon(Z, A, e, isoAmat),0.0);   
}

Double_t KVedaLoss::GetEIncOfMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t , Double_t )
{
   // Calculate incident energy (in MeV) corresponding to maximum energy loss of ion (Z,A)
   // in given thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material.
   
   FIND_MAT_AND_EXEC(GetEIncOfMaxDeltaEOfIon(Z, A, e, isoAmat),0.0);   
}

Double_t KVedaLoss::GetLinearMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculate maximum energy loss (in MeV) of ion (Z,A) in given thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearMaxDeltaEOfIon(Z, A, e, isoAmat, T, P),0.0);   
}

Double_t KVedaLoss::GetLinearEIncOfMaxDeltaEOfIon(const Char_t* mat, Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculate incident energy (in MeV) corresponding to maximum energy loss of ion (Z,A)
   // in given thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   FIND_MAT_AND_EXEC(GetLinearEIncOfMaxDeltaEOfIon(Z, A, e, isoAmat, T, P),0.0);   
}

Double_t KVedaLoss::GetEmaxValid(const Char_t* mat, Int_t Z, Int_t A)
{
   // Returns maximum energy (in MeV) for which range table is valid
   // for given material and incident ion (Z,A)
   
   FIND_MAT_AND_EXEC(GetEmaxValid(Z, A),0.0);   
}

//______________________________________________________________________________________//

TObjArray* KVedaLoss::GetListOfMaterials()
{
   // Create and fill a list of all materials for which range tables exist.
   // Each entry is a TNamed with the name and type (title) of the material.
   // User's responsibility to delete list after use (it owns its objects).
   
   if(CheckMaterialsList()){
      TObjArray* list = new TObjArray(fMaterials->GetEntries());
      list->SetOwner(kTRUE);
      TIter next(fMaterials);
      KVedaLossMaterial* mat;
      while( (mat = (KVedaLossMaterial*)next()) ){
         list->Add(new TNamed(mat->GetName(), mat->GetType()));
      }
      return list;
   }
   return 0;
}

TGeoMaterial* KVedaLoss::GetTGeoMaterial(const Char_t* mat)
{
   // Return pointer to TGeoMaterial corresponding to this material,
   // for use in ROOT geometries, VMC, etc.
   
   FIND_MAT_AND_EXEC(GetTGeoMaterial(),0x0);   
}


