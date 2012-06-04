//Created by KVClassFactory on Fri Apr 13 12:31:16 2012
//Author: John Frankland

#ifndef __KVTreeAnalyzer_H
#define __KVTreeAnalyzer_H
#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TRootBrowser
#include "TRootBrowser.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCommandPlugin
#include "TGCommandPlugin.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGTextEdit
#include "TGTextEdit.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGView
#include "TGView.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGFileBrowser
#include "TGFileBrowser.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGTextEditor
#include "TGTextEditor.h"
#endif
#ifndef ROOT_TRootCanvas
#include "TRootCanvas.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGuiBldGeometryFrame
#include "TGuiBldGeometryFrame.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif
#ifndef ROOT_TGHtmlBrowser
#include "TGHtmlBrowser.h"
#endif

#include "Riostream.h"
#include "KVListView.h"
#include "TTree.h"
#include "TH1.h"
#include "TEntryList.h"
#include "KVUniqueNameList.h"
#include "TBrowser.h"
#include "KVImpactParameter.h"
#include "KVGumbelDistribution.h"
#include "KVGausGumDistribution.h"

class KVTreeAnalyzer : public TNamed
{


   TTree* fTree;//!
   KVUniqueNameList fSelections;
   /* leaves */
   TGMainFrame *fMain_leaflist;//!
   KVListView* G_leaflist;//!
   TGLabel* G_leaf_expr;//!
   TGCheckButton* G_leaf_swap;//!
   Bool_t fSwapLeafExpr;//!
   TGPictureButton *G_leaf_draw;//!
   TString fLeafExpr;//!
   TList* fSelectedLeaves;//!
   TNamed* fXLeaf;//!
   TNamed* fYLeaf;//!
   TGTextEntry* G_alias_text;//!
   
   /* histos */
   TGMainFrame *fMain_histolist;//!
   KVListView* G_histolist;//!
   TGTextButton* G_histo_del;//!
   TGCheckButton* G_histo_same;//!
   TGCheckButton* G_histo_app_sel;//!
   TGCheckButton* G_histo_log;//!
   TGCheckButton* G_histo_new_can;//!
   TGCheckButton* G_histo_norm;//!
   Bool_t fDrawSame;//!
   Int_t fSameColorIndex;//!
   Bool_t fDrawLog;//!
   Bool_t fApplySelection;//!
   Bool_t fNewCanvas;//!
   Bool_t fNormHisto;//!
   TList* fSelectedHistos;//!
   TGTextButton* G_make_ip_scale;//!
   TGTextButton* G_fit1;//!
   TGTextButton* G_fit2;//!
   TGTextButton* G_fit3;//!
   TGTextButton* G_fit4;//!
   TGTextButton* G_fit5;//!
   TGTextButton* G_fitGG;//!
   TGTextEntry* G_make_ip_selection;//!
   KVImpactParameter* ipscale;//!
   TGLabel* G_ip_histo;//!
   KVGumbelDistribution *GDfirst;//!
   KVGumbelDistribution *GDsecond;//!
   KVGumbelDistribution *GDthird;//!
   KVGumbelDistribution *GD4;//!
   KVGumbelDistribution *GD5;//!
   KVGausGumDistribution *GausGum;//!
   
   /* selections */
   TGMainFrame *fMain_selectionlist;//!
   KVListView* G_selectionlist;//!
   TGStatusBar* G_selection_status;//!
   TGTextEntry* G_selection_text;//!
   TGTextButton* G_selection_but;//!
   TGTextButton* G_update_but;//!
   TList* fSelectedSelections;//!
   
   KVList fHistolist;
   TString fTreeName;
   TString fTreeFileName;
   Int_t fHistoNumber;
   Int_t fSelectionNumber;
   Int_t fAliasNumber;
   KVList fAliasList;
   Bool_t fNoGui;//!
   
   void AddHisto(TH1*);
   void AddSelection(TEntryList*);
   void ReconnectTree();
   void OpenGUI();
   
   void GenerateHistoTitle(TString& title, const Char_t* exp, const Char_t* sel);
   void FillLeafList();
   
   public:
   KVTreeAnalyzer(Bool_t nogui=kFALSE);
   KVTreeAnalyzer(TTree*,Bool_t nogui=kFALSE);
   virtual ~KVTreeAnalyzer();
   
   void SetTree(TTree*t);
   TH1* MakeHisto(const Char_t* expr, const Char_t* selection, Int_t nX, Int_t nY = 0);
   TH1* RemakeHisto(TH1* h, const Char_t* expr);
   TH1* MakeIntHisto(const Char_t* expr, const Char_t* selection, Int_t Xmin, Int_t Xmax);
   const KVList* GetHistoList() const { return &fHistolist; };
   TList* GetHistosByData(const Char_t* expr);
   TList* GetHistosBySelection(const Char_t* expr);
   void ParseHistoTitle(const Char_t* title, TString& exp, TString& sel);
   TH1* GetHisto(const Char_t* expr, const Char_t* selection);
   void DeleteHisto(const Char_t* expr, const Char_t* selection);
   void DeleteSelectedHisto();
   
   void MakeSelection(const Char_t* selection);
   void UpdateEntryLists();
   void GenerateSelection();
   void MakeIPScale();
   void GenerateIPSelection();
   void CombineSelections();
   void SelectionChanged();
   void SetAlias(const Char_t* name, const Char_t* expr){ fAliasList.Add(new TNamed(name,expr));};
   void GenerateAlias();
   void ShowSelections() {fSelections.ls();};
   void ShowVariables() {fTree->GetListOfLeaves()->ls();};
   void ShowAliases() {fAliasList.ls();};
   void CurrentSelection();
   Bool_t IsCurrentSelection(const Char_t* sel);
   void SetSelection(TObject*);
   void SetSelection(const Char_t*);
   
   static KVTreeAnalyzer* OpenFile(const Char_t* filename, Bool_t nogui=kFALSE);
   
   void DrawHisto(TObject*);
   void DrawLeaf(TObject*);
   void DrawLeafExpr();
   
   void SetNormHisto(Bool_t yes=kTRUE){fNormHisto=yes;};
   void SetNewCanvas(Bool_t yes=kTRUE){fNewCanvas=yes;};
   void SetDrawSame(Bool_t yes=kTRUE){fSameColorIndex=0;fDrawSame=yes;};
   void SetDrawLog(Bool_t yes=kTRUE){fDrawLog=yes;};
   void SetApplySelection(Bool_t yes=kTRUE){fApplySelection=yes;};
   void SetSwapLeafExpr(Bool_t yes=kTRUE){fSwapLeafExpr=yes;LeafChanged();};
   void LeafChanged();
   void HistoSelectionChanged();
   void Save();
   void FitGum1();
   void FitGum2();
   void FitGum3();
   void FitGum4();
   void FitGum5();
   void FitGausGum();
   
   ClassDef(KVTreeAnalyzer,1)//KVTreeAnalyzer
};

#endif
