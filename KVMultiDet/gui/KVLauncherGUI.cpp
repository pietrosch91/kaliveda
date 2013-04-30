//Created by KVClassFactory on Tue Apr 30 09:30:00 2013
//Author: dgruyer

#include "KVLauncherGUI.h"

ClassImp(KVLauncherGUI)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVLauncherGUI</h2>
<h4>Generic GUI launcher</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//________________________________________________________________

KVLauncherGUI::KVLauncherGUI() : TGMainFrame(0, 1, 1, kVerticalFrame)
{
    //  SetDefault();
    //  SetName(ClassName());
    //  Build();
    //  MapAll();
}

//________________________________________________________________
void KVLauncherGUI::SetDefault()
{
    fButtonList.SetOwner(kFALSE);
    SetCleanup(kDeepCleanup);
}

void KVLauncherGUI::Build()
{
    Error("Build","This method should be overwritten in child classes !");
}

//________________________________________________________________
void KVLauncherGUI::MapAll()
{
    MapSubwindows();
    Resize(GetDefaultSize());
    SetWindowName(GetName());
    MapWindow();
}

//________________________________________________________________
KVLauncherGUI::~KVLauncherGUI()
{
    // Destructor
}

//________________________________________________________________
void KVLauncherGUI::AddButton(const char* name, const char* method, TObject* obj)
{
    if(!obj) obj = this;
    const char* classname = ClassName();
    if(obj->Class()->GetListOfAllPublicMethods()->FindObject(method))
    {
        Error("AddAction","%s is not a public method of %s",method,classname);
        return;
    }

    TGTextButton* fButton = new TGTextButton(this, name);
    fButton->Connect("Clicked()", classname, obj, Form("%s()",method));
    fButton->SetName(name);
    AddFrame(fButton, new TGLayoutHints(kLHintsRight|kLHintsExpandX, 2, 2, 2, 2));
    fButtonList.AddLast(fButton);
}

//________________________________________________________________
TGTextButton* KVLauncherGUI::GetButton(const char* method)
{
    return (TGTextButton*)fButtonList.FindObject(method);
}

//________________________________________________________________
void KVLauncherGUI::SetEnabled(const char* method, Bool_t e)
{
    TGTextButton* fButton = GetButton(method);
    if(fButton) fButton->SetEnabled(e);
    else Warning("SetEnabled","No button corresponding to the method '%s'.",method);
}

//________________________________________________________________
Bool_t KVLauncherGUI::IsEnabled(const char* method)
{
    TGTextButton* fButton = GetButton(method);
    if(fButton) return fButton->IsEnabled();
    else
    {
        Warning("IsEnabled","No button corresponding to the method '%s'.",method);
        return kFALSE;
    }
}

//________________________________________________________________
void KVLauncherGUI::DummyMethod()
{

    TGTextButton* fButton = GetButton("DummyMethod");
    Info("DummyMethod","%s !",fButton->GetName());
    SetEnabled("DummyMethod",!IsEnabled("DummyMethod"));
}










