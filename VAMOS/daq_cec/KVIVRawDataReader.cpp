/*
$Id: KVIVRawDataReader.cpp,v 1.3 2007/11/21 11:22:59 franklan Exp $
$Revision: 1.3 $
$Date: 2007/11/21 11:22:59 $
*/

//Created by KVClassFactory on Fri Jun  1 12:16:31 2007
//Author: John Frankland

#include "KVIVRawDataReader.h"
#include "GTGanilDataVAMOS.h"
#include "KVINDRA.h"

ClassImp(KVIVRawDataReader)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIVRawDataReader</h2>
<h4>Reads raw data from INDRA-VAMOS experiments</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIVRawDataReader::KVIVRawDataReader()
{
   //Default constructor
}

KVIVRawDataReader::KVIVRawDataReader(const Char_t* f)
      : KVINDRARawDataReader()
{
   //open file for reading
   OpenFile(f);
}

KVIVRawDataReader::~KVIVRawDataReader()
{
   //Destructor
}

 //___________________________________________________________________________

GTGanilData* KVIVRawDataReader::NewGanTapeInterface()
{
   //Creates and returns new instance of class used to read GANIL acquisition data
   //for INDRA-VAMOS experiments
   return (GTGanilData*)(new GTGanilDataVAMOS);
}

 //___________________________________________________________________________

 KVIVRawDataReader* KVIVRawDataReader::Open(const Char_t* filename, Option_t* opt)
 {
    //Static method, used by KVDataSet::Open
    return new KVIVRawDataReader(filename);
 }
 
//___________________________________________________________________________

KVACQParam* KVIVRawDataReader::CheckACQParam( const Char_t* par_name )
{
   //Check the named acquisition parameter.
   //We look for a corresponding parameter in the list of acq params belonging to INDRA.
   //As VAMOS acquisition parameters have a specific treatment via the Parameters class,
   //we do not want to stock them as 'unknown parameters', therefore for any acq params not
   //belonging to INDRA we return 0.   
   return gIndra->GetACQParam( par_name );
}

