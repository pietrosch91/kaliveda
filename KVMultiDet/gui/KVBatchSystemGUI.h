//Created by KVClassFactory on Wed Apr  3 15:57:14 2013
//Author: John Frankland,,,

#ifndef __KVBATCHSYSTEMGUI_H
#define __KVBATCHSYSTEMGUI_H
#include "RQ_OBJECT.h"
#include <KVBase.h>
#include <TGFrame.h>
#include <TGButton.h>
#include "KVListView.h"

class KVBatchSystemGUI : public KVBase
{
    RQ_OBJECT("KVBatchSystemGUI")
    TGMainFrame *MainFrame;
    TGPictureButton *BrefreshDir;
    TGPictureButton *BremDir;
    KVListView* fLVJobs;
    TList* selected_jobs;

   public:
   KVBatchSystemGUI();
   KVBatchSystemGUI(const KVBatchSystemGUI& obj);
   virtual ~KVBatchSystemGUI();

   void Refresh();
   void KillJobs();

   ClassDef(KVBatchSystemGUI,1)//GUI for batch system jobs
};

#endif
