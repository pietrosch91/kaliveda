/*
$Id: KVCsI_e475s.h,v 1.4 2009/05/19 15:39:53 ebonnet Exp $
$Revision: 1.4 $
$Date: 2009/05/19 15:39:53 $
*/

//Created by KVClassFactory on Thu Apr  9 09:22:46 2009
//Author: eric bonnet,,,

#ifndef __KVCSI_E475S_H
#define __KVCSI_E475S_H

#include "KVCsI.h"
#include "KVFunctionCal.h"
#include "KVDBParameterSet.h"
#include "KVString.h"
#include "KVDataSet.h"

class KVCsI_e475s : public KVCsI
{

   protected:
	KVFunctionCal *fcalibLT;//!channel-energy function conversion (LT)
	void init();
	
	public:
   KVCsI_e475s();
   KVCsI_e475s(Float_t thick);
   virtual ~KVCsI_e475s(){ };
	
	void SetCalibrators(){};
	void SetCalibrator(KVDBParameterSet *kvdbps);
	void ChangeCalibParameters(KVDBParameterSet *kvdbps);

	Double_t GetOriginalValue(Float_t to,TString signal);
	Double_t GetCalibratedValue(Float_t from,TString signal);

	KVFunctionCal *GetCalibratorBySignal(TString signal) const;
	Bool_t 	IsCalibrated() const;
	Bool_t	IsCalibratedBySignal(TString signal) const;

	Double_t	GetCalibratedEnergy();
	Double_t	GetEnergy();
 
 	Double_t GetLightFromEnergy(UInt_t Z, UInt_t A, Double_t E = -1.){return -1;};
	Double_t GetCorrectedEnergy(UInt_t z, UInt_t a, Double_t e = -1., Bool_t transmission=kTRUE);

   Short_t GetCalcACQParam(KVACQParam*) const;
	inline virtual Bool_t Fired(Option_t * opt = "any");
   
	ClassDef(KVCsI_e475s,1)//derivation of KVCsI class for E475s experiment
};

Bool_t KVCsI_e475s::Fired(Option_t * opt)
{
   
	//opt="any" (default):
   //Returns true if ANY of the working acquisition parameters associated with the detector were fired in an event
   //opt="all" :
   //Returns true if ALL of the working acquisition parameters associated with the detector were fired in an event
   //opt="Pany" :
   //Returns true if ANY of the working acquisition parameters associated with the detector were fired in an event
   //and have a value greater than their pedestal value
   //opt="Pall" :
   //Returns true if ALL of the working acquisition parameters associated with the detector were fired in an event
   //and have a value greater than their pedestal value
   //
   //See KVACQParam::Fired()
	
	KVString inst; inst.Form("Reconstruction.DataAnalysisTask.ACQParameterList.%s",GetType());
	KVString lpar = gDataSet->GetDataSetEnv(inst);
   
	Int_t touched = 0;
   Int_t working = 0;
   Bool_t ok = kFALSE;
   Char_t opt2[] = "";
   if(opt[0]=='P'){
      opt++;
      opt2[0]='P';
   }
	
	lpar.Begin(",");
	KVACQParam *par=0;
	while (!lpar.End()){
		
		par = GetACQParam(lpar.Next());
		if (par && par->IsWorking()){
			working++;
		   if(par->Fired(opt2)){
		      ok = kTRUE;
		      touched++;
			}
		}
		
	}
	
	if (!strcmp(opt, "all")) 	return (touched == working);
	else 								return ok;
	
	return kFALSE;
}

#endif
