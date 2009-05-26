/*
$Id: SelPtot.cpp,v 1.1 2006/11/03 14:18:56 cussol Exp $
$Revision: 1.1 $
$Date: 2006/11/03 14:18:56 $
*/

//Created by KVClassFactory on Tue Oct 24 13:51:17 2006
//Author: Daniel CUSSOL

#include "SelPtot.h"
#include "KVINDRA.h"
#include "KVSelector.h"

ClassImp(SelPtot)

////////////////////////////////////////////////////////////////////////////////
// Selection on the total parallel momentum
////////////////////////////////////////////////////////////////////////////////

//_____________________________________
SelPtot::SelPtot()
{
   //Default constructor
}

//_____________________________________
SelPtot::~SelPtot()
{
   //Destructor
}

//_____________________________________
void SelPtot::Init(void)
{
// 
// Initialisations of the KVDataSelector
// This method is called before the InitAnalysis method of the KVSelector
//

// Enter your code here
 KV2Body *kv2b=GetKVSelector()->GetCurrentRun()->GetSystem()->GetKinematics();
 TVector3
 ptot=kv2b->GetNucleus(1)->GetMomentum();
 
 pzinf=0.8*ptot.Z();
 
 cout << "Ptot = " << ptot .Z()<< ", Pzinf = " << pzinf << endl;
 
 GetKVSelector()->AddGV("KVPtot","ptot");

}


//_____________________________________
Bool_t SelPtot::SelectCurrentEntry(void)
{
//
// returns Ok is the current entry has to be added to the TEventList
//
    Bool_t ok=kFALSE;
    
// enter you code here 
    ok=(GetKVSelector()->GetGV("ptot")->GetValue() >= pzinf);

    return ok;
}

