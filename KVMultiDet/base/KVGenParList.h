/*
$Id: KVGenParList.h,v 1.2 2009/05/07 12:21:33 ebonnet Exp $
$Revision: 1.2 $
$Date: 2009/05/07 12:21:33 $
*/

//Created by KVClassFactory on Tue Oct 23 15:21:35 2007
//Author: ljohn

#ifndef __KVGENPARLIST_H
#define __KVGENPARLIST_H

#include "KVParameterList.h"
#include "KVString.h"

class KVGenParList : public KVParameterList<KVString> {
public:

   KVGenParList();
   KVGenParList(const KVGenParList&);
   void Copy(KVGenParList&) const;
   virtual ~KVGenParList();

   virtual const Char_t* GetStringValue(const Char_t* name);
   virtual Int_t GetIntValue(const Char_t* name);
   virtual Double_t GetDoubleValue(const Char_t* name);

   void SetValue(const Char_t* name, const Char_t* val);
   void SetValue(const Char_t* name, const KVString& val);
   void SetValue(const Char_t* name, KVString& val);
   void SetValue(const Char_t* name, Int_t val);
   void SetValue(const Char_t* name, Double_t val);

   void IncrementValue(const Char_t* name, Int_t val);
   void IncrementValue(const Char_t* name, Double_t val);

   ClassDef(KVGenParList, 1) //A list of named parameters which can be either strings, doubles or integers
};

#endif
