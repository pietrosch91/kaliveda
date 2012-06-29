//Created by KVClassFactory on Tue Jan 17 15:10:46 2012
//Author: bonnet

#include "KVDalitzPlot.h"
#include "TMath.h"

ClassImp(KVDalitzPlot)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVDalitzPlot</h2>
<h4>Fill 3D observables {a1,a2,a3} in a dalitz plot</h4>
, a1+a2+a3 = 1
and fill in a equilateral triangle where the distance from each side of the triangle represent the value of each componment</h4>

<!-- */
// --> END_HTML
/*
- Normalize first the sum to 1 => a1+a2+a3 = 1
- Deduce the position in an equilateral triangle as following
a1n -> distance with the bottom border	  /\
a2n -> distance with the left border	 /  \
a3n -> distance with the right border	/____\ 
*/
////////////////////////////////////////////////////////////////////////////////
KVDalitzPlot::KVDalitzPlot(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup) :
TH2F(name,title,nbinsx, xlow,xup,nbinsy,ylow,yup)
{
	
}


KVDalitzPlot::KVDalitzPlot()
{
   // Default constructor
}

//________________________________________________________________

KVDalitzPlot::KVDalitzPlot (const KVDalitzPlot& obj) 
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVDalitzPlot::~KVDalitzPlot()
{
   // Destructor
}

//________________________________________________________________

void KVDalitzPlot::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVDalitzPlot::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   TH2F::Copy(obj);
   //KVDalitzPlot& CastedObj = (KVDalitzPlot&)obj;
}

Int_t KVDalitzPlot::FillAsDalitz(Double_t a1,Double_t a2,Double_t a3)
{
	//Place the 3 observable in an equilateral triangle
	//after normalized the sum to 1
	//Info : the permutation is not done 
	
	Double_t sum = a1+a2+a3;
	if (sum>0){
		Double_t a1n = a1/sum;
		Double_t a2n = a2/sum;
	
		Double_t xI = 1./TMath::Sqrt(3) * (a1n+2*a2n); 
		Double_t yI = a1n;

		return TH2F::Fill(xI,yI);
	}
	return -1;
}
