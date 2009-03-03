/***************************************************************************
                          KVIDGridManager.cpp  -  description
                             -------------------
    begin                : Jan 27 2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDGridManager.cpp,v 1.12 2009/03/03 13:36:00 franklan Exp $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVIDGridManager.h"
#include "Riostream.h"
#include "KVString.h"
#include "TClass.h"
#include "TROOT.h"
#include "KVIDGridManagerGUI.h"

ClassImp(KVIDGridManager)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KVIDGridManager
//
//Handles a stock of identification grids to be used by one or more identification telescopes.
//Each KVIDTelescope asks the grid manager for a grid (FindGrid), if one is available its
//address is given to the telescope to use. In this way many IDTelescopes can use the same
//grid without making multiple copies.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
KVIDGridManager *gIDGridManager;

KVIDGridManager::KVIDGridManager()
{
   //Default constructor
   //Initialise global pointer gIDGridManager
   //Create list for ID grids;
   gIDGridManager = this;
   fGrids = new KVList;
}

KVIDGridManager::~KVIDGridManager()
{
   //Destructor
   //Reset global pointer gIDGridManager
   //Delete list of ID grids (this deletes all the grids)
   if (gIDGridManager == this)
      gIDGridManager = 0;
   delete fGrids;
}

void KVIDGridManager::FindGrid(KVIDTelescope * idt)
{
   //Find grid for the identification telescope.
   //In fact, we loop over all grids in the collection and give their address to the telescope.
   //It is the telescope which decides whether the grid is right or not (KVIDTelescope::SetGrid).

   KVIDGraph *grid;
   TIter next(GetGrids());
   while ((grid = (KVIDGraph *) next())) {
      //SetGrid returns true if the grid is accepted by the telescope
      if (idt->SetIDGrid(grid))
         break;
   }
}

void KVIDGridManager::AddGrid(KVIDGraph * grid)
{
   //Add a grid to the collection
   GetGrids()->Add(grid);
   Modified();                  // emit signal to say something changed
}

void KVIDGridManager::DeleteGrid(KVIDGraph * grid, Bool_t update)
{
   //Remove grid from manager's list and delete it
   //update flag allows to diable the emission of the 'Modified' signal in case the GUI
   //is deleting a list of grids - in this case we don't want to update until the end
   GetGrids()->Remove(grid);
   delete grid;
   if (update)
      Modified();               // emit signal to say something changed
}

void KVIDGridManager::Clear(Option_t * opt)
{
   //Delete all grids and empty list, ready to start anew
   fGrids->Clear();
   Modified();                  // emit signal to say something changed
}

Bool_t KVIDGridManager::ReadAsciiFile(const Char_t * filename)
{
   //read file, create grids corresponding to information in file.
   //note: any existing grids are not destroyed, use Clear() beforehand if you want to
   //start afresh and anew (ais athat aOK?)

   Bool_t is_it_ok = kFALSE;
   ifstream gridfile(filename);
   if (!gridfile.good()) {
      Error("ReadAsciiFile", "File %s cannot be opened", filename);
      return is_it_ok;
   }

   KVString s;

   while (gridfile.good()) {
      //read a line
      s.ReadLine(gridfile);
      if (s.BeginsWith("++")) {
         //New grid
         //Get name of class by stripping off the '+' at the start of the line
         s.Remove(0, 2);
         /************ BACKWARDS COMPATIBILITY FIX *************
           Old grid files may contain obsolete KVIDZGrid class
           We replace by KVIDZAGrid with SetOnlyZId(kTRUE)
         */
         //Make new grid using this class
         KVIDGraph *grid = 0; Bool_t onlyz=kFALSE;
         if(s=="KVIDZGrid"){
            s="KVIDZAGrid";
            onlyz=kTRUE;
         }
         TClass *clas = TClass::GetClass(s.Data());
         grid = (KVIDGraph *) clas->New();
         //add to ID Grid manager
         gIDGridManager->AddGrid(grid);
         //read grid
         grid->ReadFromAsciiFile(gridfile);
         if(onlyz) grid->SetOnlyZId(kTRUE);
      }
   }

   gridfile.close();
   is_it_ok = kTRUE;
   Modified();                  // emit signal to say something changed
   return is_it_ok;
}

Bool_t KVIDGridManager::WriteAsciiFile(const Char_t * filename)
{
   //write file containing all grids currently in manager

   Bool_t is_it_ok = kFALSE;
   ofstream gridfile(filename);

   TIter next(fGrids);
   KVIDGraph *grid = 0;
   while ((grid = (KVIDGraph *) next())) {

      grid->WriteToAsciiFile(gridfile);

   }

   gridfile.close();
   is_it_ok = kTRUE;
   return is_it_ok;
}

KVIDGraph *KVIDGridManager::GetGrid(const Char_t * name)
{
   //Return pointer to grid with name "name"
   return (KVIDGraph *) GetGrids()->FindObjectByName(name);
}

void KVIDGridManager::StartViewer() const
{
   //Opens GUI for managing grids
   new KVIDGridManagerGUI;
}
