//Created by KVClassFactory on Fri Mar 29 08:56:52 2013
//Author: Guilain ADEMARD

#ifndef __KVVAMOSTRANSFERMATRIX_H
#define __KVVAMOSTRANSFERMATRIX_H

#include "Riostream.h"
#include "KVBase.h"
#include "KVVAMOSReconTrajectory.h"
using namespace std;

class KVVAMOSTransferMatrix : public KVBase
{
	public:

		enum EReconParam{
			kBrho = 0,
			kTheta,
			kPhi,
			kPath
		};

	protected:

		UChar_t       fOrder; // order of the matrix reconstruction
		Double_t *****fCoef;  //[4][fOrder][fOrder][fOrder][fOrder] coefficients for each parameter (Brho, theta, phi, path)
		
		enum {
			kIsReady = BIT(14) //flag set when the coef. have been set up
		};

		void NewMatrix();
		void DeleteMatrix();
		void ReadCoefficients( EReconParam par, ifstream &file );
		void RecursiveReadOfCoef( EReconParam par, ifstream &file, UChar_t order, UChar_t *n, Int_t &coef_num, Int_t prev_i=0 );


   public:

   KVVAMOSTransferMatrix( Bool_t scan_ds = kFALSE);
   KVVAMOSTransferMatrix(UChar_t order);
   KVVAMOSTransferMatrix (const KVVAMOSTransferMatrix&) ;
   virtual ~KVVAMOSTransferMatrix();
   virtual void Copy (TObject&) const;
   void init();

   virtual void Print(Option_t * option = "") const;
   Bool_t ReconstructFPtoLab( KVVAMOSReconTrajectory *traj);
   Bool_t ReconstructLabToFP( KVVAMOSReconTrajectory *traj);

   virtual void ReadCoefInDataSet();
   virtual void Reset();


   void SetCoef( EReconParam par, Int_t i, Int_t j, Int_t k, Int_t l, Double_t coef);

   //------------- inline methods ------------------------------//

   inline UChar_t GetOrder() const{ return fOrder; }
   inline void    SetOrder( UChar_t order ){ 
	   ResetBit( kIsReady );
	   if( fOrder == order ){ 
		   Reset();
		   return;
	   }
	   fOrder = order;
	   NewMatrix();
   }

   inline Bool_t IsReady() const{ return TestBit( kIsReady ); }
   inline void SetIsReady( Bool_t ok = kTRUE ){ SetBit( kIsReady, ok ); }

   ClassDef(KVVAMOSTransferMatrix,1)//VAMOS's transformation matrix to map the measured coordinates at the focal plane back to the target
};

#endif
