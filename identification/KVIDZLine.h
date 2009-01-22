/***************************************************************************
                          KVIDZLine.h  -  description
                             -------------------
    begin                : Nov 10 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZLine.h,v 1.10 2007/11/27 12:08:21 ebonnet Exp $
***************************************************************************/

#ifndef KVIDZLine_H
#define KVIDZLine_H

#include "KVIDLine.h"
#include "TGraphErrors.h"

class KVIDZLine:public KVIDLine {

  protected:
   
	UShort_t fZ;                 //atomic number of element corresponding to line
   Double_t fWidth;             //the "width" of the line
   TGraphErrors *fLineWithWidth; //! used to display width of line

 public:

    KVIDZLine();
    virtual ~ KVIDZLine();

   Int_t GetZ() const;
   Int_t Compare(const TObject *) const;
   virtual void SetZ(Int_t atnum);      // *MENU* *ARGS={atnum=>fZ}

   virtual void SetNameLine();

   void WriteAsciiFile(ofstream &, const Char_t * name_prefix = "");
   void ReadAsciiFile(ifstream &);

   void Print(Option_t * opt = "") const;
   virtual Int_t GetID() const {
      return GetZ();
   };

   Double_t GetWidth() const {
      return fWidth;
   };
   void SetWidth(Double_t w) {
      fWidth = w;
   };
   virtual void SetAsymWidth(Double_t d_l, Double_t d_r);
   
   TGraphErrors* GetLineWithWidth();
   
    ClassDef(KVIDZLine, 2)      //Base class for Z-identification lines in dE-E maps
};

#endif
