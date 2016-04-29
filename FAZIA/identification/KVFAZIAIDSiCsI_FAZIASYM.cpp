//Created by KVClassFactory on Fri Apr 29 09:21:07 2016
//Author: bonnet,,,

#include "KVFAZIAIDSiCsI_FAZIASYM.h"

ClassImp(KVFAZIAIDSiCsI_FAZIASYM)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIAIDSiCsI_FAZIASYM</h2>
<h4>identification telescope for FAZIASYM experiment</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVFAZIAIDSiCsI_FAZIASYM::KVFAZIAIDSiCsI_FAZIASYM()
   : KVFAZIAIDSiCsI()
{
   // Default constructor
}

//____________________________________________________________________________//

KVFAZIAIDSiCsI_FAZIASYM::~KVFAZIAIDSiCsI_FAZIASYM()
{
   // Destructor
}

//____________________________________________________________________________//

Double_t KVFAZIAIDSiCsI_FAZIASYM::GetIDMapX(Option_t*)
{
   //X-coordinate for Si2-CsI identification map :
   // computed fast componment
   // of the charge signal of CsI detector
   return fCsI->GetQ3FastAmplitude();
}
