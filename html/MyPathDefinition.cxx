#include "MyPathDefinition.h"
#include "TSystem.h"
#include "Riostream.h"

ClassImp(MyPathDefinition)

bool MyPathDefinition::GetDocDir(const TString& module, TString& doc_dir) const
{
	// Overrides standard TPathDefinition::GetDocDir
	// When doc is generated, the working directory is the /html
	// source directory. We look for extra module doc first in
	//  ../KVMultiDet/module
	// then, if this directory doesn't exist, in
	// ../KVIndra/module
	// ../VAMOS/module
	// and finally
	// html/examples/
   	if (module.Length()){
      	doc_dir = "../KVMultiDet/" + module;
      	doc_dir +="/";
      	if(gSystem->AccessPathName(doc_dir.Data())){
      		doc_dir = "../KVIndra/" + module;
      		doc_dir +="/";
      		if(gSystem->AccessPathName(doc_dir.Data())){
      			doc_dir = "../VAMOS/" + module;
      			doc_dir +="/";
      			if(gSystem->AccessPathName(doc_dir.Data())){
      				doc_dir = "../KVSpectrometer/" + module;
      				doc_dir +="/";
      				if(gSystem->AccessPathName(doc_dir.Data())){
       					doc_dir =  module + "/";
     				}
				}
      		}
      	}
    }
    else
    {
    	// module="": looking for product doc
    	doc_dir = "";
    }
   doc_dir += "doc";
   cout << "Documentation directory for module " << module.Data() << " : " << doc_dir.Data() << endl;
   return true;
}

