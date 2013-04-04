//Created by KVClassFactory on Wed Apr  3 14:15:55 2013
//Author: John Frankland,,,

#include "KVGEBatchJob.h"
#include "TSystem.h"

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
   cpu_scaling_factor=9.2;
}

KVGEBatchJob::~KVGEBatchJob()
{
    // Destructor
}

void KVGEBatchJob::DeleteJob()
{
    // delete this job:
    //    qdel [jobid]
    gSystem->Exec(Form("qdel %d", GetJobID()));
}

