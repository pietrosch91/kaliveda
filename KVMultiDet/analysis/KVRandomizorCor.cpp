//Created by KVClassFactory on Mon Jun 26 17:31:12 2017
//Author: Eric BONNET

#include "KVRandomizorCor.h"

ClassImp(KVRandomizorCor)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVRandomizorCor</h2>
<h4>KVRandomizor</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVRandomizorCor::KVRandomizorCor(Int_t ndim) : KVRandomizor(ndim)
{
   // Default constructor
}

//____________________________________________________________________________//

KVRandomizorCor::~KVRandomizorCor()
{
   // Destructor
}

//____________________________________________________________________________//

Double_t KVRandomizorCor::ComputeValue(Double_t* pos)
{

   Double_t prod = 1;

   return prod;
}
