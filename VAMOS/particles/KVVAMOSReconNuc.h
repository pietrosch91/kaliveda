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

		KVVAMOSReconTrajectory fRT;       //handles trajectory reconstruction data
		KVNameValueList        fTrackRes; //list of tracking results with step for each crossed detector

   	public:

   		KVVAMOSReconNuc();
   		KVVAMOSReconNuc (const KVVAMOSReconNuc&) ;
   		virtual ~KVVAMOSReconNuc();
   		virtual void Copy (TObject&) const;
		void init();

		virtual void Calibrate();
		virtual void Clear(Option_t * t = "");
		virtual void ReconstructFocalPlaneTrajectory(KVList *detlist);
		virtual void ReconstructLabTrajectory();
		virtual void RunTracking(KVList *detlist);

		//-------------- inline methods -----------------//

		inline Float_t GetBrho() const{
			return fRT.Brho;
		}

		inline Float_t GetPath() const{
			return fRT.path;
		}

		inline KVVAMOSCodes &GetCodes(){
			return fCodes;
		}

		inline Float_t GetXf() const{
			return fRT.pointFP[0];
		}

		inline Float_t GetYf() const{
			return fRT.pointFP[1];
		}

		inline Float_t GetThetaF() const{
			return fRT.GetThetaF();
		}

		inline Float_t GetPhiF() const{
			return fRT.GetPhiF();
		}

		inline Float_t GetThetaV() const{
			return fRT.GetThetaV();
		}

		inline Float_t GetPhiV() const{
			return fRT.GetPhiV();
		}

		inline Float_t GetThetaL() const{
			return fRT.GetThetaL();
		}

		inline Float_t GetPhiL() const{
			return fRT.GetPhiL();
		}

		inline KVNameValueList *GetTrackingResults(){
			return &fTrackRes;
		}

   		inline TVector3 GetFocalPlaneDirection() const{ return fRT.dirFP;  }
   		inline TVector3 GetLabDirection()        const{ return fRT.dirLab; }


   		ClassDef(KVVAMOSReconNuc,1)//Nucleus identified by VAMOS spectrometer
};

#endif
