/*
$Id: KVChIo_e475s.h,v 1.7 2009/04/15 11:39:11 ebonnet Exp $
$Revision: 1.7 $
$Date: 2009/04/15 11:39:11 $
*/

//Created by KVClassFactory on Thu Sep 20 09:46:32 2007
//Author: bonnet

#ifndef __KVCHIO_E475S_H
#define __KVCHIO_E475S_H

#include "KVChIo.h"
#include "KVFunctionCal.h"
#include "KVDBParameterSet.h"
#include "KVString.h"

class KVChIo_e475s : public KVChIo
{
	protected:
	
	KVFunctionCal *fcalibPG;//!channel-energy function conversion (PG)
	KVFunctionCal *fcalibGG;//!channel-energy function conversion (GG)
   
	public:
	
   KVChIo_e475s();
   KVChIo_e475s(Float_t pressure, Float_t thick=50.0);
   virtual ~KVChIo_e475s(){};
	
	virtual void SetCalibrators(){};
	void SetCalibrator(KVDBParameterSet *kvdbps);
	void ChangeCalibParameters(KVDBParameterSet *kvdbps);
	
	Double_t GetOriginalValue(Float_t to,TString signal);
	Double_t GetCalibratedValue(Float_t from,TString signal);

	KVFunctionCal *GetCalibratorBySignal(TString signal) const;
	virtual Bool_t IsCalibrated() const;
	Bool_t	IsCalibratedBySignal(TString signal) const;

	Double_t	GetCalibratedEnergy();
	virtual Double_t	GetEnergy();
   virtual Short_t GetCalcACQParam(KVACQParam*) const;
	inline virtual Bool_t Fired(Option_t * opt = "any");
   
	ClassDef(KVChIo_e475s,1)//derivation of KVChIo class for E475s experiment
};

Bool_t KVChIo_e475s::Fired(Option_t * opt)
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
	
	Option_t * optbis = "Pall";
	
   int touched = 0;
   int working = 0;
   Bool_t ok = kFALSE;
   Char_t opt2[] = "";
   if(optbis[0]=='P'){
      optbis++;
      opt2[0]='P';
   }
	
	KVString stpar;
   if (fACQParams) {
      TIter next(fACQParams);
      KVACQParam *par;
      while ((par = (KVACQParam *) next())) {
         stpar.Form("%s",par->GetName());
			if ( !stpar.EndsWith("_T") ){
				//cout << stpar << endl;
				if (par->IsWorking()) {
         	   working++;
         	   if(par->Fired(opt2)){
         	      ok = kTRUE;
         	      touched++;
         	   }
         	}
			}	
      }

      if (!strcmp(optbis, "all")) {
			//cout << touched << " " << working << endl;
         return (touched == working);
      } else {
         return ok;
      }
   }
   return kFALSE;
}

#endif
