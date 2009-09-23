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
}

//__________________________________________________________________________

KVINDRARawDataReader::KVINDRARawDataReader(const Char_t * file)
         : KVGANILDataReader()
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

KVINDRARawDataReader::~KVINDRARawDataReader()
{
   delete fSTAT_EVE_PAR;
   fSTAT_EVE_PAR = 0;
   delete fR_DEC_PAR;
   fR_DEC_PAR = 0;
   delete fVXCONFIG_PAR;
   fVXCONFIG_PAR = 0;
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
   
   KVGANILDataReader::ConnectArrayDataParameters();
}

//___________________________________________________________________________

Bool_t KVINDRARawDataReader::GetNextEvent()
{
   //Read next event in raw data file.
   //Returns false if no event found (end of file).

   Bool_t ok = KVGANILDataReader::GetNextEvent();
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
