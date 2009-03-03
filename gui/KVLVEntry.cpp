/*
$Id: KVLVEntry.cpp,v 1.2 2009/03/03 14:27:15 franklan Exp $
$Revision: 1.2 $
$Date: 2009/03/03 14:27:15 $
*/

//Created by KVClassFactory on Wed Apr  9 13:55:03 2008
//Author: franklan

#include "KVLVEntry.h"
#include "TInterpreter.h"
#include "TGPicture.h"
#include "TGResourcePool.h"

ClassImp(KVLVEntry)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVLVEntry</h2>
<h4>List view item class</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Pixel_t KVLVEntry::fgGreyPixel = 0;
Pixel_t KVLVEntry::fgBGColor = 0;

KVLVEntry::KVLVEntry(TObject* obj, const KVLVContainer *cnt,
			UInt_t ncols, KVLVColumnData **coldata)
			: TGLVEntry(cnt, TString(coldata[0]->GetDataString(obj)),
			TString(obj->ClassName()), 0, kVerticalFrame, GetWhitePixel())
{
   // Default constructor
	
	if( !fgGreyPixel ){
		if( !fClient->GetColorByName("#f0f0f0", fgGreyPixel) ) fgGreyPixel=0;
		fgBGColor = fgWhitePixel;
	}
	fBGColor = -1;
	
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
	
	// to update display of object characteristics when object is changed,
	// the object must have a "Connect" method (for signals-slots) and a "Modified"
	// method which emits the "Modified" signal when object is changed.
	// N.B. the object does not have to inherit from TQObject (can use RQ_OBJECT macro)
	// the "Modified" signal is connected to the KVLVEntry::Refresh method
	if( obj->IsA()->GetMethodAllAny("Modified") && obj->IsA()->GetMethodAllAny("Connect") ){
		gInterpreter->Execute(obj, obj->IsA(), "Connect",
				Form("\"Modified()\",\"KVLVEntry\",(KVLVEntry*)%#x,\"Refresh()\"", (ULong_t)this));
	}
}

void KVLVEntry::Refresh()
{
	// Update the object characteristics and ask for redraw
	
	KVLVContainer *cnt = (KVLVContainer*)GetParent();
	TObject*obj=(TObject*)fUserData;
	SetItemName( cnt->fColData[0]->GetDataString(obj) );
	for(int i=0; fSubnames[i]!=0; i++) fSubnames[i]->SetString( cnt->fColData[i+1]->GetDataString(obj) );
   for (int i = 0; fSubnames[i] != 0; ++i)
      fCtw[i] = gVirtualX->TextWidth(fFontStruct, fSubnames[i]->GetString(),
                                     fSubnames[i]->GetLength());
   SetWindowName();
   TGPosition pos = cnt->GetPagePosition();
   cnt->DrawRegion(GetX() - pos.fX, GetY() - pos.fY, GetWidth(), GetHeight());
}

//______________________________________________________________________________

void KVLVEntry::DrawCopy(Handle_t id, Int_t x, Int_t y)
{
   // Draw list view item in other window.
   // List view item is placed and layout in the container frame,
   // but is drawn in viewport.
	//
	// This is a line for line copy of TGLVEntry::DrawCopy from ROOT v5.22/00,
	// but we alternate the background colour between white and light grey, in
	// order to make the list easier to read.

	KVLVContainer *cnt = (KVLVContainer*)GetParent();
	if(fBGColor == -1 || cnt->IsBeingSorted()){
		fBGColor = fgBGColor;
		if( fgBGColor == fgWhitePixel ) fgBGColor = fgGreyPixel;
		else
			fgBGColor = fgWhitePixel;
	}
	
   Int_t ix, iy, lx, ly;
   Int_t max_ascent, max_descent;

   gVirtualX->GetFontProperties(fFontStruct, max_ascent, max_descent);
   fTWidth = gVirtualX->TextWidth(fFontStruct, fItemName->GetString(), fItemName->GetLength());
   fTHeight = max_ascent + max_descent;

   if (fViewMode == kLVLargeIcons) {
      ix = (fWidth - fCurrent->GetWidth()) >> 1;
      iy = 0;
      lx = (fWidth - fTWidth) >> 1;
      ly = fHeight - (fTHeight + 1) - 2;
   } else {
      ix = 0;
      iy = (fHeight - fCurrent->GetHeight()) >> 1;
      lx = fCurrent->GetWidth() + 2;
      ly = (fHeight - (fTHeight + 1)) >> 1;
   }

   if ((fChecked) && (fCheckMark)) {
      if (fViewMode == kLVLargeIcons) {
         fCheckMark->Draw(id, fNormGC, x + ix + 8, y + iy + 8);
         gVirtualX->SetForeground(fNormGC, fgWhitePixel);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fTWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fgBlackPixel);
      }
      else {
         fCheckMark->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgWhitePixel);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fTWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fgBlackPixel);
      }
   }
   // This if tries to print the elements with ... appened at the end if
   // the widht of the string is longer than that of the column
   if (fViewMode == kLVDetails && fSubnames && fCpos && fJmode && fCtw) {
      TString tmpString = *fItemName;
      Int_t ftmpWidth = gVirtualX->TextWidth(fFontStruct, tmpString,
                                             tmpString.Length());
      if ( ftmpWidth > (fCpos[0] - lx) ) {
         for (Int_t j = fItemName->Length() - 1 ; j > 0; j--) {
            tmpString =  (*fItemName)(0,j) + "...";
            ftmpWidth = gVirtualX->TextWidth(GetDefaultFontStruct(), tmpString,
                                             tmpString.Length());
            if ( ftmpWidth <= (fCpos[0] - lx) ) {
               break;
            }
         }
      }
      if (fActive) {
         if (fSelPic) fSelPic->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgDefaultSelectedBackground);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fCpos[0] - lx, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fClient->GetResourcePool()->GetSelectedFgndColor());
      } else {
			// inactive list items are drawn with alternating background colours: white, grey, white, grey, ...
         fCurrent->Draw(id, fNormGC, x + ix, y + iy);
         //gVirtualX->SetForeground(fNormGC, fgWhitePixel);
         gVirtualX->SetForeground(fNormGC, fBGColor);// use current background colour
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fCpos[0] - lx, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fgBlackPixel);
      }

      TGString tmpTGString(tmpString);
      tmpTGString.Draw(id, fNormGC, x+lx, y+ly + max_ascent);
   } else {
      if (fActive) {
         if (fSelPic) fSelPic->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgDefaultSelectedBackground);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fTWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fClient->GetResourcePool()->GetSelectedFgndColor());
      } else {
         fCurrent->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgWhitePixel);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fTWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fgBlackPixel);
      }
      fItemName->Draw(id, fNormGC, x+lx, y+ly + max_ascent);
   }
   gVirtualX->SetForeground(fNormGC, fgBlackPixel);

   if (fViewMode == kLVDetails) {
      if (fSubnames && fCpos && fJmode && fCtw) {
         int i;

         // Again fixes the size of the strings
         for (i = 0; fSubnames[i] != 0; ++i) {
            TString tmpString = *fSubnames[i];
            Int_t ftmpWidth = gVirtualX->TextWidth(fFontStruct, tmpString,
                                                   tmpString.Length());
            if ( ftmpWidth > (fCpos[i+1] - fCpos[i]) ) {
               for (int j = fSubnames[i]->Length() - 1 ; j > 0; j--) {
                  tmpString =  (*fSubnames[i])(0,j) + "...";
                  ftmpWidth = gVirtualX->TextWidth(GetDefaultFontStruct(),
                                                   tmpString,
                                                   tmpString.Length());
                  if ( ftmpWidth <= (fCpos[i+1] - fCpos[i]) ) {
                     break;
                  }
               }
            }
            if (fCpos[i] == 0)
               break;
            if (fJmode[i] == kTextRight)
               lx = fCpos[i+1] - ftmpWidth - 2;
            else if (fJmode[i] == kTextCenterX)
               lx = (fCpos[i] + fCpos[i+1] - ftmpWidth) >> 1;
            else // default to TEXT_LEFT
               lx = fCpos[i] + 2;

            //if (x + lx < 0) continue; // out of left boundary or mess in name
      if (fActive) {
         //if (fSelPic) fSelPic->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgDefaultSelectedBackground);
         gVirtualX->FillRectangle(id, fNormGC, x+ fCpos[i] , y + ly, fCpos[i+1]-fCpos[i], fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fClient->GetResourcePool()->GetSelectedFgndColor());
      } else {
			// inactive list items are drawn with alternating background colours: white, grey, white, grey, ...
         //fCurrent->Draw(id, fNormGC, x + ix, y + iy);
         //gVirtualX->SetForeground(fNormGC, fgWhitePixel);
         gVirtualX->SetForeground(fNormGC, fBGColor);// use current background colour
         gVirtualX->FillRectangle(id, fNormGC, x + fCpos[i], y + ly, fCpos[i+1]-fCpos[i], fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fgBlackPixel);
      }
            TGString tmpTGString(tmpString);
            tmpTGString.Draw(id, fNormGC, x + lx, y + ly + max_ascent);
         }
      }
   }
	
}
