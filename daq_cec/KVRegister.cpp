/***************************************************************************
                          kvregister.h  -  description
                             -------------------
    begin                : 10/05/2003
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVRegister.cpp,v 1.13 2005/10/10 12:59:44 franklan Exp $

*************************************************************************
* Definition de la classe registre                                       *
* P.Lesaec & D.Vintache                                                  *
* 11/02/97                                                               *
**************************************************************************/

#include "KVRegister.h"
#include "TMath.h"
#include "Riostream.h"

ClassImp(KVRegister)
////////////////////////////////////////////////////////////////////////////////
//KVRegister
//Based on Cussol/Vintache/LeSaec classes for INDRA commande & controle.
//
//Base class for description of registers in electronic modules.
//Each register is characterised by the following:
//
//	its address
//	its contents (the value stored in the register)
//	the number of bits in the register
//	the access mode of the register ("R" read only, "W" write only)
//	the numbers of the first and last channels (Voies) concerned by this
//		register, and the step in channel number
//
//In addition, the value in the register may correspond to some "real
//world" value such as a discriminator threshold value in mV. In order to
//access or set this value (which is probably not the same as the actual
//value represented by the bits in the register) methods Get/SetRealValue
//are provided.
//
//The maximum possible value storable in the register (based on the number
//of bits) is given by GetMaxCont().
//ConvertDecimalBinary() and ConvertBinaryDecimal() allow to pass from one
//representation of the register's contents to the other.
////////////////////////////////////////////////////////////////////////////////

KVRegister::KVRegister()
{
   //Default constructor

#ifdef KV_DEBUG
   cout << "Init registre\n";
#endif
   fAdresse = 0;
   fContenu = "0";
   fFonction_associee = "";
   fAcces = 'R';
   fVoie_deb = 0;
   fVoie_fin = 0;
   fVoie_pas = 0;
   SetName("KVRegister");
}

KVRegister::KVRegister(const KVRegister &reg):fFonction_associee()
{
   //Copy constructor
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   reg.Copy(*this);
#else
   ((KVRegister &) reg).Copy(*this);
#endif
}

KVRegister::~KVRegister()
{
   //Destructor
}

void KVRegister::Print(const Option_t * t)  const {
   //use Option_t* t="line" for old registre::affiche_ligne behaviour
   //otherwise this is registre::affiche

   Long64_t Adr;
   Long64_t Cont;
   Int_t NbrBits;
   Char_t Acces;
   Char_t Nom[20];
   Int_t Voie_deb;
   Int_t Voie_fin;
   Int_t Voie_pas;
   Char_t s[20];

   Adr = GetAddress();
   Cont = GetContents();
   NbrBits = GetNumberBits();
   Acces = GetAccess();
   sprintf(Nom, "%19s", GetName());
   sprintf(s, "%20s", GetAssocFunc());
   Voie_deb = GetFirstVoie();
   Voie_fin = GetLastVoie();
   Voie_pas = GetVoieStep();

   if (!strcmp(t, "line"))
   {
      //corresponds to registre::affiche_ligne
      printf("!%19s!%Ld!%Ld!%2d!%2d!%2d!%2d!%c!%20s!",
		  Nom, Adr, Cont, NbrBits, Voie_deb, Voie_fin, Voie_pas, Acces, s);
   } else {
      //corresponds to registre::affiche
      printf
          ("Registre:\nNom   :%19s\nAdresse:%Ld\nContenu:%Ld\nnbrBits:%d\nVoie_deb:%d\nVoie_fin:%d\nVoie_pas:%d\nAcces:%c\nFonction associee:%20s\n",
           Nom, Adr, Cont, NbrBits, Voie_deb, Voie_fin, Voie_pas, Acces,
           s);
   }
}

//____________________________________________________________________________________//

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVRegister::Copy(TObject & obj)  const
#else
void KVRegister::Copy(TObject & obj)
#endif
{
   //Copy this to obj

   KVBase::Copy(obj);
   KVRegister &reg = (KVRegister&)obj;
   reg.SetAccess( GetAccess() );
   reg.SetAddress( GetAddress() );
   reg.SetAssocFunc( GetAssocFunc() );
   reg.SetChannels( Form("%d-%d:%d", GetFirstVoie(), GetLastVoie(), GetVoieStep()) );
   reg.SetContents( GetContents() );
   reg.SetNumberBits( GetNumberBits() );
}

//___________________________________________________________________________

const Char_t* KVRegister::GetRealValue(const Long64_t, const Long64_t) const
{
	//Returns 0 (null string pointer).
	//Redefine in child classes for specific types of register.
	
      return 0;
}

//___________________________________________________________________________

void KVRegister::SetRealValue(const Double_t Clair, const Long64_t, const Long64_t)
{
	//Does nothing. Redefine in child classes for specific types of register.
}

//___________________________________________________________________________

