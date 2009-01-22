/*
$Id: KVQPsource.cpp,v 1.3 2007/06/27 14:53:12 franklan Exp $
$Revision: 1.3 $
$Date: 2007/06/27 14:53:12 $
*/

//Created by KVClassFactory on Wed Jul  5 17:37:01 2006
//Author: Daniel CUSSOL

#include "KVQPsource.h"

ClassImp(KVQPsource)

////////////////////////////////////////////////////////////////////////////////
// User global variable class for reconstruction of QP source
////////////////////////////////////////////////////////////////////////////////

//_________________________________________________________________
void KVQPsource::init_KVQPsource(void)
	{
//
// Initialisation des champs de KVQPsource
// Cette methode privee n'est appelee par les createurs
//
	}

 
//_________________________________________________________________
KVQPsource::KVQPsource(void):KVVarGlob()
	{
//
// Createur par default
//
	init_KVQPsource();
	SetName("KVQPsource");
	SetTitle("A KVQPsource");
	}
	
//_________________________________________________________________
KVQPsource::KVQPsource(Char_t *nom):KVVarGlob(nom)
	{
//
// Constructeur avec un nom
//
	init_KVQPsource();
	}

//_________________________________________________________________
KVQPsource::KVQPsource(const KVQPsource &a):KVVarGlob()
	{
// 
// Contructeur par Copy
//
	init_KVQPsource();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVQPsource&)a).Copy(*this);
#endif
	}

//_________________________________________________________________
KVQPsource::~KVQPsource(void)
       {
// 
// Destructeur
//
       }

//_________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVQPsource::Copy(TObject&a) const
#else
void KVQPsource::Copy(TObject&a)
#endif
	{
//
// Methode de Copy
//
	KVVarGlob::Copy(a);
	}
	
//_________________________________________________________________
KVQPsource& KVQPsource::operator = (const KVQPsource &a)
	{
//
// Operateur =
//
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVQPsource&)a).Copy(*this);
#endif
	return *this;
	}

//_________________________________________________________________
void KVQPsource::Init(void)
 {
// methode d'initialisation des
// variables internes
 	QPsource.Clear();
 
 }		
				
//_________________________________________________________________
void KVQPsource::Reset(void)		
 {
// Remise a zero avant le
// traitement d'un evenement
 	QPsource.Clear();
 }		
				
//_________________________________________________________________
 
void KVQPsource::Fill(KVNucleus *c)
{
//Add the particle's contribution to global variable.
//
	if(c->GetFrame("CM")->GetVpar() > 0)
	 {
	 QPsource+=(*c);
	 c->SetName("QP");
	 }
}		
				
//_________________________________________________________________
Double_t KVQPsource::GetValue(void) const
 {
// returns the value of the global variable
 	Double_t val=const_cast<KVQPsource*>(this)->GetValue(0);
 
// compute here the value of the global variable and set the value of val

 return val; 
 }		
				
//_________________________________________________________________
Double_t *KVQPsource::GetValuePtr(void)
 {
// On retourne un tableau de valeurs. il est organise comme suit 
//
//
// Index  Meaning
//--------------------------------------
// 0	  E*
// 1	  Zsource
// 2	  Asource
// 3	  Vz
// 4	  Vx
// 5	  Vy
// 6	  Theta
//
//
 Double_t *v=0;
 
 v=new Double_t[7];
 for(Int_t i=0;i<7;i++)
  {
  v[i]=GetValue(i);
  }


 return v;
 }	
                                
//_________________________________________________________________
Double_t KVQPsource::GetValue(Int_t i)
 {
// on retourne la ieme valeur du tableau
//
// Index  Meaning
//--------------------------------------
// 0	  E*
// 1	  Zsource
// 2	  Asource
// 3	  Vz
// 4	  Vx
// 5	  Vy
// 6	  Theta
//
//
 Double_t rval=0;
 switch (i)
  {
  case 0: rval=QPsource.GetExcitEnergy();
  	  break;

  case 1: rval=QPsource.GetZ();
  	  break;

  case 2: rval=QPsource.GetA();
  	  break;

  case 3: rval=QPsource.GetV().Z();
  	  break;

  case 4: rval=QPsource.GetV().X();
  	  break;

  case 5: rval=QPsource.GetV().Y();
  	  break;

  case 6: rval=QPsource.GetTheta();
  	  break;

  default : rval=0.;
            break;
  }
 return rval;
 }         
				
//_________________________________________________________________
Double_t KVQPsource::GetValue(Char_t *name)    
 {
// on retourne la valeur de la variable "name"
//
//	Name		 Meaning
//-----------------------------------------------------
//     E*		Excitation Energy
//     Z		Z source
//     A		A source
//     Vz		z compoenent of velocity
//     Vx		x compoenent of velocity
//     Vy		y compoenent of velocity
//     Theta		angle relative to the beam
//
// par exemple, KVQPsource::GetValue("Excit") retourne la valeur de l'energie
// d'excitation. Des noms peuvent etre "synonymes": ils peuvent retourner la meme
// valeur. Par exemple "Var1" et "Variable1" peuvent retourner la valeur de la Variable 1. 
//
 Int_t i=0;
 if(strcmp(name,"E*")==0)
  {
  i=0;
  }
 else if(strcmp(name,"Z")==0)
  {
  i=1;
  }
 else if(strcmp(name,"A")==0)
  {
  i=2;
  }
 else if(strcmp(name,"Vz")==0)
  {
  i=3;
  }
 else if(strcmp(name,"Vx")==0)
  {
  i=4;
  }
 else if(strcmp(name,"Vy")==0)
  {
  i=5;
  }
 else if(strcmp(name,"Theta")==0)
  {
  i=6;
  }
 else
  {
  i=0;
  Warning("GetValue(Char_t *name)",Form("The name \"%s\" is unknown, default value returned.",name)); 
  }

 return GetValue(i);
 }         

//_________________________________________________________________
TObject *KVQPsource::GetObject(void)    
 {
//
// Returns a pointer to an object linked to this globa variable
// This may be a list of particles, an intermediate object used to compute values, etc.
// 
 TObject *obj=(TObject *)&QPsource;

// Sets the value of the pointer here
 

 return obj;
 }
