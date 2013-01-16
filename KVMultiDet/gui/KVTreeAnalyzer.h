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
#include "TCutG.h"
#include "TH1.h"
#include "TEntryList.h"
#include "KVUniqueNameList.h"
#include "TBrowser.h"
#include "KVImpactParameter.h"
#include "KVGumbelDistribution.h"
#include "KVGausGumDistribution.h"

class KVTreeAnalyzer : public TNamed
{


   TTree* fTree;//! the analyzed TTree
   KVUniqueNameList fSelections;// list of TEntryList user selections
   /* leaves */
   TGMainFrame *fMain_leaflist;//! GUI for access to TTree leaves and aliases
   KVListView* G_leaflist;//! GUI list of TTree leaves and aliases
   TGLabel* G_leaf_expr;//!
   TGCheckButton* G_leaf_swap;//!
   Bool_t fSwapLeafExpr;//!
   TGPictureButton *G_leaf_draw;//!
   TString fLeafExpr;//!
   TList* fSelectedLeaves;//!
   TNamed* fXLeaf;//!
   TNamed* fYLeaf;//!
   TNamed* fZLeaf;//!
   TGTextEntry* G_alias_text;//!
   TGCheckButton* G_histo_bin;//!
   Bool_t fUserBinning;//!
   TGCheckButton* G_histo_weight;//!
   Bool_t fUserWeight;//!
   const char* fWeight;
   
   Int_t fNx,fNy;//
   Double_t fXmin,fXmax,fYmin,fYmax;//
   Int_t fNxF;//
   Double_t fXminF,fXmaxF;//
   
   Int_t fNxD, fNyD;//
   Int_t fOrderedDalitz;//
   
   /* histos */
   TGMainFrame *fMain_histolist;//! GUI for handling histograms
   TGPopupMenu *fMenuFile;//!
   enum {
   		MH_OPEN_FILE,
   		MH_SAVE_FILE,
   		MH_QUIT
   };
   void HistoFileMenu_Open();
   void HistoFileMenu_Save();
   TGLayoutHints *fMenuBarItemLayout;//!
   TGMenuBar *fMenuBar;//!
   KVListView* G_histolist;//! GUI list of histograms
   TGTextButton* G_histo_del;//!
   TGCheckButton* G_histo_same;//!
   TGCheckButton* G_histo_app_sel;//!
   TGCheckButton* G_histo_log;//!
   TGCheckButton* G_histo_new_can;//!
   TGCheckButton* G_histo_norm;//!
   Bool_t fDrawSame;//! =kTRUE: draw histograms in same plot
   Int_t fSameColorIndex;//!
   Bool_t fDrawLog;//! =kTRUE: draw histograms with log-Y (1-D) or log-Z (2-D) scale
   Bool_t fApplySelection;//! =kTRUE: apply current selection to existing histogram
   Bool_t fNewCanvas;//! =kTRUE: draw each histogram in a new canvas
   Bool_t fNormHisto;//! =kTRUE: generate normalised histograms
   TList* fSelectedHistos;//!
   TGTextButton* G_make_ip_scale;//!
   TGTextButton* G_fit1;//!
   TGTextButton* G_fit2;//!
   TGTextButton* G_fit3;//!
   TGTextButton* G_fitGG1;//!
   TGTextButton* G_fitGG2;//!
   TGTextButton* G_fitGG3;//!
   TGTextEntry* G_make_ip_selection;//!
   KVImpactParameter* ipscale;//!
   TGLabel* G_ip_histo;//!
   KVGumbelDistribution *GDfirst;//!
   KVGumbelDistribution *GDsecond;//!
   KVGumbelDistribution *GDthird;//!
   KVGausGumDistribution *GausGum1;//!
   KVGausGumDistribution *GausGum2;//!
   KVGausGumDistribution *GausGum3;//!
   
   /* selections */
   TGMainFrame *fMain_selectionlist;//! GUI for handling selections
   KVListView* G_selectionlist;//! GUI list of TEntryList selections
   TGStatusBar* G_selection_status;//! status bar in selections GUI
   TGTextEntry* G_selection_text;//!
   TGTextButton* G_selection_but;//!
   TGTextButton* G_selection_but_or;//!
   TGTextButton* G_update_but;//!
   TGTextButton* G_delete_but;//!
   TList* fSelectedSelections;//!
   
   KVList fHistolist;//list of generated histograms
   TString fTreeName;//name of analyzed TTree
   TString fTreeFileName;//name of file containing analyzed TTree
   Int_t fHistoNumber;//used for automatic naming of histograms
   Int_t fSelectionNumber;//used for automatic naming of selections
   Int_t fAliasNumber;//used for automatic naming of TTree aliases
   KVList fAliasList;//list of TTree aliases
   Bool_t fNoGui;//! =kTRUE if no graphical interface is required
   
   void AddHisto(TH1*);
   void AddCut(TCutG*);
   void AddSelection(TEntryList*);
   void ReconnectTree();
   
   void GenerateHistoTitle(TString& title, const Char_t* exp, const Char_t* sel);
   void FillLeafList();
   
   public:
   KVTreeAnalyzer(Bool_t nogui=kTRUE);
   KVTreeAnalyzer(TTree*,Bool_t nogui=kFALSE);
   virtual ~KVTreeAnalyzer();
   
   void Copy(TObject& obj) const;
   
   void SetTree(TTree*t);
   void OpenGUI();
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
   
   Bool_t MakeSelection(const Char_t* selection);
   void UpdateEntryLists();
   void GenerateSelection();
   void MakeIPScale();
   void GenerateIPSelection();
   void CombineSelectionsAnd();
   void CombineSelectionsOr();
   void DeleteSelections();
   void SelectionChanged();
   void SetAlias(const Char_t* name, const Char_t* expr){ fAliasList.Add(new TNamed(name,expr));};
   void GenerateAlias();
   void ShowSelections() {fSelections.ls();};
   void ShowVariables() {fTree->GetListOfLeaves()->ls();};
   void ShowAliases() {fAliasList.ls();};
   void ShowHistos() {fHistolist.ls();};
   void CurrentSelection();
   Bool_t IsCurrentSelection(const Char_t* sel);
   void SetSelection(TObject*);
   void SetSelection(const Char_t*);
   
   static KVTreeAnalyzer* OpenFile(const Char_t* filename, Bool_t nogui=kFALSE);
   void ReadFromFile(const Char_t* filename);
   void ReadFromFile(TFile* f);
   void OpenAnyFile(const Char_t* filepath);
   
   void DrawHisto(TObject* o, Bool_t gen=kTRUE);
   void DrawCut(TCutG*);
   void DrawLeaf(TObject*);
   void DrawLeafExpr();
   void DrawAsDalitz();
   void SetUserBinning(Bool_t ub){fUserBinning = ub;};
   void SetUserWeight(Bool_t uw){fUserWeight = uw;};

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
   void FitGausGum1();
   void FitGausGum2();
   void FitGausGum3();
   
   Int_t GetNx(){return fNx;};
   Int_t GetNy(){return fNy;};
   Double_t GetXmin(){return fXmin;};
   Double_t GetXmax(){return fXmax;};
   Double_t GetYmin(){return fYmin;};
   Double_t GetYmax(){return fYmax;};
   
   Int_t GetNxD(){return fNxD;};
   Int_t GetNyD(){return fNxD;};
   Bool_t GetOrderedDalitz(){return fOrderedDalitz;};
   
   Int_t GetNxF(){return fNxF;};
   Double_t GetXminF(){return fXminF;};
   Double_t GetXmaxF(){return fXmaxF;};
   
   const char* GetWeight(){return fWeight;};
   
   void DefineUserBinning(Int_t Nx, Int_t Ny, Double_t Xmin, Double_t Xmax, Double_t Ymin, Double_t Ymax);// *MENU* *ARGS={Nx=>fNx,Ny=>fNy,Xmin=>fXmin,Xmax=>fXmax,Ymin=>fYmin,Ymax=>fYmax} 
   void DefineUserBinning1F(Int_t NxF, Double_t XminF, Double_t XmaxF);// *MENU* *ARGS={NxF=>fNxF,XminF=>fXminF,XmaxF=>fXmaxF} 
   void DefineUserBinningD(Int_t NxD, Int_t NyD, Int_t ordered);// *MENU* *ARGS={NxD=>fNxD,NyD=>fNyD,ordered=>fOrderedDalitz}
   void DefineWeight(const char* Weight);// *MENU* *ARGS={Weight=>fWeight} 
   
   
   void HandleHistoFileMenu(Int_t); 
   ClassDef(KVTreeAnalyzer,1)//KVTreeAnalyzer
};

#endif
