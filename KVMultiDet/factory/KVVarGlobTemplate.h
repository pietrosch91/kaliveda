class KVVarGlobTemplate:public KVVarGlob
{
   public:
	
   KVVarGlobTemplate(void);		
	KVVarGlobTemplate(Char_t *nom);
	KVVarGlobTemplate(const KVVarGlobTemplate &a);
	
	virtual ~KVVarGlobTemplate(void);
	
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif
	KVVarGlobTemplate& operator = (const KVVarGlobTemplate &a);
   
	virtual void Init(void);	
	virtual void Reset(void);	
	virtual Double_t GetValue(void) const;	
	virtual Double_t *GetValuePtr(void);
	virtual Double_t GetValue(Int_t i);
	virtual Double_t GetValue(Char_t *name);
	virtual TObject *GetObject(void); 

	ClassDef(KVVarGlobTemplate,1)
	
};
