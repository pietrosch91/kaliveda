/***************************************************************************
                          KVRegArray.cpp  -  description
                             -------------------
    begin                : 29/07/2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVRegArray.cpp,v 1.1 2005/08/09 08:36:08 franklan Exp $
**************************************************************************/
/* Definition de la classe registre tableau                               */
/* Premier test                                                           */
/* D.Cussol                                                               */
/* 14/02/97                                                               */
/**************************************************************************/

#include "KVRegArray.h"
#include "Riostream.h"

ClassImp(KVRegArray)

//__________________________________________________________________________
//KVRegArray
//
//Register with values stored in an array

KVRegArray::KVRegArray()
{
   //Default ctor.
   fRegList = new KVList;
   GetListRegs()->SetName("List regs array");
   SetOrientation('C');
   SetName("KVRegArray");
   SetLabel("Register array");
}

//__________________________________________________________________________

KVRegArray::KVRegArray(Char_t* Name, Long_t Lines, Long_t Columns,
                       ULong_t Address, Long_t Step, Char_t Orientation, Char_t* Voies, KVRegister* Registre)
{
   //Initialise tableau with Lines*Columns registers all of the same type as Registre.
   //Does what 4 different constructors did in class "tableau" thanks to polymorphism:
   //
   //Orientation is always 'C' unless Registre->InheritsFrom("KVRegAnalogue"), in which
   //case it can be 'C' or 'L'.

   fRegList = new KVList;
   GetListRegs()->SetName("List regs array");

   if (Registre->InheritsFrom("KVRegAnalogue"))
      SetOrientation(Orientation);
   else
      SetOrientation('C');

   SetName(Name);
   SetLabel("Register array");
   SetAddress(Address);
   SetChannels(Voies);
   SetStep(Step);
   SetColumns(Columns);
   SetLines(Lines);

   Long_t nb_reg_t = GetElements();

   if (GetNumberVoies() % nb_reg_t != 0) {
      Error("KVRegArray(Char_t *, Long_t, Long_t, ULong_t, Long_t, Char_t, Char_t *, KVRegister*)",
            "Number of channels (%ld) != multiple of Cols*Lines (%ld)",
            GetNumberVoies(), nb_reg_t);
   }

   Long_t nvoie = GetNumberVoies() / nb_reg_t;

   for (Long_t i = 0; i < nb_reg_t; i++) {
      //make clone of Register
      KVRegister* reg = (KVRegister*)Registre->Clone();
      //set address of register in tableau - it is address of tableau + offset
      reg->SetAddress(GetAddress() + (i * GetStep()));

      reg->SetName(Form("%s[%ld]", GetName(), i));
      reg->SetChannels(Form("%ld-%ld", GetFirstVoie() + (i * nvoie), GetFirstVoie() + ((i + 1) * nvoie) - 1));

      GetListRegs()->Add(reg);
   }
}

//__________________________________________________________________________

KVRegArray::~KVRegArray()
{
   //Dtor by default
   delete fRegList;
   fRegList = 0;
}

//__________________________________________________________________________

Long_t KVRegArray::GetIndex(const Long_t line, const Long_t col) const
{
   //Returns index of element from position in tableau.
   //An error message is given if index is < 1 or > GetElements() and -1 is returned

   Long_t indice;
   if (GetOrientation() == 'C')
      indice = ((col - 1) * GetLines()) + line;
   else
      indice = ((line - 1) * GetColumns()) + col;

   //indice should be between 1 and GetElements()
   if ((indice > GetElements()) || (indice < 1)) {
      Error("GetAddressElement(const Long_t, const Long_t)",
            "Request for element (%ld,%ld) gives index %ld; number of elements %ld",
            line, col, indice, GetElements());
      indice = -1;
   }
   return indice;
}

//__________________________________________________________________________

KVRegister* KVRegArray::GetElement(const Long_t Id_Ligne, const Long_t Id_Colonne) const
{
   //Return the register at position (row, column) in the tableau
   // 1 <= Id_Ligne <= GetLines(); 1 <= Id_Colonne <= GetColumns()

   Long_t indice = GetIndex(Id_Ligne, Id_Colonne);
   if (indice < 1) return 0;
   return (KVRegister*)GetListRegs()->At(indice - 1);
}

//__________________________________________________________________________

ULong_t KVRegArray::GetAddressElement(const Long_t Id_Ligne, const Long_t Id_Colonne)
{
   //Return the address of a register member of the tableau, using its position (row and column)
   // 1 <= Id_Ligne <= GetLines(); 1 <= Id_Colonne <= GetColumns()

   KVRegister* r = GetElement(Id_Ligne, Id_Colonne);

   if (r) return (r->GetAddress());

   Error("GetAddressElement(const Long_t, const Long_t)",
         "Element (%ld,%ld) does not exist",
         Id_Ligne, Id_Colonne);
   return 0;
}

//__________________________________________________________________________

void KVRegArray::Print(Option_t* opt) const
{
   //Print information on register and all its elements.
   //Put opt="line" for old 'affiche_ligne' presentation

   KVRegister::Print(opt);

   for (int i = 1; i <= GetLines(); i++) {
      for (int j = 1; j <= GetColumns(); j++) {

         if (!strcmp(opt, "line")) cout << GetRealValue(i, j);
         else
            cout << "Contenu:" << GetContents(i, j) << " --> " << GetRealValue(i, j)
                 << endl
                 << "nbrBits:" << GetNumberBits() << endl;
      }
   }
}

//__________________________________________________________________________

void KVRegArray::SetRealValue(const Double_t val, const Long_t Id_Ligne,  const Long_t Id_Colonne)
{
   //Set value of register contents at (Id_Ligne, Id_Colonne) using a real value

   KVRegister* r = GetElement(Id_Ligne, Id_Colonne);
   if (r) r->SetRealValue(val);
}

//__________________________________________________________________________

const Char_t* KVRegArray::GetRealValue(const Long_t Id_Ligne,  const Long_t Id_Colonne) const
{
   //Get real value of register contents at (Id_Ligne, Id_Colonne)

   KVRegister* r = GetElement(Id_Ligne, Id_Colonne);
   if (r) return r->GetRealValue();
   return 0;
}

//__________________________________________________________________________

void KVRegArray::SetContents(const Long_t Value, const Long_t Id_Ligne,  const Long_t Id_Colonne)
{
   //Set value of register contents at (Id_Ligne, Id_Colonne)

   KVRegister* r = GetElement(Id_Ligne, Id_Colonne);
   if (r) r->SetContents(Value);
}

//__________________________________________________________________________

Long_t KVRegArray::GetContents(const Long_t Id_Ligne,  const Long_t Id_Colonne) const
{
   //Get real value of register contents at (Id_Ligne, Id_Colonne)

   KVRegister* r = GetElement(Id_Ligne, Id_Colonne);
   if (r) return r->GetContents();
   return 0;
}
