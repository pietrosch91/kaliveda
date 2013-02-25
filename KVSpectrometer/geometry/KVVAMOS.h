//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOS_H
#define __KVVAMOS_H

#include "KVDetector.h"
#include "KVSeqCollection.h"

class KVACQParam;
class KVDetector;


class KVVAMOS : public KVDetector //public KVBase
{

	private:

	protected:
		TString fDataSet;            // Name of associated dataset, used with MakeVAMOS	
		KVList *fDetectors; // List of references to all detectors of VAMOS
		KVList *fVACQParams;// References to data acquisition parameter belonging to VAMOS
		KVList *fVCalibrators;//References to calibrator belonging to VAMOS

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
   Bool_t AddCalibrator(KVCalibrator *cal, Bool_t owner = kFALSE);
   virtual void Build();
   virtual void Clear(Option_t *opt = "" );
   void Copy (TObject&) const;
   virtual void Initialize();
   static KVVAMOS *MakeVAMOS(const Char_t* name);
   virtual void SetParameters(UShort_t run);

   
   // ----- inline methods -----------

   inline KVCalibrator *GetVCalibrator(const Char_t * type) const{
   if (fVCalibrators)
      return (KVCalibrator *) fVCalibrators->FindObjectByType(type);
   return 0;
   }

   inline KVDetector* GetDetector(const Char_t *name){
	   return (KVDetector*)fDetectors->FindObject(name);
   };

   inline KVList* GetListOfDetectors()   { return fDetectors;  };
   inline KVList* GetVACQParamList()     { return fVACQParams; };
   inline KVList* GetListOfVCalibrators(){ return fVCalibrators; };

   ClassDef(KVVAMOS,1)//VAMOS: variable mode spectrometer at GANIL
};

//................  global variable
R__EXTERN KVVAMOS *gVamos;

#endif
