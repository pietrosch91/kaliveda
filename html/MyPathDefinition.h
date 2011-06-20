#include "THtml.h"
#include "TString.h"
#include "Riostream.h"

class MyPathDefinition : public THtml::TPathDefinition {
public:
// MyPathDefinition(){};
// virtual ~MyPathDefinition(){};
   virtual bool GetDocDir(const TString& module, TString& doc_dir) const;

protected:
   ClassDef(MyPathDefinition, 0);
};

