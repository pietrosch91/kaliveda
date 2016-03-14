//Created by KVClassFactory on Fri Apr 26 12:45:15 2013
//Author: John Frankland,,,

#ifndef __KVDETECTORNODE_H
#define __KVDETECTORNODE_H

#include "KVBase.h"
class KVSeqCollection;
class KVDetector;

class KVGeoDetectorNode : public KVBase {
   KVSeqCollection* fInFront;//list of detectors in front
   KVSeqCollection* fBehind;//list of detectors behind

   void init();

public:
   KVGeoDetectorNode();
   KVGeoDetectorNode(const Char_t* name);
   virtual ~KVGeoDetectorNode();

   void AddInFront(KVDetector*);
   void AddBehind(KVDetector*);
   Bool_t IsInFrontOf(KVDetector*);
   Bool_t IsBehind(KVDetector*);
   KVSeqCollection* GetDetectorsInFront() const
   {
      return fInFront;
   }
   KVSeqCollection* GetDetectorsBehind() const
   {
      return fBehind;
   }
   Int_t GetNDetsInFront() const;
   Int_t GetNDetsBehind() const;

   void RehashLists();

   void ls(Option_t* option = "") const;

   const Char_t* GetFullPathToNode() const
   {
      // Return full path to this node in ROOT geometry, i.e. "/TOP/A_1/B_2/MY_DETECTOR"
      // This information is set by KVGeoImport on creation
      return GetTitle();
   }

   ClassDef(KVGeoDetectorNode, 1) //Stores lists of detectors in front and behind this node
};

#endif
