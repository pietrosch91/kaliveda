//Created by KVClassFactory on Tue Mar 12 15:08:25 2013
//Author: Guilain ADEMARD

#ifndef __KVVAMOSCODES_H
#define __KVVAMOSCODES_H

#include "KVINDRACodes.h"

/* Global bits for code masks */
enum FocalPosCodes {
   kFPCode0 = BIT(0),           //these bits concern KVCodes::fFPMask
   kFPCode1 = BIT(1),
   kFPCode2 = BIT(2),
   kFPCode3 = BIT(3),
   kFPCode4 = BIT(4),
   kFPCode5 = BIT(5),
   kFPCode6 = BIT(6),
   kFPCode7 = BIT(7),
   kFPCode8 = BIT(8),
   kFPCode9 = BIT(9),
   kFPCode10 = BIT(10),
   kFPCode11 = BIT(11),
   kFPCode12 = BIT(12),
   kFPCode13 = BIT(13),
   kFPCode14 = BIT(14),
   kFPCode15 = BIT(15),
};

class KVVAMOSCodes : public KVINDRACodes
{

	private: 
		UShort_t fFPMask;    //16-bit mask with Focal plan Position code
   	public:
   		KVVAMOSCodes();
   		KVVAMOSCodes (const KVVAMOSCodes&) ;
   		virtual ~KVVAMOSCodes();
   		void Copy (TObject&) const;


 		UShort_t GetFPMask() {
      		return fFPMask;
   		}
		
   		void SetFPMask(UShort_t codes) {
      		fFPMask = codes;
   		}

   		Bool_t TestFPCode(UShort_t code);

   		Bool_t operator&(KVVAMOSCodes & msk) {
      		return (TestFPCode(msk.GetFPMask()) && TestIDCode(msk.GetIDMask()) && TestECode(msk.GetEMask()));
   		};

   		virtual void Clear(Option_t* opt= "") {
	   		KVINDRACodes::Clear( opt );
       		fFPMask = 0;
   		};

   ClassDef(KVVAMOSCodes,1)//Status for calibration, trajectory reconstruction and identification in VAMOS
};

#endif
