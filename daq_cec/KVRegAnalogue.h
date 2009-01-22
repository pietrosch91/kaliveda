/***************************************************************************
                          KVRegAnalogue.h  -  description
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

#ifndef KVREGANALOGUE_H
#define KVREGANALOGUE_H

#include "KVRegister.h"
#include "TString.h"

class KVRegAnalogue : public KVRegister  {

private:
// definition des champs specifiques aux analogiques
      Double_t	fDebut;
      Double_t 	fFin;
      TString   fUnits;
      
public: 

	KVRegAnalogue();
	KVRegAnalogue(const KVRegAnalogue&);
	virtual ~KVRegAnalogue(){};

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif
	Double_t GetMinValue(void) const { return fDebut; };
	Double_t GetMaxValue(void) const { return fFin; };
	const Char_t* GetUnits(void) const { return fUnits; };

	void SetMinValue(const Double_t start){ fDebut = start; };
	void SetMaxValue(const Double_t end){ fFin = end; };
	void SetUnits(const Char_t* unit){ fUnits = unit; };

	void Print(Option_t* opt="") const;
	
	const Char_t* GetRealValue(const Long64_t=0, const Long64_t=0) const;
	void SetRealValue(const Double_t, const Long64_t=0, const Long64_t=0);
	
	ClassDef(KVRegAnalogue,1)//Analogue register in module
};

#endif
