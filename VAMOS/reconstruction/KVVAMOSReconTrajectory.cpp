//Created by KVClassFactory on Thu Mar 28 14:12:49 2013
//Author: Guilain ADEMARD

#include "KVVAMOSReconTrajectory.h"

ClassImp(KVVAMOSReconTrajectory)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSReconTrajectory</h2>
<h4>Handle data used to reconstruct the trajectory of a nucleus in VAMOS</h4>
<img src="images/kvvamosrecontrajectory_refframes.png"><br>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVVAMOSReconTrajectory::init(){
	// initializes data members
   	pointFP[0] = pointFP[1] = -666;
	dirFP.SetXYZ ( 0, 0, -1);
	dirLab.SetXYZ( 0, 0, -1);
	Brho = path = -666;
}
//________________________________________________________________

KVVAMOSReconTrajectory::KVVAMOSReconTrajectory(){
   	// Default constructor
	init();
}
//________________________________________________________________

KVVAMOSReconTrajectory::KVVAMOSReconTrajectory (const KVVAMOSReconTrajectory& obj)  : KVBase(){
   	// Copy constructor.
   	// This constructor is used to make a copy of an existing KVVAMOSReconTrajectory 
   	// object.
	init();
   	obj.Copy(*this);
}
//________________________________________________________________

KVVAMOSReconTrajectory::~KVVAMOSReconTrajectory(){
   	// Destructor
}
//________________________________________________________________

void KVVAMOSReconTrajectory::Copy (TObject& obj) const{
   	// This method copies the current state of 'this' object into 'obj'
   	KVBase::Copy(obj);
   	KVVAMOSReconTrajectory& CastedObj = (KVVAMOSReconTrajectory&)obj;
   	for(Int_t i=0; i<3; i++){
   	  	if( i<2 ) CastedObj.pointFP[i] = pointFP[i];
	  	CastedObj.dirFP  = dirFP;
	  	CastedObj.dirLab = dirLab;
   	}
	CastedObj.Brho = Brho;
	CastedObj.path = path;
}
//________________________________________________________________

void KVVAMOSReconTrajectory::Reset(){
	//Reset to initial values
	init();
	ResetBit( kFPisOK  );
	ResetBit( kLabIsOK );
	ResetBit( kFPtoLabAttempted );
	ResetBit( kLabToFPattempted );
}
