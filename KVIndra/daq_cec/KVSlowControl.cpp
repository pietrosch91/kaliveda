/***************************************************************************
                          KVSlowControl.cpp  -  description
                             -------------------
    begin                : Wed Apr 2 2003
    copyright            : (C) 2003 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVSlowControl.h"

ClassImp(KVSlowControl)

////////////////////////////////////////////////////////////////////////////
//KVSlowControl
//
//Gives access to slow control data
////////////////////////////////////////////////////////////////////////////

KVSlowControl::KVSlowControl()
{
   //default constructor
   fFile = 0;
   fTree = 0;
   fBase = 0;
}

void KVSlowControl::Init(const Char_t* filename, const Char_t* treename)
{
   //open the ROOT file and set up a pointer to the TTree it contains
   fFile = new TFile(filename);
   fTree = (TTree*)ffile->Get(treename);
}

void KVSlowControl::SetRun(UInt_t run)
{
   //fill SLC database from tree for this run
   fBase = new KVSLCBase();
   UInt_t fRun;
   fTree->SetBranchAddress("dB", &fBase);
   fTree->SetBranchAddress("Run", &fRun);
   for (UInt_t i = 0; i < fTree->GetEntries(); i++) {
      fTree->GetEntry(i);
      if (fRun == run) break;
   }
}
KVSlowControl::~KVSlowControl()
{
   if (fTree) delete fTree;
   if (fFile) delete fFile;
   if (fBase) delete fBase;
}
