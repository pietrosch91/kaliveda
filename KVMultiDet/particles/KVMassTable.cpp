//Created by KVClassFactory on Mon Jul 27 11:39:45 2009
//Author: John Frankland,,,

#include "KVMassTable.h"

ClassImp(KVMassTable)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVMassTable</h2>
<h4>ABC for nuclear mass tables</h4>
Specific implementations must define the methods:
<ul>
	<li>Initialize() - called once to initialize mass table, for example by
	reading in values from a file</li>
	<li>IsKnown(Int_t Z, Int_t A) - returns kTRUE for nuclei which appear in
	mass table, kFALSE for others</li>
	<li>GetMassExcess(Int_t Z, Int_t A) - returns nuclear mass excess in keV
</ul>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVMassTable::KVMassTable()
{
   // Default constructor
}

KVMassTable::~KVMassTable()
{
   // Destructor
}

