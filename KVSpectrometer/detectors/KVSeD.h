//Created by KVClassFactory on Mon Sep  3 11:28:25 2012
//Author: Guilain ADEMARD

#ifndef __KVSED_H
#define __KVSED_H

#include "KVVAMOSDetector.h"
#include "TF1.h"
#include "KVSeDPositionCal.h"


#define IDX( dir ) (Int_t)( dir -'X') 
#define DIRECTION( idx ) (Char_t)('X'+idx) 
#define IS_IN_SED_WINDOW( X, Y )( (-20 <= X && X <= 20  && -5 <= Y && Y <= 5) )
#define Border 5
#define ONEOVERSQRTTWO	0.707106781

class TH1F;

class KVSeD : public KVVAMOSDetector
{

	protected:

		TH1F    ***fQ;     //! array of TH1F for calibrated charge [raw, calibrated, clean][X, Y] 
		TF1       *fPeakFunction;//! 
		Double_t   fRawPos[2]; //!
		Double_t   fRefPos[2]; //!
//		TSpectrum *fSpectrum;
		KVSeDPositionCal  *fPosCalib; //! position calibrator

		void init();

   public:
   KVSeD();
   KVSeD (const KVSeD&) ;
   virtual ~KVSeD();
   void Copy (TObject&) const;

   virtual void  Clear(Option_t *option = "" );
   virtual const Char_t* GetArrayName();
   virtual Double_t GetPosition(const Char_t dir = 'X');
   virtual Bool_t GetPosition(Double_t &X, Double_t &Y, Double_t xraw = -1., Double_t yraw = -1.);
   virtual Double_t GetRawPosition(const Char_t dir = 'X');
   virtual Double_t GetRawPosition2(const Char_t dir = 'X', Double_t min_amp = 100, Double_t min_sigma = 0.5, Double_t max_sigma = 3., Int_t maxNpeaks=10);
   virtual TH1F *GetCleanQHisto(const Char_t dir = 'X');
   virtual TH1F *GetQrawHisto(const Char_t dir = 'X');
   virtual TH1F *GetQHisto(const Char_t dir = 'X');
//   virtual Bool_t RemoveNoise();
   virtual void ResetCalculatedData();
   virtual void SetACQParams();
   virtual void SetCalibrators();
   virtual void ShowCleanQHisto(const Char_t dir = 'X', Option_t *opt = "");
   virtual void ShowQrawHisto(const Char_t dir = 'X', Option_t *opt = "");
   virtual void ShowQHisto(const Char_t dir = 'X', Option_t *opt = "");

   

   // ------ inline functions ----------------------//

   inline Double_t GetTSED_HF(){
	   return GetTimeHF();
   };

   inline Double_t GetRefX() const {
	   return fRefPos[0];
   };

   inline Double_t GetRefY() const {
	   return fRefPos[1];
   };

   inline Bool_t IsPositionCalibrated() const{
	   return ( fPosCalib && fPosCalib->GetStatus()  );
   };

   inline void SetRefPosition(Double_t Xref, Double_t Yref){
	   fRefPos[0] = Xref;
	   fRefPos[1] = Yref;
   };

   ClassDef(KVSeD,1)//Secondary Electron Detector, used at the focal plan of VAMOS
};

#endif
