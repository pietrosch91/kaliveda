//$Id: KVTGIDFunctions.cpp,v 1.13 2008/02/08 13:13:34 franklan Exp $

#include "KVTGIDFunctions.h"
#include "KVNucleus.h"
#include "TMath.h"
#include "Riostream.h"

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

#ifndef ROOT_RVersion
#include "RVersion.h"
#endif

#if ROOT_VERSION_CODE > ROOT_VERSION(4, 1, 2)
// Without this macro the THtml doc can not be generated
#if !defined(R__ALPHA) && !defined(R__SOLARIS) && !defined(R__ACC) && !defined(R__MACOSX)
NamespaceImp(KVTGIDFunctions)
#endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  KVTGIDFunctions namespace
//                                                              
//  Contains Tassan-Got functionals for mass and charge
//  identification used for Si-CsI, Si150-CsI and ChIo-CsI, and ChIo-Si matrices.
//Begin_Html 
//<p>
//For xxx-CsI telescopes:<br>
//tassangot_A and tassangot_Z are the original 10 parameter functionals<br>
//(corresponds to Eq.10 with Eq.12 (E vs. h) from <a href="http://arxiv.org/abs/nucl-ex/0103004">Nucl.Instrum.Meth. B194 (2002) 503-512</a>).<br>
//pawlowski_A and pawlowski_Z are modified functionals with 11 parameters.
//</p>
//
//In all cases here is the correspondance between par[] and the formulae<br>
//given in <a href="http://arxiv.org/abs/nucl-ex/0103004">Nucl.Instrum.Meth. B194 (2002) 503-512</a> (with initial values and limits used when fitting<br>
//parameters from same publication) :<br>
//</p>
//End_Html  
//
//       par[0] : lambda 
//       par[1] : alpha (1: 0.5 - 1.5)
//       par[2] : beta (0.5: 0.2 - 1)
//       par[3] : mu (1: 0.2 - 1.5)
//       par[4] : nu (1: 0.1 - 4)
//       par[5] : zeta (?? squiggly epsilon thingy ??) (>0)
//       par[6] : g
//       par[7] : pedestal of x-coordinate
//       par[8] : pedestal of y-coordinate
//Begin_Html 
//<br>       
//  In case of xxx-CsI telescopes, the relationship between E and h introduces
//  an additional parameter:
//End_Html  
// 
//       par[9] : eta (Eq.11)
//       
//Begin_Html 
//  lambda and g are determined from starting points (E=0) and initial slopes
//  of lines, respectively, and their ranges are given as between 0.25 and 4 times
//  these values.
//  If no A identification is given, beta is fixed at beta=0.5
//<br> 
//  With the Pawlowski modification, we have also:
//End_Html  
// 
//       par[10] : Pawlowski correction
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Double_t KVTGIDFunctions::chiosi_Z(Double_t * x, Double_t * par)
{
   //Tassan-Got formula used for Z identification in ChIo-Si telescopes.
   //Charity EAL mass used for Z>5
   //
   //Returns difference between measured ChIo energy (dE)
   //and that calculated for given (Z, Si energy (E)).
   //
   //Arguments are
   //x[0] Z of nucleus considered
   //par[0], ..., par[8] 9 parameters of Tassan-Got formula
   //par[9] Si energy (in MeV) X-coordinate = dE
   //par[10] ChIo energy (in MeV) Y-coordinate = E

   Double_t A = KVNucleus::GetRealAFromZ(x[0], KVNucleus::kEALMass);    //Deduce A from Z

   Double_t gamma = par[3] + par[4] + 1.0;
   Double_t E = par[9] - par[7];
   Double_t de = par[10] - par[8];

   return (TMath::Power((TMath::Power((par[6] * E), gamma)

                         +
                         TMath::
                         Power((par[0] * TMath::Power(x[0], par[1]) *
                                TMath::Power(A, par[2])), gamma)

                         + par[5] * TMath::Power(x[0], 2) * TMath::Power(A,
                                                                         par
                                                                         [3])
                         * TMath::Power((par[6] * E), par[4]))

                        , (1. / gamma))

           - par[6] * E - de);
}

//_________________________________________________________________________________________

Double_t KVTGIDFunctions::tassangot_Z(Double_t * x, Double_t * par)
{
   //Tassan-Got formula (10 parameters) used for Z identification in Si-CsI or ChIo-CsI
   //telescopes - 12 parameters
   //Charity EAL mass used for Z>5
   //Returns difference between measured ChIo/Si PG/GG channel
   //and that calculated for given (Z, CsI light output).
   //
   //Arguments are
   //x[0] Z of nucleus considered
   //par[0], ..., par[9] 10 parameters of Tassan-Got formula
   //par[10] measured total light output of CsI - X coord
   //par[11] measured ChIo/Si PG or GG channel - Y coord

   Double_t A = KVNucleus::GetRealAFromZ(x[0], KVNucleus::kEALMass);    //Deduce A from Z

   Double_t gamma = par[3] + par[4] + 1.0;
   Double_t h = par[10] - par[7];
   Double_t de = par[11] - par[8];

   Double_t rho = par[9] * TMath::Power(x[0], 2) * A;
   Double_t E;
   if(h > 0){
      if(rho > 0){
         E = TMath::Sqrt(TMath::Power(h, 2) + 2 * rho * h * (1 + TMath::Log(1 + h / rho)));
      }
      else
         E = h;
   }
   else
      E = 0;

   return (TMath::Power((TMath::Power((par[6] * E), gamma)
                         +
                         TMath::
                         Power((par[0] * TMath::Power(x[0], par[1]) *
                                TMath::Power(A, par[2])), gamma)
                         + par[5] * TMath::Power(x[0], 2) * TMath::Power(A,
                                                                         par
                                                                         [3])
                         * TMath::Power((par[6] * E), par[4])),
                        (1. / gamma)) - par[6] * E - de);
}

//_________________________________________________________________________________________

Double_t KVTGIDFunctions::pichon_Z(Double_t * x, Double_t * par)
{
   //Tassan-Got formula (10 parameters) used for Z identification in Si-CsI or ChIo-CsI
   //5th campaign telescopes - 12 parameters
   //As per Matthieu Pichon's these, we use A = 2Z for the mass.
   //Returns difference between measured ChIo/Si PG/GG channel
   //and that calculated for given (Z, CsI light output).
   //
   //Corresponds to Eq.11 of NIM B194 (2002) 503 with Eq.13 for the
   //light-energy dependence
   //
   //Arguments are
   //x[0] Z of nucleus considered
   //       par[0] : lambda 
   //       par[1] : alpha (1: 0.5 - 1.5)
   //       par[2] : beta (0.5: 0.2 - 1)
   //       par[3] : mu (1: 0.2 - 1.5)
   //       par[4] : nu (1: 0.1 - 4)
   //       par[5] : zeta (?? squiggly epsilon thingy ??) (>0)
   //       par[6] : g
   //       par[7] : pedestal of x-coordinate
   //       par[8] : pedestal of y-coordinate
   //par[10] measured total light output of CsI - X coord
   //par[11] measured ChIo/Si PG or GG channel - Y coord

   Double_t Z = x[0];   
   Double_t A = 2. * Z;      //Deduce A from Z, A=2Z

   Double_t gamma = par[3] + par[4] + 1.0;//gamma=mu+nu+1
   Double_t h = par[10] - par[7];
   Double_t de = par[11] - par[8];

   Double_t rho = par[9] * TMath::Power(Z, 2) * A;//=eta*Z^2*A
   Double_t E;
   if(h > 0){
      if(rho > 0){
         E = TMath::Sqrt(TMath::Power(h, 2) + 2 * rho * h * (1 + TMath::Log(1 + h / rho)));
      }
      else
         E = h;
   }
   else
      E = 0;

   Double_t gE = par[6] * E;//=g*E
   Double_t gE_to_the_gamma = TMath::Power(gE, gamma);
   Double_t gE_to_the_nu = TMath::Power(gE, par[4]);
   Double_t lambda_Z_alpha_A_beta = 
         par[0] * TMath::Power(Z, par[1]) * TMath::Power(A, par[2]);
   Double_t zeta_Z2_A_mu = par[5] * TMath::Power(Z, 2) * TMath::Power(A,par[3]);
   Double_t one_over_gamma = 1./gamma;
   Double_t Delta_E = gE_to_the_gamma + TMath::Power(lambda_Z_alpha_A_beta, gamma)
         + zeta_Z2_A_mu*gE_to_the_nu;
   Delta_E = TMath::Power( Delta_E , one_over_gamma ) - gE;
   return (Delta_E - de);
}

//_________________________________________________________________________________________

Double_t KVTGIDFunctions::tassangot_A(Double_t * x, Double_t * par)
{
   //Tassan-Got formula (13 parameters) for A identification (known Z)
   //Returns difference between measured ChIo/Si PG/GG channel and that calculated for given
   //(Z, A, CsI light output)
   //
   //Arguments are
   //x[0] A of nucleus considered
   //par[0], ..., par[9] 10 parameters of corrected Tassan-Got formula
   //par[10] Z of nucleus considered
   //par[11] measured total light output of CsI - X coord
   //par[12] measured ChIo/Si PG or GG channel - Y coord

   Double_t gamma = par[3] + par[4] + 1.0;
   Double_t h = par[11] - par[7];
   Double_t de = par[12] - par[8];

   Double_t rho = par[9] * TMath::Power(par[10], 2) * x[0];
   Double_t E;
   if(h > 0){
      if(rho > 0){
         E = TMath::Sqrt(TMath::Power(h, 2) + 2 * rho * h * (1 + TMath::Log(1 + h / rho)));
      }
      else
         E = h;
   }
   else
      E = 0;

   return (TMath::Power((TMath::Power((par[6] * E), gamma)
                         +
                         TMath::
                         Power((par[0] * TMath::Power(par[10], par[1]) *
                                TMath::Power(x[0], par[2])), gamma)
                         + par[5] * TMath::Power(par[10],
                                                 2) * TMath::Power(x[0],
                                                                   par[3])
                         * TMath::Power((par[6] * E), par[4])),
                        (1. / gamma)) - par[6] * E - de);
}

//___________________________________________________________________________________
Double_t KVTGIDFunctions::pawlowski_Z(Double_t * x, Double_t * par)
{
   //Tassan-Got formula with Pawlowski correction (11th parameter) for Z identification
   //A = 2*Z + 1
   //13 parameters in all.
   //Returns difference between measured Si PG/GG channel
   //and that calculated for given (Z, CsI light output).
   //
   //Arguments are
   //x[0] Z of nucleus considered
   //par[0], ..., par[10] 11 parameters of corrected Tassan-Got formula
   //par[11] measured total light output of CsI - X coord
   //par[12] measured ChIo/Si PG or GG channel - Y coord

   Double_t A = 2.*x[0] +1;    //Deduce A from Z

   Double_t gamma = par[3] + par[4] + 1.0;//gamma=mu+nu+1
   Double_t h = par[11] - par[7];
   h = 0.5 * (h + TMath::Sqrt(TMath::Power(h, 2) + par[10]));   //Correction P. Pawlowski
   Double_t de = par[12] - par[8];

   Double_t rho = par[9] * TMath::Power(x[0], 2) * A;//=eta*Z^2*A
   Double_t E;
   if(h > 0){
      if(rho > 0){
         E = TMath::Sqrt(TMath::Power(h, 2) + 2 * rho * h * (1 + TMath::Log(1 + h / rho)));
      }
      else
         E = h;
   }
   else
      E = 0;

   Double_t gE = par[6] * E;//=g*E
   Double_t gE_to_the_gamma = TMath::Power(gE, gamma);
   Double_t gE_to_the_nu = TMath::Power(gE, par[4]);
   Double_t lambda_Z_alpha_A_beta = 
         par[0] * TMath::Power(x[0], par[1]) * TMath::Power(A, par[2]);
   Double_t zeta_Z2_A_mu = par[5] * TMath::Power(x[0], 2) * TMath::Power(A,par[3]);
   Double_t one_over_gamma = 1./gamma;
   Double_t Delta_E = gE_to_the_gamma + TMath::Power(lambda_Z_alpha_A_beta, gamma)
         + zeta_Z2_A_mu*gE_to_the_nu;
   Delta_E = TMath::Power( Delta_E , one_over_gamma ) - gE;
   return (Delta_E - de);
}

//_________________________________________________________________________________________

Double_t KVTGIDFunctions::pawlowski_A(Double_t * x, Double_t * par)
{
   //Tassan-Got formula with Pawlowski correction (11th parameter) for A identification (Z known).
   //Returns difference between measured ChIo/Si PG/GG channel and that calculated for given
   //(Z, A, CsI light output)
   //14 parameters in all
   //Arguments are
   //x[0] A of nucleus considered
   //par[0], ..., par[10] 11 parameters of corrected Tassan-Got formula
   //par[11] Z of nucleus considered
   //par[12] measured total light output of CsI - X coord
   //par[13] measured ChIo/Si PG or GG channel - Y coord

   Double_t gamma = par[3] + par[4] + 1.0;
   Double_t h = par[12] - par[7];

   h = 0.5 * (h + TMath::Sqrt(TMath::Power(h, 2) + par[10]));   //Correction P. Pawlowski
   Double_t rho = par[9] * TMath::Power(par[11], 2) * x[0];//=eta*Z^2*A
   Double_t E;
   if(h > 0){
      if(rho > 0){
         E = TMath::Sqrt(TMath::Power(h, 2) + 2 * rho * h * (1 + TMath::Log(1 + h / rho)));
      }
      else
         E = h;
   }
   else
      E = 0;
   Double_t de = par[13] - par[8];


   return (TMath::Power((TMath::Power((par[6] * E), gamma)
                         +
                         TMath::
                         Power((par[0] * TMath::Power(par[11], par[1]) *
                                TMath::Power(x[0], par[2])), gamma)
                         + par[5] * TMath::Power(par[11],
                                                 2) * TMath::Power(x[0],
                                                                   par[3])
                         * TMath::Power((par[6] * E), par[4])),
                        (1. / gamma)) - par[6] * E - de);
}

//___________________________________________________________________________________

Double_t KVTGIDFunctions::starting_points_Z(Double_t * x, Double_t * par)
{
   //Function used to fit starting points' y-coordinate as a function of Z
   //and get initial values for lambda, alpha
   //Function is : DE(E=0) = lambda * (Z**alpha) * (A**0.5)
   //
   //par0 = lambda
   //par1 = alpha

   Double_t A = KVNucleus::GetRealAFromZ(x[0], KVNucleus::kEALMass);    //Deduce A from Z
   return par[0] * TMath::Power(x[0], par[1]) * TMath::Power(A, 0.5);
}

//___________________________________________________________________________________

Double_t KVTGIDFunctions::starting_points_A(Double_t * x, Double_t * par)
{
   //Function used to fit starting points' y-coordinate as a function of A
   //and get initial values for lambda, alpha and beta
   //Function is : DE(E=0) = lambda * (Z**alpha) * (A**beta)
   //
   //par0 = lambda
   //par1 = alpha
   //par2 = beta
   //par3 = Z

   Double_t Z = par[3];
   return par[0] * TMath::Power(Z, par[1]) * TMath::Power(x[0], par[2]);
}
