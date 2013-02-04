//Created by KVClassFactory on Wed Jul 25 10:14:20 2012
//Author: Guilain ADEMARD

#include "KVHarpeeIC.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"

ClassImp(KVHarpeeIC)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVHarpeeIC</h2>
<h4>Ionisiation chamber of Harpee, used at the focal plan of VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVHarpeeIC::init(){
	// Initialise non-persistent pointers

}
//________________________________________________________________

KVHarpeeIC::KVHarpeeIC()
{
   	// Default constructor
   	// This constructor must exist in order for cloning of detectors
   	// to work (as used in KVTelescope::AddDetector).
   	// Do not replace this constructor by giving a default value for
   	// the first argument of KVHarpeeIC(Float_t, Float_t).
   	init();
   	SetType("CHI");
	SetName(GetArrayName());
}
//________________________________________________________________

KVHarpeeIC::KVHarpeeIC(UInt_t number, Float_t pressure, Float_t temp, Float_t thick) : KVVAMOSDetector(number, "C4H10"){
	// Make the ionisation chamber of Harpee composed by:
	// - one 1.5 mylar window;
	// - 3 volumes of C4H10 with thicknesses (2*dz): 2.068 cm, thick, 1.278 cm.
	//   By default thick=10.457 cm;
	//   Only the 2nd gaz volume is "active".
	//
	// The width (2*dx) of this detector is ??? and the height (2*dy) is ???.
	// The ionisation chamber is closed with the silicon wall in Harpee.
	//
	// Input: pressure - pressure of the gaz in mbar
	//        temp     - temperature of the gaz in degrees celsius.
	//                   Default temperature = 19°C 
	//        thick    - the thickness in cm of the 2nd volume of gaz.
	Warning("KVHarpeeIC","Check if the width, the height and the thickness are correct in this constructor");

	init();
	SetType("CHI");
	SetName(GetArrayName());

	// number of absorber
	Int_t Nabs = 4;

	// material of each absorber
	const Char_t* mat[] = {"Myl","C4H10","C4H10","C4H10"};

	// thickness of each absorber
	Float_t th[] = {1.5*KVUnits::um,    2.068*KVUnits::cm
	   	, thick*KVUnits::cm, 1.278*KVUnits::cm};

	// active absorber flag
	Bool_t  active[] = { kFALSE, kFALSE, kTRUE, kFALSE};

	// width and height of the detector. TO BE VERIFIED
	Float_t w = 40*KVUnits::cm;
	Float_t h = 12*KVUnits::cm;

	// total thickness of the detector
	Float_t wtot = 0;
	for(Int_t i=0; i<Nabs; i++) wtot+= th[i];

	// pressure and temperature of the detector
	SetPressure(pressure*KVUnits::mbar);
	SetTemperature(temp);

	// Adding the absorbers
	TGeoShape *shape = NULL;
	for(Int_t i=0; i<Nabs; i++){

		Double_t dx = w/2;
		Double_t dy = h/2;
		Double_t dz = th[i]/2;

		fTotThick+=th[i];

		// box shape of the absorber
		shape  = new TGeoBBox(dx,dy,dz);

		// build and position absorber in mother volume.
		// Reference is the center of absorber
		Double_t ztrans = dz-wtot/2;
		for(Int_t j=0; j<Nabs-1;j++) ztrans+= (j<i)*th[j];
		TGeoTranslation* tr = new TGeoTranslation(0.,0.,ztrans);
		AddAbsorber(mat[i],shape,tr,active[i]);
	}


}

//________________________________________________________________

KVHarpeeIC::KVHarpeeIC (const KVHarpeeIC& obj)  : KVVAMOSDetector()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVHarpeeIC::~KVHarpeeIC()
{
   // Destructor
}

//________________________________________________________________

void KVHarpeeIC::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVHarpeeIC::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVVAMOSDetector::Copy(obj);
   //KVHarpeeIC& CastedObj = (KVHarpeeIC&)obj;
}
//________________________________________________________________

const Char_t* KVHarpeeIC::GetArrayName(){
	// Name of the detector given in the form
	// ECHI
	//
	// The root of the name is 'E'+ the detector type.
	
	fFName = Form("%s",GetType());
	return fFName.Data();
}
//________________________________________________________________

void KVHarpeeIC::SetACQParams(){
	// Setup acquisition parameters of this ionisation chamber.
	// ACQ parameters with type 'E':
	// E[detector type]_[A,B,C]_[detector number]

	TString name;
	Char_t idx[]={'A','B','C'};

	for(Int_t num = 1; num<=7; num++){
		for(Int_t i = 0; i<3; i++){
			KVACQParam *par = new KVACQParam;
			name.Form("E%s_%c_%d",GetType(),idx[i],num);
			par->SetName(name);
			par->SetType("E");
			par->SetNumber( 6000 + (i+1)*10 + num );
			AddACQParam(par);
		}
	}
}
