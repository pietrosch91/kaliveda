/***************************************************************************
                          kvindra.h  -  description
                             -------------------
    begin                : Mon May 20 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVINDRA.h,v 1.43 2009/01/21 10:05:51 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVINDRA_H
#define KVINDRA_H

#include "KVMultiDetArray.h"
#include "KVList.h"
#include "KVACQParam.h"
#include "KVINDRADB.h"
#include "KVINDRADBRun.h"
#include "KVDBSystem.h"
#include "KVUpDater.h"
#include "KVDataSetManager.h"

class KVLayer;
class KVNucleus;
class KVChIo;
class KVDetectorEvent;
class KVINDRAReconEvent;

//old BaseIndra type definitions
enum EBaseIndra_type {
   ChIo_GG = 1,
   ChIo_PG,                     //=2
   ChIo_T,                      //=3
   Si_GG,                       //=4
   Si_PG,                       //=5
   Si_T,                        //=6
   CsI_R,                       //=7
   CsI_L,                       //=8
   CsI_T,                       //=9
   Si75_GG,                     //=10
   Si75_PG,                     //=11
   Si75_T,                      //=12
   SiLi_GG,                     //=13
   SiLi_PG,                     //=14
   SiLi_T                       //=15
};
enum EBaseIndra_typePhos {
   Phos_R = 1,
   Phos_L,                      //=2
   Phos_T,                      //=3
};

class KVINDRA:public KVMultiDetArray {

 public:
   static Char_t SignalTypes[16][3];    //Use this static array to translate EBaseIndra_type signal type to a string giving the signal type


 private:
    UChar_t fTrigger;           //multiplicity trigger used for acquisition


 protected:
    KVList * fChIo;             //->List Of ChIo of INDRA
   KVList *fSi;                 //->List of Si detectors of INDRA
   KVList *fCsI;                //->List of CsI detectors of INDRA
   KVList *fPhoswich;           //->List of NE102/NE115 detectors of INDRA

   KVLayer *fChIoLayer;         //Reference to ChIo layer of INDRA

   Bool_t fPHDSet;//set to kTRUE if pulse height defect parameters are set
   
   virtual void MakeListOfDetectorTypes();
   virtual void MakeListOfDetectors();
   virtual void PrototypeTelescopes();
   virtual void BuildGeometry();
   virtual void SetGroupsAndIDTelescopes();
   void FillListsOfDetectorsByType();

 public:
    KVINDRA();
    virtual ~ KVINDRA();

   virtual void Build();
   virtual Bool_t ArePHDSet() const { return fPHDSet; };
   virtual void PHDSet(Bool_t yes=kTRUE) { fPHDSet=yes; };

   virtual void GetIDTelescopes(KVDetector *, KVDetector *, KVList *);

   KVLayer *GetChIoLayer();
   inline KVList *GetListOfChIo() const {
      return fChIo;
   };
   inline KVList *GetListOfSi() const {
      return fSi;
   };
   inline KVList *GetListOfCsI() const {
      return fCsI;
   };
   inline KVList *GetListOfPhoswich() const {
      return fPhoswich;
   };

   virtual KVChIo *GetChIoOf(const Char_t * detname);
   virtual KVChIo *GetChIoOf(KVDetector * kvd);
   void SetNamesChIo();
   virtual void cd(Option_t * option = "");
   virtual KVDetector *GetDetectorByType(UInt_t cou, UInt_t mod,
                                         UInt_t type) const;
   virtual KVDetector *GetDetector(const Char_t * name) const;

   void SetTrigger(UChar_t trig);
   // Methodes pour interroger la base de donnees
   inline KVINDRADBRun *GetCurrentRun() const;
   inline Int_t GetTrigger() const;
   inline KVDBSystem *GetSystem() const;
   inline void ShowSystem() const;
	
	void SetPinLasersForCsI();

    ClassDef(KVINDRA, 4)        //class describing the materials and detectors etc. to build an INDRA multidetector array
};

//................  global variable
R__EXTERN KVINDRA *gIndra;

//................ inline functions
inline void KVINDRA::cd(Option_t *)
{
   gIndra = this;
}

inline KVINDRADBRun *KVINDRA::GetCurrentRun() const
{

   if (!gIndraDB) {
      Warning("GetCurrentRun()", "DataBase not initialized !");
      return 0;
   }
   KVINDRADBRun *run = gIndraDB->GetRun(GetCurrentRunNumber());
   if (!run) {
      Warning("GetCurrentRun()", "Run %u not found !",
              GetCurrentRunNumber());
   }
   return run;
}

//------------- inline

inline Int_t KVINDRA::GetTrigger() const
{

   if (!gIndraDB) {
      return (Int_t) fTrigger;
   }
   KVINDRADBRun *run = GetCurrentRun();
   if (run) {
      return run->GetTrigger();
   }
   return 0;
}

inline KVDBSystem *KVINDRA::GetSystem() const
{

   KVINDRADBRun *run = GetCurrentRun();
   if (run) {
      return run->GetSystem();
   }
   return 0;
}

inline void KVINDRA::ShowSystem() const
{

   KVDBSystem *sys = GetSystem();
   if (sys) {
      cout << sys->GetName() << endl;
   } else {
      Warning("ShowSystem", "No system found for run");
   }
}

#endif
