/***************************************************************************
$Id: KVIDZLine.cpp,v 1.7 2007/07/23 16:46:32 franklan Exp $
                          KVIDZLine.cpp  -  description
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

#include "KVIDZLine.h"
#include "TVector2.h"
#include "Riostream.h"
#include "TMath.h"

ClassImp(KVIDZLine)
/////////////////////////////////////////////////////////////////////////////////////////
//KVIDZLine
//
//Base class for Z-identification lines such as those used in dE-E telescope maps.
//Each line is named "Z=x" where x is the atomic number of the corresponding element
//(can be set with SetZ()).
//Such lines can be sorted as a function of increasing Z.
/////////////////////////////////////////////////////////////////////////////////////////
    KVIDZLine::KVIDZLine()
{
   //Default ctor
   //Atomic number of line is undefined at this point. Set it with 'SetZ' or by changing name
   //of line to "Z=x" with x>=1.
   //Natural width is set to zero.
   
   SetWidth(0.);
   SetZ(0);
   fLineWithWidth=0;
}

KVIDZLine::~KVIDZLine()
{
   //Default dtor
   SafeDelete(fLineWithWidth);
}

//_____________________________________________________________________________________________

Int_t KVIDZLine::Compare(const TObject * obj) const
{
   //Used to sort lists of KVIDZLines.
   //Lines with smallest Z are first in order.

   int zline1 = GetZ();
   if (zline1 < 0)
      return 0;
   int zline2 =
       const_cast < KVIDZLine * >(static_cast <
                                  const KVIDZLine * >(obj))->GetZ();
   if (zline2 < 0)
      return 0;
   if (zline1 > zline2)
      return 1;
   if (zline1 < zline2)
      return -1;
   return 0;
}

//_____________________________________________________________________________________________

Int_t KVIDZLine::GetZ() const
{
   //Returns Z of line. This information is held either in the member variable 'fZ' (set by SetZ)
   //or in the name of the line ("Z=x"). 'fZ' is checked first, then the name. If the name is
   //"Z=x", x is returned and fZ is set equal to x.
   //If Z not set, returns -1.

   if (fZ)
      return fZ;
   Int_t zline1;
   if (sscanf(GetName(), "Z=%d", &zline1) != 1)
      return -1;
   const_cast < KVIDZLine * >(this)->SetZ(zline1);
   return zline1;
}

//_____________________________________________________________________________________________

void KVIDZLine::SetZ(Int_t atnum)
{
   //Set the atomic number corresponding to the element represented by the line in the dE-E map.
   //Automatically sets name of line to "Z=atnum" using SetNameLine().

   fZ = atnum;
   SetNameLine();
}

//_____________________________________________________________________________________________

void KVIDZLine::SetNameLine()
{
   //This will set the name of the line according to the Z.
   //Name will be "Z=x" where x is the Z of the line.

   SetName(Form("Z=%d", GetZ()));
}

//_____________________________________________________________________________________________

void KVIDZLine::WriteAsciiFile(ofstream & file, const Char_t * name_prefix)
{
   //Write name and coordinates of line in file buffer stream
   //Optional string name_prefix will be written just in front of the name of the line.
   //Format is :
   //
   //+classname_of_line
   //name_prefix:name_of_line
   //atomic_number_of_element_represented_by_line
   //number_of_points
   //x1    y1
   //x2    y2
   //...
   //etc. etc.
   file << '+' << ClassName() << endl;
   if (name_prefix)
      file << name_prefix << ":";
   file << GetName() << endl;
   file << GetZ() << endl;
   file << GetN() << endl;
   for (Int_t i = 0; i < GetN(); i++) {
      Double_t x, y;
      GetPoint(i, x, y);
      file << x << "   " << y << endl;
   }
}

//_____________________________________________________________________________________________

void KVIDZLine::ReadAsciiFile(ifstream & file)
{
   //Read atomic number and coordinates of line in file buffer stream
   //Format is :
   //
   //atomic_number_of_element_represented_by_line
   //number_of_points
   //x1    y1
   //x2    y2
   //...
   //etc. etc.
   Int_t N, Z;
   file >> Z;
   SetZ(Z);
   file >> N;
   for (Int_t i = 0; i < N; i++) {
      Double_t x, y;
      file >> x >> y;
      SetPoint(i, x, y);
   }
}

//_____________________________________________________________________________________________

void KVIDZLine::Print(Option_t * opt) const
{
   //Print out for line
   //The optional "opt" string, if given, is printed in parentheses after the line's name
   //This is used by KVIDGrid in order to show which lines are "ID" lines and which are
   //"OK" lines (i.e. used to define an identifiable area in a data map).
   cout << ClassName() << " : " << GetName() << "(" << opt << ")" << endl;
   cout << "Atomic number : " << GetZ() << endl;
   if(fWidth>0.0) cout << "Natural Line Width : " << GetWidth() << endl;
   TGraph::Print();
}

//_____________________________________________________________________________________________

TGraphErrors* KVIDZLine::GetLineWithWidth()
{
   //Return pointer to TGraphError object which can be used to visualise the natural width
   //of this identification line.
   //If line width = 16000, the error bar is set to 0
   
   if(!fLineWithWidth){
      fLineWithWidth = new TGraphErrors(GetN(), GetX(), GetY());
      fLineWithWidth->SetName( GetName() );
      fLineWithWidth->SetMarkerStyle(kCircle);
      fLineWithWidth->SetMarkerSize(0.8);
      fLineWithWidth->SetLineWidth(2);
      fLineWithWidth->SetEditable(kFALSE);
      for(int i=0; i<GetN(); i++){
         fLineWithWidth->SetPointError(i, 0., (GetWidth() < 16000 ? GetWidth() : 0));
      }
   }
   return fLineWithWidth;
}

//_____________________________________________________________________________________________

void KVIDZLine::SetAsymWidth(Double_t d_l, Double_t d_r)
{
   //Set the natural width of the line depending on its asymptotic separation
   //a gauche (d_l) and a droite (d_r) from a neighbouring line.
   //This method is used by KVIDZGrid::CalculateLineWidths
   //
   //For ordinary Z/(Z,A) lines in a dE-E grid, this is just the minimum of
   //the two asymptotic distances.
   SetWidth( TMath::Min( d_l, d_r ) );
}
