/***************************************************************************
                          kvbase.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVBase.h,v 1.42 2009/03/12 13:59:40 franklan Exp $
***************************************************************************/

#ifndef KVBASE_H
#define KVBASE_H

#include "TRandom3.h"
#include "TNamed.h"
#include "RVersion.h"
#include "KVString.h"
#include "KVLockfile.h"

#define KVROOT_NOT_DEFINED "Define KVROOT environment variable before using"

class TPluginHandler;

class KVBase:public TNamed {
 private:
   static UInt_t fNbObj;        //! Total number of KaliVeda objects (actually counts number of calls to default ctor)
   UInt_t fNumber;              // for numbering objects
   TString fSLabel;             //label for the object

//names of $KVROOT, $KVROOT/bin and $KVROOT/KVFiles directories
   static TString KVRootDir;    //!
   static TString KVBinDir;     //!
   static TString KVFilesDir;   //!

   static Bool_t fEnvIsInit;//! set when environment is initialised

   void init();
	static void ReadGUIMimeTypes();

 public:

    enum EKaliVedaBits {
       kIsKaliVedaObject = BIT(23)   //all KVBase-derived objects have this bit set in TObject::fBits
   };

   static void InitEnvironment();
    KVBase();
    KVBase(const Char_t * name, const Char_t * title = "");
    KVBase(const KVBase &);
    virtual ~ KVBase();

   inline void SetType(const Char_t * str) {
      SetTitle(str);
   };
   const Char_t *GetType() const {
      return GetTitle();
   };
   virtual Bool_t IsType(const Char_t* typ) const
   {
      // Returns true if GetType() returns the given type name
      return (fTitle==typ);
   };
   void SetLabel(const Char_t * lab);
   const Char_t *GetLabel() const;
   virtual void Print(Option_t * option = "") const;
   virtual void List() {
   };
   static const Char_t *GetKVRoot();
   virtual void SetNumber(UInt_t num) {
      fNumber = num;
   };
   UInt_t GetNumber() const {
      return fNumber;
   };
   UInt_t GetNumberOfObjects() const {
      return fNbObj;
   };
   virtual void Clear(Option_t * opt = "");
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif

   static Bool_t ArrContainsValue(Int_t n, Int_t * arr, Int_t val);
   static Bool_t SearchKVFile(const Char_t * name, TString & fullpath,
                              const Char_t * kvsubdir = "");
   static Bool_t SearchAndOpenKVFile(const Char_t * name, ifstream & file,
                                     const Char_t * kvsubdir = "", KVLockfile* locks = 0);
   static Bool_t SearchAndOpenKVFile(const Char_t * name, ofstream & file,
                                     const Char_t * kvsubdir = "", KVLockfile* locks = 0);
   static void BackupFileWithDate(const Char_t * path);
   static TPluginHandler *LoadPlugin(const Char_t * base,
                                     const Char_t * uri = "0");
   static const Char_t* GetPluginURI(const Char_t* base, const Char_t* plugin);
	static const Char_t* GetListOfPlugins(const Char_t* base);
   static void OpenTempFile(TString & base, ofstream & fp);     /* open temp file with basename 'base' */
   static void GetTempFileName(TString & base);

   static const Char_t *GetKVVersion();
   static const Char_t *GetKVBuildDate();
   static const Char_t *GetKVBuildUser();
   static const Char_t *GetKVSourceDir();

   static const Char_t *GetKVRootDir(void);
   static const Char_t *GetKVBinDir(void);
   static const Char_t *GetKVFilesDir(void);

   static Bool_t FindExecutable(TString & exec, const Char_t * path =
                                "$(PATH)");

   static const Char_t *FindFile(const Char_t * search, TString & wfil);
   static Bool_t FindClassSourceFiles(const Char_t* class_name, KVString& imp_file,
         KVString& dec_file, const Char_t* dir_name=".");


   ClassDef(KVBase, 3)          //Base class for all KaliVeda objects
};

//this function is implemented in TSystem.cxx
//it can be used with TSystem::ConcatFileName to mop up the returned char* pointer
extern void AssignAndDelete(TString & target, char *tobedeleted);
//Search for file in an arbitrary number of locations, return kTRUE if file found and put full path to file in 'fullpath"
Bool_t SearchFile(const Char_t * name, TString & fullpath, int ndirs, ...);
#endif
