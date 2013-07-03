//Created by KVClassFactory on Thu Sep 13 10:51:51 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSRECONNUC_H
#define __KVVAMOSRECONNUC_H

#include "KVReconstructedNucleus.h"
#include "KVVAMOSCodes.h"
#include "KVVAMOSReconTrajectory.h"

class KVVAMOSReconNuc : public KVReconstructedNucleus
{
	private:

		KVVAMOSCodes fCodes; //Focal plane Pos. recon., calib. and ident. codes

	protected:

		KVVAMOSReconTrajectory fRT;             //handles trajectory reconstruction data
		Double_t               fStripFoilEloss; // calculated energy lost in the stripping foil
		KVNameValueList        fTrackRes;       //list of tracking results with step for each crossed detector

		virtual void CalibrateFromDetList();
		virtual void CalibrateFromTracking();
		virtual void MakeDetectorList();

   	public:

   		KVVAMOSReconNuc();
   		KVVAMOSReconNuc (const KVVAMOSReconNuc&) ;
   		virtual ~KVVAMOSReconNuc();
   		virtual void Copy (TObject&) const;
		void init();

		virtual void Calibrate();
		virtual void Clear(Option_t * t = "");
 		virtual void Identify();
		virtual void ReconstructTrajectory();
		virtual void ReconstructFPtraj();
//		virtual void ReconstructFPtrajByFitting();
		virtual void ReconstructLabTraj();

		virtual void RunTrackingAtFocalPlane();
		virtual void RunTrackingAtTargetPoint();

		virtual Bool_t CheckTrackingCoherence();

		//-------------- inline methods -----------------//

                Float_t          GetBrho()                           const;
                Float_t          GetPath()                           const;
                KVVAMOSCodes    &GetCodes();
                Float_t          GetXf()                             const;
	            Float_t          GetYf()                             const;
        	    Float_t          GetThetaF()                         const;
                Float_t          GetPhiF()                           const;
                Float_t          GetThetaV()                         const;
                Float_t          GetPhiV()                           const;
	            Float_t          GetThetaL()                         const;
	            Float_t          GetPhiL()                           const;
		virtual Double_t         GetStripFoilEnergyLoss()            const;
		        KVNameValueList *GetTrackingResults();
   		        TVector3         GetFocalPlaneDirection()            const;
   		        TVector3         GetLabDirection()                   const;
 		virtual void             SetIDCode(UShort_t code_mask);
   		virtual void             SetECode(UChar_t code_mask);
   		virtual void             SetFPCode(UInt_t code_mask);
   		virtual void             SetTCode(UShort_t code_mask);
		virtual void             SetStripFoilEnergyLoss( Double_t e);

   		ClassDef(KVVAMOSReconNuc,1)//Nucleus identified by VAMOS spectrometer
};



//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetBrho() const{
	return fRT.Brho;
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetPath() const{
	return fRT.path;
}
//____________________________________________________________________________________________//

inline KVVAMOSCodes &KVVAMOSReconNuc::GetCodes(){
	return fCodes;
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetXf() const{
	return fRT.pointFP[0];
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetYf() const{
	return fRT.pointFP[1];
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetThetaF() const{
	return fRT.GetThetaF();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetPhiF() const{
	return fRT.GetPhiF();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetThetaV() const{
	return fRT.GetThetaV();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetPhiV() const{
	return fRT.GetPhiV();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetThetaL() const{
	return fRT.GetThetaL();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetPhiL() const{
	return fRT.GetPhiL();
}
//____________________________________________________________________________________________//

inline Double_t KVVAMOSReconNuc::GetStripFoilEnergyLoss() const{
	// Returns calculated energy loss in stripping foil of reconstructed nucleus ( in MeV )
	return fStripFoilEloss;
}
//____________________________________________________________________________________________//

inline KVNameValueList *KVVAMOSReconNuc::GetTrackingResults(){
	return &fTrackRes;
}
//____________________________________________________________________________________________//

inline void KVVAMOSReconNuc::SetIDCode(UShort_t code_mask)
{
   	//Sets code for identification
   	GetCodes().SetIDCode(code_mask);
}
//____________________________________________________________________________________________//

inline void KVVAMOSReconNuc::SetECode(UChar_t code_mask)
{
   	//Sets code for energy calibration
   	GetCodes().SetECode(code_mask);
}
//____________________________________________________________________________________________//

inline void KVVAMOSReconNuc::SetFPCode(UInt_t code_mask)
{
   	//Sets code for energy calibration
   	GetCodes().SetFPCode(code_mask);
}
//____________________________________________________________________________________________//

inline void KVVAMOSReconNuc::SetTCode(UShort_t code_mask)
{
   	//Sets code for energy calibration
   	GetCodes().SetTCode(code_mask);
}
//____________________________________________________________________________________________//

inline void KVVAMOSReconNuc::SetStripFoilEnergyLoss( Double_t e ){
	// Set energy loss in the stripping foil ( in MeV ) of reconstructed nucleus
	fStripFoilEloss = e;
}
#endif
