//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOS_H
#define __KVVAMOS_H

#include "KVBase.h"
#include "KVSeqCollection.h"

class KVACQParam;


class KVVAMOS : public KVBase
{

	private:

	protected:
		KVSeqCollection *fACQParams; // References to data acquisition parameters associated to detectors
		TString fDataSet;            // Name of associated dataset, used with MakeVAMOS	
		KVSeqCollection *fDetectors; // List of references to all detectors of VAMOS
		KVSeqCollection *fVACQParams;// References ro data acquisition parameter belonging to VAMOS

   virtual void BuildGeometry();
   virtual void MakeListOfDetectors();
   virtual void SetACQParams();
   virtual void SetArrayACQParams();
   virtual void SetCalibrators();
   virtual void SetIDTelescopes();
   

   public:
   KVVAMOS();
   KVVAMOS (const KVVAMOS&) ;
   virtual ~KVVAMOS();

   void init();

   void AddACQParam(KVACQParam* par, Bool_t owner = kFALSE);
   virtual void Build();
   virtual void Clear(Option_t *opt = "" );
   void Copy (TObject&) const;

   static KVVAMOS *MakeVAMOS(const Char_t* name);
   virtual void SetParameters(UShort_t run);

   
   // ----- inline methods -----------

   inline KVACQParam* GetACQParam(const Char_t *name){
	   return (KVACQParam*)fACQParams->FindObject(name);
   };

   inline KVSeqCollection* GetACQParams()      { return fACQParams;  };
   inline KVSeqCollection* GetListOfDetectors(){ return fDetectors;  };
   inline KVSeqCollection* GetVACQParams()     { return fVACQParams; };

   ClassDef(KVVAMOS,1)//VAMOS: variable mode spectrometer at GANIL
};

//................  global variable
R__EXTERN KVVAMOS *gVamos;

#endif
