//Created by KVClassFactory on Wed Nov 16 14:10:43 2011
//Author: bonnet

#include "KVINDRAUpDater_e613.h"
#include "KVDBParameterSet.h"
#include "KVINDRA.h"
#include "KVDetector.h"
#include "KVACQParam.h"

ClassImp(KVINDRAUpDater_e613)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRAUpDater_e613</h2>
<h4>Sets run parameters for INDRA_e613 dataset</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRAUpDater_e613::KVINDRAUpDater_e613()
{
   // Default constructor
}

KVINDRAUpDater_e613::~KVINDRAUpDater_e613()
{
   // Destructor
}

//_______________________________________________________________//

void KVINDRAUpDater_e613::SetGains(KVDBRun * kvrun)
{
    //Set gains used during this run
    //Read gains in database just print detectors for which gains have changed
	 //
	 
	 KVRList *gain_list = kvrun->GetLinks("Gains");
    if (!gain_list)
    {
        return;
		  Warning("SetGains","No gains defined for this run in database");
    }
    Int_t ndets = gain_list->GetSize();
    Info("SetGains","Loop on %d detectors : ...",ndets);
    
	 Int_t nchange = 0;
	 KVDetector* kvd=0;
	 Float_t oldgain;
	 TString list;
	 for (Int_t i = 0; i < ndets; i++)
    {
        KVDBParameterSet *dbps = (KVDBParameterSet *) gain_list->At(i);
        kvd = gIndra->GetDetector(dbps->GetName());
        oldgain = kvd->GetGain();
		  if ( oldgain != Float_t(dbps->GetParameter(0)) ){
		  	  kvd->SetGain(Float_t(dbps->GetParameter(0)));
           //cout << "            " << kvd->GetName() << " set gain from " << oldgain << " to G=" << kvd->GetGain() << endl;
		  	  list += kvd->GetName();
			  list += ",";
			  nchange += 1;
		  }
	}
	if (nchange==0)
		Info("SetGains","Gains of the %d detectors are the same than the run before ",ndets);
	else 
		Info("SetGains","Gains have been changed for %d detectors (total = %d)",nchange,ndets);

}

//_______________________________________________________________//

void KVINDRAUpDater_e613::SetPedestals(KVDBRun * kvrun)
{
    //Set pedestals for this run

	KVRList *ped_list = kvrun->GetLinks("Pedestals");
	if (!ped_list)
	{
		return;
		Warning("SetPedestals","No pedestals defined for this run in database");
	}
	Int_t ndets = ped_list->GetSize();
	Info("SetPedestals","Loop on %d acquisition parameter : ...",ndets);

	Int_t nchange = 0;
	KVACQParam* acq=0;
	KVDBParameterSet *dbps=0;
	Float_t oldped;
	TString list;
	for (Int_t i = 0; i < ndets; i++)
	{
		dbps = (KVDBParameterSet *) ped_list->At(i);
		acq = gIndra->GetACQParam(dbps->GetName());
	
		oldped = acq->GetPedestal();
		if ( oldped != Float_t(dbps->GetParameter(0)) ){
			acq->SetPedestal(Float_t(dbps->GetParameter(0)));
		   
			list += acq->GetName();
		   list += ",";
		   nchange += 1;
		}
	}
	if (nchange==0)
		Info("SetGains","Pedestals of the %d acquisition parameters are the same than the run before ",ndets);
	else 
		Info("SetGains","Pedestals have been changed for %d acquisition parameters (total = %d)",nchange,ndets);

}
