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

   	public:

   		KVVAMOSReconNuc();
   		KVVAMOSReconNuc (const KVVAMOSReconNuc&) ;
   		virtual ~KVVAMOSReconNuc();
   		void Copy (TObject&) const;

		virtual void Calibrate();
		virtual void ConstructFocalPlanTrajectory(KVList *detlist);
		virtual void ConstructLabTrajectory();


		inline KVVAMOSCodes &GetCodes(){
			return fCodes;
		}

   		ClassDef(KVVAMOSReconNuc,1)//Nucleus identified by VAMOS spectrometer
};

#endif
