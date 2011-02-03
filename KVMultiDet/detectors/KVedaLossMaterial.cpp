//Created by KVClassFactory on Wed Feb  2 16:13:08 2011
//Author: frankland,,,,

#include "KVedaLossMaterial.h"

ClassImp(KVedaLossMaterial)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVedaLossMaterial</h2>
<h4>Description of material properties used by KVedaLoss range calculation</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVedaLossMaterial::KVedaLossMaterial()
{
   // Default constructor
}

KVedaLossMaterial::KVedaLossMaterial(const Char_t* name, const Char_t* type, const Char_t* state,
      Double_t density, Double_t Z, Double_t A, Double_t MoleWt, Double_t temperature)
      : fState(state),
        fDens(density),
        fZmat(Z),
        fAmat(A),
        fMoleWt(MoleWt),
        fTemp(temperature),
        fAmasr(0)
{
   // create new material
   SetName(name);
   SetType(type);
}

KVedaLossMaterial::~KVedaLossMaterial()
{
   // Destructor
}

void KVedaLossMaterial::ReadRangeTable(FILE* fp)
{
   // Read Z-dependent range parameters for material
   char line[132];
                  
                  //look for energy limits to calculation validity
               if (!fgets(line, 132, fp)) {
                  Warning("ReadRangeTable", "Problem reading energy limits in range table file for %s (%s)",
                     GetName(), GetType());
                  return;
               } else {
                  while (line[0] == 'Z') {
                     Int_t z1, z2;
                     Float_t e1, e2;
                     sscanf(line, "Z = %d,%d     %f < E/A  <  %f MeV", &z1,
                            &z2, &e1, &e2);
                     fgets(line, 132, fp);
                  }
               }

               for (register int count = 0; count < ZMAX_VEDALOSS; count++) {

                  if (sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
                             &fCoeff[count][0], &fCoeff[count][1],
                             &fCoeff[count][2], &fCoeff[count][3],
                             &fCoeff[count][4], &fCoeff[count][5],
                             &fCoeff[count][6], &fCoeff[count][7])
                      != 8) {
                     Error("ReadRangeTable", "problem reading coeffs 0-7 in range table for %s (%s)", GetName(), GetType());
                     return;
                  }
                  if (!fgets(line, 132, fp)) {
                     Warning("ReadRangeTable","file too short ??? %s (%s)", GetName(), GetType());
                     return;
                  } else {
                     if (sscanf(line, "%lf %lf %lf %lf %lf %lf",
                                &fCoeff[count][8], &fCoeff[count][9],
                                &fCoeff[count][10], &fCoeff[count][11],
                                &fCoeff[count][12], &fCoeff[count][13])
                         != 6) {
                        Error("ReadRangeTable","problem reading coeffs 8-13 in range table for %s (%s)", GetName(), GetType());
                        goto bad_exit;
                     }
                  }
                  fgets(line, 132, fp);
               }
}

