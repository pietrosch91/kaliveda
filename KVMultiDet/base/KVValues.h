//Created by KVClassFactory on Wed May 19 17:23:29 2010
//Author: bonnet

#ifndef __KVVALUES_H
#define __KVVALUES_H

#include "TNamed.h"
#include "TMath.h"
#include "TFormula.h"

#include "KVString.h"
#include "KVNameValueList.h"


class KVNumberList;
class KVList;

class KVValues : public KVNameValueList
{

	protected:
	
	Int_t kdeb; //separation entre les valeurs MIN et MAX et les valeurs moments SUM%d, par defaut kdeb=2
	
	Int_t kval_base;//nbre de valeurs de base definies par le constructeur, kval_base = kdeb+kordre_mom_max+1
	Int_t kval_add; //nbre de valeurs additionelles definies via la methode DefineAdditionalValue 
	Int_t kval_tot; //nbre total de valeurs definies kval_tot = kval_base+kval_add (=KVNameValueList::GetNpar()) 
	
	Int_t knbre_val_max;//nombre maximum de valeurs pouvant etre calculees (argument du constructeur)
	Int_t kordre_mom_max;//ordre maximum pour le calcul des moments (argument du constructeur)
	
	Double_t* values;	//[knbre_val_max]  tableau ou sont stockees les valeurs
   
	
	Bool_t kToBeRecalculated;
	
	/*
	kform_add et kpar_add sont remplies en parallele et permettent le calcul des variables
	additionnelles
	*/
	KVList* kform_add;	//->liste contenant les formules des variables additionnelle (stockees en TFormula)
	KVList* kpar_add;		//->liste contenant les liste de parametres (stockees en KVNumberList)
	
	
	void init_val_base();
	void init_val_add();
	void init_val();
	
	void ComputeAdditionalValues(Int_t min=-1,Int_t max=-1);
	void LinkParameters(KVNumberList* nl);
	Bool_t AddFormula(KVString name,KVString expr);
	KVNumberList*  TransformExpression(KVString &expr);
	
	public:
	
	KVValues(){}
	
	KVValues(const Char_t* name,Int_t ordre_max=3,Int_t nbre_max=20){
		knbre_val_max = nbre_max;		//nbre max de valeurs stockables 
		kordre_mom_max = ordre_max;	//ordre max des moments calcules a chaque iteration
		SetName(name);						//nom correspondant aux valeurs calculees
		
		init_val_base();
		init_val_add();
	}
	
	void Reset(){
		init_val();
		kToBeRecalculated=kTRUE;
	}
	
	void	Clear(Option_t* option = "");
	virtual ~KVValues(){
		Clear();
		delete [] values; values=0;
		kval_tot = kval_base = kdeb = 0;
	}

	void FillVar(Double_t val){ FillVar(val,1.); }

	void FillVar(Double_t val,Double_t weight){
		if (val<values[0]) values[0] = val;	//GetIntValue("MIN")=0
		if (val>values[1]) values[1] = val;	//GetIntValue("MAX")=1
		for (Int_t nn=0;nn<=kordre_mom_max;nn+=1) values[nn+kdeb] += weight*TMath::Power(val,Double_t(nn));
		kToBeRecalculated = kTRUE;
	
	}
	
	Double_t GetValue(Int_t idx) {
		ComputeAdditionalValues();
		kToBeRecalculated = kFALSE;
		return values[idx];
	}
	
	Double_t GetValue(const Char_t* name) { return GetValue(GetValuePosition(name)); }
	Int_t GetValuePosition(const Char_t* name) { return GetIntValue(name); }
	
	TString GetValueExpression(const Char_t* name) {
		Int_t idx = GetValuePosition(name);
		return GetValueExpression(idx);
	}
	
	TString GetValueExpression(Int_t idx) {
		Int_t new_idx = idx-kval_base;
		if (new_idx<0){
			Warning("GetValueExpression","Cette methode n'est implementee que pour les valeurs additionelles idx > %d",kval_base);
			return "";
		}
		return ((TFormula* )kform_add->At(new_idx))->GetExpFormula();
	}
	
	void	Print(Option_t* option = "") const {
		Info("Print","%s : %d values computed",GetName(),kval_tot);
		for (Int_t nn=0; nn<kval_tot; nn+=1){
			printf("- %d %s %lf\n",nn,GetNameAt(nn),GetValue(nn));
		}
	}

	Int_t GetNValues(KVString opt="base"){
		if (opt=="base") 		return kval_base;
		else if (opt=="add")	return kval_add;
		else { return GetNpar(); }
	}
	
	void DefineAdditionalValue(KVString name,KVString expr);
	
	ClassDef(KVValues,1)//Handle Operation on variable

};

#endif
