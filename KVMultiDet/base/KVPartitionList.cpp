//Created by KVClassFactory on Fri Oct  8 14:27:07 2010
//Author: bonnet

#include "KVPartitionList.h"
#include "KVPartition.h"
#include "KVIntegerList.h"
#include "TTree.h"

ClassImp(KVPartitionList)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVPartitionList</h2>
<h4>Cherche si un objet KVIntegerList est deja dans la liste, si non il l'enregistre
si oui, il incremente la population de celui déjà présent AddPopulation()
</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVPartitionList::init()
{
	//Initialisation
	atrouve=kFALSE;
	knbre_diff=0;
	knbre_tot=0;
	SetOwner(kTRUE);
	mult_range = new KVPartition();

}

//_______________________________________________________
KVPartitionList::KVPartitionList()
{
   // Default constructor
	init();
}

//_______________________________________________________
KVPartitionList::KVPartitionList(const Char_t* name)
{
   // constructor with name
	init();
	SetName(name);
}

//_______________________________________________________
KVPartitionList::~KVPartitionList()
{
   // Destructor
	delete mult_range;
}

//_______________________________________________________
void KVPartitionList::Clear(Option_t* option)
{
	KVSeqCollection::Clear(option);
	mult_range->Clear();
	knbre_diff=0;
	knbre_tot=0;

}
//_______________________________________________________
Bool_t KVPartitionList::IsInTheList()
{
	
	return atrouve;

}

//_______________________________________________________
Double_t KVPartitionList::GetNbreTot()
{
	
	return knbre_tot;
	
}	

//_______________________________________________________
Double_t KVPartitionList::GetNbreDiff()
{
	
	return knbre_diff;
	
}
	
//_______________________________________________________
Bool_t KVPartitionList::Fill(KVIntegerList* par){

	knbre_tot += 1.;
	Add(par);

	return IsInTheList();

}
	
//_______________________________________________________
void KVPartitionList::AddFirst(TObject *obj)
{
	// Add an object to the list if it is not already in it
	// (no object with same name in list)
	// if it is in, the population of it is incremented

   TObject* find=0; 
	if( !(find = FindObject(obj->GetName())) ) {
		KVHashList::AddFirst(obj);
		ValidateEntrance((KVIntegerList* )obj);
	}
	else { 
		atrouve=kTRUE; 
		((KVIntegerList* )find)->AddPopulation(1);
	} 
}

//_______________________________________________________
void KVPartitionList::AddLast(TObject *obj)
{
	// Add an object to the list if it is not already in it
	// (no object with same name in list)
	// if it is in, the population of it is incremented
    
   TObject* find=0; 
	if( !(find = FindObject(obj->GetName())) ) {
		KVHashList::AddLast(obj);
		ValidateEntrance((KVIntegerList* )obj);
	}
	else { 
		atrouve=kTRUE; 
		((KVIntegerList* )find)->AddPopulation(1);
	} 
}

//_______________________________________________________
void KVPartitionList::AddAt(TObject *obj, Int_t idx)
{
	// Add an object to the list if it is not already in it
	// (no object with same name in list)
	// if it is in, the population of it is incremented

   TObject* find=0; 
	if( !(find = FindObject(obj->GetName())) ) {
		KVHashList::AddAt(obj,idx);
		ValidateEntrance((KVIntegerList* )obj);
	}
	else { 
		atrouve=kTRUE; 
		((KVIntegerList* )find)->AddPopulation(1);
	} 
}

//_______________________________________________________
void KVPartitionList::AddAfter(const TObject *after, TObject *obj)
{
	// Add an object to the list if it is not already in it
	// (no object with same name in list)
	// if it is in, the population of it is incremented
	
   TObject* find=0; 
	if( !(find = FindObject(obj->GetName())) ) {
		KVHashList::AddAfter(after,obj);
		ValidateEntrance((KVIntegerList* )obj);
	}
	else { 
		atrouve=kTRUE; 
		((KVIntegerList* )find)->AddPopulation(1);
	} 
}

//_______________________________________________________
void KVPartitionList::AddBefore(const TObject *before, TObject *obj)
{
	// Add an object to the list if it is not already in it
	// (no object with same name in list)
	// if it is in, the population of it is incremented
	
   TObject* find=0; 
	if( !(find = FindObject(obj->GetName())) ) {
		KVHashList::AddBefore(before,obj);
		ValidateEntrance((KVIntegerList* )obj);
	}
	else { 
		atrouve=kTRUE; 
		((KVIntegerList* )find)->AddPopulation(1);
	} 
}

//_______________________________________________________
void KVPartitionList::Add(TObject *obj)
{
	// Add an object to the list if it is not already in it
	// (no object with same name in list)
	// Info("Add","%s",obj->GetName());
	// if it is in, the population of it is incremented

   TObject* find=0; 
	if( !(find = FindObject(obj->GetName())) ) {
		KVHashList::Add(obj);
		ValidateEntrance((KVIntegerList* )obj);
	}
	else { 
		atrouve=kTRUE; 
		((KVIntegerList* )find)->AddPopulation(1);
	}
}

//_______________________________________________________
void KVPartitionList::ValidateEntrance(KVIntegerList* il)
{
	atrouve=kFALSE;
	knbre_diff+=1;
	mult_range->Add(il->GetNbre());
}

//_______________________________________________________
void KVPartitionList::Update()
{

	mult_range->CheckForUpdate();
	mult_range->Print("Partitions");

}

//_______________________________________________________
TTree* KVPartitionList::GenereTree(const Char_t* treename,Bool_t Compress)
{
	//Protected method	
	if (GetEntries()==0) return 0;
	
	Int_t mmax = Int_t(mult_range->GetZmax(0));
	Info("GenereTree","Multiplicite max entregistree %d",mmax);
	Int_t* tabz = new Int_t[mmax];
	Int_t mtot;
	Int_t pop;
	Info("GenereTree","Nbre de partitions entregistrees %d",GetEntries());
	
	TTree* tree = new TTree(treename,Class_Name());
	tree->Branch("mtot",			&mtot,	"mtot/I");
	tree->Branch("tabz",			tabz,		"tabz[mtot]/I");
	if (Compress)
		tree->Branch("pop",	&pop,		"pop/I");
	
	KVIntegerList* par;
	TArrayI* table = 0;
	for (Int_t kk=0;kk<GetEntries();kk+=1){
		par = (KVIntegerList* )At(kk);
		table = par->CreateTArrayI();
		
		mtot = par->GetNbre();
		if (kk%5000==0)
			Info("GenereTree","%d partitions traitees",kk);
		pop = par->GetPopulation();
		
		for (Int_t mm=0;mm<mtot;mm+=1)
			tabz[mm] = table->At(mm);
		
		if (Compress){ tree->Fill(); }
		else{
			for (Int_t pp=0; pp<pop; pp+=1)  tree->Fill();
		}
		delete table;	
	}
	Info("GenereTree","Fin du remplissage");
	tree->ResetBranchAddresses();
	delete [] tabz;
	return tree;

}

//_______________________________________________________
void KVPartitionList::SaveAsTree(const Char_t* filename,const Char_t* treename,Bool_t Compress,Option_t* option)
{
	TFile* file = new TFile(filename,option);
	GenereTree(treename)->Write();
	file->Close();
}

