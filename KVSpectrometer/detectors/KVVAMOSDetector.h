//Created by KVClassFactory on Wed Jul 25 09:43:37 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSDETECTOR_H
#define __KVVAMOSDETECTOR_H

#include "KVSpectroDetector.h"
#include "KVVAMOS.h"
#include "KVFunctionCal.h"

class KVVAMOSDetector : public KVSpectroDetector
{
	protected:
		
		TList         *fT0list; //! list of T0 saved in a KVNamedParameter
		TList         *fTlist;  //! list of Time ACQ parameters

		void init();
   		virtual Bool_t Fired(Option_t *opt, Option_t *optP);


   public:

   KVVAMOSDetector();
   KVVAMOSDetector(UInt_t number, const Char_t* type);
   KVVAMOSDetector (const KVVAMOSDetector&) ;
   virtual ~KVVAMOSDetector();
   void Copy (TObject&) const;
   inline virtual Bool_t Fired(Option_t * opt = "any");
   inline virtual Bool_t FiredP(Option_t * opt = "any");
   virtual void   Initialize();
   virtual void   SetCalibrators();
   virtual KVFunctionCal *GetECalibrator() const;
   virtual Bool_t GetPositionInVamos(Double_t &X, Double_t &Y);
   KVFunctionCal *GetTCalibrator(const Char_t *type) const;


   virtual Double_t GetCalibE();
   virtual Double_t GetCalibT(const Char_t *type);
   Double_t GetT0(const Char_t *type) const;
   virtual Bool_t IsECalibrated() const;
   Bool_t   IsTCalibrated(const Char_t *type) const;
   Bool_t   IsTfromThisDetector(const Char_t *type) const;
   void     SetFiredBitmask();
   void     SetT0(const Char_t *type, Double_t t0 = 0.);

   virtual const Char_t *GetEBaseName() const;
   virtual const Char_t *GetTBaseName() const;
   Double_t GetCalibT_HF();
   Float_t  GetT_HF();
   Double_t GetT0_HF() const;
   Bool_t   IsTHFCalibrated() const;
   void     SetT0_HF( Double_t t0 = 0.);

   // -------- inline methods ---------------//

   Float_t GetT(const Char_t *type){
	   if(!IsTfromThisDetector( type ) ) return -1;
	   return  ( gVamos ? gVamos->GetACQData(Form("T%s",type)) : -1 );
   }

   inline TList *GetListOfT0() const{
	   return fT0list;
   }

   inline TList *GetTACQParamList() const{
	   return fTlist;
   }

   ClassDef(KVVAMOSDetector,1)//Detectors of VAMOS spectrometer
};



//________________________________________________________________
Bool_t KVVAMOSDetector::Fired(Option_t * opt)
{
	// Returns kTRUE if detector was hit (fired) in an event
	//
	// The actual meaning of hit/fired depends on the context and the option string opt.
	//
   //opt="any" (default):
   //Returns true if ANY* of the working acquisition parameters associated with the detector were fired in an event
   //opt="all" :
   //Returns true if ALL* of the working acquisition parameters associated with the detector were fired in an event
   //opt="Pany" :
   //Returns true if ANY* of the working acquisition parameters associated with the detector were fired in an event
   //and have a value greater than their pedestal value
   //opt="Pall" :
   //Returns true if ALL* of the working acquisition parameters associated with the detector were fired in an event
   //and have a value greater than their pedestal value
   //
   // *the actual parameters taken into account can be fine tuned using environment variables such as
   //          KVVAMOSDetector.Fired.ACQParameterList.[type]: Q,E,T,T_HF,X,Y
   // See KVAMOSDetector::SetFiredBitmask() for more details.

   if(opt[0]=='P') return FiredP(opt+1);
   return Fired(opt,"");
}
//_________________________________________________________________________________

Bool_t KVVAMOSDetector::FiredP(Option_t * opt)
{
   	//opt="any" :
   	//Returns true if ANY* of the working acquisition parameters associated with the detector were fired in an event
   	//and have a value greater than their pedestal value
   	//opt="all" :
   	//Returns true if ALL* of the working acquisition parameters associated with the detector were fired in an event
   	//and have a value greater than their pedestal value
   	//
   	// *the actual parameters taken into account can be fine tuned using environment variables such as
   	//          KVVAMOSDetector.Fired.ACQParameterList.[type]: Q,E,T,T_HF,X,Y
   	// See KVAMOSDetector::SetFiredBitmask() for more details.

	return Fired(opt,"P");
}

#endif
