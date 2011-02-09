//Created by KVClassFactory on Wed Feb  2 16:13:08 2011
//Author: frankland,,,,

#ifndef __KVEDALOSSMATERIAL_H
#define __KVEDALOSSMATERIAL_H

#include "KVBase.h"
#include <TString.h>
#include <Riostream.h>
#include <TF1.h>
#include "KVIonRangeTable.h"

// maximum atomic number included in range tables
#define ZMAX_VEDALOSS 100
#define RTT  62.36367e+03  // cm^3.Torr.K^-1.mol^-1
#define ZERO_KELVIN  273.15

class KVedaLossMaterial : public KVBase {

protected:
   TString fState;               // state of material = "solid", "liquid", "gas", "unknown"
   Double_t fDens;              // density of material in g/cm**3
   Double_t fZmat;              // atomic number of material
   Double_t fAmat;              // atomic mass of material
   Double_t fMoleWt;            // only used for gases - mass of one mole of gas in grammes
   Double_t fEmax[ZMAX_VEDALOSS];        //[ZMAX_VEDALOSS] Z-dependent maximum energy/nucleon for calculation to be valid
   Double_t fEmin[ZMAX_VEDALOSS];        //[ZMAX_VEDALOSS] Z-dependent minimum energy/nucleon for calculation to be valid
   Double_t fCoeff[ZMAX_VEDALOSS][14];  //[ZMAX_VEDALOSS][14] parameters for range tables

   TF1* fDeltaE; // function parameterising energy loss in material
   TF1* fEres; // function parameterising residual energy after crossing material
   TF1* fRange; // function parameterising range of charged particles in material

   Double_t DeltaEFunc(Double_t*, Double_t*);
   Double_t EResFunc(Double_t*, Double_t*);
   Double_t RangeFunc(Double_t*, Double_t*);

public:
   KVedaLossMaterial();
   KVedaLossMaterial(const Char_t* name, const Char_t* type, const Char_t* state,
                     Double_t density, Double_t Z, Double_t A, Double_t MoleWt = 0.0);
   virtual ~KVedaLossMaterial();

   Bool_t ReadRangeTable(FILE* fp);
   Float_t GetEmaxValid(Int_t Z, Int_t A) const {
      return ((Z <= ZMAX_VEDALOSS && Z > 0) ? A*fEmax[Z - 1] : 0.0);
   };
   Float_t GetEminValid(Int_t Z, Int_t A) const {
      return ((Z <= ZMAX_VEDALOSS && Z > 0) ? A*fEmin[Z - 1] : 0.0);
   };
   Double_t GetMass() const {
      return fAmat;
   };
   Double_t GetZ() const {
      return fZmat;
   };
   Double_t GetDensity() const {
      // return density of material in g/cm**3.
      // for gaseous materials, you must call SetGasDensity(T,P) first in order
      // to define the temperature and pressure of the gas.
      return fDens;
   };
   void SetTemperatureAndPressure(Double_t T, Double_t P) {
      // for a gaseous material, calculate density in g/cm**3 according to given
      // conditions of temperature (T, in degrees celsius) and pressure (P, in Torr)
      if (IsGas() && T > 0 && P > 0) fDens = (fMoleWt * P) / ((T + ZERO_KELVIN) * RTT);
   };

   Bool_t IsGas() const {
      // returns kTRUE if material is in gaseous state
      return (fState == "gas");
   };

   void ls(Option_t* = "") const;
   void Print(Option_t* = "") const {
      ls();
   };

   TF1* GetRangeFunction(Int_t Z, Int_t A, Double_t isoAmat = 0);
   TF1* GetDeltaEFunction(Double_t e, Int_t Z, Int_t A, Double_t isoAmat = 0);
   TF1* GetEResFunction(Double_t e, Int_t Z, Int_t A, Double_t isoAmat = 0);

   void PrintRangeTable(Int_t Z, Int_t A, Double_t isoAmat = 0, Double_t units = Units::cm, Double_t T = -1, Double_t P = -1);

   virtual Double_t GetRangeOfIon(Int_t Z, Int_t A, Double_t E, Double_t isoAmat = 0.);
   virtual Double_t GetLinearRangeOfIon(Int_t Z, Int_t A, Double_t E, Double_t isoAmat = 0, Double_t T = -1., Double_t P = -1.);

   virtual Double_t GetDeltaEOfIon(Int_t Z, Int_t A, Double_t E, Double_t e, Double_t isoAmat = 0.);
   virtual Double_t GetLinearDeltaEOfIon(Int_t Z, Int_t A, Double_t E, Double_t e, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.);

   virtual Double_t GetEResOfIon(Int_t Z, Int_t A, Double_t E, Double_t e, Double_t isoAmat = 0.);
   virtual Double_t GetLinearEResOfIon(Int_t Z, Int_t A, Double_t E, Double_t e, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.);

   virtual Double_t GetEIncFromEResOfIon(Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat = 0.);
   virtual Double_t GetLinearEIncFromEResOfIon(Int_t Z, Int_t A, Double_t Eres, Double_t e, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.);

   virtual Double_t GetEIncFromDeltaEOfIon(Int_t Z, Int_t A, Double_t DeltaE, Double_t e, Double_t isoAmat = 0.);
   virtual Double_t GetLinearEIncFromDeltaEOfIon(Int_t Z, Int_t A, Double_t DeltaE, Double_t e, Double_t isoAmat = 0., Double_t T = -1., Double_t P = -1.);

   virtual Double_t GetDeltaEFromEResOfIon(Int_t Z, Int_t A, Double_t ERes, Double_t e, Double_t isoAmat = 0.) {
      // Calculates energy loss (in MeV) of an ion (Z,A) from residual energy ERes (MeV) after thickness e (in mg/cm**2).
      // Give Amat to change default (isotopic) mass of material,
      return GetEIncFromEResOfIon(Z, A, ERes, e, isoAmat) - ERes;
   }

   virtual Double_t GetLinearDeltaEFromEResOfIon(Int_t Z, Int_t A, Double_t ERes, Double_t e,
         Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.) {
      // Calculates energy loss (in MeV) of an ion (Z,A) from residual energy ERes (MeV) after thickness e (in cm).
      // Give Amat to change default (isotopic) mass of material,
      // give temperature (degrees C) & pressure (torr) (T,P) for gaseous materials.
      return GetLinearEIncFromEResOfIon(Z, A, ERes, e, isoAmat, T, P) - ERes;
   }

   virtual Double_t GetPunchThroughEnergy(Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0.) ;
   virtual Double_t GetLinearPunchThroughEnergy(Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.) ;

   virtual Double_t GetMaxDeltaEOfIon(Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0.);
   virtual Double_t GetEIncOfMaxDeltaEOfIon(Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0.);
   virtual Double_t GetLinearMaxDeltaEOfIon(Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.);
   virtual Double_t GetLinearEIncOfMaxDeltaEOfIon(Int_t Z, Int_t A, Double_t e, Double_t isoAmat=0., Double_t T=-1., Double_t P=-1.);
      
   ClassDef(KVedaLossMaterial, 1) //Description of material properties used by KVedaLoss range calculation
};

#endif
