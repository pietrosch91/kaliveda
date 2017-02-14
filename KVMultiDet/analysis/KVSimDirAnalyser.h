//Created by KVClassFactory on Tue Feb 14 11:13:53 2017
//Author: John Frankland,,,

#ifndef __KVSIMDIRANALYSER_H
#define __KVSIMDIRANALYSER_H

#include "KVDataAnalyser.h"

class KVSimDirAnalyser : public KVDataAnalyser {
public:
   KVSimDirAnalyser();

   virtual ~KVSimDirAnalyser();

   ClassDef(KVSimDirAnalyser, 1) //Analysis of trees containing simulated events
};

#endif
