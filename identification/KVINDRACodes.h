//$Id: KVINDRACodes.h,v 1.3 2008/02/14 10:30:18 franklan Exp $
//Author: $Author: franklan $

#ifndef __KVINDRACODES__
#define __KVINDRACODES__

#include "TObject.h"
#include "KVINDRACodeMask.h"
#include "KVIDSubCodeManager.h"

class KVINDRACodes:public KVINDRACodeMask {
 private:

   static Char_t fCodeGenIdent[14][120];
   static Char_t fCodeGenCalib[8][120];
   static UChar_t fIDCodes[14];
   static UChar_t fECodes[8];
   static UShort_t fIDBits[16];
   static UChar_t fEBits[16];

   KVIDSubCode fIDSubCodes;     //contains all subcodes returned by different identification telescopes

 public:

   enum {
      kIsoRes = BIT(14)         //isotopic resolution achieved i.e. mass was measured
   };

    KVINDRACodes();
    virtual ~ KVINDRACodes() {
      fIDSubCodes = fIDSubCodes.Max();
   };
   const Char_t *GetIDStatus();
   const Char_t *GetEStatus();
   void SetIsotopeResolve(Bool_t stat = kTRUE);
   Bool_t GetIsotopeResolve();

   static UShort_t VedaIDCodeToBitmask(UChar_t veda_id_code);
   static UChar_t VedaECodeToBitmask(UChar_t veda_e_code);

   UChar_t GetCodeIndex(UShort_t);
   UChar_t GetVedaIDCode();
   UChar_t GetVedaECode();
   void SetVedaIDCode(UChar_t);
   void SetVedaECode(UChar_t);
   UShort_t GetIDCode();
   UChar_t GetECode();
   void SetIDCode(UShort_t);
   void SetECode(UChar_t);

   inline KVIDSubCode & GetSubCodes() {
      return fIDSubCodes;
   };

   void Clear(Option_t * opt = "");

   ClassDef(KVINDRACodes, 2)    //Particle identification and calibration status (Veda6)
};

#endif
