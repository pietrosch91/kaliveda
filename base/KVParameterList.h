#ifndef __KVPARAMETERLIST_H__
#define __KVPARAMETERLIST_H__

#include "KVList.h"
#include "KVParameter.h"
#include "Riostream.h"
#include <cstdlib>
 
template < class T > class KVParameterList {
 private:
   KVList * fParameters;//->persistent list of parameters
 
   // Add a parameter to the parameters list
   void AddParameter(TObject *);

 public:
   KVParameterList() {
      fParameters = new KVList;
   };
   virtual ~ KVParameterList() {
         delete fParameters;
         fParameters = 0;
   };

   // Replaces any existing parameters in list 'obj' with all those defined in this list.
   void Copy(KVParameterList<T>&) const;
   
   // Define and/or set value of a named parameter in the list
   void SetParameter(const Char_t * name, T & val);
   
   // Define and/or set value of a named parameter in the list
   void SetParameter(const Char_t * name, const T & val) {SetParameter(name, (T&)val);};
   
   // Returns value associated to named parameter
   // Use HasParameter() before to check parameter exists: if not, this will crash
   virtual const T & GetParameter(const Char_t * name) const;
   
   // Returns kTRUE if named parameter has been defined.
   virtual Bool_t HasParameter(const Char_t * name) const;
   
   // Remove the named parameter from the parameter list
   virtual void RemoveParameter(const Char_t * name);
   
   // Clear out list of parameters (all KVParameter objects are deleted)
   virtual void Clear();
   
   // Returns total number of parameters in list
   virtual Int_t GetNPar() const {
      return fParameters->GetSize();
   };
   
   // Returns the parameter at position i in the list (i=0, 1, ...)
   virtual KVParameter < T > *GetParameter(Int_t i) const {
      return (KVParameter < T > *) fParameters->At(i);
   };
   
   // Print list of parameters with their values
   virtual void Print(Option_t*/*opt*/= "") const;

   ClassDef(KVParameterList, 2) //class handling a list of named parameters
};

//________________________________________________________________________________

template < class T > void KVParameterList <
    T >::SetParameter(const Char_t * name, T & val)
{
   //Define and/or set value of a named parameter in the list

   KVParameter < T > *par = 0;
   //parameter already exists ?
   par = (KVParameter < T > *)fParameters->FindObject(name);
   if (!par) {
      par = new KVParameter < T > (name, val);
      AddParameter(par);
   } else
      par->SetVal(val);
}

//________________________________________________________________________________

template < class T > void KVParameterList <
    T >::AddParameter(TObject * param)
{
   //Add a parameter to the parameters list
   fParameters->Add(param);
}

//________________________________________________________________________________

template < class T > void KVParameterList <
    T >::RemoveParameter(const Char_t * name)
{
   //Remove the named parameter from the parameter list
   KVParameter < T > *par = 0;
   //does parameter exist ?
   par = (KVParameter < T > *)fParameters->FindObject(name);
   if (par) {
      fParameters->Remove(par);
      delete par;
   }
}

//________________________________________________________________________________

template < class T > const T & KVParameterList <
    T >::GetParameter(const Char_t * name) const
{
   //Returns value associated to named parameter
   //Use HasParameter() before to check parameter exists: if not, this will crash

   KVParameter < T > *par =
       (KVParameter < T > *)fParameters->FindObject(name);
   if (!par) {
      cout << "FatalError in <KVParameterList::GetParameter> : ";
      cout << "No parameter " << name <<
          " defined. You should use KVParameterList::HasParameter(const Char_t*)";
      cout <<
          " to check the existence of a parameter before calling this method. Program will exit."
          << endl;
      exit(EXIT_FAILURE);
   }
   return par->GetVal();
}

//________________________________________________________________________________

template < class T > Bool_t KVParameterList <
    T >::HasParameter(const Char_t * name) const
{
   //Returns kTRUE if named parameter has been defined.

   KVParameter < T > *par =
       (KVParameter < T > *)fParameters->FindObject(name);
   return (Bool_t) (par != 0);
}

//________________________________________________________________________________

template < class T > void KVParameterList < T >::Clear()
{
   //Clear out list of parameters (all KVParameter objects are deleted)
   fParameters->Clear();
}

//________________________________________________________________________________

template < class T > void KVParameterList < T >::Copy(KVParameterList < T > & obj) const
{
   //Replaces any existing parameters in list 'obj' with all those defined
   //in this list.
   obj.Clear();
   if (GetNPar()) {
      for (int j = 0; j < GetNPar(); j++) {
         obj.SetParameter( GetParameter(j)->GetName(), (T&)GetParameter(j)->GetVal() );
      }
   }
}

//________________________________________________________________________________

template < class T > void KVParameterList < T >::Print(Option_t * /*opt*/) const
{
   //Print list of parameters with their values
   if (GetNPar()) {
      for (int j = 0; j < GetNPar(); j++) {
         cout << GetParameter(j)->GetName() << " = " << GetParameter(j)->
             GetVal() << endl;
      }
   } else {
      cout << " (parameter list is empty)" << endl;
   }
}

#endif
