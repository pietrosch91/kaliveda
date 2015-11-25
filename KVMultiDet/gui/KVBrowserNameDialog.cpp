/***************************************************************************
                          kvbrowsernamedialog.cpp  -  description
                             -------------------
    begin                : Mon Dec 2 2002
    copyright            : (C) 2002 by J.D Frankland & Alexis Mignon
    email                : frankland@ganil.fr, mignon@ganil.fr

$Id: KVBrowserNameDialog.cpp,v 1.3 2003/08/23 09:17:05 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
using std::cout;
using std::endl;
#include "KVBrowserNameDialog.h"
#include "KVBrowserWidget.h"
#include "KVWidgetList.h"
#include "TApplication.h"
#include "TVirtualX.h"
#include "TGButton.h"
#include "TGLayout.h"

ClassImp(KVBrowserNameDialog)
//______________________________________________________
// A pop-up dialogue box asking for the name of a
// layer, ring, telescope, detector etc.
//
KVBrowserNameDialog::KVBrowserNameDialog
(const TGWindow* p, const TGWindow* main, const Char_t* mess,
 Char_t* name)
   : TGTransientFrame(p, main, 200, 50)
{
   //Open a dialogue box to input a string (usually a name)
   //
   //      p = gClient->GetRoot() generally
   //   main = parent window of the dialogue box
   //   mess = string written in dialogue box as invite to user
   //   name = string input by user

   // put OK and Cancel buttons at bottom of box
   TGHorizontalFrame* fFrame1 =
      new TGHorizontalFrame(this, 60, 20, kFixedWidth);

   AddToCleanupList(fFrame1);

   KVBrowserWidget* widg = new KVBrowserWidget(this, NAMEDIALOG_OK);
   TGTextButton* fOKBut = new TGTextButton(fFrame1, "&OK", widg->GetID());
   fOKBut->Associate(this);
   widg->SetWidget(fOKBut);
   AddToWidgetList(widg);
   fOKBut->SetToolTipText("Enter a name then click OK");

   widg = new KVBrowserWidget(this, NAMEDIALOG_CANCEL);
   TGTextButton* fCanBut =
      new TGTextButton(fFrame1, "&Cancel", widg->GetID());
   fCanBut->Associate(this);
   widg->SetWidget(fCanBut);
   AddToWidgetList(widg);
   fCanBut->SetToolTipText("Click Cancel to leave name unchanged");

   TGLayoutHints* fL1 =
      new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                        2, 2, 2, 2);
   TGLayoutHints* fL2 =
      new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);

   AddToCleanupList(fL1);
   AddToCleanupList(fL2);

   fFrame1->AddFrame(fOKBut, fL1);
   fFrame1->AddFrame(fCanBut, fL1);

   fFrame1->Resize(150, fOKBut->GetDefaultHeight());
   AddFrame(fFrame1, fL2);

   MapSubwindows();
   Resize(GetDefaultSize());

   // position relative to the parent's window
   Window_t wdum;
   int ax, ay;
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                                   (Int_t)(((TGFrame*) main)->
                                           GetWidth() - fWidth) >> 1,
                                   (Int_t)(((TGFrame*) main)->
                                           GetHeight() - fHeight) >> 1,
                                   ax, ay, wdum);
   Move(ax, ay);

   SetWindowName("KVBrowserNameDialog");

   MapWindow();
}

//_________________________________________________________________________________
void KVBrowserNameDialog::CloseWindow()
{

   // Called when window is closed via the window manager.

   //DeleteWindow();
   delete this;
}

//________________________________________________________________________________

Bool_t KVBrowserNameDialog::ProcessMessage(Long_t msg, Long_t parm1,
      Long_t parm2)
{
   // Handle messages send to the TestMainFrame object. E.g. all menu button
   // messages.

   // get associated widget from widget list using calling widget ID (parm1)
   KVWidget* widg = 0;
   switch (GET_MSG(msg)) {

      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
               printf("Button was pressed, id = %ld, widget action...\n", parm1);
               if (GetWidgetList())
                  widg = GetWidgetList()->GetWidget(parm1);
               if (widg)
                  widg->Action();
               break;

            case kCM_COMBOBOX:
               if (GetWidgetList())
                  widg = GetWidgetList()->GetWidget(parm1);
               if (widg)
                  widg->Action(parm2);

            default:
               break;
         }

      case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {

            case kTE_TEXTCHANGED:
               cout << "TEXTCHANGED" << endl;
               if (GetWidgetList())
                  widg = GetWidgetList()->GetWidget(parm1);
               if (widg)
                  widg->Action();
         }

      default:
         break;
   }
   return kTRUE;
}
