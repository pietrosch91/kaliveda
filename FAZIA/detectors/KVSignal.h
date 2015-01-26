//Created by KVClassFactory on Mon Dec 22 15:46:46 2014
//Author: ,,,

#ifndef __KVSIGNAL_H
#define __KVSIGNAL_H

#include "TGraph.h"

class KVPSAResult;

class KVSignal : public TGraph
{
	protected:
   Int_t fBlock;
   Int_t fQuartet;
   TString fQuartetName;
   Int_t fTelescope;
   TString fTelName;
   TString fDetName;
   TString fType;
   TString fDet;
	Double_t fYmin,fYmax;
  
   public:
   KVSignal();
   KVSignal(const char* name, const char* title);
   KVSignal(const TString& name, const TString& title);
   virtual ~KVSignal();
   void Copy(TObject& obj) const;
	
   void SetData(Int_t nn, Double_t* xx, Double_t* yy);
   virtual KVPSAResult* TreateSignal(void);
   void DeduceFromName();
   
   void Print(Option_t* chopt = "") const;
   Int_t GetBlockNumber() const { return fBlock; }
   Int_t GetQuartetNumber() const { return fQuartet; }
   const Char_t* GetQuartetName() const { return fQuartetName.Data(); }
   Int_t GetTelescopeNumber() const { return fTelescope; }
   const Char_t* GetTelescopeName() const { return fTelName.Data(); }
   const Char_t* GetDetectorName() const { return fDetName.Data(); }
   const Char_t* GetType() const { return fType.Data(); }
   const Char_t* GetDetector() const { return fDet.Data(); }
   
   void ComputeGlobals(void);
   Double_t GetYmin() const { return fYmin; }
   Double_t GetYmax() const { return fYmax; }
   Double_t GetAmplitude() const { return fYmax-fYmin; }
   
   
   ClassDef(KVSignal,2)//simple class to store TArray in a list
};

#endif
