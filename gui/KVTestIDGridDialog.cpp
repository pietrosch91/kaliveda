#include "KVTestIDGridDialog.h"
#include "Riostream.h"
#include "KVIDGridManager.h"
#include "TGFileDialog.h"
#include "TPad.h"
#include <TGMsgBox.h>
#include <TTimer.h>
#include <TClass.h>
#include <TROOT.h>
#include <TGLayout.h>
#include <TCanvas.h>
#include "KVConfig.h"

TString KVTestIDGridDialog::fNameData = "htemp";
TString KVTestIDGridDialog::fNameZreal = "hzreal";
TString KVTestIDGridDialog::fNameZvsE = "hzvse";
Int_t KVTestIDGridDialog::hzrealbins = 500;
Int_t KVTestIDGridDialog::hzrealxmin = 0;
Int_t KVTestIDGridDialog::hzrealxmax = 100;
Int_t KVTestIDGridDialog::hzvsexbins = 500;
Double_t KVTestIDGridDialog::hzvsexmin = 0;
Double_t KVTestIDGridDialog::hzvsexmax = 5000;
Int_t KVTestIDGridDialog::hzvseybins = 500;
Int_t KVTestIDGridDialog::hzvseymin = 0;
Int_t KVTestIDGridDialog::hzvseymax = 100;

ClassImp(KVTestIDGridDialog)
////////////////////////////////////////////////////////////////
// KVTestIDGridDialog
////////////////////////////////////////////////////////////////
    KVTestIDGridDialog::KVTestIDGridDialog(const TGWindow * p,
                                           const TGWindow * main, UInt_t w,
                                           UInt_t h, KVIDGraph * g)
{
   //Dialog box for testing identification grid
   fMain = new TGTransientFrame(p, main, w, h);
   fMain->Connect("CloseWindow()", "KVTestIDGridDialog", this,
                  "DoClose()");
   fMain->DontCallClose();      // to avoid double deletions.

   fSelectedGrid = g;
   // use hierarchical cleaning
   fMain->SetCleanup(kDeepCleanup);

        /******* frame for name of data histo ********/
   fHdataFrame = new TGHorizontalFrame(fMain);
   fHdataNameLabel = new TGLabel(fHdataFrame, "Name of data histo (TH2F)");
   fHdataNameEntry = new TGTextEntry(fNameData, fHdataFrame);
   fHdataNameEntry->SetWidth(150);
   fHdataFrame->AddFrame(fHdataNameEntry,
                         new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHdataFrame->AddFrame(fHdataNameLabel,
                         new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));

        /******* frame for Zreal histo **************/
   fHzrealFrame = new TGGroupFrame(fMain, "ID test histo");
   fHzrealNameFrame = new TGHorizontalFrame(fHzrealFrame);
   fHzrealNameLabel =
       new TGLabel(fHzrealNameFrame, "Name of ID histo (TH1F)");
   fHzrealNameEntry = new TGTextEntry(fNameZreal, fHzrealNameFrame);
   fHzrealNameEntry->SetWidth(150);
   fHzrealNameFrame->AddFrame(fHzrealNameEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzrealNameFrame->AddFrame(fHzrealNameLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzrealBinsFrame = new TGHorizontalFrame(fHzrealFrame);
   fHzrealBinsLabel = new TGLabel(fHzrealBinsFrame, "nbins");
   fHzrealBinsEntry =
       new TGNumberEntry(fHzrealBinsFrame, hzrealbins, 4, 0,
                         TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEAPositive);
   fHzrealXminLabel = new TGLabel(fHzrealBinsFrame, "xmin");
   fHzrealXminEntry =
       new TGNumberEntry(fHzrealBinsFrame, hzrealxmin, 4, 0,
                         TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEAPositive);
   fHzrealXmaxLabel = new TGLabel(fHzrealBinsFrame, "xmax");
   fHzrealXmaxEntry =
       new TGNumberEntry(fHzrealBinsFrame, hzrealxmax, 4, 0,
                         TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEAPositive);
   fHzrealBinsFrame->AddFrame(fHzrealXmaxEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzrealBinsFrame->AddFrame(fHzrealXmaxLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzrealBinsFrame->AddFrame(fHzrealXminEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzrealBinsFrame->AddFrame(fHzrealXminLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzrealBinsFrame->AddFrame(fHzrealBinsEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzrealBinsFrame->AddFrame(fHzrealBinsLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzrealFrame->AddFrame(fHzrealNameFrame,
                          new TGLayoutHints(kLHintsNormal, 5, 5, 2, 2));
   fHzrealFrame->AddFrame(fHzrealBinsFrame,
                          new TGLayoutHints(kLHintsNormal, 5, 5, 2, 2));

        /******* frame for ZvsE histo **************/
   fHzvseFrame = new TGGroupFrame(fMain, "ID vs. E histo");
   fHzvseNameFrame = new TGHorizontalFrame(fHzvseFrame);
   fHzvseNameLabel = new TGLabel(fHzvseNameFrame, "Name of histo (TH2F)");
   fHzvseNameEntry = new TGTextEntry(fNameZvsE, fHzvseNameFrame);
   fHzvseNameEntry->SetWidth(150);
   fHzvseNameFrame->AddFrame(fHzvseNameEntry,
                             new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseNameFrame->AddFrame(fHzvseNameLabel,
                             new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseXBinsFrame = new TGHorizontalFrame(fHzvseFrame);
   fHzvseXBinsLabel = new TGLabel(fHzvseXBinsFrame, "nbins");
   fHzvseXBinsEntry =
       new TGNumberEntry(fHzvseXBinsFrame, hzvsexbins, 4, 0,
                         TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEAPositive);
   fHzvseXminLabel = new TGLabel(fHzvseXBinsFrame, "E : min");
   fHzvseXminEntry =
       new TGNumberEntry(fHzvseXBinsFrame, hzvsexmin, 4, 0,
                         TGNumberFormat::kNESReal);
   fHzvseXmaxLabel = new TGLabel(fHzvseXBinsFrame, "max");
   fHzvseXmaxEntry =
       new TGNumberEntry(fHzvseXBinsFrame, hzvsexmax, 4, 0,
                         TGNumberFormat::kNESReal);
   fHzvseXBinsFrame->AddFrame(fHzvseXmaxEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseXBinsFrame->AddFrame(fHzvseXmaxLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseXBinsFrame->AddFrame(fHzvseXminEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseXBinsFrame->AddFrame(fHzvseXminLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseXBinsFrame->AddFrame(fHzvseXBinsEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseXBinsFrame->AddFrame(fHzvseXBinsLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseYBinsFrame = new TGHorizontalFrame(fHzvseFrame);
   fHzvseYBinsLabel = new TGLabel(fHzvseYBinsFrame, "nbins");
   fHzvseYBinsEntry =
       new TGNumberEntry(fHzvseYBinsFrame, hzvseybins, 4, 0,
                         TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEAPositive);
   fHzvseYminLabel = new TGLabel(fHzvseYBinsFrame, "PID : min");
   fHzvseYminEntry =
       new TGNumberEntry(fHzvseYBinsFrame, hzvseymin, 4, 0,
                         TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEAPositive);
   fHzvseYmaxLabel = new TGLabel(fHzvseYBinsFrame, "max");
   fHzvseYmaxEntry =
       new TGNumberEntry(fHzvseYBinsFrame, hzvseymax, 4, 0,
                         TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEAPositive);
   fHzvseYBinsFrame->AddFrame(fHzvseYmaxEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseYBinsFrame->AddFrame(fHzvseYmaxLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseYBinsFrame->AddFrame(fHzvseYminEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseYBinsFrame->AddFrame(fHzvseYminLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseYBinsFrame->AddFrame(fHzvseYBinsEntry,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseYBinsFrame->AddFrame(fHzvseYBinsLabel,
                              new TGLayoutHints(kLHintsRight, 5, 5, 2, 2));
   fHzvseFrame->AddFrame(fHzvseNameFrame,
                         new TGLayoutHints(kLHintsNormal, 5, 5, 2, 2));
   fHzvseFrame->AddFrame(fHzvseXBinsFrame,
                         new TGLayoutHints(kLHintsNormal, 5, 5, 2, 2));
   fHzvseFrame->AddFrame(fHzvseYBinsFrame,
                         new TGLayoutHints(kLHintsNormal, 5, 5, 2, 2));

        /********** progress bar **********/
   fProgressBar = new TGHProgressBar(fMain, TGProgressBar::kFancy, 300);
   fProgressBar->SetFillType(TGProgressBar::kBlockFill);
   fProgressBar->ShowPosition();
   fProgressBar->SetBarColor("red");

        /************buttons***************/
   fButtonsFrame = new TGHorizontalFrame(fMain, 100, 50);
   fTestButton = new TGTextButton(fButtonsFrame, "&Test");
   fTestButton->Connect("Clicked()", "KVTestIDGridDialog", this,
                        "TestGrid()");
   fCloseButton = new TGTextButton(fButtonsFrame, "&Close");
   fCloseButton->Connect("Clicked()", "KVTestIDGridDialog", this,
                         "DoClose()");

   fButtonsFrame->AddFrame(fTestButton,
                           new TGLayoutHints(kLHintsExpandX |
                                             kLHintsCenterX |
                                             kLHintsCenterY, 2, 2, 5, 5));
   fButtonsFrame->AddFrame(fCloseButton,
                           new TGLayoutHints(kLHintsExpandX |
                                             kLHintsCenterX |
                                             kLHintsCenterY, 2, 2, 5, 5));

   fMain->AddFrame(fHdataFrame,
                   new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 15,
                                     5));
   fMain->AddFrame(fHzrealFrame,
                   new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 5,
                                     5));
   fMain->AddFrame(fHzvseFrame,
                   new TGLayoutHints(kLHintsLeft | kLHintsTop, 2, 2, 5,
                                     10));
   fMain->AddFrame(fProgressBar,
                   new TGLayoutHints(kLHintsCenterX | kLHintsTop, 2, 2, 5,
                                     5));
   fMain->AddFrame(fButtonsFrame,
                   new TGLayoutHints(kLHintsCenterX | kLHintsTop, 2, 2, 5,
                                     5));

   //layout and display window
   fMain->MapSubwindows();
   fMain->Resize(fMain->GetDefaultSize());

   // position relative to the parent's window
   fMain->CenterOnParent();

   fMain->SetWindowName("Test ID grid");
   fMain->MapWindow();

}

void KVTestIDGridDialog::DoClose()
{
   TTimer::SingleShot(150, "KVTestIDGridDialog", this, "CloseWindow()");
}

void KVTestIDGridDialog::CloseWindow()
{
   delete this;
}

KVTestIDGridDialog::~KVTestIDGridDialog()
{
   //Delete all widgets

   delete fMain;
}

void KVTestIDGridDialog::TestGrid()
{
   //read current values of all fields, find/create the histograms,
   //and test the identification

   fNameData = fHdataNameEntry->GetText();
   fNameZreal = fHzrealNameEntry->GetText();
   fNameZvsE = fHzvseNameEntry->GetText();

   if (!CheckNameOK(fNameData) || !CheckNameOK(fNameZreal)
       || !CheckNameOK(fNameZvsE)) {

      new TGMsgBox(gClient->GetRoot(), fMain, "Test ID Grid",
                   "Give a valid name for each histo", kMBIconExclamation);
      return;
   }

   hzrealbins = fHzrealBinsEntry->GetIntNumber();
   hzrealxmin = fHzrealXminEntry->GetIntNumber();
   hzrealxmax = fHzrealXmaxEntry->GetIntNumber();

   hzvsexbins = fHzvseXBinsEntry->GetIntNumber();
   hzvsexmin = fHzvseXminEntry->GetNumber();
   hzvsexmax = fHzvseXmaxEntry->GetNumber();

   hzvseybins = fHzvseYBinsEntry->GetIntNumber();
   hzvseymin = fHzvseYminEntry->GetIntNumber();
   hzvseymax = fHzvseYmaxEntry->GetIntNumber();

   TH2F *hdata = (TH2F *) gROOT->FindObject(fNameData.Data());
   if (!hdata) {

      new TGMsgBox(gClient->GetRoot(), fMain, "Test ID Grid",
                   "Check name of data histogram", kMBIconExclamation);
      return;
   }
   //in case user changes definitions of histo (without changing name),
   //we delete the histogram if it already exists  
   TH1F *hzreal = (TH1F *) gROOT->FindObject(fNameZreal.Data());
   if (hzreal) {
      delete hzreal;
      hzreal = 0;
   }
   hzreal =
       new TH1F(fNameZreal.Data(), "PID distribution", hzrealbins,
                hzrealxmin, hzrealxmax);

   TH2F *hzvse = (TH2F *) gROOT->FindObject(fNameZvsE.Data());
   if (hzvse) {
      delete hzvse;
      hzvse = 0;
   }
   hzvse =
       new TH2F(fNameZvsE.Data(), "PID vs. E_{res}", hzvsexbins, hzvsexmin,
                hzvsexmax, hzvseybins, hzvseymin, hzvseymax);

   //progress bar set up
   fProgressBar->SetRange(0, hdata->GetSum());
   fProgressBar->Reset();
   fSelectedGrid->Connect("Increment(Float_t)", "TGHProgressBar",
                          fProgressBar, "SetPosition(Float_t)");
   // perform initialisation of grid (sort lines, calculate line widths etc.)
   fSelectedGrid->Initialize();
#ifdef __WITHOUT_TGBUTTON_SETENABLED
#define SetEnabled(flag) \
   SetState( flag ? kButtonUp : kButtonDisabled )
#endif
   fTestButton->SetEnabled(kFALSE);
   fCloseButton->SetEnabled(kFALSE);
   fSelectedGrid->TestIdentification(hdata, hzreal, hzvse);
   fTestButton->SetEnabled(kTRUE);
   fCloseButton->SetEnabled(kTRUE);
#ifdef __WITHOUT_TGBUTTON_SETENABLED
#undef SetEnabled
#endif
   fSelectedGrid->Disconnect("Increment(Float_t)", fProgressBar,
                             "SetPosition(Float_t)");
   fProgressBar->Reset();
}

