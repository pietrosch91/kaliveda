class KVVarGlobMeanTemplate:public KVVarGlobMean
{
	public:
	KVVarGlobMeanTemplate(void);	
	KVVarGlobMeanTemplate(Char_t *nom);
	KVVarGlobMeanTemplate(const KVVarGlobMeanTemplate &a);
	
	virtual ~KVVarGlobMeanTemplate(void);
	
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject & obj) const;
#else
   virtual void Copy(TObject & obj);
#endif

	KVVarGlobMeanTemplate& operator = (const KVVarGlobMeanTemplate &a);
		
	ClassDef(KVVarGlobMeanTemplate,1)
	
};
