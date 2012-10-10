//Created by KVClassFactory on Thu Sep 27 17:23:23 2012
//Author: John Frankland,,,

#ifndef __KVElementDensity_H
#define __KVElementDensity_H

#include "KVNuclData.h"

class KVElementDensity : public KVNuclData
{
   Bool_t fIsGas;
   TString fSymbol;
   TString fElementName;
   
   public:
   KVElementDensity();
   KVElementDensity (const KVElementDensity&) ;
   virtual ~KVElementDensity();
   void Copy (TObject&) const;
   
   void SetIsGas(Bool_t g=kTRUE){fIsGas=g;};
   Bool_t IsGas() const { return fIsGas; };
   void SetElementSymbol(const Char_t* s){fSymbol=s;};
   const Char_t* GetElementSymbol() const { return fSymbol;};
   void SetElementName(const Char_t* en){fElementName=en;};
   const Char_t* GetElementName() const { return fElementName;};
   
   ClassDef(KVElementDensity,1)//Atomic element with name, symbol and density
};

#endif
