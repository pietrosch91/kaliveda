//Created by KVClassFactory on Thu May 30 10:13:17 2013
//Author: Eric Bonnet

#ifndef __FAZIABLOCK_H
#define __FAZIABLOCK_H

#include "TGeoVolume.h"

class FAZIABlock : public TGeoVolumeAssembly {

   Double_t fTotSidWBlind;
public:
   FAZIABlock();
   virtual ~FAZIABlock();

   Double_t GetTotalSideWithBlindage() const {
      // Total length of one side of block including blindage
      return fTotSidWBlind;
   }

   ClassDef(FAZIABlock, 1) //Block of 16 telescopes FAZIA
};

#endif
