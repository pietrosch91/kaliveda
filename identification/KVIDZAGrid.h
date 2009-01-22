/***************************************************************************
                          KVIDZAGrid.h  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZAGrid.h,v 1.9 2008/10/13 13:52:29 franklan Exp $
***************************************************************************/

#ifndef KVIDZAGrid_H
#define KVIDZAGrid_H

#include "KVIDZGrid.h"
#include "TObjArray.h"

class KVIDZALine;

class KVIDZAGrid:public KVIDZGrid {

 private:

 protected:

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
   Int_t fIcode;                //!code de retour

   void ClearWorkingArrays();
   virtual KVIDLine *GetNearestIDLine(Double_t x, Double_t y,
                              const Char_t * position, Int_t & idx_min,
                              Int_t & idx_max);

   void init();
   Bool_t AcceptIDForTest();

 public:

   enum {
      kICODE0,                  // ok
      kICODE1,                  // Z ok, mais les masses superieures a A sont possibles
      kICODE2,                  // Z ok, mais les masses inferieures a A sont possibles
      kICODE3,                  // Z ok, mais les masses superieures ou inferieures a A sont possibles
      kICODE4,                  // Z ok, masse hors limite superieure ou egale a A
      kICODE5,                  // Z ok, masse hors limite inferieure ou egale a A
      kICODE6,                  // au-dessus de la ligne fragment, Z est alors un Zmin
      kICODE7,                  // a gauche de la ligne fragment, Z est alors un Zmin et le plus probable
      kICODE8,                  // Z indetermine ou (x,y) hors limites
      kICODE9,                  // pas de lignes pour ce module
      kICODE10                  // gamma
   };
   
    KVIDZAGrid();
    KVIDZAGrid(const KVIDZAGrid &);
    virtual ~ KVIDZAGrid();

   virtual KVIDZALine *GetZALine(Int_t z, Int_t a, Int_t &) const;
   KVIDLine *FindNearestIDLine(Double_t x, Double_t y,
                               const Char_t * position, Int_t & idx_min,
                               Int_t & idx_max) const;

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
   inline Int_t GetICode() const {
      return fIcode;
   };

   virtual void IdentZA(Double_t x, Double_t y, Int_t & Z, Double_t & A);
   virtual void Identify(Double_t x, Double_t y,
                         KVReconstructedNucleus * nuc) const;
	virtual TClass* DefaultIDLineClass(){
		return TClass::GetClass("KVIDZALine");
	};
	
    ClassDef(KVIDZAGrid, 1)     //Base class for 2D Z & A identification grids
};

#endif
