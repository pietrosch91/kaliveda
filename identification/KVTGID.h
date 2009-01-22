/***************************************************************************
                          KVTGID.h  -  description
                             -------------------
    begin                : 5 July 2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVTGID.h,v 1.11 2008/03/06 10:10:55 franklan Exp $
***************************************************************************/

#ifndef KVTGID__H
#define KVTGID__H

#include "TF1.h"
#include "KVIDGrid.h"
#include "TString.h"

class KVTGID:public TF1 {
 private:

   Double_t fID_min;            //minimum ID fitted with functional
   Double_t fID_max;            //maximum ID fitted with functional
   Int_t fStatus;               //! transient member, holds status code of last call to GetIdentification
   void init();
   TString fTGIDFunctionName;   //name of KVTGIDFunctions:: namespace function used for identification

 protected:

    virtual void SetIdent(KVIDLine *, Double_t ID) = 0;
   virtual KVIDLine *AddLine(KVIDGrid *) = 0;
   virtual KVIDGrid *NewGrid() = 0;

 public:

   //status codes for GetIdentification
   enum {
      kStatus_OK,               //normal identification
      kStatus_NotBetween_IDMin_IDMax    //no solution between fID_min and fID_max
   };

   const Char_t *GetFunctionName() const {
      return fTGIDFunctionName.Data();
   };

    KVTGID();
    KVTGID(const Char_t * name, const Char_t * function,
           Double_t xmin, Double_t xmax, Int_t npar, Int_t par_x,
           Int_t par_y);
    virtual ~ KVTGID() {
   };

   void SetIDmax(Double_t x) {
      fID_max = x;
   };
   Double_t GetIDmax() const {
      return fID_max;
   };
   void SetIDmin(Double_t x) {
      fID_min = x;
   };
   Double_t GetIDmin() const {
      return fID_min;
   };

   virtual Double_t GetIdentification(Double_t ID_min, Double_t ID_max,
                                      Double_t & ID_quality,
                                      Double_t * par = 0);

   virtual KVIDGrid *MakeIDGrid(Double_t xmax, Double_t xmin = 0.,
                                Int_t ID_min = 0, Int_t ID_max =
                                0, Int_t npoints = 100, Bool_t logscale = kFALSE);
   virtual void AddLineToGrid(KVIDGrid * g, Int_t ID, Int_t npoints,
                              Double_t xmin, Double_t xmax, Bool_t log_scale=kFALSE);

   virtual void Print(Option_t * option = "") const;

   Int_t Compare(const TObject * obj) const;

   inline virtual Int_t GetStatus() const {
      return fStatus;
   };
   virtual const Char_t *GetStatusString() const;

   Double_t GetDistanceToLine(Double_t x, Double_t y, Int_t id,
                              Double_t * params = 0);

   ClassDef(KVTGID, 2)          //Abstract base class for particle identfication using functionals developed by L. Tassan-Got (IPN Orsay)
};

#endif
