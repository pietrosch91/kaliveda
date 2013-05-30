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

		KVHashList *fACQParams;    //References to data acquisition parameter associated to detectors
		Double_t    fAngle;        //!Angular rotation around the target (deg)
		Double_t    fBrhoRef;      //!Reference magnetic rigidity (T.m)
		Double_t    fBeamHF;       //!High frequency of the beam (MHz)
		KVHashList *fCalibrators;  //References to calibrator associated to detectors
		TString     fDataSet;      //Name of associated dataset, used with MakeVAMOS	
		KVList     *fDetectors;    //->List of references to all detectors of VAMOS
		KVList     *fFiredDets;    //!List of fired detectors of VAMOS
		Double_t    fFocalPos;     //!Position of the focal plane from target center (in cm)
		TGeoHMatrix fFocalToTarget;//!focal-plane to target position transformation matrix
		TGeoVolume *fFPvolume;     //!TGeoVolume centered on the focal plane
		UInt_t      fGr;           //!Used to number groups
		KVList     *fGroups;       //->List of groups (KVSpectroGroup) of detectors
		KVHashList *fIDTelescopes; //->deltaE-E telescopes in groups
		TGeoRotation          *fRotation;   //!rotation matrix around the target
		KVVAMOSTransferMatrix *fTransMatrix;//!Transfer matrix for the reconstruction LAB<-->FP
		KVList      *fVACQParams;  //->References to data acquisition parameter belonging to VAMOS
		TGeoVolume  *fVAMOSvol;    //!TGeoVolume of VAMOS 
		KVList      *fVCalibrators;//->References to calibrator belonging to VAMOS


   		virtual void   BuildFocalPlaneGeometry( TEnv *infos );
   		virtual Bool_t BuildGeoVolume( TEnv *infos );
   		virtual void   BuildVAMOSGeometry();
   		virtual void   InitGeometry();
   		virtual Int_t  LoadGeoInfosIn( TEnv *infos );
   		virtual void   MakeListOfDetectors();
		virtual Bool_t ReadDetectorGroupFile( ifstream &ifile );
   		virtual void   SetACQParams();
   		virtual void   SetArrayACQParams();
   		virtual void   SetCalibrators();
   		virtual void   SetGroupsAndIDTelescopes();
		virtual void   UpdateGeometry();


   	public:

   		KVVAMOS();
   		KVVAMOS ( const KVVAMOS &) ;
   		virtual ~KVVAMOS();

   		void init();

   		        void     AddACQParam  ( KVACQParam* par, Bool_t owner = kFALSE   );
   		        Bool_t   AddCalibrator( KVCalibrator *cal, Bool_t owner = kFALSE );
   		virtual void     Build();
   		virtual void     Clear( Option_t *opt = "" );
   		        void     Copy ( TObject & ) const;
   		        void     FocalToTarget( const Double_t *focal, Double_t *target );
   		        void     FocalToTargetVect( const Double_t *focal, Double_t *target );
   		virtual KVList  *GetFiredDetectors( Option_t *opt="Pany" );
		KVVAMOSTransferMatrix *GetTransferMatrix();
   		virtual void     Initialize();
   		static  KVVAMOS *MakeVAMOS( const Char_t* name );
   		virtual void     SetParameters( UShort_t run );
		        void     SetTransferMatrix( KVVAMOSTransferMatrix *mat );
   		        void     TargetToFocal( const Double_t *target, Double_t *focal );
   		        void     TargetToFocalVect( const Double_t *target, Double_t *focal );

   		// ----- inline methods

		void          GeoModified();
		KVACQParam   *GetACQParam( const Char_t *name );
   		KVHashList   *GetACQParams()                        const;
		Double_t      GetAngle()                            const;
		Double_t      GetBeamHF()                           const;
		Double_t      GetBeamPeriod()                       const;
		Double_t      GetBrhoRef()                          const;
		KVCalibrator *GetCalibrator( const Char_t * type )  const;
   		KVDetector   *GetDetector( const Char_t *name )     const;
		TGeoVolume   *GetFocalPlaneVolume()                 const;
   		TGeoHMatrix   GetFocalToTargetMatrix();
   		TGeoVolume   *GetGeoVolume()                        const;
		KVList       *GetGroups()                           const;
		KVHashList   *GetListOfCalibrators()                const;
   		KVList       *GetListOfDetectors()                  const;
   		KVList       *GetListOfVCalibrators()               const;
   		KVList       *GetVACQParams()                       const;
   		KVCalibrator *GetVCalibrator( const Char_t * type ) const;
   		Bool_t        IsBuilt()                             const;
		Bool_t        IsGeoModified()                       const;
		void          SetAngle( Double_t angle );
		void          SetBeamHF( Double_t hf );
		void          SetBrhoRef( Double_t Brho );

		//----- static methods

		static UInt_t    CalculateUniqueID ( KVBase *param, KVVAMOSDetector *det = NULL );
		static UChar_t   GetACQParamTypeIdx( const Char_t *type, KVVAMOSDetector *det = NULL );
		static UChar_t   GetACQParamTypeIdxFromID( UInt_t id ){ return (id/1000)%10;  }
		static KVString &GetACQParamTypes(){ return fACQParamTypes;	}
		static UChar_t   GetPositionTypeIdx( const Char_t *type, KVVAMOSDetector *det = NULL );
		static UChar_t   GetPositionTypeIdxFromID( UInt_t id ){ return (id/10000)%10;	}
		static KVString &GetPositionTypes(){ return fPositionTypes;	}

   		ClassDef(KVVAMOS,1)//VAMOS: variable mode spectrometer at GANIL
};

//_________________________________________________________________________

inline void KVVAMOS::GeoModified() { SetBit( kGeoModified ); }


inline KVACQParam *KVVAMOS::GetACQParam( const Char_t *name ) {
    return (KVACQParam *)fACQParams->FindObject( name );
}


inline KVHashList *KVVAMOS::GetACQParams()  const { return fACQParams; }
inline Double_t    KVVAMOS::GetAngle()      const { return fAngle;     }
inline Double_t    KVVAMOS::GetBeamHF()     const { return fBeamHF;    }


inline Double_t KVVAMOS::GetBeamPeriod() const {
	//Return the beam period in ns
 	return 1.e3/fBeamHF;
}


inline Double_t KVVAMOS::GetBrhoRef() const { return fBrhoRef; }


inline KVCalibrator *KVVAMOS::GetCalibrator( const Char_t * type ) const {
   	if ( fCalibrators )
      	return (KVCalibrator *)fCalibrators->FindObjectByType(type);
   	return 0;
}


inline KVDetector *KVVAMOS::GetDetector( const Char_t *name ) const {
	return (KVDetector *)fDetectors->FindObject( name );
}


inline TGeoVolume *KVVAMOS::GetFocalPlaneVolume() const { return fFPvolume; }


inline TGeoHMatrix KVVAMOS::GetFocalToTargetMatrix(){ 
	if( IsGeoModified() ) UpdateGeometry();
	return fFocalToTarget; 
}


inline TGeoVolume *KVVAMOS::GetGeoVolume() const {
	// Returns the global TGeoVolume of VAMOS built when the method
	// BuildVAMOSGeometry() is called.
	return fVAMOSvol;
}


inline KVList     *KVVAMOS::GetGroups()              const { return fGroups;       }
inline KVHashList *KVVAMOS::GetListOfCalibrators()   const { return fCalibrators;  }
inline KVList     *KVVAMOS::GetListOfDetectors()     const { return fDetectors;    }
inline KVList     *KVVAMOS::GetListOfVCalibrators()  const { return fVCalibrators; }
inline KVList     *KVVAMOS::GetVACQParams()          const { return fVACQParams;   }


inline KVCalibrator *KVVAMOS::GetVCalibrator( const Char_t * type ) const{
   	if ( fVCalibrators )
      	return (KVCalibrator *)fVCalibrators->FindObjectByType( type );
   	return 0;
}


inline Bool_t KVVAMOS::IsBuilt()       const { return TestBit( kIsBuilt );     }
inline Bool_t KVVAMOS::IsGeoModified() const { return TestBit( kGeoModified ); }


inline void KVVAMOS::SetAngle ( Double_t angle ){
	if( fAngle == angle ) return;
 	fAngle = angle;
	if( fRotation ){ 
		fRotation->Clear();
		fRotation->RotateY( angle ); 
	}
	GeoModified();
}


inline void KVVAMOS::SetBeamHF ( Double_t hf    ){ fBeamHF   = hf;   }
inline void KVVAMOS::SetBrhoRef( Double_t Brho  ){ fBrhoRef  = Brho; }


//................  global variable
R__EXTERN KVVAMOS *gVamos;

#endif
