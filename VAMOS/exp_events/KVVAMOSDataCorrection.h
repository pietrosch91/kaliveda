//Created by KVClassFactory on Mon Feb 13 18:44:49 2017
//Author: Quentin Fable,,,

#ifndef __KVVAMOSDATACORRECTION_H
#define __KVVAMOSDATACORRECTION_H

#include "KVBase.h"
#include "KVVAMOSReconNuc.h"
#include "KVRList.h"
#include "KVList.h"

class KVVAMOSDataCorrection : public KVBase {
protected:
   KVList* fRecords;
   TString fDataSet;  //!name of dataset associated

public:
   KVVAMOSDataCorrection();
   virtual ~KVVAMOSDataCorrection();

   // This functions add/call all the identification correction parameter
   // records specified in 'records' to the protected member KVRList 'fRecords'
   virtual Bool_t  SetIDCorrectionParameters(const KVRList* const records);
   virtual const KVList* GetIDCorrectionParameters() const;

   // Initialiser to modify in child class if needed
   virtual Bool_t Init();

   virtual void ApplyCorrections(KVVAMOSReconNuc*);

   static KVVAMOSDataCorrection* MakeDataCorrection(const Char_t* uri);

   ClassDef(KVVAMOSDataCorrection, 1) //Base class to use for VAMOS data corrections
};

#endif
