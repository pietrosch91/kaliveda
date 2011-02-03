//Created by KVClassFactory on Wed Feb  2 15:49:27 2011
//Author: frankland,,,,

#include "KVedaLoss.h"
#include "KVedaLossMaterial.h"
#include <TString.h>
#include <TSystem.h>

ClassImp(KVedaLoss)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVedaLoss</h2>
<h4>C++ implementation of VEDALOSS stopping power calculation</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

TList* KVedaLoss::fMaterials = 0x0;

KVedaLoss::KVedaLoss()
{
   // Default constructor
}

KVedaLoss::~KVedaLoss()
{
   // Destructor
}

void KVedaLoss::init_materials()
{
   // PRIVATE method - called to initialize fMaterials list of all known materials
   // properties, read from file given by TEnv variable KVedaLoss.RangeTables
   
   fMaterials = new TList;
   TString DataFilePath = gEnv->GetValue("KVedaLoss.RangeTables","");
   gSystem->ExpandPathName( DataFilePath );
   
   Char_t name[25], gtype[25],state[10];
   Float_t Amat = 0.;
   Float_t Dens = 0.;
   Float_t GasThick = 0.;
   Float_t MoleWt = 0.;
   Float_t Temp = 19.;
   Float_t Zmat = 0.;

   FILE *fp;
   if (!(fp = fopen(DataFilePath.Data(), "r"))) {
      Error("init_materials()", "Range tables file %s cannot be opened", DataFilePath.Data());
      return;
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
               return 0;
            }
//found a new material
            KVedaLossMaterial *tmp_mat = new KVedaLossMaterial(name, gtype, state, Dens, 
               Zmat, Amat, MoleWt, Temp);
            fMaterials->Add(tmp_mat);
            tmp_mat->ReadRangeTable(fp);
            break;
         }
      }
      fclose(fp);
   }
}
