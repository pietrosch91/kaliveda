/**************************************************************************/
/* Definition de la classe registre binaire                               */
/* Premier test                                                           */
/* D.Cussol                                                               */
/* 13/02/97                                                               */
/**************************************************************************/

#include "KVRegBinary.h"
#include "Riostream.h"

ClassImp(KVRBValAttribut)

////////////////////////////////////////////////////////////////////////////
//Based on val_attribut class of LeSaec/Cussol/Vintache
//
//Each bit in a binary register is a switch with a well-defined function
//or 'attribut'.
//A KVRBValAttribut has two TString labels, one for the 'On' state and one
//for the 'Off' state of a 'bit switch'.

KVRBValAttribut::KVRBValAttribut():fExp_on(),fExp_off()
{
   SetName("KVValAttribut");
}

KVRBValAttribut::KVRBValAttribut(const Char_t * On, const Char_t * Off):fExp_on(On),fExp_off(Off)
{
   SetName("KVValAttribut");
   printf("%s--\n", fExp_on.Data());
   printf("%s--\n", fExp_off.Data());
}

////////////////////////////////////////////////////////////////////////////////////

ClassImp(KVRegBinary)

/////////////////////////////////////////////////////////////////////////////
//Based on r_binaire class
//
//Binary registers are used to control modules, each bit in the register
//being an 'On/Off' switch with a well-defined function, represented by the
//KVRBValAttribut object associated to each bit.

KVRegBinary::KVRegBinary()
{
   SetName("KVRegBinary");
   SetLabel("Binary register");
   fListAttributs = new KVList;
   fListAttributs->SetName("Liste attributs");
}

KVRegBinary::KVRegBinary(const KVRegBinary &reg)
{
   SetName("KVRegBinary");
   SetLabel("Binary register");
   fListAttributs = new KVList;
   fListAttributs->SetName("Liste attributs");
   
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   reg.Copy(*this);
#else
   ((KVRegBinary &) reg).Copy(*this);
#endif
}

KVRegBinary::~KVRegBinary()
{
	delete fListAttributs;
}

void KVRegBinary::AddAttribute(const Char_t *Exp_on, const Char_t *Exp_off)
{
	//Add attribute for a bit in the register i.e. the corresponding functions
	//of positioning the bit 'on' or 'off'
	
   KVRBValAttribut *attribut;
   attribut = new KVRBValAttribut();
   attribut->SetName("attribut");
   long nbits = GetNumberBits();
   if (nbits == 0){
	   Fatal("SetAttribute","Number of bits = 0 for register %s !",GetName());
	   exit(1);
   }
   if (fListAttributs->GetSize() >= nbits){
	   Fatal("SetAttribute","Too many entries given for register %s !",GetName());
	   exit(1);
   }
   attribut->SetExpOn(Exp_on);
   attribut->SetExpOff(Exp_off);
   fListAttributs->Add(attribut);
}

const KVList *KVRegBinary::GetAttributes() const
{
   return (fListAttributs);
}

void KVRegBinary::Print(Option_t * opt) const
{
   KVRegister::Print(opt);
   cout << GetRealValue();
   //if opt="line" we don't go to newline after
   if(strcmp(opt,"line")) cout << endl;
}

const Char_t* KVRegBinary::GetRealValue(const Long_t, const Long_t) const
{
	//Returns string with binary value of register contents
	return (GetBinaryContents().String());
}

//____________________________________________________________________________________//

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVRegBinary::Copy(TObject & obj)  const
#else
void KVRegBinary::Copy(TObject & obj)
#endif
{
   //Copy this to obj

   KVRegister::Copy(obj);
   KVRegBinary &reg = (KVRegBinary&)obj;
   TIter next_att( GetAttributes());
   KVRBValAttribut *rbval;
   while( (rbval = (KVRBValAttribut*)next_att()) ){
	   reg.AddAttribute( rbval->GetExpOn(), rbval->GetExpOff() );
   }
}

