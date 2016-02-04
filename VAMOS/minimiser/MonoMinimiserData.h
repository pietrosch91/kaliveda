#ifndef __MONO_MINIMISER_DATA_H__
#define __MONO_MINIMISER_DATA_H__

#include "MinimiserData.h"

class MonoMinimiserData : public MinimiserData {
public:

   MonoMinimiserData();
   virtual ~MonoMinimiserData();

   Int_t GetForwardCounter() const;
   Int_t GetBackwardCounter() const;

   void SetForwardCounter(Int_t value);
   void SetBackwardCounter(Int_t value);

private:

   Int_t forward_counter_;
   Int_t backward_counter_;

   ClassDef(MonoMinimiserData, 1)
};

#endif
