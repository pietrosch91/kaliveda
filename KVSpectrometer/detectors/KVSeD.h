//Created by KVClassFactory on Mon Sep  3 11:28:25 2012
//Author: Guilain ADEMARD

#ifndef __KVSED_H
#define __KVSED_H

#define _SED_NX_ 128
#define _SED_NY_ 48

#include "KVVAMOSDetector.h"

class TH1F;

class KVSeD : public KVVAMOSDetector
{

	protected:

		TH1F **fQraw;  //! array of 2 TH2F's for raw charge in X and Y 
		TH1F **fQ;     //! array of 2 TH2F's for calibrated charge in X and Y


		void init();

   public:
   KVSeD();
   KVSeD (const KVSeD&) ;
   virtual ~KVSeD();
   void Copy (TObject&) const;

   virtual void  Clear(Option_t *option = "" );
   virtual const Char_t* GetArrayName();
   virtual TH1F *GetQrawHisto(const Char_t dir = 'X');
   virtual TH1F *GetQHisto(const Char_t dir = 'X');
   virtual void SetACQParams();
   virtual void ShowQrawHisto(const Char_t dir = 'X', Option_t *opt = "");
   virtual void ShowQHisto(const Char_t dir = 'X', Option_t *opt = "");

//   virtual void SetCalibrators(KVDBParameterSet *kvdbps);

   inline Double_t GetTSED_HF(){
	   return GetTimeHF();
   };


   ClassDef(KVSeD,1)//Secondary Electron Detector, used at the focal plan of VAMOS
};

#endif
