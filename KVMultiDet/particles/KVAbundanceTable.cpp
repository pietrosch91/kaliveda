//Created by KVClassFactory on Wed Feb 23 16:10:14 2011
//Author: bonnet

#include "KVAbundanceTable.h"
#include "KVFileReader.h"
#include "TEnv.h"

ClassImp(KVAbundanceTable)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVAbundanceTable</h2>
<h4>Store available values of the relative abundance</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVAbundanceTable::KVAbundanceTable():KVNuclDataTable("KVAbundance")
{
   // Default constructor
}

//_____________________________________________
KVAbundanceTable::~KVAbundanceTable()
{
   // Destructor
}

//_____________________________________________
void KVAbundanceTable::init()
{
	
}

//_____________________________________________
void KVAbundanceTable::Initialize()
{
	
	TString cl_path;
	if( !SearchKVFile(gEnv->GetValue("Abundance.DataFile",""),cl_path,"data") ){
		Error("Initialize","No file found for Abundance");
		return;
	}	
	else 
		Info("Initialize","%s will be read",gEnv->GetValue("Abundance.DataFile",""));
	
	kfilename.Form(gEnv->GetValue("Abundance.DataFile",""));
		
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
			lf->SetMeasured(kTRUE);
			
			ntot+=1;
		}
	
	}
	
	Info("Initialize","table initialised correctly for %d/%d nuclei", ntot,GetNumberOfNuclei());
	fr->CloseFile();
	delete fr;

}

//_____________________________________________
KVAbundance* KVAbundanceTable::GetAbundance(Int_t zz, Int_t aa) const 
{

	return (KVAbundance* )GetData(zz,aa);

}
