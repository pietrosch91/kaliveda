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

		Double_t fXf; //Horizontal position at the focal plan in cm
		Double_t fYf; //Vertical position at the focal plan in cm
		Double_t fThetaf; //Angle in the horizontal plan in deg
		Double_t fPhif;   //Angle in the vertical plan in deg


   	public:

   		KVVAMOSReconNuc();
   		KVVAMOSReconNuc (const KVVAMOSReconNuc&) ;
   		virtual ~KVVAMOSReconNuc();
   		void Copy (TObject&) const;

		virtual void Calibrate();
		virtual void ConstructFocalPlanTrajectory(KVList *detlist);
		virtual void ConstructLabTrajectory();

		//-------------- inline methods -----------------//

		inline KVVAMOSCodes &GetCodes(){
			return fCodes;
		}

		inline Double_t GetXf() const{
			return fXf;
		}

		inline Double_t GetYf() const{
			return fYf;
		}

		inline Double_t GetThetaf() const{
			return fThetaf;
		}

		inline Double_t GetPhif() const{
			return fPhif;
		}

   		ClassDef(KVVAMOSReconNuc,1)//Nucleus identified by VAMOS spectrometer
};

#endif
