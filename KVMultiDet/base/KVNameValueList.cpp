//Created by KVClassFactory on Tue Jun 29 14:44:22 2010
//Author: bonnet

#include "KVNameValueList.h"

#include "Riostream.h"
#include "TDirectory.h"
#include "TClass.h"
#include "TMath.h"


ClassImp(KVNameValueList)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVNameValueList</h2>
<h4>Manage a list of parameters (name and associated value), using TNamed stored in a KVHashList</h4>
<!-- */
// --> END_HTML
//Cette classe permet de gérer des listes de parametres avec pour chaque une valeur
//associee
//L'objet contient une liste (KVHashList) rempli de TNamed
//permettant de faire le lien entre un nom (TNamed::GetName) et une valeur (TNamed::GetTitle)
//
//Structure:
//---------
//Initialement cette classe héritait directement de TList,
//mais pour pouvoir l'utiliser en héritage multiple
//avec d'autre classe de KaliVeda (héritant majoritairement de TObject)
//il fallait faire une classe "orpheline".
//Elle est par exemple classe mère de KVSimNucleus et KVSimEvent pour faciliter la gestion
//des parametres dans ces classes.
//Pour éviter toute redondance avec des méthodes basiques de TObject (GetName, Print etc ...)
//On a ajouté un suffiwe au méthodes succeptibles de provoquer une indétermination:
// - Clear_NVL
// - Print_NVL
// - SetName_NVL
// - GetTitle_NVL
// - SetOwner_NVL
// - IsOwner_NVL
// La plupart de ces méthodes appellent directement les méthodes équivalentes de KVHashList
// La méthode Write, permet d'écrire l'objet seul dans un fichier root avec une seul "Key" associée
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
// Important, precision pour les valeurs Double_t
//------------------------------------------------
// - Le traitement des valeurs Double_t passe par une conversion via KVString::Form()
// pour garantir une précision relative inférieure à 1e-15, on effectue cette conversion en écriture scientifique
// avec 15 chiffres apres la virgule
// - la methode statique TestConversion, donne la difference entre la valeur double et celle restituée
// apres conversion 
// 
//	A titre de comparaison dans ROOT, la méthode TMath::Pi() { return 3.14159265358979323846; }
// retourne en fait la valeur : 3.14159265358979312e+00, seule les 15 premieres decimales sont 
// correctes

//
////////////////////////////////////////////////////////////////////////////////

//______________________________________________
KVNameValueList::KVNameValueList()
{
   // Default constructor
	
	
	//printf("KVNameValueList, Constructeur\n");
	khl = new KVHashList();
	khl->SetOwner(kTRUE);
	//Definition par defaut du format de conversion
	sfmt = "%1.15lfe%d";
	
}

//______________________________________________
KVNameValueList::~KVNameValueList()
{
   // Destructor
	Clear_NVL();
	delete khl;
}

//______________________________________________
KVHashList* KVNameValueList::GetList() const
{
	//return the pointeur of the KVHashList where
	//parameters are stored with their values
	return khl;

}

//______________________________________________
void KVNameValueList::Copy(KVNameValueList& nvl) const
{
	// Copy this to the nvl object.
	//Si des parametres sont présents dans nvl, ils seront effacés
	//

	KVHashList* copy = nvl.khl;
	if (!copy)
		printf("pb de KVHashList ....\n");
	khl->Copy(*copy);
	
	//le nom de la liste n est pas copié dans KVSeqCollection::Copy()
	copy->SetName(khl->GetName());

}
	
//______________________________________________
void KVNameValueList::Clear_NVL(Option_t* opt)
{
	//Clear all the stored parameters
	khl->Clear(opt);
}

//______________________________________________
void KVNameValueList::Print_NVL(Option_t* opt) const
{
	//Print stored parameters (name, and value)
	printf("%s, %d stored:\n",GetName_NVL(),GetNpar());
	for (Int_t ii=0;ii<GetNpar();ii+=1)
		printf("%d | %s | %s\n",ii,GetNameAt(ii),GetStringValue(ii));
}

//______________________________________________
void KVNameValueList::Write(const Char_t* name)
{
	//Write the KVNameValueList object in the gDirectory
	//Check if the gDirectory is existing and writable
	if (gDirectory)
		gDirectory->WriteObjectAny(this,"KVNameValueList",name);
}

//______________________________________________
void KVNameValueList::SetName_NVL(const char* name)
{
	//return the name
	//note (name and object are the  KVHashList object ones)
	khl->SetName(name);
}

//______________________________________________
const char* KVNameValueList::GetName_NVL() const
{
	//set the name
	//note (name and object are the  KVHashList object ones)
	return khl->GetName();
}

//______________________________________________
const char* KVNameValueList::GetTitle_NVL() const
{
	//return the title
	//note (name and object are the  KVHashList object ones)
	return khl->GetTitle();
}

//______________________________________________
void KVNameValueList::SetOwner_NVL(Bool_t enable)
{
	//set if the KVNameValueList owns its objects or not
	//by default it is owner
	khl->SetOwner(enable);
}

//______________________________________________
Bool_t KVNameValueList::IsOwner_NVL() const
{
	//return kTRUE if the object owns its objects
	//kFALSE if not
	return khl->IsOwner();
}

//______________________________________________
Int_t KVNameValueList::Compare(const KVNameValueList* nvl) const
{
	// Compare the contents of two KVNameValueObject
	// Returns the number of same parameters (name and value)
	 
	Int_t neq=0;
	Int_t np1 = this->GetNpar();
	Int_t np2 = nvl->GetNpar();
	for (Int_t ii=0;ii<np1;ii+=1)
	  for (Int_t jj=0;jj<np2;jj+=1)
	     if (  !strcmp(this->GetNameAt(ii),(nvl->GetNameAt(jj))) && 
	   		  !strcmp(this->GetStringValue(ii),nvl->GetStringValue(jj))
	   	  )  
	   	  neq+=1;

	return neq;
	 
}

//______________________________________________
void KVNameValueList::Streamer(TBuffer &R__b)
{
	if (R__b.IsReading()) {
		//printf("Streamer, Lecture\n");
		KVNameValueList::Class()->ReadBuffer(R__b, this);
	}
	else {
		//printf("Streamer, Ecriture\n");
		KVNameValueList::Class()->WriteBuffer(R__b, this);
	}
}

//______________________________________________
void KVNameValueList::TestConversion(Double_t value)
{
	//test la difference entre une valeur initiale et apres conversion
	
	KVString tostring="";
	KVString format = "%1.15lfe%d";
	if (value==0) return;

	Double_t abs_val = TMath::Abs(value);
	Int_t expo = Int_t(TMath::Log10(abs_val));

	if (abs_val<1)
		expo-=1;
	tostring.Form(format,value/TMath::Power(10.,expo),expo);
	
	Double_t val2 = tostring.Atof();
	
	cout << "valeur initiale:"<<value<<" valeur convertie:"<<val2;
	cout << " -> difference, abs:"<<val2-value<<"rel:"<<(val2-value)/value <<endl;
	
	
}

//______________________________________________
KVString KVNameValueList::DoubleToString(Double_t value)
{
	
	KVString tostring="";
	if (value==0) 
		return "0.0";
	
	Double_t abs_val = TMath::Abs(value);
	Int_t expo = Int_t(TMath::Log10(abs_val));

	if (abs_val<1)
		expo-=1;
	tostring.Form(sfmt,value/TMath::Power(10.,expo),expo);

	return tostring;
}

//______________________________________________
void KVNameValueList::SetStringValue(TNamed *tn, const Char_t* value)
{
	//protected method
	//change the title (value) of a given object TNamed
	tn->SetTitle(value);
}

//______________________________________________
void KVNameValueList::SetIntValue(TNamed *tn, Int_t value)
{
	//protected method
	//change the title (value) of a given object TNamed
	sconvert.Form("%d",value);
	SetStringValue(tn,sconvert.Data());
}

//______________________________________________
void KVNameValueList::SetDoubleValue(TNamed *tn, Double_t value)
{
	//protected method
	//change the title (value) of a given object TNamed
	
	SetStringValue(tn,DoubleToString(value).Data());
}

//______________________________________________
void KVNameValueList::AddStringValue(const Char_t* name,const Char_t* value)
{
	//protected method 
	//make the connection between a name and a title
	//via the object TNamed which will stored in the list
	khl->Add(new TNamed(name,value));
}

//______________________________________________
void KVNameValueList::AddIntValue(const Char_t* name,Int_t value)
{
	//protected method 
	//make the connection between a name and a title
	//via the object TNamed which will stored in the list
	sconvert.Form("%d",value);
	AddStringValue(name,sconvert.Data());
}

//______________________________________________
void KVNameValueList::AddDoubleValue(const Char_t* name,Double_t value)
{
	//protected method 
	//make the connection between a name and a title
	//via the object TNamed which will stored in the list

	AddStringValue(name,DoubleToString(value).Data());
}


//______________________________________________
void KVNameValueList::SetValue(const Char_t* name,const Char_t* value)
{
	//associate a parameter (define by its name) and a value
	//if the parameter is not in the list, it is added
	//if it's in the list replace its value
	TNamed* tn = 0;
	if ( !(tn  = FindTNamed(name)) ) AddStringValue(name,value);
	else 	SetStringValue(tn,value);	
}

//______________________________________________
void KVNameValueList::SetValue(const Char_t* name,Int_t value)
{
	//associate a parameter (define by its name) and a value
	//if the parameter is not in the list, it is added
	//if it's in the list replace its value
	TNamed* tn = 0;
	if ( !(tn  = FindTNamed(name)) )	AddIntValue(name,value);
	else	SetIntValue(tn,value);	
}

//______________________________________________
void KVNameValueList::SetValue(const Char_t* name,Double_t value)
{
	//associate a parameter (define by its name) and a value
	//if the parameter is not in the list, it is added
	//if it's in the list replace its value
	TNamed* tn = 0;
	if ( !(tn  = FindTNamed(name)) )	AddDoubleValue(name,value);
	else	SetDoubleValue(tn,value);	
}

//______________________________________________
TNamed* KVNameValueList::FindTNamed(const Char_t* name) const{
	//return the TNamed object with the asking name
	return (TNamed* )khl->FindObject(name);
}

//______________________________________________
void KVNameValueList::RemoveParameter(const Char_t* name)
{
	//remove parameter from the list, 
	//Warning the TNamed object associated is deleted
	delete khl->Remove(FindTNamed(name));
}

//______________________________________________
Bool_t KVNameValueList::HasParameter(const Char_t* name)
{
	//Check if there is a parameter with the asked name
	//in the list
	//kTRUE, parameter already present
	//kFALSE, if not
	if (FindTNamed(name)) return kTRUE;
	else return kFALSE;
}

//______________________________________________
Int_t KVNameValueList::GetNameIndex(const Char_t* name)
{
	//return the position in the list of a given parameter
	//using its name
	//return -1 if no parameter with such name are present
	TNamed* named = 0;
	if (!(named = FindTNamed(name))){
		printf("ERROR, GetNameIndex : \"%s\" does not correspond to an existing parameter, default value -1 is returned\n",name);
		return -1;
	}
	return khl->IndexOf(named);
}

//______________________________________________
const Char_t* KVNameValueList::GetNameAt(Int_t idx) const
{
	//return the name of the parameter store at the idx position 
	//in the list
	//if the idx is greater than the number of stored parameters
	//return empty string
	TNamed* named = 0;
	if (!(named = (TNamed* )khl->At(idx))){
		printf("ERROR, GetNameAt : index has to be less than %d, empty string is returned\n",GetNpar());
		return "";
	}
	return named->GetName();
}

//______________________________________________
Int_t KVNameValueList::GetIntValue(const Char_t* name)
{ 
	//return the value in integer format
	//for a parameter using its name
	//return -1 if no parameter with such name are present
	sconvert.Form("%s",GetStringValue(name)); 
	return sconvert.Atoi();
}

//______________________________________________
Double_t KVNameValueList::GetDoubleValue(const Char_t* name)
{ 
	//return the value in double format
	//for a parameter using its name
	//return -1 if no parameter with such name are present
	sconvert.Form("%s",GetStringValue(name)); 
	return sconvert.Atof();
}

//______________________________________________
const Char_t* KVNameValueList::GetStringValue(const Char_t* name) const
{ 
	//return the value in string format
	//for a parameter using its name
	//return string "-1" if no parameter with such name are present
	TNamed* named = 0;
	if (!(named = FindTNamed(name))){
		printf("ERROR, GetStringValue(const Char_t*) : \"%s\" does not correspond to an existing parameter, default value \"-1\" is returned\n",name);
		return "-1";
	}
	return named->GetTitle(); 
}

//______________________________________________
Int_t KVNameValueList::GetNpar() const {
	//return the number of stored parameters
	return khl->GetEntries();
}

//______________________________________________
Int_t KVNameValueList::GetIntValue(Int_t idx)
{ 
	//return the value in integer format
	//for a parameter using its position
	//return -1 idx is greater than the number of stored parameters
	sconvert.Form("%s",GetStringValue(idx)); 
	return sconvert.Atoi();
}

//______________________________________________
Double_t KVNameValueList::GetDoubleValue(Int_t idx)
{ 
	//return the value in double format
	//for a parameter using its position
	//return -1 idx is greater than the number of stored parameters
	sconvert.Form("%s",GetStringValue(idx)); 
	return sconvert.Atof();
}

//______________________________________________
const Char_t* KVNameValueList::GetStringValue(Int_t idx) const
{ 
	//return the value in string format
	//for a parameter using its position
	//return -1 idx is greater than the number of stored parameters
	if (idx>=GetNpar()) {
		printf("ERROR, GetStringValue(Int_t) : index has to be less than %d, \"-1\" is returned\n",GetNpar());
		return "-1";
	}
	return khl->At(idx)->GetTitle(); 
}
