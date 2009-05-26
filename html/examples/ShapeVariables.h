/*
$Id: ShapeVariables.h,v 1.1 2007/11/30 13:45:14 franklan Exp $
$Revision: 1.1 $
$Date: 2007/11/30 13:45:14 $
*/

//Created by KVClassFactory on Wed Nov 28 13:22:45 2007
//Author: franklan

#ifndef __SHAPEVARIABLES_H
#define __SHAPEVARIABLES_H

#include "KVSelector.h"
#include "TFile.h"
#include "TTree.h"

class ShapeVariables : public KVSelector {

   TFile* file;
   TTree* tree;
   
   public:
   ShapeVariables() {};
   virtual ~ShapeVariables() {};
   
   virtual void InitRun();
   virtual void EndRun();
   virtual void InitAnalysis();
   virtual Bool_t Analysis();
   virtual void EndAnalysis();

     ClassDef(ShapeVariables,1)//Analysis class using automatic generation of branches for global variables
};


#endif
