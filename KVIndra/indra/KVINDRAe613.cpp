//Created by KVClassFactory on Mon Jan 16 11:47:30 2012
//Author: bonnet

#include "KVINDRAe613.h"
#include "KVSilicon.h"
#include "KVGroup.h"

ClassImp(KVINDRAe613)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRAe613</h2>
<h4>INDRA multidetector array, configuration for e613 experiment</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRAe613::KVINDRAe613()
{
   // Default constructor
}

//________________________________________________________________

KVINDRAe613::KVINDRAe613 (const KVINDRAe613& obj) 
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVINDRAe613::~KVINDRAe613()
{
   // Destructor
}

//________________________________________________________________

void KVINDRAe613::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVINDRAe613::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVINDRA4::Copy(obj);
   //KVINDRAe613& CastedObj = (KVINDRAe613&)obj;
}


//________________________________________________________________

void KVINDRAe613::Build()
{
   //construct specific realisation of INDRA array for 5th campaign
   //start from INDRA4
  
   KVINDRA4::Build();
   SetName("INDRA");
   SetTitle("e613 experiment INDRA detector");
}

//________________________________________________________________

void KVINDRAe613::PrototypeTelescopes()
{
	//Redefinition de la methode de KVINDRA
	//pour la campagne e613
	//les Si75 de la couronne 10 et 12 n'était physiquement pas présent
	//on les retire de gIndra
	KVINDRA4::PrototypeTelescopes();
	KVINDRATelescope* kvt=0;
	
	kvt = (KVINDRATelescope* )fTelescopes->FindObjectByType("Etalons Ring10");
	delete fTelescopes->Remove(kvt);
		
   //Ring 10 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
   kvt->SetType("Etalons Ring10");
	kvt->SetDepth(1, 1.0);
   kvt->SetAzimuthalWidth(14.44 / 2.);
   fTelescopes->Add(kvt);
	
	kvt = (KVINDRATelescope* )fTelescopes->FindObjectByType("Etalons Ring12");
	delete fTelescopes->Remove(kvt);
		
   //Ring 12 etalon telescope
   kvt = new KVINDRATelescope;
   kvt->AddDetector((KVSiLi *) fDetectorTypes->FindObjectByLabel("SILI"));
   kvt->SetType("Etalons Ring12");
	kvt->SetDepth(1, 1.0);
   kvt->SetAzimuthalWidth(14.56 / 2.);
   fTelescopes->Add(kvt);

}
