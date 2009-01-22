//
// D.Cussol 
//
// 20/03/2006:
// Creation de la Variable Globale Ptot
//

#ifndef KVPtot_h
#define KVPtot_h
#include "KVVarGlob.h"

//#define DEBUG_KVPtot

class KVPtot:public KVVarGlob {
 public:
// Champs Statiques:
   static Int_t nb;
   static Int_t nb_crea;
   static Int_t nb_dest;
// Champs intermediaires 
 protected:
    TVector3 ptot;

// Methodes
 protected:
   void init_KVPtot(void);

 public:
    KVPtot(void);               // constructeur par defaut
    KVPtot(Char_t * nom, const Char_t * frm = "");
    KVPtot(const KVPtot & a);   // constructeur par Copy

    virtual ~ KVPtot(void);     // destructeur

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif

    KVPtot & operator =(const KVPtot & a);      // operateur =

   virtual void Init(void);     // methode d'initialisation des
   // variables Internes
   virtual void Reset(void);    // Remise a zero avant le
   // traitement d'un evenement
   virtual void Fill(KVNucleus * c);    // Remplissage de la variable.

   virtual Double_t GetValue(Int_t i);  // on retourne la ieme valeur du
   // tableau
   virtual Double_t GetValue(Char_t * name);    // on retourne la valeur de
   // la variable "name"
   virtual Double_t GetValue(void) const;       // On retourne Pz

   virtual Double_t *GetValuePtr(void); // On retourne le tableau des valeurs

   virtual TVector3 GetTVector3(void) const;    // on retourne le TVector3

    ClassDef(KVPtot, 1)         // Global variable Ptot=Sum(p(i))
};
#endif
