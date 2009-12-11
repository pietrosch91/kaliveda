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

   KVHashList *fParameters;//list of all data parameters contained in file
   KVHashList *fExtParams;//list of data parameters in file not defined by gMultiDetArray
   
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
   
   const KVSeqCollection* GetUnknownParameters() const { return fExtParams; };
   const KVSeqCollection* GetRawDataParameters() const { return fParameters; };

	KVSeqCollection* GetFiredDataParameters()
	{
		// Returns list of fired data parameters in event last read by call to GetNextEvent().
		// DELETE this list after use !!
		return fParameters->GetSubListWithMethod("1", "Fired");
	};
    
   static KVGANILDataReader* Open(const Char_t* filename, Option_t* opt = "");   

   ClassDef(KVGANILDataReader,0)//Reads GANIL acquisition files
};

#endif
