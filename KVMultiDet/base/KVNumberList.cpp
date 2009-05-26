#include "KVNumberList.h"
#include "KVString.h"
#include <TObjArray.h>
#include <TObjString.h>
#include <Riostream.h>
#include "KVError.h"
#include "TMath.h"

ClassImp(KVNumberList)
//////////////////////////////////////////////
//KVNumberList
//
//Handles lists of positive numbers such as "1-12 14 15-17" or "345,356-390'
//Strings with this format are handled and analysed by this class.
//
//Examples:
//
//Create a new list with the constructor:
//
//KVNumberList a("1-20, 51, 52-56")
//
//Change an existing list:
//
//a.SetList("1001 1003-1005,1010")
//
//Get first and last values of ranges in list (i.e. largest and smallest included values)
//
//a.First()   (would give 1001 for previous example)
//a.Last()   (would give 1010 for previous example)
//
//Get an array filled with all values corresponding to ranges defined in list:
//
//Int_t n
//Int_t* val = a.GetArray(n)
//
//val[0] is same value as 'a.First()'
//val[n-1] is same value as 'a.Last()
//
//Use AsString() or GetList() method to obtain list in its most compact form, using the "1-20, 51, 52-56" format.
//
//IsEmpty() returns kTRUE if the list is empty ;-p
//
   //Iterating over all numbers in the list:
   //
   //Initialise first by calling Begin(), then loop until End() returns kTRUE:
   //
   // KVNumberList r("1-10");
   // r.Begin();
   // while( !r.End() ){
   //    Int_t next_val = r.Next();
   //    ...
   // }
   //If list is empty, End() always returns kTRUE and Next() returns -1.
   
//____________________________________________________________________________________________//

void KVNumberList::init_numberlist()
{
   //Default initialisation used by ctors
   fMaxNLimits = 0;
   fNLimits = 0;
   fLowerBounds = new TArrayI();
   fUpperBounds = new TArrayI();
   fFirstValue = 99999999;
   fLastValue = -99999999;
   fNValues = 0;
   fValues = 0;
}

//____________________________________________________________________________________________//

KVNumberList::KVNumberList():fString()
{
   //Default constructor
   init_numberlist();
}

//____________________________________________________________________________________________//

KVNumberList::KVNumberList(const KVNumberList & orig)
{
   //Copy constructor
   init_numberlist();
   fString = orig.fString;
   ParseList();
}

//____________________________________________________________________________________________//

KVNumberList::KVNumberList(const Char_t * list):fString(list)
{
   //Initialise number list using string and parse it to fill limits arrays
   //Any number will only appear once.
   init_numberlist();
   ParseList();
}

//____________________________________________________________________________________________//

KVNumberList::~KVNumberList()
{
   //delete arrays
   delete fLowerBounds;
   delete fUpperBounds;
   if(fValues) delete [] fValues;
}

//____________________________________________________________________________________________//

void KVNumberList::ParseList()
{
   //PRIVATE METHOD
   //Breaks string containing list down and fills limits arrays accordingly
   Clear();
   ParseAndFindLimits(fString, ' ');
}

//____________________________________________________________________________________________//

void KVNumberList::Clear()
{
   //Empty number list, reset it to initial state.
   fNLimits = 0;
   fLowerBounds->Reset();
   fUpperBounds->Reset();
   fFirstValue = 99999999;
   fLastValue = -99999999;
   fNValues = 0;
}

//____________________________________________________________________________________________//

void KVNumberList::ParseAndFindLimits(TString & string, const Char_t delim)
{
   //Takes a string and breaks it up into its constituent parts,
   //which were initially separated by white space or a comma.
   //Any part which contains "-" will be sent to AddLimits().
   TObjArray *toks1 = string.Tokenize(delim);
   Int_t n_toks = toks1->GetEntries();
   for (register int i = 0; i < n_toks; i++) {
      TString tok = ((TObjString *) (*toks1)[i])->GetString();
      KVString kvtok(tok);
      if (tok.Contains(','))
         ParseAndFindLimits(tok, ',');
      else if (tok.Contains(' '))
         ParseAndFindLimits(tok, ' ');
      else if (tok.Contains('-')) {
         AddLimits(tok);
      } else if (kvtok.IsDigit()) {
         Int_t val = kvtok.Atoi();
         AddLimits(val, val);
      }
   }
   delete toks1;
}

//____________________________________________________________________________________________//

void KVNumberList::AddLimits(TString & string)
{
   //'string' should contain something like "34-59" i.e. two integers separated by a '-'.
   //these two numbers are taken for new lower and upper limits, fNLimits is increased by one,
   //if necessary the arrays' size is increased.
   TObjArray *toks1 = string.Tokenize('-');
   Int_t n_toks = toks1->GetEntries();
   if (n_toks != 2) {
      delete toks1;
      return;
   }
   KVString lower(((TObjString *) (*toks1)[0])->GetString());
   KVString upper(((TObjString *) (*toks1)[1])->GetString());
   delete toks1;
   Int_t ilow = lower.Atoi();
   Int_t iupp = upper.Atoi();
   AddLimits(ilow, iupp);
}

//____________________________________________________________________________________________//

void KVNumberList::AddLimits(Int_t min, Int_t max)
{
   //The numbers contained in the range [min,max]
   //are added to the list.
   if (++fNLimits > fMaxNLimits) {
      fMaxNLimits += 10;
      fLowerBounds->Set(fMaxNLimits);
      fUpperBounds->Set(fMaxNLimits);
   }
   (*fLowerBounds)[fNLimits - 1] = min;
   (*fUpperBounds)[fNLimits - 1] = max;
   if (min < fFirstValue)
      fFirstValue = min;
   if (max > fLastValue)
      fLastValue = max;
   fNValues += (max - min + 1);
}

//____________________________________________________________________________________________//

void KVNumberList::PrintLimits()
{
   //Print detailed break-down of list
   
   //remove duplicate entries, correct fList, then parse
   GetList();
   ParseList();
   
   cout << "KVNumberList" << endl;
   cout << "There are " << fNLimits << " limits in the string : " <<
       fString.Data() << endl;
   cout << "MIN = ";
   for (int i = 0; i < fNLimits; i++) {
      cout << Form("%5d", (*fLowerBounds)[i]);
      if (i < fNLimits - 1)
         cout << ",";
   }
   cout << endl;
   cout << "MAX = ";
   for (int i = 0; i < fNLimits; i++) {
      cout << Form("%5d", (*fUpperBounds)[i]);
      if (i < fNLimits - 1)
         cout << ",";
   }
   cout << endl;
   cout << "First value = " << fFirstValue << "  Last value = " <<
       fLastValue << endl;
}

//____________________________________________________________________________________________//

void KVNumberList::SetList(const Char_t * list)
{
   //Analyse list contained in the string given as argument,
   //use it to replace any previously defined list.
   //New ranges are calculated automatically.
   //Any number will only appear once.
   fString = list;
   ParseList();
}

//____________________________________________________________________________________________//

void KVNumberList::SetList(TString & list)
{
   //Analyse list contained in the string given as argument,
   //use it to replace any previously defined list.
   //New ranges are calculated automatically.
   //Any number will only appear once.
   SetList(list.Data());
}

//____________________________________________________________________________________________//

Bool_t KVNumberList::Contains(Int_t val) const
{
   //returns kTRUE if the value 'val' is contained in the ranges defined by the number list
   for (register int i = 0; i < fNLimits; i++) {
      if (val >= (*fLowerBounds)[i] && val <= (*fUpperBounds)[i])
         return kTRUE;
   }
   return kFALSE;
}

//____________________________________________________________________________________________//

Int_t KVNumberList::First()
{
   //Returns smallest number included in list
   return fFirstValue;
}

//____________________________________________________________________________________________//

Int_t KVNumberList::Last()
{
   //Returns largest number included in list
   return fLastValue;
}

//____________________________________________________________________________________________//

Int_t *KVNumberList::GetArray(Int_t & size)
{
   //Creates and fills a sorted array with all the unique
   //values compatible with the ranges defined in the list.
   //(Sorting is in increasing order).
   //'size' is the size of the created array.
   //USER MUST DELETE ARRAY AFTER USE.
   //Returned pointer is 0 if list is empty.

   if (IsEmpty())
      return 0;

   Int_t *temp = new Int_t[fNValues]; size=fNValues;
   Int_t index = 0;
   for (register int i = 0; i < fNLimits; i++) {
      Int_t min = (*fLowerBounds)[i];
      Int_t max = (*fUpperBounds)[i];
      for (register int j = min; j <= max; j++) {
         temp[index++] = j;
      }
   }
   //now check for duplicate entries
   //we sort the array in increasing order
   //any duplicate entries will then be adjacent
   Int_t *ind = new Int_t[fNValues];
   TMath::Sort(fNValues, temp, ind, kFALSE);  
   Int_t n_uniq=1; //number of unique values
   for (register int i = 0; i < fNValues; i++) {
      if(i && (temp[ind[i]] != temp[ind[i-1]]) ) ++n_uniq;
   }
      
   Int_t *temp2 = new Int_t[n_uniq];
   size=n_uniq; 
   if(n_uniq<fNValues){
      n_uniq=1; fString="";
      temp2[0]=temp[ind[0]];
      fString+=temp2[0];
      for (register int i = 0; i < fNValues; i++) {
         if(i && (temp[ind[i]] != temp[ind[i-1]]) ){
            temp2[n_uniq++]=temp[ind[i]];
            fString+=" "; fString+=temp[ind[i]];
         }
      }
      fNValues=size;
      ParseList();
   } else {
      for (register int i = 0; i < fNValues; i++) {
         temp2[i]=temp[ind[i]];
      }
   }
   delete [] temp; delete [] ind;
   return temp2;
}

//____________________________________________________________________________________________//

void KVNumberList::Add(Int_t n)
{
   //Add value 'n' to the list
   if(!fNValues) fString="";
   TString tmp = (fString != "" ? fString + " " : fString);
   tmp += n;
   SetList(tmp);
}

//____________________________________________________________________________________________//

void KVNumberList::Remove(Int_t n)
{
   //Remove value 'n' to the list
   TString tmp = " "+TString(GetExpandedList())+" ";
   tmp.ReplaceAll(Form(" %d ",n)," ");
	SetList(tmp);
}

//____________________________________________________________________________________________//

void KVNumberList::Add(const KVNumberList& list)
{
   //Add values in 'list' to this list
   if(!fNValues) fString="";
   TString tmp = (fString != "" ? fString + " " : fString);
   tmp += list.fString;
   SetList(tmp);
}

//____________________________________________________________________________________________//

void KVNumberList::Remove(const KVNumberList& list)
{
   //Remove values in 'list' to this list
   TString tmp = " "+TString(GetExpandedList())+" ";
	KVNumberList tampon(list);
	if (!tampon.IsEmpty()) {
		tampon.Begin(); 
		while (!tampon.End()) tmp.ReplaceAll(Form(" %d ",tampon.Next())," "); 
	}
	SetList(tmp);
}

//____________________________________________________________________________________________//

void KVNumberList::Add(const Char_t* list)
{
   //Add values in 'list' to this list
   KVNumberList tmp(list);
   Add(tmp);
}

//____________________________________________________________________________________________//

void KVNumberList::Remove(const Char_t* list)
{
   //Remove values in 'list' to this list
   KVNumberList tmp(list);
   Remove(tmp);
}

//____________________________________________________________________________________________//

void KVNumberList::Add(Int_t n, Int_t * arr)
{
   //Add n values from array arr to the list
   if(!fNValues) fString="";
   TString tmp = (fString != "" ? fString + " " : fString);
   for (register int i = 0; i < n; i++) {
         tmp += arr[i];
         tmp += " ";
   }
   SetList(tmp);
}

//____________________________________________________________________________________________//

void KVNumberList::Remove(Int_t n, Int_t * arr)
{
   //Remove n values from array arr to the list
   TString tmp = " ";
   for (register int i = 0; i < n; i++) {
         tmp += arr[i];
         tmp += " ";
   }
   Remove(tmp);
}

//____________________________________________________________________________________________//

void KVNumberList::SetMinMax(Int_t min, Int_t max)
{
   //Set list with all values from 'min' to 'max'
   TString tmp;
   for (register int i = min; i <= max; i++) {
      tmp += i;
      tmp += " ";
   }
   SetList(tmp);
}

//____________________________________________________________________________________________//

void KVNumberList::Inter(const KVNumberList& list)
{
	//keep the AND logic operation result between 'list' and this list
	KVNumberList tampon(list);
	if ( tampon.IsEmpty() || IsEmpty() ) {SetList("");}
	else {
		TString tmp="";
		tampon.Begin(); 
		while (!tampon.End()) { Int_t n=tampon.Next(); if (!Contains(n)) tmp+=n; tmp+=" "; }
		Begin(); 
		while (!End()) { Int_t n=Next(); if (!tampon.Contains(n)) tmp+=n; tmp+=" "; }
		Remove(tmp);
	}
}

//____________________________________________________________________________________________//

const Char_t *KVNumberList::GetList()
{
   //Get string containing list. This is most compact representation possible,
   //i.e. all continuous ranges are represented as "minval-maxval"
   //This string will become the new internal representation of the list.
   //Returns empty string if list is empty.

   //no numbers in list  ?
   if (!fNValues) {
      fString = "";
      return fString.Data();
   }
   //get array of all values
   Int_t n;
   Int_t *arr = GetArray(n);
   Int_t min, max;
   min = max = arr[0];   //put min & max = smallest (first) value to start with
   fString = "";
   for (register int i = 1; i < n; i++) {

      Int_t val = arr[i];        // loop over values in increasing order

         if (val - arr[i - 1] > 1) {
            //cout << "end of continuous range ?" << endl;
            if (min != max) {
               fString += Form("%d-%d ", min, max);
            } else {
               fString += Form("%d ", min);
            }
            min = max = val;
         } else {
            //cout << "continuous range" << endl;
            max = val;
         }
      
   }
   if (min != max) {
      fString += Form("%d-%d", min, max);
   } else {
      fString += Form("%d", min);
   }
   delete[]arr;
   return fString.Data();
}

//____________________________________________________________________________________________//

const Char_t *KVNumberList::GetExpandedList()
{
   //Get string containing list. Every unique value contained
   //in the list will be represented.
   //Returns empty string if list is empty.

   //no numbers in list  ?
   if (!fNValues) {
      fString = "";
      return fString.Data();
   }
   //get array of all values
   Int_t n;
   Int_t *arr = GetArray(n);
   fString = "";
   for (register int i = 0; i < n - 1; i++) {

      Int_t val = arr[i];        // loop over values in increasing order
      fString += Form("%d ", val);
   }
   Int_t val = arr[n - 1];       //last value
   fString += Form("%d", val);
   delete[]arr;
   return fString.Data();
}

//____________________________________________________________________________________________//

const Char_t *KVNumberList::GetLogical(const Char_t *observable)
{
	// Get logical expression of 'this' list in the TTree:Draw condition format
	// observable is one of the leaf of the TTree
	// 12-15 20 --> ( 12<=observable&&observable<=15 || observable==20 )
	// return "" if 'this' list  is empty
	
	if (IsEmpty()) return "";
	GetList();
	TString tmp = fString;
	tmp.ReplaceAll(" ","||");
	TObjArray *toks = tmp.Tokenize("||");
	TString cond="( ";
	Int_t nt = toks->GetEntries();
	for (Int_t ii=0;ii<nt;ii+=1) {
		TString line = ((TObjString*)(*toks)[ii])->GetString();
		if ( line.Contains("-") ) { line.ReplaceAll("-",Form("<=%s&&%s<=",observable,observable)); cond+=line; }
		else { cond+=Form("%s==",observable)+line; }
		if (ii!=nt-1) cond+="||";
	}
	cond += " )";
	delete toks;
	return cond.Data();
}

//____________________________________________________________________________________________//

KVNumberList & KVNumberList::operator=(const KVNumberList & val)
{
   //Set this number list equal to val
   fString = val.fString;
   ParseList();
   return (*this);
}

//____________________________________________________________________________________________//

Int_t KVNumberList::GetNValues()
{
   //Returns total number of unique entries in list
   //Note that this calls GetArray() just in order to remove
   //any duplicate entries in the list and make sure fNValues
   //is the number of unique entries.
   
   Int_t n;
   Int_t *j=GetArray(n);//will remove any duplicates and correct fNValues
   delete [] j;
   return fNValues;
}

//____________________________________________________________________________________________//

Int_t KVNumberList::Next()
{
   //Use this method to iterate over all numbers in the list
   //Initialise first by calling Begin(), then loop until End() returns kTRUE:
   //
   // KVNumberList r("1-10");
   // r.Begin();
   // while( !r.End() ){
   //    Int_t next_val = r.Next();
   //    ...
   // }
   //If list is empty, End() always returns kTRUE and Next() returns -1.
   
   if(!fValues){
      Warning( KV__ERROR(Next), "List is empty. -1 returned.");
      return -1;
   }
   Int_t val = fValues[fIterIndex];
   fEndList=(fIterIndex==fNValues-1);
   fIterIndex=TMath::Min(fNValues-1,fIterIndex+1);
   return val;
}

//____________________________________________________________________________________________//

void KVNumberList::Begin()
{
   //Call before using Next(). Resets iterator to beginning of list.
   //If list is empty, End() always returns kTRUE and Next() returns -1.
   fEndList = kFALSE;
   fIterIndex = 0;
   Int_t n;//dummy
   if(fValues) delete [] fValues;
   fValues = GetArray(n);
   if(!n || !fValues){
      //list is empty
      fEndList = kTRUE;
   }
}

//____________________________________________________________________________________________//

Int_t KVNumberList::At(Int_t index)
{
   // Returns value of number in list at position 'index' (index = 0, 1, .., GetNValues()-1)
   // We check that 'index' is neither too big nor too small (otherwise we return -1).
   // WARNING: the list is first sorted into ascending order (and any duplicate entries are
   // removed), so the index does not necessarily correspond to the order in which numbers
   // are added to the list.
   
   Int_t n;
   Int_t *arr = GetArray(n);
   if( index<0 || index>=n ){
      Warning( KV__ERROR(At), "Index out of bounds. -1 returned.");
      return -1;
   }
   n = arr[index];
   delete [] arr;
   return n;
}

//____________________________________________________________________________________________//

Int_t KVNumberList::operator[](Int_t index)
{
   // Returns value of number in list at position 'index' (index = 0, 1, .., GetNValues()-1)
   // We check that 'index' is neither too big nor too small (otherwise we return -1).
   // WARNING: the list is first sorted into ascending order (and any duplicate entries are
   // removed), so the index does not necessarily correspond to the order in which numbers
   // are added to the list.
   
   return At(index);
}

//____________________________________________________________________________________________//

const Char_t *KVNumberList::AsString(Int_t maxlen)
{
   //Get string containing list. This is most compact representation possible,
   //i.e. all continuous ranges are represented as "minval-maxval".
   //If maxlen>0, and if length of resulting string is longer than maxlen,
   //we truncate the list to show only the beginning and the end of the list,
   //with "..." in between, i.e. "6000-...-8910". Note that the minimum size of
   //the resulting string is 5 i.e. "6...0".
   //Returns empty string if list is empty.
   
   static TString tmp;
   tmp = GetList();
   if( maxlen ){
      maxlen = TMath::Max(maxlen,5);
      if( tmp.Length()>maxlen ){
         Int_t len_left = maxlen-3;// 3 for "..."
         Int_t len_start = len_left/2;Int_t len_end = len_left-len_start;
         TString tmp2 = tmp(0,len_start);
         tmp2+="...";
         tmp2+=tmp(tmp.Length()-len_end, tmp.Length()-1);
         tmp = tmp2;
      }
   }
   return tmp.Data();
}

//____________________________________________________________________________________________//

