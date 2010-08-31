//Created by KVClassFactory on Wed May 19 17:23:29 2010
//Author: bonnet

#include "KVValues.h"
#include "KVNumberList.h"
#include "KVList.h"

ClassImp(KVValues)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVValues</h2>
<h4>Handle Operation on variable</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
	void KVValues::init_val_base(){
	
		//Mise en correspondance du noms des differentes valeurs a calculees
		//et de leur position dans le tableau values
		// Ex: 
		// nom_valeurs -> id_valeurs -> values[id_valeurs]
		
		KVString lname="MIN MAX";
		lname.Begin(" ");
		kval_base=0;		//variables de base disponibles
		
		kval_tot=0;	//nombre de variables totales disponibles
		while (!lname.End()){
			KVString tamp = lname.Next();
			SetValue(tamp.Data(),kval_tot++);
		}
		kdeb = kval_tot;	//ici pos_deb=2 (par defaut)
		
		KVString smoment;
		for (Int_t mm=0;mm<=kordre_mom_max;mm+=1){	//Ex: moment_max = 3
			smoment.Form("SUM%d",mm);
			SetValue(smoment.Data(),kval_tot++);
		}
		kval_base = kval_tot;	//ici nbase=6 (par defaut)
		
		values = new Double_t[knbre_val_max];
		init_val();
	
	
	}
	
	void KVValues::init_val_add(){
	
		kform_add = new KVList();
		kpar_add = new KVList();
		
		kval_add = 0;
		
		kToBeRecalculated=kTRUE;
	
	}
	
	void KVValues::init_val(){
		values[GetIntValue("MIN")] = 1e6;
		values[GetIntValue("MAX")] = -1e6;
		
		for (Int_t nn=kdeb;nn<kval_tot;nn+=1) 
			values[nn]=0;
	}

	void	KVValues::Clear(Option_t* option){
	
		Reset();
		
		for (Int_t ii=0;ii<kval_add;ii+=1)	delete RemoveAt(kval_base);
		
		delete kform_add; 	kform_add=0;
		delete kpar_add;		kpar_add = 0;
			
		kval_add = 0;
		kval_tot = kval_base;
	
	}
	
	void KVValues::LinkParameters(KVNumberList* nl) { 
		kpar_add->Add(nl); 
	}

	void KVValues::ComputeAdditionalValues(Int_t min,Int_t max){
		
		if (min<0) 	 min=0;
		if (max==-1) max=kval_add;
		
		KVNumberList* nl = 0;
		TFormula* fm = 0;
		
		for (Int_t ff=min; ff<max; ff+=1){
			nl = (KVNumberList* )kpar_add->At(ff);
			fm = (TFormula* )kform_add->At(ff);
			Info("ComputeAdditionalValues","%d | %d %d %d | %s",ff,min,max,kval_add,nl->GetList());
			nl->Begin();
			while (!nl->End()){
				Int_t nn = nl->Next();
				Info("ComputeAdditionalValues","%d values[]=%lf",nn,values[nn]);
				fm->SetParameter(nn,values[nn]);
			}
			Info("ComputeAdditionalValues","Eval %lf",fm->Eval(0));
			values[kval_tot-kval_add+ff] = fm->Eval(0);
		}
	}
	

	void KVValues::DefineAdditionalValue(KVString name,KVString expr) {

		if (HasParameter(name.Data())){
			Warning("DefineAdditionalValue","le nom de variable %s est deja utilise",name.Data());
			return;
		}

		KVString expr2 = expr;
		KVNumberList* nl = TransformExpression(expr2);
		
		if (nl){
			//Info("DefineAdditionalValue","Nouvelle variable %s -> %s",expr.Data(),expr2.Data());
			if (nl->IsEmpty()){
				Warning("DefineAdditionalValue","la KVNumberList est vide #%s#",nl->GetList());
			}
			else {
				if(AddFormula(name,expr2)){
					LinkParameters(nl);
					ComputeAdditionalValues(kval_add-1,kval_add);
				}
			}
		}
		else {
			Error("DefineAdditionalValue","La traduction de l'expression %s a echouee",expr.Data());
		}
	}	
	
	Bool_t KVValues::AddFormula(KVString name,KVString expr) {
	
		TFormula* fm = new TFormula(name.Data(),expr.Data());
		if (fm->GetExpFormula().IsNull()) {
			Warning("AddFormula","%s considere comme nulle",expr.Data());
			return kFALSE;
			delete fm;
		}
		kform_add->Add(fm);
		SetValue(name.Data(),kval_tot++);
		kval_add+=1;
		return kTRUE;
	}
	
	KVNumberList* KVValues::TransformExpression(KVString &expr){
	
		KVNumberList* nl = new KVNumberList();
		
		const char* O="[";
		const char* F="]";

		KVString clone; clone.Form("%s",expr.Data());
		const char* expression = clone.Data();

		Int_t nouvert=0,nferme=0;
		Int_t posouvert=0,posferme=0;
		Int_t nvar=0;
		Bool_t ouvert=kFALSE;

		Int_t nsize = clone.Length();
		for (Int_t ii=0;ii<=nsize;ii+=1){
			if ( expression[ii]==O[0] ) {
				nouvert+=1;
				ouvert=kTRUE;
				posouvert=ii;
			}
			else if ( expression[ii]==F[0] ) {
				nferme+=1;
				ouvert=kFALSE;
				posferme=ii;
				KVString st(clone(posouvert+1,posferme-posouvert-1));
				if (st.IsDigit()){
					Int_t idx = st.Atoi();
					if (0<=idx && idx<kval_tot) {
						nl->Add(idx);
						nvar+=1;
					}
					else { delete nl; return 0; }
				}
				else {
					
					Int_t idx =GetValuePosition(st.Data());
					if (idx==-1){
						delete nl; return 0;
					}
					nl->Add(idx);
					nvar+=1;
					KVString s1; s1.Form("[%s]",st.Data());
					KVString s2; s2.Form("[%d]",idx);
					expr.ReplaceAll(s1,s2);
				}
			}
			else {}
		}

		if (nouvert!=nferme || nvar!=nouvert){
			Error("TransformExpr","nombre [ : %d - nombre de ] : %d - nombre de variables %d",nouvert,nferme,nvar);
		}

		return nl;
	
	}
