// $Id: ganil2root.cpp,v 1.1.1.1 2003/04/07 10:38:03 ljohn Exp $
// Author: $Author: ljohn $
/***************************************************************************
//                        ganil2root.cpp
//                             -------------------
//    begin                : Thu Jun 14 2001
//    copyright            : (C) 2001 by Garp
//    email                : patois@ganil.fr
//////////////////////////////////////////////////////////////////////////
//
// ganil2root
//
// Convert a tape/file in ganil format ina ROOT tree file.
//
//////////////////////////////////////////////////////////////////////////
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// -------------------------------------------------------------------------
// CVS Log:
// $Log: ganil2root.cpp,v $
// Revision 1.1.1.1  2003/04/07 10:38:03  ljohn
// Added to repository
//
// Revision 1.1  2001/08/21 17:13:03  patois
// Conversion of GANIL format to ROOT file format
//
// -------------------------------------------------------------------------

#include <cstdlib>
#include <iostream>

using std::cout;
using std::endl;

#include <TROOT.h>
#include <THtml.h>

#include "GTGanilData.H"
void usage(void);
void convert2root(const TString sourcename,const TString destname="");

//______________________________________________________________________________
void usage(void)
{
  // Print usage and exit

  cout << "Usage: ganil2root SourceFile"<<endl;
  cout << "   or: ganil2root SourceFile DestFile.root"<<endl;
  exit(0);
}

//______________________________________________________________________________
int main(int argc, char **argv)
{

  // Main switch

  TROOT ganil2root("ganil2root","ganil2root");
  // To generate HTML documentation
  if (argc==2)
  {
    TString filename=argv[1];
    convert2root(filename);
  }
  else if (argc==3)
  {
    TString sourcename=argv[1];
    TString destname  =argv[2];
    convert2root(sourcename,destname);
  }
  else
    usage();
}

//______________________________________________________________________________
void convert2root(const TString sourcename,const TString destname)
{

  // Do the conversion to ROOT tree format

  GTGanilData myData(sourcename);
  myData.SetScalerBuffersManagement(GTGanilData::kAutoWriteScaler);
  myData.Open();
  myData.MakeTree(destname);
}
