//Created by KVClassFactory on Fri Jul 17 15:10:29 2009
//Author: John Frankland

#ifndef __SRB_H
#define __SRB_H

#include "KVBase.h"
#include "KVDatime.h"

class SRBFile_t : public KVBase
{
	KVDatime fModTime; // last modification
	Bool_t fIsCont;// kTRUE if is container
	
	public:
	SRBFile_t() : KVBase() {
		fIsCont=kFALSE;
	};
	virtual ~SRBFile_t(){};
	
	KVDatime GetModTime() const { return fModTime; };
	void SetModTime(KVDatime&d){ fModTime = d; };
	UInt_t GetSize() const { return GetNumber(); };
	void SetSize(UInt_t s) { SetNumber(s); };
	void SetIsContainer(Bool_t yes=kTRUE) { fIsCont = yes; };
	Bool_t IsContainer() const { return fIsCont; };
	void ls(Option_t *opt="") const;
	
	ClassDef(SRBFile_t,1)//Describes attributes of SRB file/container
};

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

	TString Sls(const Char_t* directory="", Option_t* opt="");
	Int_t Scd(const Char_t* directory="");
	Int_t Sput(const Char_t* source, const Char_t* target=".", Option_t* opt="");
	Int_t Sget(const Char_t* source, const Char_t* target=".", Option_t* opt="");
	TString SgetD(const Char_t* file, Option_t* opt="");
	Int_t Srm(const Char_t* path, Option_t* opt="");
	Int_t Smkdir(const Char_t* path, Option_t* opt="");
	
	TList* GetFullListing(const Char_t* directory="");
	TList* GetListing(const Char_t* directory="");
	Bool_t DirectoryContains(const Char_t* name, const Char_t* directory="");
	Bool_t GetPathInfo(const Char_t* path, SRBFile_t& fs);
	
   ClassDef(SRB,1)//Interface to SRB commands
};

#endif
