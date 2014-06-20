//Created by KVClassFactory on Fri Jun 20 12:07:39 2014
//Author: Guilain ADEMARD

#include "KVIDQALine.h"

ClassImp(KVIDQALine)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDQALine</h2>
<h4>Base class for identification ridge lines and spots corresponding to different masses and charge states respectively</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDQALine::KVIDQALine()
{
   // Default constructor
}

KVIDQALine::~KVIDQALine()
{
   // Destructor
}

//________________________________________________________________

void KVIDQALine::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVIDQALine::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVIDZALine::Copy(obj);
   //KVIDQALine& CastedObj = (KVIDQALine&)obj;
}

