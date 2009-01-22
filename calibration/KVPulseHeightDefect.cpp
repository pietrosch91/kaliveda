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

Double_t PHDMoulton(Double_t * x, Double_t * par)
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
   // par[1] = a_1
   // par[2] = a_2
   // par[3] = b_1
   // par[4] = b_2
   // par[5] = Zmin
   
   if( par[0] <= par[5] ) return 0;
   Double_t Z = par[0];
   Double_t a = par[1]*Z*Z/1000. + par[2];
   Double_t b = par[3]*100./Z + par[4];
   return (TMath::Power(10,b)*TMath::Power(x[0],a));
}

TF1 KVPulseHeightDefect::fMoulton("fMoulton", PHDMoulton, 0., 5000., 6);

//___________________________________________________________________//

void KVPulseHeightDefect::init()
{
   //default initialisations
   SetType("Pulse Height Defect");
   SetZ(1);
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

   const_cast<KVPulseHeightDefect*>(this)->SetPars();
   return fMoulton.Eval(energy);
}

//___________________________________________________________________________

void KVPulseHeightDefect::SetPars()
{
   // Set parameters of TF1 prior to calculation (PRIVATE)
   Double_t par[6];
   for (register int i = 0; i < 5; i++) par[i+1] = GetParameter(i);
   par[0] = (Double_t) GetZ();
   fMoulton.SetParameters(par);
}

//___________________________________________________________________________

Double_t KVPulseHeightDefect::operator() (Double_t energy)
{
   //Same as Compute()

   return Compute(energy);
}

//___________________________________________________________________________

Double_t KVPulseHeightDefect::Invert(Double_t energy)
{
   //Given the PHD (in MeV) of a particle of charge Z
   //(set using SetZ() method), this method inverts the Moulton formula
   //in order to find the incident energy of the particle.

   const_cast<KVPulseHeightDefect*>(this)->SetPars();
	Double_t xmin, xmax; fMoulton.GetRange(xmin,xmax);
   return fMoulton.GetX(energy,xmin,xmax);
}
