// $Id: Example.cpp,v 1.1.1.1 2003/04/07 10:38:02 ljohn Exp $
// Author: $Author: ljohn $
/***************************************************************************
//                        Example.cpp  -  Example code for ROOTGanilTape
//                             -------------------
//    begin                : Thu Jun 14 2001
//    copyright            : (C) 2001 by Garp
//    email                : patois@ganil.fr
//////////////////////////////////////////////////////////////////////////
//
// Example
//
// Gives a short example of usage of ROOTganilTape package.
//
//////////////////////////////////////////////////////////////////////////
// Remeber to change the license for this file:
// GPL for an example program is not very pertinant
// I shoudl switch to a BSD-like license. 
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
// $Log: Example.cpp,v $
// Revision 1.1.1.1  2003/04/07 10:38:02  ljohn
// Added to repository
//
// Revision 1.10  2001/08/21 17:23:29  patois
// AutoDoc generation for ganil2root added
//
// Revision 1.9  2001/08/21 17:12:12  patois
// details
//
// Revision 1.8  2001/07/07 19:19:40  patois
// details
//
// Revision 1.7  2001/07/04 09:43:32  patois
// Scalers integration in progress
//
// -------------------------------------------------------------------------

#include <iostream>

using std::cout;
using std::endl;

#include <TROOT.h>
#include <THtml.h>

#include "GTGanilData.H"

//______________________________________________________________________________
// This is a sample analysis class (that dont do much ;)
//______________________________________________________________________________
class MYAnalysisClass
{
public:
  void Connect(GTGanilData &ganilData)
  {
    // This method connect class members to the corresponding parameters
    // in ganilData.
  }
  void Process(void)
  {
    // Just a dummy routine
    return;
  }

private:
};

//______________________________________________________________________________
int main(int argc, char **argv)
{
  TROOT Example("Example","Example");
  // To generate HTML documentation
  if (argc==2)
    if (!strcmp(argv[1],"HTML"))
    {
      THtml html;
      gHtml->MakeAll(kFALSE,"GT*");
      gHtml->Convert("src/Example.cpp","Example of ROOTGanilTape programm");
      gHtml->Convert("src/ganil2root.cpp","Convert GANIL tapes/files to ROOT formated files.");
      return(0);
    }
  
  GTGanilData myData(argv[1]);    // Analysing file in current directory.
  myData.SetScalerBuffersManagement(GTGanilData::kAutoWriteScaler); // Report scaler buffers
  myData.Open();                 // Open the file/tape
  myData.PrintRunParameters();   // Print class parameters on cout
  myData.DumpParameterName ();   // Print parameters on cout
  
/*  MYAnalysisClass myAnalysis;    // Create an instance of the analysis class
  myAnalysis.Connect(myData);    // Connect it to the data stream.
  */
  myData.Next();                 // Read first event
  myData.DumpEvent();            // Dump out the parameters values
  
  while (myData.Next())          // Read nexts events until EOF
  {
    if (myData.IsScalerBuffer())
    {
      cout << "Got a Scaler event!"<<endl;
      const UShort_t *scalbrut=myData.GetBrutScalerBuffer(); // Get the brut scaler buffer
      // Do something with it.
    }
/*    else
      myAnalysis.Process();        // Process current event
*/      
  }
 
}
