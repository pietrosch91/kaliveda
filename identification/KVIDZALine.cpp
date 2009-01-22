/***************************************************************************
$Id: KVIDZALine.cpp,v 1.4 2007/11/27 12:09:03 ebonnet Exp $
                          KVIDZALine.cpp  -  description
                             -------------------
    begin                : Nov 10 2004
    copyright            : (C) 2004 by J.D. Frankland
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

#include "KVIDZALine.h"
#include "TVector2.h"
#include "Riostream.h"
#include "TMath.h"

ClassImp(KVIDZALine)
/////////////////////////////////////////////////////////////////////////////////////////
//KVIDZALine
//
//Base class for identification lines which determine both Z and A for nuclei.
//Each line is named "Z=x A=y" where x is the atomic number of the corresponding element
//(can be set with SetZ()) and y is the mass number (SetA).
//The sorting function Compare() sorts the lines as a function of increasing Z,
//then increasing A (i.e. p,d,t,3He,4He,6He, etc. etc.)
/////////////////////////////////////////////////////////////////////////////////////////
    KVIDZALine::KVIDZALine()
{
   //Default ctor
   //Mass number = 0 (undefined) by default

   SetA(0);
}

KVIDZALine::~KVIDZALine()
{
   //Default dtor
}

//_____________________________________________________________________________________________

Int_t KVIDZALine::Compare(const TObject * obj) const
{
   //Used to sort lists of KVIDZALines.
   //Sorts the lines as a function of increasing Z, then increasing A
   //(i.e. p,d,t,3He,4He,6He, etc. etc.)

   //First compare Z : only if Z1=Z2 do we need to compare A
   Int_t zComp = KVIDZLine::Compare(obj);
   if (zComp)
      return zComp;

   int aline1 = GetA();
   if (aline1 < 0)
      return 0;
   int aline2 =
       const_cast < KVIDZALine * >(static_cast <
                                   const KVIDZALine * >(obj))->GetA();
   if (aline2 < 0)
      return 0;
   if (aline1 > aline2)
      return 1;
   if (aline1 < aline2)
      return -1;
   return 0;
}

//_____________________________________________________________________________________________

Int_t KVIDZALine::GetA() const
{
   //Returns A of line. This information is held either in the member variable 'fA' (set by SetA)
   //or in the name of the line ("Z=x A=y"). 'fA' is checked first, then the name. If the name is
   //"Z=x A=y", y is returned and fA is set equal to y.
   //If A not set, returns -1.

   if (fA)
      return fA;
   Int_t aline1, zline1;
   if (sscanf(GetName(), "Z=%d A=%d", &zline1, &aline1) != 2)
      return -1;
   const_cast < KVIDZALine * >(this)->SetA(aline1);
   return aline1;
}

//_____________________________________________________________________________________________

void KVIDZALine::SetA(Int_t atnum)
{
   //Set the A of the line. Automatically updates line's name (SetNameLine).

   fA = atnum;
   SetNameLine();
}

//_____________________________________________________________________________________________

void KVIDZALine::SetAandZ(Int_t atnum,Int_t ztnum)
{
   //Set the A and Z of the line. Automatically updates line's name (SetNameLine).
	SetZ(ztnum);
   SetA(atnum);
}

//_____________________________________________________________________________________________

void KVIDZALine::SetNameLine()
{
   //This will set the name of the line according to the Z & A.
   //Name will be "Z=x A=y" where x is the Z of the line and y is the A.

   SetName(Form("Z=%d A=%d", GetZ(), GetA()));
}

//_____________________________________________________________________________________________

void KVIDZALine::WriteAsciiFile(ofstream & file,
                                const Char_t * name_prefix)
{
   //Write name and coordinates of line in file buffer stream
   //Optional string name_prefix will be written just in front of the name of the line.
   //Format is :
   //
   //+classname_of_line
   //name_prefix:name_of_line
   //Z   A
   //number_of_points
   //x1    y1
   //x2    y2
   //...
   //etc. etc.
   file << '+' << ClassName() << endl;
   if (name_prefix)
      file << name_prefix << ":";
   file << GetName() << endl;
   file << GetZ() << "\t" << GetA() << endl;
   file << GetN() << endl;
   for (Int_t i = 0; i < GetN(); i++) {
      Double_t x, y;
      GetPoint(i, x, y);
      file << x << "   " << y << endl;
   }
}

//_____________________________________________________________________________________________

void KVIDZALine::ReadAsciiFile(ifstream & file)
{
   //Read atomic number and coordinates of line in file buffer stream
   //Format is :
   //
   //Z    A
   //number_of_points
   //x1    y1
   //x2    y2
   //...
   //etc. etc.
   Int_t N, Z, A;
   file >> Z >> A;
   SetZ(Z);
   SetA(A);
   file >> N;
   for (Int_t i = 0; i < N; i++) {
      Double_t x, y;
      file >> x >> y;
      SetPoint(i, x, y);
   }
}

//_____________________________________________________________________________________________

void KVIDZALine::Print(Option_t * opt) const
{
   //Print out for line
   //The optional "opt" string, if given, is printed in parentheses after the line's name
   //This is used by KVIDGrid in order to show which lines are "ID" lines and which are
   //"OK" lines (i.e. used to define an identifiable area in a data map).
   cout << ClassName() << " : " << GetName() << "(" << opt << ")" << endl;
   cout << "Z=" << GetZ() << " A=" << GetA() << endl;
   if(GetWidth()>0.0) cout << "Natural Line Width : " << GetWidth() << endl;
   TGraph::Print();
}
