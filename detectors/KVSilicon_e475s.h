/*
$Id: KVSilicon_e475s.h,v 1.7 2009/04/15 09:49:19 ebonnet Exp $
$Revision: 1.7 $
$Date: 2009/04/15 09:49:19 $
*/

//Created by KVClassFactory on Wed Sep 19 13:46:35 2007
//Author: bonnet

#ifndef __KVSILICON_E475S_H
#define __KVSILICON_E475S_H

#include "KVSilicon.h"
#include "KVFunctionCal.h"
#include "KVDBParameterSet.h"

class KVSilicon_e475s : public KVSilicon
{
   
	protected:
	
	KVFunctionCal *fcalibPG;//!channel-energy function conversion (PG)
	KVFunctionCal *fcalibGG;//!channel-energy function conversion (GG)
   
	void init();
	
	public:
	
	KVSilicon_e475s();
	KVSilicon_e475s(Float_t thick);
   virtual ~KVSilicon_e475s(){};
	
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
   
	Double_t GetCorrectedEnergy(UInt_t z, UInt_t a, Double_t e = -1., Bool_t transmission=kTRUE);

	Short_t GetCalcACQParam(KVACQParam*) const;
	inline virtual Bool_t Fired(Option_t * opt = "any");
   
   
	ClassDef(KVSilicon_e475s,1)//derivation of KVSilicon class for E475s experiment

};

Bool_t KVSilicon_e475s::Fired(Option_t * opt)
{
   //Les Marqueurs de temps ne rentrent pas en compte dans l'analyse
	//
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

   int touched = 0;
   int working = 0;
   Bool_t ok = kFALSE;
   Char_t opt2[] = "";
   if(opt[0]=='P'){
      opt++;
      opt2[0]='P';
   }

   if (fACQParams) {
      TIter next(fACQParams);
      KVACQParam *par;
      while ((par = (KVACQParam *) next())) {
         if ( strcmp(par->GetName(),"T") ){
				if (par->IsWorking()) {
         	   working++;
         	   if(par->Fired(opt2)){
         	      ok = kTRUE;
         	      touched++;
         	   }
         	}
			}	
      }

      if (!strcmp(opt, "all")) {
         return (touched == working);
      } else {
         return ok;
      }
   }
   return kFALSE;
}

#endif
