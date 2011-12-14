/*
$Id: SelZtot.cpp,v 1.1 2006/11/03 14:18:56 cussol Exp $
$Revision: 1.1 $
$Date: 2006/11/03 14:18:56 $
*/

//Created by KVClassFactory on Mon Oct 23 17:58:28 2006
//Author: Daniel CUSSOL

#include "SelZtot.h"
#include "KVINDRA.h"
#include "KVSelector.h"

ClassImp(SelZtot)

////////////////////////////////////////////////////////////////////////////////
// Data selection on the total charge
////////////////////////////////////////////////////////////////////////////////

//_____________________________________
SelZtot::SelZtot()
{
   //Default constructor
}

//_____________________________________
SelZtot::~SelZtot()
{
   //Destructor
}

//_____________________________________
void SelZtot::Init(void)
{
// 
// Initialisations of the KVDataSelector
// This method is called before the InitAnalysis method of the KVSelector
//

// Enter your code here

 KV2Body *kv2b=gIndra->GetSystem()->GetKinematics();
 Int_t ztot=kv2b->GetNucleus(1)->GetZ()+kv2b->GetNucleus(2)->GetZ();
 
 zinf=TMath::Nint(0.8*ztot);
 
 cout << "Ztot = " << ztot << ", Zinf = " << zinf << endl;
 
 GetKVSelector()->AddGV("KVZtot","ztot");

}


//_____________________________________
Bool_t SelZtot::SelectCurrentEntry(void)
{
//
// returns Ok is the current entry has to be added to the TEventList
//
    Bool_t ok=kFALSE;
    
// enter you code here 
    ok=(GetKVSelector()->GetGV("ztot")->GetValue() >= zinf);

    return ok;
}

