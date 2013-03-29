//Created by KVClassFactory on Fri Mar 29 08:56:52 2013
//Author: Guilain ADEMARD

#include "KVVAMOSTransferMatrix.h"

ClassImp(KVVAMOSTransferMatrix)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSTransferMatrix</h2>
<h4>VAMOS's transformation matrix to map the measured coordinates at the focal plane back to the target</h4>
<!-- */
// --> END_HTML
//
// 
////////////////////////////////////////////////////////////////////////////////

void KVVAMOSTransferMatrix::init(){
		fNcoef = 0;
		fOrder = 0;

}
//________________________________________________________________

KVVAMOSTransferMatrix::KVVAMOSTransferMatrix() : KVBase("VAMOSTransferMatrix","Transfer Matrix of VAMOS"){
   // Default constructor
   init();
}
//________________________________________________________________

KVVAMOSTransferMatrix::KVVAMOSTransferMatrix(UChar_t order) : KVBase("VAMOSTransferMatrix","Transfer Matrix of VAMOS"){
   // Default constructor
   init();
   fOrder = order;
}
//________________________________________________________________

KVVAMOSTransferMatrix::KVVAMOSTransferMatrix (const KVVAMOSTransferMatrix& obj)  : KVBase("VAMOSTransferMatrix","Transfer Matrix of VAMOS"){
   	// Copy constructor
   	// This ctor is used to make a copy of an existing object 
   	init();
   	obj.Copy(*this);
}
//________________________________________________________________

KVVAMOSTransferMatrix::~KVVAMOSTransferMatrix(){
   	// Destructor
}
//________________________________________________________________

void KVVAMOSTransferMatrix::Copy (TObject& obj) const{
   	// This method copies the current state of 'this' KVVAMOSTransferMatrix 
   	// object into 'obj'.

    KVBase::Copy(obj);
   	//KVVAMOSTransferMatrix& CastedObj = (KVVAMOSTransferMatrix&)obj;
   	//    CastedObj.fToto = fToto;
   	// or
   	//    CastedObj.SetToto( GetToto() );

}
