//Created by KVClassFactory on Tue Oct 13 10:57:16 2015
//Author: Quentin Fable

#ifndef __SIMPLEGEMINIMODEL_BINNED_H
#define __SIMPLEGEMINIMODEL_BINNED_H

#include "GenericModel_Binned.h"
#include "TH1.h"
#include "TH2.h"
#include "TVector3.h"


namespace BackTrack {

   class SimpleGeminiModel_Binned : public GenericModel_Binned {
      /* Simple model to generate 2 observables with the GEMINI++ desexcitation code.
       * The parameters of the GEMINI++ model are the mass A, the charge Z, the angular momentum J,
         the excitation energy E* and the velocity of the source.
       * In this example only the mass and the excitation energy of the source can be modified to
         generate events (Z, J and the velocity can be set by the user).
       * We generate 2 observables out of the model: the reconstructed mass of the source without the neutrons (from the fragments)
         and the excitation energy of the source without the neutron contribution.
      */

   private:

      Int_t     fNGen;                   // Number of events to generate for each dataset
      Int_t     fZpr;                    // Charge of the source = Fixed parameter for GEMINI++ defined by the user
      Double_t  fJpr_x;                  // Angular momentum of the source (in hbar unit) = Fixed parameter for GEMINI++ defined by the user
      Double_t  fJpr_y;
      Double_t  fJpr_z;
      TVector3  fVpr;                    // Velocity of the source = Fixed parameter for GEMINI++ defined by the user
      TH2D*     fdistri;                 // TH2D used to set the initial guesses for the fit


      void InitPar();
      void InitObs();


   public:

      SimpleGeminiModel_Binned();
      virtual ~SimpleGeminiModel_Binned();

      void InitParObs();                                                                // Initialize parameters and observables (need to InitWorkspace before)
      void SetSourceCharge(Int_t zpr);                                                  // Initialize charge of the source (fixed parameter for the example)
      void SetSourceSpin(Double_t lx, Double_t ly, Double_t lz);                        // Initialize spin of the source (fixed parameter for the example)
      void SetSourceVelocity(Double_t vx, Double_t vy, Double_t vz);                    // Initialize velocity of the source (fixed parameter for the example)
      void SetParamDistribution(TH2D* distri);                                          // Initialize the guess on (Apr,E*pr/Apr) parameters according to a 2-D distribution
      RooDataHist* GetModelDataHist(RooArgList& par);                                   // Definition for the virtual method for GenericModel_Binned class
      Double_t GetParamInitiWeight(RooArgList& par);                                    // To get the initial weights
      void generateEvent(const RooArgList& parameters, RooDataSet& data);               // Generate one event for the model
      Double_t generateWeight(const RooArgList& parameters, TH2D* distri);              // Generate guess/weight for the
      void SetNumGen(Int_t n)
      {
         fNGen = n;   // Number of events to generate
      }
      Int_t GetNumGen() const
      {
         return fNGen;
      }
      TH1* GetParameterDistributions();                                                 // For drawings after the fit



      ClassDef(SimpleGeminiModel_Binned, 1) //An example of fit with BackTrack using the GEMINI++ desexcitation code
   };

}
#endif
