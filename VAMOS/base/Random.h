#ifndef _RANDOM_CLASS
#define _RANDOM_CLASS

#include "Rtypes.h"
#include"Defines.h"

class Random {
public:
   Random(void);
   virtual ~Random(void);

private:
   Float_t* Array;
   Float_t* Ptr;

public:
   Float_t Next(void);
   Float_t Value(void);

   ClassDef(Random, 0)
};
#endif

