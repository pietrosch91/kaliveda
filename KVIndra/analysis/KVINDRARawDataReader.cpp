/***************************************************************************
                          KVINDRARawDataReader.cpp  -  description
                             -------------------
    begin                : Thu Nov 21 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr
$Id: KVINDRARawDataReader.cpp,v 1.5 2008/10/17 10:58:07 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVINDRARawDataReader.h"
#include "KVINDRA.h"
#include "KVDataSet.h"
#include "TUrl.h"
#include "GTGanilData.H"

ClassImp(KVINDRARawDataReader)

///////////////////////////////////////////////////////////////////////
// Class handling reading of raw INDRA data from GANIL acquisition files
// It interfaces with the ROOTGanilTape routines of Yannick Patois
// (patois@calvix.org).
// The link between array detectors and acquisition parameters is
// made by the KVACQParam class.

void KVINDRARawDataReader::init()
{
   //default initialisations, creates KVACQParam objects for STAT_EVE, R_DEC, CONFIG
   fSTAT_EVE_PAR = new KVACQParam();
   fSTAT_EVE_PAR->SetName("STAT_EVE");
   fR_DEC_PAR = new KVACQParam();
   fR_DEC_PAR->SetName("R_DEC");
   fVXCONFIG_PAR = new KVACQParam();
   fVXCONFIG_PAR->SetName("CONFIG");
   fExtParams = new KVList;
	fParameters = new KVList;
   fGanilData = 0;
}

//__________________________________________________________________________

KVINDRARawDataReader::KVINDRARawDataReader(const Char_t * file)
{
   //Open and initialise a raw INDRA data file for reading.
   //By default, Scaler buffers are ignored.
   //If file cannot be opened, this object will be made a zombie. Do not use.
   //To test if file is open, use IsOpen().
   //
   //The dataset corresponding to the data to be read must be known i.e. gDataSet must be defined and point
   //to the correct dataset. This will allow to build the necessary multidetector object if it has not already
   //been done, and to set the calibration parameters etc. as a function of the run number.
   //If the dataset has not been defined, this object will be made a zombie. Do not use.
   //
   //The multidetector array is initialised according to the current run number (call to KVINDRA::SetParameters).
   //The acquisition parameters are linked to the corresponding data members of the array via the KVACQParam class.

   init();
   OpenFile(file);
}

//__________________________________________________________________________

void KVINDRARawDataReader::OpenFile(const Char_t * file)
{
   //Open and initialise a raw INDRA data file for reading.
   //By default, Scaler buffers are ignored.
   //If file cannot be opened, this object will be made a zombie. Do not use.
   //To test if file is open, use IsOpen().
   //The basename of the file (excluding any path) can be obtained using GetName()
   //The full pathname of the file can be obtained using GetTitle()
   //
   //The dataset corresponding to the data to be read must be known i.e. gDataSet must be defined and point
   //to the correct dataset. This will allow to build the necessary multidetector object if it has not already
   //been done, and to set the calibration parameters etc. as a function of the run number.
   //If the dataset has not been defined, this object will be made a zombie. Do not use.
   //
   //The multidetector array is initialised according to the current run number (call to KVINDRA::SetParameters).
   //The acquisition parameters are linked to the corresponding data members of the array via the KVACQParam class.

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
   if (!gIndra) {
      gDataSet->BuildMultiDetector();
   }
   gIndra->SetParameters( fGanilData->GetRunNumber() );
   ConnectArrayDataParameters();
}

//__________________________________________________________________________

KVINDRARawDataReader::~KVINDRARawDataReader()
{
   delete fSTAT_EVE_PAR;
   fSTAT_EVE_PAR = 0;
   delete fR_DEC_PAR;
   fR_DEC_PAR = 0;
   delete fVXCONFIG_PAR;
   fVXCONFIG_PAR = 0;
   delete fExtParams;
   if(fGanilData) { delete fGanilData; fGanilData=0; }
}

//__________________________________________________________________________

void KVINDRARawDataReader::ConnectRawDataParameters()
{
   //Private utility method called by KVINDRARawDataReader ctor.
	//fParameters is filled with a KVACQParam for each parameter in the file.
	//These KVACQParam objects are completely independent of those associated
	//with the detectors in KVINDRA. No distinction is made between "known"
	//or "unknown" (non-INDRA) parameters. fParameters is just a complete
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

//__________________________________________________________________________

void KVINDRARawDataReader::ConnectArrayDataParameters()
{
   //Private utility method called by KVINDRARawDataReader ctor.
   // - add parameters for the 3 Selecteur registers to INDRA parameter list
   // - loop over all acquisition parameters
   // - INDRA acquisition parameters are connected to the corresponding detectors
   // - non-INDRA (i.e. unknown) parameters are stored in the list which can be
   //obtained using GetUnknownParameters()

   //add parameters for the 3 Selecteur registers to INDRA parameter list
   gIndra->AddACQParam(fSTAT_EVE_PAR);
   gIndra->AddACQParam(fR_DEC_PAR);
   gIndra->AddACQParam(fVXCONFIG_PAR);

   //loop over all acquisition parameters
   //INDRA acquisition parameters are connected to the corresponding detectors
   //non-INDRA (i.e. unknown) parameters are stored in the list which can be
   //obtained using GetUnknownParameters()
   TIter next( fGanilData->GetListOfDataParameters() );
   KVACQParam *par;
   GTDataPar* daq_par;
   while ((daq_par = (GTDataPar*) next())) {
      if( (par=CheckACQParam( daq_par->GetName() )) ) fGanilData->Connect(par->GetName(), par->ConnectData());
   }
}

//___________________________________________________________________________

KVACQParam* KVINDRARawDataReader::CheckACQParam( const Char_t* par_name )
{
   //Check the named acquisition parameter.
   //We look for a corresponding parameter in the list of acq params belonging to INDRA.
   //If none is found, we create a new acq param which is added to the list of "unknown parameters"
   KVACQParam *par;
   if( !(par = gIndra->GetACQParam( par_name )) ){ // INDRA acq parameter ?
      //create new unknown parameter
      par = new KVACQParam;
      par->SetName( par_name );
      fExtParams->Add( par );
   }
   return par;
}

//___________________________________________________________________________

Bool_t KVINDRARawDataReader::GetNextEvent()
{
   //Read next event in raw data file.
   //Returns false if no event found (end of file).

   Bool_t ok = fGanilData->Next();
   if (ok) {
      //read infos from Selecteur
      SetSTAT_EVE(fSTAT_EVE_PAR);
      SetR_DEC(fR_DEC_PAR);
      SetVXCONFIG(fVXCONFIG_PAR);
   }
   return ok;
}

//___________________________________________________________________________
 
Bool_t KVINDRARawDataReader::IsINDRAEvent()
 {
    //Valid INDRA events have the acquisition parameter STAT_EVE
    //present i.e. not equal to -1.
    //Test this after GetNextEvent() to know if a valid INDRA event has
    //been read.

    return fSTAT_EVE_PAR->Fired();
 }

 //___________________________________________________________________________

 KVINDRARawDataReader* KVINDRARawDataReader::Open(const Char_t* filename, Option_t* opt)
 {
    //Static method, used by KVDataSet::Open
    return new KVINDRARawDataReader(filename);
 }
 
 //___________________________________________________________________________

GTGanilData* KVINDRARawDataReader::NewGanTapeInterface()
{
   //Creates and returns new instance of class used to read GANIL acquisition data
   return new GTGanilData;
}

//___________________________________________________________________________

GTGanilData* KVINDRARawDataReader::GetGanTapeInterface()
{
   return fGanilData;
}
