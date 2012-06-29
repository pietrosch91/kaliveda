//Created by KVClassFactory on Wed Nov 30 13:54:07 2011
//Author: John Frankland,,,

#include "KVNamedParameter.h"
#include "KVBase.h"
#include "Riostream.h"
#include "TROOT.h"

ClassImp(KVNamedParameter)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVNamedParameter</h2>
<h4>A generic parameter with a name and a value</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVNamedParameter::KVNamedParameter() : TNamed(), fNumber(0.0)
{
   // Default constructor
   ResetBits();
}

KVNamedParameter::~KVNamedParameter()
{
   // Destructor
}

KVNamedParameter::KVNamedParameter(const char*nom)
   : TNamed(nom, ""), fNumber(0.0)
{
   ResetBits();
}

KVNamedParameter::KVNamedParameter(const char*nom, const char*val)
   : TNamed(nom, val), fNumber(0.0)
{
   SetType(kIsString);
}

void KVNamedParameter::Set(const char*name, const char*val)
{
   SetNameTitle(name, val);
   SetType(kIsString);
   fNumber = 0.0;
}

void KVNamedParameter::Set(const char*val)
{
   SetTitle(val);
   SetType(kIsString);
   fNumber = 0.0;
}

KVNamedParameter::KVNamedParameter(const char*nom, Double_t val)
   : TNamed(nom, "Double_t"), fNumber(val)
{
   SetType(kIsDouble);
}

void KVNamedParameter::Set(const char*name, Double_t val)
{
   SetNameTitle(name, "Double_t");
   SetType(kIsDouble);
   fNumber = val;
}

void KVNamedParameter::Set(Double_t val)
{
   SetTitle("Double_t");
   SetType(kIsDouble);
   fNumber = val;
}

KVNamedParameter::KVNamedParameter(const char*nom, Int_t val)
   : TNamed(nom, "Int_t"), fNumber(val)
{
   SetType(kIsInt);
}

void KVNamedParameter::Set(const char*name, Int_t val)
{
   SetNameTitle(name, "Int_t");
   SetType(kIsInt);
   fNumber = val;
}

void KVNamedParameter::Set(Int_t val)
{
   SetTitle("Int_t");
   SetType(kIsInt);
   fNumber = val;
}

void KVNamedParameter::Clear(Option_t*)
{
   // Removes the name and any assigned value
   SetNameTitle("", "");
   fNumber = 0.0;
   ResetBits();
}

const Char_t* KVNamedParameter::GetString() const
{
   // returns string if parameter value is of string type
   // if not, print warning and return empty string
   if (IsString()) return GetTitle();
   Warning("GetString", "Parameter %s is of type %s", GetName(), GetTitle());
   return "";
}

Double_t KVNamedParameter::GetDouble() const
{
   // returns double if parameter value is of numerical type
   // if string, print warning and return zero
   if (IsString()) {
      Warning("GetDouble", "Parameter %s is a string : %s", GetName(), GetTitle());
      return 0.0;
   }
   return fNumber;
}

Int_t KVNamedParameter::GetInt() const
{
   // returns integer if parameter value is of numerical type
   // if string, print warning and return zero
   if (IsString()) {
      Warning("GetInt", "Parameter %s is a string : %s", GetName(), GetTitle());
      return 0;
   }
   return (Int_t)fNumber;
}

Bool_t KVNamedParameter::IsEqual(const TObject* obj) const
{
   // Test for equality between two parameters
   // Returns kTRUE if both the name, the type, and the value of the parameters are identical

   if (!obj->InheritsFrom("KVNamedParameter")) return kFALSE;
   KVNamedParameter* _obj = (KVNamedParameter*)obj;
   return ((*this) == (*_obj));
}

Bool_t KVNamedParameter::operator== (const KVNamedParameter& other) const
{
   // Test for equality between two parameters
   // Returns kTRUE if both the name, the type, and the value of the parameters are identical

   if ((other.fName != fName) || other.GetType() != GetType()) return kFALSE;
   switch (GetType()) {
      case kIsString:
         if (fTitle == other.fTitle) return kTRUE;
         break;

      case kIsInt:
         if (other.GetInt() == GetInt()) return kTRUE;
         break;

      case kIsDouble:
         return KVBase::AreEqual(other.GetDouble(), GetDouble());
         break;

      default:
         return kFALSE;
   }
   return kFALSE;
}

void KVNamedParameter::Print(Option_t* opt) const
{
   if (IsString()) {
      Info("Print", "Name = %s type = string value = %s", GetName(), GetTitle());
   } else
      Info("Print", "Name = %s type = %s value = %f", GetName(), GetTitle(), fNumber);
}

void KVNamedParameter::ls(Option_t* opt) const
{
   TROOT::IndentLevel();
   if (IsString()) {
      cout << "<"<<GetName() << "=" << GetTitle() <<">"<< endl;
   } else {
      switch (GetType()) {
         case kIsInt:
            cout << "<"<<GetName() <<"="<< GetInt() <<">"<< endl;
            break;

         case kIsDouble:
            cout << "<"<< GetName() <<"="<< GetDouble() <<">"<< endl;
            break;

         default:
            break;
      }
   }
}
