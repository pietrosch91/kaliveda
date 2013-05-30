//Created by KVClassFactory on Tue Sep  4 11:26:12 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOS_H
#define __KVVAMOS_H

#include "KVBase.h"
#include "KVList.h"
#include "KVHashList.h"
#include "TGeoMatrix.h"

class KVACQParam;
class KVVAMOSDetector;
class TGeoVolume;
class TEnv;
class KVCalibrator;
class KVDetector;
class KVVAMOSTransferMatrix;

class KVVAMOS :  public KVBase
{

	private:

		enum{
			kIsBuilt     = BIT(20), //flag set when Build() is called
			kGeoModified = BIT(21)  //flag set when the geometry is modified

		};

		static KVString fACQParamTypes; //!types of available Acquision parameters
		static KVString fPositionTypes; //!types of available positions

	protected:

		TGeoRotation *fRotation;//!rotation matrix around the target
		Double_t fAngle;       //!Angular rotation around the target (deg)
		Double_t fBrhoRef;     //!Reference magnetic rigidity (T.m)
		Double_t fBeamHF;      //!High frequency of the beam (MHz)
		TString  fDataSet;     //Name of associated dataset, used with MakeVAMOS	
		KVList *fDetectors;    // List of references to all detectors of VAMOS
		KVList *fFiredDets;    //!List of fired detectors of VAMOS
		UInt_t  fGr;           //!Used to number groups
		KVList *fGroups;       //!List of groups (KVSpectroGroup) of detectors
		KVHashList *fACQParams;//References to data acquisition parameter associated to detectors
		KVList *fVACQParams;   //References to data acquisition parameter belonging to VAMOS
		KVHashList *fCalibrators;//References to calibrator associated to detectors
		KVList *fVCalibrators; //References to calibrator belonging to VAMOS
		TGeoVolume *fFPvolume; //!TGeoVolume centered on the focal plane
		TGeoVolume *fVAMOSvol; //!TGeoVolume of VAMOS 
		Double_t fFocalPos;    //!Position of the focal plane from target center (in cm)
		TGeoHMatrix fFocalToTarget; //!focal-plane to target position transformation matrix
		KVVAMOSTransferMatrix *fTransMatrix; //!Transfer matrix for the reconstruction LAB<-->FP

   		virtual void   BuildFocalPlaneGeometry(TEnv *infos);
   		virtual Bool_t BuildGeoVolume(TEnv *infos);
   		virtual void   BuildVAMOSGeometry();
   		virtual void   InitGeometry();
   		virtual Int_t  LoadGeoInfosIn(TEnv *infos);
   		virtual void   MakeListOfDetectors();
		virtual Bool_t ReadDetectorGroupFile( ifstream &ifile );
   		virtual void   SetACQParams();
   		virtual void   SetArrayACQParams();
   		virtual void   SetCalibrators();
   		virtual void   SetGroupsAndIDTelescopes();
		virtual void   UpdateGeometry();


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
		KVVAMOSTransferMatrix *GetTransferMatrix();
   		virtual void Initialize();
   		static KVVAMOS *MakeVAMOS(const Char_t* name);
   		virtual void SetParameters(UShort_t run);
		void SetTransferMatrix( KVVAMOSTransferMatrix *mat );


   		void FocalToTarget(const Double_t *focal, Double_t *target);
   		void TargetToFocal(const Double_t *target, Double_t *focal);
   		void FocalToTargetVect(const Double_t *focal, Double_t *target);
   		void TargetToFocalVect(const Double_t *target, Double_t *focal);



   		// ----- inline methods -----------


		inline void   GeoModified(){ SetBit( kGeoModified ); }
		inline Bool_t IsGeoModified() const { return TestBit( kGeoModified ); }

   		inline KVHashList* GetACQParams() const { return fACQParams; }
		inline KVCalibrator *GetCalibrator(const Char_t * type) const{
   			if (fCalibrators)
      			return (KVCalibrator *) fCalibrators->FindObjectByType(type);
   			return 0;
   		}
   		inline KVCalibrator *GetVCalibrator(const Char_t * type) const{
   			if (fVCalibrators)
      			return (KVCalibrator *) fVCalibrators->FindObjectByType(type);
   			return 0;
   		}
 		
		inline KVACQParam* GetACQParam(const Char_t *name){
           return (KVACQParam*)fACQParams->FindObject(name);
		}
   		inline KVDetector* GetDetector(const Char_t *name) const {
	   		return (KVDetector*)fDetectors->FindObject(name);
   		}
		inline TGeoVolume* GetFocalPlaneVolume() const{ return fFPvolume; }
   		inline TGeoHMatrix GetFocalToTargetMatrix(){ 
			if( IsGeoModified() ) UpdateGeometry();
			return fFocalToTarget; 
		}

   		inline TGeoVolume* GetGeoVolume() const{
			// Returns the global TGeoVolume of VAMOS built when the method
			// BuildVAMOSGeometry() is called.
			return fVAMOSvol;
		}

		inline KVList* GetGroups() const            { return fGroups;           }
   		inline KVList* GetListOfDetectors() const   { return fDetectors;        }
   		inline KVList* GetVACQParams() const        { return fVACQParams;       }
   		inline KVHashList* GetListOfCalibrators()const  { return fCalibrators;      }
   		inline KVList* GetListOfVCalibrators()const { return fVCalibrators;     }
   		inline Bool_t  IsBuilt() const              { return TestBit(kIsBuilt); }


		static KVString &GetACQParamTypes(){ return fACQParamTypes;	}
		static KVString &GetPositionTypes(){ return fPositionTypes;	}

		static UChar_t GetACQParamTypeIdx( const Char_t *type, KVVAMOSDetector *det = NULL );
		static UChar_t GetPositionTypeIdx( const Char_t *type, KVVAMOSDetector *det = NULL );
		static UInt_t CalculateUniqueID( KVBase *param, KVVAMOSDetector *det = NULL);

		static UChar_t GetACQParamTypeIdxFromID( UInt_t id ){
			return (id/1000)%10;
		}
		static UChar_t GetPositionTypeIdxFromID( UInt_t id ){
			return (id/10000)%10;
		}

		inline Double_t GetAngle()      const{ return fAngle;   }
		inline Double_t GetBrhoRef()    const{ return fBrhoRef; }
		inline Double_t GetBeamHF()     const{ return fBeamHF;  }
		inline Double_t GetBeamPeriod() const{
			//Return the beam period in ns
 		   	return 1.e3/fBeamHF;
  	  	}

		inline void SetAngle  ( Double_t angle ){
			if( fAngle == angle ) return;
 		   	fAngle = angle;
			if( fRotation ){ 
				fRotation->Clear();
				fRotation->RotateY( angle ); 
			}
			GeoModified();
		}
		inline void SetBrhoRef( Double_t Brho  ){ fBrhoRef  = Brho;  }
		inline void SetBeamHF ( Double_t hf    ){ fBeamHF   = hf;    }

   		ClassDef(KVVAMOS,1)//VAMOS: variable mode spectrometer at GANIL
};

//................  global variable
R__EXTERN KVVAMOS *gVamos;

#endif
