/***************************************************************************
                          KVIDZALine.h  -  description
                             -------------------
    begin                : Nov 10 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZALine.h,v 1.4 2007/11/27 12:09:03 ebonnet Exp $
***************************************************************************/

#ifndef KVIDZALine_H
#define KVIDZALine_H

#include "KVIDZLine.h"

class KVIDZALine:public KVIDZLine {

 protected:
 
   UShort_t fA;                 //atomic number of element corresponding to line

 public:

    KVIDZALine();
    virtual ~ KVIDZALine();

   Int_t GetA() const;
   Int_t Compare(const TObject *) const;
   void SetA(Int_t atnum);      // *MENU* *ARGS={atnum=>fA}
   void SetAandZ(Int_t atnum,Int_t ztnum);      // *MENU* *ARGS={atnum=>fA,ztnum=>fZ}

   void SetNameLine();

   void WriteAsciiFile(ofstream &, const Char_t * name_prefix = "");
   void ReadAsciiFile(ifstream &);

   void Print(Option_t * opt = "") const;
   virtual Int_t GetID() const {
      return GetA();
   };

    ClassDef(KVIDZALine, 1)     //Base class for Z & A identification lines
};

#endif
