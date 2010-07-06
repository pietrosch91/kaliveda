//Created by KVClassFactory on Mon Jul  5 15:24:25 2010
//Author: bonnet

#include "KVSimReader_SMF_asym.h"
#include "KVString.h"

ClassImp(KVSimReader_SMF_asym)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimReader_SMF_asym</h2>
<h4>Read ascii file for asymptotic events of the SMF code after SIMON deexcitation</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimReader_SMF_asym::KVSimReader_SMF_asym()
{
   // Default constructor
	init();
}

KVSimReader_SMF_asym::KVSimReader_SMF_asym(KVString filename)
{
   init();
	if (!OpenReadingFile(filename)) return;
	ReadFile();
	CloseFile();
}


KVSimReader_SMF_asym::~KVSimReader_SMF_asym()
{
   // Destructor
}

void KVSimReader_SMF_asym::ReadFile(){

	evt = new KVSimEvent();
	nuc = 0;
	
	if (HasToFill()) DeclareTree();
	
	nevt=0;

	while (f_in.good()){
		if (ReadHeader()){
			for (Int_t nd=0; nd<nv->GetIntValue("ndesexcitation"); nd+=1){
				if (ReadEvent()){
					if (HasToFill()) FillTree();
				}
			}
		}
	}
	
	if (HasToFill())
		GetTree()->ResetBranchAddress(GetTree()->GetBranch("Simulated_evts"));
	
	delete evt;

	/*
	AddObjectToBeWrittenWithTree(h1);
	Int_t netot = nv->GetEntries();
	for (Int_t ne=0; ne<netot; ne+=1)	
		AddObjectToBeWrittenWithTree(nv->RemoveAt(0));
	*/
	Info("ReadFile","%d evts lus",nevt);


}


Bool_t KVSimReader_SMF_asym::ReadHeader(){

	KVString snom;
	Int_t res = ReadLine(1);
	switch (res){
	case 0:
		return kFALSE; 
	case 1:
		snom.Form("%s",GetReadPar(0).Data());
		snom.ReplaceAll("evt_","");
		Info("ReadHeader","lecture %d",snom.Atoi());
		nv->SetValue("event_number",snom.Atoi());
		
		delete toks;
		
		break;
	default:
		delete toks;
		return kFALSE;	
	}
	
	res = ReadLine(1);
	switch (res){
	case 0:
		return kFALSE; 
	case 1:
		nv->SetValue("ndesexcitation",GetIntReadPar(0));
		delete toks;
		ndes = 0;
		
		return kTRUE;
	default:
		delete toks;
		return kFALSE;	
	}


}

//-----------------------------------------

Bool_t KVSimReader_SMF_asym::ReadEvent(){

	evt->Clear();
	
	Int_t mult=0;
	Int_t res = ReadLine(1);
	switch (res){
	case 0:
		return kFALSE; 
	case 1:
		mult = GetIntReadPar(0);
		
		delete toks;
		break;
				
	default:
		delete toks;
		return kFALSE;	
	}
	
	evt->SetNumber(nv->GetIntValue("event_number"));
	evt->GetParameters()->SetValue("sub_number",ndes);
	for (Int_t mm=0; mm<mult; mm+=1){	
		nuc = (KVSimNucleus* )evt->AddParticle();
		if (!ReadNucleus()) return kFALSE;
	}

	nevt+=1;
	ndes+=1;
	
	return kTRUE;

}

//-----------------------------------------

Bool_t KVSimReader_SMF_asym::ReadNucleus(){
	
	Int_t res = ReadLine(5);
	switch (res){
	case 0:
		Info("ReadNucleus","case 0 line est vide"); 
		return kFALSE; 
	
	case 1:
		nuc->SetZ(GetIntReadPar(1));
		nuc->SetA(GetIntReadPar(0));
		
		//Axe "faisceau dans SMF z"
		nuc->SetPx(GetDoubleReadPar(2));
		nuc->SetPy(GetDoubleReadPar(3));
		nuc->SetPz(GetDoubleReadPar(4));
		
		delete toks;
		return kTRUE;
	
	default:
		
		delete toks;
		return kFALSE;	
	}

}
