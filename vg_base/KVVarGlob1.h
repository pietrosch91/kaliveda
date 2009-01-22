//
// D.Cussol 
//
// 17/02/2004
// Creation d'une classe Variable Globale
//

#ifndef KVVarGlob1_h
#define KVVarGlob1_h
#include "KVVarGlob.h"

//#define DEBUG_KVVarGlob1

class KVVarGlob1:public KVVarGlob {
 public:
// Champs Statiques:
   static Int_t nb;
   static Int_t nb_crea;
   static Int_t nb_dest;
// Champ propre:
 protected:
    Double_t var;
// Methodes
 protected:
   void init(void);
   void FillVar(Double_t val) {
      var += val;
   };
   void SetValue(Double_t val) {
      var = val;
   };                           // On donne la valeur de la variable
static void FillMethodBody(KVString& body, int type);


 public:
   KVVarGlob1(void);            // constructeur par defaut
   KVVarGlob1(Char_t * nom);
   KVVarGlob1(const KVVarGlob1 & a);    // constructeur par Copy

   virtual ~ KVVarGlob1(void);  // destructeur


   KVVarGlob1 & operator =(const KVVarGlob1 & a);       // operateur =


#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif
   virtual void Init(void);     // methode d'initialisation des
   // variables Int_ternes
   virtual void Reset(void);    // Remise a zero avant le

   virtual Double_t GetValue(void) const;       // On retourne la valeur de la
   // variable.
   virtual Double_t *GetValuePtr(void); // On retourne un tableau de 
   // valeurs 
   virtual Double_t GetValue(Int_t i);  // on retourne la ieme valeur du
   // tableau
   virtual Double_t GetValue(const Char_t * name);    // on retourne une valeur avec
   // un nom
   static void MakeClass(const Char_t * classname,
                         const Char_t * classdesc, int type = KVVarGlob::kOneBody);

   ClassDef(KVVarGlob1, 1)      // Base class for global variables with one value 
};
#endif
