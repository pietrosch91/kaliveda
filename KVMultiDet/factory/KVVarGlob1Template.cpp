//_________________________________________________________________
KVVarGlob1Template::KVVarGlob1Template(void):KVVarGlob1()
	{
//
// Default constructor (may not be modified)
//
	init_KVVarGlob1Template();
	SetName("KVVarGlob1Template");
	SetTitle("A KVVarGlob1Template");
	}
	
//_________________________________________________________________
KVVarGlob1Template::KVVarGlob1Template(Char_t *nom):KVVarGlob1(nom)
	{
//
// Constructeur avec un nom (may not be modified)
//
	init_KVVarGlob1Template();
	}

//_________________________________________________________________
KVVarGlob1Template::KVVarGlob1Template(const KVVarGlob1Template &a):KVVarGlob1()
	{
// 
// Copy constructor (may not be modified)
//
	init_KVVarGlob1Template();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVVarGlob1Template&)a).Copy(*this);
#endif
	}

//_________________________________________________________________
KVVarGlob1Template::~KVVarGlob1Template(void)
       {
// 
// Destructeur
//
       }

//_________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVVarGlob1Template::Copy(TObject&a) const
#else
void KVVarGlob1Template::Copy(TObject&a)
#endif
	{
// Copy method
	//In order to be coherent with ROOT Copy functions, "a" becomes a copy of "this",
	//not the other way around as before.

	KVVarGlob1::Copy(a);
	// To copy a specific field, do as follows:
	//
	//((KVVarGlob1Template &)a).field=field;
	//
	// If setters and getters are available, proceed as follows
	//
	//((KVVarGlob1Template &)a).SetField(GetField());
	//
	
	}
	
//_________________________________________________________________
KVVarGlob1Template& KVVarGlob1Template::operator = (const KVVarGlob1Template &a)
	{
//
// Operator = (may not be modified)
//
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVVarGlob1Template&)a).Copy(*this);
#endif
	return *this;
	}

