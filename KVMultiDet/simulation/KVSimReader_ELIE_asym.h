//Created by KVClassFactory on Wed Jan 11 10:29:05 2017
//Author: John Frankland,,,

#ifndef __KVSIMREADER_ELIE_ASYM_H
#define __KVSIMREADER_ELIE_ASYM_H

#include "KVSimReader_ELIE.h"

class KVSimReader_ELIE_asym : public KVSimReader_ELIE {
protected:
   void define_output_filename();
public:
   KVSimReader_ELIE_asym();
   KVSimReader_ELIE_asym(KVString filename);

   virtual ~KVSimReader_ELIE_asym();
   void ConvertEventsInFile(KVString filename);
   Bool_t ReadNucleus();

   ClassDef(KVSimReader_ELIE_asym, 1) //Read ELIE events after secondary decay
};

#endif
