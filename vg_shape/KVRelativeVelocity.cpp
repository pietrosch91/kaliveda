/*
$Id: KVRelativeVelocity.cpp,v 1.3 2007/11/29 13:16:43 franklan Exp $
$Revision: 1.3 $
$Date: 2007/11/29 13:16:43 $
*/

//Created by KVClassFactory on Wed Nov 28 11:05:04 2007
//Author: Eric Bonnet

#include "KVRelativeVelocity.h"

ClassImp(KVRelativeVelocity)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVRelativeVelocity</h2>
<h4>calculate for a given a couple of particles the magnitude of relative velocity</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//_________________________________________________________________
void KVRelativeVelocity::init_KVRelativeVelocity(void)
	{

	fType = KVVarGlob::kTwoBody;
//
// Initialisation of the fields of the KVRelativeVelocity class
// This private method is called by the contrctors only. 
//
	}

//_________________________________________________________________
KVRelativeVelocity::KVRelativeVelocity(void):KVVarGlobMean()
	{
//
// Default constructor (may not be modified)
//
	init_KVRelativeVelocity();
	SetName("KVRelativeVelocity");
	SetTitle("A KVRelativeVelocity");
	}
	
//_________________________________________________________________
KVRelativeVelocity::KVRelativeVelocity(Char_t *nom):KVVarGlobMean(nom)
	{
//
// Constructeur avec un nom (may not be modified)
//
	init_KVRelativeVelocity();
	}

//_________________________________________________________________
KVRelativeVelocity::KVRelativeVelocity(const KVRelativeVelocity &a):KVVarGlobMean()
	{
// 
// Copy constructor (may not be modified)
//
	init_KVRelativeVelocity();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVRelativeVelocity&)a).Copy(*this);
#endif
	}

//_________________________________________________________________
KVRelativeVelocity::~KVRelativeVelocity(void)
       {
// 
// Destructeur
//
       }

//_________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVRelativeVelocity::Copy(TObject&a) const
#else
void KVRelativeVelocity::Copy(TObject&a)
#endif
	{
// Copy method
	//In order to be coherent with ROOT Copy functions, "a" becomes a copy of "this",
	//not the other way around as before.

	KVVarGlobMean::Copy(a);
	// To copy a specific field, do as follows:
	//
	//((KVRelativeVelocity &)a).field=field;
	//
	// If setters and getters are available, proceed as follows
	//
	//((KVRelativeVelocity &)a).SetField(GetField());
	//
	
	}
	
//_________________________________________________________________
KVRelativeVelocity& KVRelativeVelocity::operator = (const KVRelativeVelocity &a)
	{
//
// Operator = (may not be modified)
//
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVRelativeVelocity&)a).Copy(*this);
#endif
	return *this;
	}


//_________________________________________________________________
void KVRelativeVelocity::Fill2(KVNucleus *ci,KVNucleus *cj)
{

 //
 // Filling method
 //
 //
 // Use the FillVar(val) or the FillVar(val,w) method to increment the quantity 
 // of the global variable.
 // The weight w is evaluated from the properties of the KVNucleus passed as an
 // argument. 
 //
 // For example, to evaluate the mean charge of all fragments,
 // you may proceed as follows:
 //
 // FillVar(c->GetZ());
 //
 // Another example: to evaluate the mean parallel velocity weighted by the charge
 // of the nucleus:
 //
 // FillVar(c->GetV().Z(),c->GetZ());
 //
   
   if(ci==cj) return; // can be called for same nucleus
   
	TVector3 ww = ci->BoostVector() - cj->BoostVector(); 
	FillVar(ww.Mag());			

 }
