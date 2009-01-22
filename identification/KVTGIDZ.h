/***************************************************************************
                          KVTGIDZ.h  -  description
                             -------------------
    begin                : 5 July 2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVTGIDZ.h,v 1.4 2006/10/19 14:32:43 franklan Exp $
***************************************************************************/

#ifndef KVTGIDZ__H
#define KVTGIDZ__H

#include "KVTGID.h"

class KVTGIDZ:public KVTGID {
 protected:
   virtual void SetIdent(KVIDLine *, Double_t ID);
   virtual KVIDLine *AddLine(KVIDGrid *);
   virtual KVIDGrid *NewGrid();

 public:
    KVTGIDZ() {
   };
   KVTGIDZ(const Char_t * name, const Char_t * function,
           Double_t xmin, Double_t xmax, Int_t npar, Int_t x_par,
           Int_t y_par);
   virtual ~ KVTGIDZ() {
   };

   ClassDef(KVTGIDZ, 1)         //Abstract base class for Z identification functionals by L. Tassan-Got
};

#endif
