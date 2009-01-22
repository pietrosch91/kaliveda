KVVarGlobTemplate::KVVarGlobTemplate(void):KVVarGlob()
	{
//
// Createur par default
//
	init_KVVarGlobTemplate();
	SetName("KVVarGlobTemplate");
	SetTitle("A KVVarGlobTemplate");
	}
	
//_________________________________________________________________
KVVarGlobTemplate::KVVarGlobTemplate(Char_t *nom):KVVarGlob(nom)
	{
//
// Constructeur avec un nom
//
	init_KVVarGlobTemplate();
	}

//_________________________________________________________________
KVVarGlobTemplate::KVVarGlobTemplate(const KVVarGlobTemplate &a):KVVarGlob()
	{
// 
// Contructeur par Copy
//
	init_KVVarGlobTemplate();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVVarGlobTemplate&)a).Copy(*this);
#endif
	}

//_________________________________________________________________
KVVarGlobTemplate::~KVVarGlobTemplate(void)
       {
// 
// Destructeur
//
       }

//_________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVVarGlobTemplate::Copy(TObject&a) const
#else
void KVVarGlobTemplate::Copy(TObject&a)
#endif
	{
//
// Methode de Copy
//
	KVVarGlob::Copy(a);
	}
	
//_________________________________________________________________
KVVarGlobTemplate& KVVarGlobTemplate::operator = (const KVVarGlobTemplate &a)
	{
//
// Operateur =
//
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVVarGlobTemplate&)a).Copy(*this);
#endif
	return *this;
	}

//_________________________________________________________________
void KVVarGlobTemplate::Init(void)
 {
// methode d'initialisation des
// variables internes
 }		
				
//_________________________________________________________________
void KVVarGlobTemplate::Reset(void)		
 {
// Remise a zero avant le
// traitement d'un evenement
 }		
				
//_________________________________________________________________
Double_t KVVarGlobTemplate::GetValue(Char_t *name)    
{
	// on retourne la valeur de la variable "name"
	return KVVarGlob::GetValue(name);
}

//_________________________________________________________________
Double_t KVVarGlobTemplate::GetValue(void) const
 {
// returns the value of the global variable
 Double_t val=0;
 
// compute here the value of the global variable and set the value of val

 return val; 
 }		
				
//_________________________________________________________________
Double_t *KVVarGlobTemplate::GetValuePtr(void)
 {
// On retourne un tableau de valeurs. il est organise comme suit 
//
// Index  Meaning
//--------------------------------------
// 0	  Variable 1
// 1	  Variable 2
// ...	  ...
// ...	  ...
// n-1	  Variable n
//
//
 Double_t *v=0;
 
// v=new Double_t[n];
// v[0]=ValueOfVariable1;
// v[1]=ValueOfVariable2;
// ...	  ...
// ...	  ...
// v[n-1]=ValueOfVariablen;
//

 return v;
 }	
                                
//_________________________________________________________________
Double_t KVVarGlobTemplate::GetValue(Int_t i)
 {
// on retourne la ieme valeur du tableau
//
// Index  Meaning
//--------------------------------------
// 0	  Variable 1
// 1	  Variable 2
// ...	  ...
// ...	  ...
// n-1	  Variable n
//
//
 Double_t rval=0;
 switch (i)
  {
//   case 0 : rval=ValueOfVariable1;
//   	   break;
//   
//   case 1 : rval=ValueOfVariable2;
//   	   break;
//   
//   case n : rval=ValueOfVariablen;
//   	   break;
  
  default : rval=0.;
            break;
  }
 return rval;
 }         
				
//_________________________________________________________________
TObject *KVVarGlobTemplate::GetObject(void)    
 {
//
// Returns a pointer to an object linked to this global variable
// This may be a list of particles, an intermediate object used to compute values, etc.
// 
 TObject *obj=0;

// Sets the value of the pointer here
 

 return obj;
 }
