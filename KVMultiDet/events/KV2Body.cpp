/***************************************************************************
                          KV2Body.cpp  -  description
                             -------------------
    begin                : 28/11/2003
    copyright            : (C) 2003 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KV2Body.cpp,v 1.4 2009/02/02 13:52:29 ebonnet Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KV2Body.h"
#include "Riostream.h"
#include "TROOT.h"
#include "KVTelescope.h"

ClassImp(KV2Body)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KV2Body
//
//This class calculates various useful characteristics of binary nuclear reactions.
//It is based on a Fortran programme by Jean-Luc Charvet (cibi.for).
//It allows to calculate:
//      - centre of mass recoil velocity and available energy
//      - laboratory grazing angle of projectile and target
//      - maximum laboratory scattering angles
//      - Q-values
//      - CM and laboratory Rutherford cross-sections for elastic scattering
//      - CM and laboratory angles of ejectiles after elastic or inelastic scattering
//The values are calculated using relativistic kinematics.
//
//
//1. Defining the entrance channel.
//
//Either use the constructor with arguments to specify projectile and target:
//      KV2Body kin( proj, targ );//'proj' and 'targ' are KVNuclei
//or declare them afterwards:
//      KV2Body kin;
//      kin.SetProjectile( proj ); kin.SetTarget( targ );
//One can also specify the projectile & target using their Z and A:
//      kin.SetProjectile( z1, a1 );
//      kin.SetTarget( z2, a2 );
//(In each case a KVNucleus is created which will be deleted with the KV2Body object).
//If you need to further define the properties of these nuclei, you can get a pointer to
//them using GetNucleus(int i):
//      kin.GetNucleus(1)->SetMomentum(...);//set momentum of projectile
//
//2. Defining the exit channel
//
//You do not need to define an exit channel, however if you want information on
//the outgoing nuclei this can be done with the ctor with arguments at the same time as giving
//projectile and target:
//      KV2Body kin( proj, targ, QP );// QP is a KVNucleus
//      KV2Body kin( proj, targ, QP, Ediss );
//In the 2nd example, we set the amount of dissipated energy for the reaction
//(by default Ediss=0, i.e. elastic scattering).
//The other methods are:
//      kin.SetOutgoing( QP );
//      kin.SetEDiss( Ediss );  OR  kin.SetExcitEnergy( Ediss );
//
//3. Calculating and obtaining reaction information
//
//Once the entrance channel (and, if necessary, the exit channel) is(are) defined,
//the kinematical calculation is carried out by:
//      kin.CalculateKinematics();
//For a general print-out of the reaction characteristics:
//      kin.Print();
//      kin.Print("lab");//laboratory scattering angles and energies
//      kin.Print("ruth");//Rutherford scattering angles, energies and cross-sections
//
//4. Extracting information on Quasi-Projectile and/or target
//
//Several methods are defined and calculate energy/velocity/angle in C or lab frame
//for projectile (3) and/or target (4)
//The two arguments are : the angle and the angular range of which nucleus you are interested in 
//
//For example : the Double_t GetThetaCMTarget(Double_t ThetaLab,Int_t OfNucleus=3)
//give the polar angle in the CM of reaction of the target
//		depending on the polar angle in the lab of the projectile (OfNucleus=3) or of the target (OfNucleus=4)
//
//
//_____________________________________________________________________________________________________________
void KV2Body::init()
{
   //Default initialisations
   WLT = WCT = WC3 = WC4 = 0.;
   for (int i = 0; i < 5; i++) {
      VC[i] = 0.;
      EC[i] = 0.;
		K[i] = 0.;  
   	TETAMAX[i] = 0.;  
   	TETAMIN[i] = 0.;  
   }
   fEDiss = 0.0;
   fDeleteTarget = kFALSE;
   fDeleteProj = kFALSE;
   fDeleteN4 = kFALSE;
   fKoxReactionXSec = 0;
   fEqbmChargeState = 0;
}

KV2Body::KV2Body():fNuclei(4, 1)
{
   //default ctor
   init();
}

KV2Body::KV2Body(KVNucleus * proj, KVNucleus * cib, KVNucleus * proj_out, Double_t Ediss):fNuclei(4,
        1)
{
   //Set up calculation with projectile & target nuclei, outgoing projectile
   //nucleus and energy dissipated in reaction.
   //Only projectile and target have to be specified.
   //By default the dissipated energy is zero (elastic reaction).

   init();
   fNuclei[1] = proj;
   fNuclei[2] = cib;
   fEDiss = Ediss;
   if (proj_out) {
      SetOutgoing(proj_out);
   }
}

void KV2Body::SetTarget(KVNucleus * targ)
{
   //Set target for reaction. The KVNucleus will not be deleted by ~KV2Body.
   fNuclei[2] = targ;
}

void KV2Body::SetTarget(Int_t z, Int_t a)
{
   //Set target for reaction
   //Creates a new KVNucleus which will be deleted by ~KV2Body.
   fNuclei[2] = new KVNucleus(z, a);
   fDeleteTarget = kTRUE;
}

void KV2Body::SetProjectile(KVNucleus * proj)
{
   //Set projectile for reaction. The KVNucleus will not be deleted by ~KV2Body.
   fNuclei[1] = proj;
}

void KV2Body::SetProjectile(Int_t z, Int_t a)
{
   //Set projectile for reaction
   //Creates a new KVNucleus which will be deleted by ~KV2Body.
   fNuclei[1] = new KVNucleus(z, a);
   fDeleteProj = kTRUE;
}

KV2Body::~KV2Body()
{
   //Any nuclei created by the object are deleted
   if (fDeleteN4)
      delete GetNucleus(4);
   if (fDeleteTarget)
      delete GetNucleus(2);
   if (fDeleteProj)
      delete GetNucleus(1);
   fNuclei.Clear();
   if(fKoxReactionXSec) delete fKoxReactionXSec;
   if(fEqbmChargeState) delete fEqbmChargeState;
}

//_____________________________________________________________________________

Double_t KV2Body::GetVelocity(Double_t mass, Double_t E)
{
   //Static function, calculates relativistic velocity (in cm/ns) from rest mass and total
   //energy (i.e. KE + mass) 'E' for any particle

   Double_t p = TMath::Sqrt(E * E - mass * mass);
   return KVParticle::C() * p / E;
}

//_____________________________________________________________________________

void KV2Body::SetOutgoing(KVNucleus * proj_out)
{
   //Set outgoing projectile-like nucleus' properties.
   //Use this for reactions where both projectile and target are defined for
   //the entrance channel and the properties of the outgoing target-like nucleus
   //can be deduced from mass, charge and momentum/energy conservation after
   //defining the projectile-like nucleus.
   //The KVNucleus will not be deleted by ~KV2Body.
   fNuclei[3] = proj_out;
   Set4thNucleus();
}

//_____________________________________________________________________________

void KV2Body::SetOutgoing(Int_t inuc, KVNucleus * nuc)
{
   //Set one of the two nuclei in the exit channel
   //Use this e.g. for reactions where only one nucleus is defined in the entrance
   //channel (i.e. fission or evaporation process)
   //The index 'inuc' can be either 3 (projectile-like) or 4 (target-like).
   if (inuc < 3 || inuc > 4) {
      Warning("SetOutgoing(Int_t inuc, KVNucleus *nuc)",
              "inuc must be equal to 3 or 4");
      return;
   }
   fNuclei[inuc] = nuc;
}

//_____________________________________________________________________________

void KV2Body::Set4thNucleus()
{
   //Private method, used to deduce 4th nucleus (target-like) from projectile, target
   //and outgoing projectile using conservation of mass, momentum and energy.
   //This nucleus will be deleted with the object.

   KVNucleus sum = *GetNucleus(1) + *GetNucleus(2);
   if (GetNucleus(4))
      delete GetNucleus(4);
   KVNucleus *tmp4 = new KVNucleus(sum - *GetNucleus(3));
   fDeleteN4 = kTRUE;
   if (!tmp4->GetZ() && !tmp4->GetA()) {
      delete tmp4;
      tmp4 = 0;
      fDeleteN4 = kFALSE;
   }
   fNuclei[4] = tmp4;
}

//_____________________________________________________________________________

KVNucleus *KV2Body::GetNucleus(Int_t i) const
{
   //Return pointer to nucleus i (1 <= i <= 4)
   // Entrance channel nuclei .....  i=1 : projectile  i=2 : target
   // Exit channel nuclei .....  i=3 : projectile-like  i=4 : target-like

   if (i > 0 && i < 5)
      return (KVNucleus *) fNuclei[i];
   Warning("GetNucleus(Int_t i)", "Index i out of bounds, i=%d", i);
   return 0;
}

//_____________________________________________________________________________

Double_t KV2Body::GetQReaction() const
{
   //Calculate Q-value for reaction, including dissipated (excitation) energy

   if (!GetNucleus(3)) {
      Warning("GetQReaction", "Parameters for outgoing nuclei not set");
      return 0.0;
   }
   Double_t QR = GetQGroundStates() - fEDiss;

   return QR;
}

//_______________________________________________________________________________

Double_t KV2Body::GetQGroundStates() const
{
   //Calculate Q-value for reaction, assuming all nuclei in ground state

   if (!GetNucleus(3)) {
      Warning("GetQGroundStates",
              "Parameters for outgoing nuclei not set");
      return 0.0;
   }
   Double_t QGG =
       GetNucleus(1)->GetMassExcess() +
       (GetNucleus(2) ? GetNucleus(2)->GetMassExcess() : 0.)
       - GetNucleus(3)->GetMassExcess() -
       (GetNucleus(4) ? GetNucleus(4)->GetMassExcess() : 0.);
   return QGG;
}

//________________________________________________________________________________

Double_t KV2Body::GetCMEnergy() const
{
   //Return available kinetic energy in centre of mass

   return WCT - (GetNucleus(1)->GetMass() +
                 (GetNucleus(2) ? GetNucleus(2)->GetMass() : 0.));
}

//_____________________________________________________________________________
Double_t KV2Body::GetMaxAngleLab(Int_t i) const
{
   //Returns maximum scattering angle in lab for nuclei i=3 (quasiproj)
   //and i=4 (quasitarget)
   if (i < 3 || i> 4 ) {
      Warning("GetMaxAngleLab(Int_t i)",
              "Returns maximum scattering angle in lab for nuclei i=3 (quasiproj) and i=4 (quasitarget)");
      return 0.;
   }
   return TETAMAX[i];
}

//_____________________________________________________________________________
Double_t KV2Body::GetMinAngleLab(Int_t i) const
{
   //Returns minimum scattering angle in lab for nuclei i=3 (quasiproj)
   //and i=4 (quasitarget)
   if (i < 3 || i> 4 ) {
      Warning("GetMinAngleLab(Int_t i)",
              "Returns minimum scattering angle in lab for nuclei i=3 (quasiproj) and i=4 (quasitarget)");
      return 0.;
   }
   return TETAMIN[i];
}

//_____________________________________________________________________________

TVector3 KV2Body::GetCMVelocity() const
{
   //Return velocity of centre of mass of reaction

   return VCM;
}

//_____________________________________________________________________________

Double_t KV2Body::GetCMVelocity(Int_t i) const
{
   //Return velocity of nucleus "i" in the centre of mass of the reaction
   // Entrance channel nuclei .....  i=1 : projectile  i=2 : target
   // Exit channel nuclei .....  i=3 : projectile-like  i=4 : target-like
   return VC[i];
}

//_____________________________________________________________________________

Double_t KV2Body::GetLabGrazingAngle(Int_t i) const
{
   //Calculate laboratory grazing angle.
   // i = 1 (default) : projectile
   // i = 2           : target

   if (i < 1 || i > 2) {
      Warning("GetLabGrazingAngle(Int_t i)",
              "i should be 1 (proj) or 2 (targ)");
      return 0.0;
   }
   if (!GetNucleus(2)) {
      Warning("GetLabGrazingAngle(Int_t i)",
              "No target defined for reaction");
      return 0.0;
   }
   Double_t RP = 1.28 * TMath::Power(GetNucleus(1)->GetA(), 1. / 3.)
       - 0.76 + 0.8 * TMath::Power(GetNucleus(1)->GetA(), -1. / 3.);
   Double_t RT = 1.28 * TMath::Power(GetNucleus(2)->GetA(), 1. / 3.)
       - 0.76 + 0.8 * TMath::Power(GetNucleus(2)->GetA(), -1. / 3.);
   Double_t CP = RP * (1. - TMath::Power(1. / RP, 2.));
   Double_t CT = RT * (1. - TMath::Power(1. / RT, 2.));
   Double_t RINT = CP + CT + 4.49 - (CT + CP) / 6.35;
   Double_t BAR =
       1.44 * GetNucleus(1)->GetZ() * GetNucleus(2)->GetZ() / RINT;
   if (GetCMEnergy() < BAR) {
      //below Coulomb barrier
      switch (i) {
      case 1:
         return 180.;
         break;
      case 2:
         return 90.;
         break;
      }
   }
   Double_t X = 2. * RINT * GetCMEnergy();
   Double_t Y =
       X / (GetNucleus(1)->GetZ() * GetNucleus(2)->GetZ() * 1.44) - 1.;
   Double_t U = 1. / Y;
   Double_t GR = 2. * TMath::ASin(U);
   Double_t GPART = TMath::Pi() - GR;
   Double_t ARAZ =
       TMath::Sin(GR) / (TMath::Cos(GR) +
                         GetNucleus(1)->GetA() / (1.0 *
                                                  GetNucleus(2)->GetA()));
   Double_t GRAZ = TMath::ATan(ARAZ);
   if (GRAZ <= 0.)
      GRAZ += TMath::Pi();

   switch (i) {
   case 1:                     //projectile
      return (GRAZ * TMath::RadToDeg());
      break;
   case 2:                     //target
      return (GPART * TMath::RadToDeg() / 2.);
      break;
   }
   return -99.;
}

//_____________________________________________________________________________

void KV2Body::CalculateKinematics()
{
   //Called to make kinematics calculation for all nuclei, use once properties of
   //entrance-channel (and, if necessary, exit-channel) nuclei have been defined.
   //If no exit-channel nuclei are defined, only kinematical properties of entrance channel
   //are calculated.

   KVNucleus* Nuc1 = GetNucleus(1);
   KVNucleus* Nuc2 = GetNucleus(2);
   //total energy (T + m) of entrance channel
   WLT = Nuc1->E() + (Nuc2 ? Nuc2->E() : 0.);
   //velocity of CM
   VCM = Nuc1->GetMomentum() * (KVParticle::C() / WLT);
   //beta of CM
   BCM = VCM.Mag() / KVParticle::C();
   //total energy in CM
   WCT = (GetCMGamma() > 0.0 ? WLT / GetCMGamma() : 0.);
   if (WCT == 0.0)
      return;

   //total energy proj in CM
   Double_t WC1 =
       ((Nuc1->GetMass() -
         (Nuc2 ? Nuc2->GetMass() : 0.))
        * (Nuc1->GetMass() +
           (Nuc2 ? Nuc2->GetMass() : 0.)) / (2. * WCT)) +
       WCT / 2.;
   //kinetic energy proj in CM
   EC[1] = WC1 - Nuc1->GetMass();
   //tot E target in CM
   Double_t WC2 = WCT - WC1;
   //kinetic energy targ in CM
   EC[2] = (Nuc2 ? WC2 - Nuc2->GetMass() : 0.);
   VC[1] = GetVelocity(Nuc1->GetMass(), WC1);
   VC[2] =
       (Nuc2 ? GetVelocity(Nuc2->GetMass(), WC2) : 0.);

   //no exit channel defined - stop here
   if (!GetNucleus(3))
      return;

   Double_t AM3 = GetNucleus(3)->GetMass();
   Double_t AM4 = (GetNucleus(4) ? GetNucleus(4)->GetMass() : 0.0);
   //total cm energy of nucleus 3 (quasiproj)
   WC3 = (WCT - GetEDiss()) / 2. + (AM3 - AM4) * (AM3 + AM4)
       / (2. * (WCT - GetEDiss()));
   VC[3] = GetVelocity(AM3, WC3);
   //cm kinetic energy
   EC[3] = WC3 - AM3;
   K[3] = VCM.Mag() / VC[3];
   Double_t T3MAX = 0.;
   if (K[3] < 1.)
      T3MAX = TMath::Pi();
   if (K[3] == 1.)
      T3MAX = TMath::PiOver2();
   if (K[3] > 1.)
      T3MAX = TMath::ATan((1. / TMath::Sqrt(K[3] * K[3] - 1.)) / GetCMGamma());
   TETAMAX[3] = T3MAX * TMath::RadToDeg();

   if (!GetNucleus(4))
      return;                   //only valid for binary channels
   //total cm energy of nucleus 4 (quasitarg)
   WC4 = WCT - GetEDiss() - WC3;
   VC[4] = GetVelocity(AM4, WC4);
   //cm kinetic energy
   EC[4] = WC4 - AM4;
   K[4] = VCM.Mag() / VC[4];
	TETAMIN[4] = GetThetaLabTarget(TETAMAX[3]);

   Double_t T4MAX = 0.;
   if (K[4] < 1.)
      T4MAX = TMath::Pi();
   if (K[4] == 1.)
      T4MAX = TMath::PiOver2();
   if (K[4] > 1.)
      T4MAX = TMath::ATan((1. / TMath::Sqrt(K[4] * K[4] - 1.)) / GetCMGamma());
   if (TMath::Abs(GetEDiss()) < 1.E-08 && K[4] < 1.)
      T4MAX = TMath::PiOver2();
   TETAMAX[4] = T4MAX * TMath::RadToDeg();
	TETAMIN[3] = GetThetaLabTarget(TETAMAX[4]);
}

//_____________________________________________________________________________

Double_t KV2Body::GetCMEnergy(Int_t i) const
{
   //Returns kinetic energy of nucleus "i" in the centre of mass of the reaction
   // Entrance channel nuclei .....  i=1 : projectile  i=2 : target
   // Exit channel nuclei .....  i=3 : projectile-like  i=4 : target-like
   return EC[i];
}

//_____________________________________________________________________________

void KV2Body::Print(Option_t * opt) const
{
   // Print out characteristics of reaction.
   //
   // If a two-body exit channel has been defined, you can use the following options:
   //    opt = "ruth" :  list Rutherford scattering cross-sections as a function of angle in laboratory frame
   //    opt = "lab"  :  list energies and angles in laboratory frame

   cout << " ***** REACTION    " << GetNucleus(1)->GetSymbol();
   if (GetNucleus(2))
      cout << "  +  " << GetNucleus(2)->GetSymbol();
   if (GetNucleus(3)) {
      cout << " --->   " << GetNucleus(3)->GetSymbol();
   }
   if (GetNucleus(4))
      cout << "  +  " << GetNucleus(4)->GetSymbol();
   cout << "    ******" << endl;

   cout << "  E.LAB = " << GetNucleus(1)->GetEnergy() << " MEV";
   if (GetNucleus(3)) {
      cout << "     QGG = " << GetQGroundStates() << " MEV";
   }
   cout << endl;
   cout << "  E.EXC = " << GetExcitEnergy() << " MEV";
   if (GetNucleus(3)) {
      cout << " ==> Q-REACTION = " << GetQReaction() << " MEV";
   }
   cout << endl;
   cout << " AVAILABLE ENERGY IN C.M. : ECM = " << GetCMEnergy() <<
       " MEV  (" << GetCMEnergy() / (GetNucleus(1)->GetA() +
                                     (GetNucleus(2) ? GetNucleus(2)->
                                      GetA() : 0.)) << " MEV/A)" << endl;
   cout << " PROJECTILE VELOCITY IN LAB " << GetNucleus(1)->GetV().Mag()
       << " CM/NS  ( " << GetNucleus(1)->Beta() << " * C )" << endl;
   cout << " VELOCITY OF C.M.           " << GetCMVelocity().
       Mag() << " CM/NS" << endl;

   for (int i = 1; i <= 4; i++) {
      if (GetNucleus(i))
         cout << " ENERGY - VELOCITY OF NUCLEUS " << i << " IN CM : " <<
             GetCMEnergy(i) << " MEV  " << GetCMVelocity(i) << " CM/NS" <<
             endl;
   }
   if (GetNucleus(3)) {
      cout << " MAXIMUM SCATTERING ANGLE IN LABORATORY" << endl;
      cout << "        THETA #3#   " << GetMaxAngleLab(3) << " DEG." <<
          endl;
      if (GetNucleus(4))
         cout << "        THETA #4#   " << GetMaxAngleLab(4) << " DEG." <<
             endl;
   }
   cout << " GRAZING ANGLE IN LABORATORY : PROJECTILE " <<
       GetLabGrazingAngle(1) << " DEG." << endl;
   if (GetNucleus(2))
      cout << " GRAZING ANGLE IN LABORATORY : TARGET     " <<
          GetLabGrazingAngle(2) << " DEG." << endl;

   if (GetNucleus(3)) {
      Int_t nsteps = 15;

      if (!strcmp(opt, "lab") || !strcmp(opt, "LAB")
          || !strcmp(opt, "Lab")) {
         //laboratory angular distribution
         cout << endl <<
             "                   LABORATORY ANGULAR DISTRIBUTION" << endl
             << endl;
         cout <<
             "   TETA 3    EN.DIF.3        V3       TETA 4     EN.DIF.4     TETA 3"
             << endl;
         cout <<
             "   (LAB)      (LAB)        (LAB)       (LAB)       (LAB)       (C.M)"
             << endl;
         Double_t dtheta = GetMaxAngleLab(3) / nsteps;
         for (int step = 0; step <= nsteps; step++) {
            Double_t theta = dtheta * step;
            printf
                ("   %6.2f     %7.2f      %5.2f      %6.2f      %7.2f     %6.2f\n",
                 theta, GetELabProj(theta), GetVLabProj(theta),
                 GetThetaLabTarget(theta), GetELabTarget(theta),
                 GetThetaCMProj(theta));
         }
      }
      if (!strcmp(opt, "ruth") || !strcmp(opt, "RUTH")
          || !strcmp(opt, "Ruth")) {
         //laboratory angular distribution with Rutherford x-section
         cout << endl <<
             "             RUTHERFORD LABORATORY ANGULAR DISTRIBUTION" <<
             endl << endl;
         cout <<
             "   TETA 3     TETA 3    EN.DIF.3        SIG.RUT.        SIG.RUT."
             << endl;
         cout <<
             "   (LAB)       (CM)       (LAB)       (LAB) (b/sr)    (CM)  (b/sr)"
             << endl;
         //go up to grazing angle
         Double_t dtheta = GetLabGrazingAngle(1) / nsteps;
         for (int step = 0; step < nsteps; step++) {
            Double_t theta = dtheta * (step + 1);
            printf
                ("   %6.2f     %6.2f     %7.2f      %10.4g      %10.4g\n",
                 theta, GetThetaCMProj(theta), GetELabProj(theta),
                 GetXSecRuthLab(theta), GetXSecRuthCM(theta));
         }
      }
   }
}

//______________________________________________________________________________________________
Double_t KV2Body::GetELabProj(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Calculate (quasi)projectile kinetic energy as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle

   if (GetThetaLabProj(ThetaLab,OfNucleus) > TETAMAX[3]) {
      Warning("GetELabProj",
              "Projectile Angle %f is greater than max scattering angle (%f)",
              GetThetaLabProj(ThetaLab,OfNucleus), TETAMAX[3]);
      return 0.;
   }
   Double_t T3CM = GetThetaCMProj(ThetaLab,OfNucleus) * TMath::DegToRad();
   Double_t WL3 =
       WC3 * GetCMGamma() * (1. +
                             BCM * (GetCMVelocity(3) / KVParticle::C()) *
                             TMath::Cos(T3CM));
   return (WL3 - GetNucleus(3)->GetMass());
}

//______________________________________________________________________________________________

Double_t KV2Body::GetVLabProj(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Calculate (quasi)projectile velocity as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle
   Double_t etot = GetELabProj(ThetaLab,OfNucleus) + GetNucleus(3)->GetMass();
   return GetVelocity(GetNucleus(3)->GetMass(), etot);
}

//______________________________________________________________________________________________
Double_t KV2Body::GetThetaCMProj(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Calculate CM projectile scattering angle as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle
	
	Double_t par = OfNucleus;
	return const_cast<KV2Body*>(this)->ThetaCMProj(&ThetaLab,&par);
}

//______________________________________________________________________________________________
Double_t KV2Body::ThetaCMProj(Double_t *x, Double_t* par)
{
   //Calculate CM projectile scattering angle as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle
	Double_t ThetaLab = x[0];
	Int_t OfNucleus = (Int_t)par[0];
	Double_t ThetaL = ThetaLab * TMath::DegToRad();
	Double_t ThetaCM = ThetaL + TMath::ASin(K[OfNucleus] * TMath::Sin(ThetaL));
	if (OfNucleus==3)	return ThetaCM*TMath::RadToDeg();
	else 					return (TMath::Pi() - ThetaCM)*TMath::RadToDeg();
}

//______________________________________________________________________________________________
Double_t KV2Body::GetELabTarget(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Calculate (quasi)target kinetic energy as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle

   if (GetThetaLabProj(ThetaLab,OfNucleus) > TETAMAX[3]) {
      Warning("GetELabTarg",
              "Projectile Angle %f is greater than max scattering angle (%f)",
              GetThetaLabProj(ThetaLab,OfNucleus), TETAMAX[3]);
      return 0.;
   }
   Double_t T4CM = GetThetaCMTarget(ThetaLab,OfNucleus) * TMath::DegToRad();
   Double_t WL4 =
       WC4 * GetCMGamma() * (1. +
                             BCM * (GetCMVelocity(4) / KVParticle::C()) *
                             TMath::Cos(T4CM));
   return (WL4 - GetNucleus(4)->GetMass());
}

//______________________________________________________________________________________________
Double_t KV2Body::GetVLabTarget(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Calculate (quasi)target velocity as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle
   Double_t etot = GetELabTarget(ThetaLab,OfNucleus) + GetNucleus(4)->GetMass();
   return GetVelocity(GetNucleus(4)->GetMass(), etot);
}

//______________________________________________________________________________________________
Double_t KV2Body::GetThetaCMTarget(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Get quasi-target CM angle as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle

	Double_t ThetaL = ThetaLab * TMath::DegToRad();
	Double_t ThetaCM = ThetaL + TMath::ASin(K[OfNucleus] * TMath::Sin(ThetaL));
	if (OfNucleus==4)	return ThetaCM*TMath::RadToDeg();
	else 					return (TMath::Pi() - ThetaCM)*TMath::RadToDeg();
}

//______________________________________________________________________________________________
Double_t KV2Body::GetThetaLabTarget(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Get quasi-target lab angle as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle
   if (OfNucleus==4) return ThetaLab;
	Double_t T4CM = GetThetaCMTarget(ThetaLab,OfNucleus) * TMath::DegToRad();
   Double_t T4L = TMath::ATan(TMath::Sin(T4CM) / (K[4] + TMath::Cos(T4CM)));
   Double_t thlt = TMath::Abs(T4L * TMath::RadToDeg());
   if (thlt > 180.0)
      thlt = 360.0 - thlt;
   return thlt;
}

//______________________________________________________________________________________________
Double_t KV2Body::GetThetaLabProj(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Get quasi-projectile lab angle as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle
	
	Double_t par = OfNucleus;
	return const_cast<KV2Body*>(this)->ThetaLabProj(&ThetaLab,&par);
}

//______________________________________________________________________________________________
Double_t KV2Body::ThetaLabProj(Double_t *x, Double_t *par)
{
   //Get quasi-projectile lab angle as a function of projectile (OfNucleus=3) 
	//or target (OfNucleus=4) lab scattering angle
	Double_t ThetaLab = x[0];
	Int_t OfNucleus = (Int_t)par[0];
   if (OfNucleus==3) return ThetaLab;
	Double_t T3CM = ThetaCMProj(x,par) * TMath::DegToRad();
   Double_t T3L = TMath::ATan(TMath::Sin(T3CM) / (K[3] + TMath::Cos(T3CM)));
   Double_t thlt = TMath::Abs(T3L * TMath::RadToDeg());
   if (thlt > 180.0)
      thlt = 360.0 - thlt;
   return thlt;
}

//______________________________________________________________________________________________
Double_t KV2Body::GetXSecRuthCM(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Calculate Rutherford cross-section (b/sr) in the CM as a
   //function of projectile (OfNucleus=3) or target (OfNucleus=4) lab scattering angle

   Double_t par = OfNucleus;
   return const_cast<KV2Body*>(this)->XSecRuthCM(&ThetaLab,&par);
}

//______________________________________________________________________________________________
Double_t KV2Body::XSecRuthCM(Double_t *x,Double_t *par) 
{
   //Calculate Rutherford cross-section (b/sr) in the CM as a
   //function of projectile (OfNucleus=3) or target (OfNucleus=4) lab scattering angle

   if (!GetNucleus(2)) {
      Warning("GetXSecRuthCM", "No target defined for reaction");
      return 0.;
   }
   Double_t PB =
       1.44 * GetNucleus(1)->GetZ() * GetNucleus(2)->GetZ() /
       GetCMEnergy();
	Double_t T3CM = ThetaCMProj(x,par) * TMath::DegToRad(); 
	Double_t D = 1. / (16. * (TMath::Power(TMath::Sin(T3CM / 2.), 4.)));
   
	return ((TMath::Power(PB, 2.)) * D / 100.);
}

//______________________________________________________________________________________________
Double_t KV2Body::GetXSecRuthLab(Double_t ThetaLab,Int_t OfNucleus) const
{
   //Calculate Rutherford cross-section (b/sr) in the Lab as a
   //function of projectile (OfNucleus=3) or target (OfNucleus=4) lab scattering angle

   Double_t par = OfNucleus;
   return const_cast<KV2Body*>(this)->XSecRuthLab(&ThetaLab, &par);
}

//______________________________________________________________________________________________
Double_t KV2Body::XSecRuthLab(Double_t *x, Double_t *par)
{
   //Calculate Rutherford cross-section (b/sr) in the Lab as a
   //function of projectile (OfNucleus=3) or target (OfNucleus=4) lab scattering angle

   Double_t DSIDTB = XSecRuthCM(x,par);
   
	Double_t T3L = ThetaLabProj(x,par) * TMath::DegToRad();
   Double_t T3CM = ThetaCMProj(x,par) * TMath::DegToRad();
   Double_t RLC = (TMath::Power(TMath::Sin(T3CM), 3.)) /
       ((TMath::Power(TMath::Sin(T3L), 3.)) * GetCMGamma() *
        (1. + K[3] * TMath::Cos(T3CM)));
   if (DSIDTB * RLC<0) {
		Warning("GetXSecRuthLab", "negative value for choosen parameters : %lf %d\n",x[0], (Int_t)par[0]);
		return 0;
	}
	return (DSIDTB * RLC);
}

//______________________________________________________________________________________________
Double_t KV2Body::XSecRuthLabInt(Double_t *x, Double_t *par)
{
   //Rutherford cross-section (b/sr) function in the Lab as a
   //function of projectile (OfNucleus=3) or target (OfNucleus=4) lab scattering angle x[0]
   //including 'sin theta' factor needed for integrating over solid angles.

	return XSecRuthLab(x,par) * TMath::Sin(x[0]*TMath::DegToRad());
}

//______________________________________________________________________________________________
Double_t KV2Body::GetIntegratedXSecRuthLab(KVTelescope* tel,Int_t OfNucleus)
{
   //Calculate Integrated Rutherford cross-section (barns) in the Lab using
	//polar and azimuthal angular range of the given KVTelescope.
	// if (OfNucleus==3) => X-section for scattered projectile
	// if (OfNucleus==4) => X-section for scattered target
	// 
	//The returned value is in barns
	
	return GetIntegratedXSecRuthLab(tel->GetThetaMin(),tel->GetThetaMax(),tel->GetPhiMin(),tel->GetPhiMax(),OfNucleus);
}

//______________________________________________________________________________________________
Double_t KV2Body::GetIntegratedXSecRuthLab(KVDetector* det,Int_t OfNucleus)
{
   //Calculate Integrated Rutherford cross-section (barns) in the Lab using
	//polar and azimuthal angular range of the given KVDetector. These will be taken
	//from the parent KVTelescope of the detector.
	// if (OfNucleus==3) => X-section for scattered projectile
	// if (OfNucleus==4) => X-section for scattered target
	// 
	//The returned value is in barns
	
	KVTelescope*tel=det->GetTelescope();
	if(!det){
	   Error("GetIntegratedXSecRuthLab(KVDetector*,Int_t)",
	      "Detector has no parent telescope: it has not been positioned in a multidetector geometry");
	   return 0;
	}
	return GetIntegratedXSecRuthLab(tel,OfNucleus);
}

//______________________________________________________________________________________________
Double_t KV2Body::GetIntegratedXSecRuthLab(Float_t th1,Float_t th2,Float_t phi1,Float_t phi2,Int_t OfNucleus)
{
   //Calculate Integrated Rutherford cross-section (barns) in the Lab using
	//polar and azimuthal angular range expressed in degree
	// if (OfNucleus==3) This angular range is considered to be the scattered projectile one
	// if (OfNucleus==4) This angular range is considered to be the scattered target one
	// 
	//If phi1 ou phi2 ==-1 the azimuthal width is set to 2pi
	//Else if phi1=phi2 the azimuthal width is set to 1 ie the integral is only on theta
	//
	//The returned value is in barns
	
	if (th2<th1) return 0;
	Double_t dphi=0;
	//azimuthal width expressed in rad
	if (phi1==-1 || phi2==-1) dphi=2*TMath::Pi();
	else if (phi1==phi2) dphi=1;
	else { dphi = phi2-phi1; dphi*=TMath::DegToRad(); }
	Double_t theta_min = 1.;
	Double_t theta_max = 179.;
	if( th1<theta_min) theta_min = th1;
	if(th2>theta_max) theta_max=th2;
	return GetXSecRuthLabIntegralFunc(OfNucleus,theta_min,theta_max)->Integral(th1,th2)*TMath::DegToRad()*dphi;
}

//__________________________________________________________________________________________________

Double_t KV2Body::BassIntBarrier()
{
   // calculate Bass interaction barrier B_int
   // r0 = 1.07 fm
   
   const Double_t r0 = 1.07;
   const Double_t e2 = 1.44;
   Double_t A1third = pow(GetNucleus(1)->GetA(),1./3.);
   Double_t A2third = pow(GetNucleus(2)->GetA(),1./3.);
   Double_t R12 = r0 * (A1third + A2third);
   
   Double_t Bint = GetNucleus(1)->GetZ()*GetNucleus(2)->GetZ()*e2 / (R12 + 2.7)
         - 2.9 * A1third * A2third / (A1third + A2third);
   
   return Bint;
}

//__________________________________________________________________________________________________

Double_t KV2Body::KoxReactionXSec(Double_t* eproj, Double_t*)
{
   // calculate Kox reaction X-section (in barns) for a given lab energy of projectile (in MeV/nucleon)
   // 
   // r0 = 1.05 fm
   // c = 0.6 @ 30MeV/A, 1.4 @ 83 MeV/A => linear interpolation:
   //   c = 0.8/53*eproj + 0.6 - 30*0.8/53
   // uses Bass interaction barrier Bint in the (1 - B/Ecm) term
      
   const Double_t r0 = 1.05;
   const Double_t a = 1.9;
   const Double_t c0 = 0.8/53.;
   const Double_t c1 = 0.6 - 30*c0;
   
   KVNucleus* proj = (KVNucleus*)fNuclei[1];
   proj->SetEnergy(eproj[0]*proj->GetA());
   CalculateKinematics();
   Double_t ECM = GetCMEnergy();

   //printf("C.M. energy = %f\n", ECM);
   Double_t Bint = BassIntBarrier();
   //printf("Barrier = %f\n", Bint);
   Double_t EFac = 1 - Bint/ECM;
   //printf("1 - Bint/Ecm = %f\n", EFac);
   Double_t c = TMath::Max(0., c0*eproj[0] + c1);
   //printf("Kox c-factor = %f\n", c);
   Double_t A1third = pow(proj->GetA(),1./3.);
   Double_t A2third = pow(((KVNucleus*)fNuclei[2])->GetA(),1./3.);
   
   Double_t Xsec = TMath::Pi()*pow(r0,2)*
         pow(( A1third + A2third + a*A1third*A2third/(A1third + A2third) - c ), 2) *
         EFac;
   
   return Xsec/100.;
}

//__________________________________________________________________________________________________

Double_t KV2Body::GetSphereDureReactionXSec(Double_t r0)
{
   // calculate Reaction Cross Section with the "Sphere Dure"
   // approximation
      
   Double_t A1third = pow(((KVNucleus*)fNuclei[1])->GetA(),1./3.);
   Double_t A2third = pow(((KVNucleus*)fNuclei[2])->GetA(),1./3.);
   
   Double_t Xsec = TMath::Pi()*pow(r0,2)*
         pow( A1third + A2third , 2);
   
   return Xsec/100.;
}

//__________________________________________________________________________________________________

Double_t KV2Body::GetBmaxFromReactionXSec(Double_t ReacXsec){
	
	// Deduce the maximum impact parameter (in fm) from a given reaction cross section (in barn)
	// in the approximation Xsec = \int(0,bmax) 2Pi b db
	
	return 10*TMath::Sqrt( ReacXsec/TMath::Pi() );

}



//__________________________________________________________________________________________________

Double_t KV2Body::GetIntegratedXsec(Double_t b1,Double_t b2){
	
	// Integrate the cross section between two impact parameter (in fm)
	// and give the result in barn
	
	return TMath::Pi()*(TMath::Power(b2,2.)-TMath::Power(b1,2.))/100;
	
}

//__________________________________________________________________________________________________

TF1* KV2Body::GetKoxReactionXSecFunc()
{
   // Return pointer to TF1 with Kox reaction X-section in barns as a
   // function of projectile lab energy (in Mev/nucleon) for this reaction.
   // By default the range of the function is [20,100] MeV/nucleon.
   // Change with TF1::SetRange.
   
   if(!fKoxReactionXSec){
      TString name = GetNucleus(1)->GetSymbol();
      name+= " + ";
      name += GetNucleus(2)->GetSymbol();
      fKoxReactionXSec = new TF1( name.Data(),
         this, &KV2Body::KoxReactionXSec, 20, 100, 0, "KV2Body", "KoxReactionXSec");
      fKoxReactionXSec->SetNpx(1000);
   }
   return fKoxReactionXSec;
}

//__________________________________________________________________________________________________

Double_t KV2Body::EqbmChargeState(Double_t *t,Double_t*)
{
   // Calculate the mean charge state of the projectile after passage through the target,
   // assuming that the equilibrium charge state distribution is achieved*.
   // t[0] = energy of projectile after the target (in MeV/nucleon)
   //
   // We use the empirical parameterization of Leon et al., At. Dat. and Nucl. Dat. Tab. 69, 217 (1998)
   // developed for heavy ions in the GANIL energy range (it represents a fit to data measured using
   // GANIL beams).
   //
   // *N.B. Concerning equilibrium charge state distributions, it is not easy to know whether, for a given
   // combination of projectile, projectile energy, target, and target thickness, the equilibrium
   // distribution is reached or not. Here are some comments from the paper cited above which
   // may give some guidelines:
   //
   // "The energies available at the GANIL accelerator range from 24 to 95 MeV/u. Within this energy range,
   //  the equilibrium charge state is reached only for fairly thick targets (~1 mg/cm2 for C foils)."
   //
   // "Mean Charge State as a Function of the Target Thickness
   // A typical example of the variation of the mean charge as a function of the foil thickness is shown ... It is seen
   // that the mean charge initially increases due to the ionization process. Then, the equilibrium state is reached at
   // a certain thickness, the so-called equilibrium thickness, due to the equilibration of electron loss and
   // capture processes. Finally, for foils thicker than the equilibrium thickness, the mean charge decreases due to the
   // slowing down of the ions in matter leading to higher capture cross sections."
   //
   // It should be noted that, according to the data published in this and other papers, the equilibrium thickness
   // decreases with increasing atomic number of the target, and increases with increasing energy of the projectile.
   
   KVNucleus* proj = (KVNucleus*)fNuclei[1];
   Double_t Zp = proj->GetZ();
   proj->SetEnergy(t[0]*proj->GetA());
   Double_t beta = proj->Beta();
   Double_t vp = beta*KVParticle::C();
   Double_t Zt = ((KVNucleus*)fNuclei[2])->GetZ();
   
   Double_t q = Zp * (1. - TMath::Exp( -83.275*beta/pow(Zp, 0.477)) );
   
   // correction for target Z
   Double_t g = 0.929 + 0.269*TMath::Exp(-0.16*Zt) + (0.022 - 0.249*TMath::Exp(-0.322*Zt))*vp/pow(Zp, 0.477);
   q *= g;
   
   if(Zp > 54)
   {
     // f(Zp) - correction for projectiles with Z>54
      Double_t f = 1. - TMath::Exp( -12.905 + 0.2124*Zp - 0.00122*pow(Zp,2) );
      q *= f;
   }
   
   return q;
}

//__________________________________________________________________________________________________

TF1* KV2Body::GetEqbmChargeStateFunc()
{
   // Return pointer to TF1 giving mean charge state of the projectile after passage through the target,
   // assuming that the equilibrium charge state distribution is achieved, as a function of projectile
   // energy after the target (in MeV/nucleon).
   // By default the range of the function is [20,100] MeV/nucleon.
   // Change with TF1::SetRange.
   
   if(!fEqbmChargeState){
      TString name = GetNucleus(1)->GetSymbol();
      name+= " + ";
      name += GetNucleus(2)->GetSymbol();
      fEqbmChargeState = new TF1( name.Data(),
         this, &KV2Body::EqbmChargeState, 20, 100, 0, "KV2Body", "EqbmChargeState");
      fEqbmChargeState->SetNpx(1000);
   }
   return fEqbmChargeState;
}
//__________________________________________________________________________________________________

TF1* KV2Body::GetXSecRuthLabFunc(Int_t OfNucleus, Double_t theta_min, Double_t theta_max)
{
   // Return pointer to TF1 giving Rutherford cross-section (b/sr) in the Lab as a
   // function of projectile (OfNucleus=3) or target (OfNucleus=4) lab scattering angle
   // By default, theta_min = 1 degree & theta_max = 179 degrees
   
      TString name = "RuthXSec: ";
      name+=GetNucleus(1)->GetSymbol();
      name+= " + ";
      name += GetNucleus(2)->GetSymbol();
      name += " ";
      Double_t elab = GetNucleus(1)->GetEnergy()/GetNucleus(1)->GetA();
      name += Form("%6.1f AMeV ", elab);
      if(OfNucleus==3) name+="(projectile)";
      else name+="(target)";
      TF1* fXSecRuthLab = (TF1*)gROOT->GetListOfFunctions()->FindObject(name.Data());
      if(!fXSecRuthLab){
         fXSecRuthLab = new TF1( name.Data(),
            this, &KV2Body::XSecRuthLab, theta_min, theta_max, 1, "KV2Body", "XSecRuthLab");
         fXSecRuthLab->SetParameter(0, OfNucleus);
         fXSecRuthLab->SetNpx(1000);
      }
      fXSecRuthLab->SetRange(theta_min,theta_max);//in case TF1 already exists, but new range is required
   return fXSecRuthLab;
}
//__________________________________________________________________________________________________

TF1* KV2Body::GetXSecRuthLabIntegralFunc(Int_t OfNucleus, Double_t theta_min, Double_t theta_max)
{
   // Return pointer to TF1 giving Rutherford cross-section (b/sr) in the Lab as a
   // function of projectile (OfNucleus=3) or target (OfNucleus=4) lab scattering angle
   //
   // This function is equal to sin(theta)*dsigma/domega, i.e. it is the integrand
   // needed for calculating total cross-sections integrated over solid angle.
   //
   // WARNING: when integrating this function using TF1::Integral, the result must
   // be multiplied by TMath::DegToRad(), because 'x' is in degrees rather than radians,
   // e.g. the integrated cross-section in barns is given by
   //
   //    GetXSecRuthLabIntegralFunc(OfNucleus)->Integral(theta_min, theta_max)*TMath::DegToRad()
   
      TString name = "RuthXSecInt: ";
      name+=GetNucleus(1)->GetSymbol();
      name+= " + ";
      name += GetNucleus(2)->GetSymbol();
      name += " ";
      Double_t elab = GetNucleus(1)->GetEnergy()/GetNucleus(1)->GetA();
      name += Form("%6.1f AMeV ", elab);
      if(OfNucleus==3) name+="(projectile)";
      else name+="(target)";
      
      TF1* fXSecRuthLab = (TF1*)gROOT->GetListOfFunctions()->FindObject(name.Data());
      if(!fXSecRuthLab){
         fXSecRuthLab = new TF1( name.Data(),
            this, &KV2Body::XSecRuthLabInt, theta_min, theta_max, 1, "KV2Body", "XSecRuthLabInt");
         fXSecRuthLab->SetParameter(0, OfNucleus);
         fXSecRuthLab->SetNpx(1000);
      }
      fXSecRuthLab->SetRange(theta_min,theta_max);//in case TF1 already exists, but new range is required
   return fXSecRuthLab;
}

