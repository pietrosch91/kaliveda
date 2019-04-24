#ifndef KVRECONRAWDATAANALYSER_H
#define KVRECONRAWDATAANALYSER_H

#include "KVRawDataAnalyser.h"
#include "KVEventReconstructor.h"

class KVReconRawDataAnalyser : public KVRawDataAnalyser {
protected:
   unique_ptr<KVEventReconstructor> fEvRecon;
   KVReconstructedEvent* fRecev;

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

   static void Make(const Char_t* kvsname);

   ClassDef(KVReconRawDataAnalyser, 0) // Analysis of raw data with reconstruction of events
};

#endif // KVRECONRAWDATAANALYSER_H
