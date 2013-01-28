//Created by KVClassFactory on Mon Sep 17 17:49:41 2012
//Author: Dijon Aurore

#ifndef __KVIVDB_H
#define __KVIVDB_H

#include "KVINDRADB.h"
using namespace std;

class KVIVDB : public KVINDRADB
{
   protected:
	   KVDBTable *fVAMOSCalib;        // Table of VAMOS detector calibration parameters
   virtual void ReadVamosBrhoAndAngle () ;
   virtual void ReadVamosCalibrations();
   virtual void ReadVamosScalers () ;
   virtual Bool_t ReadVamosCalibFile(ifstream &infile);


   void init();
   public:
   KVIVDB();
   KVIVDB(const Char_t *);
   virtual ~KVIVDB();

   void Build () ;

   ClassDef(KVIVDB,1)//Database for e494s experiment coupling INDRA with VAMOS
};

#endif
