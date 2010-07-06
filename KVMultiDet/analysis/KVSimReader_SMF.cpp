//Created by KVClassFactory on Mon Jul  5 16:21:41 2010
//Author: bonnet

#include "KVSimReader_SMF.h"
#include "KVString.h"

ClassImp(KVSimReader_SMF)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimReader_SMF</h2>
<h4>Read ascii file for events of the SMF code after clusterization</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimReader_SMF::KVSimReader_SMF()
{
   // Default constructor
	init();
}

KVSimReader_SMF::KVSimReader_SMF(KVString filename)
{
   init();
	if (!OpenReadingFile(filename)) return;
	ReadFile();
	CloseFile();
}

KVSimReader_SMF::~KVSimReader_SMF()
{
   // Destructor
}


void KVSimReader_SMF::ReadFile(){

	evt = new KVSimEvent();
	nuc = 0;
	
	if (HasToFill()) DeclareTree();
	
	nevt=0;

	while (f_in.good()){
		if (ReadHeader() && ReadEvent()){
			if (HasToFill()) FillTree();
		}
	}
	
	if (HasToFill())
		GetTree()->ResetBranchAddress(GetTree()->GetBranch("Simulated_evts"));
	
	delete evt;

	Int_t netot = nv->GetEntries();
	for (Int_t ne=0; ne<netot; ne+=1)	
		AddObjectToBeWrittenWithTree(nv->RemoveAt(0));
	
	Info("ReadFile","%d evts lus",nevt);

}


Bool_t KVSimReader_SMF::ReadHeader(){

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
		
		return kTRUE;
	default:
		delete toks;
		return kFALSE;	
	}

}

//-----------------------------------------

Bool_t KVSimReader_SMF::ReadEvent(){

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
	Info("ReadEvent","       Lecture evt %d -> mult %d",evt->GetNumber(),mult);
	for (Int_t mm=0; mm<mult; mm+=1){	
		nuc = (KVSimNucleus* )evt->AddParticle();
		if (!ReadNucleus()) return kFALSE;
	}

	nevt+=1;
	
	return kTRUE;

}

//-----------------------------------------

Bool_t KVSimReader_SMF::ReadNucleus(){

	Int_t res = ReadLine(6);
	switch (res){
	case 0:
		Info("ReadNucleus","case 0 line est vide"); 
		return kFALSE; 
	
	default:
	
		AddReadLine();
		if (toks->GetEntries()!=12){
			Info("ReadNucleus","Nombre de parametres (%d) different de celui attendu(%d)",toks->GetEntries(),12);
			return kFALSE;	
		}
		nuc->SetZ(TMath::Nint(GetDoubleReadPar(1)));
		nuc->SetA(TMath::Nint(GetDoubleReadPar(0)));
		nuc->GetParameters()->SetValue("density",GetDoubleReadPar(2));
		nuc->SetExcitEnergy(GetDoubleReadPar(3));
		nuc->GetParameters()->SetValue("spin",GetDoubleReadPar(4));
		//Axe "faisceau dans SMF z"
		nuc->SetPx(GetDoubleReadPar(5));
		nuc->SetPy(GetDoubleReadPar(6));
		nuc->SetPz(GetDoubleReadPar(7));
		nuc->SetPosition(GetDoubleReadPar(8),GetDoubleReadPar(9),GetDoubleReadPar(10));
		nuc->GetParameters()->SetValue("avoir",GetDoubleReadPar(11));
		delete toks;
		return kTRUE;
	}
	
}
