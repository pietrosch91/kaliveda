//Created by KVClassFactory on Wed Feb  2 15:49:27 2011
//Author: frankland,,,,

#include "KVedaLoss.h"
#include "KVedaLossMaterial.h"
#include <TString.h>
#include <TSystem.h>
#include <TEnv.h>

ClassImp(KVedaLoss)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVedaLoss</h2>
<h4>C++ implementation of VEDALOSS stopping power calculation</h4>
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
               break;
         }
      }
      fclose(fp);
   }
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

Bool_t KVedaLoss::IsMaterialGas(const Char_t* material)
{
   // Returns kTRUE if material of given name or type is gaseous.
   KVedaLossMaterial* M = GetMaterial(material);
   if (M) return M->IsGas();
   return kFALSE;
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

const Char_t* KVedaLoss::GetMaterialName(const Char_t* material)
{
   // Return name of material of given type or name if it is in range tables
   KVedaLossMaterial* M = GetMaterial(material);
   if (M) return M->GetName();
   return "";
}

//________________________________________________________________________________//

Double_t KVedaLoss::GetDensity(const Char_t* material)
{
   // Return density of material (g/cm**3) of given type or name if it is in range tables
   KVedaLossMaterial* M = GetMaterial(material);
   if (M) return M->GetDensity();
   return 0.0;
}

//________________________________________________________________________________//

void KVedaLoss::SetTemperatureAndPressure(const Char_t*material, Double_t temperature, Double_t pressure)
{
   // Set temperature (in degrees celsius) and pressure (in torr) for a given
   // material. This has no effect except for gaseous materials, for which T & P
   // determine the density (in g/cm**3).

   KVedaLossMaterial* M = GetMaterial(material);
   if (M && M->IsGas()) M->SetGasDensity(temperature, pressure);
}

//________________________________________________________________________________//

void KVedaLoss::Print(Option_t*) const
{
   printf("KVedaLoss::%s\n%s\n", GetName(), GetTitle());
   printf("\nEnergy loss & range tables loaded for %d materials:\n\n", fMaterials->GetEntries());
   fMaterials->ls();
}

