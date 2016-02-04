#ifndef __MINIMISER_DATA_H__
#define __MINIMISER_DATA_H__

#include "Rtypes.h"

class MinimiserData {
public:

   MinimiserData();
   virtual ~MinimiserData();

   // Accessors
   Int_t GetZ() const;
   Int_t GetA() const;
   Double_t GetDelta() const;
   Int_t GetStatusCode() const;

   // Mutators
   void SetZ(Int_t value);
   void SetA(Int_t value);
   void SetDelta(Double_t value);
   void SetStatusCode(Int_t value);

private:

   // All minimisers must provide at least these values
   Int_t z_value_;
   Int_t a_value_;
   Double_t delta_;
   Int_t status_code_;

   ClassDef(MinimiserData, 1)
};

#endif
