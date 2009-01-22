/***************************************************************************
                          KVIDGridManager.h  -  description
                             -------------------
    begin                : Jan 27 2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDGridManager.h,v 1.9 2006/10/19 14:32:43 franklan Exp $
***************************************************************************/

#ifndef KVIDGridManager_H
#define KVIDGridManager_H

#include "KVList.h"
#include "KVIDGrid.h"
#include "KVIDTelescope.h"
#include "RQ_OBJECT.h"

class KVIDGridManagerGUI;

class KVIDGridManager:public KVBase {

   RQ_OBJECT("KVIDGridManager")

 private:

   KVList * fGrids;             //collection of all ID grids handled by manager

 protected:

 public:

   KVIDGridManager();
   virtual ~ KVIDGridManager();

   void FindGrid(KVIDTelescope *);
   void AddGrid(KVIDGrid *);
   void DeleteGrid(KVIDGrid *, Bool_t update = kTRUE);
   KVIDGrid *GetGrid(const Char_t * name);

   KVList *GetGrids() {
      return fGrids;
   };

   void Clear(Option_t * opt = "");
   Bool_t ReadAsciiFile(const Char_t * filename);
   Bool_t WriteAsciiFile(const Char_t * filename);

   void Print(Option_t * opt = "") const {
      ls();
   };
   void ls(Option_t * opt = "") const {
      fGrids->ls();
   };

   void StartViewer() const;

   void Modified() {
      Emit("Modified()");
   };                           // *SIGNAL*

   ClassDef(KVIDGridManager, 0) //Handles a collection of identification grids
};

//................  global variable
R__EXTERN KVIDGridManager *gIDGridManager;

#endif
