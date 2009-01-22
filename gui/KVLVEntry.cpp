/*
$Id: KVLVEntry.cpp,v 1.1 2008/04/10 15:43:45 franklan Exp $
$Revision: 1.1 $
$Date: 2008/04/10 15:43:45 $
*/

//Created by KVClassFactory on Wed Apr  9 13:55:03 2008
//Author: franklan

#include "KVLVEntry.h"

ClassImp(KVLVEntry)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVLVEntry</h2>
<h4>List view item class</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVLVEntry::KVLVEntry(TObject* obj, const KVLVContainer *cnt,
			UInt_t ncols, KVLVColumnData **coldata)
			: TGLVEntry(cnt, TString(coldata[0]->GetDataString(obj)),
			TString(obj->ClassName()), 0, kVerticalFrame, GetWhitePixel())
{
   // Default constructor
	fUserData = obj;
	fSubnames = new TGString* [ncols];
	for(int i=0; i<(int)ncols-1; i++) fSubnames[i] = new TGString( coldata[i+1]->GetDataString(obj) );
	fSubnames[ncols-1] = 0;
	int j;
   for (j = 0; fSubnames[j] != 0; ++j)
      ;
   fCtw = new int[j+1];
   fCtw[j] = 0;
   for (int i = 0; fSubnames[i] != 0; ++i)
      fCtw[i] = gVirtualX->TextWidth(fFontStruct, fSubnames[i]->GetString(),
                                     fSubnames[i]->GetLength());

   SetWindowName();
}

