//Created by KVClassFactory on Wed Jul 25 10:12:53 2012
//Author: Guilain ADEMARD

#ifndef __KVHARPEESI_H
#define __KVHARPEESI_H

#include "KVVAMOSDetector.h"

class KVPulseHeightDefect;

class KVHarpeeSi : public KVVAMOSDetector
{
	private:

		enum {
			kPosIsOK = BIT(22) //flag set when PositionIsOK method is called
		};
		static KVString fACQParamTypes; //!types of available Acquision parameters
		static KVString fPositionTypes; //!types of available positions


	protected:

		static KVHarpeeSi *fSiForPosition;//! Si used to obtain particle position
		static KVList     *fHarpeeSiList;//! Global list of all KVHarpeeSi objects
 		KVPulseHeightDefect *fPHD;//! pulse height defect
		void init();

   	public:

   		KVHarpeeSi();
		KVHarpeeSi(UInt_t number, Float_t thick /* um */);
   		KVHarpeeSi (const KVHarpeeSi&) ;
   		virtual ~KVHarpeeSi();
   		void Copy (TObject&) const;

		virtual const Char_t *GetArrayName();
   		virtual const Char_t *GetEBaseName() const;

   		virtual Double_t    GetCalibT(const Char_t *type);
		virtual TF1        *GetELossFunction(Int_t Z, Int_t A);
		virtual Double_t    GetEnergy();
		static  KVHarpeeSi *GetFiredHarpeeSi(Option_t *opt="Pany");
		static  KVList     *GetHarpeeSiList();
   		virtual Int_t       GetMult(Option_t *opt="Pany");
		        Double_t    GetPHD(Double_t dE, UInt_t Z);
   		virtual void        Initialize();
   		virtual Bool_t      PositionIsOK();
		virtual void        SetACQParams();
		virtual void        SetCalibrators();
		virtual void        SetMoultonPHDParameters(Double_t a1, Double_t a2, Double_t b1, Double_t b2);
   		// -------- inline methods ---------------//

		virtual KVString &GetACQParamTypes();
		virtual KVString &GetPositionTypes();
                Float_t  GetRawE();

   		ClassDef(KVHarpeeSi,1)//Silicon detectors of Harpee, used at the focal plan of VAMOS
};
//_________________________________________________________________________

inline KVString &KVHarpeeSi::GetACQParamTypes(){ return fACQParamTypes; }
inline KVString &KVHarpeeSi::GetPositionTypes(){ return fPositionTypes; }
inline Float_t   KVHarpeeSi::GetRawE()         { return GetACQData( GetEBaseName() ); }

#endif
