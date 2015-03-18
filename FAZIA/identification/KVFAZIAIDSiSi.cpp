//Created by KVClassFactory on Mon Mar 16 11:45:14 2015
//Author: ,,,

#include "KVFAZIAIDSiSi.h"
#include "KVIDZAGrid.h"


ClassImp(KVFAZIAIDSiSi)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIAIDSiSi</h2>
<h4>identification telescope for FAZIA Si-Si idcards</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVFAZIAIDSiSi::KVFAZIAIDSiSi()
{
   // Default constructor
	fGrid=0;
	SetType("Si-Si");
}

KVFAZIAIDSiSi::~KVFAZIAIDSiSi()
{
   // Destructor
}

void KVFAZIAIDSiSi::Initialize()
{
   // Initialize telescope for current run.
   // If there is at least 1 grid, we set fCanIdentify = kTRUE
   // "Natural" line widths are calculated for KVIDZAGrids.
 
  fGrid = 0; 
   
  KVIDGraph* grid = 0;
  TIter next_grid(GetListOfIDGrids());
  while((grid=(KVIDGraph*)next_grid()))
    {
    //Info("Initialize","%s %s %s",GetName(),grid->GetVarY(),grid->GetVarX());
	 if( !strcmp(grid->GetVarY(),"QH1.Amplitude") && !strcmp(grid->GetVarX(),"Q2.Amplitude"))
	 	//Info("Initialize","%s %s %s",GetName(),grid->GetVarY(),grid->GetVarX());
	 	fGrid = (KVIDZAGrid*)grid;
    	fVarX.Form("%s.%s",GetDetector(2)->GetName(),grid->GetVarX());
		fVarY.Form("%s.%s",GetDetector(1)->GetName(),grid->GetVarY());
    }
   
  if(fGrid)
    {
    SetBit(kReadyForID);
    fGrid->Initialize();
  }
  else ResetBit(kReadyForID);
   
}

//________________________________________________________________
Bool_t KVFAZIAIDSiSi::Identify(KVIdentificationResult* idr, Double_t x, Double_t y)
{
	// Particle identification and code setting using identification grids.
	// perform identification in Si(GG) - CsI(H) map
	// Sets idr->deltaEpedestal according to position in GG map
 	if (x==-1 || y==-1) return kFALSE;
 
	idr->SetIDType(GetType());
	idr->IDattempted = kTRUE;
	
	Double_t si1 = y;
	Double_t si2 = x;
	
	fGrid->Identify(si2, si1, idr);
	idr->IDcode = GetSize();
	idr->IDquality = fGrid->GetQualityCode();
	
	return kTRUE;

}

void KVFAZIAIDSiSi::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVFAZIAIDSiSi::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVFAZIAIDTelescope::Copy(obj);
   //KVFAZIAIDSiSi& CastedObj = (KVFAZIAIDSiSi&)obj;
}

