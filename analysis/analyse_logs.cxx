#include "KVLogReader.h"
#include "KVNumberList.h"
#include "KVParameterList.h"
#include "Riostream.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"

/*
analyse_logs [fmt] [files]

arguments are:
	[fmt] format of jobname allowing to extract run number, e.g. "PbAu29MeV_R%d" for jobs with names "PbAu29MeV_R8213" etc.
	[files]	list of BQS log files to be read and analysed

Example:
analyse_logs "run%d" run*.o*
*/

int main(int argc, char** argv)
{
	if(argc<2){
		cout << "\t\tAnalysis of BQS batch log files" << endl << endl;
		cout << "\tanalyse_logs [jobname]" << endl << endl;
		cout << "\targuments are:" << endl;
		cout << "\t\t[jobname] common root of name of all jobs to analyse" << endl;
		cout << "\t\t      e.g. \"PbAu29MeV\" for jobs with names like \"PbAu29MeV_R8213\" etc." << endl;
		cout << "\t\t      The programme will attempt to read all files with names like" << endl;
		cout << "\t\t          PbAu29MeV_R8213.o5647093" << endl;
		cout << "\t\t          PbAu29MeV_R8214.o5647094" << endl;
		cout << "\t\t      etc. in the current directory" << endl;		
		return 0;
	}
	KVLogReader log_reader;
	
	// take root of job name and add suffix "_R"
	KVString nameForm(argv[1]);
	nameForm+="_R";
	KVString nameFormLR=nameForm+"%d";//format for KVLogReader	
	log_reader.SetNameFormat( nameFormLR.Data() );
	
	//number lists for listing runs according to status: OK, out of time, killed, seg fault
	KVNumberList ok, oot, kill, seg;
	//max and min CPU, SCRATCH, VIRTUAL STORAGE for OK jobs
	KVParameterList<Double_t> OKlimits;
	//average CPU, SCRATCH, VIRTUAL STORAGE for OK jobs
	KVParameterList<Double_t> OKavg; Double_t Dzero=0.;
	OKavg.SetParameter("CPU", Dzero);
	OKavg.SetParameter("SCR", Dzero);
	OKavg.SetParameter("MEM", Dzero);
	//max and min CPU, SCRATCH, VIRTUAL STORAGE for killed jobs
	KVParameterList<Double_t> KILLlimits;
	//average CPU, SCRATCH, VIRTUAL STORAGE for killed jobs
	KVParameterList<Double_t> KILLavg; 
	KILLavg.SetParameter("CPU", Dzero);
	KILLavg.SetParameter("SCR", Dzero);
	KILLavg.SetParameter("MEM", Dzero);
	//max and min CPU, SCRATCH, VIRTUAL STORAGE for incomplete jobs
	KVParameterList<Double_t> INCOMPlimits;
	//average CPU, SCRATCH, VIRTUAL STORAGE for incomplete jobs
	KVParameterList<Double_t> INCOMPavg; 
	INCOMPavg.SetParameter("CPU", Dzero);
	INCOMPavg.SetParameter("SCR", Dzero);
	INCOMPavg.SetParameter("MEM", Dzero);
	Double_t CPUreq_ok, MEMreq_ok, SCRreq_ok;
   CPUreq_ok=MEMreq_ok=SCRreq_ok=0;
	Double_t CPUreq_kill, MEMreq_kill, SCRreq_kill;
   CPUreq_kill=MEMreq_kill=SCRreq_kill=0;
	Double_t CPUreq_incomp, MEMreq_incomp, SCRreq_incomp;
   CPUreq_incomp=MEMreq_incomp=SCRreq_incomp=0;
	
	//loop over files in current directory
	TSystemDirectory thisDir(".",".");
	TList* fileList=thisDir.GetListOfFiles();
	TIter nextFile(fileList);
	int nfile=0;
	TSystemFile* aFile=0;
	while( (aFile = (TSystemFile*)nextFile()) ){
		
		// look for files with names like [nameForm]1234.o6579780
		KVString fileName = aFile->GetName();
		if( !fileName.BeginsWith(nameForm.Data()) ) continue;
		if( !fileName.Contains(".o") ) continue;
   				
		nfile++;
		
		log_reader.ReadFile( fileName.Data() );
		
		Int_t run = log_reader.GetRunNumber();
		if( log_reader.JobOK() ){
			ok.Add(run);
			CPUreq_ok = log_reader.GetCPUrequest();
			MEMreq_ok = log_reader.GetMEMrequest();
			SCRreq_ok = log_reader.GetSCRATCHrequest();
			Double_t dummy;
			dummy = OKavg.GetParameter("CPU") + log_reader.GetCPUused();
			OKavg.SetParameter("CPU", dummy);
			dummy = OKavg.GetParameter("SCR") + log_reader.GetSCRATCHused();
			OKavg.SetParameter("SCR", dummy);
			dummy = OKavg.GetParameter("MEM") + log_reader.GetMEMused();
			OKavg.SetParameter("MEM", dummy);
         //max and min
			dummy = log_reader.GetCPUused();
         if(ok.GetNValues()==1){
            OKlimits.SetParameter("maxCPU", dummy);
            OKlimits.SetParameter("minCPU", dummy);
         }
         else
         {
			   if( dummy > OKlimits.GetParameter("maxCPU") ) OKlimits.SetParameter("maxCPU", dummy);
			   else if( dummy < OKlimits.GetParameter("minCPU") ) OKlimits.SetParameter("minCPU", dummy);
         }
			dummy = log_reader.GetMEMused();
         if(ok.GetNValues()==1){
            OKlimits.SetParameter("maxMEM", dummy);
            OKlimits.SetParameter("minMEM", dummy);
         }
         else
         {
			   if( dummy > OKlimits.GetParameter("maxMEM") ) OKlimits.SetParameter("maxMEM", dummy);
			   else if( dummy < OKlimits.GetParameter("minMEM") ) OKlimits.SetParameter("minMEM", dummy);
         }
			dummy = log_reader.GetSCRATCHused();
         if(ok.GetNValues()==1){
            OKlimits.SetParameter("maxSCR", dummy);
            OKlimits.SetParameter("minSCR", dummy);
         }
         else
         {
			   if( dummy > OKlimits.GetParameter("maxSCR") ) OKlimits.SetParameter("maxSCR", dummy);
			   else if( dummy < OKlimits.GetParameter("minSCR") ) OKlimits.SetParameter("minSCR", dummy);
         }
		}
		else if( log_reader.Killed() ){
			kill.Add(run);
			CPUreq_kill = log_reader.GetCPUrequest();
			MEMreq_kill = log_reader.GetMEMrequest();
			SCRreq_kill = log_reader.GetSCRATCHrequest();
			Double_t dummy;
			dummy = KILLavg.GetParameter("CPU") + log_reader.GetCPUused();
			KILLavg.SetParameter("CPU", dummy);
			dummy = KILLavg.GetParameter("SCR") + log_reader.GetSCRATCHused();
			KILLavg.SetParameter("SCR", dummy);
			dummy = KILLavg.GetParameter("MEM") + log_reader.GetMEMused();
			KILLavg.SetParameter("MEM", dummy);
         //max and min
			dummy = log_reader.GetCPUused();
         if(kill.GetNValues()==1){
            KILLlimits.SetParameter("maxCPU", dummy);
            KILLlimits.SetParameter("minCPU", dummy);
         }
         else
         {
			   if( dummy > KILLlimits.GetParameter("maxCPU") ) KILLlimits.SetParameter("maxCPU", dummy);
			   else if( dummy < KILLlimits.GetParameter("minCPU") ) KILLlimits.SetParameter("minCPU", dummy);
         }
			dummy = log_reader.GetMEMused();
         if(kill.GetNValues()==1){
				KILLlimits.SetParameter("maxMEM", dummy);
				KILLlimits.SetParameter("minMEM", dummy);
			}
         else
         {
			   if( dummy > KILLlimits.GetParameter("maxMEM") ) KILLlimits.SetParameter("maxMEM", dummy);
			   else if( dummy < KILLlimits.GetParameter("minMEM") ) KILLlimits.SetParameter("minMEM", dummy);
         }
			dummy = log_reader.GetSCRATCHused();
         if(kill.GetNValues()==1){
				KILLlimits.SetParameter("maxSCR", dummy);
				KILLlimits.SetParameter("minSCR", dummy);
         }
         else
         {
			   if( dummy > KILLlimits.GetParameter("maxSCR") ) KILLlimits.SetParameter("maxSCR", dummy);
			   else if( dummy < KILLlimits.GetParameter("minSCR") ) KILLlimits.SetParameter("minSCR", dummy);
         }
		}
		else if( log_reader.SegFault() ) seg.Add(run);
		else if( log_reader.Incomplete() ){
         oot.Add(run);
			CPUreq_incomp = log_reader.GetCPUrequest();
			MEMreq_incomp = log_reader.GetMEMrequest();
			SCRreq_incomp = log_reader.GetSCRATCHrequest();
			Double_t dummy;
			dummy = INCOMPavg.GetParameter("CPU") + log_reader.GetCPUused();
			INCOMPavg.SetParameter("CPU", dummy);
			dummy = INCOMPavg.GetParameter("SCR") + log_reader.GetSCRATCHused();
			INCOMPavg.SetParameter("SCR", dummy);
			dummy = INCOMPavg.GetParameter("MEM") + log_reader.GetMEMused();
			INCOMPavg.SetParameter("MEM", dummy);
         //max and min
			dummy = log_reader.GetCPUused();
         if(oot.GetNValues()==1){
            INCOMPlimits.SetParameter("maxCPU", dummy);
            INCOMPlimits.SetParameter("minCPU", dummy);
         }
         else
         {
			   if( dummy > INCOMPlimits.GetParameter("maxCPU") ) INCOMPlimits.SetParameter("maxCPU", dummy);
			   else if( dummy < INCOMPlimits.GetParameter("minCPU") ) INCOMPlimits.SetParameter("minCPU", dummy);
         }
			dummy = log_reader.GetMEMused();
         if(oot.GetNValues()==1){
				INCOMPlimits.SetParameter("maxMEM", dummy);
				INCOMPlimits.SetParameter("minMEM", dummy);
			}
         else
         {
			   if( dummy > INCOMPlimits.GetParameter("maxMEM") ) INCOMPlimits.SetParameter("maxMEM", dummy);
			   else if( dummy < INCOMPlimits.GetParameter("minMEM") ) INCOMPlimits.SetParameter("minMEM", dummy);
         }
			dummy = log_reader.GetSCRATCHused();
         if(oot.GetNValues()==1){
				INCOMPlimits.SetParameter("maxSCR", dummy);
				INCOMPlimits.SetParameter("minSCR", dummy);
         }
         else
         {
			   if( dummy > INCOMPlimits.GetParameter("maxSCR") ) INCOMPlimits.SetParameter("maxSCR", dummy);
			   else if( dummy < INCOMPlimits.GetParameter("minSCR") ) INCOMPlimits.SetParameter("minSCR", dummy);
         }
      }
	}
	
	cout << "BQS log analysis==============>" << endl;
	cout << "Analysed " << nfile << " jobs" << endl;
	cout << endl;
	cout << "      ";
	cout << ok.GetNValues() << " jobs were OK \n\n" << ok.GetList() << endl;
	if(ok.GetNValues()){
		cout << "\n     CPU (req / min / avg / max) :   (";
		cout << CPUreq_ok << " / ";
		cout << OKlimits.GetParameter("minCPU") << " / ";
		cout << OKavg.GetParameter("CPU")/(1.*ok.GetNValues()) << " / ";
		cout << OKlimits.GetParameter("maxCPU") << ")" << endl;
		cout << "     MEM [MB] (req / min / avg / max) :   (";
		cout << MEMreq_ok/1024. << " / ";
		cout << OKlimits.GetParameter("minMEM")/1024. << " / ";
		cout << OKavg.GetParameter("MEM")/(1.*ok.GetNValues())/1024. << " / ";
		cout << OKlimits.GetParameter("maxMEM")/1024. << ")" << endl;
		cout << "     SCRATCH [MB] (req / min / avg / max) :   (";
		cout << SCRreq_ok/1024. << " / ";
		cout << OKlimits.GetParameter("minSCR")/1024. << " / ";
		cout << OKavg.GetParameter("SCR")/(1.*ok.GetNValues())/1024. << " / ";
		cout << OKlimits.GetParameter("maxSCR")/1024. << ")" << endl << endl;
	}
	cout << "      ";
	cout << kill.GetNValues() << " jobs were KILLED :\n\n" << kill.GetList() << endl;
	if(kill.GetNValues()){
		cout << "\n     CPU (req / min / avg / max) :   (";
		cout << CPUreq_kill << " / ";
		cout << KILLlimits.GetParameter("minCPU") << " / ";
		cout << KILLavg.GetParameter("CPU")/(1.*kill.GetNValues()) << " / ";
		cout << KILLlimits.GetParameter("maxCPU") << ")" << endl;
		cout << "     MEM [MB] (req / min / avg / max) :   (";
		cout << MEMreq_kill/1024. << " / ";
		cout << KILLlimits.GetParameter("minMEM")/1024. << " / ";
		cout << KILLavg.GetParameter("MEM")/(1.*kill.GetNValues())/1024. << " / ";
		cout << KILLlimits.GetParameter("maxMEM")/1024. << ")" << endl;
		cout << "     SCRATCH [MB] (req / min / avg / max) :   (";
		cout << SCRreq_kill/1024. << " / ";
		cout << KILLlimits.GetParameter("minSCR")/1024. << " / ";
		cout << KILLavg.GetParameter("SCR")/(1.*kill.GetNValues())/1024. << " / ";
		cout << KILLlimits.GetParameter("maxSCR")/1024. << ")" << endl << endl;
	}
	cout << "      ";
	cout << oot.GetNValues() << " jobs were INCOMPLETE :\n\n" << oot.GetList() << endl<< endl;
	if(oot.GetNValues()){
		cout << "\n     CPU (req / min / avg / max) :   (";
		cout << CPUreq_incomp << " / ";
		cout << INCOMPlimits.GetParameter("minCPU") << " / ";
		cout << INCOMPavg.GetParameter("CPU")/(1.*oot.GetNValues()) << " / ";
		cout << INCOMPlimits.GetParameter("maxCPU") << ")" << endl;
		cout << "     MEM [MB] (req / min / avg / max) :   (";
		cout << MEMreq_incomp/1024. << " / ";
		cout << INCOMPlimits.GetParameter("minMEM")/1024. << " / ";
		cout << INCOMPavg.GetParameter("MEM")/(1.*oot.GetNValues())/1024. << " / ";
		cout << INCOMPlimits.GetParameter("maxMEM")/1024. << ")" << endl;
		cout << "     SCRATCH [MB] (req / min / avg / max) :   (";
		cout << SCRreq_incomp/1024. << " / ";
		cout << INCOMPlimits.GetParameter("minSCR")/1024. << " / ";
		cout << INCOMPavg.GetParameter("SCR")/(1.*oot.GetNValues())/1024. << " / ";
		cout << INCOMPlimits.GetParameter("maxSCR")/1024. << ")" << endl << endl;
	}
	cout << "      ";
	cout << seg.GetNValues() << " jobs had SEGMENTATION FAULT :\n\n" << seg.GetList() << endl << endl;
	return 0;
}
