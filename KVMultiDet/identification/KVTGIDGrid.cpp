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
}

KVTGIDGrid::KVTGIDGrid(KVTGID *tgid)
		: KVIDZAGrid()
{
   fTGID = tgid;
	TString name(fTGID->GetName()); Int_t ver=1;
	if( gIDGridManager->GetGrid( name.Data() ) ){
		do
		{
			name.Form("%s_%d",fTGID->GetName(),ver);
			ver++;
		} while( gIDGridManager->GetGrid( name.Data() ) );
	}
	SetName( name.Data() );
}

KVTGIDGrid::~KVTGIDGrid()
{
   // Destructor
}

//_______________________________________________________________________________________________//

void KVTGIDGrid::WriteToAsciiFile(ofstream & gridfile)
{
	// Write parameters of LTG fit used to generate grid in file.
	
	TIter next_tel(GetIDTelescopes());
	KVIDTelescope* idtel;
	while( (idtel = (KVIDTelescope*)next_tel()) ){
		
		gridfile << "+ " << idtel->GetName() << endl;
   	//write X & Y axis names
		gridfile << "<VARX> " << GetVarX() << endl;
		gridfile << "<VARY> " << GetVarY() << endl;
		gridfile << "ZMIN=" << (Int_t)fTGID->GetIDmin();
		gridfile << "  ZMAX=" << (Int_t)fTGID->GetIDmax() << endl;
		gridfile << "Functional=" << fTGID->GetFunctionName() << endl;
		gridfile << "Type=" << fTGID->GetFunctionalType() << endl;
		gridfile << "LightEnergyDependence=" << fTGID->GetLightEnergyDependence() << endl;
		gridfile << "ZorA=" << fTGID->GetZorA() << endl;
		if(fTGID->GetZorA()==1){
			gridfile << "MassFormula=" << fTGID->GetMassFormula() << endl;
		}
		Bool_t type1 = (fTGID->GetFunctionalType()==1);
		gridfile << Form("Lambda=%20.13e",fTGID->GetLambda()) << endl;
		if(type1){
			gridfile << Form("Alpha=%20.13e",fTGID->GetAlpha()) << endl;
			gridfile << Form("Beta=%20.13e",fTGID->GetBeta()) << endl;
		}
		gridfile << Form("Mu=%20.13e",fTGID->GetMu()) << endl;
		if(type1){
			gridfile << Form("Nu=%20.13e",fTGID->GetNu()) << endl;
			gridfile << Form("Xi=%20.13e",fTGID->GetXi()) << endl;
		}
		gridfile << Form("G=%20.13e",fTGID->GetG()) << endl;
		gridfile << Form("Pdx=%20.13e",fTGID->GetPdx()) << endl;
		gridfile << Form("Pdy=%20.13e",fTGID->GetPdy()) << endl;
		if(fTGID->GetLightEnergyDependence()==1){
			gridfile << Form("Eta=%20.13e",fTGID->GetEta()) << endl;
		}
		gridfile << "!" << endl << endl;
	}
}

