#ifndef KVGROUP_H
#define KVGROUP_H

#define KVGROUP_ADD_UNKNOWN_TELESCOPE "Attempt to add undefined telescope to group"

#include "KVPosition.h"
#include "KVList.h"
#include "KVTelescope.h"

class KVDetector;
class KVNucleus;
class KVNameValueList;
class KVReconstructedEvent;
class KVReconstructedNucleus;
class KVMultiDetArray;

class KVGroup : public KVBase {
 protected:
    enum {
       kIsRemoving = BIT(14)     //flag set during call to RemoveTelescope
    };
   KVList * fTelescopes;       //->Sorted list of telescopes belonging to the group.
   KVList *fDetectors;          //->list of all detectors in group
   KVList *fReconstructedNuclei;        //!Particles reconstructed in this group
	
 public:
   enum {
      kForwards,
      kBackwards
   };
    KVGroup();
   void init();
    virtual ~ KVGroup();
   virtual void SetNumber(UInt_t num) {
      // Setting number for group also sets name to "Group_n"
      SetName( Form("Group_%u", num) );
      KVBase::SetNumber(num);
   };
   void AddDetector(KVDetector *);
   void Add(KVDetector *);
   KVList *GetTelescopes() const {
      return fTelescopes;
   };
   virtual UInt_t GetNumberOfDetectorLayers();
   void AddTelescope(KVTelescope *);
   void SetTelescopes(KVList * list);
   void Reset();
   virtual void Print(Option_t * t = "") const;
   KVDetector *GetDetector(const Char_t * name);
   virtual TList *GetDetectorsInLayer(UInt_t lay);
   KVTelescope *GetTelescope(const Char_t * name);
   virtual TList *GetAlignedDetectors(KVDetector *, UChar_t dir = kBackwards);

   inline UInt_t GetHits() {
      if (fReconstructedNuclei)
         return fReconstructedNuclei->GetSize();
      else
         return 0;
   };
   void ClearHitDetectors();
   inline UInt_t GetNIdentified();
   inline UInt_t GetNUnidentified();
   KVList *GetParticles() {
      return fReconstructedNuclei;
   }
   void AddHit(KVReconstructedNucleus * kvd);
   void RemoveHit(KVReconstructedNucleus * kvd);
   Bool_t Contains(KVDetector *) const;

  virtual void GetIDTelescopes(TCollection *);

   Bool_t IsRemoving() {
      return TestBit(kIsRemoving);
   }
   virtual void Sort(){};
   virtual void CountLayers(){};

   KVList *GetDetectors() const {return fDetectors; }

   void AnalyseParticles();
   inline virtual Bool_t Fired(Option_t * opt = "any") const;
    void PrepareModif(KVDetector* );
    virtual void AnalyseAndReconstruct(KVReconstructedEvent *);
   void Destroy();
   void RemoveTelescope(KVTelescope * tel, Bool_t kDeleteTelescope =
                        kFALSE, Bool_t kDeleteEmptyGroup = kTRUE);

   ClassDef(KVGroup, 1)//Group of detectors having similar angular positions.
};

inline Bool_t KVGroup::Fired(Option_t * opt) const
{
   //returns kTRUE if at least one detector in group has KVDetector::Fired(opt) = kTRUE (see KVDetector::Fired() method for options)
   KVDetector *tel;
   TIter nxt(fDetectors);
   while ((tel = (KVDetector *) nxt()))
      if (tel->Fired(opt))
         return kTRUE;
   return kFALSE;
}

#ifndef KVRECONSTRUCTEDNUCLEUS_H
#include "KVReconstructedNucleus.h"
#endif
inline UInt_t KVGroup::GetNIdentified()
{
   //number of identified particles reconstructed in group
   UInt_t n = 0;
   if (GetHits()) {
      TIter next(fReconstructedNuclei);
      KVReconstructedNucleus *nuc = 0;
      while ((nuc = (KVReconstructedNucleus *) next()))
         n += (UInt_t) nuc->IsIdentified();
   }
   return n;
};
inline UInt_t KVGroup::GetNUnidentified()
{
   //number of unidentified particles reconstructed in group
   return (GetHits() - GetNIdentified());
};

#endif
