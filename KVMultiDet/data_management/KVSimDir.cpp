//Created by KVClassFactory on Tue Jul 17 08:46:41 2012
//Author: John Frankland,,,

#include "KVSimDir.h"
#include "KVSimFile.h"
#include "TSystemDirectory.h"
#include "TSystem.h"
#include "KVString.h"
#include "TTree.h"
#include "TKey.h"
#include "TFile.h"
#include "TBranchElement.h"
#include <iostream>
using namespace std;

ClassImp(KVSimDir)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimDir</h2>
<h4>Handle directory containing simulated and/or filtered simulated data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimDir::KVSimDir()
{
   // Default constructor
   init();
}

KVSimDir::KVSimDir(const Char_t* name, const Char_t* path)
      : KVBase(name,path)
{
   // Ctor with name and directory to analyse
   init();
}

void KVSimDir::init()
{
   // Default initialisations
   fSimData.SetName("Simulated Data");
   fFiltData.SetName("Filtered Simulated Data");
}

//________________________________________________________________

KVSimDir::KVSimDir (const KVSimDir& obj)  : KVBase()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVSimDir::~KVSimDir()
{
   // Destructor
}

//________________________________________________________________

void KVSimDir::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVSimDir::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVBase::Copy(obj);
   //KVSimDir& CastedObj = (KVSimDir&)obj;
}

//________________________________________________________________

void KVSimDir::AnalyseDirectory ()
{
   // Read contents of directory.
   // Each ROOT file will be opened. If there is a TTree in the file, then we look at
   // all of its branches until we find one containing objects which derive from KVEvent.
   // If they inherit from KVSimEvent, we add the file to the list of simulated data.
   // If they inherit from KVReconstructedEvent, we add the file to the list of filtered data.
   
	//loop over files in current directory
	TSystemDirectory thisDir(".", GetDirectory());
	TList* fileList=thisDir.GetListOfFiles();
	TIter nextFile(fileList);
	TSystemFile* aFile=0;
	while( (aFile = (TSystemFile*)nextFile()) ){
      
      if(aFile->IsDirectory()) continue;
      
		KVString fileName = aFile->GetName();
      if(!fileName.EndsWith(".root"))  continue; /* skip non-ROOT files */
      
      AnalyseFile(fileName);
      
   }
   delete fileList;
}

//________________________________________________________________

void KVSimDir::AnalyseFile (const Char_t* filename)
{
   TString fullpath;
   AssignAndDelete(fullpath, gSystem->ConcatFileName(GetDirectory(),filename));
   TFile* file = TFile::Open(fullpath);
   if(!file || file->IsZombie()) return;
   // look for TTrees in file
   TIter next(file->GetListOfKeys()); TKey*key;
   while( (key=(TKey*)next()) ){
      TString cn = key->GetClassName();
      if(cn=="TTree"){
         // look for branch with KVEvent objects
         TTree* tree = (TTree*)file->Get(key->GetName());
         TSeqCollection* branches = tree->GetListOfBranches();
         TIter nextB(branches);
         TBranchElement* branch;
         while( (branch = (TBranchElement*)nextB()) ){
            TString branch_classname = branch->GetClassName();
            TClass* branch_class = TClass::GetClass(branch_classname,kFALSE,kTRUE);
            if(branch_class && branch_class->InheritsFrom("KVEvent")){
               if(branch_class->InheritsFrom("KVSimEvent")){
                  fSimData.Add( new KVSimFile(filename, tree->GetTitle(), tree->GetEntries()) );
                  delete file;
                  return;
               }
               else if(branch_class->InheritsFrom("KVReconstructedEvent")){
                  // filtered data. there must be TNamed called 'DataSet', 'System', & 'Run' in the file.
                  TNamed* ds = (TNamed*)file->Get("DataSet");
                  TNamed* sys = (TNamed*)file->Get("System");
                  TNamed* r = (TNamed*)file->Get("Run");
                  TNamed* g = (TNamed*)file->Get("Geometry");
                  TString dataset; if(ds) dataset = ds->GetTitle();
                  TString system; if(sys) system = sys->GetTitle();
                  TString run; if(r) run = r->GetTitle();
                  TString geometry; if(g) geometry = g->GetTitle();
                  Int_t run_number = run.Atoi();
                  fFiltData.Add( new KVSimFile(filename, tree->GetTitle(), tree->GetEntries(),
                        dataset, system, run_number, geometry) );
                  delete file;
                  delete ds;
                  delete sys;
                  delete r;
                  return;
               }
            }
         }
      }
   }
}

//________________________________________________________________

void KVSimDir::ls(Option_t*) const
{
      TROOT::IndentLevel();
      cout << "SIMULATION SET: " << GetName() << endl;
      cout << "DIRECTORY: " << GetDirectory() << endl;
      cout <<"CONTENTS:" << endl;
      cout <<"--simulated data:" << endl;
      fSimData.ls();
      cout <<"--filtered data:" << endl;
      fFiltData.ls();
}   
