//Created by KVClassFactory on Tue Jun 29 14:44:22 2010
//Author: bonnet

#ifndef __KVNAMEVALUELIST_H
#define __KVNAMEVALUELIST_H

#include "TList.h"
#include "TNamed.h"
#include "KVString.h"

class KVNameValueList : public TList
{

   protected:
	
	KVString sconvert;
	
	void SetStringValue(TNamed *tn, const Char_t* value){
		tn->SetTitle(value);
	}
	void SetIntValue(TNamed *tn, Int_t value){
		sconvert.Form("%d",value);
		SetStringValue(tn,sconvert.Data());
	}
	void SetDoubleValue(TNamed *tn, Double_t value){
		sconvert.Form("%lf",value);
		SetStringValue(tn,sconvert.Data());
	}
	
	void AddStringValue(const Char_t* name,const Char_t* value){
		Add(new TNamed(name,value));
	}
	void AddIntValue(const Char_t* name,Int_t value){
		sconvert.Form("%d",value);
		AddStringValue(name,sconvert.Data());
	}
	void AddDoubleValue(const Char_t* name,Double_t value){
		sconvert.Form("%lf",value);
		AddStringValue(name,sconvert.Data());
	}
	
	public:
   
	KVNameValueList();
   virtual ~KVNameValueList();

	void SetValue(const Char_t* name,const Char_t* value){
		TNamed* tn = 0;
		if ( !(tn  = FindTNamed(name)) ) AddStringValue(name,value);
		else 	SetStringValue(tn,value);	
	}
	void SetValue(const Char_t* name,Int_t value){
		TNamed* tn = 0;
		if ( !(tn  = FindTNamed(name)) )	AddIntValue(name,value);
		else	SetIntValue(tn,value);	
	}
	
	void SetValue(const Char_t* name,Double_t value){
		TNamed* tn = 0;
		if ( !(tn  = FindTNamed(name)) )	AddDoubleValue(name,value);
		else	SetDoubleValue(tn,value);	
	}
	
	TNamed* FindTNamed(const Char_t* name){
		return (TNamed* )FindObject(name);
	}
	
	Int_t GetNameIndex(const Char_t* name){
		return IndexOf(FindTNamed(name));
	}
	
	Int_t GetIntValue(const Char_t* name){ 
		sconvert.Form("%s",GetStringValue(name)); 
		return sconvert.Atoi();
	}
	Double_t GetDoubleValue(const Char_t* name){ 
		sconvert.Form("%s",GetStringValue(name)); 
		return sconvert.Atof();
	}
	const Char_t* GetStringValue(const Char_t* name){ 
		return FindTNamed(name)->GetTitle(); 
	}

   ClassDef(KVNameValueList,1)//list of TNamed
};

#endif
