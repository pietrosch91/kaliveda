/***************************************************************************
                          KVIDZGrid.h  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZGrid.h,v 1.11 2008/10/13 13:52:29 franklan Exp $
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

 public:

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

   ClassDef(KVIDZGrid, 1)       //Base class for 2D Z identification grids (E - dE method)
};

#endif
