#ifndef __THREADED_MINIMISER_DATA_H__
#define __THREADED_MINIMISER_DATA_H__

#include "MinimiserData.h"

class ThreadedMinimiserData : public MinimiserData {
public:

   ThreadedMinimiserData();
   virtual ~ThreadedMinimiserData();

   Int_t GetForwardCounter() const;
   Int_t GetBackwardCounter() const;

   void SetForwardCounter(Int_t value);
   void SetBackwardCounter(Int_t value);

private:

   Int_t forward_counter_;
   Int_t backward_counter_;

   ClassDef(ThreadedMinimiserData, 1)
};

#endif
