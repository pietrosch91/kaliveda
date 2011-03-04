//Created by KVClassFactory on Thu Feb  3 10:04:41 2011
//Author: frankland,,,,

#include "KVIonRangeTable.h"
#include <TPluginManager.h>
#include <TError.h>

ClassImp(KVIonRangeTable)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIonRangeTable</h2>
<h4>Abstract base class for calculation of range & energy loss of charged particles in matter</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIonRangeTable::KVIonRangeTable()
{
   // Default constructor
}

KVIonRangeTable::~KVIonRangeTable()
{
   // Destructor
}

KVIonRangeTable* KVIonRangeTable::GetRangeTable(const Char_t* name)
{
   // Generates an instance of the KVIonRangeTable plugin class corresponding to given name.

   TPluginHandler *ph;
   //check and load plugin library
   if (!(ph = LoadPlugin("KVIonRangeTable", name))) {
      ::Error("KVIonRangeTable::GetRangeTable", "No plugin for KVIonRangeTable with name=%s found in .kvrootrc", name);
      return 0;
   }
   KVIonRangeTable *irt = (KVIonRangeTable *) ph->ExecPlugin(0);
   return irt;
}
