#ifndef KVRECONRAWDATAANALYSER_H
#define KVRECONRAWDATAANALYSER_H

#include "KVRawDataAnalyser.h"
#include "KVEventReconstructor.h"

class KVReconRawDataAnalyser : public KVRawDataAnalyser {
protected:
   unique_ptr<KVEventReconstructor> fEvRecon;
   KVReconstructedEvent* fRecev;
   Bool_t fHandledRawData;

public:
   KVReconRawDataAnalyser() {}
   virtual ~KVReconRawDataAnalyser() {}

   void preInitAnalysis();
   void preInitRun();
   void preAnalysis();

   KVReconstructedEvent* GetReconstructedEvent() const
   {
      return fEvRecon->GetEvent();
   }
   KVRawDataReader* GetRawDataReader() const
   {
      return fRunFile;
   }

   /// Return kTRUE if detector array handled data in last read event.
   /// In this case the reconstructed event is valid.
   Bool_t HandledRawData() const
   {
      return fHandledRawData;
   }

   static void Make(const Char_t* kvsname);

   ClassDef(KVReconRawDataAnalyser, 0) // Analysis of raw data with reconstruction of events
};

#endif // KVRECONRAWDATAANALYSER_H
