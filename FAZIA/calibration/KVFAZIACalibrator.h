//Created by KVClassFactory on Thu Dec  3 15:59:09 2015
//Author: bonnet,,,

#ifndef __KVFAZIACALIBRATOR_H
#define __KVFAZIACALIBRATOR_H

#include "KVCalibrator.h"
#include "TF1.h"

class KVFAZIACalibrator : public KVCalibrator {

protected:

   static UInt_t fNCalibFunc; //!counts number of existing calibration functions
   TF1* fFunc; //calibration function

public:

   KVFAZIACalibrator();
   KVFAZIACalibrator(const Char_t* name, const Char_t* type);
   void SetFunction(TF1*);
   void SetFunction(TString);
   void ChangeParameters(Double_t*);

   virtual ~KVFAZIACalibrator();
   TF1* GetFunction() const
   {
      return fFunc;
   }

   virtual Double_t Compute(Double_t) const;
   virtual Double_t operator()(Double_t);
   virtual Double_t Invert(Double_t);
   virtual void   Print(Option_t* opt = "") const;

   ClassDef(KVFAZIACalibrator, 1) //Basic class for FAZIA calibrators
};

#endif
