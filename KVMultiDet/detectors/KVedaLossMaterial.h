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
   //TF1* fEres; // function parameterising residual energy after crossing material
   TF1* fRange; // function parameterising range of charged particles in material

   Double_t DeltaEFunc(Double_t*, Double_t*);
   //Double_t EResFunc(Double_t*, Double_t*);
   Double_t RangeFunc(Double_t*, Double_t*);

public:
   KVedaLossMaterial();
   KVedaLossMaterial(const Char_t* name, const Char_t* type, const Char_t* state,
                     Double_t density, Double_t Z, Double_t A, Double_t MoleWt = 0.0);
   virtual ~KVedaLossMaterial();

   Bool_t ReadRangeTable(FILE* fp);
   Float_t GetEmaxVedaloss(UInt_t Z) const {
      return ((Z <= ZMAX_VEDALOSS && Z > 0) ? fEmax[Z - 1] : 0.0);
   };
   Float_t GetEminVedaloss(UInt_t Z) const {
      return ((Z <= ZMAX_VEDALOSS && Z > 0) ? fEmin[Z - 1] : 0.0);
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
   void SetGasDensity(Double_t T, Double_t P);

   Bool_t IsGas() const {
      // returns kTRUE if material is in gaseous state
      return (fState == "gas");
   };

   void ls(Option_t* = "") const;

   TF1* GetRangeFunction(Int_t Z, Int_t A, Double_t isoAmat = 0);
   TF1* GetDeltaEFunction(Double_t e, Int_t Z, Int_t A, Double_t isoAmat = 0);

   void PrintRangeTable(Int_t Z, Int_t A, Double_t isoAmat = 0, Double_t units = Units::cm, Double_t T = -1, Double_t P = -1);

   ClassDef(KVedaLossMaterial, 1) //Description of material properties used by KVedaLoss range calculation
};

#endif
