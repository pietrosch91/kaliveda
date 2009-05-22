/*
$Id: KVCsI_e475s.h,v 1.5 2009/05/22 14:44:18 ebonnet Exp $
$Revision: 1.5 $
$Date: 2009/05/22 14:44:18 $
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


#endif
