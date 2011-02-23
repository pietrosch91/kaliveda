//Created by KVClassFactory on Wed Feb  9 11:06:42 2011
//Author: bonnet

#ifndef __KVNUCLDATATABLE_H
#define __KVNUCLDATATABLE_H

#include "KVBase.h"
#include "TMap.h"
#include "TObject.h"
#include "TClass.h"
#include "TObjArray.h"

#include "KVString.h"
#include "KVNuclData.h"

class KVNuclDataTable : public KVBase
{
	
	protected:
	
	TClass* cl;				//pointeur pour gerer les heritages de classes de KVNuclData
	TMap *nucMap;			//mapping (Z,A) -> nucleus index
	Int_t current_idx;	//current index
	Int_t NbNuc;			//nbre de noyaux presents dans la table
	KVString kfilename; 	//nom du fichier lu
	KVString kcomments; 	//Commentaire provenant de la lecture fichier
	KVString kclassname;
	
	TObjArray* tobj;	//! array where all nucldata objects are
	
	KVNuclData* GetCurrent() const { return (KVNuclData* )tobj->At(current_idx); }
	void CreateTable(Int_t ntot);	//Creation of the table
	void CreateElement(Int_t idx);//a new KVNuclData pointeur is created and added
	void InfoOnMeasured() const;
	
	public:
   KVNuclDataTable();
   KVNuclDataTable(KVString classname);
	virtual ~KVNuclDataTable();
	void init();
   virtual void Initialize()=0;	
	
	void GiveIndexToNucleus(Int_t zz,Int_t aa,Int_t ntot);
   Bool_t IsInTable(Int_t zz, Int_t aa) const;
	
	KVNuclData* GetData(Int_t zz, Int_t aa) const;
	Double_t GetValue(Int_t zz, Int_t aa)	const;
	const Char_t*  GetUnit(Int_t zz, Int_t aa) const;
	Bool_t  IsMeasured(Int_t zz, Int_t aa) const;
	
	Int_t GetNumberOfNuclei() const;
	KVString GetReadFileName() const;
	KVString GetCommentsFromFile() const;
		
};	

namespace NDT
{
	class key : public TNamed
	{
		public:
		key(int z,int a) {
			SetName(Form("%d:%d",z,a));
		};
		virtual ~key(){};
		ClassDef(key,0)
	};
	class value : public TObject
	{
		int idx;
		public:
		value(int i) : idx(i) {};
		virtual ~value(){};
		int Index() const
		{
			return idx;
		};
		ClassDef(value,0)
	};
};

#endif
