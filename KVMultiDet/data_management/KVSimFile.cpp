//Created by KVClassFactory on Tue Jul 17 09:05:17 2012
//Author: John Frankland,,,

#include "KVSimFile.h"
#include <iostream>

ClassImp(KVSimFile)

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimFile</h2>
<h4>Handle file containing simulated and/or filtered simulated data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSimFile::KVSimFile()
{
   // Default constructor
   fFiltered=kFALSE;
}

KVSimFile::KVSimFile(const Char_t* filename, const Char_t* treeinfo, Long64_t treeEntries)
      : KVBase(filename,treeinfo), fFiltered(kFALSE), fEvents(treeEntries)
{
   // Default constructor for simulated events file
}
   
KVSimFile::KVSimFile(const Char_t* filename, const Char_t* treeinfo, Long64_t treeEntries,
                        const Char_t* dataset, const Char_t* system, Int_t run_number, const Char_t* geo_type)
      : KVBase(filename,treeinfo), fFiltered(kTRUE), fEvents(treeEntries), fDataSet(dataset), fSystem(system),
      fRunNumber(run_number), fGeoType(geo_type)
{
   // Default constructor for filtered (reconstructed) simulated events file
}
//________________________________________________________________

KVSimFile::KVSimFile (const KVSimFile& obj)  : KVBase()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}

KVSimFile::~KVSimFile()
{
   // Destructor
}

//________________________________________________________________

void KVSimFile::Copy (TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVSimFile::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVBase::Copy(obj);
   //KVSimFile& CastedObj = (KVSimFile&)obj;
}

void KVSimFile::ls(Option_t*) const
{
      TROOT::IndentLevel();
      if(!fFiltered) cout << "SIMULATED EVENTS FILE : " << GetName() <<endl;
      else cout << "FILTERED EVENTS FILE : " << GetName() <<endl;
      cout << "--infos: " << GetTitle() << endl;
      cout << "--events: " << fEvents << endl;
      if(fFiltered){
         cout << "--Filtering conditions:" << endl;
         cout << "----dataset:" << fDataSet << endl;
         cout << "----system:" << fSystem << endl;
         cout << "----run:" << fRunNumber << endl;
         cout << "----geometry:" << fGeoType << endl;
      }
}   
