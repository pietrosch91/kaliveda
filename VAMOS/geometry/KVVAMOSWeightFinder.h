//Created by KVClassFactory on Thu Feb  9 00:40:28 2017
//Author: Quentin Fable,,,

#ifndef __KVVAMOSWEIGHTFINDER_H
#define __KVVAMOSWEIGHTFINDER_H

#include "KVBase.h"
#include "KVNumberList.h"
#include "Riostream.h"

#include <vector>
#include <cassert>

class KVVAMOSWeightFinder : public KVBase {
protected:
   Bool_t fkverbose;
   KVNumberList fRunList;
   std::vector< std::vector<Float_t> > fvec_infos;

   void ReadRunListInDataSet();
   void ReadInformations(std::ifstream& file);


   void SortInfoVector();
   static Bool_t SortVectors(const std::vector<Float_t>& vector1, const std::vector<Float_t>& vector2);
   std::vector<Float_t> GetInfoLine(Int_t line_number) {
      return fvec_infos.at(line_number);
   }
   Float_t GetInfoValue(Int_t run_number, Int_t num_value);

public:
   KVVAMOSWeightFinder(KVNumberList& nl);
   KVVAMOSWeightFinder(const KVVAMOSWeightFinder&);
   virtual ~KVVAMOSWeightFinder();
   virtual void Copy(TObject&) const;

   Int_t   GetRunPosition(Int_t run_number);
   Float_t GetScalerINDRA(Int_t run_number) {
      return GetInfoValue(run_number, 4);
   }
   Float_t GetDeadTime(Int_t run_number) {
      return GetInfoValue(run_number, 3);
   }
   Float_t GetThetaVamos(Int_t run_number) {
      return GetInfoValue(run_number, 2);
   }
   Float_t BrhoRef(Int_t run_number) {
      return GetInfoValue(run_number, 1);
   }

   void SetVerbose(Bool_t verbose) {
      fkverbose = verbose;
   }

   ClassDef(KVVAMOSWeightFinder, 1) //Class used to estimate normalisation weights with VAMOS events
};

#endif
