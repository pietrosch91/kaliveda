/***************************************************************************
                          KVIDZGrid.cpp  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZGrid.cpp,v 1.16 2009/03/02 16:48:17 franklan Exp $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "KVIDZGrid.h"
#include "KVIDZLine.h"
#include "TCanvas.h"

ClassImp(KVIDZGrid)
/////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDZGrid</h2>
<h4>Identification grid with lines corresponding to different atomic number (KVIDZLine)</h4>
<h3>Identification quality codes</h3>
Values returned by GetICode():
<ul>
     <li> KVIDZGrid::kICODE0,                   ok</li>
   <li>    KVIDZGrid::kICODE7,                  (x,y) is above largest-Z line in grid; Z attributed to point is a minimum value</li>
  <li>     KVIDZGrid::kICODE8                   no identification: (x,y) out of range covered by grid</li>
</ul>

<h3>Initialisation</h3>
The identification algorithm is based on the concept of "natural width" of the identification lines.
These must be calculated before using the grid, using CalculateLineWidths().
A good place to implement this is in the Initialise() method of the ID telescope
which will use this grid for its identifications.
<!-- */
// --> END_HTML
//
/////////////////////////////////////////////////////////////////////////////
    KVIDZGrid::KVIDZGrid()
{
   //default ctor.
   init();
}


KVIDZGrid::KVIDZGrid(const KVIDZGrid & grid)
{
   //Copy constructor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   grid.Copy(*this);
#else
   ((KVIDZGrid &) grid).Copy(*this);
#endif
}

KVIDZGrid::~KVIDZGrid()
{
   //default dtor.
      if(Dline) delete [] Dline;
      if(Dline2) delete [] Dline2;
      if(ind_list) delete [] ind_list;
      if(ind_list2) delete [] ind_list2;
      if(ind_arr) delete [] ind_arr;
      if(fDistances) delete [] fDistances;
}

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVIDZGrid::Copy(TObject & obj) const
#else
void KVIDZGrid::Copy(TObject & obj)
#endif
{
   //Copy this to 'obj'
   KVIDGrid::Copy(obj);
   ((KVIDZGrid &) obj).SetZmax(const_cast <
                               KVIDZGrid * >(this)->GetZmax());
}

void KVIDZGrid::init()
{
   //private initialisation
   //set Zmax = 0
   fZMax = 0;
   fZMaxLine = 0;
   SetType("KVIDZGrid");
   Dline=0; 
   Dline2=0;          //!working array used by FindNearestIDLine
   ind_list=0;       //!working array used by FindNearestIDLine
   ind_list2=0;       //!working array used by FindNearestIDLine
   ind_arr=0;   //!working array used by FindNearestIDLine
   fClosest=0;          //!closest line to last-identified point
   fDistances=0;      //!distance from point to each of the lines in fLines
}

//_________________________________________________________________________//

void KVIDZGrid::ClearWorkingArrays()
{
   //Initialise working arrays used by FindNearestIDLine
   //When called for the first time, the arrays are allocated using the total
   //number of identification lines defined for the grid, plus one
   
   Int_t dim = (Int_t)NumberOfIDLines() + 1;
   
   //allocate arrays if necessary
   if ( !Dline ) {
      Dline = new Double_t[dim];
      Dline2 = new Double_t[dim];
      ind_list = new Double_t[dim];
      ind_list2 = new Double_t[dim];
      ind_arr = new Int_t[dim];
      fDistances = new Double_t[dim];
   }
   
   //clear working arrays
   for (register int i = 0; i < dim; i++) {
      Dline[i] = 0.0;
      ind_arr[i] = 0;
      ind_list[i] = 0.0;
      Dline2[i] = 0.0;
      ind_list2[i] = 99;
      fDistances[i] = 0.0;
   }
   Lines.Clear();
   fLines.Clear();
   
   fDistanceClosest = -1.;
   fNLines = 0;
   fClosest = 0;
   fIdxClosest = 0;
}

//_________________________________________________________________________//

KVIDZLine *KVIDZGrid::GetZLine(Int_t z, Int_t & index) const
{
   //Returns ID line for which GetZ() returns 'z'. index=index of line found in
   //fIDLines list (-1 if not found).
   //To increase speed, this is done by dichotomy, rather than by looping over
   //all the lines in the list.

   index = -1;
   Int_t nlines = NumberOfIDLines();
   UInt_t idx_min = 0;          //minimum index
   UInt_t idx_max = nlines - 1; // maximum index
   //as a first guess, we suppose that the lines are numbered sequentially from
   //Z=1 (index 0) to Z=zmax=nlines (index nlines-1) with no gaps
   UInt_t idx = ((UInt_t) z - 1 > idx_max ? idx_max : (UInt_t) z - 1);

   while (idx_max > idx_min + 1) {

      KVIDZLine *line = (KVIDZLine *) GetIDLine(idx);
      Int_t zline = line->GetZ();
      //Found it ?
      if (zline == z) {
         index = idx;
         return line;
      }


      if (z > zline) {
         //increase index
         idx_min = idx;
         idx += (Int_t) ((idx_max - idx) / 2 + 0.5);
      } else {
         //decrease index
         idx_max = idx;
         idx -= (Int_t) ((idx - idx_min) / 2 + 0.5);
      }
   }
   //if one of these two lines has the right Z, return its pointer
   KVIDZLine *line = (KVIDZLine *) GetIDLine(idx_min);
   if (line->GetZ() == z) {
      index = idx_min;
      return line;
   }
   line = (KVIDZLine *) GetIDLine(idx_max);
   if (line->GetZ() == z) {
      index = idx_max;
      return line;
   }
   //if not, there is no line in the grid with the right Z
   return 0;
}

//_______________________________________________________________________________________________//
// 
// void KVIDZGrid::Identify(Double_t x, Double_t y,
//                          KVReconstructedNucleus * nuc) const
// {
//    //Set Z of nucleus based on position in grid
//    //Check on viability of identification (IsIdentifiable = kTRUE?)
//    //should be performed separately.
//    //We simply use the Z of the line returned by FindNearestIDLine to call
//    //nuc->SetZ, then distance of point to the two lines above and below in order to
//    //calculate "real Z" i.e. Z with numbers after decimal point
//    //We assume that the 'delta-E' i.e. the y-coordinate is proportional to Z^2 
// 
//    Int_t idx_max, idx_min;
//    KVIDZLine *nl =
//        (KVIDZLine *) FindNearestIDLine(x, y, "above", idx_min, idx_max);
//    nuc->SetZ(nl->GetZ());
//    KVIDZLine *zup = (KVIDZLine *) GetLine("ID", idx_max);
//    KVIDZLine *zlow = (KVIDZLine *) GetLine("ID", idx_min);
//    Int_t dummy = 0;
//    Double_t dist_zup = TMath::Abs(zup->DistanceToLine(x, y, dummy));
//    Double_t dist_zlow = TMath::Abs(zlow->DistanceToLine(x, y, dummy));
//    UInt_t zmax = zup->GetZ();
//    UInt_t zmin = zlow->GetZ();
// 
//    Double_t Zreal =
//        TMath::Sqrt(((Double_t) zmin * zmin) +
//                    (zmax * zmax - zmin * zmin) * dist_zlow / (dist_zlow +
//                                                               dist_zup));
//    nuc->SetRealZ(Zreal);
// }

//_______________________________________________________________________________________________//

void KVIDZGrid::CalculateLineWidths()
{
   //Calculate natural "width" of each line in the grid.
   //The lines in the grid are first sorted so that they are in order of ascending 'Y'
	//The points of each line are sorted so that they are in order of increasing 'X'
   //i.e. first line is 1H, last line is the heaviest isotope (highest line).
   //
   //Then, for a given line :
   //
   //   ****  lines with Z & A (KVIDZALine) **** :
   //      - if the next line (above) has the same Z, we use the separation between these two lines corresponding to different isotopes of the same element
   //      - if the next line (above) has a different Z, but the line before (below) has the same Z, we use the separation between the line below and this one
   //      - if neither adjacent line has the same Z, the width is set to 16000 (huge).
   //
   //  **** lines with Z only (KVIDZLine) **** :
   //      - we use the separation between each pair of lines
   //
   //In each case we find D_L (the separation between the two lines at their extreme left) and  D_R (their separation at extreme right).
   //The width of the line is then calculated from these two using the method KVIDZLine::SetAsymWidth (which may be overridden in child classes).

   //sort list of lines if not already done
   if (!IsSorted())
      SortIDLines();

   for (Int_t i = 0; i < (Int_t) NumberOfIDLines(); i++) {

      KVIDZLine *_line = (KVIDZLine *) GetIDLine(i);
		//sort points in order of increasing 'X'
		_line->Sort( &TGraph::CompareX );
      //check type of lines: with Z & A or only Z ?
      Bool_t ZOnly = !(_line->InheritsFrom("KVIDZALine"));


      //Z of lines above and below this line - Zxx=-1 if there is no line above or below
      Int_t Zhi =
          (i <
           (Int_t) NumberOfIDLines() -
           1 ? ((KVIDZLine *) GetIDLine(i + 1))->GetZ() : -1);
      Int_t Zlo = (i > 0 ? ((KVIDZLine *) GetIDLine(i - 1))->GetZ() : -1);
      Int_t Z = _line->GetZ();

      Int_t i_other;
      if( ZOnly ){
         i_other = (Zhi > -1 ? i + 1 : (Zlo > -1 ? i - 1 : -1));     // index of line used to calculate width
      }
      else
      {
         //for Z&A lines, choice depends on Z of lines
         i_other = (Zhi == Z ? i + 1 : (Zlo == Z ? i - 1 : -1));     // index of line used to calculate width
      }

      //default width of 16000 in case of "orphan" line i.e. Z with only one isotope
      if (i_other < 0) {
         _line->SetWidth(16000.);
         continue;              // skip to next line
      }

      KVIDZLine *_otherline = (KVIDZLine *) GetIDLine(i_other);

      //calculate asymptotic distances between lines at left and right.
      //do this by finding which line's endpoint is between both endpoints of the other line
      Double_t D_L, D_R;

      //***************    LEFT SIDE    ********************************

      //get coordinates of first point of our line
      Double_t x1, y1;
      _line->GetStartPoint(x1, y1);
      KVIDZLine *_aline, *_bline;

      if (_otherline->IsBetweenEndPoints(x1, y1, "x")) {

         //first point of our line is "inside" the X-coords of the other line:
         //asymptotic distance LEFT is distance from our line's starting point (x1,Y1) to the other line
         _aline = _otherline;
         _bline = _line;

      } else {

         //first point of other line is "inside" the X-coords of the our line:
         //asymptotic distance LEFT is distance from other line's starting point (x1,Y1) to our line
         _aline = _line;
         _bline = _otherline;
         _otherline->GetStartPoint(x1, y1);

      }

      //calculate D_L
      Int_t dummy = 0;
      D_L = _aline->DistanceToLine(x1, y1, dummy);

      //make sure that D_L is positive : if not, we try to calculate other way (inverse line and starting point)
      if (D_L < 0.) {

         _aline->GetStartPoint(x1, y1);
         D_L = _bline->DistanceToLine(x1, y1, dummy);

         //still no good ? then print a warning message
         if (D_L < 0.) {
            Warning("CalculateLineWidths",
                    "Cannot get positive asymptotic distance between starting points of the following two lines, in grid %s.\n Will set D_L to zero.",
                    GetName());
            _line->Print();
            _otherline->Print();
            D_L = 0.;
         }
      }
      //***************   RIGHT SIDE    ********************************

      //get coordinates of last point of our line
      _line->GetEndPoint(x1, y1);

      if (_otherline->IsBetweenEndPoints(x1, y1, "x")) {

         //last point of our line is "inside" the X-coords of the other line:
         //asymptotic distance RIGHT is distance from our line's end point (x1,Y1) to the other line
         _aline = _otherline;
         _bline = _line;

      } else {

         //last point of other line is "inside" the X-coords of the our line:
         //asymptotic distance RIGHT is distance from other line's end point (x1,Y1) to our line
         _aline = _line;
         _bline = _otherline;
         _otherline->GetEndPoint(x1, y1);

      }

      //calculate D_R
      D_R = _aline->DistanceToLine(x1, y1, dummy);

      //make sure that D_R is positive : if not, we try to calculate other way (inverse line and end point)
      if (D_R < 0.) {

         _aline->GetEndPoint(x1, y1);
         D_R = _bline->DistanceToLine(x1, y1, dummy);

         //still no good ? then print a warning message
         if (D_R < 0.) {
            Warning("CalculateLineWidths",
                    "Cannot get positive asymptotic distance between end points of the following two lines, in grid %s.\n Will set D_R to zero.",
                    GetName());
            _line->Print();
            _otherline->Print();
            D_R = 0.;
         }
      }
      //***************  SET NATURAL WIDTH OF LINE   ********************************

      _line->SetAsymWidth(D_L, D_R);

   }
}

//_______________________________________________________________________________________________//

void KVIDZGrid::DrawLinesWithWidth()
{
   //This method displays the grid as in KVIDGrid::Draw, but
   //the natural line widths are shown as error bars

   if (!gPad) {
      new TCanvas("c1", GetName());
   } else {
      gPad->SetTitle(GetName());
   }
   if (!gPad->GetListOfPrimitives()->GetSize()) {
      //calculate size of pad necessary to show grid
      if (GetXmin() == GetXmax())
         const_cast < KVIDZGrid * >(this)->FindAxisLimits();
      gPad->DrawFrame(GetXmin(), GetYmin(), GetXmax(), GetYmax());
   }
   TIter nextID( GetIDLines() );
   KVIDZLine* line;
   while ( (line = (KVIDZLine*)nextID()) ){
      line->GetLineWithWidth()->Draw("3PL");
   }
   {
      GetOKLines()->R__FOR_EACH(KVIDLine, Draw) ("PL");
   }
}

//___________________________________________________________________________________

void KVIDZGrid::Initialize()
{
   // General initialisation method for identification grid.
   // This method MUST be called once before using the grid for identifications.
   // The ID lines are sorted.
   // The natural line widths of all ID lines are calculated.
   // The line with the largest Z (Zmax line) is found.
   
   KVIDGrid::Initialize();
   // Zmax should be Z of last line in sorted list
   fZMaxLine = (KVIDZLine *) GetIDLines()->Last();
   if (fZMaxLine) fZMax = fZMaxLine->GetZ();
	else 				fZMax=0;			// protection au cas ou il n y a aucune ligne de Z
}

