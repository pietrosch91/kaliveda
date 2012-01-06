//Created by KVClassFactory on Tue Jun 29 14:44:22 2010
//Author: bonnet

#include "KVNameValueList.h"
#include "KVNamedParameter.h"
#include "Riostream.h"

ClassImp(KVNameValueList)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVNameValueList</h2>
<h4>A general-purpose list of parameters (name and associated value)</h4>
<!-- */
// --> END_HTML
//Cette classe permet de gérer des listes de parametres avec pour chaque une valeur
//associee
//L'objet contient une liste (KVHashList) rempli de KVNamedParameter
//permettant de faire le lien entre un nom et une valeur
//Les objets de la liste lui appartiennent par defaut
//
//Fonctionnement:
//--------------
//Chaque parametre defini par son nom ne peut etre present qu une fois
//les valeurs peuvent etre passer et recuperer sous trois format Double_t, Int_t et Char_t*
//
//Les parametres sont ajoutes automatiquement dans la liste et leur valeur changee
//par les memes methodes SetValue
//
//Il est possible de connaitre :
// - l'index d'un parametre dans la liste -> GetNameIndex()
// - sa presence ou non -> HasParameter()
// - le nom d'un parametre pour un index donné -> GetNameAt()
//Il est possible de retirer un parametre : RemoveParameter()
//
////////////////////////////////////////////////////////////////////////////////

//______________________________________________
KVNameValueList::KVNameValueList()
   : fList()
{
   // Default constructor
	fList.SetOwner(kTRUE);
}

//______________________________________________
KVNameValueList::KVNameValueList(const Char_t* name, const Char_t* title)
   : TNamed(name,title), fList()
{
   // Ctor with name & title
	fList.SetOwner(kTRUE);
}

//______________________________________________
KVNameValueList::KVNameValueList(const KVNameValueList& NVL)
{
   // Copy constructor
	NVL.Copy(*this);
	fList.SetOwner(kTRUE);
}

//______________________________________________
KVNameValueList::~KVNameValueList()
{
   // Destructor
	fList.Clear();// will delete objects in list if owner
}

//______________________________________________
KVHashList* KVNameValueList::GetList() const
{
	//return the pointeur of the KVHashList where
	//parameters are stored with their values
	return (KVHashList*)&fList;
}

//______________________________________________
void KVNameValueList::Copy(TObject& nvl) const
{
	// Copy this to the nvl object.
	//Si des parametres sont présents dans nvl, ils seront effacés

   TNamed::Copy(nvl);
   KVNameValueList& _obj = (KVNameValueList&)nvl;
   fList.Copy(_obj.fList);
}
	
//______________________________________________
void KVNameValueList::Clear(Option_t* opt)
{
	//Clear all the stored parameters
   //Deletes the parameter objects if owner & opt!="nodelete"
	fList.Clear(opt);
}

//______________________________________________
void KVNameValueList::Print(Option_t* opt) const
{
	//Print stored parameters (name, and value)
	cout << GetName()<<" list : " <<GetTitle() <<" ("<< this << ")"<<endl;
	for (Int_t ii=0;ii<GetNpar();ii+=1){
      GetParameter(ii)->ls();
   }
}

//______________________________________________
void KVNameValueList::SetOwner(Bool_t enable)
{
	//set if the KVNameValueList owns its objects or not
	//by default it is owner
	fList.SetOwner(enable);
}

//______________________________________________
Bool_t KVNameValueList::IsOwner() const
{
	//return kTRUE if the list owns its objects
	//kFALSE if not
	return fList.IsOwner();
}

//______________________________________________
Int_t KVNameValueList::Compare(const TObject* obj) const
{
	// Compare the contents of two KVNameValueList
	// Returns the number of same parameters (name and value)
	 
   KVNameValueList* nvl = (KVNameValueList*)obj;
	Int_t neq=0;
	Int_t np1 = GetNpar();
	Int_t np2 = nvl->GetNpar();
	for (Int_t ii=0;ii<np1;ii+=1)
	  for (Int_t jj=0;jj<np2;jj+=1)
	     if ( *(GetParameter(ii)) == *(GetParameter(jj)) )  neq+=1;

	return neq;
	 
}

//______________________________________________
void KVNameValueList::SetValue(const Char_t* name,const Char_t* value)
{
	//associate a parameter (defined by its name) and a value
	//if the parameter is not in the list, it is added
	//if it's in the list replace its value
	
   KVNamedParameter* par = FindParameter(name);
	par ? par->Set(value) : fList.Add(new KVNamedParameter(name,value));	
}

//______________________________________________
void KVNameValueList::SetValue(const Char_t* name,Int_t value)
{
	//associate a parameter (define by its name) and a value
	//if the parameter is not in the list, it is added
	//if it's in the list replace its value
   KVNamedParameter* par = FindParameter(name);
	par ? par->Set(value) : fList.Add(new KVNamedParameter(name,value));	
}

//______________________________________________
void KVNameValueList::SetValue(const Char_t* name,Double_t value)
{
	//associate a parameter (define by its name) and a value
	//if the parameter is not in the list, it is added
	//if it's in the list replace its value
   KVNamedParameter* par = FindParameter(name);
	par ? par->Set(value) : fList.Add(new KVNamedParameter(name,value));	
}

//______________________________________________
KVNamedParameter* KVNameValueList::FindParameter(const Char_t* name) const{
	//return the parameter object with the asking name
	return (KVNamedParameter *)fList.FindObject(name);
}

KVNamedParameter* KVNameValueList::GetParameter(Int_t idx) const
{
   //return the parameter object with index idx
	return (KVNamedParameter *)fList.At(idx);
}

//______________________________________________
void KVNameValueList::RemoveParameter(const Char_t* name)
{
	//remove parameter from the list, 
	//Warning the TNamed object associated is deleted
   
   KVNamedParameter* par = FindParameter(name);
   if(par){
      fList.Remove(par);
      delete par;
   }
}

//______________________________________________
Bool_t KVNameValueList::HasParameter(const Char_t* name)
{
	//Check if there is a parameter with the asked name
	//in the list
	//kTRUE, parameter already present
	//kFALSE, if not
	return (FindParameter(name)!=NULL);
}

//______________________________________________
Int_t KVNameValueList::GetNameIndex(const Char_t* name)
{
	//return the position in the list of a given parameter
	//using its name
	//return -1 if no parameter with such name are present
	
   TObject* par = 0;
   Int_t idx = 0;
   TIter next(&fList);
   while( (par = next()) ){
      if(!strcmp(par->GetName(),name)) return idx;
      idx++;
   }
   Error("GetNameIndex", "Parameter \"%s\" not found, -1 returned", name);
   return -1;
}

//______________________________________________
const Char_t* KVNameValueList::GetNameAt(Int_t idx) const
{
	//return the name of the parameter store at the idx position 
	//in the list
	//if the idx is greater than the number of stored parameters
	//return empty string

   if (idx >= GetNpar()){
		Error("GetNameAt", "index has to be less than %d, empty string is returned",GetNpar());
		return "";
	}
	return fList.At(idx)->GetName();
}

//______________________________________________
Int_t KVNameValueList::GetIntValue(const Char_t* name) const
{ 
	//return the value in integer format
	//for a parameter using its name
	//return -1 if no parameter with such name are present
	
   KVNamedParameter* par = FindParameter(name);
   return (par ? par->GetInt() : -1);
}

//______________________________________________
Double_t KVNameValueList::GetDoubleValue(const Char_t* name) const
{ 
	//return the value in double format
	//for a parameter using its name
	//return -1 if no parameter with such name are present

   KVNamedParameter* par = FindParameter(name);
   return (par ? par->GetDouble() : -1.0);
}

//______________________________________________
const Char_t* KVNameValueList::GetStringValue(const Char_t* name) const
{ 
	//return the value in string format
	//for a parameter using its name
	//return string "-1" if no parameter with such name are present
   
   KVNamedParameter* par = FindParameter(name);
	if (!par){
		Error("GetStringValue(const Char_t*)", "\"%s\" does not correspond to an existing parameter, default value \"-1\" is returned",name);
		return Form("%d", -1);
	}
	return par->GetString();
}

//______________________________________________
Int_t KVNameValueList::GetNpar() const {
	//return the number of stored parameters
	return fList.GetEntries();
}

//______________________________________________
Int_t KVNameValueList::GetIntValue(Int_t idx) const
{ 
	//return the value in integer format
	//for a parameter using its position
	//return -1 idx is greater than the number of stored parameters
   KVNamedParameter* par = GetParameter(idx);
   return (par ? par->GetInt() : -1);
}

//______________________________________________
Double_t KVNameValueList::GetDoubleValue(Int_t idx) const
{ 
	//return the value in double format
	//for a parameter using its position
	//return -1 idx is greater than the number of stored parameters
   KVNamedParameter* par = GetParameter(idx);
   return (par ? par->GetDouble() : -1.0);
}

//______________________________________________
const Char_t* KVNameValueList::GetStringValue(Int_t idx) const
{ 
	//return the value in string format
	//for a parameter using its position
	//return -1 idx is greater than the number of stored parameters
	if (idx>=GetNpar()) {
		Error("GetStringValue(Int_t)", "index has to be less than %d, \"-1\" is returned\n",GetNpar());
		return Form("%d", -1);
	}
   return GetParameter(idx)->GetString();
}
	
Bool_t KVNameValueList::IsValue(const Char_t* name,const Char_t* value)
{
   // Returns kTRUE if parameter with given name exists and is equal to given value
   KVNamedParameter* par = FindParameter(name);
   if(par){
      KVNamedParameter tmp(name, value);
      return (*par)==tmp;
   }
   return kFALSE;
}
	
Bool_t KVNameValueList::IsValue(const Char_t* name,Int_t value)
{
   // Returns kTRUE if parameter with given name exists and is equal to given value
   KVNamedParameter* par = FindParameter(name);
   if(par){
      KVNamedParameter tmp(name, value);
      return (*par)==tmp;
   }
   return kFALSE;
}
	
Bool_t KVNameValueList::IsValue(const Char_t* name,Double_t value)
{
   // Returns kTRUE if parameter with given name exists and is equal to given value
   KVNamedParameter* par = FindParameter(name);
   if(par){
      KVNamedParameter tmp(name, value);
      return (*par)==tmp;
   }
   return kFALSE;
}
