// $Id: KVParameter.h,v 1.2 2009/01/16 14:55:20 franklan Exp $
// Author: Maarten Ballintijn   21/06/2004

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef __KVPARAMETER_H
#define __KVPARAMETER_H

#include "KVConfig.h"

#ifdef __WITHOUT_TPARAMETER
//TParameter::SetVal didn't exist before v4.03/04 and GetVal returned wrong type
//therfore we replace TParameter with KVParameter which is an exact copy of
//TParameter from ROOT v5.13/05
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// KVParameter<AParamType>                                               //
//                                                                      //
// Named parameter, streamable and storable.                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TObject
#include "TObject.h"
#endif

#ifndef ROOT_TString
#include "TString.h"
#endif


template <class AParamType>
class KVParameter : public TObject {

private:
   TString     fName;
   AParamType  fVal;

public:
   KVParameter()
   {
      /* leave fVal uninitialized */
   }
   KVParameter(const char* name, const AParamType& val)
      : fName(name), fVal(val) { }
   ~KVParameter() { }

   const char*       GetName() const
   {
      return fName;
   }
   const AParamType& GetVal() const
   {
      return fVal;
   }
   void              SetVal(const AParamType& val)
   {
      fVal = val;
   }

   ClassDef(KVParameter, 1) //Named templated parameter type
};
#else
//for ROOT versions with 'correct' TParameter, we use KVParameter just as
//an alias
#include "TParameter.h"
#define KVParameter TParameter
#endif
#endif
