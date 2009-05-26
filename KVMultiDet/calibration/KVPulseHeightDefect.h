/*
$Id: KVPulseHeightDefect.h,v 1.2 2008/01/24 10:09:38 franklan Exp $
$Revision: 1.2 $
$Date: 2008/01/24 10:09:38 $
*/

//Created by KVClassFactory on Mon Feb 19 17:32:55 2007
//Author: franklan

#ifndef __KVPULSEHEIGHTDEFECT_H
#define __KVPULSEHEIGHTDEFECT_H

#include <KVCalibrator.h>

Double_t PHDMoulton(Double_t * x, Double_t * par);

class KVPulseHeightDefect : public KVCalibrator
{
   static TF1 fMoulton;           //Moulton formula for PHD =  f(E,Z)
   UInt_t fZ;                   //!Z of nucleus to be calibrated

   void SetPars();
   
   public:

   void init();
   
   KVPulseHeightDefect();
   KVPulseHeightDefect(KVDetector*);
   virtual ~KVPulseHeightDefect();

   virtual Double_t Compute(Double_t E) const;
   virtual Double_t operator() (Double_t );
   virtual Double_t Invert(Double_t);

   void SetZ(UInt_t z) {
      fZ = z;
   };
   UInt_t GetZ() const {
      return fZ;
   };
   
   ClassDef(KVPulseHeightDefect,1)//Silicon PHD described by Moulton formula
};

#endif
