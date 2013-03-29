//Created by KVClassFactory on Fri Mar 29 08:56:52 2013
//Author: Guilain ADEMARD

#ifndef __KVVAMOSTRANSFERMATRIX_H
#define __KVVAMOSTRANSFERMATRIX_H

#include "KVBase.h"
#include "KVVAMOSReconTrajectory.h"

class KVVAMOSTransferMatrix : public KVBase
{

	private:


		UChar_t    fOrder; // order of the matrix reconstruction
		UShort_t   fNcoef; // Number of coefficients for each reconstructed parameter 
		Double_t **fCoef;  //[4][fNcoef] coefficients for each parameter (Brho, theta, phi, path)
		

   public:
   KVVAMOSTransferMatrix();
   KVVAMOSTransferMatrix(UChar_t order);
   KVVAMOSTransferMatrix (const KVVAMOSTransferMatrix&) ;
   virtual ~KVVAMOSTransferMatrix();
   virtual void Copy (TObject&) const;
   void init();

   void ReconstructTargetToFP( KVVAMOSReconTrajectory *){
	   Warning("ReconstructTargetToFP","To be implemented");
   }

   void ReconstructFPtoTarget( KVVAMOSReconTrajectory *){
	   Warning("ReconstructFPtoTarget","To be implemented");
   }

   ClassDef(KVVAMOSTransferMatrix,1)//VAMOS's transformation matrix to map the measured coordinates at the focal plane back to the target
};

#endif
