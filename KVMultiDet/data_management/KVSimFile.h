//Created by KVClassFactory on Tue Jul 17 09:05:17 2012
//Author: John Frankland,,,

#ifndef __KVSIMFILE_H
#define __KVSIMFILE_H

#include "KVBase.h"
#include "TROOT.h"

class KVSimDir; 

class KVSimFile : public KVBase
{
   protected:
   KVSimDir* fSimDir;//parent directory
   Bool_t fFiltered;//=kTRUE for filtered (reconstructed) simulated events
   Long64_t fEvents;//=number of events in TTree
   TString fDataSet;//name of dataset used to filter events
   TString fSystem;//name of system used to filter events
   Int_t fRunNumber;//run number used to filter events
   TString fGeoType;//type of geometry used for filtering
   TString fTreeName;//name of TTree
   TString fBranchName;//name of branch containing events
   
   public:   
   KVSimFile();
   KVSimFile(KVSimDir* parent, const Char_t* filename, const Char_t* treeinfo, Long64_t treeEntries, const Char_t* treename, const Char_t* branchname);
   KVSimFile(KVSimDir* parent, const Char_t* filename, const Char_t* treeinfo, Long64_t treeEntries, const Char_t* treename, const Char_t* branchname,
                        const Char_t* dataset, const Char_t* system, Int_t run_number, const Char_t* geo_type);
   KVSimFile (const KVSimFile&) ;
   virtual ~KVSimFile();
   void Copy (TObject&) const;
   
   KVSimDir* GetSimDir() const { return fSimDir; };
   Long64_t GetEvents() const { return fEvents; };
   const Char_t* GetDataSet() const { return fDataSet; };
   const Char_t* GetSystem() const { return fSystem; };
   Int_t GetRun() const { return fRunNumber; };
   const Char_t* GetGeometry() const { return fGeoType; };
   const Char_t* GetTreeName() const { return fTreeName; };
   const Char_t* GetBranchName() const { return fBranchName; };
   
   void ls(Option_t* opt="") const;

   ClassDef(KVSimFile,1)//Handle file containing simulated or filtered simulated data
};

#endif
