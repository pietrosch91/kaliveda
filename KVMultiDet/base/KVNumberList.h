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
   
	void init_numberlist();
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
   
	void SetList(TString &);
   void SetList(const Char_t *);

   void Add(Int_t);
   void Remove(Int_t);
	
	void Add(const Char_t*);
	void Remove(const Char_t*);
   
	void Add(const KVNumberList&);
  	void Remove(const KVNumberList&);
	
	void Add(Int_t, Int_t *);
	void Remove(Int_t n, Int_t * arr);
   
	void SetMinMax(Int_t min, Int_t max, Int_t pas=1);
	
	void Inter(const KVNumberList& list);

   Bool_t Contains(Int_t val) const;
   Int_t First();
   Int_t Last();
   Int_t At(Int_t index);
   Int_t operator[](Int_t index);
   Int_t *GetArray(Int_t & size);
   const Char_t *GetList();
   const Char_t *GetExpandedList();
   TString GetLogical(const Char_t *observable);
	const Char_t *AsString(Int_t maxchars=0);
   
   Int_t Next(void);
   void Begin(void);
   Bool_t End(void) const { return fEndList; };
   
   Bool_t IsEmpty() const {
      return (fNValues == 0);
   };
   
	void Clear(Option_t* = "");
   Int_t GetNValues();
   Int_t GetEntries()
   {
   	return GetNValues();
   };

   void PrintLimits();
   Bool_t IsFull(Int_t vinf=-1,Int_t vsup=-1);
	KVNumberList GetComplementaryList();
   KVNumberList GetSubList(Int_t vinf,Int_t vsup);
   
	KVNumberList & operator=(const KVNumberList &);
	KVNumberList operator-(const KVNumberList &);
	
	   // Type conversion
   operator const char*() const { return const_cast<KVNumberList*>(this)->GetList(); }

   ClassDef(KVNumberList, 3)    //Strings used to represent a set of ranges of values
};

#endif
