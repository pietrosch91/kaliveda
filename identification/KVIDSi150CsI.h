/***************************************************************************
$Id: KVIDSi150CsI.h,v 1.17 2008/04/04 09:06:59 franklan Exp $
                          KVIDSi150CsI.h  -  description
                             -------------------
    begin                : Fri Jul 18 2003
    copyright            : (C) 2003 by John Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVIDSi150CSI_H
#define KVIDSi150CSI_H

#include "KVIDSiCsI.h"
#include "TF1.h"
#include "KVTGIDManager.h"

#define ZMAX_IDT 8

class KVMultiDetArray;

class KVIDSi150CsI:public KVIDSiCsI, public KVTGIDManager {

   Double_t fCorr[ZMAX_IDT + 1][2];     //[ZMAX_IDT+1][2] correction parameters for identification
   Double_t fMaxSiPG;  //maximum SiPG value for identification in modules with saturated coders 
   Int_t fWhichGrid;            //!internal IdentifyZ status code, tells us which Z grid was last used
   
      Double_t fMinLumiere[ZMAX_IDT];//[ZMAX_IDT+1] minimum value of CsI total light for isotopic identification to be valid
      
 public:
   enum {                       //grid identifiers
      k_GG, k_PG
   };

   enum {                       //identification status subcodes
kOK_GG,                    //  "Z&A identification OK in SIGG-CSI"
kOK_PG,                    //  "Z&A identification OK in SIPG-CSI"
kZOK,            // "Z identification OK, Z is > than max Z for which we have isotopic identification"
      kZOK_AOutOfIDRange,       //Z id OK, mass ID attempted but point to identify outside of identification range of KVTGIDZA
      kZOK_A_ZtooSmall,         //Z id ok, mass ID attempted but Z passed to IdentA too small (<1)
      kZOK_A_ZtooLarge,         //Z id ok, mass ID attempted but Z passed to IdentA larger than max Z defined by KVTGIDZA      
kZOK_badA,  //  "Z identification OK. A not in KVNucleus mass table for this Z, ignored (A calculated from Z)"
kZOK_bigZ,   // "Z identification OK. Z found greater than last complete Z-line in the SIPG-CSI matrix"
kZOK_badFit,  //  "Z identification OK. Mass-fit is bad for measured CsI total light (A calculated from Z)"
kZmin_coder,     //        "Module with coder saturation. SiPG>max canal for which ID possible. Z given is a minimum"
 kNoID       //      "No Z identification. Bad news"
   };

    KVIDSi150CsI() {
       fMaxSiPG = 5000.; // will have no effect if not set to a more realistic value by SetIdentficationParameters
       //set minimum value of CsI total light for isotopic identification to be valid
       //to zero for all Z.
       for(int i=0;i<ZMAX_IDT;i++) fMinLumiere[i] = 0.0;
   };
   virtual ~ KVIDSi150CsI() {
   };

   Int_t LastFitTried()
   {
      //Returns either KVIDSi150CsI::k_GG or KVIDSi150CsI::k_PG
      //corresponding to whether the last fit tried to obtain Z identification
      //was in Grand Gain or Petit Gain
      return fWhichGrid;
   };
   //Set minimum value of CsI total light for isotopic identification
   //to be valid for a given Z
   void SetMinLumiere(Int_t z, Double_t Lumiere){
      if(z>0 && z<=ZMAX_IDT) fMinLumiere[z-1] = Lumiere;
   };
   //Get minimum value of CsI total light for isotopic identification
   //to be valid for a given Z
   Double_t GetMinLumiere(Int_t z){
      return fMinLumiere[z-1];
   };
   
   Double_t IdentifyZ(Double_t & funLTG);
   Double_t IdentifyA(Int_t, Double_t & funLTG);

   void SetCorrParam(Int_t Z, Int_t i, Double_t par) {
      fCorr[Z][i] = par;
   };

   Bool_t Identify(KVReconstructedNucleus *);

   void Print(Option_t * opt = "") const;
   Double_t GetIDMapX(Option_t * opt = "");
   Double_t GetIDMapY(Option_t * opt = "");
   KVIDGrid *GetTGIDGrid(const Char_t * tgid_name, Double_t xmax,
                         Double_t xmin = 0.0, Int_t ID_min =
                         0, Int_t ID_max = 0, Int_t npoints = 100, Bool_t logscale=kFALSE);
   KVIDGrid *GetTGIDGrid(const Char_t * idt_name, const Char_t * id_type,
                         const Char_t * grid_type, Double_t xmax,
                         Double_t xmin = 0.0, Int_t ID_min =
                         0, Int_t ID_max = 0, Int_t npoints = 100, Bool_t logscale=kFALSE);
   Double_t IdentA(KVIDTelescope * idt, Double_t & funLTG,
                   const Char_t * grid_type, const Char_t * signal_type, Int_t Z);

   const Char_t *GetIDSubCodeString(KVIDSubCode & concat) const;
   virtual Bool_t SetIdentificationParameters(const KVMultiDetArray*);
   virtual void RemoveIdentificationParameters();
   
   void SetMaxCanalSiPG( Double_t max ) {
      Info("SetMaxCanalSiPG", "%s : Setting maximum SIPG channel = %d", GetName(), (int)max);
      fMaxSiPG = max;
   };

   virtual void Initialize();
   
   ClassDef(KVIDSi150CsI, 4)    //Si(150u)-CsI identification telescopes of the 5th campaign
};

#endif
