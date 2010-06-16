/***************************************************************************
$Id: KVDBTable.cpp,v 1.18 2007/04/27 14:46:29 franklan Exp $
                          KVTable.cpp  -  description
                             -------------------
    begin                : jeu fév 6 2003
    copyright            : (C) 2003 by Alexis Mignon
    email                : mignon@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "KVDBTable.h"
#include "KVDBRecord.h"
#include "Riostream.h"
#include "TROOT.h"


ClassImp(KVDBTable)
///////////////////////////////////////////////
//Base class for a table of the data base
//It is a TFolder and contains a list of KVDBRecord records.
//The folder owns this list.
//
//
//__________________________________________________________________
    KVDBTable::KVDBTable()
{
   fIsUnique = kFALSE;
   SetOwner(kTRUE);
}

//__________________________________________________________________

KVDBTable::KVDBTable(const Char_t * name, const Char_t * type,
                     Bool_t unique)
:TFolder(name, type)
{
   fIsUnique = unique;
   SetOwner(kTRUE);
}

//__________________________________________________________________

KVDBTable::~KVDBTable()
{
	gROOT->GetListOfCleanups()->Remove(this);
	Info("~KVDBTable", "%s", GetName());
}

//___________________________________________________________________

Bool_t KVDBTable::AddRecord(KVDBRecord * rec)
{
   //Add a KVDBRecord to the list of available records and return kTRUE

   Add(rec);
   rec->SetTable(this);
   return kTRUE;
}

//___________________________________________________________________

void KVDBTable::RemoveRecord(KVDBRecord * rec)
{
   //Remove a KVDBRecord from the list of available records

   Remove(rec);
   rec->SetTable(0);
}

//___________________________________________________________________________________//

void KVDBTable::ls(Option_t * opt) const
{
   cout << ClassName() << " : " << GetName() << " <---> " << GetTitle() <<
       endl;
}

//___________________________________________________________________________________//

TObject *KVDBTable::FindObject(const Char_t * name) const
{
   //Redefinition of TFolder::FindObject, which doesn't seem to work correctly with names that
   //have spaces in them (???)
   //call FindObject method for each record in table

   return GetListOfFolders()->FindObject(name);
}

//___________________________________________________________________________________//

KVDBRecord *KVDBTable::GetRecord(Int_t num) const
{
   //Search for record using its number.
   //To keep things simple, we only search in the top level of the folder structure.
   TIter next(GetListOfFolders());
   KVDBRecord *obj = 0;
   while ((obj = (KVDBRecord *) next()) && (obj->GetNumber() != num));
   return (obj ? (obj->GetNumber() == num ? obj : 0) : 0);
}
