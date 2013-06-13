//Created by KVClassFactory on Mon Mar 18 10:44:28 2013
//Author: Guilain ADEMARD

#ifndef __KVIVRECONEVENT_H
#define __KVIVRECONEVENT_H

#include "KVINDRAReconEvent.h"
#include "KVVAMOSReconEvent.h"

class KVIVReconEvent : public KVINDRAReconEvent
{

	protected:

		KVVAMOSReconEvent *fVAMOSev; //!VAMOS event

   	public:

   		KVIVReconEvent(Int_t mult_indra = 50, const char *cl_name_indra = "KVINDRAReconNuc", Int_t mult_vamos = 10, const char *cl_name_vamos = "KVVAMOSReconNuc");
   	
		void init();
   		virtual ~KVIVReconEvent();

   		virtual void     Clear(Option_t * opt = "");
		virtual Int_t    GetTotalMult(Option_t * opt = "");
		virtual void     Print(Option_t * option = "") const;


		inline void AcceptECodesINDRA (UChar_t  code){ AcceptECodes( code ); }
		inline void AcceptECodesVAMOS (UChar_t  code){ fVAMOSev->AcceptECodes ( code ); }
		inline void AcceptFPCodesVAMOS(UInt_t code  ){ fVAMOSev->AcceptFPCodes( code ); }
		inline void AcceptIDCodesINDRA(UShort_t code){ AcceptIDCodes( code ); }
		inline void AcceptIDCodesVAMOS(UShort_t code){ fVAMOSev->AcceptIDCodes( code ); }

		inline Bool_t CheckINDRACodes(KVINDRACodeMask & code) {
      		//returns kTRUE if "code" is compatible with INDRA event's code mask
      		//if no code mask set for event, returns kTRUE in all cases
         	return CheckCodes( code );
   		}

		inline Bool_t CheckVAMOSCodes(KVVAMOSCodes & code) {
      		//returns kTRUE if "code" is compatible with VAMOS event's code mask.
      		//If no code mask set for event, returns kTRUE in all cases
      		return fVAMOSev->CheckCodes( code );
   		}

		inline void IdentifyVAMOSEvent_A(){ fVAMOSev->IdentifyEvent_A(); }
		inline void IdentifyVAMOSEvent_Z(){ fVAMOSev->IdentifyEvent_Z(); }
		inline void ReconstructVAMOSEvent(KVDetectorEvent *kvde){ fVAMOSev->ReconstructEvent( kvde ); }

   		ClassDef(KVIVReconEvent,1)//Event reconstructed from energy losses in INDRA array and VAMOS spectrometer
};

#endif
