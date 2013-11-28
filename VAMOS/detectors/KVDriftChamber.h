//Created by KVClassFactory on Mon Sep  3 11:29:00 2012
//Author: Guilain ADEMARD

#ifndef __KVDRIFTCHAMBER_H
#define __KVDRIFTCHAMBER_H
#define Border 5

#include "KVVAMOSDetector.h"

class TH1F;

class KVDriftChamber : public KVVAMOSDetector
{
	private:

		static KVString fACQParamTypes; //!types of available Acquision parameters
		static KVString fPositionTypes; //!types of available positions
		static Int_t    fNstripsOK;     //!minimum. numbers of strips for a good position measurement 


	protected:
		
		TH1F    ***fQ;     //! array of TH1F for calibrated charge [raw, calibrated, clean][Chamber 1, Chamber 2] 
		Double_t   fRawPos[2]; //! Measured X raw position for both Chambers
		Double_t   fERawPos[2]; //! Error of measured X raw position for both Chambers
		KVACQParam *fTfilPar;   //! TFIL acquisition parameter
		Float_t     fDriftV;    //! Electron drift velocity in cm/us

		void init();

   public:
   KVDriftChamber();
   KVDriftChamber (const KVDriftChamber&) ;
   virtual ~KVDriftChamber();
   void Copy (TObject&) const;

   virtual Float_t CalculateQThreshold( Int_t c_num );
   virtual void  Clear( Option_t *option = "" );

   virtual TH1F    *GetCleanQHisto( Int_t c_num );
   virtual TH1F    *GetQHisto( Int_t c_num );
   virtual TH1F    *GetQrawHisto( Int_t c_num );
   using KVVAMOSDetector::GetRawPosition;
   virtual Double_t GetRawPosition(const Char_t dir = 'X');
   using KVVAMOSDetector::GetRawPositionError;
   virtual Double_t GetRawPositionError(const Char_t dir = 'X');

   virtual void  Initialize(); virtual const Char_t *GetArrayName();
   virtual const Char_t *GetEBaseName();
   virtual void  ResetCalculatedData();
   virtual void  SetACQParams();

   virtual void ShowCleanQHisto(Int_t c_num=1, Option_t *opt = "");
   virtual void ShowQrawHisto(Int_t c_num=1, Option_t *opt = "");
   virtual void ShowQHisto(Int_t c_num=1 , Option_t *opt = "");


   // ------ inline functions ----------------------//

   virtual KVString &GetACQParamTypes();
   virtual KVString &GetPositionTypes();
           Float_t   GetDriftVelocity() const;
           void      SetDriftVelocity( Float_t v );

   ClassDef(KVDriftChamber,1)//Drift Chamber, used at the focal plan of VAMOS
};
//________________________________________________________________

inline KVString &KVDriftChamber::GetACQParamTypes(){
	return fACQParamTypes;
}
//________________________________________________________________

inline KVString &KVDriftChamber::GetPositionTypes(){
	return fPositionTypes;
}
//________________________________________________________________

inline Float_t KVDriftChamber::GetDriftVelocity() const{ return fDriftV; }
//________________________________________________________________

inline void KVDriftChamber::SetDriftVelocity( Float_t v ){ fDriftV = v; }



#endif
