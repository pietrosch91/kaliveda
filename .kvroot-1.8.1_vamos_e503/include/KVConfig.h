//-----------------------------------ROOT-version dependent compiler flags
//-----------------------------------to handle backwards compatibility
//
// if your code uses these functionalities, beware !!
// backwards compatibility is not assured !
// some solutions are given...*/

#ifndef KV_CONFIG__H
#define KV_CONFIG__H

//-------------------TParameter<Int_t> instantiated by ROOT only from v5.04/00 onwards
#define __WITH_TPARAMETER_INT 1

//------------------------New TCollection::Print methods in v5.21.02
#define __WITH_NEW_TCOLLECTION_PRINT 1

//------------------TParameter adds a Merge(TCollection*) method from v5.29.02 onwards
//------------------- TParameter<TString> no longer works
//SOLUTION:   use KVParameter
#define __WITHOUT_TPARAMETER 1

#endif
