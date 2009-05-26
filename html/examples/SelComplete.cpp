/*
$Id: SelComplete.cpp,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Tue Oct 24 16:53:38 2006
//Author: Daniel CUSSOL

#include "SelComplete.h"

ClassImp(SelComplete)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>SelComplete</h2>
<h4>Select complete events</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

#include "KVINDRA.h"
#include "KVSelector.h"

//_____________________________________
SelComplete::SelComplete()
{
   //Default constructor
}

//_____________________________________
SelComplete::~SelComplete()
{
   //Destructor
}

//_____________________________________
void SelComplete::Init(void)
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
 
 TVector3 ptot=kv2b->GetNucleus(1)->GetMomentum();
 pzinf=TMath::Nint(0.8*ptot.Z());
 
 cout << "Ptot = " << ptot .Z()<< ", Pzinf = " << pzinf << endl;
 
 GetKVSelector()->AddGV("KVPtot","ptot");
 GetKVSelector()->AddGV("KVZtot","ztot");

}


//_____________________________________
Bool_t SelComplete::SelectCurrentEntry(void)
{
//
// returns Ok is the current entry has to be added to the TEventList
//
    Bool_t ok=kFALSE;
    
// enter you code here 
    ok=(GetKVSelector()->GetGV("ptot")->GetValue() >= pzinf);
    ok=(ok && (GetKVSelector()->GetGV("ztot")->GetValue() >= zinf));

    return ok;
}

