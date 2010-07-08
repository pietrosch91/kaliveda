//Created by KVClassFactory on Fri Jul  2 15:16:15 2010
//Author: bonnet

#include "KVSimReader_HIPSE_asym.h"
#include "KV2Body.h"

ClassImp(KVSimReader_HIPSE_asym)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimReader_HIPSE_asym</h2>
<h4>Read ascii file for asymptotic events of the HIPSE code after SIMON deexcitation</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimReader_HIPSE_asym::KVSimReader_HIPSE_asym()
{
   // Default constructor
	init();
}

KVSimReader_HIPSE_asym::KVSimReader_HIPSE_asym(KVString filename)
{
   init();
	if (!OpenReadingFile(filename)) return;
	Run();
	CloseFile();
}

KVSimReader_HIPSE_asym::~KVSimReader_HIPSE_asym()
{
   // Destructor
	if (h1) delete h1; h1 = 0;
}


void KVSimReader_HIPSE_asym::ReadFile(){

	h1 = new TH1F("impact_parameter","distri",200,0,20);
	
	if (!ReadHeader()) return;

	while (f_in.good()){
		while (ReadEvent()){
			h1->Fill(evt->GetParameters()->GetDoubleValue("Bparstore"));
			if (HasToFill()) FillTree();
		}
	}	
	
	
	AddObjectToBeWrittenWithTree(h1);
	Int_t netot = nv->GetEntries();
	for (Int_t ne=0; ne<netot; ne+=1)	
		AddObjectToBeWrittenWithTree(nv->RemoveAt(0));

}

Bool_t KVSimReader_HIPSE_asym::ReadHeader(){
	
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
		return kTRUE;
		
	
	default:
		delete toks;
		return kFALSE;	
	}

}

Bool_t KVSimReader_HIPSE_asym::ReadEvent(){

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
		evt->GetParameters()->SetValue("mult",mtotal);
		delete toks;
		break;
	default:
		delete toks;
		return kFALSE;	
	}

/*---------------------------------------------
	Esa		 = excitation per nucleon
	vcm		 = center of mass energy
	Bparstore = impact parameter 
//---------------------------------------------      	 
*/	
	
	res = ReadLine(3);
	switch (res){
	case 0:
		return kFALSE; 
	case 1:
		evt->GetParameters()->SetValue("Esa",GetDoubleReadPar(0));
		evt->GetParameters()->SetValue("vcm",GetDoubleReadPar(1));
		evt->GetParameters()->SetValue("Bparstore",GetDoubleReadPar(2));
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

	nevt+=1;
	return kTRUE;
	
}

Bool_t KVSimReader_HIPSE_asym::ReadNucleus(){

	Int_t res = ReadLine(3);
	switch (res){
	case 0:
		Info("ReadNucleus","case 0 line est vide"); 
		return kFALSE; 
	
	case 1:
		/*
		proven = 0 -> fusion of the QP and QT
		proven = 1 -> QP
		proven = 2 -> QT
		proven > 2 -> other
		*/
		
		nuc->SetA(GetIntReadPar(0));
		nuc->SetZ(GetIntReadPar(1));
		nuc->GetParameters()->SetValue("proven",GetDoubleReadPar(2));
		
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
