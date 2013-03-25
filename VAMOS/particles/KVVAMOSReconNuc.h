//Created by KVClassFactory on Thu Sep 13 10:51:51 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSRECONNUC_H
#define __KVVAMOSRECONNUC_H

#include "KVReconstructedNucleus.h"
#include "KVVAMOSCodes.h"

class KVVAMOSReconNuc : public KVReconstructedNucleus
{
	private:

		KVVAMOSCodes fCodes; //Focal plan Pos. recon., calib. and ident. codes

	protected:

		Float_t fXf; //Horizontal position at the focal plan in cm
		Float_t fYf; //Vertical position at the focal plan in cm
		Float_t fFPdir[3];//direction from point (Xf,Yf) on the focal plan

   	public:

   		KVVAMOSReconNuc();
   		KVVAMOSReconNuc (const KVVAMOSReconNuc&) ;
   		virtual ~KVVAMOSReconNuc();
   		void Copy (TObject&) const;
		void init();

		virtual void Calibrate();
		virtual void Clear(Option_t * t = "");
		virtual void ConstructFocalPlanTrajectory(KVList *detlist);
		virtual void ConstructLabTrajectory();

		//-------------- inline methods -----------------//

		inline KVVAMOSCodes &GetCodes(){
			return fCodes;
		}

		inline Float_t GetXf() const{
			return fXf;
		}

		inline Float_t GetYf() const{
			return fYf;
		}

		inline Float_t GetThetaf() const{
			return (TMath::RadToDeg()*fFPdir[0])/fFPdir[2];
		}

		inline Float_t GetPhif() const{
			return (TMath::RadToDeg()*fFPdir[1])/fFPdir[2];
		}

   		ClassDef(KVVAMOSReconNuc,1)//Nucleus identified by VAMOS spectrometer
};

#endif
