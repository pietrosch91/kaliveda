/*
$Id: SelMult.cpp,v 1.2 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Fri Oct 20 17:07:39 2006
//Author: Daniel CUSSOL

#include "SelMult.h"
#include "KVSelector.h"

ClassImp(SelMult)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>SelMult</h2>
<h4>Select events based on multiplicity</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//_____________________________________
SelMult::SelMult()
{
   //Default constructor
}

//_____________________________________
SelMult::~SelMult()
{
   //Destructor
}

//_____________________________________
void SelMult::Init(void)
{
// 
// Initialisations of the KVDataSelector
// This method is called before the InitAnalysis method of the KVSelector
//
//	GetKVSelector()->AddGV("KVMult","mult");
}


//_____________________________________
Bool_t SelMult::SelectCurrentEntry(void)
{
//
// returns Ok is the current entry has to be added to the TEventList
//
    Bool_t ok=kFALSE;
    
    ok = (GetKVSelector()->GetEvent()->GetMult("ok") >= 40);

    return ok;
}

