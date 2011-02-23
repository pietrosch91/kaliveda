//Created by KVClassFactory on Fri Feb 11 10:36:24 2011
//Author: bonnet

#include "KVMassExcessTable.h"
#include "KVFileReader.h"
#include "TEnv.h"

ClassImp(KVMassExcessTable)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVMassExcessTable</h2>
<h4>manage mass excess data for nuclei</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVMassExcessTable::KVMassExcessTable():KVNuclDataTable("KVMassExcess")
{
   // Default constructor
	init();
}

//_____________________________________________
KVMassExcessTable::~KVMassExcessTable()
{
   // Destructor

}

//_____________________________________________
void KVMassExcessTable::init()
{
	
}

//_____________________________________________
void KVMassExcessTable::Initialize()
{
	
	TString cl_path;
	if( !SearchKVFile(gEnv->GetValue("MassExcess.DataFile",""),cl_path,"data") ){
		Error("Initialize","No file found for MassExcess");
		return;
	}	
	else 
		Info("Initialize","%s will be read",gEnv->GetValue("MassExcess.DataFile",""));
	
	kfilename.Form(gEnv->GetValue("MassExcess.DataFile",""));
		
	Int_t ntot=0;
	nucMap = new TMap(50,2);
	KVFileReader* fr = new KVFileReader();
	fr->OpenFileToRead(cl_path.Data());
	
	//Premier passage
	//Lecture du nombre de noyaux a enregistrer
	while (fr->IsOK()){
	
		fr->ReadLine(" ");
		if (fr->GetCurrentLine().IsNull()){ break; }
		else if (fr->GetNparRead()==0){ break; }
		else if (fr->GetReadPar(0).BeginsWith("//")){ 
		
			kcomments+=fr->GetReadPar(0);
			kcomments+="\n";
		
		}
		else {
			Int_t zz = fr->GetIntReadPar(0);
			Int_t aa = fr->GetIntReadPar(1);
			GiveIndexToNucleus(zz,aa,ntot);
			ntot+=1;
		}
	}
	
	if ( !fr->PreparForReadingAgain() ) return;
	
	Info("Initialize","Set up map for %d nuclei", ntot);
	CreateTable(ntot);
	
	KVNuclData* lf = 0;
	ntot=0;
	while (fr->IsOK()){
		fr->ReadLine(" ");
		if (fr->GetCurrentLine().IsNull()){ break; }
		else if (fr->GetNparRead()==0){ break; }
		else if (fr->GetReadPar(0).BeginsWith("//")){ }
		else {
			CreateElement(ntot);
			lf = GetCurrent();
			
			Double_t val = fr->GetDoubleReadPar(2);
			lf->SetValue(val);
			
			if (fr->GetNparRead()>3){
				Int_t meas = fr->GetReadPar(3).Atoi();
				lf->SetMeasured(meas);
			}
			else {
				if (ntot==0)
					InfoOnMeasured();
			}
			lf->SetMeasured(kFALSE);
			
			ntot+=1;
		}
	
	}
	
	Info("Initialize","table initialised correctly for %d/%d nuclei", ntot,GetNumberOfNuclei());
	fr->CloseFile();
	delete fr;

}

//_____________________________________________
KVMassExcess* KVMassExcessTable::GetMassExcess(Int_t zz, Int_t aa) const 
{

	return (KVMassExcess* )GetData(zz,aa);

}
