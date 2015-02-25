//Created by KVClassFactory on Fri Jan 23 18:44:27 2015
//Author: ,,,

#include "KVFAZIADetector.h"
#include "KVSignal.h"
#include "KVChargeSignal.h"
#include "KVCurrentSignal.h"

ClassImp(KVFAZIADetector)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIADetector</h2>
<h4>Base class for FAZIA detector</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//________________________________________________________________
void KVFAZIADetector::init()
{
   //default initialisations
   fSignals = 0;
}

//________________________________________________________________
KVFAZIADetector::KVFAZIADetector()
{
   // Default constructor
   init();
}

//________________________________________________________________
KVFAZIADetector::KVFAZIADetector(const Char_t* type, const Float_t thick) : KVDetector(type, thick)
{
   // Write your code here
   init();
}

//________________________________________________________________
KVFAZIADetector::~KVFAZIADetector()
{
   // Destructor
   delete fSignals;
   
}

//________________________________________________________________
void KVFAZIADetector::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVFAZIADetector::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVDetector::Copy(obj);
   //KVFAZIADetector& CastedObj = (KVFAZIADetector&)obj;
}

//________________________________________________________________
void	KVFAZIADetector::Clear(Option_t* opt)
{

	//Info("Clear","Call %s",GetName());
   KVDetector::Clear("");
   if (fSignals){
   	fSignals->Execute("Set","0");
	}
   
}

//________________________________________________________________
void KVFAZIADetector::SetName(const char* name)
{
	TNamed::SetName(name);
	SetProperties();
}

//________________________________________________________________
Bool_t KVFAZIADetector::SetProperties()
{
	// detector name are assumed to be defined as
   // SI2-T2-Q2-B001
	//	CSI-T2-Q2-B001
	// SI1-T1-Q1-B001
	//
   
   KVString tmp;
   KVString sname(GetName());
   sname.Begin("-");
   
   fFAZIAType = sname.Next();
	tmp = sname.Next(); tmp.ReplaceAll("T",""); fTelescope = tmp.Atoi();
	tmp = sname.Next(); tmp.ReplaceAll("Q",""); fQuartet = tmp.Atoi();
	tmp = sname.Next(); tmp.ReplaceAll("B",""); fBlock = tmp.Atoi();
	
   //"QH1", "I1", "QL1", "Q2", "I2", "Q3
	if (fSignals)
   	delete fSignals;
   fSignals = new KVList(kTRUE);
   if (fFAZIAType=="SI1"){
   	fSignals->Add(new KVChargeSignal("QH1"));
   	fSignals->Add(new KVCurrentSignal("I1"));
   	fSignals->Add(new KVChargeSignal("QL1"));
   }
   else if (fFAZIAType=="SI2"){
   	fSignals->Add(new KVChargeSignal("Q2"));
   	fSignals->Add(new KVCurrentSignal("I2"));
   }
	else if (fFAZIAType=="CSI"){
   	fSignals->Add(new KVChargeSignal("Q3"));
   }
   else{
   	Warning("SetProperties","Unknown FAZIA type \"%s\" for this detector : %s\n",fFAZIAType.Data(),GetName());
   }
   return kTRUE;
}
//________________________________________________________________
Bool_t KVFAZIADetector::Fired(Option_t * opt)
{
    // Returns kTRUE if detector was hit (fired) in an event
    //
    // The actual meaning of hit/fired depends on the context and the option string opt.
    //
    // If the detector is in "simulation mode", i.e. if SetSimMode(kTRUE) has been called,
    // this method returns kTRUE if the calculated energy loss in the active layer is > 0.
    //
    // In "experimental mode" (i.e. IsSimMode() returns kFALSE), depending on the option:
    //
 	 //Info("Fired","Appel - %s",GetName());	
    if (!IsDetecting()) return kFALSE; //detector not working, no answer at all
    if (IsSimMode()) return (GetActiveLayer()->GetEnergyLoss()>0.); // simulation mode: detector fired if energy lost in active layer
	 KVSignal* sig;	
	 if (fSignals){
    	TIter next(fSignals);
    	while ( (sig = (KVSignal* )next()) )
    	{
    		if (sig->GetN()>0){
      		if (!strcmp(sig->GetTitle(),"Charge") && sig->GetAmplitude()>20){
         		return kTRUE;
         	}
         	else{
      			//printf("\t\t hitted :-) %lf\n",sig->GetAmplitude());
         	   //Info("Fired","YES %s %d",GetName(),GetGroupNumber());
         	   return kFALSE;
      		}
      	}
         else{
         	Warning("Fired","%s has empty signal %s",GetName(),sig->GetName());
         }
      }
    }
    else{
    	Warning("Fired","%s : No signal attached to this detector ...",GetName());
      return kFALSE;
    }
    //Info("Fired","Sort - %s",GetName());
    //Info("Fired","NO %s %d",GetName(),GetGroupNumber());
    return kFALSE;
}

//_________________________________________________________________________________
void KVFAZIADetector::SetSignal(KVSignal* signal,const Char_t* type)
{	
	if (!fSignals)	
   	return;
	
   KVSignal* sig = GetSignal(type);
   if (sig)
   	sig->SetData(signal->GetN(),signal->GetX(),signal->GetY());
   else
   	Warning("SetSignal","%s : No signal of type %s is available",GetName(),type);   
}

//_________________________________________________________________________________
Bool_t KVFAZIADetector::HasSignal() const
{
	return (fSignals && fSignals->GetEntries()>0);
}

//_________________________________________________________________________________
KVSignal* KVFAZIADetector::GetSignal(const Char_t* name) const
{	
	if (fSignals)	
   	return (KVSignal* )fSignals->FindObject(name);
	return 0;
}

//_________________________________________________________________________________
KVSignal* KVFAZIADetector::GetSignal(Int_t idx) const
{	
	if (fSignals && 0<= idx && idx<fSignals->GetEntries())	
   	return (KVSignal* )fSignals->At(idx);
	return 0;
}

//_________________________________________________________________________________
Int_t KVFAZIADetector::GetNumberOfSignals() const
{	
	if (fSignals)
   	return fSignals->GetEntries();
   return 0;
}

//_________________________________________________________________________________
KVList* KVFAZIADetector::GetListOfSignals() const
{	
	return fSignals;
}
