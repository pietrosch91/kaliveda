//Created by KVClassFactory on Fri Apr 13 12:31:16 2012
//Author: John Frankland

#include "KVTreeAnalyzer.h"
#include "TString.h"
#include "TDirectory.h"
#include "TSystem.h"
#include "TEntryList.h"
#include "Riostream.h"
#include "KVCanvas.h"
#include "TCutG.h"
#include "TLeaf.h"
#include "TPaveStats.h"
#include "TSystem.h"
#include "TPad.h"
#include "TKey.h"
#include "TROOT.h"
#include "TGMsgBox.h"
#include "KVFileDialog.h"
#include "KVDalitzPlot.h"
#include "TEnv.h"
#include "TColor.h"
#include <KVHistogram.h>
#include <TChain.h>
using namespace std;

ClassImp(KVTreeAnalyzer)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVTreeAnalyzer</h2>
<h4>KVTreeAnalyzer</h4>
<h5>Configure initial state of interface</h5>
In .kvrootrc file, user can configure the following options, corresponding to check boxes in the interface:<br>
<pre>
KVTreeAnalyzer.LogScale:         off
KVTreeAnalyzer.UserBinning:           off
KVTreeAnalyzer.UserWeight:       off
KVTreeAnalyzer.NewCanvas:      off
KVTreeAnalyzer.Normalize:      off
KVTreeAnalyzer.Stats:      off
</pre>
Change value to 'on' if required.
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////


/* colours used for displaying several 1-D spectra on same plot */
#define MAX_COLOR_INDEX 5
Int_t my_color_array[] = {
   kBlue+2,
   kRed+2,
   kGreen+3,
   kCyan-2,
   kOrange+7,
   kViolet+2
};
   
KVTreeAnalyzer *gTreeAnalyzer = 0x0;

KVTreeAnalyzer::KVTreeAnalyzer(Bool_t nogui)
   : TNamed("KVTreeAnalyzer", "KVTreeAnalyzer"), fTree(0), fSelections(kTRUE), fHistoNumber(1), fSelectionNumber(1), fAliasNumber(1)
{
   // Default constructor - used when loading from a file.
   // The 'nogui' option (default=kTRUE) controls whether or not to
   // launch the graphical interface
   
    gTreeAnalyzer = this;

   KVBase::InitEnvironment();
   fMain_histolist=0;
   fMain_leaflist=0;
   fMain_selectionlist=0;
   fMenuFile=0;
   fDrawSame = fApplySelection = fProfileHisto = kFALSE;
   fDrawLog = gEnv->GetValue("KVTreeAnalyzer.LogScale", kFALSE);
   fUserBinning = gEnv->GetValue("KVTreeAnalyzer.UserBinning", kFALSE);
   fUserWeight =  gEnv->GetValue("KVTreeAnalyzer.UserWeight", kFALSE);
   fNewCanvas = gEnv->GetValue("KVTreeAnalyzer.NewCanvas", kFALSE);
   fNormHisto = gEnv->GetValue("KVTreeAnalyzer.Normalize", kFALSE);
   fStatsHisto = gEnv->GetValue("KVTreeAnalyzer.Stats", kFALSE);
   fAutoSaveHisto = kFALSE;
   fSameColorIndex=0;
   fSelectedSelections=0;
   fSelectedLeaves=0;
   fSelectedHistos=0;
   ipscale=0;
      GDfirst=new KVGumbelDistribution("Gum1",1);
      GDsecond=new KVGumbelDistribution("Gum2",2);
      GDthird=new KVGumbelDistribution("Gum3",3);
      GausGum1=new KVGausGumDistribution("GausGum1",1);
      GausGum2=new KVGausGumDistribution("GausGum2",2);
      GausGum3=new KVGausGumDistribution("GausGum3",3);
      fNoGui=nogui;
//    fNx = fNy = 0;
//    fXmin = fXmax = fYmin = fYmax = 0.;
//    fWeight = "";
   SetAnalysisModifiedSinceLastSave(kFALSE);
      OpenGUI();
}


KVTreeAnalyzer::KVTreeAnalyzer(TTree*t,Bool_t nogui)
   : TNamed("KVTreeAnalyzer", t->GetTitle()), fTree(t), fSelections(kTRUE), fHistoNumber(1), fSelectionNumber(1), fAliasNumber(1)
{
   // Initialize analyzer for a given TTree.
   // If 'nogui' option (default=kFALSE) is kTRUE we do not launch the graphical interface.
   
    gTreeAnalyzer = this;

    KVBase::InitEnvironment();
   fMain_histolist=0;
   fMain_leaflist=0;
   fMain_selectionlist=0;
   fMenuFile=0;
   fTreeName = t->GetName();
   SetTreeFileName(t);
   SetAnalysisModifiedSinceLastSave(kFALSE);

   fDrawSame = fApplySelection = fProfileHisto  = kFALSE;
   fDrawLog = gEnv->GetValue("KVTreeAnalyzer.LogScale", kFALSE);
   fUserBinning = gEnv->GetValue("KVTreeAnalyzer.UserBinning", kFALSE);
   fUserWeight =  gEnv->GetValue("KVTreeAnalyzer.UserWeight", kFALSE);
   fNewCanvas = gEnv->GetValue("KVTreeAnalyzer.NewCanvas", kFALSE);
   fNormHisto = gEnv->GetValue("KVTreeAnalyzer.Normalize", kFALSE);
   fStatsHisto = gEnv->GetValue("KVTreeAnalyzer.Stats", kFALSE);
   fAutoSaveHisto = kFALSE;
   fNoGui=nogui;
   OpenGUI();
   fSameColorIndex=0;
   fSelectedSelections=0;
   fSelectedLeaves=0;
   fSelectedHistos=0;
   ipscale=0;
      GDfirst=new KVGumbelDistribution("Gum1",1);
      GDsecond=new KVGumbelDistribution("Gum2",2);
      GDthird=new KVGumbelDistribution("Gum3",3);
      GausGum1=new KVGausGumDistribution("GausGum1",1);
      GausGum2=new KVGausGumDistribution("GausGum2",2);
      GausGum3=new KVGausGumDistribution("GausGum3",3);
      
   fNx = fNy = 500;
   fXmin = fXmax = fYmin = fYmax = -1.;
   fWeight = "1./(abs(vper))";
   
   fNxF = 200;
   fXminF = fXmaxF = -1.;
   
   fNxD = fNyD = 120;
   fOrderedDalitz = 0;
}

KVTreeAnalyzer::~KVTreeAnalyzer()
{
   // Destructor
   SafeDelete(fSelectedSelections);
   SafeDelete(fSelectedHistos);
   SafeDelete(ipscale);
   SafeDelete(fMain_histolist);
   SafeDelete(fMain_leaflist);
   SafeDelete(fMain_selectionlist);
   if(gTreeAnalyzer==this) gTreeAnalyzer=0x0;
}

//________________________________________________________________

void KVTreeAnalyzer::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVTreeAnalyzer::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   TNamed::Copy(obj);
   KVTreeAnalyzer& CastedObj = (KVTreeAnalyzer&)obj;
   fSelections.Copy(CastedObj.fSelections);// list of TEntryList user selections
   fHistolist.Copy(CastedObj.fHistolist);//list of generated histograms
   CastedObj.fTreeName = fTreeName;//name of analyzed TTree
   CastedObj.fTreeFileName = fTreeFileName;//name of file containing analyzed TTree
   CastedObj.fHistoNumber=fHistoNumber;//used for automatic naming of histograms
   CastedObj.fSelectionNumber=fSelectionNumber;//used for automatic naming of selections
   CastedObj.fAliasNumber=fAliasNumber;//used for automatic naming of TTree aliases
   fAliasList.Copy(CastedObj.fAliasList);//list of TTree aliases
   CastedObj.SetAnalysisModifiedSinceLastSave(fAnalysisModifiedSinceLastSave);
}

void KVTreeAnalyzer::GenerateHistoTitle(TString& title, const Char_t* expr, const Char_t* selection)
{
   // PRIVATE utility method
   // Encodes the histogram title for the desired expression and an optional selection.
   // The expression and selection should be valid TTreeFormula strings
   // (i.e. they use TTree leaves and/or alias names)
   // If there is already an active selection (TEntryList set on TTree)
   // then the corresponding selection expression will also be included in the title.
   //
   // The format of the resulting title string is one of the following:
   //
   //    "expr1[:expr2]"
   //    "expr1[:expr2] {selection}"
   //    "expr1[:expr2] {active selection}"
   //    "expr1[:expr2] {(active selection) && (selection)}"

   
   TString _selection(selection);
   TString _elist;
   if(fTree->GetEntryList()) _elist = fTree->GetEntryList()->GetTitle();
   if(_selection!="" && _elist!="")
      title.Form("%s {(%s) && (%s)}", expr, _elist.Data(), selection);
   else if(_selection!="")
      title.Form("%s {%s}", expr, selection);
   else if(_elist!="")
      title.Form("%s {%s}", expr, _elist.Data());
   else
      title.Form("%s", expr);
}

TH1* KVTreeAnalyzer::MakeHisto(const Char_t* expr, const Char_t* selection, Int_t nX, Int_t nY)
{
   // Create and fill a new histogram with the desired expression (expr="expr1[:expr2]" etc.)
   // with the given selection (selection="" if no selection required).
   // Any currently active selection (TEntryList set on TTree) will also be applied.
   // The new histogram is not drawn but added to the internal list of histograms
   // (see method AddHisto).
   //
   // Histograms are automatically named 'h1', 'h2', etc. in order of creation.
   // Histogram title is generated with method GenerateHistoTitle.
   // Number of bins on X (and Y for a 2-D spectrum) are given. Axis limits are
   // automatically adjusted to data.
   //
   // For 2-D spectra the drawing option is set to "COL"
   //
   // If normalisation of spectra is required (fNormHisto = kTRUE) the histogram
   // bin contents are divided by the integral (sum of weights).
   
   TString name;
   name.Form("h%d",fHistoNumber);
   TString drawexp(expr), histo, histotitle;
   GenerateHistoTitle(histotitle, expr, selection);
   if((!nY)&&(fUserBinning))
     {
       ResetMethodCalled();
       Bool_t ok = KVBase::OpenContextMenu("DefineUserBinning1F", this, "DefineUserBinning");
       if(!ok) return 0;
       // cancel was pressed ?
       if(MethodNotCalled()) return 0;
     }
   if(fUserWeight)
     {
       ResetMethodCalled();
       Bool_t ok = KVBase::OpenContextMenu("DefineWeight", this);
       if(!ok) return 0;
       // cancel was pressed ?
       if(MethodNotCalled()) return 0;
     }

   if(nY) histo.Form(">>%s(%d,0.,0.,%d,0.,0.)", name.Data(), nX, nY);
   else histo.Form(">>%s(%d,%lf,%lf)", name.Data(), (fUserBinning ? fNxF : nX), (fUserBinning ? fXminF : 0.), (fUserBinning ?  fXmaxF: 0));
   
   if(!fProfileHisto) drawexp += histo;
   if(fProfileHisto) fTree->Draw(Form("%s>>%s",drawexp.Data(), name.Data()), selection, "prof,goff");
   else  fTree->Draw(drawexp, selection, "goff");
   TH1* h = (TH1*)gDirectory->Get(name);
   h->SetTitle(histotitle);
   if(h->InheritsFrom("TH2")) h->SetOption("col");
   h->SetDirectory(0);
   AddHisto(h);
   fHistoNumber++;
   if(fNormHisto && !fProfileHisto){
      h->Sumw2();
      h->Scale(1./h->Integral());
   }
   return h;
}

TH1* KVTreeAnalyzer::MakeIntHisto(const Char_t* expr, const Char_t* selection, Int_t Xmin, Int_t Xmax)
{
   // Like MakeHisto but only used for 1-D spectra of integer variables.
   // The number of bins is Xmax-Xmin+1 and bins are defined over [x-0.5,x+0.5]
   // for all values of x.
   //
   // Histograms are automatically named 'Ih1', 'Ih2', etc. in order of creation.
   
   TString name;
   name.Form("Ih%d",fHistoNumber);
   TString drawexp(expr), histo, histotitle;
   GenerateHistoTitle(histotitle, expr, selection);

   if(fUserBinning)
     {
       ResetMethodCalled();
       Bool_t ok = KVBase::OpenContextMenu("DefineUserBinning1F", this, "DefineUserBinning");
       if(!ok) return 0;
       // cancel was pressed ?
       if(MethodNotCalled()) return 0;
     }

   histo.Form(">>%s(%d,%f,%f)", name.Data(), (fUserBinning ? fNxF :(Xmax-Xmin)+1),
              (fUserBinning ? fXminF : Xmin-0.5), (fUserBinning ?  fXmaxF: Xmax+0.5));
   drawexp += histo;
   fTree->Draw(drawexp, selection, "goff");
   TH1* h = (TH1*)gDirectory->Get(name);
   h->SetTitle(histotitle);
   if(h->InheritsFrom("TH2")) h->SetOption("col");
   h->SetDirectory(0);
   AddHisto(h);
   fHistoNumber++;
   if(fNormHisto){
      h->Sumw2();
      h->Scale(1./h->Integral());
   }
   return h;
}

TH1 *KVTreeAnalyzer::GetHistogram(const Char_t *name) const
{
    // Return histogram with given name

    KVHistogram* h = (KVHistogram*)fHistolist.FindObject(name);
    if(h->IsType("Histo")) return h->GetHisto();
    return NULL;
}
   
Bool_t KVTreeAnalyzer::MakeSelection(const Char_t* selection)
{
   // Generate a new user-selection (TEntryList) of events in the TTree
   // according to the given selection expression (valid TTreeFormula expression
   // using TTree leaf and/or alias names).
   // The new selection is not applied immediately but added to the internal
   // list of selections (see method AddSelection).
   //
   // If there is already an active selection (TEntryList set on TTree)
   // this will generate the composite selection, {(active selection) && (selection)}.
   // The selection's title will then be in the following format:
   //
   //    "[(active selection) && ](selection)"
   //
   // TEntryList objects are automatically named 'el1', 'el2', etc. in order of creation.
   
   TObject* tmpObj = gROOT->FindObject(selection);
   if(tmpObj) 
     {
     if(tmpObj->InheritsFrom("TCutG"))
       {
       TCutG* cut = (TCutG*) tmpObj;
       cut->SetTitle(cut->GetName());
       AddCut(cut);
       }
     }

   TString name;
   name.Form("el%d",fSelectionNumber);
   TString drawexp(name.Data());
   drawexp.Prepend(">>");
   if( fTree->Draw(drawexp, selection, "entrylist") < 0 ){
	   new TGMsgBox(gClient->GetRoot(),0,"Warning","Mistake in your new selection!",kMBIconExclamation,kMBClose);
	   return kFALSE;
   }
   TEntryList*el = (TEntryList*)gDirectory->Get(name);
   if(fTree->GetEntryList()){
      TString _elist = fTree->GetEntryList()->GetTitle();
      TString title;
      title.Form("(%s) && (%s)", _elist.Data(), selection);
      el->SetTitle(title);
   }
   fSelectionNumber++;
   AddSelection(el);
   SelectionChanged();
   return kTRUE;
}
   
void KVTreeAnalyzer::SetSelection(TObject* obj)
{
   // Method called when a selection is double-clicked in the GUI list.
   // The required selection is passed as argument (address of TEntryList object)
   // and becomes the currently active selection (TEntryList set on TTree).
   // If the requested selection was already active, it is deactivated
   // (remove TEntryList from TTree).
   // The 'CURRENT SELECTION' message in the GUI status bar is updated.
   
   if(!obj->InheritsFrom("TEntryList")) return;
   TEntryList* el = dynamic_cast<TEntryList*>(obj);
   if(fTree->GetEntryList()==el){
	   fTree->SetEntryList(0);
      G_selection_status->SetText("CURRENT SELECTION:",0);
	   return;
   }
   fTree->SetEntryList(el);
   G_selection_status->SetText(Form("CURRENT SELECTION: %s (%lld)",el->GetTitle(),el->GetN()),0);
}
   
void KVTreeAnalyzer::CurrentSelection()
{
   // Print the currently active selection (TEntryList set on TTree).
   
   TString tmp;
   if(fTree->GetEntryList()) tmp = fTree->GetEntryList()->GetTitle();
   else tmp="";
   if(tmp!="") cout << "CURRENT SELECTION : " << tmp << endl;
}

void KVTreeAnalyzer::FillLeafList()
{
   // Fills the GUI list with the names of all leaves in the TTree and
   // all aliases defined by the user
   
   TList stuff;
   if(fTree) {
      stuff.AddAll(fTree->GetListOfLeaves());
      stuff.AddAll(fTree->GetListOfAliases());
   }
   stuff.AddAll(&fAliasList);
   G_leaflist->Display(&stuff);
}

void KVTreeAnalyzer::AnalysisSaveCheck()
{
    // if analysis has been modified since last save,
    // open an invite to ask if user wants to save with current default filename

    if(!fAnalysisModifiedSinceLastSave) return;

    if(fNoGui){
        // text-only interface
        cout << "Analysis has been modified. Save before continuing? [y] : " << flush;
        char reply;
        cin.get(reply);
        cout << endl;
        if(reply=='n' || reply=='N') return;
        cout << "Give name of file [" << fSaveAnalysisFileName << "] : " << flush;
        char filename[256];
        cin.get(filename,256);
        if(filename[0]!=0) fSaveAnalysisFileName=filename;
        Save();
    }
    else{
        Int_t ret_code;
        new TGMsgBox(gClient->GetDefaultRoot(), fMain_histolist, "Save Analysis ?",
                     "Analysis has been modified. Save before continuing?", kMBIconStop,
                     kMBYes|kMBNo, &ret_code);
        if(ret_code==kMBNo) return;
        HistoFileMenu_Save();
    }
}

void KVTreeAnalyzer::SetAnalysisModifiedSinceLastSave(Bool_t x)
{
    fAnalysisModifiedSinceLastSave=x;
    if(fMenuFile){
        if(x){
            fMenuFile->EnableEntry(MH_SAVE);
            fMenuFile->EnableEntry(MH_SAVE_FILE);
        }
        else
        {
            fMenuFile->DisableEntry(MH_SAVE);
            fMenuFile->DisableEntry(MH_SAVE_FILE);
        }
    }
}

void KVTreeAnalyzer::OpenGUI()
{
   // Launch the GUI (unless fNoGui=kTRUE in which case this does nothing)
   
   if(fNoGui) return;
   
   ULong_t red,cyan,green,yellow,magenta,gura,gurb,gurc,gurd,gure,gurf;
   gClient->GetColorByName("#ff00ff",magenta);
   gClient->GetColorByName("#ff0000",red);
   gClient->GetColorByName("#00ff00",green);
   gClient->GetColorByName("#00ffff",cyan);
   gClient->GetColorByName("#ffff00",yellow);
   gClient->GetColorByName("#cf14b2",gura);
   gClient->GetColorByName("#cd93e6",gurb);
   gClient->GetColorByName("#c1e91a",gurc);
   gClient->GetColorByName("#d1a45b",gurd);
   gClient->GetColorByName("#b54cfe",gure);
   gClient->GetColorByName("#a325ef",gurf);

   /********* MAIN WINDOW **************/
   //
   fMain_histolist = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMain_histolist->SetName("fMain_histolist");
   if(!fTree)
       fMain_histolist->SetWindowName("Tree Analyzer");
   else
       fMain_histolist->SetWindowName(Form("%s (%s)", fTree->GetTitle(), fTreeFileName.Data()));
   fMain_histolist->SetIconName("TreeAnalyzer");
   fMain_histolist->SetIconPixmap("root_s.xpm");
   UInt_t hWidth = 400, hHeight = 400;

             /* menus */
   fMenuFile = new TGPopupMenu(gClient->GetRoot());
   fMenuFile->AddEntry("Open...", MH_OPEN_FILE);
   fMenuFile->AddEntry("Save analysis", MH_SAVE);
   fMenuFile->AddEntry("Save as...", MH_SAVE_FILE);
   fMenuFile->AddEntry("Apply analysis...", MH_APPLY_ANALYSIS);
   SetAnalysisModifiedSinceLastSave(fAnalysisModifiedSinceLastSave);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("Quit", MH_QUIT);
   fMenuFile->Connect("Activated(Int_t)", "KVTreeAnalyzer", this, "HandleHistoFileMenu(Int_t)");
   fMenuSelections = new TGPopupMenu(gClient->GetRoot());
   fSelCombMenu = new TGPopupMenu(gClient->GetRoot());
   fSelCombMenu->AddEntry("AND (&&)",SEL_COMB_AND);
   fSelCombMenu->AddEntry("OR (||)",SEL_COMB_OR);
   fMenuSelections->AddPopup("Combine...", fSelCombMenu);
   fSelCombMenu->DisableEntry(SEL_COMB_AND);
   fSelCombMenu->DisableEntry(SEL_COMB_OR);
   fMenuSelections->AddEntry("Update", SEL_UPDATE);
   fMenuSelections->AddEntry("Delete", SEL_DELETE);
   fMenuSelections->DisableEntry(SEL_DELETE);
   fMenuSelections->Connect("Activated(Int_t)", "KVTreeAnalyzer", this, "HandleSelectionsMenu(Int_t)");
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBar = new TGMenuBar(fMain_histolist, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Selections", fMenuSelections, fMenuBarItemLayout);
   fMain_histolist->AddFrame(fMenuBar,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
   TGHorizontal3DLine *lh = new TGHorizontal3DLine(fMain_histolist);
   fMain_histolist->AddFrame(lh, new TGLayoutHints(kLHintsTop | kLHintsExpandX));

   // Horizontal frame to contain the VARIABLES list (left) and SELECTIONS list (right)
   TGHorizontalFrame* hf = new TGHorizontalFrame(fMain_histolist);

   /*********  VARIABLES **************/
   // Group frame for TTree variables/aliases
   fMain_leaflist = new TGGroupFrame(hf, "VARIABLES");
   UInt_t lWidth = 300, lHeight = 300;
   /* leaf list */

   /* make selection */
   TGHorizontalFrame* fHorizontalFrame = new TGHorizontalFrame(fMain_leaflist,lWidth,36,kHorizontalFrame);
   TGLabel* lab = new TGLabel(fHorizontalFrame, "Make alias : ");
   fHorizontalFrame->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));
   G_alias_text = new TGTextEntry(fHorizontalFrame, new TGTextBuffer(50));
   G_alias_text->SetMaxLength(4096);
   G_alias_text->SetAlignment(kTextLeft);
   G_alias_text->Resize(lWidth-100,G_alias_text->GetDefaultHeight());
   G_alias_text->Connect("ReturnPressed()", "KVTreeAnalyzer", this, "GenerateAlias()");
   fHorizontalFrame->AddFrame(G_alias_text, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,5,2,2));
   fMain_leaflist->AddFrame(fHorizontalFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));

   G_leaflist = new KVListView(TNamed::Class(), fMain_leaflist, lWidth, lHeight);
   G_leaflist->SetDataColumns(1);
   G_leaflist->SetDataColumn(0, "Title");
   G_leaflist->ActivateSortButtons();
   G_leaflist->AllowContextMenu(kFALSE);
   G_leaflist->SetDoubleClickAction("KVTreeAnalyzer", this, "DrawLeaf(TObject*)");
   G_leaflist->Connect("SelectionChanged()", "KVTreeAnalyzer", this, "LeafChanged()");
//   G_leaflist->Connect("ReturnPressed()", "KVTreeAnalyzer", this, "ShowVar()");
   fMain_leaflist->AddFrame(G_leaflist, new TGLayoutHints(kLHintsLeft|kLHintsTop|
                                       kLHintsExpandX|kLHintsExpandY,
                       5,5,5,5));

   //fMain_leaflist->MapSubwindows();

   fMain_leaflist->Resize(fMain_leaflist->GetDefaultSize());
   //fMain_leaflist->MapWindow();
   fMain_leaflist->Resize(lWidth,lHeight);
   FillLeafList();
   /*********end of VARIABLES **************/
   hf->AddFrame(fMain_leaflist, new TGLayoutHints(kLHintsLeft,5,5,5,5));

   /******* SELECTIONS *********/
   UInt_t sWidth = 600, sHeight = lHeight;
   fMain_selectionlist = new TGGroupFrame(hf,"SELECTIONS");
   /* current selection */
   G_selection_status = new TGStatusBar(fMain_selectionlist, sWidth, 10);
   G_selection_status->SetText("CURRENT SELECTION:",0);
   fMain_selectionlist->AddFrame(G_selection_status, new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0));
   /* make selection */
   TGHorizontalFrame *fHorizontalFrame1614 = new TGHorizontalFrame(fMain_selectionlist,sWidth,36,kHorizontalFrame);
   lab = new TGLabel(fHorizontalFrame1614, "Make selection : ");
   fHorizontalFrame1614->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));
   G_selection_text = new TGTextEntry(fHorizontalFrame1614, new TGTextBuffer(50));
   G_selection_text->SetMaxLength(4096);
   G_selection_text->SetAlignment(kTextLeft);
   G_selection_text->Resize(sWidth-100,G_selection_text->GetDefaultHeight());
   G_selection_text->Connect("ReturnPressed()", "KVTreeAnalyzer", this, "GenerateSelection()");
   fHorizontalFrame1614->AddFrame(G_selection_text, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,5,2,2));
   fMain_selectionlist->AddFrame(fHorizontalFrame1614, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));

   /* selection list */
   G_selectionlist = new KVListView(TEntryList::Class(), fMain_selectionlist, sWidth, sHeight);
   G_selectionlist->SetDataColumns(3);
   G_selectionlist->SetDataColumn(0, "Selection", "GetTitle");
   G_selectionlist->SetDataColumn(1, "Reapply", "GetReapplyCut");
   G_selectionlist->GetDataColumn(1)->SetIsBoolean();
   G_selectionlist->SetDataColumn(2, "Events", "GetN", kTextRight);
   G_selectionlist->ActivateSortButtons();
   G_selectionlist->SetDoubleClickAction("KVTreeAnalyzer", this, "SetSelection(TObject*)");
   G_selectionlist->Connect("SelectionChanged()", "KVTreeAnalyzer", this, "SelectionChanged()");
   fMain_selectionlist->AddFrame(G_selectionlist, new TGLayoutHints(kLHintsLeft|kLHintsTop|
                                       kLHintsExpandX|kLHintsExpandY,
                       5,5,5,5));

   //fMain_selectionlist->MapSubwindows();

   fMain_selectionlist->Resize(fMain_selectionlist->GetDefaultSize());
   //fMain_selectionlist->MapWindow();
   fMain_selectionlist->Resize(sWidth,sHeight);
   G_selectionlist->Display(&fSelections);
   /******end of SELECTIONS *********/
   hf->AddFrame(fMain_selectionlist, new TGLayoutHints(kLHintsLeft|kLHintsExpandX,5,5,5,5));
   fMain_histolist->AddFrame(hf, new TGLayoutHints(kLHintsTop|kLHintsCenterX|kLHintsExpandX));


   /**** Histo creation group ********/
   TGGroupFrame* histo_opts = new TGGroupFrame(fMain_histolist, "CREATE HISTO", kHorizontalFrame);
   fHorizontalFrame = new TGHorizontalFrame(histo_opts,lWidth,36,kHorizontalFrame);
   G_leaf_draw = new TGPictureButton(fHorizontalFrame, "draw_t.xpm");
   G_leaf_draw->SetEnabled(kFALSE);
   G_leaf_draw->Connect("Clicked()", "KVTreeAnalyzer", this, "DrawLeafExpr()");
   fHorizontalFrame->AddFrame(G_leaf_draw, new TGLayoutHints(kLHintsTop|kLHintsLeft,2,2,2,2));
   fLeafExpr="          ";
   G_leaf_expr = new TGLabel(fHorizontalFrame, fLeafExpr.Data());
   G_leaf_expr->Resize();
   fHorizontalFrame->AddFrame(G_leaf_expr, new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsCenterY,2,2,2,2));
   histo_opts->AddFrame(fHorizontalFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));

   G_histo_prof = new TGCheckButton(histo_opts, "Profile");
   G_histo_prof->SetToolTipText("Generate a profile histogram");
   G_histo_prof->SetState((EButtonState) fProfileHisto );
   G_histo_prof->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetProfileHisto(Bool_t)");
   histo_opts->AddFrame(G_histo_prof, new TGLayoutHints(kLHintsLeft,15,2,8,2));

   G_histo_norm = new TGCheckButton(histo_opts, "Normalize");
   G_histo_norm->SetToolTipText("Generate normalized histogram with integral=1");
   G_histo_norm->SetState((EButtonState) fNormHisto );
   G_histo_norm->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetNormHisto(Bool_t)");
   histo_opts->AddFrame(G_histo_norm, new TGLayoutHints(kLHintsLeft,15,2,8,2));

   G_histo_weight = new TGCheckButton(histo_opts, "Weight");
   G_histo_weight->SetToolTipText("User defined binning of the histogram");
   G_histo_weight->SetState((EButtonState) fUserWeight );
   G_histo_weight->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetUserWeight(Bool_t)");
   histo_opts->AddFrame(G_histo_weight, new TGLayoutHints(kLHintsLeft,15,2,8,2));

   G_histo_bin = new TGCheckButton(histo_opts, "Bins");
   G_histo_bin->SetToolTipText("User defined binning of the histogram");
   G_histo_bin->SetState((EButtonState) fUserBinning);
   G_histo_bin->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetUserBinning(Bool_t)");
   histo_opts->AddFrame(G_histo_bin, new TGLayoutHints(kLHintsLeft,15,2,8,2));
   fMain_histolist->AddFrame(histo_opts, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,5,5,5,5));

   /******** HISTOGRAMS *****************/
   hWidth = lWidth + sWidth + 20;
   TGGroupFrame* histo_group = new TGGroupFrame(fMain_histolist, "HISTOGRAMS");
   /* ip scale */
//   histo_opts = new TGGroupFrame(histo_group, "Impact parameter", kHorizontalFrame);
//   G_make_ip_scale = new TGTextButton(histo_opts,"Make scale");
//   G_make_ip_scale->SetTextJustify(36);
//   G_make_ip_scale->SetMargins(0,0,0,0);
//   G_make_ip_scale->SetWrapLength(-1);
//   G_make_ip_scale->Resize();
//   G_make_ip_scale->SetEnabled(kFALSE);
//   G_make_ip_scale->Connect("Clicked()","KVTreeAnalyzer",this,"MakeIPScale()");
//   G_make_ip_scale->ChangeBackground(green);
//   histo_opts->AddFrame(G_make_ip_scale, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,10,2));
//   lab = new TGLabel(histo_opts,"b <");
//   histo_opts->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsTop,5,2,12,2));
//   G_make_ip_selection = new TGTextEntry(histo_opts, new TGTextBuffer(5));
//   G_make_ip_selection->SetMaxLength(10);
//   G_make_ip_selection->SetAlignment(kTextLeft);
//   G_make_ip_selection->Resize(50,G_make_ip_selection->GetDefaultHeight());
//   G_make_ip_selection->Connect("ReturnPressed()", "KVTreeAnalyzer", this, "GenerateIPSelection()");
//   G_make_ip_selection->SetEnabled(kFALSE);
//   histo_opts->AddFrame(G_make_ip_selection, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,10,2));
//   G_ip_histo = new TGLabel(histo_opts,"-");
//   histo_opts->AddFrame(G_ip_histo, new TGLayoutHints(kLHintsLeft|kLHintsTop,5,2,12,2));
//   histo_group->AddFrame(histo_opts, new TGLayoutHints(kLHintsLeft|kLHintsExpandX,5,5,5,5));
//   /* ip scale */
//   histo_opts = new TGGroupFrame(histo_group, "Fits", kHorizontalFrame);
//   lab = new TGLabel(histo_opts,"Gumbel : ");
//   histo_opts->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,5,2));
//   G_fit1 = new TGTextButton(histo_opts, " 1 ");
//   G_fit1->SetTextJustify(36);
//   G_fit1->SetMargins(0,0,0,0);
//   G_fit1->SetWrapLength(-1);
//   G_fit1->Resize();
//   G_fit1->SetEnabled(kFALSE);
//   G_fit1->ChangeBackground(gura);
//   G_fit1->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGum1()");
//   histo_opts->AddFrame(G_fit1, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
//   G_fit2 = new TGTextButton(histo_opts, " 2 ");
//   G_fit2->SetTextJustify(36);
//   G_fit2->SetMargins(0,0,0,0);
//   G_fit2->SetWrapLength(-1);
//   G_fit2->Resize();
//   G_fit2->SetEnabled(kFALSE);
//   G_fit2->ChangeBackground(gurb);
//   G_fit2->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGum2()");
//   histo_opts->AddFrame(G_fit2, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
//   G_fit3 = new TGTextButton(histo_opts, " 3 ");
//   G_fit3->SetTextJustify(36);
//   G_fit3->SetMargins(0,0,0,0);
//   G_fit3->SetWrapLength(-1);
//   G_fit3->Resize();
//   G_fit3->SetEnabled(kFALSE);
//   G_fit3->ChangeBackground(gurc);
//   G_fit3->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGum3()");
//   histo_opts->AddFrame(G_fit3, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
//   lab = new TGLabel(histo_opts,"Gaus+Gum : ");
//   histo_opts->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsTop,10,2,5,2));
//   G_fitGG1 = new TGTextButton(histo_opts, " 1 ");
//   G_fitGG1->SetTextJustify(36);
//   G_fitGG1->SetMargins(0,0,0,0);
//   G_fitGG1->SetWrapLength(-1);
//   G_fitGG1->Resize();
//   G_fitGG1->SetEnabled(kFALSE);
//   G_fitGG1->ChangeBackground(gura);
//   G_fitGG1->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGausGum1()");
//   histo_opts->AddFrame(G_fitGG1, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
//   G_fitGG2 = new TGTextButton(histo_opts, " 2 ");
//   G_fitGG2->SetTextJustify(36);
//   G_fitGG2->SetMargins(0,0,0,0);
//   G_fitGG2->SetWrapLength(-1);
//   G_fitGG2->Resize();
//   G_fitGG2->SetEnabled(kFALSE);
//   G_fitGG2->ChangeBackground(gurb);
//   G_fitGG2->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGausGum2()");
//   histo_opts->AddFrame(G_fitGG2, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
//   G_fitGG3 = new TGTextButton(histo_opts, " 3 ");
//   G_fitGG3->SetTextJustify(36);
//   G_fitGG3->SetMargins(0,0,0,0);
//   G_fitGG3->SetWrapLength(-1);
//   G_fitGG3->Resize();
//   G_fitGG3->SetEnabled(kFALSE);
//   G_fitGG3->ChangeBackground(gurc);
//   G_fitGG3->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGausGum3()");
//   histo_opts->AddFrame(G_fitGG3, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
//   histo_group->AddFrame(histo_opts, new TGLayoutHints(kLHintsLeft|kLHintsExpandX,5,5,5,5));

   /* histo list */
   //G_histolist = new KVListView(TNamed::Class(), histo_group, hWidth, hHeight);
   //G_histolist->SetDataColumns(1);
   //G_histolist->SetDataColumn(0, "Data", "GetTitle", kTextLeft);
   G_histolist = new KVListView(KVHistogram::Class(), histo_group, hWidth, hHeight);
   G_histolist->SetDataColumns(6);
   G_histolist->SetDataColumn(0, "Name", "GetHistoTitle", kTextLeft);
   G_histolist->SetDataColumn(1, "VarX", "", kTextLeft);
   G_histolist->SetDataColumn(2, "VarY", "", kTextLeft);
   G_histolist->SetDataColumn(3, "VarZ", "", kTextLeft);
   G_histolist->SetDataColumn(4, "Selection", "", kTextLeft);
   G_histolist->SetDataColumn(5, "Type", "GetLabel", kTextLeft);
   G_histolist->ActivateSortButtons();
   G_histolist->SetMaxColumnSize(30);
   G_histolist->SetUseObjLabelAsRealClass();//to have icons & context menus of TH* & TCutG classes, not KVHistogram
   G_histolist->SetDoubleClickAction("KVTreeAnalyzer", this, "DrawHisto(TObject*)");
   G_histolist->Connect("SelectionChanged()","KVTreeAnalyzer",this,"HistoSelectionChanged()");
   histo_group->AddFrame(G_histolist, new TGLayoutHints(kLHintsLeft|kLHintsTop|
                                       kLHintsExpandX|kLHintsExpandY,
                       5,5,5,5));

   fMain_histolist->AddFrame(histo_group, new TGLayoutHints(kLHintsCenterX|kLHintsExpandX|kLHintsExpandY, 5,5,5,5));
   G_histolist->Display(&fHistolist);
   /* histo options */
   histo_opts = new TGGroupFrame(fMain_histolist, "OPTIONS", kHorizontalFrame);

   fHorizontalFrame = new TGHorizontalFrame(histo_opts,150,36,kHorizontalFrame);
   G_histo_del = new TGPictureButton(fHorizontalFrame, "sm_delete.xpm");
   G_histo_del->SetEnabled(kFALSE);
   G_histo_del->Connect("Clicked()", "KVTreeAnalyzer", this, "DeleteSelectedHisto()");
   fHorizontalFrame->AddFrame(G_histo_del, new TGLayoutHints(kLHintsTop|kLHintsLeft,2,2,2,2));
   lab = new TGLabel(fHorizontalFrame, "DELETE");
   lab->Resize();
   fHorizontalFrame->AddFrame(lab, new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsCenterY,2,2,2,2));
   histo_opts->AddFrame(fHorizontalFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,50,1,1));

   G_histo_new_can = new TGCheckButton(histo_opts, "New canvas");
   G_histo_new_can->SetToolTipText("Draw in a new canvas");
   G_histo_new_can->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetNewCanvas(Bool_t)");
   G_histo_new_can->SetState((EButtonState)fNewCanvas);
   histo_opts->AddFrame(G_histo_new_can, new TGLayoutHints(kLHintsLeft,15,2,8,2));
   G_histo_same = new TGCheckButton(histo_opts, "Same");
   G_histo_same->SetToolTipText("Draw in same pad");
   G_histo_same->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetDrawSame(Bool_t)");
   histo_opts->AddFrame(G_histo_same, new TGLayoutHints(kLHintsLeft,15,2,8,2));
   G_histo_app_sel = new TGCheckButton(histo_opts, "Apply selection");
   G_histo_app_sel->SetToolTipText("Apply current selection to generate new histo");
   G_histo_app_sel->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetApplySelection(Bool_t)");
   histo_opts->AddFrame(G_histo_app_sel, new TGLayoutHints(kLHintsLeft,15,2,8,2));
   G_histo_log = new TGCheckButton(histo_opts, "Log scale");
   G_histo_log->SetToolTipText("Use log scale in Y (1D) or Z (2D)");
   G_histo_log->SetState((EButtonState)fDrawLog);
   G_histo_log->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetDrawLog(Bool_t)");
   histo_opts->AddFrame(G_histo_log, new TGLayoutHints(kLHintsLeft,15,2,8,2));
   G_histo_stats = new TGCheckButton(histo_opts, "Stats");
   G_histo_stats->SetToolTipText("Display histogram statistics box");
   G_histo_stats->SetState((EButtonState) fStatsHisto );
   G_histo_stats->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetStatsHisto(Bool_t)");
   histo_opts->AddFrame(G_histo_stats, new TGLayoutHints(kLHintsLeft,15,2,8,2));
   G_histo_autosave = new TGCheckButton(histo_opts, "AutoSave");
   G_histo_autosave->SetToolTipText("Automatically generate histo image files");
   G_histo_autosave->SetState((EButtonState) fAutoSaveHisto );
   G_histo_autosave->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetAutoSaveHisto(Bool_t)");
   histo_opts->AddFrame(G_histo_autosave, new TGLayoutHints(kLHintsLeft,15,2,8,2));
   fMain_histolist->AddFrame(histo_opts, new TGLayoutHints(kLHintsCenterX,5,5,5,5));

   fMain_histolist->MapSubwindows();

   fMain_histolist->Resize(fMain_histolist->GetDefaultSize());
   fMain_histolist->MapWindow();

   hHeight = hHeight + lHeight + 50;

   fMain_histolist->Resize(hWidth,hHeight);
   /********* end of HISTOGRAMS *************/



}  
   
void KVTreeAnalyzer::AddHisto(TH1*h)
{
   // Adds histogram to internal list of user histograms
   // and updates GUI display
   
    SetAnalysisModifiedSinceLastSave(kTRUE);//new histogram needs saving
    fHistolist.Add( new KVHistogram(h) );
   G_histolist->Display(&fHistolist);
}
   
void KVTreeAnalyzer::AddSelection(TEntryList*e)
{
   // Adds selection to internal list of user histograms
   // and updates GUI display
   
    SetAnalysisModifiedSinceLastSave(kTRUE);//new selection needs saving
   fSelections.Add(e);
   G_selectionlist->Display(&fSelections);
}

void KVTreeAnalyzer::AddCut(TCutG* c)
{
   // Adds histogram to internal list of user histograms
   // and updates GUI display
   
    SetAnalysisModifiedSinceLastSave(kTRUE);//new selection needs saving
    fHistolist.Add( new KVHistogram(c) );
   G_histolist->Display(&fHistolist);
}

void KVTreeAnalyzer::SetTreeFileName(TTree* t)
{
   // For a single TTree, we store the name of the file in order to reload the TTree
   // if analysis is saved and subsequently reopened. This file name is used as the
   // basis for the default analysis backup filename.
   // If t is a TChain, we use the name of the first file in the TChain's list.
   // This means that if the analysis is saved and subsequently reopened, we will
   // only reconnect the TTree in the first file, not the entire dataset.

   if(t->InheritsFrom("TChain"))
      fTreeFileName = ((TChain*)t)->GetListOfFiles()->First()->GetTitle();
   else
      fTreeFileName = t->GetCurrentFile()->GetName();
   fSaveAnalysisFileName.Form("Analysis_%s", gSystem->BaseName(fTreeFileName.Data()));
}

void KVTreeAnalyzer::SetTree(TTree* t)
{
   // Connects a TTree for analysis
   
   fTree = t;
   fTreeName = t->GetName();
   SetTreeFileName(t);
   if(fMain_histolist) fMain_histolist->SetWindowName(Form("%s (%s)",t->GetTitle(),fTreeFileName.Data()));
}
   
void KVTreeAnalyzer::ReconnectTree()
{
   // Reconnects a TTree for analysis using the stored
   // informations on the file name and TTree name
   
   if(fTree) fTree->GetCurrentFile()->Close();
   TFile*f = TFile::Open(fTreeFileName);
   TTree *t = (TTree*)f->Get(fTreeName);
   SetTree(t);
}

KVTreeAnalyzer* KVTreeAnalyzer::OpenFile(const Char_t* filename, Bool_t nogui)
{
   // STATIC method to open a previously saved analysis session.
   //
   // Use:
   //    root[0] KVTreeAnalyzer* TA = KVTreeAnalyzer::OpenFile("my_analysis.root")
   //
   // If option nogui=kTRUE the GUI will not be launched
   
   TFile* f = TFile::Open(filename);
   KVTreeAnalyzer* anal = (KVTreeAnalyzer*)f->Get("KVTreeAnalyzer");
   delete f;
   anal->fNoGui=nogui;
   anal->ReconnectTree();
   anal->OpenGUI();
   return anal;
}

void KVTreeAnalyzer::ReadFromFile(const Char_t* filename)
{
   // open a previously saved analysis session.
   
   TFile* f = TFile::Open(filename);
   ReadFromFile(f);
}

void KVTreeAnalyzer::ReadFromFile(TFile* f)
{
   // open a previously saved analysis session.
   
   KVTreeAnalyzer* anal = (KVTreeAnalyzer*)f->Get("KVTreeAnalyzer");
   delete f;
   anal->Copy(*this);
   delete anal;
   ReconnectTree();
   G_selectionlist->Display(&fSelections);
   G_histolist->Display(&fHistolist);
   FillLeafList();
}

void KVTreeAnalyzer::DrawCut(TCutG* cut)
{
  TString name = Form("%s:%s", cut->GetVarY(), cut->GetVarX());
  KVList padList(kFALSE);
  Bool_t testHisto = kFALSE;
  
  if(!gPad) testHisto = kTRUE;

  if(!testHisto)
    {
    testHisto = kTRUE;
    padList.AddAll(((TPad*)gPad)->GetListOfPrimitives());
    TIter next(&padList);
    TObject* o = 0;
    while((o = next()))
      {
      if(o->InheritsFrom("TH1"))
        {
	TH1* hh = (TH1*) o;
        TString hName = hh->GetTitle();
	if(hName.Contains(name.Data())) 
	  {
	  cut->Draw("PL");
	  gPad->Update();
	  testHisto = kFALSE;
	  }
	}
      }
    }
    
  if(testHisto)
  {
      TIter next(&fHistolist);
      KVHistogram* hhh;
      TH1* hh = 0;
      while((hhh = (KVHistogram*)next()))
      {
          if((hh = hhh->GetHisto())){
              TString hName = hh->GetTitle();
              if(hName.Contains(name.Data()))
              {
                  DrawHisto(hh,kFALSE);
                  cut->Draw("PL");
                  gPad->Update();
                  break;
              }
          }
      }
  }
  //  else cut->Draw("PAL");
  
  
//   TIter next(fHistoList);
//   TH1* tmpHist = 0;
//   while((tmpHisto = (TH1*)next()))
//     {
//     
//     }

  return;
}

void KVTreeAnalyzer::DrawHisto(TObject* obj, Bool_t gen)
{
   // Method called when a user double-clicks a histogram in the GUI list.
   //
   // * if histogram is already displayed in active pad and if the pad also
   // contains a graphical contour (TCutG) object, we use the contour to define
   // a new data selection (TEntryList) which is added to the internal list.
   //
   // * if 'reapply selection' is activated and if the current active selection
   // is not the same as that used to generate the histogram, we generate a new
   // histogram displaying the same variables but with the current selection.
   //
   // * if 'draw same' is active we superimpose the (1-D) spectrum on the existing
   // plot with a different colour and add it to the automatically generated
   // legend which is also displayed in the plot
   // 
   // * if 'new canvas' is active the histogram is displayed in a new KVCanvas
   //
   // * in all cases when a histogram is displayed the log/linear scale of
   // Y (1-D) or Z (2-D) axis is automatically adjusted according to the 'log scale'
   // check box

    KVHistogram* kvhisto = 0;
    TCutG* cut = 0;
    TH1* histo = 0;
    if(obj->InheritsFrom("KVHistogram")) {
        kvhisto = dynamic_cast<KVHistogram*>(obj);
        if(kvhisto->IsType("Cut")) cut = kvhisto->GetCut();
        else if(kvhisto->IsType("Histo")) histo = kvhisto->GetHisto();
    }
    else if(obj->InheritsFrom("TCutG"))
    {
        cut = dynamic_cast<TCutG*>(obj);
    }
    else if(obj->InheritsFrom("TH1"))
    {
        histo = dynamic_cast<TH1*>(obj);
    }

    if(cut){
        DrawCut(cut);
        return;
    }
    if(!histo) return;
   
   // if histogram is already displayed in active pad and if the pad also
   // contains a graphical contour (TCutG) object, we use the contour to define
   // a new data selection (TEntryList) which is added to the internal list.
   if(gPad && gPad->GetListOfPrimitives()->FindObject(histo) && (gen)){
      TIter next(gPad->GetListOfPrimitives());
      TObject* o;
      while( (o = next()) ){
          if((o->IsA() == TCutG::Class()) && !(fHistolist.FindObjectWithNameAndType(o->GetName(),"Cut"))){
            MakeSelection(o->GetName());
            return;
         }
      }
   }
   
   KVString exp,sel;
   if(kvhisto) {
       exp= kvhisto->GetExpression();
       sel = kvhisto->GetSelection();
   }
   else
   {
       KVHistogram::ParseHistoTitle(histo->GetTitle(),exp,sel);
   }
   
   // if 'reapply selection' is activated and if the current active selection
   // is not the same as that used to generate the histogram, we generate a new
   // histogram displaying the same variables but with the current selection.
   if(!IsCurrentSelection(sel) && fApplySelection){
       histo = RemakeHisto(histo,exp);
       if(!histo) return;
   }
   
   if(fDrawSame)
   {
      // if 'draw same' is active we superimpose the (1-D) spectrum on the existing
      // plot with a different colour and add it to the automatically generated
      // legend which is also displayed in the plot
      histo->SetLineColor(my_color_array[++fSameColorIndex]);
      if(fSameColorIndex==MAX_COLOR_INDEX) fSameColorIndex=-1;
      histo->Draw("same");
      TObject* legend = gPad->GetListOfPrimitives()->FindObject("TPave");
      if(legend){
         gPad->GetListOfPrimitives()->Remove(legend);
         delete legend;
      }
      ((TPad*) gPad)->BuildLegend();
      if(histo->InheritsFrom("TH2")) gPad->SetLogz(fDrawLog);
      else {
         gPad->SetLogy(fDrawLog);
         // adjust y-scale to new histogram if needed
         // find first histogram
         TIter nxt(gPad->GetListOfPrimitives());
         TObject* h;
         while( (h=nxt()) ){
            if(h->InheritsFrom("TH1")){
               TH1* hh = (TH1*)h;
               if(histo->GetMaximum()>hh->GetMaximum()) hh->SetMaximum(histo->GetMaximum()+1);
               break;
            }
         }
      }
      gPad->Modified();
      gPad->Update();
   }
   else
   {
      // if 'new canvas' is active the histogram is displayed in a new KVCanvas
      // create a new canvas also if none exists
      if(fNewCanvas || !gPad) {KVCanvas*c=new KVCanvas; c->SetTitle(histo->GetTitle());c->SetWindowSize(700,700);}
      else if(gPad) { // update title of existing canvas
         gPad->GetCanvas()->SetTitle(histo->GetTitle());
      }
      histo->SetLineColor(my_color_array[0]);
      if(histo->InheritsFrom("TH2")) gPad->SetLogz(fDrawLog);
      else {
         histo->SetMaximum(-1111);//in case maximum was changed to accomodate superimposition
         gPad->SetLogy(fDrawLog);
      }
      histo->SetStats(fStatsHisto);//show/hide stat box according to check-box
      histo->Draw();
      gPad->Modified();gPad->Update();
      if(fAutoSaveHisto) AutoSaveHisto(histo);
   }
}
   
Bool_t KVTreeAnalyzer::IsCurrentSelection(const Char_t* sel)
{
   // Returns kTRUE if "sel" corresponds to current active selection
   // (i.e. entry list of TTree)
   
    if(!fTree) return kTRUE;
   TString test_sel(sel);
   TString tree_sel;TEntryList* el;
   if((el = fTree->GetEntryList())) tree_sel = el->GetTitle();
   return (test_sel == tree_sel);
}
   
TH1* KVTreeAnalyzer::RemakeHisto(TH1* h, const Char_t* expr)
{
   // Remake an existing histogram of data 'expr' using the current active selection
   // If such a histogram already exists, we just return its address.
   // We try to have the same binning in the new as in the original histogram.
   
   TString htit;
   GenerateHistoTitle(htit,expr,"");
   KVHistogram* kvhisto = (KVHistogram*)fHistolist.FindObjectWithMethod(htit,"GetHistoTitle");
   TH1* histo = 0;
   if(kvhisto) histo = kvhisto->GetHisto();
   if(histo && (histo->IsA()==h->IsA())) return histo;
   Int_t nx,ny=0;
   TString hname(h->GetName());
   if(hname.BeginsWith("I")){
      Int_t xmin = h->GetXaxis()->GetXmin()+0.5;
      Int_t xmax = h->GetXaxis()->GetXmax()-0.5;
      cout << "Remake histo with xmin = " << xmin << "  xmax = " << xmax << endl;
      h = MakeIntHisto(expr, "", xmin, xmax);
      return h;
   }
   nx = h->GetNbinsX();
   if(h->InheritsFrom("TH2")) ny = h->GetNbinsY();
   cout << "Remake histo with nx = " << nx << "  ny = " << ny << endl;
   if(h->InheritsFrom("TProfile")){
       // make a new profile histogram
       Bool_t oldProfileState = fProfileHisto;
       fProfileHisto = kTRUE;
       h = MakeHisto(expr, "", nx, ny);
       fProfileHisto = oldProfileState;
   }
   else
     h = MakeHisto(expr, "", nx, ny);
   return h;
}

void KVTreeAnalyzer::GenerateSelection()
{
   // Method called when user hits 'return' in selection GUI text-box
   // Takes expression from text-box and generates the corresponding
   // selection which is added to the GUI list of selections.
   
   TString selection = G_selection_text->GetText();
   if( selection.IsNull() ) return;
   if( MakeSelection(selection) ) G_selection_text->Clear();
}

void KVTreeAnalyzer::GenerateAlias()
{
   // Method called when user hits 'return' in TTree leaf/alias GUI text-box
   // Generates a new alias using the expression in the text-box.
   
   TString alias = G_alias_text->GetText();
   TString name;
   name.Form("a%d", fAliasNumber++);
   SetAlias(name, alias);
   FillLeafList();
   G_alias_text->Clear();
}

void KVTreeAnalyzer::CombineSelectionsAnd()
{
   // Method called when user hits 'combine selections' button in selections GUI.
   // Generates new selection which is the intersection (logical AND) of
   // the currently selected selections.
   
   if(fSelectedSelections){
      TEntryList* save_elist = fTree->GetEntryList();
      fTree->SetEntryList((TEntryList*)fSelectedSelections->First());
      TString newselect;
      int nsel = fSelectedSelections->GetEntries();
      for(int i=1; i<nsel; i++)
      {
         TString tmp; TEntryList* el = (TEntryList*)fSelectedSelections->At(i);
         tmp.Form("(%s)", el->GetTitle());
         if(i>1) newselect += " && ";
         newselect += tmp.Data();
      }
      MakeSelection(newselect);
      fTree->SetEntryList(save_elist);
   }
}

void KVTreeAnalyzer::CombineSelectionsOr()
{
   // Method called when user hits 'combine selections' button in selections GUI.
   // Generates new selection which is the intersection (logical AND) of
   // the currently selected selections.
   
   if(fSelectedSelections){
      TEntryList* save_elist = fTree->GetEntryList();
//      fTree->SetEntryList((TEntryList*)fSelectedSelections->First());
      fTree->SetEntryList((TEntryList*)0);
      TString newselect;
      int nsel = fSelectedSelections->GetEntries();
      for(int i=0; i<nsel; i++)
      {
         TString tmp; 
	 TEntryList* el = (TEntryList*)fSelectedSelections->At(i);
         tmp.Form("(%s)", el->GetTitle());
         if(i>0) newselect += " || ";
         newselect += tmp.Data();
      }
      MakeSelection(newselect);
      fTree->SetEntryList(save_elist);
   }
}

void KVTreeAnalyzer::DeleteSelections()
{
   // Delete the currently selected selection(s)
   
    if(fSelectedSelections){
        int nsel = fSelectedSelections->GetEntries();
        if(nsel<1) return;
        for(int i=0; i<nsel; i++){
            TEntryList* el = (TEntryList*)fSelectedSelections->At(i);
            if(!el) continue;
            fSelections.Remove(el);
            delete el;
            SetAnalysisModifiedSinceLastSave(kTRUE);
        }
        G_selectionlist->Display(&fSelections);
    }
    fMenuSelections->DisableEntry(SEL_DELETE);

}

void KVTreeAnalyzer::SelectionChanged()
{
   // Method called whenever the selected selection in the GUI list changes

    SafeDelete(fSelectedSelections);
    fSelectedSelections = G_selectionlist->GetSelectedObjects();
    Bool_t resetSel = kTRUE;
    fSelCombMenu->DisableEntry(SEL_COMB_AND);
    fSelCombMenu->DisableEntry(SEL_COMB_OR);
    fMenuSelections->DisableEntry(SEL_DELETE);

    if(!fSelectedSelections || fSelectedSelections->GetEntries()==0)
    {
    }
    else if(fSelectedSelections->GetEntries()==1)
    {
        fMenuSelections->EnableEntry(SEL_DELETE);
        KVSeqCollection* tmp = fSelections.GetSubListWithMethod(G_selection_text->GetText(),"GetTitle");
        if(tmp->GetSize()!=0||!strcmp("",G_selection_text->GetText()))
        {
            G_selection_text->SetText(((TNamed*)fSelectedSelections->At(0))->GetTitle());
            resetSel = kFALSE;
        }
        delete tmp;
    }
    else if(fSelectedSelections->GetEntries()>1)
    {
        fMenuSelections->EnableEntry(SEL_DELETE);
        fSelCombMenu->EnableEntry(SEL_COMB_AND);
        fSelCombMenu->EnableEntry(SEL_COMB_OR);
    }

    if(resetSel)
    {
        KVSeqCollection* tmp = fSelections.GetSubListWithMethod(G_selection_text->GetText(),"GetTitle");
        if(tmp->GetSize()!=0) G_selection_text->SetText("");
        delete tmp;
    }
}

void KVTreeAnalyzer::LeafChanged()
{
   // Method called whenever the leaf/alias selection in the TTree GUI list changes.
   // Updates the names of the leaves/aliases displayed next to the 'draw' button.
   
   fSelectedLeaves = G_leaflist->GetPickOrderedSelectedObjects();
   fLeafExpr="-";
   fXLeaf=fYLeaf=0;
//   Bool_t resetSel = kTRUE;
   G_leaf_draw->SetEnabled(kFALSE);
   Int_t nleaf = fSelectedLeaves->GetEntries();
   if(nleaf){
      if(nleaf==1){
          fXLeaf = (TNamed*)fSelectedLeaves->First();
          fLeafExpr = (fXLeaf->InheritsFrom("TLeaf") ? fXLeaf->GetName():  fXLeaf->GetTitle());
          G_leaf_draw->SetEnabled(kTRUE);

          KVSeqCollection* tmp =  ((KVList*)((KVLVContainer*)G_leaflist->GetContainer())->GetUserItems())->GetSubListWithMethod(G_alias_text->GetText(),"GetName");
          KVSeqCollection* tmp1 = ((KVList*)((KVLVContainer*)G_leaflist->GetContainer())->GetUserItems())->GetSubListWithMethod(G_alias_text->GetText(),"GetTitle");
          if(tmp->GetSize()!=0||tmp1->GetSize()!=0||!strcmp("",G_alias_text->GetText()))
          {
              G_alias_text->SetText(fLeafExpr.Data()); //resetSel = kFALSE;
          }
          else if(strcmp(fLeafExpr.Data(),G_leaf_expr->GetTitle()))
          {
              TString tmps = G_alias_text->GetText();
              Int_t pos = G_alias_text->MaxMark();
              if(pos>=tmps.Sizeof()) pos = G_alias_text->MinMark();
              if(pos>=tmps.Sizeof()) pos = tmps.Sizeof()-1;
              tmps.Insert(pos,fLeafExpr.Data());
              G_alias_text->SetText(tmps.Data());
              G_alias_text->SetCursorPosition(pos+fLeafExpr.Sizeof());
          }
          delete tmp;
          delete tmp1;

      }
      else if(nleaf==2){
          fXLeaf = (TNamed*)fSelectedLeaves->At(1);
          fYLeaf = (TNamed*)fSelectedLeaves->First();
          TString X,Y;
          X = (fXLeaf->InheritsFrom("TLeaf") ? fXLeaf->GetName():  fXLeaf->GetTitle());
          Y = (fYLeaf->InheritsFrom("TLeaf") ? fYLeaf->GetName():  fYLeaf->GetTitle());
          fLeafExpr.Form("%s:%s", Y.Data(), X.Data());
          G_leaf_draw->SetEnabled(kTRUE);
          G_alias_text->SetText("");
      }
      else if(nleaf==3){
          fXLeaf = (TNamed*)fSelectedLeaves->At(2);
          fYLeaf = (TNamed*)fSelectedLeaves->At(1);
          fZLeaf = (TNamed*)fSelectedLeaves->At(0);
          TString X,Y,Z;
          X = (fXLeaf->InheritsFrom("TLeaf") ? fXLeaf->GetName():  fXLeaf->GetTitle());
          Y = (fYLeaf->InheritsFrom("TLeaf") ? fYLeaf->GetName():  fYLeaf->GetTitle());
          Z = (fZLeaf->InheritsFrom("TLeaf") ? fZLeaf->GetName():  fZLeaf->GetTitle());
          fLeafExpr.Form("%s:%s:%s", Z.Data(), Y.Data(), X.Data());
          G_leaf_draw->SetEnabled(kTRUE);
          G_alias_text->SetText("");
      }
      else{
          fLeafExpr="-";
          G_alias_text->SetText("");
      }
   }
   else{
       fLeafExpr="-";
//       G_alias_text->SetText("");
       KVSeqCollection* tmp =  ((KVList*)((KVLVContainer*)G_leaflist->GetContainer())->GetUserItems())->GetSubListWithMethod(G_alias_text->GetText(),"GetName");
       KVSeqCollection* tmp1 = ((KVList*)((KVLVContainer*)G_leaflist->GetContainer())->GetUserItems())->GetSubListWithMethod(G_alias_text->GetText(),"GetTitle");
       if(tmp->GetSize()!=0||tmp1->GetSize()!=0)
       {
           G_alias_text->SetText(""); //resetSel = kFALSE;
       }
       delete tmp;
       delete tmp1;
   }

   G_leaf_expr->SetText(fLeafExpr);
   G_leaf_expr->Resize();
}

void KVTreeAnalyzer::DefineUserBinning(Int_t Nx, Int_t Ny, Double_t Xmin, Double_t Xmax, Double_t Ymin, Double_t Ymax)
{
    fMethodCalled=kTRUE;
  fNx = Nx;
  fNy = Ny;
  fXmin = Xmin;
  fXmax = Xmax;
  fYmin = Ymin;
  fYmax = Ymax;
}
   
void KVTreeAnalyzer::DefineUserBinning1F(Int_t Nx, Double_t Xmin, Double_t Xmax)
{
    fMethodCalled=kTRUE;
  fNxF = Nx;
  fXminF = Xmin;
  fXmaxF = Xmax;
}
   
void KVTreeAnalyzer::DefineUserBinningD(Int_t Nx, Int_t Ny, Int_t ordered)
{
    fMethodCalled=kTRUE;
  fNxD = Nx;
  fNyD = Ny;
  fOrderedDalitz = ordered;
}
   
void KVTreeAnalyzer::DefineWeight(const Char_t *Weight)
{
    fMethodCalled=kTRUE;
  fWeight = Weight;
}
   
void KVTreeAnalyzer::DrawLeafExpr()
{
   // Method called when user hits 'draw' button in TTree GUI.
   // If only one leaf/alias is selected, this actually calls DrawLeaf.
   
   if(fLeafExpr=="-")return;
   Bool_t threeDexp = fSelectedLeaves->GetEntries()==3;
   if(threeDexp && !fProfileHisto){
      DrawAsDalitz();
      return;
   }
   if(fSelectedLeaves->GetEntries()==1){
      DrawLeaf(fSelectedLeaves->First());
      return;
   }
   
   if(fUserBinning)
     {
       ResetMethodCalled();
       Bool_t ok = KVBase::OpenContextMenu("DefineUserBinning", this);
       if(!ok) return;
       // cancel was pressed ?
       if(MethodNotCalled()) return;
     }
   if(fUserWeight)
     {
       ResetMethodCalled();
       Bool_t ok = KVBase::OpenContextMenu("DefineWeight", this);
       if(!ok) return;
       // cancel was pressed ?
       if(MethodNotCalled()) return;
     }
 
   int nx=500,ny=500;
   double xmin,xmax,ymin,ymax;
   xmin=xmax=ymin=ymax=0;
   TString Xexpr,Yexpr,Zexpr;
   Xexpr = (fXLeaf->InheritsFrom("TLeaf") ? fXLeaf->GetName():fXLeaf->GetTitle());
   Yexpr = (fYLeaf->InheritsFrom("TLeaf") ? fYLeaf->GetName():fYLeaf->GetTitle());
   if(threeDexp) Zexpr = (fZLeaf->InheritsFrom("TLeaf") ? fZLeaf->GetName():fZLeaf->GetTitle());
   
      xmin = fTree->GetMinimum(Xexpr);
      xmax = fTree->GetMaximum(Xexpr);
   if(fXLeaf->InheritsFrom("TLeaf") && 
         (!strcmp(((TLeaf*)fXLeaf)->GetTypeName(),"Int_t")||!strcmp(((TLeaf*)fXLeaf)->GetTypeName(),"Short_t")||!strcmp(((TLeaf*)fXLeaf)->GetTypeName(),"Char_t"))
         ){
      xmin -= 0.5;
      xmax += 0.5;
      nx = xmax-xmin;
   }
      ymin = fTree->GetMinimum(Yexpr);
      ymax = fTree->GetMaximum(Yexpr);
   if(fYLeaf->InheritsFrom("TLeaf") && 
         (!strcmp(((TLeaf*)fYLeaf)->GetTypeName(),"Int_t")||!strcmp(((TLeaf*)fYLeaf)->GetTypeName(),"Short_t")||!strcmp(((TLeaf*)fYLeaf)->GetTypeName(),"Char_t"))
         ){
      ymin -= 0.5;
      ymax += 0.5;
      ny = ymax-ymin;
   }
   if(fXLeaf->InheritsFrom("TLeaf") && !strcmp(((TLeaf*)fXLeaf)->GetTypeName(),"Char_t")){
      TString tmp = Xexpr;
      Xexpr.Form("int(%s)",tmp.Data());
   }
   if(fYLeaf->InheritsFrom("TLeaf") && !strcmp(((TLeaf*)fYLeaf)->GetTypeName(),"Char_t")){
      TString tmp = Yexpr;
      Yexpr.Form("int(%s)",tmp.Data());
   }
   
   if(fUserBinning)
     {
     nx = fNx;
     ny = fNy;
     xmin = fXmin;
     xmax = fXmax;
     ymin = fYmin;
     ymax = fYmax;
     }
   
   if(threeDexp) fLeafExpr.Form("%s:%s:%s", Zexpr.Data(), Yexpr.Data(), Xexpr.Data());
   else fLeafExpr.Form("%s:%s", Yexpr.Data(), Xexpr.Data());
   TString name;
   name.Form("h%d",fHistoNumber);
   TString drawexp(fLeafExpr), histo, histotitle;
   GenerateHistoTitle(histotitle, fLeafExpr, "");
   if(!fProfileHisto) histo.Form(">>%s(%d,%f,%f,%d,%f,%f)", name.Data(), nx,xmin,xmax,ny,ymin,ymax);
   else histo.Form(">>%s", name.Data());
   drawexp += histo;
   TString ww = "";
   if(fUserWeight) ww += fWeight;
   if(!fProfileHisto) fTree->Draw(drawexp, ww.Data(), "goff");
   else  fTree->Draw(drawexp, "", "prof,goff");
   TH1* h = (TH1*)gDirectory->Get(name);
   h->SetTitle(histotitle);
   if(h->InheritsFrom("TH2")) h->SetOption("col");
   h->SetDirectory(0);
   if(h->InheritsFrom("TH1")) h->GetXaxis()->SetTitle(fXLeaf->GetTitle());
   if(h->InheritsFrom("TH2")||h->InheritsFrom("TProfile")) h->GetYaxis()->SetTitle(fYLeaf->GetTitle());
   AddHisto(h);
   fHistoNumber++;
   DrawHisto(h);
}


void KVTreeAnalyzer::DrawAsDalitz()
{
   if((!fXLeaf->InheritsFrom("TLeaf"))||(!fYLeaf->InheritsFrom("TLeaf"))||(!fZLeaf->InheritsFrom("TLeaf"))) 
     {
     Warning("DrawAsDalitz", "Cannot be used with aliases !");
     return;
     }

   TString Xexpr,Yexpr, Zexpr;
   Xexpr = (fXLeaf->InheritsFrom("TLeaf") ? fXLeaf->GetName():fXLeaf->GetTitle());
   Yexpr = (fYLeaf->InheritsFrom("TLeaf") ? fYLeaf->GetName():fYLeaf->GetTitle());
   Zexpr = (fZLeaf->InheritsFrom("TLeaf") ? fZLeaf->GetName():fZLeaf->GetTitle());
   fLeafExpr.Form("%s:%s:%s", Xexpr.Data(), Yexpr.Data(), Zexpr.Data());
   
   TString xType(((TLeaf*)fXLeaf)->GetTypeName());
   TString yType(((TLeaf*)fYLeaf)->GetTypeName());
   TString zType(((TLeaf*)fZLeaf)->GetTypeName());
   
   if((!xType.Contains(yType.Data()))||(!yType.Contains(zType.Data())))
     {
     Warning("DrawAsDalitz", "Leaves %s must have the same type !", fLeafExpr.Data());
     return;
     }
   
   Double_t x, y, z;
   Double_t var1, var2, var3;
   Float_t  varf1, varf2, varf3;
   Int_t    vari1, vari2, vari3;
   Short_t  vars1, vars2, vars3;
   Char_t   varc1, varc2, varc3;
   x=y=z=0.;
   
   if(xType.Contains("Int_t"))
     {
     fTree->SetBranchAddress(Xexpr.Data(),&vari1);
     fTree->SetBranchAddress(Yexpr.Data(),&vari2);
     fTree->SetBranchAddress(Zexpr.Data(),&vari3);
     }
   else if(xType.Contains("Short_t"))
     {
     fTree->SetBranchAddress(Xexpr.Data(),&vars1);
     fTree->SetBranchAddress(Yexpr.Data(),&vars2);
     fTree->SetBranchAddress(Zexpr.Data(),&vars3);
     }
   else if(xType.Contains("Char_t"))
     {
     fTree->SetBranchAddress(Xexpr.Data(),&varc1);
     fTree->SetBranchAddress(Yexpr.Data(),&varc2);
     fTree->SetBranchAddress(Zexpr.Data(),&varc3);
     }
   else if(xType.Contains("Double_t"))
     {
     fTree->SetBranchAddress(Xexpr.Data(),&var1);
     fTree->SetBranchAddress(Yexpr.Data(),&var2);
     fTree->SetBranchAddress(Zexpr.Data(),&var3);
     }
   else if(xType.Contains("Float_t"))
     {
     fTree->SetBranchAddress(Xexpr.Data(),&varf1);
     fTree->SetBranchAddress(Yexpr.Data(),&varf2);
     fTree->SetBranchAddress(Zexpr.Data(),&varf3);
     }
   
   if(fUserBinning) {
       ResetMethodCalled();
       Bool_t ok = KVBase::OpenContextMenu("DefineUserBinningD", this, "DefineUserBinning");
       if(!ok) return;
       // cancel was pressed ?
       if(MethodNotCalled()) return;
   }

   TString histotitle;
   GenerateHistoTitle(histotitle, fLeafExpr, "");
   
   KVDalitzPlot* h = 0;
   if(fUserBinning) h = new KVDalitzPlot(Form("h%d",fHistoNumber),histotitle.Data(),fOrderedDalitz,fNxD,0.,1.2,fNyD,0.,1.2);
   else             h = new KVDalitzPlot(Form("h%d",fHistoNumber),histotitle.Data());
   fHistoNumber++;
   
   TEntryList* el = fTree->GetEntryList();
   if(el) el->GetEntry(0);
   Int_t nentries = fTree->GetEntries();
   
   if(el) nentries = el->GetN();
   for(int i=0; i<nentries; i++)
     {
     Int_t  j = i;
     if(el) j = el->Next();
     fTree->GetEntry(j);
     if(xType.Contains("Int_t"))
       {
       x = vari1;
       y = vari2;
       z = vari3;
       }
     else if(xType.Contains("Short_t"))
       {
       x = vars1;
       y = vars2;
       z = vars3;
       }
     else if(xType.Contains("Char_t"))
       {
       x = varc1;
       y = varc2;
       z = varc3;
       }
     else if(xType.Contains("Double_t"))
       {
       x = var1;
       y = var2;
       z = var3;
       }
     else if(xType.Contains("Float_t"))
       {
       x = varf1;
       y = varf2;
       z = varf3;
       }
     h->FillAsDalitz(x,y,z);
     }

   h->SetOption("col");
   h->SetDirectory(0);
   AddHisto((TH2F*)h);
   DrawHisto(h);
   fTree->ResetBranchAddresses();
  
}

void KVTreeAnalyzer::DrawLeaf(TObject* obj)
{
   // Method called when user double-clicks a leaf/alias in list
   
   TH1* histo = 0;
   if(obj->InheritsFrom("TLeaf")){
      TLeaf* leaf = dynamic_cast<TLeaf*>(obj);
      TString expr = leaf->GetName();
      TString type = leaf->GetTypeName();
      // check histo not already in list
       TString htit;
       GenerateHistoTitle(htit,expr,"");
       KVHistogram* kvhisto = (KVHistogram*)fHistolist.FindObjectWithMethod(htit,"GetHistoTitle");
       if(kvhisto) histo = kvhisto->GetHisto();
      if(!histo){
         if(type=="Int_t"||type=="Char_t"||type=="Short_t"){
            Int_t xmin = fTree->GetMinimum(expr);
            Int_t xmax = fTree->GetMaximum(expr);
            if(type=="Char_t"){
               TString tmp; tmp.Form("int(%s)",expr.Data());
               expr=tmp.Data();
            }
            histo = MakeIntHisto(expr, "", xmin, xmax);
            if(!histo) return;
            histo->GetXaxis()->SetTitle(leaf->GetName());
         }
         else
         {
            histo = MakeHisto(expr, "", 500);
            histo->GetXaxis()->SetTitle(leaf->GetName());
         }
         if(!histo) return;
      }
//       if(fNewCanvas)  {KVCanvas*c=new KVCanvas; c->SetTitle(histo->GetTitle()); c->SetWindowSize(600,600);}
//       histo->Draw();
//       if(histo->InheritsFrom("TH2")) gPad->SetLogz(fDrawLog);
//       else gPad->SetLogy(fDrawLog);
//       gPad->Modified();gPad->Update();
      DrawHisto(histo);
   }
   else{
      TString expr = obj->GetTitle();
      // check histo not already in list
       TString htit;
       GenerateHistoTitle(htit,expr,"");
       KVHistogram* kvhisto = (KVHistogram*)fHistolist.FindObjectWithMethod(htit,"GetHistoTitle");
       if(kvhisto) histo = kvhisto->GetHisto();
      if(!histo) histo = MakeHisto(expr, "", 500);
      if(!histo) return;
      histo->GetXaxis()->SetTitle(obj->GetTitle());
//       if(fNewCanvas)  {KVCanvas*c=new KVCanvas; c->SetTitle(histo->GetTitle());}
//       histo->Draw();
//       if(histo->InheritsFrom("TH2")) gPad->SetLogz(fDrawLog);
//       else gPad->SetLogy(fDrawLog);
//       gPad->Modified();gPad->Update();
      DrawHisto(histo);
   }
}

void KVTreeAnalyzer::HistoSelectionChanged()
{
   // Method called when user histo selection changes in GUI histogram list
   
    SafeDelete(fSelectedHistos);
    G_histo_del->SetEnabled(kFALSE);
//    G_make_ip_scale->SetEnabled(kFALSE);
//    G_fit1->SetEnabled(kFALSE);
//    G_fit2->SetEnabled(kFALSE);
//    G_fit3->SetEnabled(kFALSE);
//    G_fitGG1->SetEnabled(kFALSE);
//    G_fitGG2->SetEnabled(kFALSE);
//    G_fitGG3->SetEnabled(kFALSE);
    fSelectedHistos = G_histolist->GetSelectedObjects();
    if(fSelectedHistos){
        if(fSelectedHistos->GetEntries()>0)
        {
            G_histo_del->SetEnabled(kTRUE);
            if(fSelectedHistos->GetEntries()==1 && fSelectedHistos->First()->IsA()==TH1F::Class()){
//                G_make_ip_scale->SetEnabled(kTRUE);
//                G_fit1->SetEnabled(kTRUE);
//                G_fit2->SetEnabled(kTRUE);
//                G_fit3->SetEnabled(kTRUE);
//                G_fitGG1->SetEnabled(kTRUE);
//                G_fitGG2->SetEnabled(kTRUE);
//                G_fitGG3->SetEnabled(kTRUE);
            }
        }
    }
}

void KVTreeAnalyzer::GenerateIPSelection()
{
   // Method called when user hits 'return' in the 'b<...' text-box
   // of the histogram GUI.
   // Create a new selection of events based on the impact parameter
   // scale previously created using MakeIPScale.
   
   TString bmax = G_make_ip_selection->GetText();
   Double_t Bmax = bmax.Atof();
   TGString histotit = G_ip_histo->GetText();
   KVString ipvar,ipsel;
   KVHistogram::ParseHistoTitle(histotit.Data(),ipvar,ipsel);
   Double_t varCut = ipscale->GetObservable(Bmax);
   TString selection;
   selection.Form("%s>%f", ipvar.Data(), varCut);
   TEntryList* save_elist = fTree->GetEntryList();
   SetSelection(ipsel);
   MakeSelection(selection);
   fTree->SetEntryList(save_elist);
}

void KVTreeAnalyzer::MakeIPScale()
{
   SafeDelete(ipscale);
   TH1* histo = dynamic_cast<TH1*>(fSelectedHistos->First());
   if(!histo) return;
   ipscale = new KVImpactParameter(histo);
   ipscale->MakeScale();
   G_ip_histo->SetText(histo->GetTitle());
   G_ip_histo->Resize();
   G_make_ip_selection->SetEnabled(kTRUE);
}

void KVTreeAnalyzer::SetSelection(const Char_t* sel)
{
   TObject* obj = fSelections.FindObjectWithMethod(sel,"GetTitle");
   fTree->SetEntryList((TEntryList*)obj);
}

void KVTreeAnalyzer::Save()
{
   SaveAs(fSaveAnalysisFileName);
   SetAnalysisModifiedSinceLastSave(kFALSE);
}

void KVTreeAnalyzer::FitGum1()
{
   TH1* histo = dynamic_cast<TH1*>(fSelectedHistos->First());
   if(!histo) return;
   GDfirst->SetParameters(histo->GetMean(),histo->GetRMS());
   histo->Fit(GDfirst,"EM");
   TPaveStats* stats = (TPaveStats*)histo->FindObject("stats");
   if(!stats){
      histo->SetStats(1);
      histo->Draw();
      gPad->Update();
      stats = (TPaveStats*)histo->FindObject("stats");
   }
   if(stats){
      // if canvas's 'no stats' option has been set by user,
      // there will still be no valid stats object,
      // so this test is to avoid the ensuing seg fault
      stats->SetFitFormat("10.9g");
      stats->SetOptFit(111);
   }
   histo->SetOption("e1");
   histo->SetMarkerStyle(24);
   histo->SetMarkerColor(kBlue+2);
   gPad->Modified();gPad->Update();
   SetAnalysisModifiedSinceLastSave(kTRUE);
}
void KVTreeAnalyzer::FitGum2()
{
   TH1* histo = dynamic_cast<TH1*>(fSelectedHistos->First());
   if(!histo) return;
   GDsecond->SetParameters(histo->GetMean(),histo->GetRMS());
   histo->Fit(GDsecond,"EM");
   TPaveStats* stats = (TPaveStats*)histo->FindObject("stats");
   if(!stats){
      histo->SetStats(1);
      histo->Draw();
      gPad->Update();
      stats = (TPaveStats*)histo->FindObject("stats");
   }
   if(stats){
      // if canvas's 'no stats' option has been set by user,
      // there will still be no valid stats object,
      // so this test is to avoid the ensuing seg fault
      stats->SetFitFormat("10.9g");
      stats->SetOptFit(111);
   }
   histo->SetOption("e1");
   histo->SetMarkerStyle(24);
   histo->SetMarkerColor(kBlue+2);
   gPad->Modified();gPad->Update();
   SetAnalysisModifiedSinceLastSave(kTRUE);
}
void KVTreeAnalyzer::FitGum3()
{
   TH1* histo = dynamic_cast<TH1*>(fSelectedHistos->First());
   if(!histo) return;
   GDthird->SetParameters(histo->GetMean(),histo->GetRMS());
   histo->Fit(GDthird,"EM");
   TPaveStats* stats = (TPaveStats*)histo->FindObject("stats");
   if(!stats){
      histo->SetStats(1);
      histo->Draw();
      gPad->Update();
      stats = (TPaveStats*)histo->FindObject("stats");
   }
   if(stats){
      // if canvas's 'no stats' option has been set by user,
      // there will still be no valid stats object,
      // so this test is to avoid the ensuing seg fault
      stats->SetFitFormat("10.9g");
      stats->SetOptFit(111);
   }
   histo->SetOption("e1");
   histo->SetMarkerStyle(24);
   histo->SetMarkerColor(kBlue+2);
   gPad->Modified();gPad->Update();
   SetAnalysisModifiedSinceLastSave(kTRUE);
}
void KVTreeAnalyzer::FitGausGum1()
{
   TH1* histo = dynamic_cast<TH1*>(fSelectedHistos->First());
   if(!histo) return;
   GausGum1->SetParameters(0.5,histo->GetMean()+histo->GetRMS(),1,histo->GetRMS(),1);
   histo->Fit(GausGum1,"EM");
   TPaveStats* stats = (TPaveStats*)histo->FindObject("stats");
   if(!stats){
      histo->SetStats(1);
      histo->Draw();
      gPad->Update();
      stats = (TPaveStats*)histo->FindObject("stats");
   }
   if(stats){
      // if canvas's 'no stats' option has been set by user,
      // there will still be no valid stats object,
      // so this test is to avoid the ensuing seg fault
      stats->SetFitFormat("10.9g");
      stats->SetOptFit(111);
   }
   histo->SetOption("e1");
   histo->SetMarkerStyle(24);
   histo->SetMarkerColor(kBlue+2);
   gPad->Modified();gPad->Update();
   SetAnalysisModifiedSinceLastSave(kTRUE);
}
   
void KVTreeAnalyzer::FitGausGum2()
{
   TH1* histo = dynamic_cast<TH1*>(fSelectedHistos->First());
   if(!histo) return;
   GausGum2->SetParameters(0.5,histo->GetMean()+histo->GetRMS(),1,histo->GetRMS(),1);
   histo->Fit(GausGum2,"EM");
   TPaveStats* stats = (TPaveStats*)histo->FindObject("stats");
   if(!stats){
      histo->SetStats(1);
      histo->Draw();
      gPad->Update();
      stats = (TPaveStats*)histo->FindObject("stats");
   }
   if(stats){
      // if canvas's 'no stats' option has been set by user,
      // there will still be no valid stats object,
      // so this test is to avoid the ensuing seg fault
      stats->SetFitFormat("10.9g");
      stats->SetOptFit(111);
   }
   histo->SetOption("e1");
   histo->SetMarkerStyle(24);
   histo->SetMarkerColor(kBlue+2);
   gPad->Modified();gPad->Update();
   SetAnalysisModifiedSinceLastSave(kTRUE);
}
   
void KVTreeAnalyzer::FitGausGum3()
{
   TH1* histo = dynamic_cast<TH1*>(fSelectedHistos->First());
   if(!histo) return;
   GausGum3->SetParameters(0.5,histo->GetMean()+histo->GetRMS(),1,histo->GetRMS(),1);
   histo->Fit(GausGum3,"EM");
   TPaveStats* stats = (TPaveStats*)histo->FindObject("stats");
   if(!stats){
      histo->SetStats(1);
      histo->Draw();
      gPad->Update();
      stats = (TPaveStats*)histo->FindObject("stats");
   }
   if(stats){
      // if canvas's 'no stats' option has been set by user,
      // there will still be no valid stats object,
      // so this test is to avoid the ensuing seg fault
      stats->SetFitFormat("10.9g");
      stats->SetOptFit(111);
   }
   histo->SetOption("e1");
   histo->SetMarkerStyle(24);
   histo->SetMarkerColor(kBlue+2);
   gPad->Modified();gPad->Update();
   SetAnalysisModifiedSinceLastSave(kTRUE);
}
   
TList* KVTreeAnalyzer::GetHistosByData(const Char_t* expr)
{
   // fill and return TList with histos containing the given data
   // which may be 1D ("mult", "zmax", etc.) or 2D ("zmax:mult")
   // DELETE LIST AFTER USE
   TList* hlist = new TList;
   TIter next(&fHistolist);

   KVHistogram* h;
   while( (h=(KVHistogram*)next()) ){

      if(h->IsType("Histo") && !strcmp(h->GetExpression(),expr)) hlist->Add(h->GetHisto());
   }
   return hlist;
}
   
TList* KVTreeAnalyzer::GetHistosBySelection(const Char_t* expr)
{
   // fill and return TList with histos using the given selection criteria
   // DELETE LIST AFTER USE
   TList* hlist = new TList;
   TIter next(&fHistolist);

   KVHistogram* h;
   while( (h=(KVHistogram*)next()) ){

       if(h->IsType("Histo") && !strcmp(h->GetSelection(),expr)) hlist->Add(h->GetHisto());
   }
   return hlist;
}
   
TH1* KVTreeAnalyzer::GetHisto(const Char_t* expr, const Char_t* selection)
{
   TIter next(&fHistolist);

   KVHistogram* h;
   while( (h=(KVHistogram*)next()) ){

       if(h->IsType("Histo") && !strcmp(h->GetExpression(),expr) && !strcmp(h->GetSelection(),selection)) return h->GetHisto();
   }
   return 0;
}
   
void KVTreeAnalyzer::DeleteHisto(const Char_t* expr, const Char_t* selection)
{
    TIter next(&fHistolist);

    KVHistogram* h;
    while( (h=(KVHistogram*)next()) ){

        if(h->IsType("Histo") && !strcmp(h->GetExpression(),expr) && !strcmp(h->GetSelection(),selection)){
            cout << "Deleting histo " << h->GetName() << endl;
            fHistolist.Remove(h);
            delete h;
            SetAnalysisModifiedSinceLastSave(kTRUE);
        }
    }
    G_histolist->Display(&fHistolist);
}

void KVTreeAnalyzer::DeleteSelectedHisto()
{
    // Delete all currently selected histograms
    if(fSelectedHistos){
        Int_t nsel = fSelectedHistos->GetEntries();
        if(nsel<1) return;
        for(int i=0; i<nsel; i++){
            TObject* obj = fSelectedHistos->At(i);
            fHistolist.Remove(obj);
            delete obj;
            SetAnalysisModifiedSinceLastSave(kTRUE);
        }
        G_histolist->Display(&fHistolist);
    }
    HistoSelectionChanged();
}


void KVTreeAnalyzer::UpdateEntryLists()
{
   // regenerate entry lists for all selections
   TList old_lists;
   old_lists.AddAll(&fSelections);
   fSelections.Clear();
   G_selectionlist->RemoveAll();
   TIter next(&old_lists);
   TEntryList* old_el;
   fTree->SetEntryList(0);
   SelectionChanged();
   while( (old_el = (TEntryList*)next()) ){
      cout << "REGENERATING SELECTION : " << old_el->GetTitle() << endl;
      MakeSelection(old_el->GetTitle());
	  ((TEntryList *)fSelections.Last())->SetReapplyCut( old_el->GetReapplyCut() );
   	  G_selectionlist->Display(&fSelections);
   }
   old_lists.Delete();
}
   
void KVTreeAnalyzer::HandleHistoFileMenu(Int_t id)
{
   switch(id){
      case MH_OPEN_FILE:
         HistoFileMenu_Open();
         break;
   case MH_APPLY_ANALYSIS:
       HistoFileMenu_Apply();
       break;
   case MH_SAVE_FILE:
      HistoFileMenu_Save();
      break;
   case MH_SAVE:
      Save();
      break;
      case MH_QUIT:
       AnalysisSaveCheck();
         gROOT->ProcessLine(".q");
         break;
         
      default:
         break;
   }
}
void KVTreeAnalyzer::HandleSelectionsMenu(Int_t id)
{
   switch(id){
   case SEL_COMB_AND:
       CombineSelectionsAnd();
       break;

   case SEL_COMB_OR:
       CombineSelectionsOr();
       break;

   case SEL_DELETE:
       DeleteSelections();
       break;

   case SEL_UPDATE:
       UpdateEntryLists();
       break;

   default:
       break;
   }
}

void KVTreeAnalyzer::HistoFileMenu_Open()
{
   static TString dir(".");
   const char *filetypes[] = {
      "ROOT files", "*.root",
      0, 0
   };
   AnalysisSaveCheck();
   TGFileInfo fi;
   fi.fFileTypes = filetypes;
   fi.fIniDir = StrDup(dir);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain_histolist, kFDOpen, &fi);
   if (fi.fFilename) {
      OpenAnyFile(fi.fFilename);
   }
   dir = fi.fIniDir;
}
void KVTreeAnalyzer::HistoFileMenu_Apply()
{
   static TString dir(".");
   const char *filetypes[] = {
      "ROOT files", "*.root",
      0, 0
   };
   TGFileInfo fi;
   fi.fFileTypes = filetypes;
   fi.fIniDir = StrDup(dir);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain_histolist, kFDOpen, &fi);
   if (fi.fFilename) {
      ReapplyAnyFile(fi.fFilename);
   }
   dir = fi.fIniDir;
}

void KVTreeAnalyzer::HistoFileMenu_Save()
{
   static TString dir(".");
   const char *filetypes[] = {
      "ROOT files", "*.root",
      0, 0
   };
   TGFileInfo fi;
   fi.fFileTypes = filetypes;
   fi.fIniDir = StrDup(dir);
   fi.fFilename = StrDup(fSaveAnalysisFileName);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain_histolist, kFDSave, &fi);
   if (fi.fFilename) {
      //if no ".xxx" ending given, we add ".root"
      TString filenam(fi.fFilename);
      if (!filenam.Contains('.'))
         filenam += ".root";
      // update name of file to autosave analysis in
      fSaveAnalysisFileName = filenam;
      Save();
   }
   dir = fi.fIniDir;
}

void KVTreeAnalyzer::OpenAnyFile(const Char_t* filepath)
{
   // assuming filepath is the URL of a ROOT file, open it and
   // either
   //   i) open the KVTreeAnalyzer object stored in it
   // or ii) if no KVTreeAnalyzer object is found, open first TTree in file
    // Any histograms in the file are added to the list of histograms
   
    AnalysisSaveCheck();

   TFile* file = TFile::Open(filepath);
   TObject*kvta = file->GetListOfKeys()->FindObject("KVTreeAnalyzer");
   if(kvta){
      ReadFromFile(file);
   }
   else
   {
      fHistolist.Clear();
      KVList keys(0);
      keys.AddAll(file->GetListOfKeys());
      // Get list of trees in file
      KVSeqCollection* trees = keys.GetSubListWithMethod("TTree", "GetClassName");
      if(trees->GetEntries()){
          // Get name of first tree
          TString aTreeName=trees->First()->GetName();
          if(fTree) fTree->GetCurrentFile()->Close();
          fTree=0;
          // Use this tree
         TTree* t=(TTree*)file->Get(aTreeName);
         SetTitle(t->GetTitle());
         SetTree(t);
         fSelections.Clear();
         fAliasList.Clear();
         fHistoNumber=1;
         fSelectionNumber=1;
         fAliasNumber=1;
         fSameColorIndex=0;
         fSelectedSelections=0;
         fSelectedLeaves=0;
         fSelectedHistos=0;
         SetAnalysisModifiedSinceLastSave(kFALSE);
         G_selectionlist->Display(&fSelections);
         FillLeafList();
      }
      delete trees;
      TIter next(&keys);
      TKey* akey;
      while( (akey=(TKey*)next()) ){
          if(TClass::GetClass(akey->GetClassName())->InheritsFrom("TH1")){
              if(!fHistolist.FindObject(akey->GetName())){
                  TH1* h = (TH1*)file->Get(akey->GetName());
                  if(h->InheritsFrom("TH2")) h->SetOption("col");
                  h->SetDirectory(0);
                  fHistolist.Add(new KVHistogram(h));
              }
          }
      }
      G_histolist->Display(&fHistolist);
   }
}
void KVTreeAnalyzer::ReapplyAnyFile(const Char_t* filepath)
{
   // assuming filepath is the URL of a ROOT file, open it and,
   // if no KVTreeAnalyzer object is found, open first TTree in file
    // and apply all selections and generate all histograms which
    // were made for this analysis.
    // Any histograms in the file are added to the list of histograms

   TFile* file = TFile::Open(filepath);
   TObject*kvta = file->GetListOfKeys()->FindObject("KVTreeAnalyzer");
   if(kvta){
       Info("ReapplyAnyFile","For the moment, can only apply analysis to TTree");
       return;
   }
   else
   {
       delete file;
       KVTreeAnalyzer* applyAnal = new KVTreeAnalyzer(kFALSE);
       applyAnal->OpenAnyFile(filepath);
       applyAnal->GenerateAllSelections(&fSelections);
       applyAnal->GenerateAllHistograms(&fHistolist);
   }
}

void KVTreeAnalyzer::SetAlias(const Char_t *name, const Char_t *expr)
{
    TString exp = expr;
    exp.ReplaceAll("d2r","TMath::DegToRad()");
    exp.ReplaceAll("r2d","TMath::RadToDeg()");
    fAliasList.Add(new TNamed(name,exp.Data()));
    SetAnalysisModifiedSinceLastSave(kTRUE);
}

static const char *gSaveAsTypes[] = { "PostScript",   "*.ps",
                                      "Encapsulated PostScript", "*.eps",
                                      "PDF",          "*.pdf",
                                      "SVG",          "*.svg",
                                      "TeX",          "*.tex",
                                      "GIF",          "*.gif",
                                      "ROOT files",   "*.root",
                                      "XML",          "*.xml",
                                      "PNG",          "*.png",
                                      "XPM",          "*.xpm",
                                      "JPEG",         "*.jpg",
                                      "TIFF",         "*.tiff",
                                      "XCF",          "*.xcf",
                                      0,              0 };
                                      
void KVTreeAnalyzer::SetUpHistoAutoSave()
{
   // Open file dialog box for user to choose directory and
   // image file-type for generating picture files of all
   // histos as they are drawn
   
   Info("SetUpHistoAutoSave", "Select image filetype and directory");
   TString workdir = gSystem->WorkingDirectory();
   static TString dir(".");
   static Int_t typeidx = 0;
   static Bool_t overwr = kFALSE;
   TGFileInfo fi;
   fi.fFileTypes   = gSaveAsTypes;
   fi.fIniDir      = StrDup(dir);
   fi.fFileTypeIdx = typeidx;
   fi.fOverwrite = overwr;
   new KVFileDialog(gClient->GetDefaultRoot(), fMain_histolist, kKVFDDirectory, &fi);
   gSystem->ChangeDirectory(workdir.Data());
   TString ft = fi.fFileTypes[fi.fFileTypeIdx+1];
   dir     = fi.fIniDir;
   typeidx = fi.fFileTypeIdx;
   overwr  = fi.fOverwrite;
   fAutoSaveDir = dir.Data();
   fAutoSaveType = ft(1,ft.Length()).Data();
   Info("SetUpHistoAutoSave", "file-type:%s directory:%s", fAutoSaveType.Data(), fAutoSaveDir.Data());
}

void KVTreeAnalyzer::AutoSaveHisto(TH1* h)
{
   // Save currently displayed histo as an image file
   
   TString title = h->GetTitle();
   title.ReplaceAll(" ", "_");
   title.ReplaceAll("/", "#");
   title.Append(fAutoSaveType);
   title.Prepend("/");
   title.Prepend(fAutoSaveDir);
   Info("AutoSaveHisto", "Saved as: %s", title.Data());
   gPad->SaveAs(title);
}

void KVTreeAnalyzer::GenerateAllSelections(TCollection *list)
{
    // We take the title of every object in 'list'
    // and generate the corresponding selection

    TIter nextSel(list);
    TObject* sel;
    while( (sel = nextSel()) ) {
        Info("GenerateAllSelections","Generating selection: %s", sel->GetTitle());
        MakeSelection(sel->GetTitle());
    }
}

void KVTreeAnalyzer::GenerateAllHistograms(TCollection *list)
{
    // For every histogram in the list, we generate histograms for
    // the same expression and the same selection
    TIter nextHist(list);
    KVHistogram* obj;
    TH1* hist;
    while( (obj = (KVHistogram*)nextHist()) ){
        if(!obj->IsType("TCutG")){
            hist = obj->GetHisto();
            TString exp = obj->GetExpression();
            TString sel = obj->GetSelection();
            // set selection
            Info("GenerateAllHistograms","Generating histogram: %s", hist->GetTitle());
            SetSelection(sel);
            RemakeHisto(hist,exp);
        }
    }
}

void KVTreeAnalyzer::Streamer(TBuffer &R__b)
{
    // Read serialized object from file
    // For versions < 4, fHistolist contained TH* or TCutG objects:
    // we convert to a list of KVHistogram objects.
    // Flag will be set to say analysis needs saving.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      R__b.ReadClassBuffer(KVTreeAnalyzer::Class(),this,R__v,R__s,R__c);
      if(R__v<4){
          //Info("Streamer","Converting old histo list");
          // convert fHistolist
          if(fHistolist.GetEntries()){
              TList tmp;
              tmp.AddAll(&fHistolist);
              //Info("Streamer","List of histos to import:");
              //tmp.ls();
              fHistolist.SetOwner(kFALSE);
              fHistolist.Clear();
              fHistolist.SetOwner(kTRUE);
              TNamed* obj;
              TIter next(&tmp);
              while( (obj = (TNamed*)next()) ){
                  if(obj->InheritsFrom("TCutG"))
                      fHistolist.Add(new KVHistogram(dynamic_cast<TCutG*>(obj)));
                  else if(obj->InheritsFrom("TH1"))
                      fHistolist.Add(new KVHistogram(dynamic_cast<TH1*>(obj)));
              }
              //Info("Streamer","New histolist:");
              fHistolist.ls();
              SetAnalysisModifiedSinceLastSave(kTRUE);
          }
      }
   } else {
      R__b.WriteClassBuffer(KVTreeAnalyzer::Class(),this);
   }
}





