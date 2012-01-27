//Created by KVClassFactory on Mon Oct 24 14:38:16 2011
//Author: bonnet

#include "KVINDRADB_e613.h"
#include "KVINDRA.h"
#include "KVFileReader.h"
#include "TObjArray.h"
#include "KVDBParameterSet.h"
#include "KVDBChIoPressures.h"

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
void KVINDRADB_e613::ReadChIoPressures()
{
   //Read ChIo pressures for different run ranges and enter into database.
   //Format of file is:
   //
   //# some comments
   //#which start with '#'
   //RunRange	6001 6018
   //2_3	50.0
   //4_5	50.0
   //6_7	50.0
   //8_12	30.0
   //13_17	30.0
   //
   //Pressures (of C3F8) are given in mbar).

   ifstream fin;
   if (!OpenCalibFile("Pressures", fin)) {
      Error("ReadChIoPressures()", "Could not open file %s",
            GetCalibFileName("Pressures"));
      return;
   }
   Info("ReadChIoPressures()", "Reading ChIo pressures parameters...");

   TString sline;
   
	Bool_t prev_rr = kFALSE;     //was the previous line a run range indication ?
   Bool_t read_pressure = kFALSE; // have we read any pressures recently ?
	KVNumberList nl;
   KVDBChIoPressures *parset=0;
   TList *par_list = new TList();
	TObjArray* toks = 0;
         //any ChIo not in list is assumed absent (pressure = 0)
	
	Float_t pressure[5] = { 0, 0, 0, 0, 0 };

   while (fin.good()) {         // parcours du fichier

      sline.ReadLine(fin);
      if (sline.BeginsWith("#")){
		
		}
		else if (sline.BeginsWith("RunRange")) {    // run range found
         if (!prev_rr) {        // New set of run ranges to read

            //have we just finished reading some pressures ?
            if (read_pressure){
               parset = new KVDBChIoPressures(pressure);
               GetTable("ChIo Pressures")->AddRecord(parset);
               par_list->Add(parset);
               LinkListToRunRange(par_list, nl);
               par_list->Clear();
               for(register int zz=0;zz<5;zz++) pressure[zz]=0.;
               read_pressure=kFALSE;
            }
         }
         toks = sline.Tokenize("\t");
			
			if (toks->GetEntries()!=2){
				Error("ReadChIoPressures","Pb de format, il faut RunRange\tRun1-Run2 ... ");
				return;
			}
			
			prev_rr = kTRUE;
         nl.SetList(((TObjString* )toks->At(1))->GetString().Data());
			delete toks;
		}                         // Run Range found
      else if (fin.eof()) {          //fin du fichier
      	//have we just finished reading some pressures ?
      	if (read_pressure){
				parset = new KVDBChIoPressures(pressure);
				GetTable("ChIo Pressures")->AddRecord(parset);
				par_list->Add(parset);
				LinkListToRunRange(par_list, nl);
				par_list->Clear();
				for(register int zz=0;zz<5;zz++) pressure[zz]=0.;
				read_pressure=kFALSE;
			}
      }
      else {
			prev_rr = kFALSE;

			toks = sline.Tokenize("\t");
			if (toks->GetEntries()!=2){
				Error("ReadChIoPressures","Pb de format, il faut numero de la chio (ex 2_3)\tpression");
				return;
			}
			
			TString chio = ((TObjString*)toks->At(0))->String();
         TString press = ((TObjString*)toks->At(1))->String();
         
			printf("%s %lf\n",chio.Data(),press.Atof());
			delete toks;

         read_pressure=kTRUE;
			
			if( chio == "2_3" ) 			pressure[0] = press.Atof();
         else if( chio == "4_5" ) 	pressure[1] = press.Atof();
         else if( chio == "6_7" ) 	pressure[2] = press.Atof();
         else if( chio == "8_12" ) 	pressure[3] = press.Atof();
         else if( chio == "13_17" ) pressure[4] = press.Atof();
         else {
				printf("#%s# ne correspond a rien\n",chio.Data());
				read_pressure=kFALSE;
			}
      }                         //line with ChIo pressure data
   }                            //parcours du fichier
   delete par_list;
   fin.close();
}

//____________________________________________________________________________
void KVINDRADB_e613::ReadGainList()
{

   // Read the file listing any detectors whose gain value changes during experiment
	// need description of INDRA geometry
	// information are in  [dataset name].INDRADB.Gains:    ...
	//
	
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
void KVINDRADB_e613::ReadPedestalList()
{

   //Read the names of pedestal files to use for each run range, found
   //in file with name defined by the environment variable:
   //   [dataset name].INDRADB.Pedestals:    ...
	
	KVFileReader flist;
	TString fp;
	if (!KVBase::SearchKVFile(gDataSet->GetDataSetEnv("INDRADB.Pedestals",""), fp, gDataSet->GetName())){
		Error("ReadPedestalList","Fichier %s, inconnu au bataillon",gDataSet->GetDataSetEnv("INDRADB.Pedestals",""));
		return;
	}
	fPedestals->SetTitle("Values of pedestals");
	if (!flist.OpenFileToRead(fp.Data())){
		return;
	}
	TEnv* env = 0;
	TEnvRec* rec = 0;
	KVDBParameterSet* par = 0;
	
	while (flist.IsOK()){
		flist.ReadLine(NULL);
		KVString file = flist.GetCurrentLine();
		KVNumberList nl;
		if ( file!="" && !file.BeginsWith('#') ){
			if ( KVBase::SearchKVFile(file.Data(), fp, gDataSet->GetName()) ){
				Info("ReadPedestalList","Lecture de %s",fp.Data());
				env = new TEnv();
				env->ReadFile(fp.Data(),kEnvAll);
				TIter it(env->GetTable());
				while ( (rec = (TEnvRec* )it.Next()) ){
					if (!strcmp(rec->GetName(),"RunRange")){
						nl.SetList(rec->GetValue());
					}
					else {
						par = new KVDBParameterSet(rec->GetName(), "Piedestal", 1);
						par->SetParameter(env->GetValue(rec->GetName(),0.0));
						fPedestals->AddRecord(par);
						LinkRecordToRunRange(par,nl);
					}
				}
				delete env;
				
			}
		}
	}
   Info("ReadPedestalList","End of reading");
}

//____________________________________________________________________________
void KVINDRADB_e613::ReadChannelVolt()
{
	Info("ReadChannelVolt","En cours d implementation");
/*
   //Read the names of pedestal files to use for each run range, found
   //in file with name defined by the environment variable:
   //   [dataset name].INDRADB.Pedestals:    ...
	
	KVFileReader flist;
	TString fp;
	if (!KVBase::SearchKVFile(gDataSet->GetDataSetEnv("INDRADB.ElectronicCalibration",""), fp, gDataSet->GetName())){
		Error("ReadChannelVolt","Fichier %s, inconnu au bataillon",gDataSet->GetDataSetEnv("INDRADB.ElectronicCalibration",""));
		return;
	}
	
	if (!flist.OpenFileToRead(fp.Data())){
		return;
	}
	TEnv* env = 0;
	TEnvRec* rec = 0;
	KVDBParameterSet* par = 0;
	
	while (flist.IsOK()){
		flist.ReadLine(NULL);
		KVString file = flist.GetCurrentLine();
		KVNumberList nl;
		if ( file!="" && !file.BeginsWith('#') ){
			if ( KVBase::SearchKVFile(file.Data(), fp, gDataSet->GetName()) ){
				Info("ReadChannelVolt","Lecture de %s",fp.Data());
				TString cal_type;
				if (fp.BeginsWith("PGtoVolt")) cal_type="Channel-Volt PG";
				else if (fp.BeginsWith("GGtoVolt")) cal_type="Channel-Volt GG";
				
				env = new TEnv();
				env->ReadFile(fp.Data(),kEnvAll);
				TIter it(env->GetTable());
				while ( (rec = (TEnvRec* )it.Next()) ){
					TString srec(rec->GetName());
					if (srec.BeginsWith("Ring")){
						srec.ReplaceAll("Ring","");
						lring->SetValue(srec.Data(),rec->GetValue());
					}
					else {
						
						//Int_t rr = ((KVINDRADetector* )gIndra->GetDetector(srec.Data()))->GetRingNumber();
						//parset = new KVDBParameterSet(srec.Data(), cal_type.Data(), 5);
            		//parset->SetParameters(a0, a1, a2, dum1, dum2);
						
						
						//par = new KVDBParameterSet(rec->GetName(), "Piedestal", 1);
						//par->SetParameter(env->GetValue(rec->GetName(),0.0));
						//fPedestals->AddRecord(par);
						//LinkRecordToRunRange(par,nl);
						
					}
				}
				delete env;
				
			}
		}
	}
   Info("ReadPedestalList","End of reading");
*/
}

