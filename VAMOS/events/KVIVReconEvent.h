//Created by KVClassFactory on Mon Mar 18 10:44:28 2013
//Author: Guilain ADEMARD

#ifndef __KVIVRECONEVENT_H
#define __KVIVRECONEVENT_H

#include "KVINDRAReconEvent.h"
#include "KVVAMOSReconNuc.h"
#include "KVVAMOSCodes.h"

class KVIVReconEvent : public KVINDRAReconEvent
{
	private:

		KVVAMOSCodes *fVAMOSCodeMask; //!codes accepted for "good" VAMOS nucleus (IsOK)
		inline KVVAMOSCodes *GetVAMOSCodeMask(){
      		if (!fVAMOSCodeMask)
         		fVAMOSCodeMask = new KVVAMOSCodes;
      		return fVAMOSCodeMask;
   		};


	protected:

		KVVAMOSReconNuc *fVAMOSnuc; //!VAMOS nucleus of this event
		Bool_t fNucInVAMOS;         //flag set when  a nucleus reconstructed in VAMOS


   	public:

   		KVIVReconEvent(Int_t mult = 50, const char *classname = "KVINDRAReconNuc");
   		void init();
   		virtual ~KVIVReconEvent();


   		        void     AcceptECodesVAMOS (UChar_t  code);
		        void     AcceptFPCodesVAMOS(UInt_t   code);
		        void     AcceptIDCodesVAMOS(UShort_t code);
		virtual void     CalibrateVAMOSevent();
   		virtual void     Clear(Option_t * opt = "");
		virtual Int_t    GetTotalMult(Option_t * opt = "");
   		KVVAMOSReconNuc *GetVAMOSNuc(Option_t *opt = "");
		virtual void     IdentifyVAMOSevent_A();
		virtual void     IdentifyVAMOSevent_Z();
		virtual void     Print(Option_t * option = "") const;
		virtual Bool_t   ReconstructVAMOSevent();



		inline void AcceptECodesINDRA (UChar_t  code){ AcceptECodes( code ); }
		inline void AcceptIDCodesINDRA(UShort_t code){ AcceptIDCodes( code ); }

		inline Bool_t CheckINDRACodes(KVINDRACodeMask & code) {
      		//returns kTRUE if "code" is compatible with INDRA event's code mask
      		//if no code mask set for event, returns kTRUE in all cases
         	return CheckCodes( code );
   		}

		inline Bool_t CheckVAMOSCodes(KVVAMOSCodes & code) {
      		//returns kTRUE if "code" is compatible with VAMOS event's code mask.
      		//If no code mask set for event, returns kTRUE in all cases
      		if (!fVAMOSCodeMask)
         		return kTRUE;
      		return ((*fVAMOSCodeMask) & code);
   		}

   		ClassDef(KVIVReconEvent,1)//Event reconstructed from energy losses in INDRA array and VAMOS spectrometer
};

#endif
