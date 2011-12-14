/*
$Id: SelZVtot.cpp,v 1.1 2006/11/03 14:18:56 cussol Exp $
$Revision: 1.1 $
$Date: 2006/11/03 14:18:56 $
*/

//Created by KVClassFactory on Mon Oct 30 09:52:31 2006
//Author: Daniel CUSSOL

#include "SelZVtot.h"

ClassImp(SelZVtot)

////////////////////////////////////////////////////////////////////////////////
// Selection ZVtot >= 80% ZVInit
////////////////////////////////////////////////////////////////////////////////

#include "KVINDRA.h"
#include "KVSelector.h"
#include "KVNucleus.h"

//_____________________________________
SelZVtot::SelZVtot()
{
   //Default constructor
   nsel=0;
}

//_____________________________________
SelZVtot::~SelZVtot()
{
   //Destructor
}

//_____________________________________
void SelZVtot::Init(void)
{
// 
// Initialisations of the KVDataSelector
// This method is called when a TEventList is build
// The pointer to the current KVSelector which is using this KVDataSelector 
// can be obtained with the GetKVSelectorMethod(). This may be useful when 
// a KVVarGlob is used for the dataselection. It can be added to the list
// of global variables of the KVSelector by a call like:
//
    GetKVSelector()->AddGV("KVZVtot","zvtot");

    KV2Body *kv2b=gIndra->GetSystem()->GetKinematics();
    KVNucleus *proj=kv2b->GetNucleus(1);
    zvinf=0.8*proj->GetZ()*proj->GetVpar();
    
    cout << "ZVinit = " << proj->GetZ()*proj->GetVpar() << ", ZVinf = " << zvinf << endl;
    
    
}


//_____________________________________
Bool_t SelZVtot::SelectCurrentEntry(void)
{
//
// returns Ok is the current entry has to be added to the TEventList
//
    Bool_t ok=kFALSE;
    
// enter you code here 
// The pointer to the current event can be obtained by:
//
//  KVINDRAReconEvent *evt=GetKVSelector()->GetEvent();
//
// When a global variable is used, its value can be obtained by:
//
    Double_t val=GetKVSelector()->GetGV("zvtot")->GetValue();
    
    ok=(val >= zvinf);
     
    return ok;
}

