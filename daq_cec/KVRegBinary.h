/***************************************************************************
                          KVRegBinary.h  -  description
                             -------------------
    begin                : 10/5/2003
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVRegBinary.h,v 1.6 2005/08/09 08:36:52 franklan Exp $
**************************************************************************
* Definition de la classe registre binaire                               *
* Premier test                                                           *
* D.Cussol                                                               *
* 13/02/97                                                               *
**************************************************************************/

#ifndef KVREGBINARY_H
#define KVREGBINARY_H

#include "KVBase.h"
#include "TString.h"
#include "KVRegister.h"
#include "KVList.h"


class KVRegBinary:public KVRegister {

	private:

	KVList *fListAttributs;//->list of attributes of register switches

	public:

    KVRegBinary();
    KVRegBinary(const KVRegBinary &);
    virtual ~ KVRegBinary();

    void AddAttribute(const Char_t *, const Char_t *);
    const KVList *GetAttributes() const;
    
    virtual void Print(Option_t * opt = "") const;
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   	virtual void Copy(TObject &) const;
#else
   	virtual void Copy(TObject &);
#endif

    const Char_t* GetRealValue(const Long_t=0, const Long_t=0) const;

    ClassDef(KVRegBinary,1)//Binary register in module
};

//////////////////////////////////////////////////////////////////////////////////////

class KVRBValAttribut:public KVBase {

	private:

	TString fExp_on;
	TString fExp_off;

	public:

    KVRBValAttribut();
    KVRBValAttribut(const Char_t *, const Char_t *);
    virtual ~KVRBValAttribut(){}

// Ecriture des champs
   void SetExpOn(const Char_t * on) {
	   fExp_on = on;
   }
   void SetExpOff(const Char_t * off) {
	   fExp_off = off;
   }
// Lecture des champs
   const TString& GetExpOn(void) const {
      return fExp_on;
   }
   const TString& GetExpOff(void) const {
      return fExp_off;
   }

   ClassDef(KVRBValAttribut,1)//'On' & 'Off' states of binary register bit switch
};

#endif
