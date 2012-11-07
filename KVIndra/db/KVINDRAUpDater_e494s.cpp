//Created by KVClassFactory on Thu Oct 11 18:23:43 2012
//Author: Dijon Aurore

#include "KVINDRAUpDater_e494s.h"
#include "KVINDRA.h"
#include "KVMultiDetArray.h"
#include "KVIDGridManager.h"
#include "KVRTGIDManager.h"
#include "KVDBParameterSet.h"
#include "KVINDRADetector.h"
#include "KVINDRADB.h"
#include "KVSilicon.h"

ClassImp(KVINDRAUpDater_e494s)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRAUpDater_e494s</h2>
<h4>Class for setting INDRA-VAMOS parameter for each run (e494s/e503 experiment)</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRAUpDater_e494s::KVINDRAUpDater_e494s()
{
   // Default constructor
}
//________________________________________________________________

KVINDRAUpDater_e494s::~KVINDRAUpDater_e494s()
{
   // Destructor
}
//________________________________________________________________

void KVINDRAUpDater_e494s::SetIDGrids(UInt_t run){
	// Set identification grid or identification function (KVTGID) 
	// for all ID telescopes for this run. 
    // The global ID grid manager gIDGridManager is used to set the
    // grids. The static function KVRTGIDManager::SetIDFuncInTelescopes
    //  is used to set the ID functions. First, any previously set grids/functions are removed.
    // Then all grids/functions for current run are set in the associated ID telescopes.

	Info("KVINDRAUpDater_e494s::SetIDGrids","Setting Identification Grids/Functions");
    TIter next_idt(gMultiDetArray->GetListOfIDTelescopes());

    KVIDTelescope  *idt    = NULL;
	KVRTGIDManager *rtgidm = NULL;

    while ((idt = (KVIDTelescope *) next_idt()))
    {
        idt->RemoveGrids();
		if(idt->InheritsFrom("KVRTGIDManager")){
 			rtgidm = (KVRTGIDManager *)idt->IsA()->DynamicCast(KVRTGIDManager::Class(),idt);
 			rtgidm->RemoveAllTGID();
		}

    }
    gIDGridManager->SetGridsInTelescopes(run);
	KVRTGIDManager::SetIDFuncInTelescopes(run);
}
//________________________________________________________________

void KVINDRAUpDater_e494s::SetParameters(UInt_t run){

	KVINDRAUpDater::SetParameters(run);
	KVDBRun *kvrun = gIndraDB->GetRun(run);
	if(!kvrun) return;
   	SetPedestalCorrections(kvrun);
}
//________________________________________________________________

void KVINDRAUpDater_e494s::SetPedestalCorrections(KVDBRun *run){
	KVRList *dp_list = run->GetLinks("DeltaPedestal");
	if(!dp_list) return;

	KVDBParameterSet *dp = NULL;
	TIter next_dp(dp_list);
	TString QDCnum;

	// Loop over INDRA detectors
	KVINDRADetector *det = NULL;
	TIter next_det(gMultiDetArray->GetListOfDetectors());
	while( (det = (KVINDRADetector *)next_det()) ){

		// Initializing each ACQ parameter pedestal correction for
		// all detectors
		KVACQParam *acqpar = NULL;
		TIter next_acqp(det->GetACQParamList());
		while( (acqpar = (KVACQParam *)next_acqp()) )
			acqpar->SetDeltaPedestal(0.);

		// Set the pedestal correction if a value exists for
		// the QDC associated to this detector
		next_dp.Reset();
		while( (dp = (KVDBParameterSet *)next_dp()) ){
			QDCnum = dp->GetName();
			QDCnum.Remove(0,3);
			if(det->GetNumeroCodeur() == QDCnum.Atoi()){
				acqpar = det->GetACQParam(dp->GetTitle());
				acqpar->SetDeltaPedestal(dp->GetParameter());
			}
		}
	}
}
//________________________________________________________________

void KVINDRAUpDater_e494s::SetPedestals(KVDBRun * kvrun)
{
//Set pedestals for this run   
// modifie MFR nov 2012 : add specific reading for etalons

    SetChIoSiPedestals(kvrun);
    SetSi75SiLiPedestals(kvrun);
    SetCsIPedestals(kvrun);

}
//________________________________________________________________

void KVINDRAUpDater_e494s::SetChIoSiPedestals(KVDBRun * kvrun)
{
//read Chio-Si pedestals
// modified MFR november 2012 : suppress etalons from this reading list

    if (!kvrun->GetKey("Pedestals"))
        return;
    if (!kvrun->GetKey("Pedestals")->GetLinks())
        return;
    if (!kvrun->GetKey("Pedestals")->GetLinks()->At(0))
        return;

    ifstream file_pied_chiosi;
    if (!KVBase::
            SearchAndOpenKVFile(kvrun->GetKey("Pedestals")->GetLinks()->At(0)->
                                GetName(), file_pied_chiosi, fDataSet.Data()))
    {
        Error("SetPedestals", "Problem opening file %s",
              kvrun->GetKey("Pedestals")->GetLinks()->At(0)->GetName());
        return;
    }
    cout << "--> Setting Pedestals" << endl;
    cout << "    ChIo/Si/Etalons: " << kvrun->GetKey("Pedestals")->
    GetLinks()->At(0)->GetName() << endl;

    //skip first 5 lines - header
    TString line;
    for (int i = 5; i; i--)
    {
        line.ReadLine(file_pied_chiosi);
    }

    int cou, mod, type, n_phys, n_gene;
    float ave_phys, sig_phys, ave_gene, sig_gene;

    while (file_pied_chiosi.good())
    {

        file_pied_chiosi >> cou >> mod >> type >> n_phys >> ave_phys >>
        sig_phys >> n_gene >> ave_gene >> sig_gene;

        KVDetector *det = gIndra->GetDetectorByType(cou, mod, type);
        if (det)
        {
            switch (type)
            {

            case ChIo_GG:

                det->SetPedestal("GG", ave_gene);
                break;

            case ChIo_PG:

                det->SetPedestal("PG", ave_gene);
                break;

            case Si_GG:

                det->SetPedestal("GG", ave_gene);
                break;

            case Si_PG:

                det->SetPedestal("PG", ave_gene);
                break;

/*            case SiLi_GG:

                det->SetPedestal("GG", ave_gene);
                break;

            case SiLi_PG:

                det->SetPedestal("PG", ave_gene);
                break;

            case Si75_GG:

                det->SetPedestal("GG", ave_gene);
                break;

            case Si75_PG:

                det->SetPedestal("PG", ave_gene);
                break;
*/
            default:

                break;
            }
        }
    }
    file_pied_chiosi.close();
}
//________________________________________________________________

void KVINDRAUpDater_e494s::SetSi75SiLiPedestals(KVDBRun * kvrun)
{
//read Etalons pedestals
// new method MFR november 2012 : reading etalons pedestals

    if (!kvrun->GetKey("Pedestals"))
        return;
    if (!kvrun->GetKey("Pedestals")->GetLinks())
        return;
    if (!kvrun->GetKey("Pedestals")->GetLinks()->At(2))
        return;

    ifstream file_pied_etalons;
    if (!KVBase::
            SearchAndOpenKVFile(kvrun->GetKey("Pedestals")->GetLinks()->At(2)->
                                GetName(), file_pied_etalons, fDataSet.Data()))
    {
        Error("SetPedestals", "Problem opening file %s",
              kvrun->GetKey("Pedestals")->GetLinks()->At(2)->GetName());
        return;
    }
    cout << "--> Setting Pedestals" << endl;
    cout << "    Etalons: " << kvrun->GetKey("Pedestals")->
    GetLinks()->At(2)->GetName() << endl;

    TString line;

    int cou, mod, type, n_phys, n_gene;
    float ave_phys, sig_phys, ave_gene, sig_gene;

    while (file_pied_etalons.good())  {
      line.ReadLine(file_pied_etalons);
//      cout<<"ligne lue :"<<line.Data()<<endl;
      
      if( (line.Sizeof() >1) && !(line.BeginsWith("#") ) )  {
	if( sscanf(line.Data(),"%d %d %d %d %f %f %d %f %f", &cou, &mod, &type,
	    &n_phys, &ave_phys, &sig_phys, &n_gene, &ave_gene, &sig_gene) !=9){
	  Warning("ReadPedestalEtalons","Bad Format in line :\n%s\nUnable to read",
		  line.Data());   
	} else  {	
	  KVDetector *det = gIndra->GetDetectorByType(cou, mod, type);

	  if (det) {
            switch (type) {

            case SiLi_GG:
                det->SetPedestal("GG", ave_gene);
                break;

            case SiLi_PG:
                det->SetPedestal("PG", ave_gene);
                break;

            case Si75_GG:
                det->SetPedestal("GG", ave_gene);
                break;

            case Si75_PG:
                det->SetPedestal("PG", ave_gene);
                break;

            default:

                break;
            }  //end switch
	  } // end if det
	} // end if line.Data else
      } // end line.Begins
    } // end while
	 
    file_pied_etalons.close();
}
