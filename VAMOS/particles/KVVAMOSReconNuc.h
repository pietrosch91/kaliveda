//Created by KVClassFactory on Thu Sep 13 10:51:51 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSRECONNUC_H
#define __KVVAMOSRECONNUC_H

#include "KVReconstructedNucleus.h"
#include "KVVAMOSCodes.h"
#include "KVVAMOSReconTrajectory.h"

class KVVAMOSDetector;

class KVVAMOSReconNuc : public KVReconstructedNucleus
{
	private:

		KVVAMOSCodes fCodes; //Focal plane Pos. recon., calib. and ident. codes

	protected:

		KVVAMOSReconTrajectory fRT;             //handles trajectory reconstruction data
		Double_t               fStripFoilEloss; // calculated energy lost in the stripping foil
		Float_t                fToF;            //Time of Flight value in ns
		Float_t                fFlightDist;     //Flight distance in cm
		KVNameValueList        fTrackRes;       //list of tracking results with step for each crossed detector

		virtual void CalibrateFromDetList();
		virtual void CalibrateFromTracking();
		virtual void MakeDetectorList();
		virtual Bool_t SetCorrectedToF( Double_t tof );
		virtual Bool_t SetFlightDistance( KVVAMOSDetector *start, KVVAMOSDetector *stop=NULL );

   	public:

		KVVAMOSReconNuc();
   		KVVAMOSReconNuc (const KVVAMOSReconNuc&) ;
   		virtual ~KVVAMOSReconNuc();
   		virtual void Copy (TObject&) const;
		void init();

		virtual void     Calibrate();
		virtual Bool_t   CheckTrackingCoherence();
		virtual void     Clear(Option_t * t = "");
		        Double_t GetRealA()        const;
		        Double_t GetRealAoverQ()   const;
 		virtual void     Identify();
		virtual void     ReconstructTrajectory();
		virtual void     ReconstructFPtraj();
//		virtual void     ReconstructFPtrajByFitting();
		virtual void     ReconstructLabTraj();

		virtual void     RunTrackingAtFocalPlane();
		virtual void     SetFlightDistanceAndTime();


		//-------------- inline methods -----------------//

		        Double_t         GetBetaFromToF()                    const;
                Float_t          GetBrho()                           const;
                KVVAMOSCodes    &GetCodes();
				Float_t          GetFlightDistance()                 const;
   		const   TVector3        &GetFocalPlaneDirection()            const;
				Double_t         GetGammaFromToF()                   const;
   		const   TVector3        &GetLabDirection()                   const;
                Float_t          GetPath()                           const;
                Float_t          GetPhiF()                           const;
	            Float_t          GetPhiL()                           const;
                Float_t          GetPhiV()                           const;
		virtual Double_t         GetStripFoilEnergyLoss()            const;
        	    Float_t          GetThetaF()                         const;
	            Float_t          GetThetaL()                         const;
                Float_t          GetThetaV()                         const;
				Float_t          GetTimeOfFlight()                   const;
				Float_t          GetToF()                            const;
		        KVNameValueList *GetTrackingResults();
				Double_t         GetVelocityFromToF()                const;
                Float_t          GetXf()                             const;
	            Float_t          GetYf()                             const;
   		virtual	void             SetECode(UChar_t code_mask);
   		virtual void             SetFPCode(UInt_t code_mask);
 		virtual void             SetIDCode(UShort_t code_mask);
		virtual void             SetStripFoilEnergyLoss( Double_t e);
   		virtual void             SetTCode(UShort_t code_mask);
   		virtual void             SetTCode(const Char_t *parname);

   		ClassDef(KVVAMOSReconNuc,1)//Nucleus identified by VAMOS spectrometer
};


//____________________________________________________________________________________________//

inline Double_t KVVAMOSReconNuc::GetBetaFromToF() const{
	return GetVelocityFromToF()/C();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetBrho() const{
	return fRT.Brho;
}
//____________________________________________________________________________________________//

inline KVVAMOSCodes &KVVAMOSReconNuc::GetCodes(){
	return fCodes;
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetFlightDistance() const{
	return fFlightDist;
}
//____________________________________________________________________________________________//

inline const TVector3 &KVVAMOSReconNuc::GetFocalPlaneDirection() const{
	return fRT.dirFP;
}
//____________________________________________________________________________________________//

inline Double_t KVVAMOSReconNuc::GetGammaFromToF() const{
	Double_t b = GetBetaFromToF();
	return 1.0/TMath::Sqrt( 1 - b*b );
}
//____________________________________________________________________________________________//

inline const TVector3 &KVVAMOSReconNuc::GetLabDirection() const{
	return fRT.dirLab;
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetPath() const{
	return fRT.path;
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetPhiF() const{
	return fRT.GetPhiF();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetPhiL() const{
	return fRT.GetPhiL();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetPhiV() const{
	return fRT.GetPhiV();
}
//____________________________________________________________________________________________//

inline Double_t KVVAMOSReconNuc::GetStripFoilEnergyLoss() const{
	// Returns calculated energy loss in stripping foil of reconstructed nucleus ( in MeV )
	return fStripFoilEloss;
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetThetaF() const{
	return fRT.GetThetaF();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetThetaL() const{
	return fRT.GetThetaL();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetThetaV() const{
	return fRT.GetThetaV();
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetTimeOfFlight() const{
	return fToF;
}
//____________________________________________________________________________________________//

inline Float_t KVVAMOSReconNuc::GetToF() const{
	return GetTimeOfFlight();
}
//____________________________________________________________________________________________//

inline KVNameValueList *KVVAMOSReconNuc::GetTrackingResults(){
	return &fTrackRes;
}
//____________________________________________________________________________________________//

inline Double_t KVVAMOSReconNuc::GetVelocityFromToF() const{
return (fToF ? fFlightDist/fToF : 0); 
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

inline void KVVAMOSReconNuc::SetIDCode(UShort_t code_mask)
{
   	//Sets code for identification
   	GetCodes().SetIDCode(code_mask);
}
//____________________________________________________________________________________________//

inline void KVVAMOSReconNuc::SetStripFoilEnergyLoss( Double_t e ){
	// Set energy loss in the stripping foil ( in MeV ) of reconstructed nucleus
	fStripFoilEloss = e;
}
//____________________________________________________________________________________________//

inline void KVVAMOSReconNuc::SetTCode(UShort_t code_mask)
{
   	//Sets code for energy calibration
   	GetCodes().SetTCode(code_mask);
}
//____________________________________________________________________________________________//
inline void KVVAMOSReconNuc::SetTCode(const Char_t *parname){
   	//Sets code for energy calibration from the name of the acquisition 
	//parameter used for the time of flight of the nucleus
   	GetCodes().SetTCode(parname);
}
#endif
