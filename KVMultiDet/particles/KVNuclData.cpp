//Created by KVClassFactory on Wed Feb 23 10:21:20 2011
//Author: bonnet

#include "KVNuclData.h"
#include "Riostream.h"

ClassImp(KVNuclData)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVNuclData</h2>
<h4>Simple abstract class to store value related to nuclear data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVNuclData::KVNuclData(const Char_t* name): fName(name)
{
   //Constructor with name
	init();

}

//_________________________________
KVNuclData::KVNuclData(): fName()
{
   // Default constructor
	init();
}

//_________________________________
KVNuclData::~KVNuclData()
{
   // Destructor
}

//_________________________________
void KVNuclData::init()
{
	
	SetMeasured(kTRUE);
	SetValue(0);
}


//_________________________________
const Char_t* KVNuclData::GetName() const
{

	return fName.Data();

}

//_________________________________
void KVNuclData::SetValue(Double_t val)
{

	kValue = val;

}

//_________________________________
Double_t KVNuclData::GetValue() const
{

	return kValue;

}

//_________________________________
const Char_t* KVNuclData::GetUnit() const{

	TString unit="NONE";
	return unit.Data();
}

//_________________________________
void KVNuclData::Print(Option_t* option) const{
	
	TString post=" ";
	if (!IsMeasured()) post += "(SYST)";
		
	cout << ClassName() << " : " << GetName() << " " << GetValue() << " " << GetUnit() << " " << post.Data() << endl;
}

