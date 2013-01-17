/*
$Id: KVTGIDGrid.cpp,v 1.2 2009/03/03 14:27:15 franklan Exp $
$Revision: 1.2 $
$Date: 2009/03/03 14:27:15 $
*/

//Created by KVClassFactory on Fri Apr 25 15:55:08 2008
//Author: franklan

#include "KVTGIDGrid.h"
#include "KVTGID.h"
#include "KVIDGridManager.h"

using namespace std;

ClassImp(KVTGIDGrid)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVTGIDGrid</h2>
<h4>Grid representing result of fit</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVTGIDGrid::KVTGIDGrid()
{
   // Default constructor
   fTGID=0;
}

KVTGIDGrid::KVTGIDGrid(KVTGID *tgid,KVIDZAGrid*original)
		: KVIDZAGrid()
{
   // Create a grid in order to visualize the results of a fit of an identification grid.
   // We keep any "cuts" from the original grid and add them to the fitted grid
   
   fTGID = tgid;
    SetVarX( fTGID->GetVarX() );
    SetVarY( fTGID->GetVarY() );
    SetRuns( fTGID->GetValidRuns() );
    TCollection* idtels = fTGID->GetIDTelescopes();
    AddIDTelescopes( (const TList*)idtels );
    delete idtels;
	TString name(fTGID->GetName()); Int_t ver=1;
	if( gIDGridManager->GetGrid( name.Data() ) ){
		do
		{
			name.Form("%s_%d",fTGID->GetName(),ver);
			ver++;
		} while( gIDGridManager->GetGrid( name.Data() ) );
	}
	SetName( name.Data() );
   if(original) original->GetCuts()->Copy((TObject &)*fCuts);  
   
    fPar->SetValue("HasTGID",1);
//    
//    fPar->SetValue("KVTGID::Name",fTGID->GetName());
//    fPar->SetValue("KVTGID::Type",fTGID->GetFunctionalType());
//    fPar->SetValue("KVTGID::",fTGID->GetLightEnergyDependence());
//    fPar->SetValue("KVTGID::ZorA",fTGID->GetZorA());
//    if(fTGID->GetZorA()==1) fPar->SetValue("KVTGID::MassFormula",fTGID->GetMassFormula());
//    fPar->SetValue("KVTGID::Functional",fTGID->GetFunctionName());
//    fPar->SetValue("KVTGID::ZMIN",(Int_t)fTGID->GetIDmin());
//    fPar->SetValue("KVTGID::ZMAX",(Int_t)fTGID->GetIDmin());
//    
//    Bool_t type1 = (fTGID->GetFunctionalType()==1);
//    fPar->SetValue("KVTGID::Lambda",Form("%20.13e",fTGID->GetLambda()));
//    if(type1) fPar->SetValue("KVTGID::Alpha",Form("%20.13e",fTGID->GetAlpha()));
//    if(type1) fPar->SetValue("KVTGID::Beta",Form("%20.13e",fTGID->GetBeta()));
//    fPar->SetValue("KVTGID::Mu",Form("%20.13e",fTGID->GetMu()));
//    if(type1) fPar->SetValue("KVTGID::Nu",Form("%20.13e",fTGID->GetNu()));
//    if(type1) fPar->SetValue("KVTGID::Xi",Form("%20.13e",fTGID->GetXi()));
//    
//    fPar->SetValue("KVTGID::G",Form("%20.13e",fTGID->GetG()));
//    fPar->SetValue("KVTGID::Pdx",Form("%20.13e",fTGID->GetPdx()));
//    fPar->SetValue("KVTGID::Pdy",Form("%20.13e",fTGID->GetPdy()));
//    
//    if(fTGID->GetLightEnergyDependence()==1) fPar->SetValue("KVTGID::Eta",Form("%20.13e",fTGID->GetEta()));
      
}

KVTGIDGrid::~KVTGIDGrid()
{
   // Destructor
}

//_______________________________________________________________________________________________//

void KVTGIDGrid::WriteToAsciiFile(ofstream & gridfile)
{
	// Write parameters of LTG fit used to generate grid in gridfile.
    KVIDGraph::WriteToAsciiFile(gridfile);
    if(fTGID) fTGID->WriteToAsciiFile(gridfile);
}

//_______________________________________________________________________________________________//

void KVTGIDGrid::ReadFromAsciiFile(ifstream & gridfile)
{
	// Read grid and parameters of LTG fit used to generate grid in gridfile.
    KVIDGraph::ReadFromAsciiFile(gridfile);
    KVString line;
    if(fPar->GetIntValue("HasTGID"))
      {
      line.ReadLine(gridfile);  
      if(line.BeginsWith("++KVTGID"))  fTGID = KVTGID::ReadFromAsciiFile(GetName(), gridfile);
      }
}

//___________________________________________________________________________//

void KVTGIDGrid::Generate(Double_t xmax, Double_t xmin, Int_t ID_min, Int_t ID_max, Int_t npoints, Bool_t logscale)
{
    //(Re)generate ID grid from the functional using current values of parameters.
    //
    //      xmin, xmax - min and max values of 'x' coordinate used in corresponding
    //                                      identification map
    //      ID_min, ID_max - min and max 'ID' of lines in grid
    //      npoints    - number of points in each line
    //
    //DEFAULT ARGUMENT VALUES:
    //=========================
    //   xmin = 0.
    // If the grid gets "ugly" for small x, you might try setting xmin = fitted X-pedestal
    // This is the value of parameter 7 in most cases....
    //
    //  ID_min, ID_max = min and max ID of lines on which functional was fitted
    //  npoints = 100
    //
    // if logscale=kTRUE (default is kFALSE) lines are generated with more points at
    // the beginning of the lines than at the end.
    
    if(!GetTGID()) 
      {
      Error("Generate","No parameter stored to initialize a functional !");
      return;
      }
    

    ID_min = (ID_min ? ID_min : (Int_t) fTGID->GetIDmin());
    ID_max = (ID_max ? ID_max : (Int_t) fTGID->GetIDmax());

    // clear existing lines
    Bool_t was_drawn = kFALSE;
    TVirtualPad* last_pad=fPad;
    if(fPad) {UnDraw(); was_drawn=kTRUE;}
    
    // delete all previous identification lines
    fIdentifiers->Delete();
    fXmin = fYmin = fXmax = fYmax = 0;

    for (Int_t ID = ID_min; ID <= ID_max; ID++) {
        fTGID->AddLineToGrid(this, ID, npoints, xmin, xmax, logscale);
    }
	if(was_drawn) {last_pad->cd(); Draw();}
}

//___________________________________________________________________________//
const KVTGID* KVTGIDGrid::GetTGID() const
{ 
	// a completer mais j'ai la fleme...  
  return fTGID;
}

//___________________________________________________________________________//
void KVTGIDGrid::AddIdentifier(KVIDentifier *id)
{
   id->SetLineColor(kGray+2);
   id->SetLineStyle(2);
   KVIDGraph::AddIdentifier(id);
}
