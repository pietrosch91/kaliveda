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
//
// It takes all nuclei with centre of mass velocities > 0 and adds them
// together (charge, mass, momentum, energy) in order to calculate
// a 'source' nucleus, for which we can obtain various quantities (see below).
// Each nucleus which is included in the source definition is added
// to a group called "QPsource". This can be used for further analysis
// of the particles (by testing KVParticle::BelongsToGroup("QPsource") method).
//
//	Index     Name		 	Meaning
//-----------------------------------------------------
//   0		  E*			Excitation Energy
//   1		  Z			Z source
//   2		  A			A source
//   3		  Vz			z component of velocity
//   4		  Vx			x component of velocity
//   5 	 	  Vy			y component of velocity
//   6		  Theta		polar angle relative to the beam
//
// These quantities can be obtained either by name:
//
//      Double_t estar = GetValue("E*");
//
// or by index:
//
//      Double_t estar = GetValue(0);
//
// or in an array:
//
//     Double_t *vals = GetValuePtr();
//     Double_t estar = vals[0]
//
////////////////////////////////////////////////////////////////////////////////

//_________________________________________________________________
void KVQPsource::init_KVQPsource(void)
	{
		// Initialisation called by ctor.
		// Set up correspondance between variable names and index.
		
 SetNameIndex("E*",0);
 SetNameIndex("Z",1);
 SetNameIndex("A",2);
 SetNameIndex("Vz",3);
 SetNameIndex("Vx",4);
 SetNameIndex("Vy",5);
 SetNameIndex("Theta",6);
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
//Chosen particles are labelled as belonging to "QPsource" group
	if(c->GetFrame("CM")->GetVpar() > 0)
	 {
	 QPsource+=(*c);
	 c->AddGroup("QPsource");
	 }
}		
				
//_________________________________________________________________
Double_t KVQPsource::getvalue_void() const
 {
	// returns the excitation energy of the source
	return QPsource.GetExcitEnergy(); 
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
 for(Int_t i=0;i<7;i++)
  {
  fVal[i]=GetValue(i);
  }


 return fVal;
 }	
                                
//_________________________________________________________________
Double_t KVQPsource::getvalue_int(Int_t i)
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
TObject *KVQPsource::GetObject(void)    
 {
//
// Returns address of the KVNucleus used to calculate source properties
// 
 TObject *obj=(TObject *)&QPsource;
 

 return obj;
 }
