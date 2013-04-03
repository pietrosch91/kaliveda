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

KVBatchSystemGUI::KVBatchSystemGUI()
{
   // Default constructor
    MainFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
    MainFrame->SetName("BatchSystem GUI");
    TGHorizontalFrame *hf = new TGHorizontalFrame(MainFrame,10,10,kHorizontalFrame);
    BrefreshDir = new TGPictureButton(hf,gClient->GetPicture("refresh2.xpm"));
    hf->AddFrame(BrefreshDir, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    BrefreshDir->Resize(40,40);
    BrefreshDir->SetToolTipText("Update");
    BrefreshDir->Connect("Clicked()", "KVBatchSystemGUI", this, "Refresh()");
    BremDir = new TGPictureButton(hf,gClient->GetPicture("mb_stop_s.xpm"));
    hf->AddFrame(BremDir, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    BremDir->Resize(40,40);
    BremDir->SetToolTipText("Kill job(s)");
    BremDir->Connect("Clicked()", "KVBatchSystemGUI", this, "KillJobs()");
    MainFrame->AddFrame(hf, new TGLayoutHints(kLHintsTop|kLHintsExpandX,2,2,2,2));
    fLVJobs = new KVListView(KVBatchJob::Class(), MainFrame, 550, 200);
    fLVJobs->SetDataColumns(6);
    fLVJobs->SetDataColumn(0, "JobID");
    fLVJobs->SetDataColumn(1, "Name");
    fLVJobs->SetDataColumn(2, "Status");
    fLVJobs->SetDataColumn(3, "Submitted");
    fLVJobs->GetDataColumn(3)->SetIsDateTime(KVDatime::kSQL);
    fLVJobs->SetDataColumn(4, "CPUusage");
    fLVJobs->SetDataColumn(5, "MemUsed");
    fLVJobs->ActivateSortButtons();

    new KVBatchSystemManager;
    gBatchSystemManager->GetDefaultBatchSystem()->cd();

    selected_jobs = 0;

    Refresh();

    MainFrame->AddFrame(fLVJobs, new TGLayoutHints(kLHintsTop|kLHintsExpandX|kLHintsExpandY,5,5,10,10));
    MainFrame->SetMWMHints(kMWMDecorAll,
                         kMWMFuncAll,
                         kMWMInputModeless);
    MainFrame->MapSubwindows();

    MainFrame->Resize(MainFrame->GetDefaultSize());
    MainFrame->MapWindow();
}

KVBatchSystemGUI::~KVBatchSystemGUI()
{
    // Destructor
    SafeDelete(selected_jobs);
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