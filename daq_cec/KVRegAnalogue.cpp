/***************************************************************************
                          KVRegAnalogue.cpp  -  description
                             -------------------
    begin                : ven déc 17 2004
    copyright            : (C) 2004 by J.D Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVRegAnalogue.h"
#include "Riostream.h"

ClassImp(KVRegAnalogue)

////////////////////////////////////////////////////////////////////////////
//KVRegAnalogue
//
//An analogue register, i.e. one whose contents represent a real world
//value, such as a discriminator threshold, in mV.

KVRegAnalogue::KVRegAnalogue():fUnits("mV")
{
	//Default ctor
	//Default units are "mv"
	
	     fDebut=0.0;
	     fFin=1024.0;
	     SetName("KVRegAnalogue");
	     SetLabel("Analogue register");
}

//___________________________________________________________________________

KVRegAnalogue::KVRegAnalogue(const KVRegAnalogue &reg)
{
	//Copy ctor
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   reg.Copy(*this);
#else
   ((KVRegister &) reg).Copy(*this);
#endif
}
             
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVRegAnalogue::Copy(TObject & obj)  const
#else
void KVRegAnalogue::Copy(TObject & obj)
#endif
{
   //Copy this to obj

   KVRegister::Copy(obj);
   KVRegAnalogue &reg = (KVRegAnalogue&)obj;
   reg.SetMinValue( GetMinValue() );
   reg.SetMaxValue( GetMaxValue() );
   reg.SetUnits( GetUnits() );
}

//___________________________________________________________________________

void KVRegAnalogue::Print(Option_t* opt) const {

	//Print infos on register.
	//Option='line' gives old 'affiche_ligne' presentation
	
	KVRegister::Print(opt);
	if(strcmp(opt,"line")){
		cout << "Contents:" << GetContents()
				<< " --> " << GetRealValue() << endl
				<< "nbrBits:" << GetNumberBits() << endl;
	} else {
		//option "line"
		cout << GetRealValue() << endl;
	}
}

//___________________________________________________________________________

const Char_t* KVRegAnalogue::GetRealValue(const Long64_t, const Long64_t) const
{
	//Returns string with real world value of register
	
	Long64_t Cont;  Double_t Clair, Cmax=GetMaxCont();

      Cont = GetContents();
	
	Clair=GetMinValue()+((Double_t)Cont *(GetMaxValue()-GetMinValue())/Cmax);
	
      return Form("%f",Clair);
}

//___________________________________________________________________________

void KVRegAnalogue::SetRealValue(const Double_t Clair, const Long64_t, const Long64_t)
{
	//Set the real world value of the register.
	//If this value is less than GetMinValue or greater than GetMaxValue,
	//the contents of the register will be set to either the minimum or
	//maximum allowed value.
	
	Long64_t Cont;   Double_t Cmax=GetMaxCont();
	
      Cont = (Long64_t)( (Clair-GetMinValue()) * Cmax / ( GetMaxValue()-GetMinValue() ) + 0.5 );
      
	if(Clair<GetMinValue()) Cont=0; if(Clair>GetMaxValue()) Cont=(Long64_t)Cmax;
	
      SetContents(Cont);
}

