/***************************************************************************
                          KVIDGCsI.cpp  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDGCsI.cpp,v 1.25 2009/04/06 15:09:29 franklan Exp $
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVIDGCsI.h"
#include "KVIDCutLine.h"
#include "KVIDCsIRLLine.h"
#include "KVINDRAReconNuc.h"
#include "KVINDRA.h"

ClassImp(KVIDGCsI)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//KVIDGCsI
//
//Identification grids for CsI R-L matrices.
//
//The identification procedure is supposed to be identical to that of the FORTRAN algorithm IdnCsOr
//developed by Laurent Tassan-Got and used by the INDRA collaboration since 1993.
//
//The status codes returned by GetQualityCode() are the same as IdnCsOr, with the addition of kICODE10
//for identification of gammas:
//
// KVIDGCsI::kICODE0  ok
// KVIDGCsI::kICODE1  Z ok, mais les masses superieures a A sont possibles
// KVIDGCsI::kICODE2  Z ok, mais les masses inferieures a A sont possibles
// KVIDGCsI::kICODE3  Z ok, mais les masses superieures ou inferieures a A sont possibles
// KVIDGCsI::kICODE4  Z ok, masse hors limite superieure ou egale a A
// KVIDGCsI::kICODE5  Z ok, masse hors limite inferieure ou egale a A
// KVIDGCsI::kICODE6  au-dessus de la ligne fragment, Z est alors un Zmin
// KVIDGCsI::kICODE7  a gauche de la ligne fragment, Z est alors un Zmin et le plus probable
// KVIDGCsI::kICODE8  Z indetermine ou (x,y) hors limites
// KVIDGCsI::kICODE9  pas de lignes pour ce module
// KVIDGCsI::kICODE10 gamma
   
	
KVIDGCsI::KVIDGCsI()
{
   //Default constructor
   IMFLine = 0;
   GammaLine = 0;
}

KVIDGCsI::KVIDGCsI(const KVIDGCsI & grid)
{
   //Copy constructor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   grid.Copy(*this);
#else
   ((KVIDGCsI &) grid).Copy(*this);
#endif
}


KVIDGCsI::~KVIDGCsI()
{
   //Dtor.
}

//______________________________________________________________________________________________________//

Bool_t KVIDGCsI::IsIdentifiable(Double_t x, Double_t y) const
{
   //Performs rejection of gammas - returns kTRUE if point above gamma line
   //(note - position w.r.t. IMF line is not considered)
   //Returns kFALSE for points below gamma line i.e. for gammas ;-)
   //If no gamma line defined, returns kTRUE for all points


   return (GammaLine? GammaLine->
           WhereAmI(x, y, "above") : kTRUE);
}

//______________________________________________________________________________________________//

KVIDLine *KVIDGCsI::GetNearestIDLine(Double_t x, Double_t y,
                                     const Char_t * position,
                                     Int_t & idx_min, Int_t & idx_max)
{
   //For a point (x,y) in the CsI R-L grid, we look for the closest lines to the point,
   //only considering lines for whom x1 <= x <= x2, where x1 and x2 are the x-coordinates of
   //the leftmost (first) and rightmost (last) points of the line.
   //In accordance with IdnCsOr, the 'IMF line' is included in the search as well as the lD lines.
   //After this search is performed, the closest lines and their distances from the point
   //are accessible using GetClosestLines and GetDistanceToLine.
   //Call IdentZA after using this function in order to obtain Z, A and real A.

   idx_min = idx_max = -1;

   //We work with a subset of ID lines for which the composante lente (X-coord) of the point to
   //be identified is inbetween the minimum and maximum X-coords of the line.
   Int_t nlines;
   TList *idlines = GetIDLinesEmbracingPoint("x", x, y, nlines);

   //if the IMF line 'embraces' the point, we add it to the list
   if (IMFLine->IsBetweenEndPoints(x, y, "x")) {
      idlines->Add(IMFLine);
      nlines++;
   }
   //not a single line found to identify the point!!
   if (!nlines) {
//      Info("GetNearestIDLine","No lines to identify point!");
      delete idlines;
      return 0;
   }
   //Dline[] and Lines[] : distances to and pointers of lines
   //ind_list[] : index of line in complete list of all ID lines
   ClearWorkingArrays();        // (re)initialise arrays

 //     Info("GetNearestIDLine","Lines embracing point:");
      
   if (nlines > 1) {
      //loop over lines, filling in the arrays
      TIter next(idlines);
      KVIDLine *line;
      Int_t idx = 0;
      while ((line = (KVIDLine *) next())) {

   //      cout << line->GetName() << endl;
         
         Int_t dummy;
         Double_t dist = TMath::Abs(line->DistanceToLine(x, y, dummy));
         Dline[idx] = dist;
         Lines[idx] = line;
         ind_list[idx] = (Double_t) GetIdentifiers()->IndexOf(line);
         if (ind_list[idx] < 0)
            ind_list[idx] = 98; //IMF line
         idx++;
      }
   } else {
      Int_t dummy;
      KVIDLine *line = (KVIDLine *) idlines->At(0);
         
     // cout << line->GetName() << endl;
      
      Double_t dist = TMath::Abs(line->DistanceToLine(x, y, dummy));
      Dline[0] = dist;
      Lines[0] = line;
      ind_list[0] = (Double_t) GetIdentifiers()->IndexOf(line);
      if (ind_list[0] < 0)
         ind_list[0] = 98;      //IMF line
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
      // i.e. in order of increasing 'Rapide' component
      TMath::BubbleLow(fNLines, ind_list2, ind_arr);
   } else {
      ind_arr[0] = 0;
   }

    //  Info("GetNearestIDLine","Nearest lines to point, from bottom to top:");
      
   for (register int i = 0; i < fNLines; i++) {
      Int_t idx = (Int_t) ind_list2[ind_arr[i]];
      if (idx == idx_closest){
         fIdxClosest = i;
  //       cout << "CLOSEST : " ;
      }
      if (idx < 98) {
         fLines[i] = (KVIDLine*)GetIdentifiers()->At(idx);
         fDistances[i] = Dline2[ind_arr[i]];
   //      cout << i << " : " << fLines[i]->GetName() << "   d= " << fDistances[i] << endl;
      } else if (idx == 98) {
         fLines[i] = IMFLine;
         fDistances[i] = Dline2[ind_arr[i]];
  //       cout << i << " : " << fLines[i]->GetName() << "   d= " << fDistances[i] << endl;
      } else {
         fLines[i] = 0;
         fDistances[i] = -99.;
      }
   }

   return fClosest;
}

//_______________________________________________________________________________________________//

void KVIDGCsI::Identify(Double_t x, Double_t y,
                        KVReconstructedNucleus * nuc) const
{
   //Set Z and A of nucleus based on position in R-L grid
   //The identification of gammas (kICODE10) and charged particles is performed

   nuc->SetZMeasured(kFALSE);
   nuc->SetAMeasured(kFALSE);
		
   if (!IsIdentifiable(x, y)) {
      //point below gamma line
      const_cast < KVIDGCsI * >(this)->fICode = kICODE10;
      nuc->SetZ(0);
      return;
   }
   Int_t i1, i2;
   KVIDLine *nearest = FindNearestIDLine(x, y, "above", i1, i2);
   if (!nearest) {
      //no lines corresponding to point were found
      const_cast < KVIDGCsI * >(this)->fICode = kICODE8;        // Z indetermine ou (x,y) hors limites
      return;
   }
   Int_t Z;
   Double_t A;
   const_cast < KVIDGCsI * >(this)->IdentZA(x, y, Z, A);
   nuc->SetZ(Z);
   nuc->SetZMeasured(kTRUE);
   if (A > -1) {
      nuc->SetRealA(A);
      nuc->SetAMeasured(kTRUE);
      nuc->SetA(TMath::Nint(A));
   }
}

//_________________________________________________________________________//

KVIDZALine *KVIDGCsI::GetZLine(Int_t z, Int_t & index) const
{
   //Returns first ID line in sorted list for which GetZ() returns 'z'.
   //index=index of line found in fIDLines list (-1 if not found).
   //This is done by looping over all the lines in the list, not by dichotomy as in base class KVIDZAGrid,
   //because of the 8Be line being in between 6He and 6Li.

   index = 0;
   Int_t nlines = GetNumberOfIdentifiers();
   while ((dynamic_cast < KVIDZALine * >(GetIdentifiers()->At(index))->GetZ() != z)
          && (index < (nlines - 1)))
      index++;
   KVIDZALine *line = dynamic_cast < KVIDZALine * >(GetIdentifiers()->At(index));
   if (line->GetZ() != z) {
      index = -1;
      return 0;
   }
   return line;
}

//_________________________________________________________________________//

KVIDZALine *KVIDGCsI::GetZALine(Int_t z, Int_t a, Int_t & index) const
{
   //Returns ID line corresponding to nucleus (Z,A) and its index in fIDLines.
   //This is done by looping over all the lines in the list, not as in base class KVIDZAGrid,
   //because of the 8Be line being in between 6He and 6Li.

   index = 0;
   Int_t nlines = GetNumberOfIdentifiers();
   KVIDZALine *line = dynamic_cast < KVIDZALine * >(GetIdentifiers()->At(index));
   while ((line->GetZ() != z || line->GetA() != a)
          && (index < (nlines - 1))) {
      line = dynamic_cast < KVIDZALine * >(GetIdentifiers()->At(++index));
   }
   if (line->GetZ() != z || line->GetA() != a) {
      index = -1;
      return 0;
   }
   return line;
}

//_________________________________________________________________________//

void KVIDGCsI::IdentZA(Double_t x, Double_t y, Int_t & Z, Double_t & A)
{
   //Finds Z, A and 'real A' for point (x,y) once closest lines to point have been found.
   //This is a line-for-line copy of the latter part of IdnCsOr, even the same
   //variable names and comments have been used (as much as possible).

   //Work out kinfi, kinf, ksup and ksups like in IdnCsIOr
   Int_t kinfi, kinf, ksup, ksups;
   kinfi = kinf = ksup = ksups = -1;
   fICode = kICODE0;
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
		KVIDZALine* idl = (KVIDZALine *) GetClosestLines(kinf);
      if (idl != IMFLine) {
         Zinf = idl->GetZ();
         Ainf = idl->GetA();
         winf = idl->GetWidth();
      } else {
         Zinf = -1;
         Ainf = -1;
         winf = 16000.;
      }
      dinf = GetDistanceToLine(kinf);
   }
   if (ksup > -1) {
		KVIDZALine* idl = (KVIDZALine *) GetClosestLines(ksup);
      if (idl != IMFLine) {
         Zsup = idl->GetZ();
         Asup = idl->GetA();
         wsup = idl->GetWidth();
      } else {
         Zsup = -1;
         Asup = -1;
         wsup = 16000.;
      }
      dsup = GetDistanceToLine(ksup);
   }
   if (kinfi > -1) {
		KVIDZALine* idl = (KVIDZALine *) GetClosestLines(kinfi);
      if (GetClosestLines(kinfi) != IMFLine) {
         Zinfi = idl->GetZ();
         Ainfi = idl->GetA();
         winfi = idl->GetWidth();
      } else {
         Zinfi = -1;
         Ainfi = -1;
         winfi = 16000.;
      }
      dinfi = GetDistanceToLine(kinfi);
   }
   if (ksups > -1) {
		KVIDZALine* idl = (KVIDZALine *) GetClosestLines(ksups);
      if (idl != IMFLine) {
         Zsups = idl->GetZ();
         Asups = idl->GetA();
         wsups = idl->GetWidth();
      } else {
         Zsups = -1;
         Asups = -1;
         wsups = 16000.;
      }
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
                  fICode = kICODE7;     //a gauche de la ligne fragment, Z est alors un Zmin et le plus probable
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
            fICode = kICODE7;   //a gauche de la ligne fragment, Z est alors un Zmin et le plus probable
            y2 = y1;
            ix2 = 1;
            y1 = -y1;
            ix1 = -1;
         }
      } else {
         fICode = kICODE8;      //  Z indetermine ou (x,y) hors limites
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
         fICode = kICODE6;      // au-dessus de la ligne fragment, Z est alors un Zmin
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
         fICode = kICODE7;      // a gauche de la ligne fragment, Z est alors un Zmin et le plus probable
      }
   }
        /*****************Aucune ligne n'a ete trouvee*********************************/
   else {
      fICode = kICODE8;         // Z indetermine ou (x,y) hors limites
   }
        /****************Test des bornes********************************************/
   if (k > -1 && fICode == kICODE0) {
      if (yy > y2)
         fICode = kICODE4;      // Z ok, masse hors limite superieure ou egale a A
   }
   if (k > -1 && (fICode == kICODE0 || fICode == kICODE7)) {
      if (yy < y1)
         fICode = kICODE5;      // Z ok, masse hors limite inferieure ou egale a A
   }
   if (fICode == kICODE4 || fICode == kICODE5)
      A = -1;
        /****************Interpolation de la masse: da = f*log(1+b*dy)********************/
   if (fICode == kICODE0 || (fICode == kICODE7 && yy <= y2)) {
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
   if (fICode == kICODE0) {
      //cout << "icode = 0, ibif = " << ibif << endl;
                /***Masse superieure***/
      if (ibif == 1 || ibif == 3) {
         //We look at next line in the complete list of lines, after the closest line.
         //If it has the same Z as the closest line, but was excluded from research for closest line
         //because the point lies outside the endpoints, there remains a doubt about the mass:
         //on rajoute 1 a fICode, effectivement on le met = kICODE1
         Int_t idx = GetIdentifiers()->IndexOf(GetClosestLine());
         if (idx > -1 && ++idx < GetNumberOfIdentifiers()) {
            KVIDCsIRLLine *nextline =
                (KVIDCsIRLLine *) GetIdentifiers()->At(idx);
            if (nextline->GetZ() == Z
                && !nextline->IsBetweenEndPoints(x, y, "x")){
               fICode++;        // Z ok, mais les masses superieures a A sont possibles
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
         Int_t idx = GetIdentifiers()->IndexOf(GetClosestLine());
         if (idx > -1 && --idx >= 0) {
            KVIDCsIRLLine *nextline =
                (KVIDCsIRLLine *) GetIdentifiers()->At(idx);
            if (nextline->GetZ() == Z
                && !nextline->IsBetweenEndPoints(x, y, "x")){
               fICode+=2;
               //cout << "//on rajoute 2 a fICode, so it can be = kICODE2 or kICODE3" << endl;
            }
         }
      }
   }
   
   //cout << "Z = " << Z << " A = " << A << " icode = " << fICode << endl;
}

//___________________________________________________________________________________

void KVIDGCsI::Initialize()
{
   // General initialisation method for identification grid.
   // This method MUST be called once before using the grid for identifications.
   // The ID lines are sorted.
   // The natural line widths of all ID lines are calculated.
   // The line with the largest Z (Zmax line) is found.
   // IMF & Gamma line pointers are initialised
   
   KVIDZAGrid::Initialize();
   GammaLine = (KVIDLine*)GetCut("gamma_line");
   IMFLine = (KVIDLine*)GetCut("IMF_line");
}

//___________________________________________________________________________________

void KVIDGCsI::BackwardsCompatibilityFix()
{
	// Called after reading a grid from an ascii file.
	// Tries to convert information written by an old version of the class:
	//
	//<PARAMETER> Ring min=...  ----> <PARAMETER> IDTelescopes=...
	//<PARAMETER> Ring max=...
	//<PARAMETER> Mod min=...
	//<PARAMETER> Mod max=...
	
	KVIDZAGrid::BackwardsCompatibilityFix();
	if( fPar->HasParameter("IDTelescopes") ) return;
	if( fPar->HasParameter("Ring min") && gIndra ){
		for(int r=fPar->GetIntValue("Ring min"); r<=fPar->GetIntValue("Ring max"); r++){
			for(int m=fPar->GetIntValue("Mod min"); m<=fPar->GetIntValue("Mod max"); m++){
				KVIDTelescope* id = gIndra->GetIDTelescope( Form("CSI_R_L_%02d%02d", r, m) );
				if(id) AddIDTelescope(id);
			}
		}
		WriteParameterListOfIDTelescopes();
		fPar->RemoveParameter("Ring min");
		fPar->RemoveParameter("Ring max");
		fPar->RemoveParameter("Mod min");
		fPar->RemoveParameter("Mod max");
	}
   GammaLine = (KVIDLine*)GetCut("gamma_line");
   IMFLine = (KVIDLine*)GetCut("IMF_line");
	if(GammaLine) ((KVIDCutLine*)GammaLine)->SetAcceptedDirection("above");
	if(IMFLine) ((KVIDCutLine*)IMFLine)->SetAcceptedDirection("below");
	SetVarY("CSI-R");
	SetVarX("CSI-L");
}
