//Created by KVClassFactory on Wed Dec 12 10:11:10 2012
//Author: dgruyer

#include "KVZAFinderDialog.h"

Int_t   KVZAFinderDialog::fZmin  = -1;
Int_t   KVZAFinderDialog::fZmax  = -1;
TString KVZAFinderDialog::fAList = "1,4,7,9,11,12,14,16,19,21,23,25,27,29,31,34,37,40,41";

Int_t   KVZAFinderDialog::fBinsByZ      = 60;
Int_t   KVZAFinderDialog::fStepsByLine  = -1;

ClassImp(KVZAFinderDialog)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVZAFinderDialog</h2>
<h4>dialog box for KVZALineFinder classe</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//________________________________________________________________
KVZAFinderDialog::KVZAFinderDialog(KVIDGraph * g, TH2* data_histo)
{
  fGrid  = g;
  fHisto = data_histo;
  if((fZmin<0)&&fGrid) fZmin = ((KVIDentifier*)fGrid->GetIdentifiers()->First())->GetPID();
  if((fZmax<0)&&fGrid) fZmax = ((KVIDentifier*)fGrid->GetIdentifiers()->Last())->GetPID() + 1.0;
  
  //Dialog box for testing identification grid
  fMain = new TGTransientFrame(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), 10, 10);
  fMain->Connect("CloseWindow()", "KVSpIdGUI", this, "DoClose()");
  fMain->DontCallClose();
  fMain->SetCleanup(kDeepCleanup);

       /******* frame for initialization **************/
  fInitFrame = new TGGroupFrame(fMain, "Initialization");
  
  fZpFrame = new TGHorizontalFrame(fInitFrame);
  fZminLabel = new TGLabel(fZpFrame, "Zmin ");
  fZminEntry = new TGNumberEntry(fZpFrame, fZmin, 6, 0, 
		       TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber);
  fZpFrame->AddFrame(fZminLabel,new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
  fZpFrame->AddFrame(fZminEntry,new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
  
  fZmaxLabel = new TGLabel(fZpFrame, "  Zmax ");
  fZmaxEntry = new TGNumberEntry(fZpFrame, fZmax, 6, 0,
		       TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber);
  fZpFrame->AddFrame(fZmaxLabel,new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
  fZpFrame->AddFrame(fZmaxEntry,new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
  
  fZbinLabel = new TGLabel(fZpFrame, "  Bins ");
  fZbinEntry = new TGNumberEntry(fZpFrame, fBinsByZ, 6, 0,
		       TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber);
  
  fZpFrame->AddFrame(fZbinLabel,new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
  fZpFrame->AddFrame(fZbinEntry,new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
  fInitFrame->AddFrame(fZpFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));

  fApFrame = new TGHorizontalFrame(fInitFrame);
  fALabel = new TGLabel(fApFrame, "A  ");
  fAEntry = new TGTextEntry(fAList.Data(), fApFrame);
  fApFrame->AddFrame(fALabel, new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
  fApFrame->AddFrame(fAEntry, new TGLayoutHints(kLHintsLeft|kLHintsExpandX, 2, 2, 2, 2));
  fInitFrame->AddFrame(fApFrame, new TGLayoutHints(kLHintsExpandX, 2, 2, 5, 5));
    
       /********** linearization bar **********/
  fLinearFrame = new TGGroupFrame(fMain, "Linearization");
  fLinearBar = new TGHProgressBar(fLinearFrame, TGProgressBar::kFancy, 300);
  fLinearBar->SetFillType(TGProgressBar::kBlockFill);
  fLinearBar->ShowPosition();
  fLinearBar->SetBarColor("green");
  fLinearFrame->AddFrame(fLinearBar,new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));

       /********** progress bar **********/
  fProgressFrame = new TGGroupFrame(fMain, "Progress");
  fProgressBar = new TGHProgressBar(fProgressFrame, TGProgressBar::kFancy, 300);
  fProgressBar->SetFillType(TGProgressBar::kBlockFill);
  fProgressBar->ShowPosition();
  fProgressBar->SetBarColor("red");
  fProgressFrame->AddFrame(fProgressBar,new TGLayoutHints(kLHintsExpandX | kLHintsTop,5,5,5,5));
  
       /************buttons***************/
  fButtonsFrame = new TGHorizontalFrame(fMain, 100, 50);
  fTestButton = new TGTextButton(fButtonsFrame, "&Process");
  fTestButton->Connect("Clicked()", "KVZAFinderDialog", this,
		       "ProcessIdentification()");
  fCloseButton = new TGTextButton(fButtonsFrame, "&Close");
  fCloseButton->Connect("Clicked()", "KVZAFinderDialog", this,
			"DoClose()");

  fButtonsFrame->AddFrame(fTestButton,
			  new TGLayoutHints(kLHintsExpandX |
					    kLHintsCenterX |
					    kLHintsCenterY, 2, 2, 5, 5));
  fButtonsFrame->AddFrame(fCloseButton,
			  new TGLayoutHints(kLHintsExpandX |
					    kLHintsCenterX |
					    kLHintsCenterY, 2, 2, 5, 5));


  fMain->AddFrame(fInitFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 2, 2, 10, 2));
  fMain->AddFrame(fLinearFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
  fMain->AddFrame(fProgressFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
//   fMain->AddFrame(fProgressBar, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 5, 5, 5,5));
   fMain->AddFrame(fButtonsFrame, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 2, 2, 2, 2));

  //layout and display window
  fMain->MapSubwindows();
  fMain->Resize(fMain->GetDefaultSize());

  // position relative to the parent's window
  fMain->CenterOnParent();

  fMain->SetWindowName("Masses Identification");
  fMain->MapWindow();


}

//________________________________________________________________
KVZAFinderDialog::KVZAFinderDialog (const KVZAFinderDialog& obj) 
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

}

//________________________________________________________________
KVZAFinderDialog::~KVZAFinderDialog()
{
   // Destructor
}

//________________________________________________________________
void KVZAFinderDialog::DoClose()
{
   TTimer::SingleShot(150, "KVZAFinderDialog", this, "CloseWindow()");
}

//________________________________________________________________
void KVZAFinderDialog::CloseWindow()
{
   delete this;
}

//________________________________________________________________
void KVZAFinderDialog::ProcessIdentification()
{

}
