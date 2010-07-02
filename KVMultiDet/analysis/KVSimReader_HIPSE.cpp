//Created by KVClassFactory on Fri Jul  2 15:16:15 2010
//Author: bonnet

#include "KVSimReader_HIPSE.h"

ClassImp(KVSimReader_HIPSE)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimReader_HIPSE</h2>
<h4>Reading class of HIPSE generator</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimReader_HIPSE::KVSimReader_HIPSE()
{
   // Default constructor
	init();
}

KVSimReader_HIPSE::KVSimReader_HIPSE(KVString filename)
{
   init();
	if (!OpenReadingFile(filename)) return;
	ReadFile();
	CloseFile();
}

/*
KVSimReader_HIPSE::KVSimReader_HIPSE(KVString filename):KVSimReader(filename)
{
   init();
}
*/
KVSimReader_HIPSE::~KVSimReader_HIPSE()
{
   // Destructor
}


void KVSimReader_HIPSE::ReadFile(){

	evt = new KVSimEvent();
	nuc = 0;
	
	if (HasToFill()) DeclareTree();
	
	nevt=0;

	if (!ReadHeader()) return;

	while (f_in.good()){
		while (ReadEvent()){
			if (HasToFill()) FillTree();
		}
	}	
	
	if (HasToFill())
		GetTree()->ResetBranchAddress(GetTree()->GetBranch("Simulated_evts"));
	
	delete evt;

	Info("ReadFile","%d evts lus",nevt);
	
}

Bool_t KVSimReader_HIPSE::ReadHeader(){
	
	Int_t res = ReadLine(2);
	switch (res){
	case 0:
		return kFALSE; 
	case 1:
		nv->SetValue("Aproj",GetDoubleReadPar(0));
		nv->SetValue("Zproj",GetDoubleReadPar(1));
		delete toks;
		break;
	default:
		delete toks;
		return kFALSE;	
	}
	
	res = ReadLine(2);
	switch (res){
	case 0:
		return kFALSE; 
	case 1:
		nv->SetValue("Atarg",GetDoubleReadPar(0));
		nv->SetValue("Ztarg",GetDoubleReadPar(1));
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
		nv->SetValue("Ebeam",GetDoubleReadPar(0));
		delete toks;
		return kTRUE;
	default:
		delete toks;
		return kFALSE;	
	}

}

Bool_t KVSimReader_HIPSE::ReadEvent(){

	evt->Clear();
	Int_t mult=0,mtotal=0;
	Int_t res = ReadLine(2);
	switch (res){
	case 0:
		return kFALSE; 
	case 1:
		evt->SetNumber(nevt);
		mult = GetIntReadPar(0);
		mtotal = GetIntReadPar(1);
		evt->GetParameters()->SetValue("mtotal",mtotal);
		delete toks;
		break;
	default:
		delete toks;
		return kFALSE;	
	}
	
	res = ReadLine(3);
	switch (res){
	case 0:
		return kFALSE; 
	case 1:
		evt->GetParameters()->SetValue("exci",GetDoubleReadPar(0));
		evt->GetParameters()->SetValue("vCM",GetDoubleReadPar(1));
		evt->GetParameters()->SetValue("impact_par",GetDoubleReadPar(2));
		delete toks;
		break;
	default:
		delete toks;
		return kFALSE;	
	}	
	
	res = ReadLine(3);
	switch (res){
	case 0:
		return kFALSE; 
	case 1:
		delete toks;
		break;
	default:
		delete toks;
		return kFALSE;	
	}	
	
	evt->SetNumber(nevt);
	for (Int_t mm=0; mm<mult; mm+=1){	
		nuc = (KVSimNucleus* )evt->AddParticle();
		if (!ReadNucleus()) return kFALSE;
	}
	
	/*
	evt->GetParameters()->Print();
	evt->Print();
	*/
		
	nevt+=1;
	return kTRUE;
	
}

Bool_t KVSimReader_HIPSE::ReadNucleus(){

	Int_t res = ReadLine(3);
	switch (res){
	case 0:
		Info("ReadNucleus","case 0 line est vide"); 
		return kFALSE; 
	
	case 1:
		
		nuc->SetA(GetIntReadPar(0));
		nuc->SetZ(GetIntReadPar(1));
		nuc->GetParameters()->SetValue("Provenance",GetDoubleReadPar(2));
		
		delete toks;
		break;

	default:
		
		delete toks;
		return kFALSE;	
	}

	res = ReadLine(3);
	switch (res){
	case 0:
		Info("ReadNucleus","case 0 line est vide"); 
		return kFALSE; 
	
	case 1:
		//Axe "faisceau dans HIPSE x -> on effectue une rotation X,Y,Z -> Y,Z,X"
		nuc->SetPx(GetDoubleReadPar(1));
		nuc->SetPy(GetDoubleReadPar(2));
		nuc->SetPz(GetDoubleReadPar(0));
		
		delete toks;
		return kTRUE;
	
	default:
		
		delete toks;
		return kFALSE;	
	}
	
	
}
