//Created by KVClassFactory on Thu Sep 30 16:48:24 2010
//Author: John Frankland,,,,

#ifndef __KVPARTITIONFUNCTION_H
#define __KVPARTITIONFUNCTION_H

#include "Rtypes.h"

class KVPartitionFunction
{
    Double_t sneppen_Nclass(int A, int Z, int M, int B);
    Double_t sneppen_Np(int A, int Z, int M);
    Double_t calc_sneppen_Nclass(int A, int Z, int M, int B);
    Double_t calc_sneppen_Np(int A, int Z, int M);

   public:
   KVPartitionFunction();
   virtual ~KVPartitionFunction();
    
    Double_t PartFunc(int A, int M);
    Double_t PartSum(int A);
    Double_t PartFunc(int A, int Z, int M);
    Double_t PartSum(int A, int Z);

   ClassDef(KVPartitionFunction,0)//Calculates number of partitions of A or (A,Z) into fragments
};

#endif
