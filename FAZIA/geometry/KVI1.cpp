//Created by KVClassFactory on Tue Jan 13 15:11:11 2015
//Author: ,,,

#include "KVI1.h"
#include "KVPSAResult.h"

ClassImp(KVI1)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVI1</h2>
<h4>digitized current signal</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
void KVI1::init()
{
   SetDefaultValues();
	fChannel = kI1;
	
}

KVI1::KVI1()
{
   init();
}

//________________________________________________________________

KVI1::KVI1(const char* name) : KVSignal(name, "Current")
{
	SetType(name);
	init();
}

//________________________________________________________________

KVI1::~KVI1()
{
   // Destructor
}

//________________________________________________________________

void KVI1::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVI1::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVSignal::Copy(obj);
   //KVI1& CastedObj = (KVI1&)obj;
}

void KVI1::SetDefaultValues()
{
    SetChannelWidth(4);
    SetBaseLineLength(30);
}

void KVI1::LoadPSAParameters()
{
	
	Double_t val = GetPSAParameter("BaseLineLength");
	SetBaseLineLength(val);
	
	val = GetPSAParameter("ChannelWidth");
	SetChannelWidth(val);

	val = GetPSAParameter("InterpolatedChannelWidth");
	SetInterpolatedChannelWidth(val);
	
	val = GetPSAParameter("Interpolation");
	SetWidthInterpolation( (val==1) );

}


void KVI1::TreateSignal()
{
	if (GetN()==0) return;
	
	ComputeBaseLine();
   fBaseLine  = GetBaseLine();
   fSigmaBase = GetSigmaBaseLine();
   Add(-1.*fBaseLine);
   if(fWithInterpolation)
		BuildCubicSignal();
   ComputeAmplitude();
   fAmplitude = GetAmplitude();
	
	fPSAIsDone=kTRUE;
	
}


KVPSAResult* KVI1::GetPSAResult() const
{
	if (!fPSAIsDone) return 0;
	
	KVPSAResult *psa = new KVPSAResult();
	psa->SetValue(Form("%s.%s.BaseLine",fDetName.Data(),fType.Data()),fBaseLine);
	psa->SetValue(Form("%s.%s.SigmaBaseLine",fDetName.Data(),fType.Data()),fSigmaBase);
	psa->SetValue(Form("%s.%s.Amplitude",fDetName.Data(),fType.Data()),fAmplitude);
	psa->SetValue(Form("%s.%s.RawAmplitude",fDetName.Data(),fType.Data()),GetRawAmplitude());
	return psa;

}
