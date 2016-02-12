/***************************************************************************
                          kvdetectedparticle.h  -  description
                             -------------------
    begin                : Thu Oct 10 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVFAZIAReconNuc.h,v 1.39 2009/04/03 14:28:37 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVFAZIAReconNuc_H
#define KVFAZIAReconNuc_H


#include "KVReconstructedNucleus.h"

class KVFAZIADetector;

class KVFAZIAReconNuc: public KVReconstructedNucleus {

protected:
   Float_t fECsI;//csi contribution to energy
   Float_t fESi1;//si1 contribution to energy
   Float_t fESi2;//si2 contribution to energy

   virtual void MakeDetectorList();

public:

   KVFAZIAReconNuc();
   KVFAZIAReconNuc(const KVFAZIAReconNuc&);
   void init();
   virtual ~ KVFAZIAReconNuc();

   virtual void Copy(TObject&) const;

   virtual void Clear(Option_t* t = "");
   void Print(Option_t* option = "") const;
   virtual void Identify();
   virtual void Calibrate();

   Float_t GetEnergySi1()
   {
      // Return the calculated SI1 contribution to the particle's energy
      //
      // This may be negative, in case the SI1 contribution was calculated
      // because either (1) the SI1 was not calibrated, or (2) coherency check
      // between all identifications indicates contribution
      // of several particles to SI1 energy

      return fESi1;
   };
   Float_t GetEnergySi2()
   {
      // Return the calculated SI2 contribution to the particle's energy
      //
      // This may be negative, in case the SI2 contribution was calculated
      // because all identifications indicates contribution
      // of several particles to SI2 energy

      return fESi2;
   };
   Float_t GetEnergyCsI()
   {
      // Return the calculated CsI contribution to the particle's energy
      return fECsI;
   };

   KVFAZIADetector* Get(const Char_t* label) const;
   KVFAZIADetector* GetSI1() const;
   KVFAZIADetector* GetSI2() const;
   KVFAZIADetector* GetCSI() const;

   Int_t GetIndex() const;
   Int_t GetIdentifierOfStoppingDetector() const;

   Bool_t StoppedIn(const Char_t* dettype) const;
   Bool_t StoppedInSI1() const;
   Bool_t StoppedInSI2() const;
   Bool_t StoppedInCSI() const;

   ClassDef(KVFAZIAReconNuc, 2) //Nucleus identified by FAZIA array
};

#endif
