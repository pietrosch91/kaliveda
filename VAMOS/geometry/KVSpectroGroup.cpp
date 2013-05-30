//Created by KVClassFactory on Wed May 29 14:59:50 2013
//Author: Guilain ADEMARD

#include "KVSpectroGroup.h"
#include "KVSpectroDetector.h"

ClassImp(KVSpectroGroup)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSpectroGroup</h2>
<h4>Group of spectrometer detectors on a same trajectory</h4>
<!-- */
//
//The notion of a "group of detectors" in a spectrometer is used to associate
//detectors (with different geometries) through which nuclei punshing
//through the spectrometer may pass. This is essential for reconstruction 
//of the nucleus reconstruction and usefull for handling dentification
//telescopes used for the Z-identification of this nucleus. 
//
//One detector can be in several groups, for example if its surface covers
//the surface of several detectors placed behind.  
//
//WARNING: the notion of group here is very different from the one of the
//         class KVGroup used in a KVMultiDetArray object.

// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSpectroGroup::KVSpectroGroup(){
   	// Default constructor
   	init();
}
//________________________________________________________________

KVSpectroGroup::KVSpectroGroup(const KVSpectroGroup& obj) : KVBase(){
   	// Copy constructor

	init();
   	obj.Copy(*this);
}
//________________________________________________________________

KVSpectroGroup::KVSpectroGroup(const Char_t* name, const Char_t* title) : KVBase(name, title){
   	init();
}
//________________________________________________________________

KVSpectroGroup::~KVSpectroGroup(){
   	// Destructor
   	SafeDelete( fDetectors );
}
//________________________________________________________________

void KVSpectroGroup::init(){
	//Default initialisation

	fDetectors = new KVList( kFALSE );
	fDetectors->SetCleanup();
}
//________________________________________________________________

void KVSpectroGroup::Copy(TObject& obj) const{
   	// This method copies the current state of 'this' object into 'obj'

   	KVBase::Copy(obj);
   	KVSpectroGroup& CastedObj = (KVSpectroGroup&)obj;
   	CastedObj.SetDetectors( GetDetectors() );
}
//________________________________________________________________

void KVSpectroGroup::Add( KVSpectroDetector *det ){
	//Add a detector to the current group.

	if( det ){
		fDetectors->Add( det );
		det->AddGroup( this );
	}
	else Warning("Add","Attempt to add undefined detector to group");
}
//________________________________________________________________
		
const Char_t* KVSpectroGroup::GetTitle() const{
	// Returns the title which is the list of names of the detectors
	
	KVSpectroGroup *sg = ((KVSpectroGroup *) this);
	sg->fTitle = "";
	TIter next( fDetectors );
	TObject *obj = NULL;
	while( (obj = next()) ){
		sg->fTitle.Append( obj->GetName() );
		sg->fTitle.Append(" ");
	}
	sg->fTitle.Remove(TString::kBoth,' ');
	return KVBase::GetTitle();
}
//________________________________________________________________

void KVSpectroGroup::SetDetectors( KVList *list ){
	// Set list of detectors by copying list
	
	list->Copy( *fDetectors );
}
//________________________________________________________________

void KVSpectroGroup::SetNumber(UInt_t num){
    // Setting number for group also sets name to "SpectroGroup_n"
    SetName( Form("SpectroGroup_%ud", num) );
    KVBase::SetNumber(num);
};

