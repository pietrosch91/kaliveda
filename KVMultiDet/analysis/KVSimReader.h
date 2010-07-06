//Created by KVClassFactory on Wed Jun 30 17:45:01 2010
//Author: bonnet

#ifndef __KVSIMREADER_H
#define __KVSIMREADER_H

#include "KVBase.h"
#include "Riostream.h"
#include "KVSimEvent.h"
#include "KVSimNucleus.h"
#include "KVString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "KVList.h"
#include "TObject.h"


class KVSimReader : public KVBase
{

   protected:
	TTree* tree;//!
	KVSimEvent* evt;//!
	KVSimNucleus* nuc;//!
	
	TObjArray* toks;//!
	Int_t nevt;
	KVString tree_name,file_name,branch_name;
	Bool_t kmode;
	
	ifstream f_in;
	
	//Liste de Variables specifiques a une simulation
	//
	KVNameValueList* nv; //Liste de variables specifiques a une simulation
								//permettant de stocker des informations pouvant être reutilisées
	
	KVList* linked_objects;//liste d'objets a enregistrer avec l'arbre
	
	public:
   
	KVSimReader();
	KVSimReader(KVString filename);
   virtual ~KVSimReader();
	
	virtual void init(){
	
		kmode=kTRUE;
		tree = 0;
		evt = 0;
		nuc = 0 ;
		toks = 0;
		nevt = 0;
		
		file_name = "";
		tree_name = "SIMULATION_NAME";
		branch_name = "Simulated_evts";
		
		nv = new KVNameValueList();
		linked_objects = new KVList(kFALSE);
		
	}
	
	virtual Bool_t OpenReadingFile(KVString filename){
	
		file_name = filename;
		
		f_in.open(filename.Data());
		Bool_t status = f_in.good();
		
		if (!status)
			Info("OpenReadingFile","Echec dans l ouverture du fichier %s",filename.Data());
		
		AddObjectToBeWrittenWithTree(new TNamed("ascii file read",filename.Data()));
		
		return status;
	
	}
	
	void AddObjectToBeWrittenWithTree(TObject* obj){ 
		Info("AddObjectToBeWrittenWithTree","Ajout %s",obj->GetName());
		GetLinkedObjects()->Add(obj);  
	}
	
	virtual void CloseFile(){ f_in.close(); }
	
	void SetFillingMode(Bool_t mode=kTRUE){ kmode = mode;}
	
	void virtual DeclareTree(){	
		
		tree = new TTree(tree_name.Data(),"SIMULATION");
		tree->Branch(branch_name.Data(), "KVSimEvent", &evt, 10000000, 0)->SetAutoDelete(kFALSE);
	
	}
	
	TTree* GetTree() { return tree; }
	virtual void FillTree(){ GetTree()->Fill(); }
	virtual Bool_t HasToFill(){ return kmode; }
	virtual void SaveTree(KVString filename){
	
		TFile* file = new TFile(filename.Data(),"recreate");
		
		TList* list = GetTree()->GetUserInfo();
		Int_t no = GetLinkedObjects()->GetEntries();
		for (Int_t nn=0;nn<no;nn+=1){
			list->Add(GetLinkedObjects()->RemoveAt(0));
		}
		GetTree()->Write();
		file->Close();
	
	}
	
	KVList* GetLinkedObjects(){
		return linked_objects;
	}
	
	virtual void ReadFile();
	virtual Bool_t ReadHeader();
	virtual Bool_t ReadEvent();
	virtual Bool_t ReadNucleus();
	
	Int_t ReadLine(Int_t number,KVString pattern=" "){
	
		KVString line;
		line.ReadLine(f_in);
		//Info("ReadLine","%s",line.Data());
		if (line=="") { return 0;}
		toks = line.Tokenize(pattern);
		if (toks->GetEntries()==number) { return 1; }
		else { return 2; }
	}
	
	void AddReadLine(KVString pattern=" "){
	
		KVString line;
		line.ReadLine(f_in);
		toks->AddAll(line.Tokenize(pattern));
	
	}
	
	Double_t GetDoubleReadPar(Int_t pos){
		return GetReadPar(pos).Atof();
	}
	Int_t GetIntReadPar(Int_t pos){
		return GetReadPar(pos).Atoi();
	}
	TString GetReadPar(Int_t pos){
		return ((TObjString*)toks->At(pos))->GetString();
	}
	
	
   ClassDef(KVSimReader,1)	//Read/Write output/input files for simulation
};

#endif
