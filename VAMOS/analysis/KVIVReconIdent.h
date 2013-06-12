#ifndef KVINDRAReconIdent_h
#define KVINDRAReconIdent_h

#include "KVIVSelector.h"

class TFile;
class TTree;

class KVIVReconIdent: public KVIVSelector {

   	protected:
   		TFile *fIdentFile;           //new file
   		TTree *fIdentTree;           //new tree
   		Int_t fRunNumber;
   		Int_t fEventNumber;

 	public:
    	KVIVReconIdent() {
      		fIdentFile = 0;
      		fIdentTree = 0;
   		};
   		virtual ~ KVIVReconIdent() {
   		};

   		virtual void InitRun();
   		virtual void EndRun();
   		virtual void InitAnalysis();
   		virtual Bool_t Analysis();
   		virtual void EndAnalysis();

   		ClassDef(KVIVReconIdent, 0) //Identification and reconstruction of VAMOS and INDRA events from recon data 
};

#endif
