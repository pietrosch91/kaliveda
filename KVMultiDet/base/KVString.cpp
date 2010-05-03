//$Id: KVString.cpp,v 1.15 2008/04/04 13:08:00 franklan Exp $

#include "KVString.h"
#include "Riostream.h"
#include "TObjString.h"
#include <list>
#include <stdlib.h>
#include <ctype.h>

#include "snprintf.h"
#include "TBuffer.h"
#include "TError.h"
#include "Bytes.h"
#include "TClass.h"
#include <list>

#ifdef R__GLOBALSTL
namespace std {
   using::list;
}
#endif
ClassImp(KVString)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KVString
//
// Allows backwards compatibility with ROOT v3.10/02 onwards.
// TString::Tokenize and IsDigit methods are added here if they don't exist
// in the version of ROOT being used.
//
// IsFloat() method is added: returns true if string is a floating-point number, i.e. if it
//              has any of following formats (only first one returns kTRUE with TString::IsDigit()):
//      64320
//      6.4320  6,4320
//      6.43e20   6.43E20 6,43e20
//      6.43e-20  6.43E-20 6,43e-20
//              
// Also Atoi() and Atof() are extended to include the following possibilities:
//
// Atoi(): with string="123 456", TString::Atoi() gives value=123
//              KVString::Atoi() gives value=123456
// Atof(): with string="12,34", TString::Atof() gives value=12
//              KVString::Atof() gives value=12.34
//
// Begin(), Next() and End() can be used to loop over items in
// a string separated by the delimiter character given as argument
// to Begin().
//
// Example:
//   KVString str("First | Second | Third");
//   str.Begin("|");
//   while( !str.End() ){
//     cout << str.Next().Data() << endl;
//   }
//
// This will give the following output:
//
// First
//  Second
//  Third
//
// or, if Next is called with argument strip_whitespace=kTRUE,
//
// First
// Second
// Third
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __WITHOUT_TSTRING_TOKENIZE
TObjArray *KVString::Tokenize(const TString & delim) const
{
   // This function is used to isolate sequential tokens in a TString.
   // These tokens are separated in the string by at least one of the
   // characters in delim. The returned array contains the tokens
   // as TObjString's. The returned array is the owner of the objects,
   // and must be deleted by the user.

   std::list < Int_t > splitIndex;

   Int_t i, start, nrDiff = 0;
   for (i = 0; i < delim.Length(); i++) {
      start = 0;
      while (start < Length()) {
         Int_t pos = Index(delim(i), start);
         if (pos == kNPOS)
            break;
         splitIndex.push_back(pos);
         start = pos + 1;
      }
      if (start > 0)
         nrDiff++;
   }
   splitIndex.push_back(Length());

   if (nrDiff > 1)
      splitIndex.sort();

   TObjArray *arr = new TObjArray();
   arr->SetOwner();

   start = -1;
   std::list < Int_t >::const_iterator it;
#ifndef R__HPUX
   for (it = splitIndex.begin(); it != splitIndex.end(); it++) {
#else
   for (it = splitIndex.begin();
        it != (std::list < Int_t >::const_iterator) splitIndex.end();
        it++) {
#endif
      Int_t stop = *it;
      if (stop - 1 >= start + 1) {
         TString tok = (*this) (start + 1, stop - start - 1);
         TObjString *objstr = new TObjString(tok);
         arr->Add(objstr);
      }
      start = stop;
   }

   return arr;
}
#endif

#ifdef __WITH_KVSTRING_ISDIGIT
Bool_t KVString::IsDigit() const
{
   // Returns true if all characters in string are digits (0-9) or whitespaces,
   // i.e. "123456" and "123 456" are both valid integer strings.
   // Returns false in case string length is 0 or string contains other
   // characters or only whitespace.

   const char *cp = Data();
   Ssiz_t len = Length();
   if (len == 0) return kFALSE;
   Int_t b = 0, d = 0;
   for (Ssiz_t i = 0; i < len; ++i) {
      if (cp[i] != ' ' && !isdigit(cp[i])) return kFALSE;
      if (cp[i] == ' ') b++;
      if (isdigit(cp[i])) d++;
   }
   if (b && !d)
      return kFALSE;
   return kTRUE;
}
#endif

#ifdef __WITH_KVSTRING_REMOVE
KVString & KVString::Remove(TString::EStripType st, char c)
{
   // Remove char c at begin and/or end of string (like Strip() but
   // modifies directly the string.
   *this = Strip(st, c);
   return *this;
}
#endif

#ifdef __WITH_KVSTRING_ATOI
Int_t KVString::Atoi() const
{
   //Enhanced version of TString::Atoi().
   //Atoi(): with string="123 456", TString::Atoi() gives value=123
   //              KVString::Atoi() gives value=123456

   Int_t end = Index(" ");
   //if no whitespaces in string, just use atoi()
   if (end == -1)
      return atoi(Data());

   Int_t start = 0;
   TString tmp;
   while (end > -1) {
      tmp += (*this) (start, end - start);
      start = end + 1;
      end = Index(" ", start);
   }
   end = Length();
   tmp += (*this) (start, end - start);
   return atoi(tmp.Data());
}
#endif

#ifdef __WITH_KVSTRING_ATOF
Double_t KVString::Atof() const
{
   //Enhanced version of TString::Atof().
   //Atof(): with string="12,34", TString::Atof() gives value=12
   //              KVString::Atof() gives value=12.34

   //look for a comma and some whitespace
   Int_t comma = Index(",");
   Int_t end = Index(" ");
   //if no commas & no whitespace in string, just use atof()
   if (comma == -1 && end == -1)
      return atof(Data());
   TString tmp = *this;
   if (comma > -1) {
      //replace comma with full stop
      tmp.Replace(comma, 1, ".");
   }
   //no whitespace ?
   if (end == -1)
      return atof(tmp.Data());
   //remove whitespace
   Int_t start = 0;
   TString tmp2;
   while (end > -1) {
      tmp2 += tmp(start, end - start);
      start = end + 1;
      end = tmp.Index(" ", start);
   }
   end = tmp.Length();
   tmp2 += tmp(start, end - start);
   return atof(tmp2.Data());
}
#endif

#ifdef __WITH_KVSTRING_ISFLOAT
Bool_t KVString::IsFloat() const
{
   //Returns kTRUE if string contains a floating point or integer number
   // Examples of valid formats are:
   //    64320
   //    64 320
   //    6 4 3 2 0
   //    6.4320     6,4320
   //    6.43e20   6.43E20    6,43e20
   //    6.43e-20  6.43E-20   6,43e-20

   //we first check if we have an integer, in this case, IsDigit() will be true straight away
   if (IsDigit()) return kTRUE;

   TString tmp = *this;
   //now we look for occurrences of '.', ',', e', 'E', '+', '-' and replace each
   //with ' '. if it is a floating point, IsDigit() will then return kTRUE
   Int_t i_dot, i_e, i_plus, i_minus, i_comma;
   i_dot = i_e = i_plus = i_minus = i_comma = -1;

   i_dot = tmp.First('.');
   if (i_dot > -1) tmp.Replace(i_dot, 1, " ", 1);
   i_comma = tmp.First(',');
   if (i_comma > -1) tmp.Replace(i_comma, 1, " ", 1);
   i_e = tmp.First('e');
   if (i_e > -1)
      tmp.Replace(i_e, 1, " ", 1);
   else {
      //try for a capital "E"
      i_e = tmp.First('E');
      if (i_e > -1) tmp.Replace(i_e, 1, " ", 1);
   }
   i_plus = tmp.First('+');
   if (i_plus > -1) tmp.Replace(i_plus, 1, " ", 1);
   i_minus = tmp.First('-');
   if (i_minus > -1) tmp.Replace(i_minus, 1, " ", 1);

   //test if it is now uniquely composed of numbers
   return tmp.IsDigit();
}
#endif

KVString & KVString::Substitute(const Char_t c1, const Char_t c2)
{
   //Replace every occurence of 'c1' with 'c2'
   ReplaceAll(&c1, 1, &c2, 1);
   return (*this);
}

#ifdef __WITH_KVSTRING_ISWHITESPACE
Bool_t KVString::IsWhitespace() const
{
   //Returns kTRUE if string is empty (IsNull()) or if every character = ' '
   return (Length() == CountChar(' '));
}
#endif

Int_t KVString::Sscanf(const Char_t * fmt, ...)
{
   //A more strict implementation than the standard 'sscanf'
   //We require that any characters in the format string which are not format descriptors
   //("%d" etc.) be identical to the characters in this KVString
   //
   //i.e. for a string "file6.root", sscanf("file6.root", "file%d.root", &some_integer) returns 1
   //but so does sscanf("file6.root", "file%danything_you_want_here_i_dont_care", &some_integer).
   //
   //Use this method when you ONLY want "file%d.root" to match "file6.root" 
   //
   //Returns number of fields read from string
   //
   //HANDLED FORMAT DESCRIPTORS
   //
   //      %d   -  simple integer descriptor
   //      %3d   -  simple integer descriptor with length - only integers of correct length accepted
	//               (leading characters may be white space, i.e. '  4' for 4 written with '%3d')
   //      %03d   -  simple integer descriptor with length + zero padding - only integers of correct length accepted
	//               (leading characters may be zeroes, i.e. '004' for 4 written with '%03d')
   //      %*   -  just garbage, no argument required, it is not read. we ignore the rest of the string
   //                      and the rest of the format. this is not counted as a field to be read. i.e. Sscanf("%*")
   //                      gives 0 for any string, not because it doesn't match, but because there is nothing to read.

   va_list args;
   va_start(args, fmt);

   Int_t str_index = 0;
   Int_t read_items = 0;
   Int_t fmt_index = 0;
   const char *cp = Data();
	Int_t int_format_length_descriptor = 0;
	Bool_t zero_padding = kFALSE;

   while (fmt[fmt_index] != '\0') {

      if (fmt[fmt_index] == '%') {
         //handle format descriptor
         fmt_index++;
         if (fmt[fmt_index] >= '0' && fmt[fmt_index] <= '9') {
				// case of %nd or %0nd with n=some integer
				zero_padding = (fmt[fmt_index]=='0');
				if(zero_padding) fmt_index++;
				// stick together all figures until 'd' (or some other non-number) is found
				KVString length_of_number="";
				while( fmt[fmt_index] >= '0' && fmt[fmt_index] <= '9'){
					length_of_number+=fmt[fmt_index++];
				}
				int_format_length_descriptor = length_of_number.Atoi();
			}
         if (fmt[fmt_index] == 'd') {
            //read an integer
            KVString dummy;
				if(int_format_length_descriptor){
					if(zero_padding){
						// fixed length integer with leading zeroes
						// i.e. for %03d, '3' will be represented by '003'
						// we must read int_format_length_descriptor consecutive integers
						Int_t figures_read=0;
            		while (cp[str_index] >= '0' && cp[str_index] <= '9'){
               		dummy += cp[str_index++];
							figures_read++;
						}
						if(figures_read!=int_format_length_descriptor){
							// number is not correct length, string is not good
            			va_end(args);
            			return 0;
						}
						else
						{
							// good
            			*(va_arg(args, int *)) = dummy.Atoi();
            			fmt_index++;
            			read_items++;
						}
					}
					else
					{
						// fixed length integer with white-space padding
						// i.e. for %3d, '3' will be represented by '  3'
						// we must read int_format_length_descriptor consecutive characters
						// which are either white-space or numbers, at least the last one must
						// be a number, and once we start reading numbers we cannot have any more
						// white space
						Bool_t no_more_whitespace = kFALSE;
						while(int_format_length_descriptor){
							if(cp[str_index]=='\0'){
								// tried to read past end of string - no good
            				va_end(args);
            				return 0;
							}
							if((cp[str_index]!=' ')&&(cp[str_index]<'0'||cp[str_index]>'9')){
								// read a non-whitespace non-number - no good
            				va_end(args);
            				return 0;
							}
							if((cp[str_index]==' ')&&no_more_whitespace){
								// read a whitespace after starting to read numbers - no good
            				va_end(args);
            				return 0;
							}
							if(cp[str_index]!=' '){
								no_more_whitespace=kTRUE;
								dummy+=cp[str_index];
							}
							str_index++;
							int_format_length_descriptor--;
						}
						// check we read at least one number
						if(!no_more_whitespace){
            			va_end(args);
            			return 0;
						}
						// check that next character in string is not a number
						if(cp[str_index+1]!='\0'&&(cp[str_index+1]<'0'||cp[str_index+1]>'9')){
            			va_end(args);
            			return 0;
						}
						// good
            		*(va_arg(args, int *)) = dummy.Atoi();
            		fmt_index++;
            		read_items++;
						
					}
				}
				else
				{
					// any length of integer i.e. '%d'
            	while (cp[str_index] >= '0' && cp[str_index] <= '9')
               	dummy += cp[str_index++];
            	*(va_arg(args, int *)) = dummy.Atoi();
            	fmt_index++;
            	read_items++;
				}
         } else if (fmt[fmt_index] == '*') {
            //rest of string is garbage
            va_end(args);
            return read_items;
         }
      } else {
         //check character in format against string
         if (fmt[fmt_index] != cp[str_index]) {
            va_end(args);
            return 0;           //not the same string, return 0
         }
         fmt_index++;
         str_index++;
      }


   }

   va_end(args);

   //if we haven't got to the end of the string, it must not match the format
   if (cp[str_index] != '\0')
      return 0;

   return read_items;
}


Bool_t KVString::Match(TString pattern)
{
	// Check if pattern fit the considered string
	// As in ls shell command the * symbol represents the non discriminant part
	// of the pattern
	// if no * is present in the pattern, the result correspond to TString::Contains method
	// Example KVString st(file_R45.dat);
	// st.Match("*") -> kTRUE
	// st.Match("file") ->kTRUE
	// st.Match("*file*R*") ->kTRUE
	// etc ....  
	if (!pattern.Contains("*")) return this->Contains(pattern);
	else if (pattern=="*") return kTRUE;
	else {
		TObjArray *tok = pattern.Tokenize("*");
		Int_t n_tok = tok->GetEntries();
		if (!pattern.BeginsWith("*"))
			if ( !this->BeginsWith(((TObjString* )tok->First())->GetString()) ){
				delete tok;
				return kFALSE;
			}
		if (!pattern.EndsWith("*"))
			if ( !this->EndsWith(((TObjString* )tok->Last())->GetString()) ) {
				delete tok;
				return kFALSE;
			}
		
		Int_t idx=0,num=0;
		for (Int_t ii=0;ii<n_tok;ii+=1){
			idx = this->Index( ((TObjString* )tok->At(ii))->GetString() ,idx);
			if (idx!=-1){
				num+=1;
				idx++;
			}
			else break;
		}
      delete tok;
		if (num==n_tok) return kTRUE;
		else return kFALSE;
	}
}

void KVString::Begin(TString delim)
{
	// Begin(), Next() and End() can be used to loop over items in
	// a string separated by the delimiter character given as argument
	// to Begin().
	//
	// Example:
	//   KVString str("First | Second | Third");
	//   str.Begin("|");
	//   while( !str.End() ){
	//     cout << str.Next().Data() << endl;
	//   }
	//
	// This will give the following output:
	//
	// First
	//  Second
	//  Third
	fEndList=kFALSE;
	fIterIndex=0;
	if (IsNull()) { fEndList=kTRUE;}
	else {
		if (kObjArr) delete kObjArr;
		kObjArr = Tokenize(delim);
	   if (!kObjArr || !kObjArr->GetEntries()) { fEndList=kTRUE;}
	}
}

Bool_t KVString::End()
{
	// Begin(), Next() and End() can be used to loop over items in
	// a string separated by the delimiter character given as argument
	// to Begin().
	//
	// Example:
	//   KVString str("First | Second | Third");
	//   str.Begin("|");
	//   while( !str.End() ){
	//     cout << str.Next().Data() << endl;
	//   }
	//
	// This will give the following output:
	//
	// First
	//  Second
	//  Third
	return fEndList;
}

KVString KVString::Next(Bool_t strip_whitespace)
{
	// Begin(), Next() and End() can be used to loop over items in
	// a string separated by the delimiter character given as argument
	// to Begin().
	// If strip_whitespace=kTRUE (default is kFALSE), any leading or
	// trailing whitespace is removed from each item.
	//
	// Example:
	//   KVString str("First | Second | Third");
	//   str.Begin("|");
	//   while( !str.End() ){
	//     cout << str.Next(kTRUE).Data() << endl;
	//   }
	//
	// This will give the following output:
	//
	// First
	// Second
	// Third
	//
	// whereas if Next() is used (i.e. strip_whitespace=kFALSE),
	// this gives:
	//
	// First
	//  Second
	//  Third
	
	KVString st = "";
	if(!kObjArr || !kObjArr->GetEntries() || fIterIndex>=kObjArr->GetEntries()) return st;
	st = ((TObjString* )kObjArr->At(fIterIndex++))->GetString();
	fEndList = (fIterIndex==kObjArr->GetEntries());
	if(strip_whitespace) st.Remove(kBoth,' ');
	return st;
}

Int_t KVString::GetNValues(TString delim)
{
	Int_t nn=0;
	Begin(delim); while (!End()) {KVString dummy=Next(); nn+=1;}
	return nn;
}
