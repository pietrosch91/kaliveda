/*
$Id: KVIDCutLine.cpp,v 1.2 2009/03/03 13:36:00 franklan Exp $
$Revision: 1.2 $
$Date: 2009/03/03 13:36:00 $
*/

//Created by KVClassFactory on Fri Feb 15 15:00:17 2008
//Author: franklan

#include "KVIDCutLine.h"
#include "Riostream.h"

ClassImp(KVIDCutLine)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDCutLine</h2>
<h4>Line in ID grid used to delimit regions where no identification is possible</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDCutLine::KVIDCutLine() : fAcceptedDirection("")
{
   // Default constructor
}

KVIDCutLine::~KVIDCutLine()
{
   // Destructor
}

//__________________________________________________________

void KVIDCutLine::WriteAsciiFile_extras(ofstream & file, const Char_t * name_prefix)
{
   // Write accepted direction for cut
	
   file << fAcceptedDirection.Data() << endl;
}

//__________________________________________________________

void KVIDCutLine::ReadAsciiFile_extras(ifstream & file)
{
   // Read accepted direction for cut
	
   file >> fAcceptedDirection;
}

