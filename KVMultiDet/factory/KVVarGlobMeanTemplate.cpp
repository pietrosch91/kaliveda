KVVarGlobMeanTemplate::KVVarGlobMeanTemplate(void):KVVarGlobMean()
	{
//
// Default constructor (may not be modified)
//
	init_KVVarGlobMeanTemplate();
	SetName("KVVarGlobMeanTemplate");
	SetTitle("A KVVarGlobMeanTemplate");
	}
	
//_________________________________________________________________
KVVarGlobMeanTemplate::KVVarGlobMeanTemplate(Char_t *nom):KVVarGlobMean(nom)
	{
//
// Constructeur avec un nom (may not be modified)
//
	init_KVVarGlobMeanTemplate();
	}

//_________________________________________________________________
KVVarGlobMeanTemplate::KVVarGlobMeanTemplate(const KVVarGlobMeanTemplate &a):KVVarGlobMean()
	{
// 
// Copy constructor (may not be modified)
//
	init_KVVarGlobMeanTemplate();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVVarGlobMeanTemplate&)a).Copy(*this);
#endif
	}

//_________________________________________________________________
KVVarGlobMeanTemplate::~KVVarGlobMeanTemplate(void)
       {
// 
// Destructeur
//
       }

//_________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVVarGlobMeanTemplate::Copy(TObject&a) const
#else
void KVVarGlobMeanTemplate::Copy(TObject&a)
#endif
	{
// Copy method
	//In order to be coherent with ROOT Copy functions, "a" becomes a copy of "this",
	//not the other way around as before.

	KVVarGlobMean::Copy(a);
	// To copy a specific field, do as follows:
	//
	//((KVVarGlobMeanTemplate &)a).field=field;
	//
	// If setters and getters are available, proceed as follows
	//
	//((KVVarGlobMeanTemplate &)a).SetField(GetField());
	//
	
	}
	
//_________________________________________________________________
KVVarGlobMeanTemplate& KVVarGlobMeanTemplate::operator = (const KVVarGlobMeanTemplate &a)
	{
//
// Operator = (may not be modified)
//
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVVarGlobMeanTemplate&)a).Copy(*this);
#endif
	return *this;
	}

