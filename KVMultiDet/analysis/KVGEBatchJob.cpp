//Created by KVClassFactory on Wed Apr  3 14:15:55 2013
//Author: John Frankland,,,

#include "KVGEBatchJob.h"

ClassImp(KVGEBatchJob)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGEBatchJob</h2>
<h4>Job handled by Grid Engine batch system</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVGEBatchJob::KVGEBatchJob()
{
   // Default constructor
}

//________________________________________________________________

KVGEBatchJob::KVGEBatchJob(const KVGEBatchJob& obj) : KVBatchJob()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

}

KVGEBatchJob::~KVGEBatchJob()
{
   // Destructor
}

