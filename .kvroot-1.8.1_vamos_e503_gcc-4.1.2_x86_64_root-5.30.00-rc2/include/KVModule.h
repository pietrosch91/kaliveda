/***************************************************************************
                          kvmodule.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVModule.h,v 1.4 2003/08/23 09:17:06 franklan Exp $
 ***************************************************************************/
/**************************************************************************/
/* Definition de la classe module   pour le commande et controle INDRA    */
/* Premier test                                                           */
/* D.Cussol                                                               */
/* 11/02/97                                                               */
/**************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVMODULE_H
#define KVMODULE_H

#ifndef KVD_RECPRC_CNXN
#define KVD_RECPRC_CNXN 1
#endif
#ifndef KVD_NORECPRC_CNXN
#define KVD_NORECPRC_CNXN 0
#endif

#include "KVBase.h"

class KVList;
class KVRList;
class KVDetector;

class KVModule:public KVBase {
 private:
   Long_t fBaie;
   Long_t fChassis;
   Long_t fSlot;
   Long_t fNbre_de_voies;
   //ensemble<registre> *regs;
   KVList *fRegisters;          //->list of all registers in module
   KVRList *fDetectors;         //list of all detectors connected to a module
 public:
    KVModule();
    virtual ~ KVModule();
   void ConnectDetector(KVDetector *, const int fcon = KVD_RECPRC_CNXN);

// Lecture du numero de baie
   Long_t GetBaie(void) {
      return fBaie;
   };
// Lecture du numero de chassis
   Long_t GetChassis(void) {
      return fChassis;
   };
// Lecture du numero de slot
   Long_t GetSlot(void) {
      return fSlot;
   };
// Lecture du nombre de voies
   Long_t GetNbreDeVoies(void) {
      return fNbre_de_voies;
   };
// Ecriture du numero de baie
   void SetBaie(Long_t b) {
      fBaie = b;
   };
// Ecriture du numero de chassis
   void SetChassis(Long_t c) {
      fChassis = c;
   };
// Ecriture du numero de slot
   void SetSlot(Long_t s) {
      fSlot = s;
   };
// Ecriture du nombre de voies
   void SetNbreDeVoies(Long_t n) {
      fNbre_de_voies = n;
   };
// Ecriture de l'ensemble regs
   void SetRegs(KVList * r) {
      fRegisters = r;
   };
// Lecture de l'ensemble regs
   KVList *GetRegs() {
      return fRegisters;
   };

   ClassDef(KVModule, 1)        //Base class for electronic modules, power supply, etc
};

#endif
