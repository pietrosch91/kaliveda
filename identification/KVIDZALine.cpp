/***************************************************************************
$Id: KVIDZALine.cpp,v 1.5 2009/03/03 13:36:00 franklan Exp $
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
//Base class for identification ridge lines corresponding to different nuclear species.
//
//Each line is named "Z=x A=y" where x is the atomic number of the corresponding element
//(can be set with SetZ()) and y is the mass number (SetA). If only Z is set, the mass number is
//calculated according to one of the mass formulae defined in KVNucleus class. The default is
//the default for KVNucleus; you can change it with SetMassFormula.
//
//The sorting function Compare() sorts the lines as a function of increasing Z,
//then increasing A (i.e. p,d,t,3He,4He,6He, etc. etc.)
/////////////////////////////////////////////////////////////////////////////////////////

KVIDZALine::KVIDZALine()
{
   // Default ctor
   // Line width is set to zero
   // Default identification is set to proton

   SetWidth(0.);
   fLineWithWidth=0;
}

KVIDZALine::~KVIDZALine()
{
   //Default dtor
}

//_____________________________________________________________________________________________

void KVIDZALine::WriteAsciiFile_extras(ofstream & file,
                                const Char_t * name_prefix)
{
	// Write Z & A of line
	
	KVIDLine::WriteAsciiFile_extras(file,name_prefix);
   file << GetZ() << "\t" << GetA() << endl;
}

//_____________________________________________________________________________________________

void KVIDZALine::ReadAsciiFile_extras(ifstream & file)
{
	// Read Z & A of line
	
	KVIDLine::ReadAsciiFile_extras(file);
   Int_t Z, A;
   file >> Z >> A;
   SetZ(Z);
   SetA(A);
}

//_____________________________________________________________________________________________

void KVIDZALine::ReadAsciiFile_KVIDZLine(ifstream & file)
{
   //************ BACKWARDS COMPATIBILITY FIX *************
   //       special read method for old KVIDZLines
   //
   //Read coordinates of line in file buffer stream
   //Format is :
   //Z
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

//_____________________________________________________________________________________________

TGraphErrors* KVIDZALine::GetLineWithWidth()
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

void KVIDZALine::SetAsymWidth(Double_t d_l, Double_t d_r)
{
   //Set the natural width of the line depending on its asymptotic separation
   //a gauche (d_l) and a droite (d_r) from a neighbouring line.
   //This method is used by KVIDGrid::CalculateLineWidths
   //
   //For ordinary Z/(Z,A) lines in a dE-E grid, this is just the minimum of
   //the two asymptotic distances.
   SetWidth( TMath::Min( d_l, d_r ) );
}
