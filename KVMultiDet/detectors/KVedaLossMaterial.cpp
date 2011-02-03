//Created by KVClassFactory on Wed Feb  2 16:13:08 2011
//Author: frankland,,,,

#include "KVedaLossMaterial.h"
#include <TMath.h>
#include "KVIonRangeTable.h"
//#define RTT				623.61040
#define RTT  62.36367e+03  // cm^3.Torr.K^-1.mol^-1
#define ZERO_KELVIN	273.15

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
      : fState("unknown"),
        fDens(0.),
        fZmat(0),
        fAmat(0),
        fMoleWt(0),
        fRange(0)
{
   // Default constructor
   for (int i = 0; i < 100; i++) {
      fEmin[i] = 0.0;
      fEmax[i] = 500.0;
   }
}

KVedaLossMaterial::KVedaLossMaterial(const Char_t* name, const Char_t* type, const Char_t* state,
      Double_t density, Double_t Z, Double_t A, Double_t MoleWt)
      : fState(state),
        fDens(density),
        fZmat(Z),
        fAmat(A),
        fMoleWt(MoleWt),
        fRange(0)
{
   // create new material
   SetName(name);
   SetType(type);
   for (int i = 0; i < 100; i++) {
      fEmin[i] = 0.0;
      fEmax[i] = 500.0;
   }
}

KVedaLossMaterial::~KVedaLossMaterial()
{
   // Destructor
}

Bool_t KVedaLossMaterial::ReadRangeTable(FILE* fp)
{
   // Read Z-dependent range parameters for material
   char line[132];
                  
                  //look for energy limits to calculation validity
               if (!fgets(line, 132, fp)) {
                  Warning("ReadRangeTable", "Problem reading energy limits in range table file for %s (%s)",
                     GetName(), GetType());
                  return kFALSE;
               } else {
                  while (line[0] == 'Z') {
                     Int_t z1, z2;
                     Float_t e1, e2;
                     sscanf(line, "Z = %d,%d     %f < E/A  <  %f MeV", &z1,
                            &z2, &e1, &e2);
                     char* tmp; tmp = fgets(line, 132, fp);
                     for (int i = z1; i <= z2; i++) {
                        fEmin[i - 1] = e1;
                        fEmax[i - 1] = e2;
                     }
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
                     return kFALSE;
                  }
                  if (!fgets(line, 132, fp)) {
                     Warning("ReadRangeTable","file too short ??? %s (%s)", GetName(), GetType());
                     return kFALSE;
                  } else {
                     if (sscanf(line, "%lf %lf %lf %lf %lf %lf",
                                &fCoeff[count][8], &fCoeff[count][9],
                                &fCoeff[count][10], &fCoeff[count][11],
                                &fCoeff[count][12], &fCoeff[count][13])
                         != 6) {
                        Error("ReadRangeTable","problem reading coeffs 8-13 in range table for %s (%s)", GetName(), GetType());
                        return kFALSE;
                     }
                  }
                  char* tmp; tmp = fgets(line, 132, fp);
               }

   fRange = new TF1( Form("KVedaLossMaterial:%s:Range",GetType()), this, &KVedaLossMaterial::RangeFunc,
         0.1, 1.e+04, 3, "KVedaLossMaterial", "RangeFunc");
    fRange->SetNpx(1000);
         
   fDeltaE = new TF1( Form("KVedaLossMaterial:%s:EnergyLoss",GetType()), this, &KVedaLossMaterial::DeltaEFunc,
         0.1, 1.e+04, 4, "KVedaLossMaterial", "DeltaEFunc");
    fDeltaE->SetNpx(1000);
     return kTRUE;
}

Double_t KVedaLossMaterial::CalculateGasDensity(Double_t T, Double_t P) const
{
   // for a gaseous material, calculate density in g/cm**3 according to given
   // conditions of temperature (T, in degrees celsius) and pressure (P, in Torr)
   
   return (fMoleWt * P) / ((T + ZERO_KELVIN) * RTT);
}

void KVedaLossMaterial::ls(Option_t*) const
{
   printf("KVedaLossMaterial::%s     Material type : %s    State : %s\n", GetName(), GetType(), fState.Data());
   printf("\tZ=%f  A=%f  ", fZmat, fAmat);
   if(IsGas()) printf(" Mole Weight = %f g.    Density at S.T.P. = %f g/cm**3\n\n", fMoleWt, CalculateGasDensity(19., 760.));
   else printf(" Density = %f g/cm**3\n\n", GetDensity());
}

Double_t KVedaLossMaterial::DeltaEFunc(Double_t* E, Double_t* Mypar)
{
   // Function parameterising the energy loss of charged particles in this material.
   // The incident energy E[0] is given in MeV.
   // The energy loss is calculated in MeV.
   //
   //Parameters:
   //  Mypar[0]  = thickness of material in g/cm**2
   //  Mypar[1]  = Z of charged particle
   //  Mypar[2]  = A of charged particle
   //  Mypar[3]  = isotope of material element (0 if material is not isotopically pure)
   
   // if range < thickness, particle stops: dE = E0
   Double_t R0 = RangeFunc(E, &Mypar[1]);
   if( R0 < Mypar[0] ) {  return E[0]; }
   
   // calculate energy loss - invert range function to find E corresponding to (R0 - thickness)
   R0-=Mypar[0];
   Double_t dE = E[0] - fRange->GetX(R0);
   return dE;
}

Double_t KVedaLossMaterial::RangeFunc(Double_t* E, Double_t* Mypar)
{
   // Function parameterising the range of charged particles in this material.
   // The energy E[0] is given in MeV.
   // The range is calculated in units of g/cm**2
   //
   //Parameters:
   //  Mypar[0]  = Z of charged particle
   //  Mypar[1]  = A of charged particle
   //  Mypar[2]  = isotope of material element (0 if material is not isotopically pure)

   Int_t Z = (Int_t )Mypar[0];
   Double_t A = Mypar[1];
   // get parameters for this Z
   Double_t *par = fCoeff[Z - 1];
   
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
   // calculate energy loss
   Double_t eps = E[0] / A;       //energy in MeV/nucleon
   Double_t dleps = TMath::Log(eps);
   Double_t MatIsoR = 0.;
   if(Mypar[2] > 0.0) MatIsoR = TMath::Log(Mypar[2] / fAmat);
   Double_t riso = TMath::Log(A / par[1]) + MatIsoR;

   if (eps < 0.1)
      ran = adm * dleps + adn;
   else {
      ran = 0.0;
      for (register int j = 2; j < 7; j++)
         ran += par[j + 1] * TMath::Power(dleps, (Double_t) (j - 1));
      ran += par[2];
   }
   ran += riso;

   // range in g/cm**2
   Double_t range = TMath::Exp(ran) * Units::mg/pow(Units::cm,2);
   return range;
}

TF1* KVedaLossMaterial::GetRangeFunction(Int_t Z, Int_t A, Double_t isoAmat)
{
   // Return function giving range (in centimetres) as a function of energy (in MeV) for
   // charged particles (Z,A) in this material.
   // If required, the isotopic mass of the material can be given.
   
   fRange->SetParameters(Z, A, isoAmat);
   return fRange;
}

TF1* KVedaLossMaterial::GetDeltaEFunction(Double_t e, Int_t Z, Int_t A, Double_t isoAmat)
{
   // Return function giving energy loss (in MeV) as a function of incident energy (in MeV) for
   // charged particles (Z,A) traversing (or not) the thickness e (in g/cm**2) of this material.
   // If required, the isotopic mass of the material can be given.
   
   fRange->SetParameters(Z, A, isoAmat);
   fDeltaE->SetParameters(e, Z, A, isoAmat);
   return fDeltaE;
}

