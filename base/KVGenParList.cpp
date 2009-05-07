/*
$Id: KVGenParList.cpp,v 1.3 2009/05/07 12:21:33 ebonnet Exp $
$Revision: 1.3 $
$Date: 2009/05/07 12:21:33 $
*/

//Created by KVClassFactory on Tue Oct 23 15:21:35 2007
//Author: ljohn

#include "KVGenParList.h"

ClassImp(KVGenParList)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVGenParList</h2>
<h4>A list of named parameters which can be either strings, doubles or integers</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVGenParList::KVGenParList()
{
   // Default constructor
}

KVGenParList::~KVGenParList()
{
   // Destructor
}

//______________________________________________________________________________

KVGenParList::KVGenParList(const KVGenParList& obj)
{
   // Copy constructor
   
   obj.Copy( *this );
}

//______________________________________________________________________________

void KVGenParList::Copy(KVGenParList& obj) const
{
   // Replaces any existing parameters in list 'obj' with all those defined in this list.
   
   KVParameterList<KVString>::Copy( (KVParameterList<KVString>&)obj );
}

//______________________________________________________________________________

const Char_t* KVGenParList::GetStringValue(const Char_t* name)
{
   // Return string containing value of named parameter
   // Returns empty string if parameter is not defined.
   
   if( !HasParameter(name) ) return "";
   return GetParameter(name).Data();
}

//______________________________________________________________________________

Int_t KVGenParList::GetIntValue(const Char_t* name)
{
   // Return integer value of named parameter
   // Returns 0 if parameter is not defined, or if value is non-numeric.
   
   if( !HasParameter(name) ) return 0;
   KVString tmp = GetParameter(name);
   if( tmp.IsFloat() ) return (Int_t)tmp.Atof();
   return 0;
}

//______________________________________________________________________________

Double_t KVGenParList::GetDoubleValue(const Char_t* name)
{
   // Return floating-point value of named parameter
   // Returns 0 if parameter is not defined, or if value is non-numeric.
   
   if( !HasParameter(name) ) return 0;
   KVString tmp = GetParameter(name);
   if( tmp.IsFloat() ) return tmp.Atof();
   return 0;
}

//______________________________________________________________________________

void KVGenParList::SetValue(const Char_t* name, const KVString& val)
{
   // Set value of parameter "name" to be the string "val".
   // New parameter is created if "name" does not exist.
   
   SetParameter(name, val);
}

//______________________________________________________________________________

void KVGenParList::SetValue(const Char_t* name, KVString& val)
{
   // Set value of parameter "name" to be the string "val".
   // New parameter is created if "name" does not exist.
   
   SetParameter(name, val);
}

//______________________________________________________________________________

void KVGenParList::SetValue(const Char_t* name, const Char_t* val)
{
   // Set value of parameter "name" to be the string "val".
   // New parameter is created if "name" does not exist.
   
   KVString tmp(val);
   SetParameter(name, tmp);
}

//______________________________________________________________________________

void KVGenParList::SetValue(const Char_t* name, Int_t val)
{
   // Set value of parameter "name" to be the integer value "val".
   // New parameter is created if "name" does not exist.
   
   KVString tmp;
   tmp.Form("%d", val); // write value in temporary string
   SetParameter(name, tmp);
}

//______________________________________________________________________________

void KVGenParList::SetValue(const Char_t* name, Double_t val)
{
   // Set value of parameter "name" to be the floating-point value "val".
   // New parameter is created if "name" does not exist.
   
   KVString tmp;
   tmp.Form("%g", val); // write value in temporary string
   SetParameter(name, tmp);
}

//______________________________________________________________________________

void KVGenParList::IncrementValue(const Char_t* name, Int_t val)
{
   // Increment parameter value
   // New parameter is created if "name" does not exist.
   
   KVString tmp;
   tmp.Form("%d", val+GetIntValue(name)); // write value in temporary string
   SetParameter(name, tmp);
}

//______________________________________________________________________________

void KVGenParList::IncrementValue(const Char_t* name, Double_t val)
{
   // Increment parameter value
   // New parameter is created if "name" does not exist.
   
   KVString tmp;
   tmp.Form("%g", val+GetDoubleValue(name)); // write value in temporary string
   SetParameter(name, tmp);
}
