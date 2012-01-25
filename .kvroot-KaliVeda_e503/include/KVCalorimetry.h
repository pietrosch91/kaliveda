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
#include "KVNameValueList.h"
#include "KVNucleus.h"

class KVCalorimetry:public KVVarGlob
{
   
	protected:
	
	KVNucleus nn;	//! permet d utiliser des methodes de KVNucleus
	KVNameValueList* nvl_ing;//!-> //Contains all ingredients computed
	KVNameValueList* nvl_par;//!-> //Contains all parameters needed for the computation
	
	Bool_t 	kIsModified;	//indique les ingredients ont été modifiés
	Double_t kracine_max,kracine_min; //deux racines issues de la resolution de RootSquare
	Int_t 	kroot_status;	//statut pour la methode de RootSquare
	
	Bool_t kfree_neutrons_included;	// = kTRUE -> Estimation des neutrons libre est faite
	Bool_t kchargediff;	// = kTRUE -> distinction entre particule et fragment
	Bool_t ktempdeduced;	// = kTRUE -> calcul de la temperature
	
	void 		SumUp(); 
	Bool_t 	RootSquare(Double_t aaa,Double_t bbb,Double_t ccc);
	
	virtual Double_t getvalue_int(Int_t);
	
	void SetFragmentMinimumCharge(Double_t value);
	void SetParticleFactor(Double_t value);
	void SetLevelDensityParameter(Double_t value);
	void SetAsurZ(Double_t value);
	void SetNeutronMeanEnergyFactor(Double_t value);
	
	Double_t GetIngValue(Int_t idx);
	void SetIngValue(KVString name,Double_t value);
	void AddIngValue(KVString name,Double_t value);
	void SetParameter(const Char_t* par, Double_t value);
	void init_KVCalorimetry();
	
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
   
	void Reset(void);	
	void Print(Option_t* opt="") const;
	KVNameValueList* GetList(Option_t* opt="ing") const;
	
	Int_t GetNumberOfValues() const;
	
	Double_t GetIngValue(KVString name);
	Double_t GetParValue(KVString name);
	Bool_t HasParameter(KVString name);
	Int_t GetNameIndex(const Char_t * name);
	const Char_t* GetValueName(Int_t i) const;
	Double_t* GetValuePtr(void);
	
	void UseChargeDiff(Int_t FragmentMinimumCharge,Double_t ParticleFactor);
	void DeduceTemperature(Double_t LevelDensityParameter);
	void IncludeFreeNeutrons(Double_t AsurZ,Double_t NeutronMeanEnergyFactor,Double_t LevelDensityParameter);
	
	void 	Fill(KVNucleus*);
   void 	Calculate(void);
	
	ClassDef(KVCalorimetry,1)//compute calorimetry
	
};


#endif
