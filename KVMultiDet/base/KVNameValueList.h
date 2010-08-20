//Created by KVClassFactory on Tue Jun 29 14:44:22 2010
//Author: bonnet

#ifndef __KVNAMEVALUELIST_H
#define __KVNAMEVALUELIST_H

#include "TList.h"
#include "TNamed.h"
#include "KVString.h"
#include "KVGenParList.h"

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
	
	Bool_t HasParameter(const Char_t* name){
		if (FindTNamed(name)) return kTRUE;
		else return kFALSE;
	}
	
	Int_t GetNameIndex(const Char_t* name){
		TNamed* named = 0;
		if (!(named = FindTNamed(name))){
			Error("GetNameIndex","\"%s\" does not correspond to an existing parameter, default value -1 is returned",name);
			return -1;
		}
		return IndexOf(named);
	}
	
	const Char_t* GetNameAt(Int_t idx){
		TNamed* named = 0;
		if (!(named = (TNamed* )At(idx))){
			Error("GetNameAt","index has to be less than %d, empty string is returned",idx);
			return "";
		}
		return named->GetName();
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
		TNamed* named = 0;
		if (!(named = FindTNamed(name))){
			Error("GetStringValue(const Char_t*)","\"%s\" does not correspond to an existing parameter, default value -1 is returned",name);
			return "-1";
		}
		return named->GetTitle(); 
	}
	Int_t GetNpar(){
		return GetEntries();
	}
	Int_t GetIntValue(Int_t idx){ 
		sconvert.Form("%s",GetStringValue(idx)); 
		return sconvert.Atoi();
	}
	Double_t GetDoubleValue(Int_t idx){ 
		sconvert.Form("%s",GetStringValue(idx)); 
		return sconvert.Atof();
	}
	const Char_t* GetStringValue(Int_t idx){ 
		if (idx>=GetNpar()) {
			Error("GetStringValue(Int_t)","index has to be less than %d, default value -1 is returned",GetNpar());
			return "-1";
		}
		return At(idx)->GetTitle(); 
	}
	
	void Convert(KVGenParList& gen){
		for (Int_t np=0;np<gen.GetNPar();np+=1){
			SetValue(gen.GetParameter(np)->GetName(),gen.GetParameter(np)->GetVal());
		}
	}
	
	void Transfert(KVNameValueList& lnv){
		for (Int_t np=0;np<lnv.GetNpar();np+=1){
			SetValue( lnv.GetNameAt(np), lnv.GetStringValue(np) );
		}
	}
	
   ClassDef(KVNameValueList,1)//list of TNamed
};

#endif
