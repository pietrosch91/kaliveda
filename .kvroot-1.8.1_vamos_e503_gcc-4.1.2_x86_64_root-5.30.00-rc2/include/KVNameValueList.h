//Created by KVClassFactory on Tue Jun 29 14:44:22 2010
//Author: bonnet

#ifndef __KVNAMEVALUELIST_H
#define __KVNAMEVALUELIST_H

#include "KVHashList.h"
#include "TNamed.h"
#include "KVString.h"

class KVNameValueList
{

   protected:
	
	KVString sfmt;//!
	KVString sconvert;//!
	KVHashList* khl;//->
	
	void SetStringValue(TNamed *tn, const Char_t* value);
	void SetIntValue(TNamed *tn, Int_t value);
	void SetDoubleValue(TNamed *tn, Double_t value);
	void AddStringValue(const Char_t* name,const Char_t* value);
	void AddIntValue(const Char_t* name,Int_t value);
	void AddDoubleValue(const Char_t* name,Double_t value);
	KVString DoubleToString(Double_t value);
	
	public:
   
	KVNameValueList();
   virtual ~KVNameValueList();
	
	static void TestConversion(Double_t val);
	
	KVHashList* GetList() const;

	virtual void Clear_NVL(Option_t* opt = "");
	virtual void Print_NVL(Option_t* opt = "") const;
	void Write(const Char_t* name);
	virtual void SetName_NVL(const char* name);
	virtual const char* GetName_NVL() const;
	virtual const char* GetTitle_NVL() const;
	void SetOwner_NVL(Bool_t enable = kTRUE);
	Bool_t IsOwner_NVL() const;
	
	void Copy(KVNameValueList& nvl) const;
	Int_t Compare(const KVNameValueList* nvl) const;
	
	void SetValue(const Char_t* name,const Char_t* value);
	void SetValue(const Char_t* name,Int_t value);
	void SetValue(const Char_t* name,Double_t value);
	
	TNamed* FindTNamed(const Char_t* name) const;
	void RemoveParameter(const Char_t* name);
	Bool_t HasParameter(const Char_t* name);
	Int_t GetNameIndex(const Char_t* name);
	const Char_t* GetNameAt(Int_t idx) const;
	Int_t GetNpar() const;
	
	Int_t GetIntValue(const Char_t* name);
	Double_t GetDoubleValue(const Char_t* name);
	const Char_t* GetStringValue(const Char_t* name) const;
	
	Int_t GetIntValue(Int_t idx);
	Double_t GetDoubleValue(Int_t idx);
	const Char_t* GetStringValue(Int_t idx) const;
	
	ClassDef(KVNameValueList,2)//KVHashList of TNamed
};

#endif
