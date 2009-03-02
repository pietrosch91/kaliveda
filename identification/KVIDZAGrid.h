/***************************************************************************
                          KVIDZAGrid.h  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZAGrid.h,v 1.10 2009/03/02 16:48:17 franklan Exp $
***************************************************************************/

#ifndef KVIDZAGrid_H
#define KVIDZAGrid_H

#include "KVIDZGrid.h"
#include "TObjArray.h"

class KVIDZALine;

class KVIDZAGrid:public KVIDZGrid {

 private:

 protected:

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

   
    KVIDZAGrid();
    KVIDZAGrid(const KVIDZAGrid &);
    virtual ~ KVIDZAGrid();

   virtual KVIDZALine *GetZALine(Int_t z, Int_t a, Int_t &) const;

   virtual void IdentZA(Double_t x, Double_t y, Int_t & Z, Double_t & A);
   virtual void Identify(Double_t x, Double_t y,
                         KVReconstructedNucleus * nuc) const;
	virtual TClass* DefaultIDLineClass(){
		return TClass::GetClass("KVIDZALine");
	};
	
    ClassDef(KVIDZAGrid, 1)     //Base class for 2D Z & A identification grids
};

#endif
