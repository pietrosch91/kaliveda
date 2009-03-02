/***************************************************************************
                          KVIDZGrid.h  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZGrid.h,v 1.14 2009/03/02 16:48:17 franklan Exp $
***************************************************************************/

#ifndef KVIDZGrid_H
#define KVIDZGrid_H

#include "KVIDGrid.h"

class KVIDZLine;

class KVIDZGrid:public KVIDGrid {

 protected:

   UShort_t fZMax;              //largest Z of lines in grid
   KVIDZLine*  fZMaxLine;//! line with biggest Z

   void init();
   void SetZmax(Int_t z) {
      fZMax = z;
   };
   Double_t *Dline;  //!working array used by FindNearestIDLine
   Double_t *Dline2;          //!working array used by FindNearestIDLine
   TObjArray Lines; //!working array used by FindNearestIDLine
   Double_t *ind_list;       //!working array used by FindNearestIDLine
   Double_t *ind_list2;       //!working array used by FindNearestIDLine
   Int_t *ind_arr;   //!working array used by FindNearestIDLine

   TObjArray fLines;         //! closest lines to last-identified point, in order of increasing ordinate
   KVIDLine *fClosest;          //!closest line to last-identified point
   Double_t *fDistances;      //!distance from point to each of the lines in fLines
   Double_t fDistanceClosest;   //!distance from point to closest line
   Int_t fNLines;             //!number of lines in fLines array 
   Int_t fIdxClosest;         //!index of closest line in fLines array 
   Int_t fICode;                //!code de retour

   void ClearWorkingArrays();
   virtual KVIDLine *GetNearestIDLine(Double_t x, Double_t y,
                              const Char_t * position, Int_t & idx_min,
                              Int_t & idx_max);


 public:

   enum {
      kICODE0,                  
      kICODE1,                  
      kICODE2,                  
      kICODE3,                  
      kICODE4,                  
      kICODE5,                  
      kICODE6,                  
      kICODE7,                  
      kICODE8,                  
      kICODE9,                  
      kICODE10                  
   };
   
   KVIDZGrid();
   KVIDZGrid(const KVIDZGrid &);
   virtual ~ KVIDZGrid();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif

   virtual void Initialize();

   virtual void CalculateLineWidths();
   Int_t GetZmax() const { return (Int_t)fZMax; };
   virtual KVIDZLine *GetZLine(Int_t z, Int_t &) const;
   KVIDZLine *GetZmaxLine() const { return fZMaxLine; };

   virtual void Identify(Double_t x, Double_t y,
                         KVReconstructedNucleus * nuc) const;
   void DrawLinesWithWidth();
	virtual TClass* DefaultIDLineClass(){
		return TClass::GetClass("KVIDZLine");
	};

   inline KVIDLine *GetClosestLine() const {
      return fClosest;
   };
   inline KVIDLine *GetClosestLines(Int_t i) const {
      return (KVIDLine*)fLines[i];
   };
   inline Double_t GetDistanceToLine(Int_t i) const {
      return fDistances[i];
   };
   inline Double_t GetDistanceClosestLine() const {
      return fDistanceClosest;
   };
   inline UChar_t GetNClosestLines() const {
      return fNLines;
   };
   inline UChar_t GetIndexClosest() const {
      return fIdxClosest;
   };
   ClassDef(KVIDZGrid, 1)       //Base class for 2D Z identification grids (E - dE method)
};

#endif
