/***************************************************************************
                          kvbrowserwidget.cpp  -  description
                             -------------------
    begin                : Mon Nov 18 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVBrowserWidget.cpp,v 1.6 2006/10/19 14:32:43 franklan Exp $
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
#include "TROOT.h"
#include "TGClient.h"
#include "TApplication.h"
#include "TVirtualX.h"
#include "KVBrowserWidget.h"
#include "KVBrowserNameDialog.h"
#include "KVMultiDetArray.h"
#include "KVMultiDetBrowser.h"
#include "KVRing.h"

ClassImp(KVBrowserWidget)
//______________________________________________________________________________
//Extension de la classe KVWidget pour les besoins specifiques de KVBrowser.
//
//______________________________________________________________________________
KVMultiDetArray* KVBrowserWidget::fMDA = 0;

KVBrowserWidget::KVBrowserWidget(): KVWidget()
{
}

KVBrowserWidget::KVBrowserWidget(TObject* obj, Long_t func): KVWidget(obj,
         func)
{
//In ctor, check if object pointer being passed points to multidet array.
//If it does then it is used to intialise the static pointer fMDA so that
//all widgets have access to the array structure.
   if (obj->InheritsFrom("KVMultiDetArray"))
      fMDA = (KVMultiDetArray*) obj;
}

KVBrowserWidget::~KVBrowserWidget()
{
}

void KVBrowserWidget::Action(Long_t parm) const
{
//Perform the action associated with the widget.
//These actions are specific to KVBrowser

   switch (GetFunction()) {

      case ADD_LAYER:
         // add a layer to the structure
         cout << "ADD_LAYER for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
//Char_t name[32];
//new KVBrowserNameDialog(gClient->GetRoot(), fMDA->GetBrowser(),
//                        "Enter a name for the layer:", name);
         fMDA->AddLayer();
         fMDA->UpdateArray();
         break;

      case UPDATE_ARRAY:
         /*cout << "UPDATE_ARRAY for " << GetObject()->ClassName()
            << " " << ((KVBase *)GetObject())->GetName() << endl; */
         fMDA->UpdateArray();
         break;

      case QUIT_KVBROWSER:
         /*cout << "QUIT_KVBROWSER for " << GetObject()->ClassName()
            << " " << GetObject()->GetName() << endl; */
         //((KVMultiDetBrowser *)GetObject())->CloseWindow();
         fMDA->CloseBrowser();
         break;

      case ADD_RING:
//cout << "ADD_RING for " << GetObject()->ClassName()
//<< " " << ((KVBase *) GetObject())->GetName() << endl;
         ((KVLayer*) GetObject())->AddRing();
         fMDA->UpdateArray();
         break;

      case REMOVE_LAYER:
         /*cout << "REMOVE_LAYER for " << GetObject()->ClassName()
            << " " << ((KVBase *)GetObject())->GetName() << endl; */
         fMDA->RemoveLayer((KVLayer*) GetObject());
         fMDA->UpdateArray();
         break;

      case SET_NUMBER_TELESCOPES_RING:
         /*cout << "SET_NUMBER_TELESCOPES_RING for " << GetObject()->ClassName()
            << " " << ((KVBase *)GetObject())->GetName() << endl; */
         UInt_t num;
         num = (UInt_t)((TGNumberEntry*) GetWidget())->GetIntNumber();
         //cout << "num = " << num << endl;
         if (num)
            ((KVRing*) GetObject())->SetNumberTelescopes(num);
         cout << " *** You should press Update Array next ***" << endl;
         break;

      case SET_THETA_MIN_RING:
         cout << "SET_THETA_MIN_RING for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
         break;

      case SET_THETA_MAX_RING:
         cout << "SET_THETA_MAX_RING for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
         break;

      case SET_PHI_MIN_RING:
         cout << "SET_PHI_MIN_RING for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
         break;

      case SET_PHI_MAX_RING:
         cout << "SET_PHI_MAX_RING for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
         break;

      case ADD_TELESCOPE:
         /*cout << "ADD_TELESCOPE for " << GetObject()->ClassName()
         << " " << ((KVBase *) GetObject())->GetName() << endl;*/
         ((KVRing*) GetObject())->AddTelescope();
         fMDA->UpdateArray();
         break;

      case REMOVE_RING:
         /*cout << "REMOVE_RING for " << GetObject()->ClassName()
         << " " << ((KVBase *) GetObject())->GetName() << endl;*/
         fMDA->RemoveRing((KVRing*) GetObject());
         fMDA->UpdateArray();
         break;

      case LAUNCH_BROWSER_TELESCOPE:
         cout << "LAUNCH_BROWSER_TELESCOPE for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
         break;

      case SET_PHI_MIN_TELESCOPE:
         cout << "SET_PHI_MIN_TELESCOPE for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
         break;

      case SET_PHI_MAX_TELESCOPE:
         cout << "SET_PHI_MAX_TELESCOPE for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
         break;

      case NAMEDIALOG_OK:
         cout << "NAMEDIALOG_OK for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
         //((KVBrowserNameDialog*)GetObject())->CloseWindow();
         break;

      case NAMEDIALOG_CANCEL:
         cout << "NAMEDIALOG_CANCEL for " << GetObject()->ClassName()
              << " " << ((KVBase*) GetObject())->GetName() << endl;
         //((KVBrowserNameDialog*)GetObject())->CloseWindow();
         break;

      default:
         cout << "KVBrowserWidget: Unknown widget or action" << endl;
   }
}
