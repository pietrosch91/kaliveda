//Created by KVClassFactory on Wed Feb  2 16:13:08 2011
//Author: frankland,,,,

#ifndef __KVEDALOSSMATERIAL_H
#define __KVEDALOSSMATERIAL_H

#include "KVBase.h"
#include <TString.h>
#include <Riostream.h>

// maximum atomic number included in range tables
#define ZMAX_VEDALOSS 100

class KVedaLossMaterial : public KVBase
{
   TString fState;               // state of material = "solid", "liquid", "gas"
   Double_t fDens;              //density of material
   Double_t fAmat;              //atomic mass of material
   Double_t fZmat;              //atomic number of material
   Double_t fPressure;          //! only used for gases
   Double_t fMoleWt;            //only used for gases
   Double_t fTemp;              //only used for gases
   Double_t fCoeff[ZMAX_VEDALOSS][15];  //[ZMAX_VEDALOSS][15] parameters for range tables
   Double_t fAmasr;              //isotopic mass for material; if not set will use default value

   public:
   KVedaLossMaterial();
   KVedaLossMaterial(const Char_t* name, const Char_t* type, const Char_t* state,
      Double_t density, Double_t Z, Double_t A, Double_t MoleWt = 0.0, Double_t temperature = 19.0);
   virtual ~KVedaLossMaterial();
   
   void ReadRangeTable(FILE* fp);

   ClassDef(KVedaLossMaterial,1)//Description of material properties used by KVedaLoss range calculation
};

#endif
