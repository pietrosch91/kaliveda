/*
$Id: KVQPsource.h,v 1.3 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.3 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Wed Jul  5 17:37:01 2006
//Author: Daniel CUSSOL

#ifndef __KVQPSOURCE_H
#define __KVQPSOURCE_H

#include <KVVarGlob.h>
#include <KVNucleus.h>

class KVQPsource:public KVVarGlob
	{
// Champs Statiques:
	public:

// Champ proteges:
        private:
	KVNucleus QPsource;
	Double_t fVal[7];//! used by GetValuePtr

	
// Methodes
	protected:
	void init_KVQPsource(void);
	virtual Double_t getvalue_void() const;	
	virtual Double_t getvalue_int(Int_t i);
				

	public:
	KVQPsource(void);			// constructeur par defaut
	KVQPsource(Char_t *nom);
	KVQPsource(const KVQPsource &a);	// constructeur par Copy
	
	virtual ~KVQPsource(void);		// destructeur
	
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif

	KVQPsource& operator = (const KVQPsource &a); // operateur =
	

	virtual void Init(void);
	virtual void Reset(void);	
	virtual void Fill(KVNucleus *c);  
	virtual Double_t *GetValuePtr(void); 
	virtual TObject *GetObject(void); 
	
	ClassDef(KVQPsource,1)//User global variable class for reconstruction of QP source
	
	};
#endif
