//Created by KVClassFactory on Sat Jul 18 16:54:29 2009
//Author: John Frankland

#include "SRBDataRepository.h"
#include "SRB.h"

ClassImp(SRBDataRepository)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>SRBDataRepository</h2>
<h4>Remote data repository using SRB</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

SRBDataRepository::SRBDataRepository()
{
   // Default constructor
   fDMS = new SRB();//set up interface to SRB
}

SRBDataRepository::~SRBDataRepository()
{
   // Destructor
}
