//Created by KVClassFactory on Mon Dec 22 15:46:46 2014
//Author: ,,,

#include "KVSignal.h"
#include "KVString.h"

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

//________________________________________________________________
void KVSignal::DeduceFromName()
{

	fBlock = fQuartet = fTelescope = -1;
   fType = fDet = fTelName = fQuartetName = "";
	KVString tmp = GetName();
   KVString part = "";
   if (tmp.BeginsWith("B"))
   {
   	//FAZIA telescope denomination
   	tmp.Begin("-");
      part = tmp.Next(); part.ReplaceAll("B",""); fBlock = part.Atoi();
		part = tmp.Next(); part.ReplaceAll("Q",""); fQuartet = part.Atoi();
		part = tmp.Next(); part.ReplaceAll("T",""); fTelescope = part.Atoi();
		fType = tmp.Next();
      fDet = GetTitle();
      
      fTelName.Form("B%03d-Q%d-T%d",fBlock,fQuartet,fTelescope);
      fQuartetName.Form("B%03d-Q%d",fBlock,fQuartet);
   }
   
}
//________________________________________________________________
void KVSignal::Print(Option_t* chopt) const
{
	Info("Print","\nName: %s - Title: %s",GetName(),GetTitle());
	printf("\tBlock# %d - Quartet# %d - Telescope# %d\n",fBlock,fQuartet,fTelescope);
   printf("\tType: %s - Detecteur: %s\n",fType.Data(),fDet.Data());
   
}
//________________________________________________________________

KVPSAResult* KVSignal::TreateSignal(void)
{
	//to be implemented in child class
   Info("TreateSignal","To be implemented in child classes");
	return 0;
}

