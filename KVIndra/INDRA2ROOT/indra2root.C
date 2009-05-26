//$Id: indra2root.C,v 1.29 2006/10/27 15:31:53 franklan Exp $
// Lecture d'un fichier d'evenements INDRA et transformation en arbre ROOT
//
#include "Riostream.h"
#include "TFile.h"
#include "TTree.h"
#include "KVINDRA.h"
#include "KVINDRACodes.h"
#include "KVINDRAReconNuc.h"
#include "KVINDRAReconEvent.h"
#include "KVChIo.h"
#include "KVSilicon.h"
#include "KVCsI.h"
#include "KVPhoswich.h"
#include "KVDetector.h"
#include "KVIDTelescope.h"
#include "KVDBRun.h"
#include "TProcessID.h"
#include "KVDataRepositoryManager.h"
#include "KVDataSetManager.h"
#include "KVDataRepository.h"
#include "KVDataSet.h"
#include "TString.h"

void faire_arbre_lire(const Char_t*, char* file_fmt="arbre_root_%d.txt", long int nbevt=-1);

#ifndef __CINT__
//
// Programme principal
//

int main(int argc,char **argv)
{

if(argc==1){
	cout << "indra2root: Usage:" << endl;
	cout << "                                  indra2root [dataset] [arbre_root_%d.txt] [nevents]"<<endl;
}
else if(argc==3)
	faire_arbre_lire(argv[1],argv[2]);
else if(argc==4)
	faire_arbre_lire(argv[1],argv[2],atoi(argv[3]));

return 0;
}
#endif


//_____________________________________________________________________________________________//

void lire_evt(ifstream &f_in,KVINDRAReconEvent *evt);

Int_t file_number=1; TString file_name, file_name_fmt;
//set to kTRUE when reading 2nd campaign: => phoswich code is 4, not 2
Bool_t camp2=kFALSE;
//set to kTRUE when reading 4th campaign: => ring1 is Si-CsI
Bool_t camp4=kFALSE;
//total number of events read from each file - for tests
Int_t events_in_file=0;
//total number of events written at end of last fortran file
Int_t total_fortran=1;
//totals of events read and trees filled
Int_t events_good, events_read;
//tree for writing events
TTree* data_tree;

void faire_arbre_lire(const Char_t* dataset, char* file_fmt, long int nbevt)
{
//
// Fabriquer un arbre de Events
//
	//file_fmt = format string representing name of text-file(s) to read
	//	by default, this is "arbre_root_%d.txt"
	//Every file with this format of name will be read, starting with
	//"arbre_root_1.txt", then 'arbre_root_2.txt", until no more files are found

	events_good=events_read=0;
	//create and initialise data repository manager
	new KVDataRepositoryManager;
	gDataRepositoryManager->Init();
	KVDataSet* ds = gDataSetManager->GetDataSet( dataset );
	ds->cd();
	ds->BuildMultiDetector();
	camp2 = !strcmp(dataset, "INDRA_camp2");
	if(camp2){
		cout << "Special treatment INDRA 2nd campaign data:"<<endl;
		cout << "   particles detected in phoswich ring 1 have general ID code=4" << endl;
		cout << "   after translation, they will have Veda ID code=2 (like 1st campaign)" << endl << endl;
	}
	camp4 = !strcmp(dataset, "INDRA_camp4");

	//write name of first file to read : arbre_root_1.txt, by default
	file_name_fmt = file_fmt; file_name.Form(file_fmt, file_number);
	//open first file
	ifstream f_in(file_name.Data());
	//check file is OK
	if( !f_in.is_open() ) {
		cout << "Error opening file " << file_name.Data() << endl;
		exit(1);
	} else {
		//run number is first line in first file
		Int_t run_number=0;
		events_in_file=0;//total number events read from each file
		f_in >> run_number;
		cout << "Reading run number " << run_number << endl;
		gIndra->SetParameters(run_number);
		if(gIndra->GetCurrentRun()) gIndra->GetCurrentRun()->Print();
		KVINDRAReconEvent *evt=new KVINDRAReconEvent();

		evt->SetTitle( gIndra->GetSystem()->GetName() );
		evt->SetName( gIndra->GetCurrentRun()->GetName() );

		//create new ROOT file for converted events
		TFile* fi = gDataSet->NewRunfile("root", run_number);
		data_tree = new TTree("tree", Form("%s : %s : %s events converted from DST",
			 gIndra->GetCurrentRun()->GetName(), gIndra->GetSystem()->GetName(), "root"));
		data_tree->SetAutoSave(30000000);
		data_tree->Branch("data", "KVINDRAReconEvent", &evt, 64000, 0)->SetAutoDelete(kFALSE);

		if(nbevt < 0) nbevt=1000000000;
		cout << "debut lecture ... " <<endl;

		while(f_in.good() && (events_read < nbevt)){
		
			lire_evt(f_in,evt);
  
  			evt->Clear();
  
 			if(events_read%10000 == 0 && events_read > 0){
  				cout << events_read << "th event read... " << endl;
  			}
 		}
		
		if( gIndra->GetCurrentRun()->GetEvents() ){
			//check number of events against scaler info for this run
			Double_t check_events=(1.*events_read)/((Double_t)total_fortran);
			cout << "Nb. evts read from files: " << events_read << endl;
			cout << "Nb. evts written to TTree: " << events_good << endl;
			cout << "Nb. evts in FORTRAN file: " << total_fortran <<endl;
			if( check_events < 0.99 ) cout << "ERROR: number of events read <99% number in FORTRAN file" << endl;
		}
		
		f_in.close();

		//At the end of each run we:
		//	write the tree and INDRA into the new file
		//	add the file to the repository
	
		fi->cd();
		gIndra->Write("INDRA");//write INDRA to file
		data_tree->Write();//write tree to file
		gDataSet->CommitRunfile("root", run_number, fi);
	}
}

//_____________________________________________________________________________________________//
//functions handling particles with different identification codes
KVDetector* Code2and9and10(int ring, int mod);
KVDetector* Code3(int ring, int mod);
KVDetector* Code0(int ring, int mod);
KVDetector* Code1(int ring, int mod);
KVDetector* Code7(int ring, int mod);
KVDetector* Code4and5and6and8(int ring, int mod);
//array of pointers associating each ID code with the corresponding function
KVDetector* (*CodeFunc[])(int,int)={
		Code0,			//code0
		Code1,			//code1
		Code2and9and10,	//code2
		Code3,			//code3
		Code4and5and6and8,//code4
		Code4and5and6and8,//code5
		Code4and5and6and8,//code6
		Code7,			//code7
		Code4and5and6and8,//code8
		Code2and9and10,	//code9
		Code2and9and10	//code10
};
//_____________________________________________________________________________________________//

Int_t mt;
Double_t de1,de2,de3,de4,de5;
Double_t canal[16];
KVIDTelescope* identifying_telescope;

void lire_evt(ifstream &f_in,KVINDRAReconEvent *evt)
{
	//
	//Lire un evenement
	//
	
TString buff;
  int mul,num_ev;
  //check for "TOTAL=" line at end of last file with total number of events
//  if(f_in.good()) f_in >> num_ev;
  buff.ReadLine(f_in);
  	if(buff.BeginsWith("TOTAL=")){
	  	buff.Remove(0,6); total_fortran=buff.Atoi();
	  	//just a dummy read to make f_in.good() return kFALSE : this is the end of the last file
	  	f_in >> num_ev;
  	}
  	else{
	  	num_ev = buff.Atoi();
	}
  if(f_in.good()){
	  events_in_file++;//total number events in each file
	  events_read++;//total events read
	  events_good++;
	evt->SetNumber(num_ev);
	f_in >> mul;
	for(int i=0;i<mul;i++){
		int z,a,code,icou,imod,ecode,ncans;
		double z_indra,a_indra,ener,de_mylar;
		for(int gj=0;gj<16;gj++) canal[gj]=0.0;
		f_in >> a;
  		f_in >> z;
  		f_in >> z_indra;
  		f_in >> a_indra;
//Check energy is not NAN
//  		f_in >> ener;
		buff.ReadLine(f_in);
		if(buff.IsFloat()) ener=buff.Atof();
		else {
			ener=0.;
			cout << "BAD:  event#" << num_ev << " z="<<z<<" a="<<a<<" / Non-numeric value read for ener. Set E=0" << endl;
			cout << "(read: " << buff.Data() << ")" << endl;
		}
  		f_in >> icou;
  		f_in >> imod;
  		f_in >> code;
  		f_in >> ecode;
//  		f_in >> de_mylar;
		buff.ReadLine(f_in);
		if(buff.IsFloat()) de_mylar=buff.Atof();
		else {
			de_mylar=0.;
			cout << "BAD:  event#" << num_ev << " z="<<z<<" a="<<a<<" / Non-numeric value read for de_mylar. Set de_mylar=0" << endl;
			cout << "(read: " << buff.Data() << ")" << endl;
		}
//  		f_in >> de1;
		buff.ReadLine(f_in);
		if(buff.IsFloat()) de1=buff.Atof();
		else {
			de1=0.;
			cout << "BAD:  event#" << num_ev << " z="<<z<<" a="<<a<<" / Non-numeric value read for de1. Set de1=0" << endl;
			cout << "(read: " << buff.Data() << ")" << endl;
		}
//  		f_in >> de2;
		buff.ReadLine(f_in);
		if(buff.IsFloat()) de2=buff.Atof();
		else {
			de2=0.;
			cout << "BAD:  event#" << num_ev << " z="<<z<<" a="<<a<<" / Non-numeric value read for de2. Set de2=0" << endl;
			cout << "(read: " << buff.Data() << ")" << endl;
		}
//  		f_in >> de3;
		buff.ReadLine(f_in);
		if(buff.IsFloat()) de3=buff.Atof();
		else {
			de3=0.;
			cout << "BAD:  event#" << num_ev << " z="<<z<<" a="<<a<<" / Non-numeric value read for de3. Set de3=0" << endl;
			cout << "(read: " << buff.Data() << ")" << endl;
		}
//  		f_in >> de4;
		buff.ReadLine(f_in);
		if(buff.IsFloat()) de4=buff.Atof();
		else {
			de4=0.;
			cout << "BAD:  event#" << num_ev << " z="<<z<<" a="<<a<<" / Non-numeric value read for de4. Set de4=0" << endl;
			cout << "(read: " << buff.Data() << ")" << endl;
		}
//  		f_in >> de5;
		buff.ReadLine(f_in);
		if(buff.IsFloat()) de5=buff.Atof();
		else {
			de5=0.;
			cout << "BAD:  event#" << num_ev << " z="<<z<<" a="<<a<<" / Non-numeric value read for de5. Set de5=0" << endl;
			cout << "(read: " << buff.Data() << ")" << endl;
		}
  		f_in >> mt;
  		f_in >> ncans;
  		
		if( !icou ){
			cout << "BAD:  event#" << num_ev << "  particle#" << i << "  Z=" << z << "  code=" << code << endl;
		}
		
  		if(ncans){
	  		int kk; double chan;
			for(int mk=1;mk<=ncans;mk++)
			{
				f_in >> kk >> chan;
				canal[kk] = chan;
			}
  		}
	
  		if(code<11 && icou>0 && imod>0 && icou<18 && imod<25){		
			//work out where particle stopped
			//set up corresponding detectors with energy losses and raw data (canal)
			//set stopping detector's marqueur de temps to the MT of the particle
			//(the other Mt are not written on the DST's).
			//then use KVINDRAReconNuc::Reconstruct to set up list of hit detectors,
			//hit group and telescope for this particle
			KVDetector* det_stop=0; identifying_telescope=0;
			det_stop = (*CodeFunc[code])(icou, imod);
			if(!det_stop) {
				cout << "det_stop=0: event#"<<num_ev<<" particle#"<<i<<"  icou="<<icou<<" imod="<<imod<<
						" z="<<z<<" a="<<a<<" code="<<code<<" ecode="<<ecode<<endl;
			} else {
				KVINDRAReconNuc *tmp = evt->AddParticle();
				if (!tmp) {
					Fatal("lire_evt","KVINDRAReconEvent::AddParticle() returns NULL pointer.");
					gSystem->Exit(1);
				}
				det_stop->GetACQParam("T")->SetData((UShort_t)mt);
				tmp->Reconstruct(det_stop);
				if(identifying_telescope){
					tmp->SetIdentifyingTelescope( identifying_telescope );
					tmp->SetIsIdentified();
				}
				if(ecode>0) tmp->SetIsCalibrated();
				//modify ID code for phoswich particles 2nd campaign
				//we put VEDA ID code=2 just like for 1st campaign
				if(camp2 && icou==1 && code==4) code=2;
				tmp->SetIDCode( KVINDRACodes::VedaIDCodeToBitmask(code) );
				tmp->SetECode( KVINDRACodes::VedaECodeToBitmask(ecode) );
				tmp->SetZ(z);
				tmp->SetRealZ(z_indra);
				tmp->SetA(TMath::Abs(a));
				if( a>0 && (code==2 || code==3) ){
					tmp->GetCodes().SetIsotopeResolve();
					tmp->SetRealA(a_indra);
				}
				else
					tmp->GetCodes().SetIsotopeResolve( kFALSE );
				if( code == 1 ){
					//set Z and realZ = 0 in case of 'neutron'
					tmp->SetZ(0); tmp->SetRealZ(0);tmp->SetA(1); tmp->SetRealA(0);
				}
				tmp->SetEnergy(ener);
			}//if(det_stop)
  		}//if code<11
	}//for(int i=0;

	//write event to tree
	data_tree->Fill();
	
  }//if(f_in.good

  if(!f_in.good()){
	  //we appear to have reached the end of the file we're currently reading.
	  //We will try to open another one.
	  cout << "Closing file : "  << file_name.Data() << endl;
	  cout << "I read " << events_in_file << " events from that file (should be 50000)" << endl;
	  events_in_file=0;
	  file_number++;
	  f_in.close(); f_in.clear();
	  file_name.Form(file_name_fmt.Data(), file_number);
	  f_in.open(file_name.Data());
	  if(f_in.is_open()) {
		  cout << "Opened file : " << file_name.Data() << endl;
	  } else {
		  cout << "File " <<  file_name.Data() << " not found: is this the end ?" << endl;
	  }
  }
}
//_____________________________________________________________________________________________//
KVDetector* Code2and9and10(int ring, int mod)
{
//code = 2 : part. ident. dans CsI 
	//for 1st campaign: code=2 for phoswich (ring 1) also
//code = 9 : ident. incomplete dans CsI ou Phoswich (Z.min)
//code =10 : ident. "entre les lignes" dans CsI
//set coder values of detectors concerned by a particle with ID code 2
//Les codes 9 et 10 sont des codes qui ont ete ajoutes pour donner
//au code 2 la notion d'identification sans ambiguites.	
	
	//returns pointer of the detector for which 
	//KVINDRAReconNuc::Reconstruct must be called.
	//cout << "Code2and9and10" << endl;
 	 if(ring==1){
	  if(!(gIndra->InheritsFrom("KVINDRA4")))
 		 {
 		 //phoswich
 		 KVPhoswich *phos = 
 			 (KVPhoswich*)gIndra->GetDetectorByType(ring,mod,Phos_R);
		 if(!phos) return 0;
 		 phos->SetEnergy( de1 );
 		 phos->GetACQParam("R")->SetData( (UShort_t)canal[Phos_R] );
 		 phos->GetACQParam("L")->SetData( (UShort_t)canal[Phos_L] );
 		 phos->GetACQParam("T")->SetData( (UShort_t)mt );
		 identifying_telescope = gIndra->GetIDTelescope(Form("PHOS_R_L_%02d", mod));
		 return phos;
 		 }
 	  else // campagne 4: GSI Campaign
 		 {
 		 KVCsI *csi = (KVCsI*) gIndra->GetDetectorByType(ring,mod,CsI_R);
		 if(!csi) return 0;
 		 KVSilicon *si = (KVSilicon*) gIndra->GetDetectorByType(ring,mod,Si_GG);
 		 if(si)si->SetEnergy( de2 );
 		 csi->SetEnergy( de3 );
 		 if(si){
 			 si->GetACQParam("GG")->SetData( (UShort_t)canal[Si_GG] );
 			 si->GetACQParam("PG")->SetData( (UShort_t)canal[Si_PG] );
 			 si->GetACQParam("T")->SetData( (UShort_t)canal[Si_T] );
 		 }
 		 csi->GetACQParam("R")->SetData( (UShort_t)canal[CsI_R] );
 		 csi->GetACQParam("L")->SetData( (UShort_t)canal[CsI_L] );
 		 csi->GetACQParam("T")->SetData( (UShort_t)mt );
		 identifying_telescope = gIndra->GetIDTelescope(Form("CSI_R_L_%02d%02d", ring, mod));
 		 return csi;
 		 }  
 	 }
 	 else if(ring>=2 && ring<=9){
 		 //chio-si-csi
 		 KVCsI *csi = (KVCsI*) gIndra->GetDetectorByType(ring,mod,CsI_R);
		 if(!csi) return 0;
 		 KVSilicon *si = (KVSilicon*) gIndra->GetDetectorByType(ring,mod,Si_GG);
 		 KVChIo *chio = gIndra->GetChIoOf( csi );
		 if(!chio) return 0;
 		 chio->SetEnergy( de1 );
 		 if(si)si->SetEnergy( de2 );
 		 csi->SetEnergy( de3 );
 		 chio->GetACQParam("GG")->SetData( (UShort_t)canal[ChIo_GG] );
 		 chio->GetACQParam("PG")->SetData( (UShort_t)canal[ChIo_PG] );
 		 chio->GetACQParam("T")->SetData( (UShort_t)canal[ChIo_T] );
 		 if(si){
 			 si->GetACQParam("GG")->SetData( (UShort_t)canal[Si_GG] );
 			 si->GetACQParam("PG")->SetData( (UShort_t)canal[Si_PG] );
 			 si->GetACQParam("T")->SetData( (UShort_t)canal[Si_T] );
 		 }
 		 csi->GetACQParam("R")->SetData( (UShort_t)canal[CsI_R] );
 		 csi->GetACQParam("L")->SetData( (UShort_t)canal[CsI_L] );
 		 csi->GetACQParam("T")->SetData( (UShort_t)mt );
		 identifying_telescope = gIndra->GetIDTelescope(Form("CSI_R_L_%02d%02d", ring, mod));
 		 return csi;
 	 }
 	 else if(ring>9){
 		 //chio-csi or chio-si75-sili-csi
 		 KVCsI *csi = (KVCsI*) gIndra->GetDetectorByType(ring,mod,CsI_R);
		 if(!csi) return 0;
 		 KVChIo *chio = gIndra->GetChIoOf( csi );
		 if(!chio) return 0;
 		 KVSi75 *si75 = 0;
 		 KVSiLi *sili = 0;
 		 if(canal[Si75_GG]>0) {
 			 si75 = (KVSi75*)gIndra->GetDetectorByType(ring,mod,Si75_GG);
 		 }
 		 if(canal[SiLi_GG]>0) {
 			 sili = (KVSiLi*)gIndra->GetDetectorByType(ring,mod,SiLi_GG);
 		 }
 		 chio->SetEnergy( de1 );
 		 if(si75)si75->SetEnergy( de4 );
 		 if(sili)sili->SetEnergy( de5 );
 		 csi->SetEnergy( de3 );
 		 chio->GetACQParam("GG")->SetData( (UShort_t)canal[ChIo_GG] );
 		 chio->GetACQParam("PG")->SetData( (UShort_t)canal[ChIo_PG] );
 		 chio->GetACQParam("T")->SetData( (UShort_t)canal[ChIo_T] );
 		 if(si75){
 			 si75->GetACQParam("GG")->SetData( (UShort_t)canal[Si75_GG] );
 			 si75->GetACQParam("PG")->SetData( (UShort_t)canal[Si75_PG] );
 			 si75->GetACQParam("T")->SetData( (UShort_t)canal[Si75_T] );
 		 }
 		 if(sili){
 			 sili->GetACQParam("GG")->SetData( (UShort_t)canal[SiLi_GG] );
 			 sili->GetACQParam("PG")->SetData( (UShort_t)canal[SiLi_PG] );
 			 sili->GetACQParam("T")->SetData( (UShort_t)canal[SiLi_T] );
 		 }
 		 csi->GetACQParam("R")->SetData( (UShort_t)canal[CsI_R] );
 		 csi->GetACQParam("L")->SetData( (UShort_t)canal[CsI_L] );
 		 csi->GetACQParam("T")->SetData( (UShort_t)mt );
		 identifying_telescope = gIndra->GetIDTelescope(Form("CSI_R_L_%02d%02d", ring, mod));
 		 return csi;
 	 }
	 return 0;
}
//_____________________________________________________________________________________________//
KVDetector* Code3(int ring, int mod)
{
//	cout << "Code3" << endl;
//code = 3 : fragment identifie dans Si-CsI
//		   ou fragment ident. dans Si75-SiLi ou SiLi-CsI
	//set coder values of detectors concerned by a particle with ID code 3
	//returns pointer of the detector for which 
	//KVINDRAReconNuc::Reconstruct must be called.
	
	// First Ring if GSI Campaign
	if(ring==1 && gIndra->InheritsFrom("KVINDRA4")){
		KVCsI *csi = (KVCsI*) gIndra->GetDetectorByType(ring,mod,CsI_R);
		if(!csi) return 0;
		KVSilicon *si = (KVSilicon*) gIndra->GetDetectorByType(ring,mod,Si_GG);
		if( !si ) return 0;
		si->SetEnergy( de2 );
		csi->SetEnergy( de3 );
		si->GetACQParam("GG")->SetData( (UShort_t)canal[Si_GG] );
		si->GetACQParam("PG")->SetData( (UShort_t)canal[Si_PG] );
		si->GetACQParam("T")->SetData( (UShort_t)canal[Si_T] );
		csi->GetACQParam("R")->SetData( (UShort_t)canal[CsI_R] );
		csi->GetACQParam("L")->SetData( (UShort_t)canal[CsI_L] );
		csi->GetACQParam("T")->SetData( (UShort_t)mt );
		 identifying_telescope = gIndra->GetIDTelescope(Form("SI_CSI_%02d%02d", ring, mod));
		return csi;
	}
	else if(ring>=2 && ring <= 9){
		//chio-si-csi
		KVCsI *csi = (KVCsI*) gIndra->GetDetectorByType(ring,mod,CsI_R);
		if(!csi) return 0;
		KVSilicon *si = (KVSilicon*) gIndra->GetDetectorByType(ring,mod,Si_GG);
		if( !si ) return 0;
		KVChIo *chio = gIndra->GetChIoOf( csi );
		if( !chio ) return 0;
		chio->SetEnergy( de1 );
		chio->GetACQParam("GG")->SetData( (UShort_t)canal[ChIo_GG] );
		chio->GetACQParam("PG")->SetData( (UShort_t)canal[ChIo_PG] );
		chio->GetACQParam("T")->SetData( (UShort_t)canal[ChIo_T] );
		si->SetEnergy( de2 );
		csi->SetEnergy( de3 );
		si->GetACQParam("GG")->SetData( (UShort_t)canal[Si_GG] );
		si->GetACQParam("PG")->SetData( (UShort_t)canal[Si_PG] );
		si->GetACQParam("T")->SetData( (UShort_t)canal[Si_T] );
		csi->GetACQParam("R")->SetData( (UShort_t)canal[CsI_R] );
		csi->GetACQParam("L")->SetData( (UShort_t)canal[CsI_L] );
		csi->GetACQParam("T")->SetData( (UShort_t)mt );
		 identifying_telescope = gIndra->GetIDTelescope(Form("SI_CSI_%02d%02d", ring, mod));
		return csi;
	}
	else if(ring>=10){
		KVCsI *csi = (KVCsI*) gIndra->GetDetectorByType(ring,mod,CsI_R);
		if(!csi) return 0;
		KVChIo *chio = gIndra->GetChIoOf( csi );
		if( !chio ) return 0;
		KVSi75 *si75 = (KVSi75*)gIndra->GetDetectorByType(ring,mod,Si75_GG);
		if( !si75 ) return 0;
		KVSiLi *sili = (KVSiLi*)gIndra->GetDetectorByType(ring,mod,SiLi_GG);
		if( !sili ) return 0;
		chio->SetEnergy( de1 );
		si75->SetEnergy( de4 );
		sili->SetEnergy( de5 );
		chio->GetACQParam("GG")->SetData( (UShort_t)canal[ChIo_GG] );
		chio->GetACQParam("PG")->SetData( (UShort_t)canal[ChIo_PG] );
		chio->GetACQParam("T")->SetData( (UShort_t)canal[ChIo_T] );
		si75->GetACQParam("GG")->SetData( (UShort_t)canal[Si75_GG] );
		si75->GetACQParam("PG")->SetData( (UShort_t)canal[Si75_PG] );
		si75->GetACQParam("T")->SetData( (UShort_t)canal[Si75_T] );
		sili->GetACQParam("GG")->SetData( (UShort_t)canal[SiLi_GG] );
		sili->GetACQParam("PG")->SetData( (UShort_t)canal[SiLi_PG] );
		sili->GetACQParam("T")->SetData( (UShort_t)canal[SiLi_T] );
		if(canal[CsI_R] > 0.){
			csi->SetEnergy( de3 );
			csi->GetACQParam("R")->SetData( (UShort_t)canal[CsI_R] );
			csi->GetACQParam("L")->SetData( (UShort_t)canal[CsI_L] );
			csi->GetACQParam("T")->SetData( (UShort_t)mt );
		 	identifying_telescope = gIndra->GetIDTelescope(Form("SILI_CSI_%02d%02d", ring, mod));
			return csi;
		} else {
		 	identifying_telescope = gIndra->GetIDTelescope(Form("SI75_SILI_%02d%02d", ring, mod));
			return sili;
		}						
	}
	return 0;
}
//_____________________________________________________________________________________________//
KVDetector* Code0(int ring, int mod)
{
//	cout << "Code0" << endl;
//code = 0 : gamma	
//  ** Mention speciale pour les Gammas :
// 
// 	Pour ceux-ci, on ne stocke que certaines infos disponibles :
// 
// 	 - Signal codeur CsI/Phoswich rapide  --> de3(i)  canaux en negatif
// 	 - Marqueur de temps --> Mt(i)	
	
 	if(ring==1 && !(gIndra->InheritsFrom("KVINDRA4")))
 	{
 		 //phoswich
 		 KVPhoswich *phos = (KVPhoswich*)gIndra->GetDetectorByType(ring,mod,Phos_R);
		 if(!phos) return 0;
 		 phos->GetACQParam("R")->SetData( (UShort_t)TMath::Abs(de3) );
 		 phos->GetACQParam("T")->SetData( (UShort_t)mt );
		 identifying_telescope = gIndra->GetIDTelescope(Form("PHOS_R_L_%02d", mod));
		 return phos;
 	}
	KVCsI *csi = (KVCsI*) gIndra->GetDetectorByType(ring,mod,CsI_R);
	if(csi){
		csi->GetACQParam("T")->SetData( (UShort_t)mt );
		csi->GetACQParam("R")->SetData( (UShort_t)TMath::Abs(de3) );
	}
	identifying_telescope = gIndra->GetIDTelescope(Form("CSI_R_L_%02d%02d", ring, mod));
	return csi;
}
//_____________________________________________________________________________________________//
KVDetector* Code1(int ring, int mod)
{
//	cout << "Code1" << endl;
//code = 1 : neutron (seulement couronnes 2 a 9)
	
	if(ring<2 || ring>9) return 0;
	
	KVCsI *csi = (KVCsI*) gIndra->GetDetectorByType(ring,mod,CsI_R);
	if(!csi) return 0;
    csi->SetEnergy( de3 );
    csi->GetACQParam("R")->SetData( (UShort_t)canal[CsI_R] );
    csi->GetACQParam("L")->SetData( (UShort_t)canal[CsI_L] );
    csi->GetACQParam("T")->SetData( (UShort_t)mt );
	identifying_telescope = gIndra->GetIDTelescope(Form("CSI_R_L_%02d%02d", ring, mod));
	return csi;
}
//_____________________________________________________________________________________________//
KVDetector* Code4and5and6and8(int ring, int mod)
{
//  code = 4 : fragment identifie dans ChIo-Si
//		            ou fragment ou part. identifie dans ChIo-Si75	
	//for 2nd campaign: code=4 => phoswich (ring 1) also
//  code = 5 : fragment non-ident. (montee de Bragg)(Z min.)
//                      ou stoppe dans Chio (Z min)
//code = 6 : frag. cree par la coherence CsI  (stoppe ds Si.)
// En general, lorsqu'il n'y a pas "multiple hit" dans un module
// INDRA, canal contient les valeurs bruts de codeurs. Dans le
// cas d'un "multiple hit" avec creation de particule , canal
// contient les valeurs estimees de la contribution CHIO ou SI
// obtenues par la coherence.
// code = 8 : multiple comptage dans ChIo avec arret
// 					- dans les siliciums (cour. 2-9)
// 					- dans les CsI (cour. 10-17)
// Nota: Pour le code 8 la contribution ChIo est partagee egalement entre
// 	  toutes les particules qui s'arretent dans les modules suivants.
// 	  Le Z individuel est donc surement faux mais cela permet
// 	  d'avoir une assez bonne estimation du Z.total.

	if(ring==1 && camp2)
 	{
 		 //phoswich
 		 KVPhoswich *phos = 
 			 (KVPhoswich*)gIndra->GetDetectorByType(ring,mod,Phos_R);
		 if(!phos) return 0;
 		 phos->SetEnergy( de1 );
 		 phos->GetACQParam("R")->SetData( (UShort_t)canal[Phos_R] );
 		 phos->GetACQParam("L")->SetData( (UShort_t)canal[Phos_L] );
 		 phos->GetACQParam("T")->SetData( (UShort_t)mt );
		 identifying_telescope = gIndra->GetIDTelescope(Form("PHOS_R_L_%02d", mod));
		 return phos;
 	}
 	else if(ring >=2 && ring <=9){
 		 //chio-si  	 
 		 KVSilicon *si = (KVSilicon*) gIndra->GetDetectorByType(ring,mod,Si_GG);
		 if(!si) return 0;
 		 KVChIo *chio = gIndra->GetChIoOf( si );
		 if(!chio) return 0;
 		 chio->SetEnergy( de1 );
 		 chio->GetACQParam("GG")->SetData( (UShort_t)canal[ChIo_GG] );
 		 chio->GetACQParam("PG")->SetData( (UShort_t)canal[ChIo_PG] );
 		 chio->GetACQParam("T")->SetData( (UShort_t)canal[ChIo_T] );
		 if(de2>0.0){
 		 	si->SetEnergy( de2 );
 		 	si->GetACQParam("GG")->SetData( (UShort_t)canal[Si_GG] );
 		 	si->GetACQParam("PG")->SetData( (UShort_t)canal[Si_PG] );
 		 	si->GetACQParam("T")->SetData( (UShort_t)canal[Si_T] );
			identifying_telescope = gIndra->GetIDTelescope(Form("CI_SI_%02d%02d", ring, mod));
 		 	return si;
		 } else
			return chio;
 	 }
 	 else if(ring>=10){
 		 //chio-csi or chio-si75
 		 KVCsI *csi = (KVCsI*) gIndra->GetDetectorByType(ring,mod,CsI_R);
		 if(!csi) return 0;
 		 KVChIo *chio = gIndra->GetChIoOf( csi );
		 if( !chio) return 0;
 		 KVSi75 *si75 = (KVSi75*)gIndra->GetDetectorByType(ring,mod,Si75_GG);
		 if(!si75) return 0;
 		 chio->SetEnergy( de1 );
 		 chio->GetACQParam("GG")->SetData( (UShort_t)canal[ChIo_GG] );
 		 chio->GetACQParam("PG")->SetData( (UShort_t)canal[ChIo_PG] );
 		 chio->GetACQParam("T")->SetData( (UShort_t)canal[ChIo_T] );
 		 if(canal[CsI_R] > 0.){
 			 csi->SetEnergy( de3 );
 			 csi->GetACQParam("R")->SetData( (UShort_t)canal[CsI_R] );
 			 csi->GetACQParam("L")->SetData( (UShort_t)canal[CsI_L] );
 			 csi->GetACQParam("T")->SetData( (UShort_t)mt );
			identifying_telescope = gIndra->GetIDTelescope(Form("CI_CSI_%02d%02d", ring, mod));
 			 return csi;
 		 } else if(de4>0.0) {
 			 si75->SetEnergy( de4 );
 			 si75->GetACQParam("GG")->SetData( (UShort_t)canal[Si75_GG] );
 			 si75->GetACQParam("PG")->SetData( (UShort_t)canal[Si75_PG] );
 			 si75->GetACQParam("T")->SetData( (UShort_t)canal[Si75_T] );
			identifying_telescope = gIndra->GetIDTelescope(Form("CI_SI75_%02d%02d", ring, mod));
 			 return si75;
 		 } else
			 return chio;	 
 	 }  	 
	 return 0;
}
//_____________________________________________________________________________________________//
KVDetector* Code7(int ring, int mod)
{
//		cout << "Code7" << endl;

//code = 7 : frag. cree par la coherence ChIo (stoppe ds ChIo)	
	
	KVChIo* chio = (KVChIo*)gIndra->GetDetectorByType(ring, mod, ChIo_GG);
	if(!chio) {
		//if chio not found, may be because particule has (ring,mod) corresponding
		//to a Si/CsI, and not those of a ChIo module
		//try CsI with given ring number and module
		KVCsI* csi = (KVCsI*)gIndra->GetDetectorByType(ring, mod, CsI_R);
		if(csi) chio=gIndra->GetChIoOf(csi);
		if(!chio) return 0;
	}
 	chio->SetEnergy( de1 );
 	chio->GetACQParam("GG")->SetData( (UShort_t)canal[ChIo_GG] );
 	chio->GetACQParam("PG")->SetData( (UShort_t)canal[ChIo_PG] );
 	chio->GetACQParam("T")->SetData( (UShort_t)canal[ChIo_T] );
	return chio;
}
