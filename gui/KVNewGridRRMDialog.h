/*
$Id: KVNewGridRRMDialog.h,v 1.3 2006/10/19 14:32:43 franklan Exp $
$Revision: 1.3 $
$Date: 2006/10/19 14:32:43 $
$Author: franklan $
*/

#ifndef __KVNEWGRRMDIA_H
#define __KVNEWGRRMDIA_H

#include "KVNewGridDialog.h"
#include "KVINDRARRMValidator.h"
#include "TGTextEntry.h"

class KVNewGridRRMDialog:public KVNewGridDialog {

   RQ_OBJECT("KVNewGridRRMDialog")

 protected:
   KVINDRARRMValidator * fRRMGrid;
   TGTextEntry *fRunList;
   TGTextEntry *fRingList;
   TGTextEntry *fModList;
   TGHorizontalFrame *fHFRuns;
   TGHorizontalFrame *fHFRings;
   TGHorizontalFrame *fHFModules;
   TGHorizontalFrame *fHFScales;
   TString fRunListString;
   TString fRingListString;
   TString fModListString;

   virtual void set_selected_grid(KVIDGrid * g);
   virtual void layout_parameter_control_widgets();

 public:

    KVNewGridRRMDialog();
    KVNewGridRRMDialog(const TGWindow * p, const TGWindow * main,
                       UInt_t w = 1, UInt_t h = 1, Option_t * type =
                       "New", KVIDGrid * g = 0);
    virtual ~ KVNewGridRRMDialog() {
   };

   virtual void SelectRuns(Bool_t);
   virtual void ReadRuns();
   virtual void SelectRings(Bool_t);
   virtual void ReadRings();
   virtual void SelectMods(Bool_t);
   virtual void ReadMods();
   virtual void ReadAndSetSelectedGridProperties(KVIDGrid *);

   ClassDef(KVNewGridRRMDialog, 0)      //Dialog box for creation of new grid in KVIDGridManagerGUI, for grids which inherit from KVINDRARRMValidator
};
#endif
