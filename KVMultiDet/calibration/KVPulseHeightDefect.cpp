/*
$Id: KVPulseHeightDefect.cpp,v 1.5 2008/10/07 15:55:20 franklan Exp $
$Revision: 1.5 $
$Date: 2008/10/07 15:55:20 $
*/

//Created by KVClassFactory on Mon Feb 19 17:32:55 2007
//Author: franklan

#include "KVPulseHeightDefect.h"
#include "TMath.h"

ClassImp(KVPulseHeightDefect)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVPulseHeightDefect</h2>
<h4>Silicon PHD described by Moulton formula</h4>

<p>
The static TF1 "fMoulton" calculates the PHD of a particle of incident energy
E and charge Z calculated according to Moulton's formula:
</p>

<p>
log_10(PHD) = b(Z) + a(Z)*log_10(E)  for Z > Zmin,<br>
PHD = 0  for Z <= Zmin<br>
 with  a(Z) = a_1*(Z**2/1000) + a_2<br>
         b(Z) = b_1*(100/Z) + b_2<br>
           E = incident energy of particle
</p>
<p>The five parameters a_1, a_2, b_1, b_2, Zmin can be set using method<br>
SetParameters(a_1,a_2,b_1,b_2, Zmin)
</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Double_t KVPulseHeightDefect::PHDMoulton(Double_t * x, Double_t * par)
{
   //Returns Moulton PHD for given E and Z:
   // 
   // log_10(PHD) = b(Z) + a(Z)*log_10(E), for Z > Zmin
   //
   //               PHD  = 0,  for Z <= Zmin
   //
   //  with  a(Z) = a_1*(Z**2/1000) + a_2
   //          b(Z) = b_1*(100/Z) + b_2
   //            E = incident energy of particle
   //
   // x[0] = E (MeV)
   // par[0] = Z
   
   Int_t Z = par[0];
   if( Z <= fZmin ) return 0;
   Double_t a = a_1*Z*Z/1000. + a_2;
   Double_t b = b_1*100./Z + b_2;
   return (TMath::Power(10,b)*TMath::Power(x[0],a));
}

//___________________________________________________________________//

void KVPulseHeightDefect::init()
{
   //default initialisations
   SetType("Pulse Height Defect");
   SetZ(1);
   fMoulton = fDeltaEphd = 0;
   a_1=a_2=b_1=b_2=0.;
   fZmin=0;
}

KVPulseHeightDefect::KVPulseHeightDefect():KVCalibrator(5)
{
   //Default constructor
   init();
}

KVPulseHeightDefect::KVPulseHeightDefect(KVDetector*d):KVCalibrator(5)
{
   //Associate PHD calculation to detector
   init();
   SetDetector(d);
}

KVPulseHeightDefect::~KVPulseHeightDefect()
{
   //Destructor
}

//___________________________________________________________________________

Double_t KVPulseHeightDefect::Compute(Double_t energy) const
{
   //Calculate the pulse height defect (in MeV) for a given incident energy
   //and a given Z (the Z  of the particle should be set first using method SetZ)
   //The Moulton formula is used:
   //
   // log_10(PHD) = b(Z) + a(Z)*log_10(E)
   //
   //               PHD  = 0,  for Z <= Zmin
   //
   //  with  a(Z) = a_1*(Z**2/1000) + a_2
   //          b(Z) = b_1*(100/Z) + b_2
   //            E = incident energy of particle  

   return const_cast<KVPulseHeightDefect*>(this)->GetMoultonPHDFunction(GetZ())->Eval(energy);
}

//___________________________________________________________________________

Double_t KVPulseHeightDefect::operator() (Double_t energy)
{
   //Same as Compute()

   return Compute(energy);
}

TF1* KVPulseHeightDefect::GetELossFunction(Int_t Z, Int_t A)
{
   // Return pointer to TF1 giving energy loss in active layer of detector minus
   // the pulse height defect for a given nucleus (Z,A).
   
   if(!fDeltaEphd){
      fDeltaEphd = new TF1(Form("KVPulseHeightDefect:%s:ELossActive", GetDetector()->GetName()),
           this, &KVPulseHeightDefect::ELossActive, 0., 1.e+04, 2, "KVPulseHeightDefect", "ELossActive");
      fDeltaEphd->SetNpx( gEnv->GetValue("KVPulseHeightDefect.EnergyLoss.Npx", 20) );
   }
   fDeltaEphd->SetParameters((Double_t)Z, (Double_t)A);
   GetMoultonPHDFunction(Z);
   return fDeltaEphd;
}

Double_t KVPulseHeightDefect::ELossActive(Double_t *x, Double_t *par)
{
   // Calculate energy lost in active layer of detector minus the Moulton PHD
   // x[0] = incident energy
   // par[0] = Z
   // par[1] = A
   
   Double_t e = x[0];
   TIter next(GetDetector()->GetListOfAbsorbers()); KVMaterial* mat;
   while( (mat = (KVMaterial*)next()) != GetDetector()->GetActiveLayer() ){
      // calculate energy losses in absorbers before active layer
         e = mat->GetERes(par[0], par[1], e);     //residual energy after layer
         if (e < KVDETECTOR_MINIMUM_E)
            return 0.;          // return 0 if particle stops in layers before active layer
   }
   // calculate energy loss in active layer
   Double_t dE = mat->GetDeltaE(par[0], par[1], e);
   // calculate Moulton PHD corresponding to incident energy in active layer
   Double_t phd = PHDMoulton(&e, par);
   
   return dE - phd; 
}

   TF1* KVPulseHeightDefect::GetMoultonPHDFunction(Int_t Z)
   {
      // Create TF1* fMoulton if not already done
   
      if(!fMoulton) {
         fMoulton = new TF1(Form("MoultonPHD:%s", GetDetector()->GetName()),
            this, &KVPulseHeightDefect::PHDMoulton, 0., 1.e+04, 1, "KVPulseHeightDefect", "PHDMoulton");
         a_1 = GetParameter(0);
         a_2 = GetParameter(1);
         b_1 = GetParameter(2);
         b_2 = GetParameter(3);
         fZmin = (Int_t)GetParameter(4);
         fMoulton->SetNpx(500);
      }
      fMoulton->SetParameter(0,Z);
      return fMoulton;
   }

//___________________________________________________________________________

Double_t KVPulseHeightDefect::Invert(Double_t energy)
{
   //Given the PHD (in MeV) of a particle of charge Z
   //(set using SetZ() method), this method inverts the Moulton formula
   //in order to find the incident energy of the particle.

   const_cast<KVPulseHeightDefect*>(this)->GetMoultonPHDFunction(GetZ());
	Double_t xmin, xmax; fMoulton->GetRange(xmin,xmax);
   return fMoulton->GetX(energy,xmin,xmax);
}
