//Created by KVClassFactory on Mon Feb  3 11:02:18 2014
//Author: John Frankland,,,

#include "KVHistogram.h"
#include <TROOT.h>

ClassImp(KVHistogram)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVHistogram</h2>
<h4>Wrapper for histograms used by KVTreeAnalyzer</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVHistogram::KVHistogram(TH1* h)
{
   // Default constructor
	fHisto = h;
    fCut = 0;
	if(h){
		KVString exp,sel,x,y,z;
		ParseHistoTitle(h->GetTitle(),exp,sel);
		ParseExpressionString(exp,x,y,z);
		fParams.SetValue("VARX",x);
		fParams.SetValue("VARY",y);
		fParams.SetValue("VARZ",z);
		fParams.SetValue("SELECTION",sel);
        fParams.SetValue("EXPRESSION",exp);
        SetType("Histo");
        SetName(h->GetName());
        SetLabel(h->ClassName());
    }
}

KVHistogram::KVHistogram(TCutG *cut)
{
    // Ctor for TCutG object
    fHisto = 0;
    fCut = cut;
    SetType("Cut");
    fParams.SetValue("VARX",cut->GetVarX());
    fParams.SetValue("VARY",cut->GetVarY());
    fParams.SetValue("VARZ","");
    fParams.SetValue("SELECTION","");
    fParams.SetValue("EXPRESSION", "");
    SetName(cut->GetName());
    SetLabel(cut->ClassName());
}

KVHistogram::~KVHistogram()
{
   // Destructor
    SafeDelete(fHisto);
}

void KVHistogram::ParseHistoTitle(const Char_t* title, KVString& exp, KVString& sel)
{
   // Take histo title "VAREXP { SELECTION }"
   // and separate the two components
   
   exp="";
   sel="";
   TString tmp(title);
   Int_t ss = tmp.Index("{");
   if(ss>0){
      Int_t se = tmp.Index("}");
      sel = tmp(ss+1, se-ss-1);
      sel.Remove(TString::kBoth,' ');
      exp = tmp(0, ss);
      exp.Remove(TString::kBoth,' ');
   }
   else
   {
      exp = tmp;
      exp.Remove(TString::kBoth,' ');      
   }
}
   
void KVHistogram::ParseExpressionString(const Char_t* exp, KVString& varX, KVString& varY,
	         KVString& varZ)
{
   // Parse expression strings "VARZ:VARY:VARX" or "VARY:VARX" or "VARX"
	
    KVString tmp(exp);
    Int_t nvar = tmp.CountChar(':');
    tmp.Begin(":");
	varX=varY=varZ="";
    if(nvar==2) varZ=tmp.Next();
    if(nvar>=1) varY=tmp.Next();
    varX=tmp.Next();
}

const Char_t* KVHistogram::GetVarX() const
{
   return fParams.GetStringValue("VARX");
}
const Char_t* KVHistogram::GetVarY() const
{
   return fParams.GetStringValue("VARY");
}
const Char_t* KVHistogram::GetVarZ() const
{
   return fParams.GetStringValue("VARZ");
}
const Char_t* KVHistogram::GetSelection() const
{
    return fParams.GetStringValue("SELECTION");
}

void KVHistogram::ls(Option_t *option) const
{
    TROOT::IndentLevel();
    TROOT::IncreaseDirLevel();
    cout << "KVHistogram::" << GetName() << " " << GetLabel();
    cout << " X:" << GetVarX();
    if(IsType("Cut")){
        cout << " Y:" << GetVarY();
    }
    else
    {
        if(strcmp(GetVarY(),"")) cout << " Y:" << GetVarY();
        if(strcmp(GetVarZ(),"")) cout << " Z:" << GetVarZ();
        if(strcmp(GetSelection(),"")) cout << " [" << GetSelection() << "]";
    }
    cout << endl;
    TROOT::DecreaseDirLevel();
}

TObject *KVHistogram::GetObject()
{
    // In order to have context menu access to the contained histogram or cut
    if(fHisto) return (TObject*)fHisto;
    return fCut;
}

const Char_t* KVHistogram::GetExpression() const
{
    return fParams.GetStringValue("EXPRESSION");
}

const Char_t *KVHistogram::GetHistoTitle() const
{
    if(fHisto) return fHisto->GetTitle();
    if(fCut) return fCut->GetName();
    return "";
}
