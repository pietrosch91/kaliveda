/***************************************************************************
                          KVRegArray.h  -  description
                             -------------------
    begin                : 29/07/2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVRegArray.h,v 1.1 2005/08/09 08:36:08 franklan Exp $
**************************************************************************/
/* Definition de la classe registre tableau                               */
/* Premier test                                                           */
/* D.Cussol                                                               */
/* 14/02/97                                                               */
/**************************************************************************/

#ifndef KVTAB__H
#define KVTAB__H

#include "KVRegister.h"
#include "KVList.h"

class KVRegArray : public KVRegister
{
private:
	Long_t fCols, fLignes, fStep;
	Char_t fOrient;
	KVList *fRegList;
	
public:
 	KVRegArray();
 	KVRegArray(Char_t *, Long_t, Long_t, ULong_t, Long_t, Char_t, Char_t *, KVRegister*);
	
	virtual ~KVRegArray();
	
 	Long_t GetLines() const {return fLignes;};
	Long_t GetColumns() const {return fCols;};
 	Char_t GetOrientation() const {return fOrient;};
 	Long_t GetStep() const {return fStep;};
 	KVList* GetListRegs() const {return fRegList;};
	ULong_t GetAddressElement(const Long_t line, const Long_t col);
  	const Char_t * GetRealValue(const Long_t=0, const Long_t=0) const;
  	Long_t GetContents(const Long_t=0, const Long_t=0) const;
	Long_t GetElements() const { return (GetLines()*GetColumns()); };
	Long_t GetIndex(const Long_t line, const Long_t col) const;
	KVRegister* GetElement(const Long_t Id_Ligne, const Long_t Id_Colonne) const;
	
 	void Print(Option_t *opt="") const;
	
  	void SetRealValue(const Double_t, const Long_t=0, const Long_t=0);
  	void SetContents(const Long_t, const Long_t=0, const Long_t=0);
	
 	void SetLines(Long_t l) {fLignes=l;};
	void SetColumns(Long_t c) {fCols=c;};
 	void SetOrientation(Char_t o) {fOrient=o;};
 	void SetStep(Long_t s) {fStep=s;};
	
	ClassDef(KVRegArray, 1)//A register in the form of an array
	
};

#endif
