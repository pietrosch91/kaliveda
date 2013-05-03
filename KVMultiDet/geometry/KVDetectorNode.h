//Created by KVClassFactory on Fri Apr 26 12:45:15 2013
//Author: John Frankland,,,

#ifndef __KVDETECTORNODE_H
#define __KVDETECTORNODE_H

#include "KVBase.h"
class KVSeqCollection;
class KVDetector;

class KVDetectorNode : public KVBase
{
    KVSeqCollection* fInFront;//list of detectors in front
    KVSeqCollection* fBehind;//list of detectors behind

    void init();

   public:
   KVDetectorNode();
   KVDetectorNode(const Char_t* name);
   virtual ~KVDetectorNode();

   void AddInFront(KVDetector*);
   void AddBehind(KVDetector*);
   Bool_t IsInFrontOf(KVDetector*);
   Bool_t IsBehind(KVDetector*);
   KVSeqCollection* GetDetectorsInFront() const { return fInFront; }
   KVSeqCollection* GetDetectorsBehind() const { return fBehind; }
   Int_t GetNDetsInFront() const;
   Int_t GetNDetsBehind() const;

   void ls(Option_t *option="") const;

   ClassDef(KVDetectorNode,1)//Stores lists of detectors in front and behind this node
};

#endif
