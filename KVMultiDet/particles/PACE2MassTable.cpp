//Created by KVClassFactory on Mon Jul 27 11:49:41 2009
//Author: John Frankland,,,

#include "PACE2MassTable.h"
#include "TNamed.h"
#include "Riostream.h"
#include "TEnv.h"

using namespace PACE2Map;

ClassImp(key)
ClassImp(value)

ClassImp(PACE2MassTable)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>PACE2MassTable</h2>
<h4>The pace2_2001.data mass table used in the SIMON code</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

PACE2MassTable::PACE2MassTable()
{
   // Default constructor
	theTable = 0;
	nucMap = 0;
}

PACE2MassTable::~PACE2MassTable()
{
   // Destructor
	if(theTable) delete theTable;
	if(nucMap) delete nucMap;
}

void PACE2MassTable::Initialize()
{
	// Read mass excess values from file. Name of file given by
	//
	//  PACE2MassTable.DataFile:     pace2_2009.data
	//
	// and by default (unless absolute file name given)
	// we look for it in $KVROOT/KVFiles.
	//
	// Format of file is:
	//    - comment lines begin with '#'
	//    - for each nucleus there is a line with Z,A, and Mass Excess (in keV), like:
	// 2  4   2424.91
	//    i.e. Z=2 A=4 mass excess=2424.91 keV
	
	ifstream tabfile;
	if(SearchAndOpenKVFile(gEnv->GetValue("PACE2MassTable.DataFile",""),
			tabfile,"data")){
		
		// read file once to see how many nuclei are in it
		// and set up TMap
		nucMap = new TMap(50,2);
		nucMap->SetOwnerKeyValue();
		
		Int_t ntot=0; KVString Sline;
		while(tabfile.good()){
			Sline.ReadLine(tabfile);
			if(!Sline.BeginsWith("#")){
				Sline.Begin(" ");
				Int_t z=-1;Int_t a=-1; Double_t MXX=-1;
				if(!Sline.End()) z=Sline.Next(kTRUE).Atoi();
				if(!Sline.End()) a=Sline.Next(kTRUE).Atoi();
				if(!Sline.End()) MXX=Sline.Next(kTRUE).Atof();
				if(z>-1&&a>-1){
					nucMap->Add(new PACE2Map::key(z,a), new PACE2Map::value(ntot));
					ntot++;
				}
			}
		}
		Info("Initialize","Set up map for %d nuclei", ntot);
		theTable = new Double_t[ntot];
		
		// now read mass excesses
		tabfile.clear();
		tabfile.seekg(0, ios::beg);
		ntot=0;
		while(tabfile.good()){
			Sline.ReadLine(tabfile);
			if(!Sline.BeginsWith("#")){
				Sline.Begin(" ");
				Int_t z=-1;Int_t a=-1; Double_t MXX=-1;
				if(!Sline.End()) z=Sline.Next(kTRUE).Atoi();
				if(!Sline.End()) a=Sline.Next(kTRUE).Atoi();
				if(!Sline.End()) MXX=Sline.Next(kTRUE).Atof();
				if(z>-1&&a>-1){
					theTable[ntot] = MXX;
					ntot++;
				}				
			}
		}
		Info("Initialize","Mass table initialised correctly for %d nuclei", ntot);
	}
	else
	{
		Error("Initialize", "Cannot find mass table file %s given by PACE2MassTable.DataFile",
				gEnv->GetValue("PACE2MassTable.DataFile",""));
	}
}

Double_t PACE2MassTable::GetMassExcess(Int_t Z, Int_t A)
{
	// Returns mass excess for nucleus (Z,A) in keV, if known
	// If not, returns -500.;
	
	Double_t MXX;
	PACE2Map::value* val = (PACE2Map::value*)nucMap->GetValue(Form("%d:%d",Z,A));
	MXX = (val ? theTable[val->Index()] : -500.);
	return MXX;
}

Bool_t PACE2MassTable::IsKnown(Int_t Z, Int_t A)
{
	// Returns kTRUE if nucleus (Z,A) is in mass table.
	
	PACE2Map::value* val = (PACE2Map::value*)nucMap->GetValue(Form("%d:%d",Z,A));
	return (val!=0);
}
