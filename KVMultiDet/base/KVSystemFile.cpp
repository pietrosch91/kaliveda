//Created by KVClassFactory on Mon Sep 24 10:45:24 2012
//Author: John Frankland,,,

#include "KVSystemFile.h"
#include "KVBase.h"
#include "TROOT.h"

ClassImp(KVSystemFile)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSystemFile</h2>
<h4>TSystemFile with added info on file size etc.</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSystemFile::KVSystemFile()
{
   // Default constructor
   fUserInfo=0;
}

//________________________________________________________________

KVSystemFile::KVSystemFile(const Char_t* filename, const Char_t* dirname)
	: TSystemFile(filename,dirname)
{
   // Get file infos
   TString path;
   AssignAndDelete(path, gSystem->ConcatFileName(dirname,filename));
   gSystem->GetPathInfo(path,fFileInfos);
   fUserInfo = gSystem->GetUserInfo(fFileInfos.fUid);
}

//________________________________________________________________

KVSystemFile::KVSystemFile (const KVSystemFile& obj)  : TSystemFile()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVSystemFile::~KVSystemFile()
{
   // Destructor
   SafeDelete(fUserInfo);
}

//________________________________________________________________

void KVSystemFile::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVSystemFile::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   TSystemFile::Copy(obj);
   //KVSystemFile& CastedObj = (KVSystemFile&)obj;
}

void KVSystemFile::ls(Option_t*) const
{
   TROOT::IndentLevel();
   printf("[%s:%s]\t%s\t%12lld\t%s\n",
         GetUser(), GetGroup(), GetDate(), GetSize(), GetName());
}
