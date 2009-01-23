/*
$Id: KVCalorimetry.h,v 1.3 2009/01/23 15:25:52 franklan Exp $
$Revision: 1.3 $
$Date: 2009/01/23 15:25:52 $
*/

//Created by KVClassFactory on Mon Apr 14 15:01:51 2008
//Author: eric bonnet,,,

#ifndef __KVCALORIMETRY_H
#define __KVCALORIMETRY_H

#include "KVVarGlob.h"

class KVCalorimetry:public KVVarGlob
{
   
	protected:
	
	KVString ingredient_list;
	KVString parameter_list;
	Double_t*	ingredients;//!
	Int_t 	nbre_ingredients;
	
	Bool_t 	kIsModified;
	Bool_t	kParameterChanged;
	Double_t kracine_max,kracine_min;
	Int_t 	kroot_status;
	
	void 		SumUp(); 
	Bool_t 	RootSquare(Double_t aaa,Double_t bbb,Double_t ccc);
	Double_t GetIngValue(KVString ingredient_name){ return ingredients[GetNameIndex(ingredient_name.Data())]; }
	void 		SetIngValue(KVString ingredient_name,Double_t value){ ingredients[GetNameIndex(ingredient_name.Data())]=value; }
	void	 	AddIngValue(KVString ingredient_name,Double_t value){ ingredients[GetNameIndex(ingredient_name.Data())]+=value; }
	
	virtual Double_t getvalue_int(Int_t);
	
	public:
	
   KVCalorimetry(void);		
	KVCalorimetry(Char_t *nom);
	KVCalorimetry(const KVCalorimetry &a);
	
	virtual ~KVCalorimetry(void);
	
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif
	KVCalorimetry& operator = (const KVCalorimetry &a);
   
	void SetParameter(const Char_t* par, Double_t value){

		if (!fParameters.HasParameter(par)) parameter_list += KVString(par)+" ";
		KVVarGlob::SetParameter(par,value);
		kParameterChanged=kTRUE;

	}				

	void SetAsurZ(Double_t fact)								{ SetParameter("AsurZ",fact); }
	void SetMinimumChargeForFragment(Double_t fact)		{ SetParameter("zmin_frag",fact); }
	void SetFactorForNeutronsMeanEnergy(Double_t fact)	{ SetParameter("neutron_factor",fact); }
	void SetLevelDensityParameter(Double_t fact)			{ SetParameter("level_parameter",fact); }
	void SetFactorForParticles(Double_t fact)				{ SetParameter("particle_factor",fact); }
	
	void Reset(void);	
	
	void 	init_KVCalorimetry();
   void 	Fill(KVNucleus*);
   
	void 	Calculate(Double_t neutron_factor=-1,Double_t level_parameter=-1);
	
	void 	PrintIngredients(){
		if (ingredients) {
			ingredient_list.Begin(" ");
			while (!ingredient_list.End()){
				KVString name = ingredient_list.Next();
				Int_t idx = GetNameIndex(name.Data());
				printf("%d - %s - %lf\n",idx,name.Data(),GetValue(idx));
			}
		}
	}
	
	void 	PrintParameters(){
		parameter_list.Begin(" ");
		while (!parameter_list.End()){
			KVString name = parameter_list.Next();
			printf("%s - %lf\n",name.Data(),GetParameter(name.Data()));
		}
	}
	
	ClassDef(KVCalorimetry,1)//compute calorimetry
	
};


#endif
