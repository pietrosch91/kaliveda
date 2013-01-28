//Created by KVClassFactory on Mon Sep 17 17:49:41 2012
//Author: Ademard Guilain
// Updated by A. Chbihi on 20/08/2009
// In addition to set the VAMOS scalers in the data base using ReadVamosScalers ()  method
//  it set the Vamos Brho's and angle.
// The new method of this class is ReadVamosBrhoAndAngle ().



#include "KVIVDB.h"

ClassImp(KVIVDB)

	////////////////////////////////////////////////////////////////////////////////
	// BEGIN_HTML <!--
	/* -->
	   <h2>KVIVDB</h2>
	   <h4>Database for e494s experiment coupling INDRA with VAMOS</h4>
	   <!-- */
	// --> END_HTML
	////////////////////////////////////////////////////////////////////////////////

KVIVDB::KVIVDB()
{
   	// Default constructor
   	init();
}
//________________________________________________________________

KVIVDB::KVIVDB (const Char_t * name) : KVINDRADB(name)
{
	init();
}
//________________________________________________________________

KVIVDB::~KVIVDB()
{
   	// Destructor
}
//________________________________________________________________

   void KVIVDB::init(){
	   // Default initialisations
	   fVAMOSCalib = AddTable("VAMOS calibration","Calibration parameters for detectors of VAMOS");

   }
//________________________________________________________________

void KVIVDB::Build () 
{
	// Read runlist and systems list, then read VAMOS scalers
   	// Read Brho and angle of Vamos for each run

   	KVINDRADB::Build();
   	ReadVamosScalers();
   	ReadVamosBrhoAndAngle ();
}
//________________________________________________________________

   Bool_t KVIVDB::ReadVamosCalibFile(ifstream &infile){
	   // Reads the calibration file loaded in 'infile'.
	   // Retruns kTRUE if the mimimum of information is 
	   // given inside the file (Run list, type of calibration,
	   // function of calibration, acquisition parameter 
	   // associated to the calibration parameters... ).
	   // The format is the following:
	   //
	   // RUNS: 1-30 34 50-200
	   // TYPE: channel->Volt  (or channel->ns, Volt->MeV)
	   // FUNCTION: pol1       (or formula format of TF1)
	   // 
	   // this is the miminum of informations to give in the file.
	   // The following depends of the calibration type:
	   // - If the type contains 'channel' then the calibration
	   //   parameters have to be associated to an acquisition
	   //   parameter. In this case the name of the acquisition
	   //   parameter has to be given first followed by ':' and the parameters.
	   // - else the  calibration parameters have to be 
	   //   associated to the corresponding detector.
	   //
	   // The comment line begins with '#'.
	   //
	   // You can change when you want the run list, or the
	   // function. The example below illustrates these
	   // possibilities:
	   //
	   // RUNS: 1-30 34 50-200
	   // TYPE: channel->Volt
	   // FUNCTION: pol1
	   // SED1_X_001: par0 par1
	   // SED1_X_002: par0 par1
	   // #
	   // FUNCTION: [0]+[1]*x+[2]/x
	   // SED1_X_003: par0 par1 par2
	   // ...
	   // ...
	   // RUNS: 201 203 207-266
	   // SED1_X_001: par0 par1 par2
	   // SED1_X_002: par0 par1 par3
	   // ...
	   // ...
	   // RUNS: 1-266
	   // FUNCTION: pol6
	   // TYPE: channel->ns
	   // SED1_HF: par0 par1 par2 par3 par4 par5 par6





	   return kTRUE;

   }
//________________________________________________________________

void KVIVDB::ReadVamosBrhoAndAngle () 
{
	TString sline; 
    ifstream fin;

    if( !OpenCalibFile("VamosBrhoAngle", fin) ){
        Warning("VamosBrhoAngle", "VAMOS Brho and angle file not found : %s",
				GetCalibFileName("VamosBrhoAngle"));
        return;
   	}

   	Info("ReadVamosBrhoAngle", "Reading in VamosBrho and angle file : %s",
			GetCalibFileName("VamosBrhoAngle"));

   	Int_t run = 0;
   	Float_t Brho = -1;
   	Float_t theta = -1.;
	while (fin.good()) {         //reading the file
      	sline.ReadLine(fin);
      	if (!fin.eof()) {          //fin du fichier
		   	if (sline.Sizeof() > 1 && !sline.BeginsWith("#") ){
				sscanf(sline.Data(), "%d %f %f ", &run, &Brho, &theta);
              	cout<<" run = "<<run<<", Brho = "<<Brho<<", Theta = "<<theta<<endl;
               	if(Brho==0){
                  	Brho = -1.;
                  	theta = -1.;
               	}
               	KVINDRADBRun * idb = GetRun(run);
               	if (idb){
                  	idb->Set("Brho",Brho);
                  	idb->Set("Theta",theta);
                }            
      		}
   		}
	}

   	fin.close();         
}
//________________________________________________________________

void KVIVDB::ReadVamosCalibrations(){
	// All the calibration files of VAMOS have the same structure.
	// This method lookfor the global file where the list of calibration
	// files are written.
	// This global file is given in the  environment variable
	// 'VAMOSCalibFile' (see OpenCalibFile(...)).
	//

	ifstream inf;
	if(!OpenCalibFile("VAMOSCalibFile",inf)){
		Error("ReadVamosCalibrations","Could not open file %s",GetCalibFileName("VAMOSCalibFile"));
		return;
	}

	Info("ReadVamosCalibrations","Reading calibration parameters of VAMOS...");

	TString sline;
	while(inf.good()){
		sline.ReadLine(inf);

		// Skip comment line
		if(sline.BeginsWith("#")) continue;

		ifstream inf2;	
		if(!gDataSet->OpenDataSetFile(sline.Data(),inf2)){
			Error("ReadVamosCalibrations","Could not open file %s",sline.Data());
			inf2.close();
			continue;
		}

		Info("ReadVamosCalibrations","Reading %s",sline.Data());
		if(!ReadVamosCalibFile(inf2)) Error("ReadVamosCalibrations","Bad structure inside the file %s",sline.Data());

		inf2.close();
	}
	inf.close();
}
//________________________________________________________________

void KVIVDB::ReadVamosScalers () 
{
    ifstream fin;

    if( !OpenCalibFile("VamosScaler", fin) ){
        Warning("ReadVamosScalers", "VAMOS scalers file not found : %s",
				GetCalibFileName("VamosScaler"));
        return;
   	}
   	Info("ReadVamosScalers", "Reading in Vamos Scaler file : %s",
			GetCalibFileName("VamosScaler"));

   	Float_t  NormScaler = 0;
   	Float_t DT=0;
   	Float_t NormVamos = 0;
	TString sline; 
   	Int_t irun=0;
   	Int_t ScalerValue;
   	Char_t Crunnumber[40];
   	Char_t str[30];
   	Bool_t runfound = kFALSE;
 	while (fin.good()) {         //reading the file
      	sline.ReadLine(fin);
      	if (!fin.eof()) {          //fin du fichier
		   	if (sline.Sizeof() > 1 && !sline.BeginsWith("#") ){
            	if (sline.BeginsWith("ScalersOfRunNumber")){
               		sscanf(sline.Data(), "%30s %d ", Crunnumber, &irun); 
               		KVINDRADBRun * idb = GetRun(irun);
			   		if (idb){
						runfound = kTRUE;
               			for (Int_t i =0;i<5;i++) {       //retrouve dans les 4 prochaines lignes NormVamos,normSca etDT
					   		sline.ReadLine(fin);

					   		if (sline.BeginsWith("NormVamos")){
						   		sscanf(sline.Data(), "%20s %f ", str, &NormVamos);
                    			idb->Set(str,NormVamos);
					   		} 
					   		if(sline.BeginsWith("NormScalers")){
						   		sscanf(sline.Data(), "%20s %f ", str, &NormScaler);
                     			idb->Set(str,NormScaler);
					   		} 
					   		if(sline.BeginsWith("DT")){
						   		sscanf(sline.Data(), "%20s %f ", str, &DT);
                     			idb->Set(str,DT);
					   		} 

               			}
						for (Int_t i = 0;i<40;i++){
							sline.ReadLine(fin);
							sscanf(sline.Data(), "%s %d ", str, &ScalerValue);
                  			idb->SetScaler(str,ScalerValue);
						}

			   		}
             	}
			}   
		} 
	}			
   	fin.close();         
}


