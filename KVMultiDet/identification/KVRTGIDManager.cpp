//Created by KVClassFactory on Thu Oct 11 11:51:14 2012
//Author: Dijon Aurore

#include "KVRTGIDManager.h"

ClassImp(KVRTGIDManager)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVRTGIDManager</h2>
<h4>Run-dependant KVTGIDManager</h4>
<!-- */
// --> END_HTML
//
//As KVTGIDManager, this class handles a set of Tassan-Got functional-based identifications (KVTGID objects) for use by KVIDTelescope.
//However, it allows to load the identification functions associated to a given run. To do that, you can remove all identification
//functions loaded in the KVIDTelescopes (which inherits from KVRTGIDManager) with the method RemoveAllTGID(). Then use the static 
//function SetIDFuncInTelescopes(run) to load the functions, which are valid for the given run, in the KVIDTelescope associated to
//these functions. See the following exemple:
//
//    TIter next_idt(gMultiDetArray->GetListOfIDTelescopes());
//
//    KVIDTelescope  *idt    = NULL;
//    KVRTGIDManager *rtgidm = NULL;
//
//    while ((idt = (KVIDTelescope *) next_idt()))
//    {
//		if(idt->InheritsFrom("KVRTGIDManager")){
// 			rtgidm = (KVRTGIDManager *)idt->IsA()->DynamicCast(KVRTGIDManager::Class(),idt);
// 			rtgidm->RemoveAllTGID();
//		}
//
//    }
//    KVRTGIDManager::SetIDFuncInTelescopes(run);
//
////////////////////////////////////////////////////////////////////////////////


KVList *KVRTGIDManager::fIDGlobalList = NULL;

KVRTGIDManager::KVRTGIDManager()
{
   // Default constructor
   // The list fIDList is not owner anymore.
   // Now the identification function belong to fIDGlobalList,
   // the list return by GetListOfIDFunctions() is not the owner
   // of identification functions anymore.

	fIDList.SetOwner(kFALSE);
	
}
//________________________________________________________________

KVRTGIDManager::~KVRTGIDManager()
{
   // Destructor
   // Remove the global list of identification functions if
   // it is empty.

	if(fIDGlobalList && !fIDGlobalList->GetEntries()) SafeDelete(fIDGlobalList);
}
//________________________________________________________________

void KVRTGIDManager::AddTGID(KVTGID * _tgid)
{
   // Add an identification object to the global list (fIDGlobalList).
   // The identification object is not added to the local list (fIDList) in this method anymore. To do that, see SetIDFunctionInTelescopes)
	if(!_tgid) return;
	if(!fIDGlobalList){
 	   	fIDGlobalList = new KVList;
		fIDGlobalList->SetCleanup();
	}
	fIDGlobalList->Add(_tgid);
}
//________________________________________________________________

void KVRTGIDManager::SetTGID(KVTGID * _tgid)

{
   // Add an identification object to the local list (fIDList).
	KVTGIDManager::AddTGID(_tgid);
}
//________________________________________________________________

void KVRTGIDManager::RemoveTGID(const Char_t * name)
{
   	//Remove the identification object with name from the local list.

   	KVTGID* tgid=0;
   	// Remove the function from the current list
   	if( !(tgid = (KVTGID*)fIDList.FindObject(tgid)) ) return;
    fIDList.Remove(tgid);
    //recalculate fID_max if needed
    if( tgid->GetIDmax() == fID_max ){
        fID_max = 0.; TIter n(&fIDList); KVTGID* t;
        while( (t = (KVTGID*)n()) ) fID_max = TMath::Max(fID_max, t->GetIDmax());
    }
}
//________________________________________________________________

void KVRTGIDManager::DeleteTGID(const Char_t * name)
{
	if(!fIDGlobalList) return;

   	//Delete the identification object with name from the global list.
   	KVTGID* tgid=0;

   	// Delete the function from the global list
   	if( !(tgid = (KVTGID*)fIDGlobalList->FindObject(name)) ) return;
   	fIDGlobalList->Remove(tgid);
	delete tgid;
}
//________________________________________________________________

void KVRTGIDManager::RemoveAllTGID(){
   //Remove all identification objects of the local list.
   fIDList.Clear(); fID_max = 0.;
}
//________________________________________________________________

void KVRTGIDManager::Clear(){
   //Delete the global list of identification objects. Since this
   //list is owner then all listed identification objects are deleted.
  
	SafeDelete(fIDGlobalList);
}
//________________________________________________________________

void KVRTGIDManager::SetIDFuncInTelescopes(UInt_t run){
	// For each identification function of the global list which is valid for this run, we add it in the associated telescope which
	// have to inherits from KVTGIDManager and from a KVIDTelescope.

	TIter next(fIDGlobalList);
	KVTGID *tgid = NULL;

	while( (tgid = (KVTGID *)next())){

		if(!tgid->IsValidForRun(run)) continue;

		// this list have to be deleted after use
		TCollection *lidtel = tgid->GetIDTelescopes();	
		TIter nextidt(lidtel);
		TObject        *idt   = NULL;
		KVRTGIDManager *tgidm = NULL;

		while( (idt = nextidt()) ){
			if(!idt->InheritsFrom("KVRTGIDManager")){
				Error("KVRTGIDManager::SetIDFuncInTelescopes","The IDtelescope %s does not inherit from KVRTGIDManager",idt->GetName());
				continue;
			}
			tgidm = (KVRTGIDManager* )idt->IsA()->DynamicCast(KVRTGIDManager::Class(),idt);
			tgidm->SetTGID(tgid);
		}
		delete lidtel;
	}
}
