/*
$Id: KVINDRADataAnalyser.h,v 1.5 2007/05/31 09:59:22 franklan Exp $
$Revision: 1.5 $
$Date: 2007/05/31 09:59:22 $
*/

//Created by KVClassFactory on Wed Apr  5 23:50:04 2006
//Author: John Frankland

#ifndef __KVINDRADATAANALYSER_H
#define __KVINDRADATAANALYSER_H

#include "KVDataAnalyser.h"
#include "KVNumberList.h"
#include "KVString.h"
#include "TTree.h"

class KVINDRADataAnalyser:public KVDataAnalyser {

 protected:
       
   virtual KVNumberList PrintAvailableRuns(KVString & datatype);

 public:

   KVINDRADataAnalyser();
   virtual ~ KVINDRADataAnalyser();
	void WriteBatchInfo(TTree* );
	
   ClassDef(KVINDRADataAnalyser, 0) //For analysing INDRA data
};

#endif
