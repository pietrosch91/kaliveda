//Created by KVClassFactory on Wed Sep 23 16:07:38 2009
//Author: Chbihi 

#ifndef __KVGANILDATAREADER_H
#define __KVGANILDATAREADER_H

#include "KVRawDataReader.h"
#include "KVBase.h"
#include "KVACQParam.h"
#include "KVHashList.h"

class GTGanilData;

class KVGANILDataReader : public KVRawDataReader
{
   protected:  
   GTGanilData* fGanilData;//object used to read GANIL acquisition file
   
   virtual void ConnectRawDataParameters();
   virtual void ConnectArrayDataParameters();

   KVHashList *fExtParams;//list of unknown data parameters (not defined in KVMultiDetArray object)
   KVHashList *fParameters;//->list of all data parameters contained in file
   
   virtual GTGanilData* NewGanTapeInterface();
   virtual KVACQParam* CheckACQParam(const Char_t*);   

   public:
   KVGANILDataReader() { init(); };
   KVGANILDataReader(const Char_t*);
   virtual void OpenFile(const Char_t *);
   virtual ~KVGANILDataReader();

   void init();
   virtual Bool_t GetNextEvent();
   virtual GTGanilData* GetGanTapeInterface();
   
   const TCollection* GetUnknownParameters() const { return fExtParams; };
   const TCollection* GetRawDataParameters() const { return fParameters; };
   
   static KVGANILDataReader* Open(const Char_t* filename, Option_t* opt = "");   

   ClassDef(KVGANILDataReader,0)//Reads GANIL acquisition files
};

#endif
