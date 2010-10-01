//Created by KVClassFactory on Thu Sep 30 16:48:24 2010
//Author: John Frankland,,,,

#ifndef __KVPARTITIONFUNCTION_H
#define __KVPARTITIONFUNCTION_H

#include "TObject.h"

class KVPartitionFunction : public TObject
{
   #define SNEPPENMAXTAB 100
    UInt_t Nclass[SNEPPENMAXTAB][SNEPPENMAXTAB][SNEPPENMAXTAB][SNEPPENMAXTAB];
    UInt_t Np[SNEPPENMAXTAB][SNEPPENMAXTAB/2][SNEPPENMAXTAB];
    
    Double_t maxvalueNp;
    Double_t maxvalueNclass;
    Double_t NvalsNp;
    Double_t NvalsNcl;
    
    Double_t sneppen_Nclass(int A, int Z, int M, int B);
    Double_t sneppen_Np(int A, int Z, int M);
    Double_t calc_sneppen_Nclass(int A, int Z, int M, int B);
    Double_t calc_sneppen_Np(int A, int Z, int M);
    void init();
    
   public:
   KVPartitionFunction();
   virtual ~KVPartitionFunction();
    
    Double_t PartFunc(int A, int M);
    Double_t PartSum(int A);
    Double_t PartFunc(int A, int Z, int M);
    Double_t PartSum(int A, int Z);
    
    Double_t GetMaxValueNp() const { return maxvalueNp; };
    Double_t GetMaxValueNclass() const { return maxvalueNclass; };
    
    /*Double_t MeanNA(int A0, int A);
    Double_t MeanNA(int A0, int Z0, int A);
    Double_t MeanNZ(int A0, int Z0, int Z);
    Double_t MeanNAZ(int A0, int Z0, int A, int Z);
*/
   ClassDef(KVPartitionFunction,0)//Calculates number of partitions of A or (A,Z) into fragments
};

#endif
