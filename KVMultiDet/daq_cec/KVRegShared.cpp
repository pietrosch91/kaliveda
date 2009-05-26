/***************************************************************************
                          KVRegShared.cpp  -  description
                             -------------------
    begin                : 29/07/2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVRegShared.cpp,v 1.4 2005/09/28 17:17:01 franklan Exp $
**************************************************************************/
/* Definition de la classe registre tableau                               */
/* Premier test                                                           */
/* D.Cussol                                                               */
/* 14/02/97                                                               */
/**************************************************************************/

#include "KVRegShared.h"
#include "Riostream.h"

ClassImp(KVRegShared)

//__________________________________________________________________________
//KVRegShared
//
//Shared register

KVRegShared::KVRegShared()
{
	//Default ctor.
	fRegList = new KVList; GetListRegs()->SetName("List regs share");
	
	SetName("KVRegShared");
	SetLabel("Register share");
}

//__________________________________________________________________________

 KVRegShared::~KVRegShared()
 {
	 //Dtor by default
	 //Delete list of registers (deletes registers also)
	 delete fRegList; fRegList=0;
 }
 
 //__________________________________________________________________________

void KVRegShared::Print(Option_t* opt) const
{
	//Print information on register and all its elements.
	//Put opt="line" for old 'affiche_ligne' presentation
	
	KVRegister::Print(opt);
	
	if(strcmp(opt,"line"))
		printf("Contenu:%Ld --> %s\nnbrBits:%d\n", GetContents(), GetRealValue(), GetNumberBits());
	else
		printf("%s", GetRealValue());
}

 //__________________________________________________________________________

const Char_t* KVRegShared::GetRealValue (const Long64_t Id_Ligne,  const Long64_t Id_Colonne) const 
{
	//returns contents of register in binary format
	
	return (GetBinaryContents().String());
}

 //__________________________________________________________________________

void KVRegShared::SetContents (const Long64_t Value, const Long64_t reg_index,  const Long64_t) 
{
	//Set value of register contents for register reg_index (between 1 and N, where N is number of subregisters)
	// Le bit de poids fort d'un sous registre extrait du registre partage se trouve a gauche.
	// La valeur en entree est convertie en une valeur binaire et ecrite dans une chaine de caracteres.
	// La valeur a ecrire dans le sous registre (id_groupe) est extraite de cette chaine de caracteres.
	// La fonction fonction_partage::Set_Contenu(double) fait exactement le meme travail en considerant
	// que le bit de poids fort est situe a gauche du registre.
	
	//first, convert 'Value' to binary number with the number of bits of the entire register
	Binary_t  val(Value); val.SetNBits( GetNumberBits() );
	
	//now we find the number of the first bit to use by summing the number of bits in all subregisters
	//until we get to the required subregister, 'reg_index'
	TIter next_reg( GetListRegs() ); KVRegister* reg=0;
	Int_t first_bit = 0; Int_t id=1;
	while( (reg = (KVRegister*)next_reg()) && id!=reg_index ) { id++; first_bit+=reg->GetNumberBits(); }
	
	//now we convert first_bit to a bit number (i.e. like BIT(0), BIT(1) etc.)
	first_bit = GetNumberBits() - first_bit - 1;
	
	//set value of register using bits first_bit to (first_bit - reg->GetNumberBits() + 1) of 'Value'
	reg->SetContents( val.Subvalue( first_bit, reg->GetNumberBits() ) );
}

 //__________________________________________________________________________

Long64_t KVRegShared::GetContents (const Long64_t id_groupe,  const Long64_t) const
{
	//if id_groupe=0: binary contents of all registers are chained together and the value of the
	//                         resulting number is returned - first register value (most significant bits)
	//                         is from first register in list of registers
	//if id_groupe>0: registers are taken in reverse order from list of registers, and binary values
	//                         are chained together, EXCEPT that only for register 'id_groupe' in the list
	//                         (first register numbered 1) is its actual value used; for all the others just a
	//                         binary zero (with the requisite number of bits) is written.....???
	
	if( id_groupe ){
		
	//if id_groupe>0: registers are taken in reverse order from list of registers, and binary values
	//                         are chained together, EXCEPT that only for register 'id_groupe' in the list
	//                         (first register numbered 1) is its actual value used; for all the others just a
	//                         binary zero (with the requisite number of bits) is written.....???
	
		TString total(""); TIter nextReg( GetListRegs(), kIterBackward ); KVRegister* reg=0;
		Int_t index = GetListRegs()->GetSize() ;
		Binary_t zero;
		while( (reg = (KVRegister*)nextReg()) ){
			if( index == id_groupe) total += reg->GetRealValue();
			else total += zero.String( reg->GetNumberBits() );
		}
		//convert binary to decimal
		Binary_t tmp( total.Data() ); return tmp.Value();
	}
	
	//if id_groupe=0: binary contents of all registers are chained together and the value of the
	//                         resulting number is returned - first register value (most significant bits)
	//                         is from first register in list of registers
	
	TString total(""); TIter nextReg( GetListRegs() ); KVRegister* reg=0;
	while( (reg = (KVRegister*)nextReg()) ){
		total += reg->GetRealValue();
	}
	//convert binary to decimal
	Binary_t tmp( total.Data() ); return tmp.Value();
	
}
