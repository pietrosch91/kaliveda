//Created by KVClassFactory on Thu Jan 27 17:23:54 2011
//Author: Eric Bonnet,

#include "KVSpinParity.h"
#include "Riostream.h"
ClassImp(KVSpinParity)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSpinParity</h2>
<h4>Simple class for store life time information of nucleus</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//_________________________________
KVSpinParity::KVSpinParity(const Char_t* name): KVNuclData(name, "s")
{
   // Default constructor
   init();

}

KVSpinParity::KVSpinParity(const KVSpinParity& o) : KVNuclData()
{
   // Copy ctor
   o.Copy(*this);
}

void KVSpinParity::Copy(TObject& object) const
{
   KVNuclData::Copy(object);
   KVSpinParity& lt = (KVSpinParity&)object;
   lt.SetMultiple(IsMultiple());
}

//_________________________________
KVSpinParity::KVSpinParity(): KVNuclData()
{
   // Default constructor
   init();

}

//_________________________________
KVSpinParity::~KVSpinParity()
{
   // Destructor
}

//_________________________________
void KVSpinParity::init()
{

   SetMultiple(kFALSE);

}
