/*
$Id: KVUpDater.cpp,v 1.4 2006/10/19 14:32:43 franklan Exp $
$Revision: 1.4 $
$Date: 2006/10/19 14:32:43 $
$Author: franklan $
*/

#include "KVUpDater.h"
#include "TPluginManager.h"
#include "KVBase.h"

ClassImp(KVUpDater)
//////////////////////////////////////////////////////////////////////////////////
//
//KVUpDater
//
//Abstract class implementing necessary methods for setting multidetector parameters
//for each run of the current dataset, gDataSet
//
//In specific implementations for different detectors, the method
//      void SetParameters (UInt_t run)
//must be defined. This updates the current detector array gMultiDetArray with the
//parameter values for the run found in the current gDataBase database.
    KVUpDater::KVUpDater()
{
   //Default ctor for KVUpDater object.
}

//_______________________________________________________________//

KVUpDater::~KVUpDater()
{
   //Destructor.
}

KVUpDater *KVUpDater::MakeUpDater(const Char_t * uri)
{
   //Looks for plugin (see $KVROOT/KVFiles/.kvrootrc) with name 'uri'(=name of dataset),
   //loads plugin library, creates object and returns pointer.
   //check and load plugin library
   TPluginHandler *ph;
   if (!(ph = KVBase::LoadPlugin("KVUpDater", uri)))
      return 0;

   KVUpDater *upd = (KVUpDater *) ph->ExecPlugin(0);
   upd->fDataSet = uri;
   return upd;
}
