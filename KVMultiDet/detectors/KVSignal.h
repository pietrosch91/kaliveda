//Created by KVClassFactory on Mon Dec 22 15:46:46 2014
//Author: ,,,

#ifndef __KVSIGNAL_H
#define __KVSIGNAL_H

#include "TGraph.h"

class KVSignal : public TGraph
{

   
   public:
   KVSignal();
   KVSignal(const char* name, const char* title);
   KVSignal(const TString& name, const TString& title);
   virtual ~KVSignal();
   void Copy(TObject& obj) const;
	
   void SetData(Int_t nn, Double_t* xx, Double_t* yy);
   
   ClassDef(KVSignal,1)//simple class to store TArray in a list
};

#endif
