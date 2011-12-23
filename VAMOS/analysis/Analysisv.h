#ifndef _ANALYSIS_CLASS
#define _ANALYSIS_CLASS

#include "Rtypes.h"
#include "Defines.h"
#include "LogFile.h"

#include "KVTelescope.h"
#include "KVSiliconVamos.h"
#include "PlaneAbsorber.h"
#include "KVCsIVamos.h"
#include "KVDetector.h"

class TTree;
class TFile;

class Analysisv
{
 public:
  LogFile *L;
  
  TFile *inTree;
  TTree *inT;
  Int_t innEntries;
  Int_t totalEntries;
  TFile *outTree;
  TTree *outT;
  
  Analysisv(LogFile* Log){
     L=Log;
     totalEntries = 0;
  };
  virtual ~Analysisv(){ delete L; };

  virtual void OpenInputTree(TTree* );
  virtual void CloseInputTree(const char *inTreeName);
  virtual void OpenOutputTree(TTree*);
  virtual void CloseOutputTree(const char *outTreeName);
  static Analysisv* NewAnalyser(const Char_t *dataset,LogFile*Log);
  
  virtual void inAttach()=0; //Attaching the variables 
  virtual void outAttach()=0; //Attaching the variables 
  virtual void Treat()=0; // Treating data
  virtual void CreateHistograms()=0;
  virtual void FillHistograms()=0;
  
  virtual void SetRunFlag(Int_t)=0; 
  virtual Int_t GetRunFlag(void)=0;
  virtual void SetTel1(KVDetector *si)=0;
  virtual void SetTel2(KVDetector *gap)=0;  
  virtual void SetTel3(KVDetector *csi)=0;  
  ClassDef(Analysisv,0)

};

#endif
