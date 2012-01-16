//
// D.Cussol 
//
// 16/02/2004:
// Creation d'une classe Variable Globale pour KaliVeda
//


#ifndef KVVarGlob_h
#define KVVarGlob_h
#include "Riostream.h"
#include "KVBase.h"
#include "KVNucleus.h"
#include "KVParameterList.h"
#include "KVString.h"
#include "KVParticleCondition.h"

//#define DEBUG_KVVarGlob

class KVVarGlob:public KVBase {

 public:
// Champs Statiques:
   static Int_t nb;
   static Int_t nb_crea;
   static Int_t nb_dest;

   // global variable types
   enum {
      kOneBody, // one-body variable: Fill (KVNucleus*) must be defined
      kTwoBody, // two-body variable: Fill2 (KVNucleus*, KVNucleus*) must be defined
      kNBody      // N-body variable: FillN (KVEvent*) must be defined
   };
   
// Champs Prive
 private:
    KVParameterList < Int_t > nameList;//correspondence between variable name and index

// Methodes
 protected:
   void init(void);
   void SetNameIndex(const Char_t * name, Int_t index); // associe un nom et un index
   void ClearNameIndex()
   {
   	// Delete previously defined associations between variable name and index
   	nameList.Clear();
   };

   KVString fFrame;//(optional) name of reference frame used for kinematics
   KVParameterList<KVString> fOptions;//list of options
   KVParameterList<Double_t> fParameters;//list of parameters
   KVParticleCondition* fSelection;//(optional) condition used to select particles
   
   Int_t fType;   // type of variable global; = kOneBody, kTwoBody or kNBody
   
   static void FillMethodBody(KVString& body, int type);
   static void AddInitMethod(const Char_t* classname, KVClassFactory &cf, KVString &body, int type);
   static void AddFillMethod(KVClassFactory &cf, int type);
   static void AddFillMethodBody(KVClassFactory &cf, KVString& body, int type);
   
	/*** protected methods to be overridden in child classes ***/
	
	virtual Double_t getvalue_void() const
	{
		// Redefine this method in child classes to change the behaviour of
		// KVVarGlob::GetValue(void)
		
		AbstractMethod("getvalue_void");
		return 0;
	};
	virtual Double_t getvalue_char(const Char_t* name)
	{
		// By default, this method returns the value of the variable "name"
		// using the name-index table set up with SetNameIndex(const Char_t*,Int_t).
		// Redefine this method in child classes to change the behaviour of
		// KVVarGlob::GetValue(const Char_t*)
		
   	return getvalue_int(GetNameIndex(name));
	};
	virtual Double_t getvalue_int(Int_t)
	{
		// Redefine this method in child classes to change the behaviour of
		// KVVarGlob::GetValue(Int_t)
		
		AbstractMethod("getvalue_int");
		return 0;
	};
	
 public:
    KVVarGlob(void);            // constructeur par defaut
    KVVarGlob(Char_t * nom);
    KVVarGlob(const KVVarGlob & a);     // constructeur par Copy

    virtual ~ KVVarGlob(void);  // destructeur

    KVVarGlob & operator =(const KVVarGlob & a);        // operateur =
    
    // returns kTRUE for variables of one-body type for which Fill(KVNucleus*) method must be defined
    Bool_t IsOneBody() { return fType==kOneBody; };
    
    // returns kTRUE for variables of two-body type for which Fill2(KVNucleus*,KVNucleus*) method must be defined
    Bool_t IsTwoBody() { return fType==kTwoBody; };
    
    // returns kTRUE for variables of N-body type for which FillN(KVEvent*) method must be defined
    Bool_t IsNBody() { return fType==kNBody; };
    
   virtual void Init(void) {
   	// Initialisation of internal variables, called once before beginning treatment
   	Info("Init", "Default method. Does nothing."); 
   };
   
   virtual void Reset(void) {
   	// Reset internal variables, called before treatment of each event
   	Info("Reset", "Default method. Does nothing."); 
   };
   
   
   virtual void Fill(KVNucleus * c);
   virtual void Fill2(KVNucleus * n1, KVNucleus* n2);
   virtual void FillN(KVEvent *e);

   void FillWithCondition(KVNucleus * c){
   	// Evaluate contribution of particle to variable only if it satisfies
      // the particle selection criteria given with SetSelection(KVParticleCondition&)
      Bool_t ok = (fSelection ? fSelection->Test(c) : kTRUE);
      if( ok ) Fill(c);  
   };
   void Fill2WithCondition(KVNucleus * n1, KVNucleus* n2){
   	// Evaluate contribution of particles to variable only if both satisfy
      // the particle selection criteria given with SetSelection(KVParticleCondition&)
      Bool_t ok = (fSelection ? (fSelection->Test(n1) && fSelection->Test(n2))  : kTRUE);
      if( ok ) Fill2(n1,n2);
   };

   Double_t GetValue(void) const
	{
   	// On retourne la valeur de la variable.
		// To override behaviour of this method in child classes,
		// redefine the protected method getvalue_void()
		
		return getvalue_void();
	};
   Double_t GetValue(const Char_t * name)
	{
   	// on retourne la valeur de la variable "name"
		// To override behaviour of this method in child classes,
		// redefine the protected method getvalue_char(const Char_t*)
		
		return getvalue_char(name);
	};
   Double_t GetValue(Int_t i)
	{
   	// on retourne la ieme valeur du tableau
		// To override behaviour of this method in child classes,
		// redefine the protected method getvalue_int(Int_t)
		
		return getvalue_int(i);
	};
   virtual Double_t *GetValuePtr(void)
	{
   	// On retourne un tableau de valeurs 
		AbstractMethod("GetValuePtr");
		return NULL;
	};
   // On retourne la valeur de la variable.
   Double_t operator() (void) { return GetValue(); };
   // on retourne la valeur de la variable "name"
   Double_t operator() (const Char_t * name) { return GetValue(name); };
   // on retourne la ieme valeur du tableau
   Double_t operator() (Int_t i) { return GetValue(i); };
   // on retourne un pointeur sur un objet
   virtual TObject *GetObject(void);
   // on retourne l'index associe au nom contenu dans name
   virtual Int_t GetNameIndex(const Char_t * name);

   static void MakeClass(const Char_t * classname, const Char_t * classdesc, int type = kOneBody);
   
   virtual void SetFrame(const Char_t*);
   
   //Name of reference frame used for kinematics
   virtual const Char_t* GetFrame(const Char_t*){
      return fFrame.Data();
   };
   
   virtual void SetOption(const Char_t* option, const Char_t* value);   
   virtual Bool_t IsOptionGiven(const Char_t* option);
   virtual KVString& GetOptionString(const Char_t* option) const;
   virtual void UnsetOption(const Char_t* opt);

   virtual void SetParameter(const Char_t* par, Double_t value);   
   virtual Bool_t IsParameterSet(const Char_t* par);
   virtual Double_t GetParameter(const Char_t* par);
   virtual void UnsetParameter(const Char_t* par);
   
   virtual void SetSelection(const KVParticleCondition&);

   virtual Double_t AsDouble() const { return GetValue(); };
   operator double() const { return AsDouble(); };
   
   virtual Int_t GetNumberOfValues() const
   {
   	// Returns number of values associated with global variable.
   	// This is the number of indices defined using SetNameIndex method.
   	return nameList.GetNPar();
   };
   const Char_t* GetValueName(Int_t i) const
   {
   	// Returns name of value associated with index 'i',
   	// as defined by using SetNameIndex method.
   	for(int j=0; j<GetNumberOfValues(); j++){
   		if(nameList.GetParameter(j)->GetVal() == i) return nameList.GetParameter(j)->GetName();
   	}
   	return "unknown";
   };
   
   ClassDef(KVVarGlob, 4)      // Base class for global variables
};
#endif
