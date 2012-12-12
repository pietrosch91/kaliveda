//Created by KVClassFactory on Thu Oct 18 10:38:52 2012
//Author: John Frankland

#include "IRODSDataRepository.h"
#include "IRODS.h"

ClassImp(IRODSDataRepository)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>IRODSDataRepository</h2>
<h4>Remote data repository using IRODS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

IRODSDataRepository::IRODSDataRepository()
{
   // Default constructor
   fDMS = new IRODS();// set up interface to IRODS
}

IRODSDataRepository::~IRODSDataRepository()
{
   // Destructor
}
