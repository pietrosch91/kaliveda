//Created by KVClassFactory on Sun Jan 20 13:31:10 2013
//Author: John Frankland,,,

#ifndef __KVSIMREADER_ELIE_H
#define __KVSIMREADER_ELIE_H

#include "KVSimReader.h"

class KVSimReader_ELIE : public KVSimReader {
   void init()
   {
      tree_name = "ELIE";
      elie_params = new KVNameValueList;
   }

protected:
   KVNucleus proj, targ;
   Double_t ebeam;
   KVNameValueList* elie_params;
   virtual void define_output_filename();
   void transform_to_cm();

public:
   KVSimReader_ELIE();
   KVSimReader_ELIE(KVString filename);
   virtual ~KVSimReader_ELIE();
   virtual void ConvertEventsInFile(KVString filename);

   void ReadFile();
   Bool_t ReadHeader();
   Bool_t ReadEvent();
   Bool_t ReadNucleus();

   ClassDef(KVSimReader_ELIE, 1) //Read ELIE events before secondary decay
};

#endif
