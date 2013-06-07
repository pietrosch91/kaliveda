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

#include "KVDalitzPlot.h"
#include "TEnv.h"
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
   fDrawSame = fApplySelection = fProfileHisto = kFALSE;
   fDrawLog = gEnv->GetValue("KVTreeAnalyzer.LogScale", kFALSE);
   fUserBinning = gEnv->GetValue("KVTreeAnalyzer.UserBinning", kFALSE);
   fUserWeight =  gEnv->GetValue("KVTreeAnalyzer.UserWeight", kFALSE);
   fNewCanvas = gEnv->GetValue("KVTreeAnalyzer.NewCanvas", kFALSE);
   fNormHisto = gEnv->GetValue("KVTreeAnalyzer.Normalize", kFALSE);
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
   fTreeName = t->GetName();
   fTreeFileName = t->GetCurrentFile()->GetName();
   fDrawSame = fApplySelection = fProfileHisto  = kFALSE;
   fDrawLog = gEnv->GetValue("KVTreeAnalyzer.LogScale", kFALSE);
   fUserBinning = gEnv->GetValue("KVTreeAnalyzer.UserBinning", kFALSE);
   fUserWeight =  gEnv->GetValue("KVTreeAnalyzer.UserWeight", kFALSE);
   fNewCanvas = gEnv->GetValue("KVTreeAnalyzer.NewCanvas", kFALSE);
   fNormHisto = gEnv->GetValue("KVTreeAnalyzer.Normalize", kFALSE);
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
   histo.Form(">>%s(%d,%f,%f)", name.Data(), (Xmax-Xmin)+1, Xmin-0.5, Xmax+0.5);
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
   if(fTree) stuff.AddAll(fTree->GetListOfLeaves());
   stuff.AddAll(&fAliasList);
   G_leaflist->Display(&stuff);   
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

   // main frame
   fMain_leaflist = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMain_leaflist->SetName("fMain_leaflist");
   fMain_leaflist->SetWindowName("VARIABLES");
   UInt_t lWidth = 400, lHeight = 500;
   /* leaf list */
   TGHorizontalFrame *fHorizontalFrame = new TGHorizontalFrame(fMain_leaflist,lWidth,36,kHorizontalFrame);
   G_leaf_draw = new TGPictureButton(fHorizontalFrame, "draw_t.xpm");
   G_leaf_draw->SetEnabled(kFALSE);
   G_leaf_draw->Connect("Clicked()", "KVTreeAnalyzer", this, "DrawLeafExpr()");
   fHorizontalFrame->AddFrame(G_leaf_draw, new TGLayoutHints(kLHintsTop|kLHintsLeft,2,2,2,2));
   fLeafExpr="          ";
   G_leaf_expr = new TGLabel(fHorizontalFrame, fLeafExpr.Data());
   G_leaf_expr->Resize();
   fHorizontalFrame->AddFrame(G_leaf_expr, new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsCenterY,2,2,2,2));
   fMain_leaflist->AddFrame(fHorizontalFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));

   TGGroupFrame* histo_opts = new TGGroupFrame(fMain_leaflist, "Options", kVerticalFrame);
   fHorizontalFrame = new TGHorizontalFrame(histo_opts,lWidth,36,kHorizontalFrame);
   G_histo_prof = new TGCheckButton(fHorizontalFrame, "Profile");
   G_histo_prof->SetToolTipText("Generate a profile histogram");
   G_histo_prof->SetState((EButtonState) fProfileHisto );
   G_histo_prof->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetProfileHisto(Bool_t)");
   fHorizontalFrame->AddFrame(G_histo_prof, new TGLayoutHints(kLHintsTop|kLHintsCenterX|kLHintsCenterY,2,2,2,2));

   G_histo_weight = new TGCheckButton(fHorizontalFrame, "Weight");
   G_histo_weight->SetToolTipText("User defined binning of the histogram");
   G_histo_weight->SetState((EButtonState) fUserWeight );
   G_histo_weight->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetUserWeight(Bool_t)");
   fHorizontalFrame->AddFrame(G_histo_weight, new TGLayoutHints(kLHintsTop|kLHintsCenterX|kLHintsCenterY,2,2,2,2));

   G_histo_bin = new TGCheckButton(fHorizontalFrame, "Bins");
   G_histo_bin->SetToolTipText("User defined binning of the histogram");
   G_histo_bin->SetState((EButtonState) fUserBinning);
   G_histo_bin->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetUserBinning(Bool_t)");
   fHorizontalFrame->AddFrame(G_histo_bin, new TGLayoutHints(kLHintsTop|kLHintsCenterX|kLHintsCenterY,2,2,2,2));
   histo_opts->AddFrame(fHorizontalFrame, new TGLayoutHints(kLHintsCenterX|kLHintsTop,2,2,2,2));
   fMain_leaflist->AddFrame(histo_opts, new TGLayoutHints(kLHintsLeft|kLHintsTop|kLHintsExpandX,5,5,5,5));
   
   /* make selection */
   fHorizontalFrame = new TGHorizontalFrame(fMain_leaflist,lWidth,36,kHorizontalFrame);
   TGLabel* lab = new TGLabel(fHorizontalFrame, "Make alias : ");
   fHorizontalFrame->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,2,2,2,2));
   G_alias_text = new TGTextEntry(fHorizontalFrame, new TGTextBuffer(50));
   G_alias_text->SetMaxLength(4096);
   G_alias_text->SetAlignment(kTextLeft);
   G_alias_text->Resize(500,G_alias_text->GetDefaultHeight());
   G_alias_text->Connect("ReturnPressed()", "KVTreeAnalyzer", this, "GenerateAlias()");
   fHorizontalFrame->AddFrame(G_alias_text, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,5,2,2));
   fMain_leaflist->AddFrame(fHorizontalFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));
      
   G_leaflist = new KVListView(TNamed::Class(), fMain_leaflist, lWidth, lHeight);
   G_leaflist->SetDataColumns(1);
   G_leaflist->SetDataColumn(0, "Title");
   G_leaflist->ActivateSortButtons();
   G_leaflist->SetDoubleClickAction("KVTreeAnalyzer", this, "DrawLeaf(TObject*)");
   G_leaflist->Connect("SelectionChanged()", "KVTreeAnalyzer", this, "LeafChanged()");
//   G_leaflist->Connect("ReturnPressed()", "KVTreeAnalyzer", this, "ShowVar()");
   fMain_leaflist->AddFrame(G_leaflist, new TGLayoutHints(kLHintsLeft|kLHintsTop|
                                       kLHintsExpandX|kLHintsExpandY,
				       5,5,5,5));
  
   fMain_leaflist->MapSubwindows();

   fMain_leaflist->Resize(fMain_leaflist->GetDefaultSize());
   fMain_leaflist->MapWindow();
   fMain_leaflist->Resize(lWidth,lHeight);
   FillLeafList();
   // histogram manager window
   fMain_histolist = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMain_histolist->SetName("fMain_histolist");
   fMain_histolist->SetWindowName("HISTOS");
   UInt_t hWidth = 400, hHeight = 600;

             /* menus */
   fMenuFile = new TGPopupMenu(gClient->GetRoot());
   fMenuFile->AddEntry("&Open...", MH_OPEN_FILE);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Save as...", MH_SAVE_FILE);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Quit", MH_QUIT);
   fMenuFile->Connect("Activated(Int_t)", "KVTreeAnalyzer", this, "HandleHistoFileMenu(Int_t)");
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBar = new TGMenuBar(fMain_histolist, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMain_histolist->AddFrame(fMenuBar,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
   TGHorizontal3DLine *lh = new TGHorizontal3DLine(fMain_histolist);
   fMain_histolist->AddFrame(lh, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
   
   G_histo_del = new TGTextButton(fMain_histolist,"DELETE HISTO!");
   G_histo_del->SetTextJustify(36);
   G_histo_del->SetMargins(0,0,0,0);
   G_histo_del->SetWrapLength(-1);
   G_histo_del->Resize(hWidth,G_histo_del->GetDefaultHeight());
   G_histo_del->Connect("Clicked()", "KVTreeAnalyzer", this, "DeleteSelectedHisto()");
   G_histo_del->SetEnabled(kFALSE);
   G_histo_del->ChangeBackground(red);
   fMain_histolist->AddFrame(G_histo_del, new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,2,2,2,2));
   /* histo options */
   histo_opts = new TGGroupFrame(fMain_histolist, "Options", kVerticalFrame);
   fHorizontalFrame = new TGHorizontalFrame(histo_opts,hWidth,50,kHorizontalFrame);
   G_histo_new_can = new TGCheckButton(fHorizontalFrame, "New canvas");
   G_histo_new_can->SetToolTipText("Draw in a new canvas");
   G_histo_new_can->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetNewCanvas(Bool_t)");
   G_histo_new_can->SetState((EButtonState)fNewCanvas);
   fHorizontalFrame->AddFrame(G_histo_new_can, new TGLayoutHints(kLHintsLeft|kLHintsCenterX,2,2,2,2));
   G_histo_same = new TGCheckButton(fHorizontalFrame, "Same");
   G_histo_same->SetToolTipText("Draw in same pad");
   G_histo_same->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetDrawSame(Bool_t)");
   fHorizontalFrame->AddFrame(G_histo_same, new TGLayoutHints(kLHintsLeft|kLHintsCenterX,2,2,2,2));
   G_histo_app_sel = new TGCheckButton(fHorizontalFrame, "Apply selection");
   G_histo_app_sel->SetToolTipText("Apply current selection to generate new histo");
   G_histo_app_sel->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetApplySelection(Bool_t)");
   fHorizontalFrame->AddFrame(G_histo_app_sel, new TGLayoutHints(kLHintsLeft|kLHintsCenterX,2,2,2,2));
   histo_opts->AddFrame(fHorizontalFrame, new TGLayoutHints(kLHintsCenterX|kLHintsTop,2,2,2,2));
   fHorizontalFrame = new TGHorizontalFrame(histo_opts,hWidth,50,kHorizontalFrame);
   G_histo_log = new TGCheckButton(fHorizontalFrame, "Log scale");
   G_histo_log->SetToolTipText("Use log scale in Y (1D) or Z (2D)");
   G_histo_log->SetState((EButtonState)fDrawLog);
   G_histo_log->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetDrawLog(Bool_t)");
   fHorizontalFrame->AddFrame(G_histo_log, new TGLayoutHints(kLHintsLeft|kLHintsCenterX,2,2,2,2));
   G_histo_norm = new TGCheckButton(fHorizontalFrame, "Normalize");
   G_histo_norm->SetToolTipText("Generate normalized histogram with integral=1");
   G_histo_norm->SetState((EButtonState) fNormHisto );
   G_histo_norm->Connect("Toggled(Bool_t)", "KVTreeAnalyzer", this, "SetNormHisto(Bool_t)");
   fHorizontalFrame->AddFrame(G_histo_norm, new TGLayoutHints(kLHintsLeft|kLHintsCenterX,2,2,2,2));
   histo_opts->AddFrame(fHorizontalFrame, new TGLayoutHints(kLHintsCenterX|kLHintsTop,2,2,2,2));
   fMain_histolist->AddFrame(histo_opts, new TGLayoutHints(kLHintsLeft|kLHintsTop|kLHintsExpandX,5,5,5,5));
   /* ip scale */
   histo_opts = new TGGroupFrame(fMain_histolist, "Impact parameter", kHorizontalFrame);
   G_make_ip_scale = new TGTextButton(histo_opts,"Make scale");
   G_make_ip_scale->SetTextJustify(36);
   G_make_ip_scale->SetMargins(0,0,0,0);
   G_make_ip_scale->SetWrapLength(-1);
   G_make_ip_scale->Resize();
   G_make_ip_scale->SetEnabled(kFALSE);
   G_make_ip_scale->Connect("Clicked()","KVTreeAnalyzer",this,"MakeIPScale()");
   G_make_ip_scale->ChangeBackground(green);
   histo_opts->AddFrame(G_make_ip_scale, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,10,2));
   lab = new TGLabel(histo_opts,"b <");
   histo_opts->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsTop,5,2,12,2));
   G_make_ip_selection = new TGTextEntry(histo_opts, new TGTextBuffer(5));
   G_make_ip_selection->SetMaxLength(10);
   G_make_ip_selection->SetAlignment(kTextLeft);
   G_make_ip_selection->Resize(50,G_make_ip_selection->GetDefaultHeight());
   G_make_ip_selection->Connect("ReturnPressed()", "KVTreeAnalyzer", this, "GenerateIPSelection()");
   G_make_ip_selection->SetEnabled(kFALSE);
   histo_opts->AddFrame(G_make_ip_selection, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,10,2));
   G_ip_histo = new TGLabel(histo_opts,"-");
   histo_opts->AddFrame(G_ip_histo, new TGLayoutHints(kLHintsLeft|kLHintsTop,5,2,12,2));
   fMain_histolist->AddFrame(histo_opts, new TGLayoutHints(kLHintsLeft|kLHintsExpandX,5,5,5,5));
   /* ip scale */
   histo_opts = new TGGroupFrame(fMain_histolist, "Fits", kHorizontalFrame);
   lab = new TGLabel(histo_opts,"Gumbel : ");
   histo_opts->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,5,2));
   G_fit1 = new TGTextButton(histo_opts, " 1 ");
   G_fit1->SetTextJustify(36);
   G_fit1->SetMargins(0,0,0,0);
   G_fit1->SetWrapLength(-1);
   G_fit1->Resize();
   G_fit1->SetEnabled(kFALSE);
   G_fit1->ChangeBackground(gura);
   G_fit1->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGum1()");
   histo_opts->AddFrame(G_fit1, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
   G_fit2 = new TGTextButton(histo_opts, " 2 ");
   G_fit2->SetTextJustify(36);
   G_fit2->SetMargins(0,0,0,0);
   G_fit2->SetWrapLength(-1);
   G_fit2->Resize();
   G_fit2->SetEnabled(kFALSE);
   G_fit2->ChangeBackground(gurb);
   G_fit2->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGum2()");
   histo_opts->AddFrame(G_fit2, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
   G_fit3 = new TGTextButton(histo_opts, " 3 ");
   G_fit3->SetTextJustify(36);
   G_fit3->SetMargins(0,0,0,0);
   G_fit3->SetWrapLength(-1);
   G_fit3->Resize();
   G_fit3->SetEnabled(kFALSE);
   G_fit3->ChangeBackground(gurc);
   G_fit3->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGum3()");
   histo_opts->AddFrame(G_fit3, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
   lab = new TGLabel(histo_opts,"Gaus+Gum : ");
   histo_opts->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsTop,10,2,5,2));
   G_fitGG1 = new TGTextButton(histo_opts, " 1 ");
   G_fitGG1->SetTextJustify(36);
   G_fitGG1->SetMargins(0,0,0,0);
   G_fitGG1->SetWrapLength(-1);
   G_fitGG1->Resize();
   G_fitGG1->SetEnabled(kFALSE);
   G_fitGG1->ChangeBackground(gura);
   G_fitGG1->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGausGum1()");
   histo_opts->AddFrame(G_fitGG1, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
   G_fitGG2 = new TGTextButton(histo_opts, " 2 ");
   G_fitGG2->SetTextJustify(36);
   G_fitGG2->SetMargins(0,0,0,0);
   G_fitGG2->SetWrapLength(-1);
   G_fitGG2->Resize();
   G_fitGG2->SetEnabled(kFALSE);
   G_fitGG2->ChangeBackground(gurb);
   G_fitGG2->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGausGum2()");
   histo_opts->AddFrame(G_fitGG2, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
   G_fitGG3 = new TGTextButton(histo_opts, " 3 ");
   G_fitGG3->SetTextJustify(36);
   G_fitGG3->SetMargins(0,0,0,0);
   G_fitGG3->SetWrapLength(-1);
   G_fitGG3->Resize();
   G_fitGG3->SetEnabled(kFALSE);
   G_fitGG3->ChangeBackground(gurc);
   G_fitGG3->Connect("Clicked()", "KVTreeAnalyzer", this, "FitGausGum3()");
   histo_opts->AddFrame(G_fitGG3, new TGLayoutHints(kLHintsLeft|kLHintsTop,2,2,2,2));
   fMain_histolist->AddFrame(histo_opts, new TGLayoutHints(kLHintsLeft|kLHintsExpandX,5,5,5,5));
   
   /* histo list */
   G_histolist = new KVListView(TNamed::Class(), fMain_histolist, hWidth, hHeight);
   G_histolist->SetDataColumns(1);
   G_histolist->SetDataColumn(0, "Data", "GetTitle", kTextLeft);
   G_histolist->ActivateSortButtons();
   G_histolist->SetMaxColumnSize(500);
   G_histolist->SetDoubleClickAction("KVTreeAnalyzer", this, "DrawHisto(TObject*)");
   G_histolist->Connect("SelectionChanged()","KVTreeAnalyzer",this,"HistoSelectionChanged()");
   fMain_histolist->AddFrame(G_histolist, new TGLayoutHints(kLHintsLeft|kLHintsTop|
                                       kLHintsExpandX|kLHintsExpandY,
				       5,5,5,5));
  
   fMain_histolist->MapSubwindows();

   fMain_histolist->Resize(fMain_histolist->GetDefaultSize());
   fMain_histolist->MapWindow();
   fMain_histolist->Resize(hWidth,hHeight);
   G_histolist->Display(&fHistolist);
   // SELECTIONS main frame
   UInt_t sWidth = 600, sHeight = 360;
   fMain_selectionlist = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
   fMain_selectionlist->SetName("fMain_selectionlist");
   fMain_selectionlist->SetWindowName("SELECTIONS");
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
   G_selection_text->Resize(500,G_selection_text->GetDefaultHeight());
   G_selection_text->Connect("ReturnPressed()", "KVTreeAnalyzer", this, "GenerateSelection()");
   fHorizontalFrame1614->AddFrame(G_selection_text, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,5,2,2));
   fMain_selectionlist->AddFrame(fHorizontalFrame1614, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));
   
   TGHorizontalFrame *fHorizontalFrameSel = new TGHorizontalFrame(fMain_selectionlist,sWidth,36,kHorizontalFrame);
   G_selection_but = new TGTextButton(fHorizontalFrameSel,"Combine (&and)");
   G_selection_but->SetTextJustify(36);
   G_selection_but->SetMargins(0,0,0,0);
   G_selection_but->SetWrapLength(-1);
   G_selection_but->Resize(sWidth,G_selection_but->GetDefaultHeight());
   G_selection_but->Connect("Clicked()", "KVTreeAnalyzer", this, "CombineSelectionsAnd()");
   G_selection_but->SetEnabled(kFALSE);
   G_selection_but->ChangeBackground(cyan);
   fHorizontalFrameSel->AddFrame(G_selection_but, new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,2,2,2,2));
   
   G_selection_but_or = new TGTextButton(fHorizontalFrameSel,"Combine (&or)");
   G_selection_but_or->SetTextJustify(36);
   G_selection_but_or->SetMargins(0,0,0,0);
   G_selection_but_or->SetWrapLength(-1);
   G_selection_but_or->Resize(sWidth,G_selection_but_or->GetDefaultHeight());
   G_selection_but_or->Connect("Clicked()", "KVTreeAnalyzer", this, "CombineSelectionsOr()");
   G_selection_but_or->SetEnabled(kFALSE);
   G_selection_but_or->ChangeBackground(cyan);
   fHorizontalFrameSel->AddFrame(G_selection_but_or, new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,2,2,2,2));
   
   G_delete_but = new TGTextButton(fHorizontalFrameSel,"&Delete");
   G_delete_but->SetTextJustify(36);
   G_delete_but->SetMargins(0,0,0,0);
   G_delete_but->SetWrapLength(-1);
   G_delete_but->Resize(sWidth,G_delete_but->GetDefaultHeight());
   G_delete_but->Connect("Clicked()", "KVTreeAnalyzer", this, "DeleteSelections()");
   G_delete_but->SetEnabled(kFALSE);
   G_delete_but->ChangeBackground(red);
   fHorizontalFrameSel->AddFrame(G_delete_but, new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,2,2,2,2));
   
   G_update_but = new TGTextButton(fHorizontalFrameSel,"&Update");
   G_update_but->SetTextJustify(36);
   G_update_but->SetMargins(0,0,0,0);
   G_update_but->SetWrapLength(-1);
   G_update_but->Resize(sWidth,G_update_but->GetDefaultHeight());
   G_update_but->Connect("Clicked()", "KVTreeAnalyzer", this, "UpdateEntryLists()");
   G_update_but->SetEnabled(kTRUE);
   G_update_but->ChangeBackground(yellow);
   fHorizontalFrameSel->AddFrame(G_update_but, new TGLayoutHints(kLHintsLeft | kLHintsTop| kLHintsExpandX,2,2,2,2));
   fMain_selectionlist->AddFrame(fHorizontalFrameSel, new TGLayoutHints(kLHintsExpandX | kLHintsTop,1,1,1,1));

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
   
   fMain_selectionlist->MapSubwindows();

   fMain_selectionlist->Resize(fMain_selectionlist->GetDefaultSize());
   fMain_selectionlist->MapWindow();
   fMain_selectionlist->Resize(sWidth,sHeight);
   G_selectionlist->Display(&fSelections);
}  
   
void KVTreeAnalyzer::AddHisto(TH1*h)
{
   // Adds histogram to internal list of user histograms
   // and updates GUI display
   
   fHistolist.Add(h);
   G_histolist->Display(&fHistolist);
}
   
void KVTreeAnalyzer::AddSelection(TEntryList*e)
{
   // Adds selection to internal list of user histograms
   // and updates GUI display
   
   fSelections.Add(e);
   G_selectionlist->Display(&fSelections);
}

void KVTreeAnalyzer::AddCut(TCutG* c)
{
   // Adds histogram to internal list of user histograms
   // and updates GUI display
   
   fHistolist.Add(c);
   G_histolist->Display(&fHistolist);
}

void KVTreeAnalyzer::SetTree(TTree* t)
{
   // Connects a TTree for analysis
   
   fTree = t;
   fTreeName = t->GetName();
   fTreeFileName = t->GetCurrentFile()->GetName();
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
    TH1* hh = 0;
    while((hh = (TH1*)next()))
      {
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
   
   if(obj->InheritsFrom("TCutG"))
     {
     TCutG* cut = dynamic_cast<TCutG*>(obj);
     DrawCut(cut);
     return;
     }
   
   if(!obj->InheritsFrom("TH1")) return;
   TH1* histo = dynamic_cast<TH1*>(obj);
   
   // if histogram is already displayed in active pad and if the pad also
   // contains a graphical contour (TCutG) object, we use the contour to define
   // a new data selection (TEntryList) which is added to the internal list.
   if(gPad && gPad->GetListOfPrimitives()->FindObject(histo) && (gen)){
      TIter next(gPad->GetListOfPrimitives());
      TObject* o;
      while( (o = next()) ){
         if(o->IsA() == TCutG::Class()){
            MakeSelection(o->GetName());
            return;
         }
      }
   }
   
   TString exp,sel;
   
   ParseHistoTitle(histo->GetTitle(),exp,sel);
   
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
      if(fNewCanvas || !gPad) {KVCanvas*c=new KVCanvas; c->SetTitle(histo->GetTitle());c->SetWindowSize(600,600);}
      histo->SetLineColor(my_color_array[0]);
      if(histo->InheritsFrom("TH2")) gPad->SetLogz(fDrawLog);
      else {
         histo->SetMaximum(-1111);//in case maximum was changed to accomodate superimposition
         gPad->SetLogy(fDrawLog);
      }
      histo->Draw();
      gPad->Modified();gPad->Update();
   }
}
   
void KVTreeAnalyzer::ParseHistoTitle(const Char_t* title, TString& exp, TString& sel)
{
   // Take histo title "VAREXP { SELECTION }"
   // and separate the two components
   
   exp="";
   sel="";
   TString tmp(title);
   Int_t ss = tmp.Index("{");
   if(ss>0){
      Int_t se = tmp.Index("}");
      sel = tmp(ss+1, se-ss-1);
      sel.Remove(TString::kBoth,' ');
      exp = tmp(0, ss);
      exp.Remove(TString::kBoth,' ');
   }
   else
   {
      exp = tmp;
      exp.Remove(TString::kBoth,' ');      
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
   TH1* histo = (TH1*)fHistolist.FindObjectWithMethod(htit,"GetTitle");
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
   // Method called when user hits 'combine selections' button in selections GUI.
   // Generates new selection which is the intersection (logical AND) of
   // the currently selected selections.
   
   if(fSelectedSelections){
      int nsel = fSelectedSelections->GetEntries();
      if(nsel<1) return;
      for(int i=0; i<nsel; i++){
	TEntryList* el = (TEntryList*)fSelectedSelections->At(i);
	if(!el) continue;
	fSelections.Remove(el);
	delete el;
	
      }
	G_selectionlist->Display(&fSelections);
   }
}

void KVTreeAnalyzer::SelectionChanged()
{
   // Method called whenever the selected selection in the GUI list changes
   
   SafeDelete(fSelectedSelections);
   fSelectedSelections = G_selectionlist->GetSelectedObjects();
   if(fSelectedSelections && fSelectedSelections->GetEntries()>1) 
     {
     G_selection_but_or->SetEnabled(kTRUE);
     G_selection_but->SetEnabled(kTRUE);
     }
   else 
     {
     G_selection_but_or->SetEnabled(kFALSE);
     G_selection_but->SetEnabled(kFALSE);
     }

   if(fSelectedSelections && fSelectedSelections->GetEntries()>0) G_delete_but->SetEnabled(kTRUE);
   else G_delete_but->SetEnabled(kFALSE);
}

void KVTreeAnalyzer::LeafChanged()
{
   // Method called whenever the leaf/alias selection in the TTree GUI list changes.
   // Updates the names of the leaves/aliases displayed next to the 'draw' button.
   
   fSelectedLeaves = G_leaflist->GetPickOrderedSelectedObjects();
   fLeafExpr="-";
   fXLeaf=fYLeaf=0;
   G_leaf_draw->SetEnabled(kFALSE);
   Int_t nleaf = fSelectedLeaves->GetEntries();
   if(nleaf){
      if(nleaf==1){
         fXLeaf = (TNamed*)fSelectedLeaves->First();
         fLeafExpr = (fXLeaf->InheritsFrom("TLeaf") ? fXLeaf->GetName():  fXLeaf->GetTitle()); 
         G_leaf_draw->SetEnabled(kTRUE);
      }
      else if(nleaf==2){
          fXLeaf = (TNamed*)fSelectedLeaves->At(1);
          fYLeaf = (TNamed*)fSelectedLeaves->First();
          TString X,Y;
          X = (fXLeaf->InheritsFrom("TLeaf") ? fXLeaf->GetName():  fXLeaf->GetTitle());
          Y = (fYLeaf->InheritsFrom("TLeaf") ? fYLeaf->GetName():  fYLeaf->GetTitle());
          fLeafExpr.Form("%s:%s", Y.Data(), X.Data());
          G_leaf_draw->SetEnabled(kTRUE);
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
      }
      else{
          fLeafExpr="-";
      }
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
       histo = (TH1*)fHistolist.FindObjectWithMethod(htit,"GetTitle");
      if(!histo){
         if(type=="Int_t"||type=="Char_t"||type=="Short_t"){
            Int_t xmin = fTree->GetMinimum(expr);
            Int_t xmax = fTree->GetMaximum(expr);
            if(type=="Char_t"){
               TString tmp; tmp.Form("int(%s)",expr.Data());
               expr=tmp.Data();
            }
            histo = MakeIntHisto(expr, "", xmin, xmax);
         }
         else
         {
            histo = MakeHisto(expr, "", 500);
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
       histo = (TH1*)fHistolist.FindObjectWithMethod(htit,"GetTitle");
      if(!histo) histo = MakeHisto(expr, "", 500);
      if(!histo) return;
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
   G_make_ip_scale->SetEnabled(kFALSE);
         G_fit1->SetEnabled(kFALSE);
         G_histo_del->SetEnabled(kFALSE);
         G_fit2->SetEnabled(kFALSE);
         G_fit3->SetEnabled(kFALSE);
         G_fitGG1->SetEnabled(kFALSE);
         G_fitGG2->SetEnabled(kFALSE);
         G_fitGG3->SetEnabled(kFALSE);
   fSelectedHistos = G_histolist->GetSelectedObjects();
   if(fSelectedHistos && fSelectedHistos->GetEntries()==1)
   {
         G_histo_del->SetEnabled(kTRUE);
      if(fSelectedHistos->First()->IsA()==TH1F::Class()){
         G_make_ip_scale->SetEnabled(kTRUE);
         G_fit1->SetEnabled(kTRUE);
         G_fit2->SetEnabled(kTRUE);
         G_fit3->SetEnabled(kTRUE);
         G_fitGG1->SetEnabled(kTRUE);
         G_fitGG2->SetEnabled(kTRUE);
         G_fitGG3->SetEnabled(kTRUE);
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
   TString ipvar,ipsel;
   ParseHistoTitle(histotit.Data(),ipvar,ipsel);
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
   TString filename;
   filename.Form("Analysis_%s", gSystem->BaseName(fTreeFileName.Data()));
   SaveAs(filename);
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
}
   
TList* KVTreeAnalyzer::GetHistosByData(const Char_t* expr)
{
   // fill and return TList with histos containing the given data
   // which may be 1D ("mult", "zmax", etc.) or 2D ("zmax:mult")
   // DELETE LIST AFTER USE
   TList* hlist = new TList;
   TIter next(&fHistolist);
   TString var,sel;
   TH1* h;
   while( (h=(TH1*)next()) ){
      ParseHistoTitle(h->GetTitle(),var,sel);
      if(var==expr) hlist->Add(h);
   }
   return hlist;
}
   
TList* KVTreeAnalyzer::GetHistosBySelection(const Char_t* expr)
{
   // fill and return TList with histos using the given selection criteria
   // DELETE LIST AFTER USE
   TList* hlist = new TList;
   TIter next(&fHistolist);
   TString var,sel;
   TH1* h;
   while( (h=(TH1*)next()) ){
      ParseHistoTitle(h->GetTitle(),var,sel);
      if(sel==expr) hlist->Add(h);
   }
   return hlist;
}
   
TH1* KVTreeAnalyzer::GetHisto(const Char_t* expr, const Char_t* selection)
{
   TIter next(&fHistolist);
   TString var,sel;
   TH1* h;
   while( (h=(TH1*)next()) ){
      ParseHistoTitle(h->GetTitle(),var,sel);
      if(var==expr&&sel==selection) return h;
   }
   return 0;
}
   
void KVTreeAnalyzer::DeleteHisto(const Char_t* expr, const Char_t* selection)
{
   TH1* h = GetHisto(expr,selection);
   if(h){
      cout << "Deleting histo " << h->GetName() << endl;
      fHistolist.Remove(h);
      delete h;
   }
   G_histolist->Display(&fHistolist);
}

void KVTreeAnalyzer::DeleteSelectedHisto()
{
   TObject* histo = fSelectedHistos->First();
   if(!histo) return;
   fHistolist.Remove(histo);
   if(histo->InheritsFrom("TH1")) delete histo;
   G_histolist->Display(&fHistolist);   
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
         
      case MH_SAVE_FILE:
         HistoFileMenu_Save();
         break;
      case MH_QUIT:
         gROOT->ProcessLine(".q");
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
   TGFileInfo fi;
   fi.fFileTypes = filetypes;
   fi.fIniDir = StrDup(dir);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain_histolist, kFDOpen, &fi);
   if (fi.fFilename) {
      OpenAnyFile(fi.fFilename);
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
   TString filename;
   filename.Form("Analysis_%s", gSystem->BaseName(fTreeFileName.Data()));
   fi.fFilename = StrDup(filename);
   new TGFileDialog(gClient->GetDefaultRoot(), fMain_histolist, kFDSave, &fi);
   if (fi.fFilename) {
      //if no ".xxx" ending given, we add ".root"
      TString filenam(fi.fFilename);
      if (!filenam.Contains('.'))
         filenam += ".root";
      SaveAs(filenam);
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
         G_selectionlist->Display(&fSelections);
         FillLeafList();
      }
      delete trees;
      TIter next(&keys);
      TKey* akey;
      while( (akey=(TKey*)next()) ){
          if(TClass::GetClass(akey->GetClassName())->InheritsFrom("TH1")){
             TH1*h=(TH1*)fHistolist.FindObject(akey->GetName());
             if(!h) h = (TH1*)file->Get(akey->GetName());
             if(h->InheritsFrom("TH2")) h->SetOption("col");
             h->SetDirectory(0);
             fHistolist.Add(h);
         }
      }
      G_histolist->Display(&fHistolist);
   }
}
