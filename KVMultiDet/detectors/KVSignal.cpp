//Created by KVClassFactory on Mon Dec 22 15:46:46 2014
//Author: ,,,

#include "KVSignal.h"

ClassImp(KVSignal)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSignal</h2>
<h4>simple class to store TArray in a list</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSignal::KVSignal()
{
   // Default constructor
}

//________________________________________________________________

KVSignal::KVSignal(const char* name, const char* title) : TGraph(name, title)
{
   // Write your code here
}

//________________________________________________________________

KVSignal::KVSignal(const TString& name, const TString& title) : TGraph(name, title)
{
   // Write your code here
}

KVSignal::~KVSignal()
{
   // Destructor
}

//________________________________________________________________

void KVSignal::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVSignal::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   //TGraph::Copy((TGraph&)obj);
   //KVSignal& CastedObj = (KVSignal&)obj;
}

//________________________________________________________________

void KVSignal::SetData(Int_t nn, Double_t* xx, Double_t* yy)
{

	Set(nn);
   for (Int_t np=0;np<nn;np+=1)
   	SetPoint(np,xx[np],yy[np]);

}
