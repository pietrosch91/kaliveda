//Created by KVClassFactory on Tue Jan 13 15:11:11 2015
//Author: ,,,

#include "KVCurrentSignal.h"
#include "TMath.h"

ClassImp(KVCurrentSignal)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVCurrentSignal</h2>
<h4>digitized intensity signal</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVCurrentSignal::KVCurrentSignal()
{
   // Default constructor
}

//________________________________________________________________

KVCurrentSignal::KVCurrentSignal(const char* name) : KVSignal(name, "Current")
{
   // Write your code here
}

//________________________________________________________________

KVCurrentSignal::~KVCurrentSignal()
{
   // Destructor
}

//________________________________________________________________

void KVCurrentSignal::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVCurrentSignal::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVSignal::Copy(obj);
   //KVCurrentSignal& CastedObj = (KVCurrentSignal&)obj;
}

void KVCurrentSignal::SetDefaultValues()
{
    fChannelWidth = 4.;
    fFirstBL = 0;
    fLastBL  = 30;
    fTauRC = -1;
    fTrapRiseTime = -1;
    fTrapFlatTop  = -1;
    fGaussSigma   = -1;
}

KVPSAResult *KVCurrentSignal::TreateSignal(Bool_t with_pole_zero_correction)
{
    //to be implemented in child class
   KVPSAResult* psa = new KVPSAResult(GetName());

   Init();

   fBaseLine  = FindMedia(fFirstBL, fLastBL);
   fSigmaBase = FindSigma2(fFirstBL, fLastBL);

   Add(-1.*fBaseLine);
   ComputeAmplitude();

   // storing result
   psa->SetValue(Form("%s.%s.BaseLine",fDetName.Data(),fType.Data()),fBaseLine);
   psa->SetValue(Form("%s.%s.SigmaBaseLine",fDetName.Data(),fType.Data()),TMath::Sqrt(fSigmaBase));
   psa->SetValue(Form("%s.%s.Amplitude",fDetName.Data(),fType.Data()),fAmplitude);

   return psa;

}
