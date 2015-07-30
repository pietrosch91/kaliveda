/*******************************************************************************
$Id: KVNumberList.h,v 1.20 2009/03/03 14:27:15 franklan Exp $
$Revision: 1.20 $
$Date: 2009/03/03 14:27:15 $
$Author: franklan $
*******************************************************************************/

#ifndef __KVNUMBERLIST_H
#define __KVNUMBERLIST_H

#include <TString.h>
#include <TArrayI.h>
#include <TObject.h>

class KVNumberList : public TObject {

   TString fString;
   TArrayI *fLowerBounds;       //->
   TArrayI *fUpperBounds;       //->
   Int_t fNLimits;              //number of limits in arrays
   Int_t fMaxNLimits;           //size of arrays
   Int_t fFirstValue;           //smallest value included in list
   Int_t fLastValue;            //largest value included in list
   Int_t fNValues;              //total number of values included in ranges

   Int_t fIterIndex;//! used by Next() to iterate over list
   Bool_t fEndList;//! used by Next() & End() to iterate over list
   Int_t* fValues;//! used by Next() to iterate over list
   TString   fName;//name of the list

   Bool_t fIsParsed;//!
   
   void init_numberlist();
   void clear();
   void ParseList();
   void AddLimits(Int_t min, Int_t max);
   void AddLimits(TString & string);
   void ParseAndFindLimits(TString & string, const Char_t delim);

   public:

   KVNumberList();
   KVNumberList(const KVNumberList &);
   KVNumberList(const Char_t *);
   KVNumberList(Int_t deb,Int_t fin, Int_t pas);
   virtual ~ KVNumberList();

   virtual void     SetName(const char *name);
   virtual const char *GetName() const{
      return fName.Data();
   }
   
   /* LIST MODIFIERS */
   /* Set the number list */
   void Set(const TString& l) { SetList(l); }
   void Set(Int_t min, Int_t max, Int_t pas=1) { SetMinMax(min,max,pas); }
   void SetList(const TString&);
   void SetMinMax(Int_t min, Int_t max, Int_t pas=1);
   KVNumberList & operator=(const KVNumberList &);
   void Copy(TObject &) const;

   /* Add numbers/lists to the list */
   void Add(Int_t);
   void Add(const Char_t*);
   void Add(const KVNumberList&);
   void Add(Int_t, Int_t *);

   /* Remove numbers/lists from the list */
   void Remove(Int_t);
   void Remove(const Char_t*);
   void Remove(const KVNumberList&);
   void Remove(Int_t n, Int_t * arr);

   /* Clear the list */
   void Clear(Option_t* = "");

   /* Intersection of two sets of numbers i.e. those which are in both lists */
   void Inter(const KVNumberList& list);

   /* LIST PROPERTIES */
   Bool_t Contains(Int_t val) const;
   Int_t First();
   Int_t Last();
   Int_t GetNValues();
   Int_t GetEntries()
   {
      return GetNValues();
   };
   Bool_t IsEmpty() const {
      if(!fIsParsed) const_cast<KVNumberList*>(this)->ParseList();
      return (fNValues == 0);
   };
   Bool_t IsFull(Int_t vinf=-1,Int_t vsup=-1);

   /* LIST MEMBER ACCESS */
   Int_t At(Int_t index);
   Int_t operator[](Int_t index);
   Int_t *GetArray(Int_t & size);
   const Char_t *GetList();
   const Char_t *GetExpandedList();
   const Char_t *AsString(Int_t maxchars=0);
   
   /* LIST ITERATORS */
   Int_t Next(void);
   void Begin(void);
   Bool_t End(void) const { return fEndList; };
      
   /* LIST ARITHMETIC OPERATIONS */
   KVNumberList operator+(const KVNumberList &);
   KVNumberList operator-(const KVNumberList &);
   KVNumberList GetComplementaryList();
   KVNumberList GetSubList(Int_t vinf,Int_t vsup);

   /* MISCELLANEOUS */
   /* Generate TTree::Draw selection string */
   TString GetLogical(const Char_t *observable);
   /* Convert to c-string */
   operator const char*() const { return const_cast<KVNumberList*>(this)->GetList(); }
   /* Print list properties */
   void Print(Option_t* = "") const;
   void PrintLimits();

   ClassDef(KVNumberList, 3)    //Strings used to represent a set of ranges of values
};

#endif
