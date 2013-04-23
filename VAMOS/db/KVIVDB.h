//Created by KVClassFactory on Mon Sep 17 17:49:41 2012
//Author: Dijon Aurore

#ifndef __KVIVDB_H
#define __KVIVDB_H

#include "KVINDRADB.h"
using namespace std;

class KVIVDB : public KVINDRADB
{
   	protected:
	   	KVDBTable *fDeltaPed;   //-> table of INDRA detector pedestal corrections
	   	KVDBTable *fVAMOSCalConf; //-> table of VAMOS detector calibration and configuration parameters

   		virtual void   ReadDeltaPedestal(ifstream &ifile);
	   	virtual void   ReadPedestalCorrection();
   		virtual void   ReadPedestalList () ;
   		virtual Bool_t ReadVamosCalibFile(ifstream &ifile);
   		virtual void   ReadVamosCalibrations();
   		virtual void   ReadVamosScalers () ;


   		void init();
   	public:
   		KVIVDB();
   		KVIVDB(const Char_t *);
   		virtual ~KVIVDB();

   		void Build () ;

   		ClassDef(KVIVDB,1)//Database for e494s experiment coupling INDRA with VAMOS
};

#endif
