//Created by KVClassFactory on Wed Apr  3 15:57:14 2013
//Author: John Frankland,,,

#include "KVBatchSystemGUI.h"
#include <KVBatchJob.h>
#include <KVBatchSystemManager.h>

ClassImp(KVBatchSystemGUI)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVBatchSystemGUI</h2>
<h4>GUI for batch system jobs</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Bool_t KVBatchSystemGUI::fOpen=kFALSE;

KVBatchSystemGUI::KVBatchSystemGUI()
{
   // Default constructor
   fOpen=kTRUE;
   
    MainFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
    MainFrame->SetName("BatchSystem GUI");
    TGHorizontalFrame *hf = new TGHorizontalFrame(MainFrame,10,10,kHorizontalFrame);
    BrefreshDir = new TGPictureButton(hf,gClient->GetPicture("refresh2.xpm"));
    hf->AddFrame(BrefreshDir, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    BrefreshDir->Resize(40,40);
    BrefreshDir->SetToolTipText("Update");
    BrefreshDir->Connect("Clicked()", "KVBatchSystemGUI", this, "Refresh()");
    BalterJobs = new TGPictureButton(hf,gClient->GetPicture("bld_edit_s.png"));
    hf->AddFrame(BalterJobs, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    BalterJobs->Resize(40,40);
    BalterJobs->SetToolTipText("Alter job(s) resources");
    BalterJobs->Connect("Clicked()", "KVBatchSystemGUI", this, "AlterJobs()");
    BremDir = new TGPictureButton(hf,gClient->GetPicture("mb_stop_s.xpm"));
    hf->AddFrame(BremDir, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    BremDir->Resize(40,40);
    BremDir->SetToolTipText("Kill job(s)");
    BremDir->Connect("Clicked()", "KVBatchSystemGUI", this, "KillJobs()");
    MainFrame->AddFrame(hf, new TGLayoutHints(kLHintsTop|kLHintsExpandX,2,2,2,2));
    fLVJobs = new KVListView(KVBatchJob::Class(), MainFrame, 550, 200);
    fLVJobs->SetDataColumns(9);
    fLVJobs->SetDataColumn(0, "JobID");
    fLVJobs->SetDataColumn(1, "Name");
    fLVJobs->SetDataColumn(2, "Status");
    fLVJobs->SetDataColumn(3, "Submitted");
    fLVJobs->GetDataColumn(3)->SetIsDateTime(KVDatime::kSQL);
    fLVJobs->SetDataColumn(4, "CPUusage");
    fLVJobs->SetDataColumn(5, "CPUmax");
    fLVJobs->SetDataColumn(6, "MemUsed");
    fLVJobs->SetDataColumn(7, "MemMax");
    fLVJobs->SetDataColumn(8, "DiskMax");
    fLVJobs->ActivateSortButtons();

    if(!gBatchSystemManager) new KVBatchSystemManager;
    if(!gBatchSystem) gBatchSystemManager->GetDefaultBatchSystem()->cd();

    selected_jobs = 0;

    Refresh();
    
    // automatic update every 60 second
    fTimer = new TTimer;
	 fTimer->Connect("Timeout()", "KVBatchSystemGUI", this, "Refresh()");
	 fTimer->Start(60000);

    MainFrame->AddFrame(fLVJobs, new TGLayoutHints(kLHintsTop|kLHintsExpandX|kLHintsExpandY,5,5,10,10));
    MainFrame->SetMWMHints(kMWMDecorAll,
                         kMWMFuncAll,
                         kMWMInputModeless);
    MainFrame->MapSubwindows();

    MainFrame->Resize(MainFrame->GetDefaultSize());
    MainFrame->MapWindow();
    
   MainFrame->Connect("CloseWindow()", "KVBatchSystemGUI", this, "DoClose()");
   MainFrame->DontCallClose();      // to avoid double deletions.
}

KVBatchSystemGUI::~KVBatchSystemGUI()
{
    // Destructor
   fOpen=kFALSE;
    SafeDelete(selected_jobs);
    delete fTimer;
}

void KVBatchSystemGUI::Refresh()
{
    KVList* jobs = gBatchSystem->GetListOfJobs();
    fLVJobs->Display(jobs);
}

void KVBatchSystemGUI::KillJobs()
{
    SafeDelete(selected_jobs);
    selected_jobs = fLVJobs->GetSelectedObjects();
    if(!selected_jobs->GetEntries()) return;
    TIter next(selected_jobs);
    KVBatchJob* job;
    while( (job = (KVBatchJob*)next())) job->DeleteJob();
    Refresh();
}

void KVBatchSystemGUI::AlterJobs()
{
    SafeDelete(selected_jobs);
    selected_jobs = fLVJobs->GetSelectedObjects();
    if(!selected_jobs->GetEntries()) return;
    
    gBatchSystem->AlterJobs(MainFrame, selected_jobs);
    
    Refresh();
}

void KVBatchSystemGUI::DoClose()
{
   TTimer::SingleShot(150, "KVBatchSystemGUI", this, "CloseWindow()");
}

void KVBatchSystemGUI::CloseWindow()
{
   delete this;
}

