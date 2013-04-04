//Created by KVClassFactory on Wed Apr  3 14:15:11 2013
//Author: John Frankland,,,

#include "KVBatchJob.h"
#include "Riostream.h"
using namespace std;

ClassImp(KVBatchJob)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVBatchJob</h2>
<h4>Job handled by batch system</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVBatchJob::KVBatchJob()
{
   // Default constructor
    cpu_used=0;
    mem_used="-";
    cpu_max=0;
}


KVBatchJob::~KVBatchJob()
{
   // Destructor
}

void KVBatchJob::ls(Option_t *) const
{
    cout << GetJobID() << " " << GetName() << " " << GetStatus() << " " << GetSubmitted().AsSQLString();
    if(!strcmp(GetStatus(),"r")) cout << " " << GetCPUusage() << " " << GetMemUsed();
    cout << endl;
}

