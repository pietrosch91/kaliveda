//Created by KVClassFactory on Thu Sep 13 10:51:51 2012
//Author: Guilain ADEMARD

#ifndef __KVVAMOSRECONNUC_H
#define __KVVAMOSRECONNUC_H

#include "KVReconstructedNucleus.h"

class KVVAMOSReconNuc : public KVReconstructedNucleus
{

   public:
   KVVAMOSReconNuc();
   KVVAMOSReconNuc (const KVVAMOSReconNuc&) ;
   virtual ~KVVAMOSReconNuc();
   void Copy (TObject&) const;

   ClassDef(KVVAMOSReconNuc,1)//Nucleus identified by VAMOS spectrometer
};

#endif
