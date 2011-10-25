//Created by KVClassFactory on Mon Jan  3 10:50:53 2011
//Author: chbihi

#include "KVLightEnergyCsIVamos.h"
#include "TMath.h"
#include "KVCsI.h"

ClassImp(KVLightEnergyCsIVamos)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVLightEnergyCsIVamos</h2>
<h4>Specific light-Energy Function for Vamos CsI</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Double_t CalculLumiereVamos(Double_t * x, Double_t * par)
{
   //Calcul de la lumiere totale a partir de Z, A d'une particule et son energie
   // 
   // x[0] = energie (MeV)
   // par[0] = a1
   // par[1] = a2
   // par[2] = a3 =0
   // par[3] = Z
   // par[4] = A

   Double_t Z = par[3];
   Double_t A = par[4];
    Double_t w=A*Z*Z;
   Double_t energie = x[0]/w;
   Double_t lumcalc = par[0] * (energie - par[1]*TMath::Log(1.+energie/par[1]));
   
   return lumcalc*w;
}

//__________________________________________________________________________
//KVLightEnergyCsIVamos::KVLightEnergyCsIVamos()
//         : fLightVamos("fLight_CsI", CalculLumiereVamos, 0., 10000., 5)
TF1 KVLightEnergyCsIVamos::fLightVamos("fLight_CsI", CalculLumiereVamos, 0., 10000., 6);

KVLightEnergyCsIVamos::KVLightEnergyCsIVamos()
{
   // Default constructor
}
  
KVLightEnergyCsIVamos::KVLightEnergyCsIVamos(KVDetector * kvd)
        : KVLightEnergyCsI(kvd)
{
   // ctor avec adresse du detecteur associe
}

KVLightEnergyCsIVamos::~KVLightEnergyCsIVamos()
{
   // Destructor
}
//___________________________________________________________________________
Double_t KVLightEnergyCsIVamos::Compute(Double_t light) const
{
   // Calculate the calibrated energy (in MeV) for a given total light output.
   // The Z and A of the particle should be given first using SetZ, SetA.
   // By default, Z=A=1 (proton).
   //
   // This is done by inversion of the light-energy function using TF1::GetX.

   //set parameters of light-energy function
   Double_t par[5];
   for (int i = 0; i < 3; i++)
   par[i] = GetParameter(i);
   par[3] =  (Double_t)fZ;
   par[4] =  (Double_t)fA;
  
   //const_cast<KVLightEnergyCsIVamos*>(this)->fLightVamos.SetParameters(par);
   fLightVamos.SetParameters(par);
   
   /*
   cout<<"====	This	===="<<endl;
   this->Print();
   cout<<"====	Vamos	===="<<endl;
   fLightVamos.Print();
   */
   //invert light vs. energy function to find energy
   Double_t xmin, xmax; fLightVamos.GetRange(xmin,xmax);
   Double_t energy = fLightVamos.GetX(light, xmin, xmax);

   return energy;
}

//___________________________________________________________________________
Double_t KVLightEnergyCsIVamos::Invert(Double_t energy)
{
   //Given the calibrated (or simulated) energy in MeV,
   //calculate the corresponding total light output according to the
   //calibration parameters (useful for filtering simulations).

   //set parameters of light-energy function
   Double_t par[5];
   for (int i = 0; i < 3; i++)
      par[i] = GetParameter(i);
   par[3] =  (Double_t)fZ;
   par[4] =  (Double_t)fA;
   fLightVamos.SetParameters(par);

   return fLightVamos.Eval(energy);
}

