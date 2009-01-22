/***************************************************************************
                          KVIDCsIRLLine.h  -  description
                             -------------------
    begin                : Nov 10 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDCsIRLLine.h,v 1.5 2007/07/23 16:46:32 franklan Exp $
***************************************************************************/

#ifndef KVIDCsIRLLine_H
#define KVIDCsIRLLine_H

#include "KVIDZALine.h"

class KVIDCsIRLLine:public KVIDZALine {

 public:

    KVIDCsIRLLine();
    virtual ~ KVIDCsIRLLine();
   Int_t Compare(const TObject *) const;

   void Print(Option_t * opt = "") const;
   virtual void SetAsymWidth(Double_t d_l, Double_t d_r);

   ClassDef(KVIDCsIRLLine, 3)   //Base class for CsI R-L identification lines
};

#endif
