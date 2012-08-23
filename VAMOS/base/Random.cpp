#include "Random.h"
#include "Riostream.h"
#include <cstdlib>
//Author: Maurycy Rejmund

using namespace std;

ClassImp(Random)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Random</h2>
<p>
Part of the VAMOS analysis package kindly contributed by Maurycy Rejmund (GANIL).
</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Random::Random(void)
{
#ifdef DEBUG
  cout << "Random::Constructor" << endl;
#endif
  
  Array = Ptr = new Float_t[255];
  if(!Array)
    {
      cout << "Coud not allocate memory to hold Array !" << endl;
      exit(EXIT_FAILURE);
    }

  for(Int_t i=0;i<255;i++)
    *(Ptr++) = ((Float_t) i)/254.0 - 0.5; 
  
  Ptr = Array;
}
Random::~Random(void)
{
#ifdef DEBUG
  cout << "Random::Destructor" << endl;
#endif
  delete Array;
}

Float_t Random::Next(void)
{
#ifdef DEBUG
  //  cout << "Random::R()" << endl;
#endif
  if(Ptr < Array+254) 
    Ptr++;
  else
    Ptr = Array; 

  return(*Ptr);
}
Float_t Random::Value(void)
{
#ifdef DEBUG
  //  cout << "Random::(Float_t)R()" << endl;
#endif
  return(*Ptr);
}


