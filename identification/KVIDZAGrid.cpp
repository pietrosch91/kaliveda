/***************************************************************************
                          KVIDZAGrid.cpp  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZAGrid.cpp,v 1.10 2008/10/13 13:52:29 franklan Exp $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "KVIDZAGrid.h"
#include "KVIDZALine.h"

ClassImp(KVIDZAGrid)
/////////////////////////////////////////////////////////////////////////////
//KVIDZAGrid
//
/////////////////////////////////////////////////////////////////////////////
    KVIDZAGrid::KVIDZAGrid()
{
   //default ctor.
   init();
}

KVIDZAGrid::~KVIDZAGrid()
{
   //default dtor.
      if(Dline) delete [] Dline;
      if(Dline2) delete [] Dline2;
      if(ind_list) delete [] ind_list;
      if(ind_list2) delete [] ind_list2;
      if(ind_arr) delete [] ind_arr;
      if(fDistances) delete [] fDistances;
}

KVIDZAGrid::KVIDZAGrid(const KVIDZAGrid & grid)
{
   //Copy constructor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   grid.Copy(*this);
#else
   ((KVIDZAGrid &) grid).Copy(*this);
#endif
}

//_________________________________________________________________________//

void KVIDZAGrid::init()
{
   //initialisation
   SetType("KVIDZAGrid");
   Dline=0; 
   Dline2=0;          //!working array used by FindNearestIDLine
   ind_list=0;       //!working array used by FindNearestIDLine
   ind_list2=0;       //!working array used by FindNearestIDLine
   ind_arr=0;   //!working array used by FindNearestIDLine
   fClosest=0;          //!closest line to last-identified point
   fDistances=0;      //!distance from point to each of the lines in fLines
}

//_________________________________________________________________________//

void KVIDZAGrid::ClearWorkingArrays()
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

KVIDZALine *KVIDZAGrid::GetZALine(Int_t z, Int_t a, Int_t & index) const
{
   //Returns ID line corresponding to nucleus (Z,A) and its index in fIDLines.
   //First we use KVIDZGrid::GetLine(z) to find a line with the right Z (in
   //principal there are several in the grid), then search for the correct
   //isotope among these.

   Int_t idx;
   index = -1;
   KVIDZALine *line = (KVIDZALine *) GetZLine(z, idx);
   //no line with correct Z ?
   if (!line)
      return 0;
   //got right mass?
   if (line->GetA() == a) {
      index = idx;
      return line;
   }
   //increase/decreas index depending on if mass of line is greater than or less than a
   if (a > line->GetA()) {
      for (int i = idx; i < (int) NumberOfIDLines(); i++) {
         line = (KVIDZALine *) GetIDLine(i);
         if (line->GetZ() != z)
            return 0;           //no longer correct Z
         if (line->GetA() == a) {
            index = i;
            return line;
         }
      }
      //gone to the end of the list and still not found it
      return 0;
   } else if (a < line->GetA()) {
      for (int i = idx; i > 0; i--) {
         line = (KVIDZALine *) GetIDLine(i);
         if (line->GetZ() != z)
            return 0;           //no longer correct Z
         if (line->GetA() == a) {
            index = i;
            return line;
         }
      }
      //got to start of list and still not found
      return 0;
   }
   return 0;
}

//_______________________________________________________________________________________________//

KVIDLine *KVIDZAGrid::FindNearestIDLine(Double_t x, Double_t y,
                                      const Char_t * position,
                                      Int_t & idx_min,
                                      Int_t & idx_max) const
{
   //See GetNearestIDLine
   //This is just a wrapper because of "const" attribute of base class method

   return (const_cast <KVIDZAGrid*>(this)->GetNearestIDLine(x, y, position, idx_min,
                                                idx_max));
}

//______________________________________________________________________________________________//

KVIDLine *KVIDZAGrid::GetNearestIDLine(Double_t x, Double_t y,
                                     const Char_t * position,
                                     Int_t & idx_min, Int_t & idx_max)
{
   //For a point (x,y) in the grid, we look for the closest lines to the point,
   //only considering lines for whom x1 <= x <= x2, where x1 and x2 are the x-coordinates of
   //the leftmost (first) and rightmost (last) points of the line.
   //After this search is performed, the closest lines and their distances from the point
   //are accessible using GetClosestLines and GetDistanceToLine.
   //Call IdentZA after using this function in order to obtain Z, A and real A.

   idx_min = idx_max = -1;

   //We work with a subset of ID lines for which the X-coord of the point to
   //be identified is inbetween the minimum and maximum X-coords of the line.
   Int_t nlines;
   TList *idlines = GetIDLinesEmbracingPoint("x", x, y, nlines);

   //not a single line found to identify the point!!
   if (!nlines) {
      delete idlines;
      return 0;
   }
   
   //Dline[] and Lines[] : distances to and pointers of lines
   //ind_list[] : index of line in complete list of all ID lines
   ClearWorkingArrays();        // (re)initialise arrays

   //Info("GetNearestIDLine","Lines embracing point:");
      
   if (nlines > 1) {
      //loop over lines, filling in the arrays
      TIter next(idlines);
      KVIDLine *line;
      Int_t idx = 0;
      while ((line = (KVIDLine *) next())) {

         //cout << line->GetName() << endl;
         
         Int_t dummy;
         Double_t dist = TMath::Abs(line->DistanceToLine(x, y, dummy));
         Dline[idx] = dist;
         Lines.AddAtAndExpand(line, idx);
         ind_list[idx] = (Double_t) GetIDLines()->IndexOf(line);
         idx++;
      }
   } else {
      Int_t dummy;
      KVIDLine *line = (KVIDLine *) idlines->At(0);
         
      //cout << line->GetName() << endl;
      
      Double_t dist = TMath::Abs(line->DistanceToLine(x, y, dummy));
      Dline[0] = dist;
      Lines.AddAtAndExpand(line, 0);
      ind_list[0] = (Double_t) GetIDLines()->IndexOf(line);
   }
   if (nlines > 1) {
      //sort in order of increasing distance from point
      TMath::BubbleLow(nlines, Dline, ind_arr);
   } else {
      ind_arr[0] = 0;
   }

   //keep pointer to closest line
   fClosest = (KVIDLine*)Lines[ind_arr[0]];
   //index of closest line
   Int_t idx_closest = (Int_t) ind_list[ind_arr[0]];
   delete idlines;
   //copy closest lines' indexes into ind_list2, their distances from point into Dline2
   fNLines = nlines;
   for (register int i = 0; i < fNLines; i++) {
      Dline2[i] = Dline[ind_arr[i]];
      ind_list2[i] = ind_list[ind_arr[i]];
   }
   if (fNLines > 1) {
      //sort fNLines closest lines in order of increasing index in main list of IDLines
      // i.e. in order of increasing Y-coordinate
      TMath::BubbleLow(fNLines, ind_list2, ind_arr);
   } else {
      ind_arr[0] = 0;
   }

      //Info("GetNearestIDLine","Nearest lines to point, from bottom to top:");
      
   for (register int i = 0; i < fNLines; i++) {
      Int_t idx = (Int_t) ind_list2[ind_arr[i]];
      if (idx == idx_closest){
         fIdxClosest = i;
         //cout << "CLOSEST : " ;
      }
      if (idx < 99) {
         fLines.AddAtAndExpand(GetIDLine(idx), i);
         fDistances[i] = Dline2[ind_arr[i]];
         //cout << i << " : " << fLines[i]->GetName() << "   d= " << fDistances[i] << endl;
      } else {
         fLines.AddAtAndExpand(0, i);
         fDistances[i] = -99.;
      }
   }

   return fClosest;
}

//_________________________________________________________________________//

void KVIDZAGrid::IdentZA(Double_t x, Double_t y, Int_t & Z, Double_t & A)
{
   //Finds Z, A and 'real A' for point (x,y) once closest lines to point have been found.
   //This is a line-for-line copy of the latter part of IdnCsOr, even the same
   //variable names and comments have been used (as much as possible).

   //Work out kinfi, kinf, ksup and ksups like in IdnCsIOr
   Int_t kinfi, kinf, ksup, ksups;
   kinfi = kinf = ksup = ksups = -1;
   fIcode = kICODE0;
   if (GetClosestLine()->WhereAmI(x, y, "above")) {
      //point is above closest line, closest line is "kinf"
      //Info("IdentZA","point is above closest line, closest line is kinf");
      kinf = GetIndexClosest();
      ksup = (kinf + 1 < GetNClosestLines()? kinf + 1 : -1);
   } else {
      //point is below closest line, closest line is "ksup"
      //Info("IdentZA","point is below closest line, closest line is ksup");
      ksup = GetIndexClosest();
      kinf = TMath::Max(ksup - 1, -1);
   }
   kinfi = TMath::Max(kinf - 1, -1);
   //look for line above ksup, if ksup exists (> -1)
   if(ksup > -1) ksups = (ksup + 1 < GetNClosestLines()? ksup + 1 : -1);
   Double_t dinf, dsup, dinfi, dsups;
   dinf = dsup = dinfi = dsups = 0.;
   Double_t winf, wsup, winfi, wsups;
   winf = wsup = winfi = wsups = 0.;
   Int_t Zinfi, Zinf, Zsup, Zsups, Ainfi, Ainf, Asup, Asups;
   Zinfi = Zinf = Zsup = Zsups = Ainfi = Ainf = Asup = Asups = 0;
   if (kinf > -1) {
         Zinf = ((KVIDZLine *) GetClosestLines(kinf))->GetZ();
         Ainf = ((KVIDZALine *) GetClosestLines(kinf))->GetA();
         winf = ((KVIDZLine *) GetClosestLines(kinf))->GetWidth();
         dinf = GetDistanceToLine(kinf);
   }
   if (ksup > -1) {
         Zsup = ((KVIDZLine *) GetClosestLines(ksup))->GetZ();
         Asup = ((KVIDZALine *) GetClosestLines(ksup))->GetA();
         wsup = ((KVIDZLine *) GetClosestLines(ksup))->GetWidth();
      dsup = GetDistanceToLine(ksup);
   }
   if (kinfi > -1) {
         Zinfi = ((KVIDZLine *) GetClosestLines(kinfi))->GetZ();
         Ainfi = ((KVIDZALine *) GetClosestLines(kinfi))->GetA();
         winfi = ((KVIDZLine *) GetClosestLines(kinfi))->GetWidth();
      dinfi = GetDistanceToLine(kinfi);
   }
   if (ksups > -1) {
         Zsups = ((KVIDZLine *) GetClosestLines(ksups))->GetZ();
         Asups = ((KVIDZALine *) GetClosestLines(ksups))->GetA();
         wsups = ((KVIDZLine *) GetClosestLines(ksups))->GetWidth();
      dsups = GetDistanceToLine(ksups);
   }
/*   cout << "kinfi = " << kinfi << " Zinfi = " << Zinfi << "  Ainfi = " << Ainfi << "  winfi = " << winfi << "  dinfi = " << dinfi << endl;
   cout << "kinf = " << kinf << " Zinf = " << Zinf << "  Ainf = " << Ainf << "  winf = " << winf << "  dinf = " << dinf << endl;
   cout << "ksup = " << ksup << " Zsup = " << Zsup << "  Asup = " << Asup << "  wsup = " << wsup << "  dsup = " << dsup << endl;
   cout << "ksups = " << ksups << " Zsups = " << Zsups << "  Asups = " << Asups << "  wsups = " << wsups << "  dsups = " << dsups << endl;
 */
   Int_t ibif = 0;
   Int_t k = -1;
   Double_t yy, y1, y2;
   Int_t ix1, ix2;
   yy = y1 = y2 = 0;
   ix1 = ix2 = 0;
   if (ksup > -1) {
      if (kinf > -1) {
               //cout << " /******************* 2 lignes encadrant le point ont ete trouvees ************************/" << endl;
         Double_t dt = dinf + dsup;     //distance between the 2 lines
         if (Zinf == Zsup) {
               //   cout << "      /****************meme Z**************/" << endl;
            Z = Zinf;
            Int_t dA = Asup - Ainf;
            Double_t dist = dt / dA;    //distance between the 2 lines normalised to difference in A of lines
                                /*** A = Asup ***/
            if (dinf > dsup) {  //point is closest to upper line, 'sup' line
               ibif = 1;
               k = ksup;
               yy = -dsup;
               A = Asup;
               if (ksups > -1) {        // there is a 'sups' line above the 2 which encadrent le point
                  y2 = dsups - dsup;
                  if (Zsups == Zsup) {
                     ibif = 0;
                     y2 /= 2.;
                     ix2 = Asups - Asup;
                  } else {
                     if (Zsups > 0)
                        y2 /= 2.;       // 'sups' line is not IMF line
                     Double_t x2 = wsup;
                     x2 = 0.5 * TMath::Max(x2, dist);
                     y2 = TMath::Min(y2, x2);
                     ix2 = 1;
                  }
               } else {         // ksups == -1 i.e. no 'sups' line
                  y2 = wsup;
                  y2 = 0.5 * TMath::Max(y2, dist);
                  ix2 = 1;
               }
               y1 = -dt / 2.;
               ix1 = -dA;
            }
                                /*** A = Ainf ***/
            else {              //point is closest to lower line, 'inf' line
               ibif = 2;
               k = kinf;
               yy = dinf;
               A = Ainf;
               if (kinfi > -1) {        // there is a 'infi' line below the 2 which encadrent le point
                  y1 = 0.5 * (dinfi - dinf);
                  if (Zinfi == Zinf) {
                     ibif = 0;
                     ix1 = Ainfi - Ainf;
                     y1 = -y1;
                  } else {
                     Double_t x1 = winf;
                     x1 = 0.5 * TMath::Max(x1, dist);
                     y1 = -TMath::Min(y1, x1);
                     ix1 = -1;
                  }
               } else {         // kinfi = -1 i.e. no 'infi' line
                  y1 = winf;
                  y1 = -0.5 * TMath::Max(y1, dist);
                  ix1 = -1;
               }
               y2 = dt / 2.;
               ix2 = dA;
            }
         }
         else {
               //cout << "         /****************Z differents**************/ " << endl;
            if (Zsup == -1) {   //'sup' is the IMF line
               dt *= 2.;
               dsup = dt - dinf;
            }
                                /*** Z = Zsup ***/
            ibif = 3;
            if (dinf > dsup) {  // closest to upper 'sup' line
               k = ksup;
               yy = -dsup;
               Z = Zsup;
               A = Asup;
               y1 = 0.5 * wsup;
               if (ksups > -1) {        // there is a 'sups' line above the 2 which encadrent the point
                  y2 = dsups - dsup;
                  if (Zsups == Zsup) {
                     ibif = 2;
                     ix2 = Asups - Asup;
                     Double_t x1 = y2 / ix2 / 2.;
                     y1 = TMath::Max(y1, x1);
                     y1 = -TMath::Min(y1, dt / 2.);
                     ix1 = -1;
                     y2 /= 2.;
                  } else {
                     if (Zsups > 0)
                        y2 /= 2.;       // 'sups" is not IMF line
                     y2 = TMath::Min(y1, y2);
                     ix2 = 1;
                     y1 = -TMath::Min(y1, dt / 2.);
                     ix1 = -1;
                  }
               } else {         // ksups == -1, i.e. no 'sups' line
                  fIcode = kICODE7;     //a gauche de la ligne fragment, Z est alors un Zmin et le plus probable
                  y2 = y1;
                  ix2 = 1;
                  y1 = -TMath::Min(y1, dt / 2.);
                  ix1 = -1;
               }
            }
                                /*** Z = Zinf ***/
            else {              // closest to lower 'inf' line
               k = kinf;
               yy = dinf;
               Z = Zinf;
               A = Ainf;
               y2 = 0.5 * winf;
               if (kinfi > -1) {        // there is a 'infi' line below the 2 which encadrent the point
                  y1 = dinfi - dinf;
                  if (Zinfi == Zinf) {
                     ibif = 1;
                     ix1 = Ainfi - Ainf;
                     Double_t x2 = -y1 / ix1 / 2.;
                     y2 = TMath::Max(y2, x2);
                     y2 = TMath::Min(y2, dt / 2.);
                     ix2 = 1;
                     y1 /= -2.;
                  } else {
                     y1 = -TMath::Min(y2, y1 / 2.);
                     ix1 = -1;
                     y2 = TMath::Min(y2, dt / 2.);
                     ix2 = 1;
                  }
               } else {         // no kinfi line, kinfi==-1
                  y1 = -y2;
                  ix1 = -1;
                  y2 = TMath::Min(y2, dt / 2.);
                  ix1 = 1;
               }
            }
         }
      }//if(kinf>-1)...
      else if (Zsup > 0) {      // 'sup' is not IMF line
               //cout<<" /****************** Seule une ligne superieure a ete trouvee *********************/" << endl;
         ibif = 3;
         k = ksup;
         yy = -dsup;
         Z = Zsup;
         A = Asup;
         y1 = 0.5 * wsup;
         if (ksups > -1) {      // there is a 'sups' line above the closest line to the point
            y2 = dsups - dsup;
            if (Zsups == Zsup) {
               ibif = 2;
               ix2 = Asups - Asup;
               Double_t x1 = y2 / ix2 / 2.;
               y1 = -TMath::Max(y1, x1);
               ix1 = -1;
               y2 /= 2.;
            } else {
               if (Zsups > 0)
                  y2 /= 2.;     // 'sups' is not IMF line
               y2 = TMath::Min(y1, y2);
               ix2 = 1;
               y1 = -y1;
               ix1 = -1;
            }
         } else {               // no 'sups' line above closest line
            fIcode = kICODE7;   //a gauche de la ligne fragment, Z est alors un Zmin et le plus probable
            y2 = y1;
            ix2 = 1;
            y1 = -y1;
            ix1 = -1;
         }
      } else {
         fIcode = kICODE8;      //  Z indetermine ou (x,y) hors limites
      }
   }
   else if (kinf > -1) {
        //cout <<"/****************** Seule une ligne inferieure a ete trouvee ***********************/" << endl;
                /*** Sep. fragment ***/
      if (Zinf == -1) {         // 'inf' is IMF line
         //point is above IMF line. Z = Z of last line in grid + 1, A = -1
         k = -1;
         Z = GetZmax();
         A = -1;
         fIcode = kICODE6;      // au-dessus de la ligne fragment, Z est alors un Zmin
      }
                /*** Ligne de crete (Z,A line)***/
      else {
         ibif = 3;
         k = kinf;
         Z = Zinf;
         A = Ainf;
         yy = dinf;
         y2 = 0.5 * winf;
         if (kinfi > -1) {
            y1 = dinfi - dinf;
            if (Zinfi == Zinf) {
               ibif = 1;
               ix1 = Ainfi - Ainf;
               Double_t x2 = -y1 / ix1 / 2.;
               y2 = TMath::Max(y2, x2);
               ix2 = 1;
               y1 /= -2.;
            } else {
               y1 = -TMath::Min(y2, y1 / 2.);
               ix1 = -1;
               ix2 = 1;
            }
         } else {
            y1 = -y2;
            ix1 = -1;
            ix2 = 1;
         }
         fIcode = kICODE7;      // a gauche de la ligne fragment, Z est alors un Zmin et le plus probable
      }
   }
        /*****************Aucune ligne n'a ete trouvee*********************************/
   else {
      fIcode = kICODE8;         // Z indetermine ou (x,y) hors limites
   }
        /****************Test des bornes********************************************/
   if (k > -1 && GetICode() == kICODE0) {
      if (yy > y2)
         fIcode = kICODE4;      // Z ok, masse hors limite superieure ou egale a A
   }
   if (k > -1 && (GetICode() == kICODE0 || GetICode() == kICODE7)) {
      if (yy < y1)
         fIcode = kICODE5;      // Z ok, masse hors limite inferieure ou egale a A
   }
   if (GetICode() == kICODE4 || GetICode() == kICODE5)
      A = -1;
        /****************Interpolation de la masse: da = f*log(1+b*dy)********************/
   if (GetICode() == kICODE0 || (GetICode() == kICODE7 && yy <= y2)) {
      Double_t deltaA = 0.;
      Bool_t i = kFALSE;
      Double_t dt, dist = y1 * y2;
      dt = 0.;
      if (ix2 == -ix1) {        //dA1 = dA2
         dt = -(y1 + y2) / dist;
         i = kTRUE;
      } else if (ix2 == -ix1 * 2) {     // dA2 = 2*dA1
         dt = -(y1 + 2. * y2 -
                TMath::Sqrt(y1 * y1 - 4. * dist)) / dist / 2.;
         i = kTRUE;
      } else if (ix1 == -ix2 * 2) {     // dA1 = 2*dA2
         dt = -(y2 + 2. * y1 +
                TMath::Sqrt(y2 * y2 - 4. * dist)) / dist / 2.;
         i = kTRUE;
      }
      if (i) {
         dist = dt * y2;
         if (TMath::Abs(dist) < 0.001) {
            deltaA = yy * ix2 / y2 / 2.;
         } else {
            deltaA =
                ix2 / 2. / TMath::Log(1. + dist) * TMath::Log(1. +
                                                              dt * yy);
         }
         A += deltaA;
      }
   }
        /***************D'autres masses sont-elles possibles ?*************************/
   if (GetICode() == kICODE0) {
      //cout << "icode = 0, ibif = " << ibif << endl;
                /***Masse superieure***/
      if (ibif == 1 || ibif == 3) {
         //We look at next line in the complete list of lines, after the closest line.
         //If it has the same Z as the closest line, but was excluded from research for closest line
         //because the point lies outside the endpoints, there remains a doubt about the mass:
         //on rajoute 1 a fICode, effectivement on le met = kICODE1
         Int_t idx = GetIDLines()->IndexOf(GetClosestLine());
         if (idx > -1 && ++idx < GetIDLines()->GetSize()) {
            KVIDZLine *nextline =
                (KVIDZLine *) GetIDLines()->At(idx);
            if (nextline->GetZ() == Z
                && !nextline->IsBetweenEndPoints(x, y, "x")){
               fIcode++;        // Z ok, mais les masses superieures a A sont possibles
               //cout <<"//on rajoute 1 a fICode, effectivement on le met = kICODE1" << endl;
            }
         }
      }
                /***Masse inferieure***/
      if (ibif == 2 || ibif == 3) {
         //We look at line below the closest line in the complete list of lines.
         //If it has the same Z as the closest line, but was excluded from research for closest line
         //because the point lies outside the endpoints, there remains a doubt about the mass:
         //on rajoute 2 a fICode, so it can be = kICODE2 or kICODE3
         Int_t idx = GetIDLines()->IndexOf(GetClosestLine());
         if (idx > -1 && --idx >= 0) {
            KVIDZLine *nextline =
                (KVIDZLine *) GetIDLines()->At(idx);
            if (nextline->GetZ() == Z
                && !nextline->IsBetweenEndPoints(x, y, "x")){
               fIcode+=2;
               //cout << "//on rajoute 2 a fICode, so it can be = kICODE2 or kICODE3" << endl;
            }
         }
      }
   }
   
  // cout << "Z = " << Z << " A = " << A << " icode = " << fIcode << endl;
}

//_______________________________________________________________________________________________//

void KVIDZAGrid::Identify(Double_t x, Double_t y, KVReconstructedNucleus * nuc) const
{
   //Set Z and A of nucleus based on position in grid

   Int_t i1, i2;
   KVIDLine *nearest = FindNearestIDLine(x, y, "above", i1, i2);
   if (!nearest) {
      //no lines corresponding to point were found
      const_cast < KVIDZAGrid * >(this)->fIcode = kICODE8;        // Z indetermine ou (x,y) hors limites
      return;
   }
   Int_t Z;
   Double_t A;
   const_cast < KVIDZAGrid * >(this)->IdentZA(x, y, Z, A);
   nuc->SetZ(Z);
   if (A > -1) {
      nuc->SetRealA(A);
      nuc->SetA(TMath::Nint(A));
   }
}

//___________________________________________________________________________________

Bool_t KVIDZAGrid::AcceptIDForTest()
{
   //Used by TestIdentification.
   //The result of the identification may be excluded from the histograms of PID
   //and PID vs. Eres, depending on e.g. some status code of the identification
   //algorithm.
   //We only include particles with Icode < 4 as being "well-identified"
   return (fIcode<4);
}

