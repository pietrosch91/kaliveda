/*
$Id: Analysisv_e494s.h,v 1.2 2007/06/08 15:49:10 franklan Exp $
$Revision: 1.2 $
$Date: 2007/06/08 15:49:10 $
*/

//Created by KVClassFactory on Fri Jun  8 15:26:12 2007
//Author: e_ivamos

#ifndef __ANALYSISV_E494S_H
#define __ANALYSISV_E494S_H

#include"Defines.h"

#include "Random.h"

#define SED1
#define SED2
#define IONCHAMBER
#define SI

#ifdef SED1
#ifdef SED2
#define SED12
#endif
#endif

#include"IonisationChamberv.h"
#include"Siv.h"
#include"SeDv.h"
#include"SeD12v.h"
#include"ReconstructionSeDv.h"

#include "Analysisv.h"

class Analysisv_e494s : public Analysisv {
public:

   SeDv* SeD1;
   SeDv* SeD2;
   SeD12v* SeD12;
   ReconstructionSeDv* RC;
   IonisationChamberv* Ic;
   Siv* Si;

   UShort_t T_Raw[10];

   Analysisv_e494s(LogFile* Log);
   virtual ~Analysisv_e494s(void);

   void inAttach(); //Attaching the variables
   void outAttach(); //Attaching the variables
   void Treat(); // Treating data
   void CreateHistograms();
   void FillHistograms();

   ClassDef(Analysisv_e494s, 0) //VAMOS calibration for E494S

};

#endif
