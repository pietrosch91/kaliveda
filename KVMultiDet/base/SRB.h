//Created by KVClassFactory on Fri Jul 17 15:10:29 2009
//Author: John Frankland

#ifndef __SRB_H
#define __SRB_H

#include "KVBase.h"

class SRB : public KVBase
{
	TString fexec;//string holding full path to executable
	TString fcmd;//string holding full path to command line with arguments
	TString fout;//string holding results of command
	
	protected:
	Bool_t   buildCommand(const Char_t* scmd, const Char_t* args="", Option_t* opts="");
	Int_t    execCommand();
	TString  pipeCommand();
	Int_t Sinit();
	Int_t Sexit();
			
   public:
   SRB();
   virtual ~SRB();

	TString Sls(const Char_t* directory="");
	Int_t Scd(const Char_t* directory="");
	TString SgetD(const Char_t* file, Option_t* opt="");
	
	TList* Ge
	
   ClassDef(SRB,1)//Interface to SRB commands
};

#endif
