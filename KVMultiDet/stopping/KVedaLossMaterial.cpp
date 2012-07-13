//Created by KVClassFactory on Wed Feb  2 16:13:08 2011
//Author: frankland,,,,

#include "KVedaLossMaterial.h"
#include <TMath.h>
#include "KVIonRangeTable.h"
#include <TEnv.h>
#include "TGeoManager.h"
#include "TGeoMaterial.h"

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
   fCompound(0),
   fComposition(0),
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
   fCompound(0),
   fComposition(0),
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
   SafeDelete(fComposition);
}

Bool_t KVedaLossMaterial::ReadRangeTable(FILE* fp)
{
   // Read Z- & A-dependent range parameters for material
   //
   // For each material we create 3 TF1 objects:
   //   KVedaLossMaterial:[type]:Range                -  gives range in g/cm**2 as a function of particle energy
   //   KVedaLossMaterial:[type]:EnergyLoss           -  gives dE as a function of particle energy
   //   KVedaLossMaterial:[type]:ResidualEnergy       -  gives energy after material (0 if particle stops)
   //
   // The TF1::fNpx parameter for these functions is defined by the environment variables
   //
   //   KVedaLoss.Range.Npx:         20
   //   KVedaLoss.EnergyLoss.Npx:         50
   //   KVedaLoss.ResidualEnergy.Npx:         20
   //
   
   char line[132];

   //look for energy limits to calculation validity
   if (!fgets(line, 132, fp)) {
      Warning("ReadRangeTable", "Problem reading energy limits in range table file for %s (%s)",
              GetName(), GetType());
      return kFALSE;
   } else {
      if(!strncmp(line,"COMPOUND",8)){
      	// material is compound. read composition for TGeoMixture.
      	SetCompound();
      	fgets(line, 132, fp);
      	fComposition = new TList; // create table of elements
      	fComposition->SetOwner(kTRUE);
      	int nel = atoi(line);  // read number of elements
      	float total_poids=0;
      	for(int el=0; el<nel; el++){
      		fgets(line, 132, fp);
      		char elname[5]; float poids=-1.;
      		sscanf(line, "%s %f", elname, &poids);
      		total_poids+=poids;
      		fComposition->Add(new TNamed(elname, Form("%f",poids)));
      	}
      	// normalise weights
      	for(int el=0; el<nel; el++){
      		TNamed* elob = (TNamed*)fComposition->At(el);
      		float poids = atof(elob->GetTitle());
      		poids/=total_poids;
      		elob->SetTitle(Form("%f",poids));
      	}
      }
   }
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
         char* tmp;
         tmp = fgets(line, 132, fp);
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
         Warning("ReadRangeTable", "file too short ??? %s (%s)", GetName(), GetType());
         return kFALSE;
      } else {
         if (sscanf(line, "%lf %lf %lf %lf %lf %lf",
                    &fCoeff[count][8], &fCoeff[count][9],
                    &fCoeff[count][10], &fCoeff[count][11],
                    &fCoeff[count][12], &fCoeff[count][13])
             != 6) {
            Error("ReadRangeTable", "problem reading coeffs 8-13 in range table for %s (%s)", GetName(), GetType());
            return kFALSE;
         }
      }
      char* tmp;
      tmp = fgets(line, 132, fp);
   }

   // get require Npx value from (user-defined) environment variables
   Int_t my_npx = gEnv->GetValue("KVedaLoss.Range.Npx", 100);   
   
   fRange = new TF1(Form("KVedaLossMaterial:%s:Range", GetType()), this, &KVedaLossMaterial::RangeFunc,
                    0., 1.e+03, 0, "KVedaLossMaterial", "RangeFunc");
   fRange->SetNpx(my_npx);

   my_npx = gEnv->GetValue("KVedaLoss.EnergyLoss.Npx", 100);   
   fDeltaE = new TF1(Form("KVedaLossMaterial:%s:EnergyLoss", GetType()), this, &KVedaLossMaterial::DeltaEFunc,
                     0., 1.e+03, 0, "KVedaLossMaterial", "DeltaEFunc");
   fDeltaE->SetNpx(my_npx);

   my_npx = gEnv->GetValue("KVedaLoss.ResidualEnergy.Npx", 100);   
   fEres = new TF1(Form("KVedaLossMaterial:%s:ResidualEnergy", GetType()), this, &KVedaLossMaterial::EResFunc,
                   0., 1.e+03, 0, "KVedaLossMaterial", "EResFunc");
   fEres->SetNpx(my_npx);

   return kTRUE;
}

void KVedaLossMaterial::ls(Option_t*) const
{
   printf("KVedaLossMaterial::%s     Material type : %s    State : %s\n", GetName(), GetType(), fState.Data());
   printf("\tZ=%f  A=%f  ", fZmat, fAmat);
   if (IsGas()) printf(" Mole Weight = %f g.", fMoleWt);
   if (fDens > 0) printf(" Density = %f g/cm**3", fDens);
   printf("\n\n");
}

Double_t KVedaLossMaterial::DeltaEFunc(Double_t* E, Double_t* Mypar)
{
   // Function parameterising the energy loss of charged particles in this material.
   // The incident energy E[0] is given in MeV.
   // The energy loss is calculated in MeV.

   // if range < thickness, particle stops: dE = E0
   Double_t R0 = RangeFunc(E, 0);
   if (R0 < thickness) {
      return E[0];
   }

   // calculate energy loss - invert range function to find E corresponding to (R0 - thickness)
   R0 -= thickness;

   // invert range function to get energy after absorber
   return (E[0] - fRange->GetX(R0));

   /*
      // VEDALOSS inversion of R(E)
      // range in mg/cm**2 for VEDALOSS
      R0 /= (KVUnits::mg / pow(KVUnits::cm,2));
      // get parameters for this Z
      Int_t Z = (Int_t)Mypar[1];
      Double_t A = Mypar[2];
      Double_t *par = fCoeff[Z - 1];
      Double_t ranx = TMath::Log(R0);
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
      Double_t eres = A * epres;

      // garde-fou - calculated energy after absorber > incident energy ?!!
      //if(eres > E[0]) return 0.0;
      return E[0] - eres;
      */
}

Double_t KVedaLossMaterial::EResFunc(Double_t* E, Double_t* Mypar)
{
   // Function parameterising the residual energy of charged particles in this material.
   // The incident energy E[0] is given in MeV.
   // The residual energy is calculated in MeV.

   // if range < thickness, particle stops: Eres=0
   Double_t R0 = RangeFunc(E, 0);
   if (R0 < thickness) {
      return 0.0;
   }

   // calculate energy after absorber - invert range function to find Eres corresponding to (R0 - thickness)
   R0 -= thickness;

   // invert range function to get energy after absorber
   return fRange->GetX(R0);
}


TF1* KVedaLossMaterial::GetRangeFunction(Int_t Z, Int_t A, Double_t isoAmat)
{
   // Return function giving range (in g/cm**2) as a function of energy (in MeV) for
   // charged particles (Z,A) in this material.
   // If required, the isotopic mass of the material can be given.

   //fRange->SetParameters(Z, A, isoAmat);
   RF_Z = Z;
   RF_A = A;
   // get parameters for this Z
   par = fCoeff[Z - 1];
   // set up polynomial
   Double_t x1 = TMath::Log(0.1);
   Double_t x2 = TMath::Log(0.2);
   ran = 0.0;
   for (register int j = 2; j < 7; j++)
      ran += par[j + 1] * TMath::Power(x2, (Double_t)(j - 1));
   ran += par[2];
   Double_t y2 = ran;
   ran = 0.0;
   for (register int jj = 2; jj < 7; jj++)
      ran += par[jj + 1] * TMath::Power(x1, (Double_t)(jj - 1));
   ran += par[2];
   Double_t y1 = ran;
   adm = (y2 - y1) / (x2 - x1);
   adn = (y1 - adm * x1);
   riso = RF_A / par[1];
   if (isoAmat > 0.0) riso *= (isoAmat / fAmat);
   
   fRange->SetRange(0., GetEmaxValid(Z,A));
   return fRange;
}

Double_t KVedaLossMaterial::RangeFunc(Double_t* E, Double_t*)
{
   // Function parameterising the range of charged particles in this material.
   // The energy E[0] is given in MeV.
   // The range is calculated in units of g/cm**2

   eps = E[0] / RF_A;
   dleps = TMath::Log(eps);
   if (eps < 0.1)
      ran = adm * dleps + adn;
   else {
   	DLEP = dleps;
      ran = par[2] + par[3] * DLEP;
      ran += par[4] * (DLEP *= dleps);
      ran += par[5] * (DLEP *= dleps);
      ran += par[6] * (DLEP *= dleps);
      ran += par[7] * (DLEP *= dleps);
   }

   // range in g/cm**2
   return riso * TMath::Exp(ran) * KVUnits::mg;
}

TF1* KVedaLossMaterial::GetDeltaEFunction(Double_t e, Int_t Z, Int_t A, Double_t isoAmat)
{
   // Return function giving energy loss (in MeV) as a function of incident energy (in MeV) for
   // charged particles (Z,A) traversing (or not) the thickness e (in g/cm**2) of this material.
   // If required, the isotopic mass of the material can be given.

   GetRangeFunction(Z, A, isoAmat);
   thickness = e;
   fDeltaE->SetRange(0., GetEmaxValid(Z,A));
   return fDeltaE;
}

TF1* KVedaLossMaterial::GetEResFunction(Double_t e, Int_t Z, Int_t A, Double_t isoAmat)
{
   // Return function giving residual energy (in MeV) as a function of incident energy (in MeV) for
   // charged particles (Z,A) traversing (or not) the thickness e (in g/cm**2) of this material.
   // If required, the isotopic mass of the material can be given.

   GetRangeFunction(Z, A, isoAmat);
   thickness = e;
   fEres->SetRange(0., GetEmaxValid(Z,A));
   return fEres;
}

void KVedaLossMaterial::PrintRangeTable(Int_t Z, Int_t A, Double_t isoAmat, Double_t units, Double_t T, Double_t P)
{
   // Print range of element (in g/cm**2) as a function of incident energy (in MeV).
   // For solid elements, print also the linear range (in cm). To change the default units,
   // set optional argument units (e.g. to have linear range in microns, call with units = KVUnits::um).
   // For gaseous elements, give the temperature (in degrees) and the pressure (in torr)
   // in order to print the range in terms of length units.

   fRange->SetParameters(Z, A, isoAmat);
   printf("  ****  VEDALOSS Range Table  ****\n\n");
   ls();
   printf(" Element: Z=%d A=%d\n\n", Z, A);
   printf("\tENERGY (MeV)\t\tRANGE (g/cm**2)");
   if (!IsGas() || (IsGas() && T > 0 && P > 0)) printf("\t\tLIN. RANGE");
   SetTemperatureAndPressure(T, P);
   printf("\n\n");
   for (Double_t e = 0.1; e <= 1.e+4; e *= 10) {
      printf("\t%10.5g\t\t%10.5g", e, fRange->Eval(e));
      if (!IsGas() || (IsGas() && T > 0 && P > 0)) printf("\t\t\t%10.5g", fRange->Eval(e) / GetDensity() / units);
      printf("\n");
   }
}

Double_t KVedaLossMaterial::GetRangeOfIon(Int_t Z, Int_t A, Double_t E, Double_t isoAmat)
{
   // Returns range (in g/cm**2) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   TF1* f = GetRangeFunction(Z, A, isoAmat);
   return f->Eval(E);
}

Double_t KVedaLossMaterial::GetLinearRangeOfIon(Int_t Z, Int_t A, Double_t E, Double_t isoAmat, Double_t T, Double_t P)
{
   // Returns range (in cm) of ion (Z,A) with energy E (MeV) in material.
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.

   SetTemperatureAndPressure(T, P);
   if (fDens > 0) return GetRangeOfIon(Z, A, E, isoAmat) / GetDensity();
   else return 0.;
}

Double_t KVedaLossMaterial::GetDeltaEOfIon(Int_t Z, Int_t A, Double_t E, Double_t e, Double_t isoAmat)
{
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,

   TF1* f = GetDeltaEFunction(e, Z, A, isoAmat);
   return f->Eval(E);
}

Double_t KVedaLossMaterial::GetLinearDeltaEOfIon(Int_t Z, Int_t A, Double_t E, Double_t e,
                                                 Double_t isoAmat, Double_t T, Double_t P)
{
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.

   SetTemperatureAndPressure(T, P);
   e *= GetDensity();
   return GetDeltaEOfIon(Z, A, E, e, isoAmat);
}

Double_t KVedaLossMaterial::GetEResOfIon(Int_t Z, Int_t A, Double_t E, Double_t e,
                                         Double_t isoAmat)
{
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   TF1* f = GetEResFunction(e, Z, A, isoAmat);
   return f->Eval(E);
}

Double_t KVedaLossMaterial::GetLinearEResOfIon(Int_t Z, Int_t A, Double_t E, Double_t e,
                                               Double_t isoAmat, Double_t T, Double_t P)
{
   // Returns energy lost (in MeV) by ion (Z,A) with energy E (MeV) after thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.

   SetTemperatureAndPressure(T, P);
   e *= GetDensity();
   return GetEResOfIon(Z, A, E, e, isoAmat);
}

Double_t KVedaLossMaterial::GetEIncFromEResOfIon(Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) with residual energy Eres (MeV) after thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   GetRangeFunction(Z, A, isoAmat);
   Double_t R0 = fRange->Eval(Eres) + e;
   return fRange->GetX(R0);
}

Double_t KVedaLossMaterial::GetLinearEIncFromEResOfIon(Int_t Z, Int_t A, Double_t Eres, Double_t e,
                                                       Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) with residual energy Eres (MeV) after thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.

   SetTemperatureAndPressure(T, P);
   e *= GetDensity();
   return GetEIncFromEResOfIon(Z, A, Eres, e, isoAmat);
}

Double_t KVedaLossMaterial::GetEIncFromDeltaEOfIon(Int_t Z, Int_t A, Double_t DeltaE, Double_t e, enum KVIonRangeTable::SolType type, Double_t isoAmat)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material,
   GetDeltaEFunction(e, Z, A, isoAmat);
   Double_t e1,e2;
   fDeltaE->GetRange(e1,e2);
   switch(type){
      case KVIonRangeTable::kEmin:
         e2=GetEIncOfMaxDeltaEOfIon(Z,A,e,isoAmat);
         break;
      case KVIonRangeTable::kEmax:
         e1=GetEIncOfMaxDeltaEOfIon(Z,A,e,isoAmat);
         break;
   }
   return fDeltaE->GetX(DeltaE,e1,e2);
}

Double_t KVedaLossMaterial::GetLinearEIncFromDeltaEOfIon(Int_t Z, Int_t A, Double_t deltaE, Double_t e, enum KVIonRangeTable::SolType type,
                                                         Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculates incident energy (in MeV) of an ion (Z,A) from energy loss DeltaE (MeV) in thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material,
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.

   SetTemperatureAndPressure(T, P);
   e *= GetDensity();
   return GetEIncFromDeltaEOfIon(Z, A, deltaE, e, type, isoAmat);
}

Double_t KVedaLossMaterial::GetPunchThroughEnergy(Int_t Z, Int_t A, Double_t e, Double_t isoAmat)
{
   // Calculate incident energy (in MeV) for ion (Z,A) for which the range is equal to the
   // given thickness e (in g/cm**2). At this energy the residual energy of the ion is (just) zero,
   // for all energies above this energy the residual energy is > 0.
   // Give Amat to change default (isotopic) mass of material.
   
   return GetRangeFunction(Z,A,isoAmat)->GetX(e);
}

Double_t KVedaLossMaterial::GetLinearPunchThroughEnergy(Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t T, Double_t P) 
{
   // Calculate incident energy (in MeV) for ion (Z,A) for which the range is equal to the
   // given thickness e (in cm). At this energy the residual energy of the ion is (just) zero,
   // for all energies above this energy the residual energy is > 0.
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   SetTemperatureAndPressure(T, P);
   e *= GetDensity();
   return GetPunchThroughEnergy(Z,A,e,isoAmat);
}

Double_t KVedaLossMaterial::GetMaxDeltaEOfIon(Int_t Z, Int_t A, Double_t e, Double_t isoAmat)
{
   // Calculate maximum energy loss (in MeV) of ion (Z,A) in given thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material.
   
   return GetDeltaEFunction(e,Z,A,isoAmat)->GetMaximum();
}

Double_t KVedaLossMaterial::GetEIncOfMaxDeltaEOfIon(Int_t Z, Int_t A, Double_t e, Double_t isoAmat)
{
   // Calculate incident energy (in MeV) corresponding to maximum energy loss of ion (Z,A)
   // in given thickness e (in g/cm**2).
   // Give Amat to change default (isotopic) mass of material.
   
   return GetDeltaEFunction(e,Z,A,isoAmat)->GetMaximumX();
}

Double_t KVedaLossMaterial::GetLinearMaxDeltaEOfIon(Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculate maximum energy loss (in MeV) of ion (Z,A) in given thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   SetTemperatureAndPressure(T, P);
   e *= GetDensity();
   return GetMaxDeltaEOfIon(Z,A,e,isoAmat);
}

Double_t KVedaLossMaterial::GetLinearEIncOfMaxDeltaEOfIon(Int_t Z, Int_t A, Double_t e, Double_t isoAmat, Double_t T, Double_t P)
{
   // Calculate incident energy (in MeV) corresponding to maximum energy loss of ion (Z,A)
   // in given thickness e (in cm).
   // Give Amat to change default (isotopic) mass of material.
   // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
   
   SetTemperatureAndPressure(T, P);
   e *= GetDensity();
   return GetEIncOfMaxDeltaEOfIon(Z,A,e,isoAmat);
}

TGeoMaterial* KVedaLossMaterial::GetTGeoMaterial() const
{
	// Create and return pointer to a TGeoMaterial or TGeoMixture (for compound materials)
	// with the properties of this material.
	// gGeoManager must exist.
	
	TGeoMaterial* gmat=0x0;
	if(!gGeoManager) return gmat;
	if(IsCompound()){
		gmat = new TGeoMixture(GetTitle(), GetComposition()->GetEntries(), GetDensity());
		TIter next(GetComposition());
		TNamed* el;
		while((el = (TNamed*)next())){
			TGeoElement* gel = gGeoManager->GetElementTable()->FindElement(el->GetName());
			float poids = atof(el->GetTitle());
			((TGeoMixture*)gmat)->AddElement(gel, poids);
		}
	}
	else
	{
		gmat = new TGeoMaterial( GetTitle(), GetMass(), GetZ(), GetDensity() );
	}
   // set state of material
   if(IsGas()) gmat->SetState(TGeoMaterial::kMatStateGas);
   else gmat->SetState(TGeoMaterial::kMatStateSolid);
	return gmat;
}
