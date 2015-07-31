 
 #include "RooCmdArg.h"
 
 namespace MyGlobalFunc {
 
  RooCmdArg SetEpsilon(Double_t eps) {return RooCmdArg("SetEpsilon",0,0,eps,0,0,0,0,0) ; };
  RooCmdArg SetMaxIter(Int_t maxiter) {return RooCmdArg("SetMaxIter",maxiter,0,0,0,0,0,0,0) ; };
  RooCmdArg SetMaxCalls(Int_t maxcalls) {return RooCmdArg("SetMaxCalls",maxcalls,0,0,0,0,0,0,0) ; };
  
  }
