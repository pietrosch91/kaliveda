//Created by KVClassFactory on Wed Jul 18 11:55:11 2012
//Author: John Frankland,,,

#ifndef __KVSIMDIRGUI_H
#define __KVSIMDIRGUI_H

#include "KVBase.h"
#include "KVList.h"
#include "TGListTree.h"
#include "TGFrame.h"
#include "TGTab.h"
#include "TEnv.h"
#include "TGTextEntry.h"
#include "TGNumberEntry.h"
#include "TGButton.h"
#include "TGComboBox.h"
#include "RQ_OBJECT.h"
#include "KVListView.h"

class TChain;

class KVSimDirGUI : public KVBase
{   
   RQ_OBJECT("KVSimDirGUI")
         
   KVList fListOfDirs;
   
   TGMainFrame *MainFrame;
   /* picture buttons for managing directories */
   TGPictureButton *BaddDir;
   TGPictureButton *BremDir;
   TGPictureButton *BrefreshDir;
   /* list tree for managing directories */
   TGCanvas *fDirListCanvas;
   TGViewPort *fDirListViewPort;
   TGListTree *fDirListTree;
   const TGPicture *popen;       //used for list tree items
   const TGPicture *pclose;      //used for list tree items
   /* lists of data */
   KVListView* fLVsimData;
   KVListView* fLVfiltData;
   /* tabs for filtering, analysing */
   TGTab* fAnalysisTabs;
   TGCompositeFrame* fAnalTab; // analysis tab
   TGCompositeFrame* fFiltTab; // filter tab
   TGTextEntry* fTEAnalysisClassFileName;
   KVString fAnalClassHeader;
   KVString fAnalClassImp;
   KVString fAnalClassDir;
   TGCheckButton* fCBAllEvents;
   TGNumberEntry* fNENumberEvents;
   /* filter */
   TGComboBox* fCBdataset;
   TGComboBox* fCBsystem;
   TGComboBox* fCBrun;
   KVString fDataset;
   KVString fSystem;
   KVString fRun;
   enum {
      kFTGeo=1,
      kFTSeuils,
      kFTFull
   };
   enum {
      kGTKaliVeda=1,
      kGTROOT
   };
   enum {
      kKCM=1,
      kKLab
   };
   Int_t fFilterType;
   Int_t fGeoType;
   Int_t fKine;
   //TGTextEntry* fTEOutputDir;
   
   TEnv fGuirc;
   
   public:
   KVSimDirGUI();
   KVSimDirGUI (const KVSimDirGUI&) ;
   virtual ~KVSimDirGUI();
   void Copy (TObject&) const;

   void AddSimDir();
   void RemSimDir();
   void RefreshSimDir();
   void FillTreeList();
   void EmptyTreeList();
   void SelectSimDir(TGListTreeItem*,Int_t);
   
   void EnableEventNumberEntry(Bool_t);
   void SelectAnalysisClass();
   void RunAnalysis();
   TChain* BuildChain(TList*);
   
   void SelectDataSet(const char*);
   void SelectSystem(const char*);
   void SelectRun(const char*);
   void FilterType(Int_t i)
   {
      fFilterType = i;
   };
   void GeoType(Int_t i)
   {
      fGeoType = i;
   };
   void Kinematics(Int_t i)
   {
      fKine = i;
   };
   //void ChangeOutputDirectory();
   void RunFilter();
   
   ClassDef(KVSimDirGUI,0)//GUI for simulated data
};

#endif
