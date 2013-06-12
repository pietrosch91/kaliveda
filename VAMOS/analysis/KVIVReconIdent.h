#ifndef KVIVReconIdent_h
#define KVIVReconIdent_h

#include "KVINDRAReconIdent.h"

class KVIVReconIdent: public KVINDRAReconIdent {

	protected:

		Bool_t fIsIVevent; // flag set when the event class inherits from KVIVReconEvent;


 	public:

    	KVIVReconIdent() { 
			fIsIVevent = kFALSE;
 	   	}
   		virtual ~ KVIVReconIdent() {}

   		virtual void InitRun();
   		virtual void InitAnalysis();
   		virtual Bool_t Analysis();
   		virtual void EndAnalysis();

   		ClassDef(KVIVReconIdent, 0) //Identification and reconstruction of VAMOS and INDRA events from recon data 
};

#endif
