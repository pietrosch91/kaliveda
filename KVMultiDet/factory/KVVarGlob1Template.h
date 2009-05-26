class KVVarGlob1Template:public KVVarGlob1
{
	public:
         
	KVVarGlob1Template(void);
	KVVarGlob1Template(Char_t *nom);
	KVVarGlob1Template(const KVVarGlob1Template &a);
	
	virtual ~KVVarGlob1Template(void);	
	
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif

	KVVarGlob1Template& operator = (const KVVarGlob1Template &a);
	
	ClassDef(KVVarGlob1Template,1)
	
};
