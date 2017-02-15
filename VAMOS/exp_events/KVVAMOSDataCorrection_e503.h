//Created by KVClassFactory on Mon Feb 13 18:45:07 2017
//Author: Quentin Fable,,,

#ifndef __KVVAMOSDATACORRECTION_E503_H
#define __KVVAMOSDATACORRECTION_E503_H

#include "KVVAMOSDataCorrection.h"

class KVVAMOSDataCorrection_e503 : public KVVAMOSDataCorrection {
protected:

   Bool_t   fkverbose;
   Bool_t   fkInitialised;

   Float_t ftof_corr_sicsi;          //correction of ToF (in ns) for AoQ duplication correction for SiCsI telescopes
   Float_t ftof_corr_icsi;           //correction of ToF (in ns) for AoQ duplication correction for ICSi telescopes
   KVList* flist_aoq_cut_sicsi;       //list of TCutG* for AoQ duplication correction for SiCsI telescopes
   KVList* flist_aoq_cut_icsi;        //list of TCutG* for AoQ duplication correction for ICSi telescopes


   //Si-CsI
   virtual void ReadDuplicationSiCsICutFileListInDataSet();
   virtual void ReadDuplicationICSiCutFileListInDataSet();
   virtual void ReadDuplicationCutFileList(std::ifstream& file, Int_t type);

   virtual Bool_t ApplyAoverQDuplicationCorrections(KVVAMOSReconNuc*, KVList*, Float_t);

public:
   KVVAMOSDataCorrection_e503();
   virtual ~KVVAMOSDataCorrection_e503();

   virtual void Copy(TObject&) const;

   virtual void Init();
   virtual void ApplyCorrections(KVVAMOSReconNuc*);

   virtual void SetVerbose(Bool_t status = kTRUE) {
      fkverbose = status;
   }

   ClassDef(KVVAMOSDataCorrection_e503, 1) //Base class to use for VAMOS data corrections for e503 experiment
};

#endif
