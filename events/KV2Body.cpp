/***************************************************************************
                          KV2Body.cpp  -  description
                             -------------------
    begin                : 28/11/2003
    copyright            : (C) 2003 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KV2Body.cpp,v 1.3 2009/01/22 13:56:13 ebonnet Exp $
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
//_____________________________________________________________________________________________________________
void KV2Body::init()
{
   //Default initialisations
   TETA3MAX = TETA4MAX = WLT = WCT = WC3 = WC4 = K3 = K4 = 0.;
   for (int i = 0; i < 5; i++) {
      VC[i] = 0.;
      EC[i] = 0.;
   }
   fEDiss = 0.0;
   fDeleteTarget = kFALSE;
   fDeleteProj = kFALSE;
   fDeleteN4 = kFALSE;
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
   if (i < 3) {
      Warning("GetMaxAngleLab(Int_t i)",
              "Returns maximum scattering angle in lab for nuclei i=3 (quasiproj) and i=4 (quasitarget)");
      return 0.;
   }
   switch (i) {
   case 3:
      return TETA3MAX;
      break;
   case 4:
      return TETA4MAX;
      break;
   }
   return 0.;
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

   //total energy (T + m) of entrance channel
   WLT = GetNucleus(1)->E() + (GetNucleus(2) ? GetNucleus(2)->E() : 0.);
   //velocity of CM
   VCM = GetNucleus(1)->GetMomentum() * (KVParticle::C() / WLT);
   //beta of CM
   BCM = VCM.Mag() / KVParticle::C();
   //total energy in CM
   WCT = (GetCMGamma() > 0.0 ? WLT / GetCMGamma() : 0.);
   if (WCT == 0.0)
      return;

   //total energy proj in CM
   Double_t WC1 =
       ((GetNucleus(1)->GetMass() -
         (GetNucleus(2) ? GetNucleus(2)->GetMass() : 0.))
        * (GetNucleus(1)->GetMass() +
           (GetNucleus(2) ? GetNucleus(2)->GetMass() : 0.)) / (2. * WCT)) +
       WCT / 2.;
   //kinetic energy proj in CM
   EC[1] = WC1 - GetNucleus(1)->GetMass();
   //tot E target in CM
   Double_t WC2 = WCT - WC1;
   //kinetic energy targ in CM
   EC[2] = (GetNucleus(2) ? WC2 - GetNucleus(2)->GetMass() : 0.);
   VC[1] = GetVelocity(GetNucleus(1)->GetMass(), WC1);
   VC[2] =
       (GetNucleus(2) ? GetVelocity(GetNucleus(2)->GetMass(), WC2) : 0.);

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
   K3 = VCM.Mag() / VC[3];
   Double_t T3MAX = 0.;
   if (K3 < 1.)
      T3MAX = TMath::Pi();
   if (K3 == 1.)
      T3MAX = TMath::PiOver2();
   if (K3 > 1.)
      T3MAX = TMath::ATan((1. / TMath::Sqrt(K3 * K3 - 1.)) / GetCMGamma());
   TETA3MAX = T3MAX * TMath::RadToDeg();

   if (!GetNucleus(4))
      return;                   //only valid for binary channels
   //total cm energy of nucleus 4 (quasitarg)
   WC4 = WCT - GetEDiss() - WC3;
   VC[4] = GetVelocity(AM4, WC4);
   //cm kinetic energy
   EC[4] = WC4 - AM4;
   K4 = VCM.Mag() / VC[4];

   Double_t T4MAX = 0.;
   if (K4 < 1.)
      T4MAX = TMath::Pi();
   if (K4 == 1.)
      T4MAX = TMath::PiOver2();
   if (K4 > 1.)
      T4MAX = TMath::ATan((1. / TMath::Sqrt(K4 * K4 - 1.)) / GetCMGamma());
   if (TMath::Abs(GetEDiss()) < 1.E-08 && K4 < 1.)
      T4MAX = TMath::PiOver2();
   TETA4MAX = T4MAX * TMath::RadToDeg();
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
   //Print out characteristics of reaction

   cout << " ***** REACTION    " << GetNucleus(1)->
       GetA() << GetNucleus(1)->GetSymbol();
   if (GetNucleus(2))
      cout << "  +  " << GetNucleus(2)->GetA() << GetNucleus(2)->
          GetSymbol();
   if (GetNucleus(3)) {
      cout << " --->   " <<
          GetNucleus(3)->GetA() << GetNucleus(3)->GetSymbol();
   }
   if (GetNucleus(4))
      cout << "  +  " << GetNucleus(4)->GetA() << GetNucleus(4)->
          GetSymbol();
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

Double_t KV2Body::GetELabProj(Double_t ThetaLab_Proj) const
{
   //Calculate (quasi)projectile kinetic energy as a function of scattering angle.

   if (ThetaLab_Proj > TETA3MAX) {
      Warning("GetELabProj",
              "Angle %f is greater than max scattering angle (%f)",
              ThetaLab_Proj, TETA3MAX);
      return 0.;
   }
   Double_t T3CM = GetThetaCMProj(ThetaLab_Proj) * TMath::DegToRad();
   Double_t WL3 =
       WC3 * GetCMGamma() * (1. +
                             BCM * (GetCMVelocity(3) / KVParticle::C()) *
                             TMath::Cos(T3CM));
   return (WL3 - GetNucleus(3)->GetMass());
}

Double_t KV2Body::GetThetaCMProj(Double_t ThetaLab_Proj) const
{
   //Calculate CM projectile scattering angle from lab angle

   Double_t T3L = ThetaLab_Proj * TMath::DegToRad();
   Double_t T3CM = T3L + TMath::ASin(K3 * TMath::Sin(T3L));
   return T3CM * TMath::RadToDeg();
}

//______________________________________________________________________________________________

Double_t KV2Body::GetVLabProj(Double_t ThetaLab_Proj) const
{
   //Calculate (quasi)projectile velocity as a function of scattering angle.
   Double_t etot = GetELabProj(ThetaLab_Proj) + GetNucleus(3)->GetMass();
   return GetVelocity(GetNucleus(3)->GetMass(), etot);
}

Double_t KV2Body::GetThetaCMTarget(Double_t ThetaLab_Proj) const
{
   //Get quasi-target CM angle as a function of projectile scattering angle
   Double_t T3CM = GetThetaCMProj(ThetaLab_Proj) * TMath::DegToRad();
   Double_t T4CM = TMath::Pi() - T3CM;
   return T4CM * TMath::RadToDeg();
}

Double_t KV2Body::GetThetaLabTarget(Double_t ThetaLab_Proj) const
{
   //Get quasi-target lab angle as a function of projectile scattering angle
   Double_t T4CM = GetThetaCMTarget(ThetaLab_Proj) * TMath::DegToRad();
   Double_t T4L = TMath::ATan(TMath::Sin(T4CM) / (K4 + TMath::Cos(T4CM)));
   Double_t thlt = TMath::Abs(T4L * TMath::RadToDeg());
   if (thlt > 180.0)
      thlt = 360.0 - thlt;
   return thlt;
}

Double_t KV2Body::GetELabTarget(Double_t ThetaLab_Proj) const
{
   //Calculate (quasi)target kinetic energy as a function of projectile scattering angle.

   Double_t T4CM = GetThetaCMTarget(ThetaLab_Proj) * TMath::DegToRad();
   Double_t WL4 =
       WC4 * GetCMGamma() * (1. +
                             BCM * (GetCMVelocity(4) / KVParticle::C()) *
                             TMath::Cos(T4CM));
   Double_t elabt = WL4 - GetNucleus(4)->GetMass();
   if (elabt < 0.)
      elabt = 0.0;
   return elabt;
}

Double_t KV2Body::GetXSecRuthCM(Double_t ThetaLab_Proj) const
{
   //Calculate Rutherford cross-section (b/sr) in the CM as a
   //function of projectile lab scattering angle

   if (!GetNucleus(2)) {
      Warning("GetXSecRuthCM", "No target defined for reaction");
      return 0.;
   }
   Double_t PB =
       1.44 * GetNucleus(1)->GetZ() * GetNucleus(2)->GetZ() /
       GetCMEnergy();
   Double_t T3CM = GetThetaCMProj(ThetaLab_Proj) * TMath::DegToRad();
   Double_t D = 1. / (16. * (TMath::Power(TMath::Sin(T3CM / 2.), 4.)));
   return ((TMath::Power(PB, 2.)) * D / 100.);
}

Double_t KV2Body::GetXSecRuthLab(Double_t ThetaLab_Proj) const
{
   //Calculate Rutherford cross-section (b/sr) in the Lab as a
   //function of projectile lab scattering angle

   Double_t DSIDTB = GetXSecRuthCM(ThetaLab_Proj);
   Double_t T3L = ThetaLab_Proj * TMath::DegToRad();
   Double_t T3CM = GetThetaCMProj(ThetaLab_Proj) * TMath::DegToRad();
   Double_t RLC = (TMath::Power(TMath::Sin(T3CM), 3.)) /
       ((TMath::Power(TMath::Sin(T3L), 3.)) * GetCMGamma() *
        (1. + K3 * TMath::Cos(T3CM)));
   return (DSIDTB * RLC);
}

Double_t KV2Body::GetIntegratedXSecRuthLab(Float_t th1,Float_t th2,Float_t phi1,Float_t phi2) const
{
   //Calculate Integrated Rutherford cross-section (b/sr) in the Lab using
	//polar and azimuthal angular range expressed in degree
	//This angular range is considered to be the scattered projectile one
	//
	//If th1 > the maximum scattering angle (GetMaxAngleLab(3==outgoing projectile)) the returned value is 0
	//If th2 > the maximum scattering angle (GetMaxAngleLab(3==outgoing projectile)) 
	//we use the maximum scattering angle as a upper limit
	// 
	//If phi1 ou phi2 ==-1 the azimuthal width is set to 2pi
	//Else if phi1=phi2 the azimuthal width is set to 1 ie the integral is only on theta
	//
	//Integral is computed using the so called trapeze method
	//using 1000 steps for integrate theta (in degree)
	//The returned value is in barn
	
	Float_t tmax_proj = GetMaxAngleLab(3);
	Float_t th1_d = th1;	if (th1_d>tmax_proj) return 0;
	Float_t th2_d = TMath::Min(th2,tmax_proj);
	
	Int_t nn=1000;
	Double_t dphi=0;
	//azimuthal width expressed in rad
	if (phi1==-1 || phi2==-1) dphi=2*TMath::Pi();
	else if (phi1==phi2) dphi=1;
	else { dphi = phi2-phi1; dphi*=TMath::DegToRad(); }
	
	//polar angle expressed in radian
	Float_t th1_r = th1_d*TMath::DegToRad();
	Float_t th2_r = th2_d*TMath::DegToRad();
	
	Double_t norm = (th2_r-th1_r)/nn;
	Double_t integ0 = (GetXSecRuthLab(th1_d)*TMath::Sin(th1_r) + GetXSecRuthLab(th2_d)*TMath::Sin(th2_r))/2.;

	Double_t interm = 0;
	for (Int_t kk=1;kk<nn;kk+=1){
		interm+=GetXSecRuthLab(th1_d+kk*(th2_d-th1_d)/nn)*TMath::Sin((th1_r+kk*(th2_r-th1_r)/nn));
	}
	
   return norm*(integ0+interm)*dphi;
}
