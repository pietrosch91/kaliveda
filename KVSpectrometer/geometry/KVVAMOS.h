//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOS_H
#define __KVVAMOS_H

#include "KVDetector.h"
#include "KVSeqCollection.h"
#include "TGeoMatrix.h"

class KVACQParam;
class KVVAMOSDetector;


class KVVAMOS : public KVDetector //public KVBase
{

	private:

		enum{
			kIsBuilt = BIT(20) //flag set when Build() is called
		};

		static KVString fACQParamTypes; //!types of available Acquision parameters
		static KVString fPositionTypes; //!types of available positions

	protected:

		TString fDataSet;      //Name of associated dataset, used with MakeVAMOS	
		KVList *fDetectors;    // List of references to all detectors of VAMOS
		KVList *fFiredDets;    //!List of fired detectors of VAMOS
		KVList *fVACQParams;   //References to data acquisition parameter belonging to VAMOS
		KVList *fVCalibrators; //References to calibrator belonging to VAMOS
		TGeoVolume *fFPvolume; //!TGeoVolume centered on the focal plane
		Double_t fFocalPos;    //!Position of the focal plan from target center (in cm)
		TGeoHMatrix fFocalToTarget; //!focal-plan to target position transformation matrix

   		virtual void   BuildFocalPlaneGeometry(TEnv *infos);
   		virtual Bool_t BuildGeoVolume(TEnv *infos);
   		virtual void   BuildVAMOSGeometry();
   		virtual void   InitGeometry();
   		virtual Int_t  LoadGeoInfosIn(TEnv *infos);
   		virtual void   MakeListOfDetectors();
   		virtual void   SetACQParams();
   		virtual void   SetArrayACQParams();
   		virtual void   SetCalibrators();
   		virtual void   SetIDTelescopes();


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
   		virtual KVList *GetFiredDetectors(Option_t *opt="Pany");
   		virtual void Initialize();
   		static KVVAMOS *MakeVAMOS(const Char_t* name);
   		virtual void SetParameters(UShort_t run);


   		void FocalToTarget(const Double_t *focal, Double_t *target);
   		void TargetToFocal(const Double_t *target, Double_t *focal);
   		void FocalToTargetVect(const Double_t *focal, Double_t *target);
   		void TargetToFocalVect(const Double_t *target, Double_t *focal);


   		// ----- inline methods -----------


   		inline KVCalibrator *GetVCalibrator(const Char_t * type) const{
   			if (fVCalibrators)
      			return (KVCalibrator *) fVCalibrators->FindObjectByType(type);
   			return 0;
   		}

   		inline KVDetector* GetDetector(const Char_t *name){
	   		return (KVDetector*)fDetectors->FindObject(name);
   		}

   		inline TGeoHMatrix GetFocalToTargetMatrix() const { return fFocalToTarget; }
   		inline KVList* GetListOfDetectors()   { return fDetectors;  }
   		inline KVList* GetVACQParamList()     { return fVACQParams; }
   		inline KVList* GetListOfVCalibrators(){ return fVCalibrators; }
   		inline Bool_t  IsBuilt()              { return TestBit(kIsBuilt); }


		static KVString &GetACQParamTypes(){
			return fACQParamTypes;
		}

		static KVString &GetPositionTypes(){
			return fPositionTypes;
		}

		static UChar_t GetACQParamTypeIdx( const Char_t *type, KVVAMOSDetector *det = NULL );
		static UChar_t GetPositionTypeIdx( const Char_t *type, KVVAMOSDetector *det = NULL );
		static UInt_t CalculateUniqueID( KVBase *param, KVVAMOSDetector *det = NULL);

		static UChar_t GetACQParamTypeIdxFromID( UInt_t id ){
			return (id/1000)%10;
		}
		static UChar_t GetPositionTypeIdxFromID( UInt_t id ){
			return (id/10000)%10;
		}



   		ClassDef(KVVAMOS,1)//VAMOS: variable mode spectrometer at GANIL
};

//................  global variable
R__EXTERN KVVAMOS *gVamos;

#endif
