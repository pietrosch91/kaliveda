//Created by KVClassFactory on Thu Feb 24 11:24:52 2011
//Author: bonnet

#include "KVNDTManager.h"
#include "KVNuclDataTable.h"
#include "KVNuclData.h"
#include "Riostream.h"
#include "TObjArray.h"

ClassImp(KVNDTManager)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVNDTManager</h2>
<h4>Allow to navigate between different tables of nuclear data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
KVNDTManager* gNDTManager;

KVNDTManager::KVNDTManager()
{
   // Default constructor
	SetOwner(kTRUE);
	init();
}

KVNDTManager::~KVNDTManager()
{
   // Destructor
}

void KVNDTManager::init()
{
	Arange = 0;
	Zrange = 0;
	
	Add((KVNuclDataTable* )TClass::GetClass("KVMassExcessTable")->New());
	Add((KVNuclDataTable* )TClass::GetClass("KVAbundanceTable")->New());
	Add((KVNuclDataTable* )TClass::GetClass("KVLifeTimeTable")->New());
	
	Execute("Initialize","");

}

KVNuclDataTable* KVNDTManager::GetTable(const Char_t* name) const
{

	return (KVNuclDataTable* )FindObject(name);

}

Bool_t KVNDTManager::IsInTable(Int_t zz, Int_t aa,const Char_t* name) const
{
	KVNuclDataTable* tab=0;
	return ( (tab = GetTable(name)) && tab->IsInTable(zz,aa) );

}

Double_t KVNDTManager::GetValue(Int_t zz, Int_t aa, const Char_t* name) const
{

	KVNuclDataTable* tab=0;
	if ( (tab = GetTable(name)) ) return tab->GetValue(zz,aa);
	return -666;

}

KVNuclData* KVNDTManager::GetData(Int_t zz, Int_t aa, const Char_t* name) const
{

	KVNuclDataTable* tab=0;
	if ( (tab = GetTable(name)) ) return tab->GetData(zz,aa);
	return 0;

}

Bool_t KVNDTManager::IsMeasured(Int_t zz, Int_t aa, const Char_t* name)const
{

	KVNuclDataTable* tab=0;
	if ( (tab = GetTable(name)) ) return tab->IsMeasured(zz,aa);
	return kFALSE;

}

const Char_t* KVNDTManager::GetUnit(Int_t zz, Int_t aa, const Char_t* name) const
{

	KVNuclDataTable* tab=0;
	if ( (tab = GetTable(name)) ) return tab->GetUnit(zz,aa);
	return "";


}



void KVNDTManager::Print(Option_t* option)const
{
	printf("--------\n");
	Info("Print","%d Available nuclear data tables",GetEntries());
	for (Int_t ii=0;ii<GetEntries();ii+=1){
		printf("name=%s file=%s number of nuclei stored=%d\n",
			((KVNuclDataTable* )At(ii))->GetName(),
			((KVNuclDataTable* )At(ii))->GetTitle(),
			((KVNuclDataTable* )At(ii))->GetNumberOfNuclei()
		);
	}
	printf("--------\n");

}
