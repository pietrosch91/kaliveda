//Created by KVClassFactory on Wed Sep 23 16:07:38 2009
//Author: Chbihi 

#include "KVGANILDataReader.h"
#include "GTGanilData.h"
#include "KVDataSet.h"
#include "KVMultiDetArray.h"
#include "TSystem.h"
#include "TUrl.h"

ClassImp(KVGANILDataReader)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGANILDataReader</h2>
<h4>Reads GANIL acquisition files</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVGANILDataReader::KVGANILDataReader(const Char_t * file)
{
   //Open and initialise a GANIL data file for reading.
   //By default, Scaler buffers are ignored.
   //If file cannot be opened, this object will be made a zombie. Do not use.
   //To test if file is open, use IsOpen().
   //The basename of the file (excluding any path) can be obtained using GetName()
   //The full pathname of the file can be obtained using GetTitle()
   //
   //If the dataset corresponding to the data to be read is known i.e. if gDataSet has been defined and points
   //to the correct dataset, this will allow to build the necessary multidetector object if it has not already
   //been done, and to set the calibration parameters etc. as a function of the run number.
	//
   //If not (i.e. if no information is available on detectors, calibrations, geometry, etc.),
	//then a list of KVACQParam objects will be generated and connected ready for reading the data.
	//This list can be obtained with method GetRawDataParameters().

   init();
   OpenFile(file);
}

KVGANILDataReader::~KVGANILDataReader()
{
   // Destructor
   if(fGanilData) { delete fGanilData; fGanilData=0; }
   if(fExtParams){
		fExtParams->Delete();
		delete fExtParams;
	}
	fParameters->Delete();
	delete fParameters;
}

void KVGANILDataReader::init()
{
   //default initialisations
   
   fExtParams = 0;
   fParameters = new KVHashList;
   fParameters->SetOwner(kTRUE);
   fGanilData = 0;
}

void KVGANILDataReader::OpenFile(const Char_t * file)
{
   //Open and initialise a GANIL data file for reading.
   //By default, Scaler buffers are ignored.
   //If file cannot be opened, this object will be made a zombie. Do not use.
   //To test if file is open, use IsOpen().
   //The basename of the file (excluding any path) can be obtained using GetName()
   //The full pathname of the file can be obtained using GetTitle()
   //
   //If the dataset corresponding to the data to be read is known i.e. if gDataSet has been defined and points
   //to the correct dataset, this will allow to build the necessary multidetector object if it has not already
   //been done, and to set the calibration parameters etc. as a function of the run number.
	//
   //If not (i.e. if no information is available on detectors, calibrations, geometry, etc.),
	//then a list of KVACQParam objects will be generated and connected ready for reading the data.
	//This list can be obtained with method GetRawDataParameters().

   if(fGanilData){ delete fGanilData; fGanilData=0; }
   
   fGanilData = NewGanTapeInterface();
   
   fGanilData->SetFileName(file);
   SetName( gSystem->BaseName(file) );
   SetTitle(file);
   fGanilData->SetScalerBuffersManagement(GTGanilData::kSkipScaler);
   fGanilData->Open();
   
   //test whether file has been opened
   if(!fGanilData->IsOpen()){
      //if initial attempt fails, we try to open the file as a 'raw' TFile
      //This may work when we are attempting to open a remote file i.e.
      //via rfio, and a subsequent attempt to open the file using the GanTape
      //routines may then succeed.
      TUrl rawtfile(file,kTRUE); rawtfile.SetOptions("filetype=raw");
      TFile* rawfile=TFile::Open(rawtfile.GetUrl());
      if(!rawfile){
         Error("OpenFile","File cannot be opened: %s",
            file);
         MakeZombie();
         return;
      }
      //TFile::Open managed to open file! Try again...
      delete rawfile;
      fGanilData->Open();
      if(!fGanilData->IsOpen()){
         //failed again ??!
         Error("OpenFile","File cannot be opened: %s",
            file);
         MakeZombie();
         return;
      }
   }
   
   ConnectRawDataParameters();
   if (gDataSet && !gMultiDetArray) {
      gDataSet->BuildMultiDetector();
   }
   if(gMultiDetArray){
		gMultiDetArray->SetParameters( fGanilData->GetRunNumber() );
   	ConnectArrayDataParameters();
	}
}

//__________________________________________________________________________

void KVGANILDataReader::ConnectRawDataParameters()
{
   //Private utility method called by KVGANILDataReader ctor.
	//fParameters is filled with a KVACQParam for each parameter in the file.
	//These KVACQParam objects are completely independent of those associated
	//with the detectors in the KVMultiDetArray. No distinction is made between "known"
	//or "unknown" parameters. fParameters is just a complete
	//list of the parameters in the file. It can be retrieved after the
	//file is opened, use GetRawDataParameters().
   
   TIter next( fGanilData->GetListOfDataParameters() );
   KVACQParam *par;
   GTDataPar* daq_par;
   while ((daq_par = (GTDataPar*) next())) {//loop over all parameters
      //create new KVACQParam parameter
      par = new KVACQParam;
      par->SetName( daq_par->GetName() );
		fParameters->Add(par);
		fGanilData->Connect(par->GetName(), par->ConnectData());
   }
}

void KVGANILDataReader::ConnectArrayDataParameters()
{
   //Private utility method called by KVGANILDataReader ctor.
   // - loop over all acquisition parameters
   // - known acquisition parameters are connected to the corresponding detectors
   // - unknown parameters are stored in the list which can be obtained using GetUnknownParameters()

   TIter next( fGanilData->GetListOfDataParameters() );
   KVACQParam *par;
   GTDataPar* daq_par;
   while ((daq_par = (GTDataPar*) next())) {
      if( (par=CheckACQParam( daq_par->GetName() )) ) fGanilData->Connect(par->GetName(), par->ConnectData());
   }
}

//___________________________________________________________________________

KVACQParam* KVGANILDataReader::CheckACQParam( const Char_t* par_name )
{
   //Check the named acquisition parameter.
   //We look for a corresponding parameter in the list of acq params belonging to
   //the current KVMultiDetArray (if one exists).
   //If none is found, we create a new acq param which is added to the list of "unknown parameters"
   
   KVACQParam *par;
   if( !gMultiDetArray || !(par = gMultiDetArray->GetACQParam( par_name )) ){
      //create new unknown parameter
      par = new KVACQParam;
      par->SetName( par_name );
		if(!fExtParams){
			fExtParams=new KVHashList;
			fExtParams->SetOwner(kTRUE);
		}
      fExtParams->Add( par );
   }
   return par;
}

//___________________________________________________________________________

Bool_t KVGANILDataReader::GetNextEvent()
{
   //Read next event in raw data file.
   //Returns false if no event found (end of file).

   Bool_t ok = fGanilData->Next();
   return ok;
}

 //___________________________________________________________________________

GTGanilData* KVGANILDataReader::NewGanTapeInterface()
{
   //Creates and returns new instance of class used to read GANIL acquisition data
   return new GTGanilData;
}

//____________________________________________________________________________

GTGanilData* KVGANILDataReader::GetGanTapeInterface()
{
   return fGanilData;
}

 //____________________________________________________________________________

 KVGANILDataReader* KVGANILDataReader::Open(const Char_t* filename, Option_t* opt)
 {
    //Static method, used by KVDataSet::Open
    return new KVGANILDataReader(filename);
 }
 
 //____________________________________________________________________________
 
