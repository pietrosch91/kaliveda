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
   fTGID=0;
}

KVTGIDGrid::KVTGIDGrid(KVTGID *tgid)
		: KVIDZAGrid()
{
   // Create a grid in order to visualize the results of a fit of an identification grid.
   
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
    fTGID->WriteToAsciiFile(gridfile);
}

