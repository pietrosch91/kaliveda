//Created by KVClassFactory on Wed Jul 25 10:12:53 2012
//Author: Guilain ADEMARD

#ifndef __KVHARPEESI_H
#define __KVHARPEESI_H

#include "KVVAMOSDetector.h"

class KVHarpeeSi : public KVVAMOSDetector
{
	private:

	enum {
		kPosIsOK = BIT(20) //flag set when PositionIsOK method is called
	};

	protected:
		static KVHarpeeSi *fSiForPosition;//! Si used to obtain particle position
		static KVList     *fHarpeeSiList;//! Global list of all KVHarpeeSi objects
		
		void init();

   	public:
   		KVHarpeeSi();
		KVHarpeeSi(UInt_t number, Float_t thick /* um */);
   		KVHarpeeSi (const KVHarpeeSi&) ;
   		virtual ~KVHarpeeSi();
   		void Copy (TObject&) const;


		virtual const Char_t* GetArrayName();
   		virtual Double_t GetCalibT(const Char_t *type);
   		virtual const Char_t *GetEBaseName() const;
		static KVHarpeeSi *GetFiredHarpeeSi(Option_t *opt="Pany");
		static KVList *GetHarpeeSiList();
   		virtual Int_t  GetMult(Option_t *opt="Pany");
   		virtual void   Initialize();
   		virtual Bool_t PositionIsOK();
		
		virtual void SetACQParams();

   		// -------- inline methods ---------------//

		inline Float_t GetRawE(){
	   		return  GetACQData( GetEBaseName() );
		}

   ClassDef(KVHarpeeSi,1)//Silicon detectors of Harpee, used at the focal plan of VAMOS
};

#endif
