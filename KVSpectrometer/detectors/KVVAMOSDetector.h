//Created by KVClassFactory on Wed Jul 25 09:43:37 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSDETECTOR_H
#define __KVVAMOSDETECTOR_H

#include "KVSpectroDetector.h"
#include "KVFunctionCal.h"

class KVVAMOSDetector : public KVSpectroDetector
{
	protected:
		
		TList         *fT0list; //! list of T0 saved in a KVNamedParameter

		void init();

   public:

   KVVAMOSDetector();
   KVVAMOSDetector(UInt_t number, const Char_t* type);
   KVVAMOSDetector (const KVVAMOSDetector&) ;
   virtual ~KVVAMOSDetector();
   void Copy (TObject&) const;

   virtual void   Initialize();
   virtual void   SetCalibrators();
   virtual KVFunctionCal *GetECalibrator() const;
   virtual Bool_t GetPositionInVamos(Double_t &X, Double_t &Y);
   KVFunctionCal *GetTCalibrator(const Char_t *type) const;


   virtual Double_t GetCalibE();
   Double_t GetCalibT(const Char_t *type);
   Double_t GetT0(const Char_t *type) const;
   virtual Bool_t IsECalibrated() const;
   Bool_t   IsTCalibrated(const Char_t *type) const;
   Bool_t   IsTfromThisDetector(const Char_t *type) const;
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
	   return  GetACQData( Form("T%s",type) );
   }

   inline TList *GetListOfT0() const{
	   return fT0list;
   }

   ClassDef(KVVAMOSDetector,1)//Detectors of VAMOS spectrometer
};

#endif
