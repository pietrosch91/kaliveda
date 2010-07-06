//Created by KVClassFactory on Tue Jul  6 16:48:05 2010
//Author: Eric Bonnet

#include "KVSimReader_MMM.h"

ClassImp(KVSimReader_MMM)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimReader_MMM</h2>
<h4>Read ascii file for events of the MMM code at Freeze Out</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimReader_MMM::KVSimReader_MMM()
{
   // Default constructor
	init();
}

KVSimReader_MMM::KVSimReader_MMM(KVString filename)
{
   init();
	if (!OpenReadingFile(filename)) return;
	ReadFile();
	CloseFile();
}

KVSimReader_MMM::~KVSimReader_MMM()
{
   // Destructor
}

void KVSimReader_MMM::ReadFile(){

	evt = new KVSimEvent();
	nuc = 0;
	
	if (HasToFill()) DeclareTree();
	
	nevt=0;

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


Bool_t KVSimReader_MMM::ReadEvent(){

	evt->Clear();
	Int_t mult=0,nlus=0,natt=0;
	
	Int_t res = ReadLine(2);
	switch (res){
	case 0:
		return kFALSE; 
	default:
		nlus = toks->GetEntries();
		idx = 0;
		mult = GetIntReadPar(idx++);
		natt = 6*mult+4+1;
		if (natt!=nlus){
			Info("ReadEvent","Nombre de parametres (%d) different de celui attendu (%d)",nlus,natt);
			return kFALSE;
		}
		evt->GetParameters()->SetValue("Eth",GetDoubleReadPar(idx++));
		evt->GetParameters()->SetValue("Eclb",GetDoubleReadPar(idx++));
		evt->GetParameters()->SetValue("Eexci",GetDoubleReadPar(idx++));
		evt->GetParameters()->SetValue("Ebind",GetDoubleReadPar(idx++));
		
		for (Int_t mm=0; mm<mult; mm+=1){
			nuc = (KVSimNucleus* )evt->AddParticle();
			ReadNucleus();
		}
		delete toks;
		evt->SetNumber(nevt);
		nevt+=1;
		return kTRUE;
	}
	
}

Bool_t KVSimReader_MMM::ReadNucleus(){

	nuc->SetA(GetIntReadPar(idx++));
	nuc->SetZ(GetIntReadPar(idx++));
	nuc->SetExcitEnergy(GetDoubleReadPar(idx++));
	nuc->SetPx(GetDoubleReadPar(idx++));
	nuc->SetPy(GetDoubleReadPar(idx++));
	nuc->SetPz(GetDoubleReadPar(idx++));
	
	return kTRUE;

}
