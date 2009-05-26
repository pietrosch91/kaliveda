/***************************************************************************
                          kvregister.h  -  description
                             -------------------
    begin                : 10/05/2003
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVRegister.h,v 1.11 2005/10/10 12:59:44 franklan Exp $
**************************************************************************
* Definition de la classe registre pour le commande et controle INDRA    *
* Premier test                                                           *
* D.Cussol                                                               *
* 11/02/97                                                               *
**************************************************************************/

#ifndef KVREGISTER_H
#define KVREGISTER_H

#include "KVBase.h"
#include "TString.h"
#include "TMath.h"
#include "Binary_t.h"

class KVRegister:public KVBase {

   Long64_t fAdresse;
   Binary_t fContenu;
   Char_t fAcces;
   Int_t fVoie_deb;
   Int_t fVoie_fin;
   Int_t fVoie_pas;
   TString fFonction_associee;
   
 public:

// Createur
    KVRegister(void);
    KVRegister(const KVRegister &);
    
// Destructeur 
    virtual ~ KVRegister();

// Lecture des champs:
   inline virtual Long64_t GetAddress(void) const;
   inline virtual Long64_t GetContents(const Long64_t=0, const Long64_t=0) const;
   inline virtual Binary_t GetBinaryContents() const;
   inline virtual Int_t GetNumberBits(void) const;
   inline virtual Char_t GetAccess(void) const;
   inline virtual Int_t GetFirstVoie(void) const;
   inline virtual Int_t GetLastVoie(void) const;
   inline virtual Int_t GetVoieStep(void) const;
   inline virtual Int_t GetNumberVoies(void) const;
   inline virtual const Char_t* GetAssocFunc() const;
// Ecriture des champs:
   inline virtual void SetAddress(const Long64_t);
   inline virtual void SetContents(const Long64_t, const Long64_t=0, const Long64_t=0);
   inline virtual void SetContents(const Char_t*, const Long64_t=0, const Long64_t=0);
   inline virtual void SetContents(const Binary_t&, const Long64_t=0, const Long64_t=0);
   inline virtual void SetNumberBits(const Char_t);
   inline virtual void SetAccess(const Char_t);
   inline virtual void SetChannels(const Char_t *);
   inline virtual void SetAssocFunc(const Char_t*);
	
   virtual const Char_t* GetRealValue(const Long64_t=0, const Long64_t=0) const;
   virtual void SetRealValue(const Double_t, const Long64_t=0, const Long64_t=0);


// Affichage normalise des champs du registre
   virtual void Print(Option_t * opt = "") const;
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif

//calculate maximum value containable in register
   virtual Double_t GetMaxCont() const { return (TMath::Power(2., GetNumberBits()) - 1.);};
		   
    ClassDef(KVRegister, 1);    //Base class for description of registers in modules

};

// Lecture des champs:
inline Long64_t KVRegister::GetAddress(void) const
{
   return (fAdresse);
}

inline Long64_t KVRegister::GetContents(const Long64_t, const Long64_t) const
{
   return (fContenu.Value());
}

inline Int_t KVRegister::GetNumberBits(void) const
{
   return (Int_t)(fContenu.GetNBits());
}

inline Char_t KVRegister::GetAccess(void) const
{
   return (fAcces);
}

inline Int_t KVRegister::GetFirstVoie(void) const
{
   return (fVoie_deb);
}

inline Int_t KVRegister::GetLastVoie(void) const
{
   return (fVoie_fin);
}

inline Int_t KVRegister::GetVoieStep(void) const
{
   return (fVoie_pas);
}
inline const Char_t* KVRegister::GetAssocFunc() const
{
   return fFonction_associee;
}

// Ecriture des champs:
inline void KVRegister::SetAddress(const Long64_t Adresse)
{
   fAdresse = Adresse;
}
inline void KVRegister::SetContents(const Long64_t Contenu, const Long64_t, const Long64_t)
{
   fContenu.Set(Contenu);
}
inline void KVRegister::SetNumberBits(const Char_t NbrBits)
{
	//sets number of bits used for binary representation of register's value
	fContenu.SetNBits(NbrBits);
}
inline void KVRegister::SetAccess(const Char_t Acces)
{
   fAcces = Acces;
}
inline void KVRegister::SetChannels(const Char_t * Voie)
{
   if(sscanf(Voie, "%d-%d:%d", &fVoie_deb, &fVoie_fin, &fVoie_pas) != 3){
	   Error("SetChannel(const Char_t*)","Cannot read string : %s",Voie);
	   return;
   }
   if (fVoie_pas == 0)
      fVoie_pas = 1;
}

inline void KVRegister::SetAssocFunc(const Char_t* Fonction_associee)
{
   fFonction_associee = Fonction_associee;
}

inline void KVRegister::SetContents(const Char_t* val, const Long64_t, const Long64_t)
{
	//Set contents of register in binary format
	fContenu = val;
}

//___________________________________________________________________________

inline void KVRegister::SetContents(const Binary_t& val, const Long64_t, const Long64_t)
{
	//Set contents of register in binary format
	fContenu = val;
}

//___________________________________________________________________________

inline Binary_t KVRegister::GetBinaryContents() const
{
	//Returns contents of register in binary format
	return (fContenu);
}

inline Int_t KVRegister::GetNumberVoies(void) const
{
	//Calculate number of channels = difference between number of first and last channel + 1
	//Does not include channel step in calculation.
	
	return (GetFirstVoie() - GetLastVoie() + 1);
}

#endif
