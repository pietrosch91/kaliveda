//Created by KVClassFactory on Mon Oct 24 14:38:16 2011
//Author: bonnet

#include "KVINDRADB_e613.h"
#include "KVINDRA.h"
#include "KVFileReader.h"
#include "TObjArray.h"
#include "KVDBParameterSet.h"

ClassImp(KVINDRADB_e613)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRADB_e613</h2>
<h4>Child class for e613 experiment</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRADB_e613::KVINDRADB_e613()
{
   // Default constructor
}

//____________________________________________________________________________
KVINDRADB_e613::KVINDRADB_e613(const Char_t * name):KVINDRADB(name)
{
	Info("KVINDRADB_e613","Hi coquine, tu es sur la manip e613 ...");
}

//____________________________________________________________________________
KVINDRADB_e613::~KVINDRADB_e613()
{
   // Destructor
}

//____________________________________________________________________________
void KVINDRADB_e613::ReadGainList()
{

   // Read the file listing any detectors whose gain value changes during exeriment
	  //need description of INDRA geometry
   if (!gIndra) {
      KVMultiDetArray::MakeMultiDetector(fDataSet.Data());
   }
   //gIndra exists, but has it been built ?
   if (!gIndra->IsBuilt())
      gIndra->Build();

	KVFileReader flist;
	TString fp;
	if (!KVBase::SearchKVFile(GetCalibFileName("Gains"), fp, fDataSet.Data())){
		Error("ReadGainList","Fichier %s, inconnu au bataillon",GetCalibFileName("Gains"));
		return;
	}
	
	if (!flist.OpenFileToRead(fp.Data())){
		//Error("ReadGainList","Error opening file named %s",fp.Data());
		return;
	}
	Info("ReadGainList()", "Reading gains ...");
   //Add table for gains
   fGains = AddTable("Gains", "Gains of detectors during runs");
	
	TObjArray* toks = 0;
	while (flist.IsOK()){
	
		flist.ReadLine(".");
		if (! flist.GetCurrentLine().IsNull() ){
			toks = flist.GetReadPar(0).Tokenize("_");
			Int_t nt = toks->GetEntries();
			Int_t ring=-1;
			Int_t mod=-1;
			KVList* sl = 0;
			KVList* ssl = 0;
			TString det_type="";
			if (nt<=1){
				Warning("ReadGainList","format non gere");
			}
			else {
				//format : Gain_[det_type]_R[RingNumber].dat
				//exemple  : Gain_SI_R07.dat
				det_type = ((TObjString* )toks->At(1))->GetString();
				//on recupere les detecteurs par type
				sl = (KVList* )gIndra->GetListOfDetectors()->GetSubListWithType(det_type.Data());
				if (nt==2){
					ring=0;
				}	
				else if (nt==3){
					sscanf(((TObjString* )toks->At(2))->GetString().Data(),"R%d",&ring);
				}
				else {
					Warning("ReadGainList","format non gere");
				}
				if (ring==0){ }
				else {
					ssl = (KVList* )sl->GetSubListWithMethod(Form("%d",ring),"GetRingNumber");
					delete sl;
					sl = ssl;
				}
			}
			delete toks;
			
			if (sl){
				KVDBParameterSet* par = 0;
				TIter it(sl);
				TObject* obj = 0;
				KVNumberList nl;
				KVFileReader ffile;
				if ( KVBase::SearchKVFile(flist.GetCurrentLine().Data(), fp, fDataSet.Data()) ){
					ffile.OpenFileToRead(fp.Data());
					//Info("ReadGainList","Lecture de %s",fp.Data());
					while ( ffile.IsOK() ){
						ffile.ReadLine(":");
						if (! ffile.GetCurrentLine().IsNull() ){
							
							toks = ffile.GetReadPar(0).Tokenize(".");
							//liste des runs ...
							nl.SetList(((TObjString* )toks->At(1))->GetString());
							// ... associee a la valeur de gain
							Double_t gain = ffile.GetDoubleReadPar(1);
							
							printf("%s Ring %d -> Runs=%s Gain=%1.3lf\n",det_type.Data(),ring,nl.AsString(),gain);
							
							it.Reset();
							while ( (obj = it.Next()) ){
								par = new KVDBParameterSet(obj->GetName(), "Gains", 1);
								par->SetParameter(gain);
								fGains->AddRecord(par);
								LinkRecordToRunRange(par,nl);
							}
						
							delete toks;
						}	
					}
				}
				ffile.CloseFile();
				delete sl;
			}
		}
	}
	
	flist.CloseFile();

}

//____________________________________________________________________________
