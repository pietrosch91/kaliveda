#ifndef __PARAMETER_NAME_H__
#define __PARAMETER_NAME_H__

#include "Rtypes.h"
#include <cstring>

class ParameterName {

   const size_t maxlen_;

public:

   char* Name;

   ParameterName();
   virtual ~ParameterName();

   void Init();

   ClassDef(ParameterName, 1)

};

#endif // __PARAMETER_NAME_H__ not defined

