/***************************************************************************
                          KVIDZAGrid.h  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZAGrid.h,v 1.12 2009/03/03 14:27:15 franklan Exp $
***************************************************************************/

#ifndef KVIDZAGrid_H
#define KVIDZAGrid_H

#include "KVIDGrid.h"
#include "TObjArray.h"

class KVIDZALine;

class KVIDZAGrid:public KVIDGrid {

 private:

 protected:

   UShort_t fZMax;              //largest Z of lines in grid
   KVIDZALine*  fZMaxLine;//! line with biggest Z and A
   
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
   void init();
 
   // Used by TestIdentification.
   // The result of the identification may be excluded from the histograms of PID
   // and PID vs. Eres, depending on the quality code of the identification algorithm.
   // (given by GetQualityCode()).
   // For a general (Z,A) grid we only include particles with GetQualityCode() < 4 as being "well-identified"
   Bool_t AcceptIDForTest()
   {
      return (GetQualityCode()<4);
   };

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
   
   
    KVIDZAGrid();
    KVIDZAGrid(const KVIDZAGrid &);
    virtual ~ KVIDZAGrid();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif

   virtual void Initialize();//*MENU*
   void DrawLinesWithWidth();// *MENU={Hierarchy="View.../DrawLinesWithWidth"}*

   virtual void CalculateLineWidths();
   Int_t GetZmax() const { return (Int_t)fZMax; };
   virtual KVIDZALine *GetZLine(Int_t z, Int_t &) const;
   KVIDZALine *GetZmaxLine() const { return fZMaxLine; };
   virtual KVIDZALine *GetZALine(Int_t z, Int_t a, Int_t &) const;

   virtual void IdentZA(Double_t x, Double_t y, Int_t & Z, Double_t & A);
	virtual TClass* DefaultIDLineClass(){
		return TClass::GetClass("KVIDZALine");
	};
   virtual void IdentZ(Double_t x, Double_t y, Double_t & Z);
   Int_t GetQualityCode() const 
   {
      // Return quality code for previously-attempted identification
      // Meanings of code values are given in class description
      return fICode;
   };

   virtual void Identify(Double_t x, Double_t y, KVReconstructedNucleus * nuc) const;

   KVIDLine *FindNearestIDLine(Double_t x, Double_t y,
                               const Char_t * position, Int_t & idx_min,
                               Int_t & idx_max) const
   {
      //See GetNearestIDLine
      //This is just a wrapper because of "const" attribute of base class method
      return (const_cast <KVIDZAGrid*>(this)->GetNearestIDLine(x, y, position, idx_min,
                                                idx_max));
   }

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
   
    ClassDef(KVIDZAGrid, 2)     //Base class for 2D Z & A identification grids
};

#endif
