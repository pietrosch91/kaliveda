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

	
// Methodes
	protected:
	void init_KVQPsource(void);

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
	

	virtual void Init(void);		// methode d'initialisation des
						// variables internes
	virtual void Reset(void);		// Remise a zero avant le
						// traitement d'un evenement
	virtual void Fill(KVNucleus *c);  	// Remplissage de la variable.
	
	virtual Double_t GetValue(void) const;	// On retourne la valeur de la
						// variable.
	virtual Double_t *GetValuePtr(void);	// On retourne un tableau de 
	                                        // valeurs 
	virtual Double_t GetValue(Int_t i);     // on retourne la ieme valeur du
						// tableau
	virtual Double_t GetValue(Char_t *name); // on retourne la valeur de
						 // la variable "name"
	virtual TObject *GetObject(void);      // on retourne un pointeur sur un
					       // objet

	ClassDef(KVQPsource,1)//User global variable class for reconstruction of QP source
	
	};
#endif
