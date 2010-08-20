//Created by KVClassFactory on Wed Jun 30 17:45:01 2010
//Author: bonnet

#ifndef __KVSIMREADER_H
#define __KVSIMREADER_H

#include "KVBase.h"
#include "Riostream.h"
#include "KVSimEvent.h"
#include "KVSimNucleus.h"
#include "KVString.h"
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "KVList.h"
#include "TObject.h"
#include "TDatime.h"
#include "TStopwatch.h"
#include "KVFileReader.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TDirectory.h"


class KVSimReader : public KVFileReader
{

   protected:

	TTree* tree;//!
	TFile* file;//!
	KVSimEvent* evt;//!
	KVSimNucleus* nuc;//!
	
	Int_t nevt;
	KVString tree_name,branch_name,root_file_name;
	Bool_t kmode;
	
	//Liste de Variables specifiques a une simulation
	//
	KVNameValueList* linked_info; //Liste de variables specifiques (TNamed) a une simulation
											//permettant de stocker des informations pouvant être reutilisées
	
	KVNameValueList* nv; 	//Liste de variables specifiques (TNamed) a une simulation
	KVList* linked_objects;	//liste d'objets a enregistrer avec l'arbre
	
	public:
   
	KVSimReader();
	KVSimReader(KVString filename);
   virtual ~KVSimReader();
	
	void init(){
		Info("init","rentre");
		
		kmode=kTRUE;
		tree = 0;
		evt = 0;
		nuc = 0 ;
		nevt = 0;
		
		tree_name = "SIMULATION_NAME";
		root_file_name = "Output.root";
		//ascii_file_name = "";
		branch_name = "Simulated_evts";
		
		CreateObjectList();
		CreateInfoList();
		
		nv = new KVNameValueList();				
		nv->SetName("Temporary List");
	}
	
	virtual KVString GetDate(){
		TDatime now;
		KVString stime;
		stime.Form("%d_%0.2d_%0.2d_%0.2d:%0.2d:%0.2d",
			now.GetYear(),
			now.GetMonth(),
			now.GetDay(),
			now.GetHour(),
			now.GetMinute(),
			now.GetSecond()
		);
		return stime;	
	
	}
	
	
	void SetFillingMode(Bool_t mode=kTRUE){ kmode = mode;}
	void virtual DeclareTree(KVString filename,Option_t* option){	
		
		Info("DeclareTree","Ouverture du fichier de stockage %s",filename.Data());
		root_file_name = filename;
		file = new TFile(root_file_name.Data(),option);
		
		tree = new TTree(tree_name.Data(),"SIMULATION");
		tree->Branch(branch_name.Data(), "KVSimEvent", &evt, 10000000, 0)->SetAutoDelete(kFALSE);
	}
	TTree* GetTree() { return tree; }
	virtual void FillTree(){ GetTree()->Fill(); }
	virtual Bool_t HasToFill(){ return kmode; }
	virtual void SaveTree(){
		
		Info("SaveTree","Ecriture de l arbre");

		WriteObjects();
		WriteInfo();

		GetTree()->Write();
		//gDirectory->Delete("*");
		
		CleanAll();
		
		file->Close();
		Info("SaveTree","Fin de Routine");
	
	}
	
	void Run(KVString filename="Output.root",Option_t* option = "recreate"){
		
		AddInfo("ascii file read",GetFileName().Data());
		
		TStopwatch chrono;
		chrono.Start();
		
		evt = new KVSimEvent();
		if (HasToFill()) DeclareTree(filename,option);
		nuc = 0;
		nevt=0;
		
		ReadFile();
		
		if (HasToFill())
			GetTree()->ResetBranchAddress(GetTree()->GetBranch(branch_name.Data()));
	
		delete evt;
		chrono.Stop();
		
		Info("Run","%d evts lus en %lf seconds",GetNumberOfEvents(),chrono.RealTime());
		
		KVString snevt; snevt.Form("%d",nevt);
		AddInfo("number of events read",snevt.Data());
		AddInfo("date",GetDate().Data());
		AddInfo("user",gSystem->GetUserInfo()->fUser.Data());
		
		Info("Run","To store the tree call SaveTree method ...");
	
	}
	
	//------------------
	KVList* GetLinkedObjects(){
		return linked_objects;
	}
	void AddObject(TObject* obj){
		Info("AddObject","Ajout %s",obj->GetName());
		GetLinkedObjects()->Add(obj);  
	}
	
	void WriteObjects(){
		
		TList* list = GetTree()->GetUserInfo();
		list->Add(new KVList());
		KVList* ltamp = (KVList* )list->Last();
		ltamp->SetName(GetLinkedObjects()->GetName());
		Int_t ne = GetLinkedObjects()->GetEntries(); 
		for (Int_t kk=0;kk<ne;kk+=1)	ltamp->Add(GetLinkedObjects()->RemoveAt(0));
			
	}
	void CreateObjectList(){
	
		linked_objects = new KVList(kFALSE);	
		linked_objects->SetName("List of objects");
	}
	
	//------------------
	KVNameValueList* GetSimuInfo(){
		return linked_info;
	}
	void AddInfo(const Char_t* name, const Char_t* val){
		Info("AddInfo","Ajout %s %s",name,val);
		GetSimuInfo()->SetValue(name,val); 
	}
	void AddInfo(TNamed* named){
		AddInfo(named->GetName(),named->GetTitle()); 
	}
	void WriteInfo(){
		
		TList* list = GetTree()->GetUserInfo();
		list->Add(new KVNameValueList());
		KVNameValueList* ltamp = (KVNameValueList* )list->Last();
		ltamp->SetName(GetSimuInfo()->GetName());
		Int_t ne = GetSimuInfo()->GetEntries(); 
		for (Int_t kk=0;kk<ne;kk+=1)	ltamp->Add(GetSimuInfo()->RemoveAt(0));
		
	}
	void CreateInfoList(){
	
		linked_info = new KVNameValueList();	
		linked_info->SetOwner(kFALSE); 
		linked_info->SetName("Simulation info");
	
	}

	virtual void ReadFile();
	virtual Bool_t ReadHeader();
	virtual Bool_t ReadEvent();
	virtual Bool_t ReadNucleus();

	Int_t GetNumberOfEvents(){
		return nevt;
	}
	
	void CleanAll(){
		TList* list = GetTree()->GetUserInfo();
		KVList* kl = (KVList* )list->FindObject("List of objects");
		for (Int_t nn=0; nn<kl->GetEntries(); nn+=1) gDirectory->Delete(kl->At(nn)->GetName());
	}
	
   ClassDef(KVSimReader,1)	//Base class to read output files for simulation and create tree using KVEvent type class
};

#endif
