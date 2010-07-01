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
		Add(tn);
	}
	void SetIntValue(TNamed *tn, Int_t value){
		sconvert.Form("%d",value);
		SetStringValue(tn,sconvert.Data());
	}
	void SetDoubleValue(TNamed *tn, Double_t value){
		sconvert.Form("%lf",value);
		SetStringValue(tn,sconvert.Data());
	}
	
	
	public:
   
	KVNameValueList();
   virtual ~KVNameValueList();

	void SetValue(const Char_t* name,const Char_t* value){
		TNamed* tn = 0;
		if ( !(tn  = FindTNamed(name)) ) 
			tn = new TNamed(name,"");
		SetStringValue(tn,value);	
	}
	void SetValue(const Char_t* name,Int_t value){
		TNamed* tn = 0;
		if ( !(tn  = FindTNamed(name)) ) 
			tn = new TNamed(name,"");
		SetIntValue(tn,value);	
	}
	void SetValue(const Char_t* name,Double_t value){
		TNamed* tn = 0;
		if ( !(tn  = FindTNamed(name)) ) 
			tn = new TNamed(name,"");
		SetDoubleValue(tn,value);	
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
