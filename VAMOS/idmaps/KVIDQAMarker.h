//Created by KVClassFactory on Wed Jul  9 15:38:39 2014
//Author: Guilain ADEMARD

#ifndef __KVIDQAMARKER_H
#define __KVIDQAMARKER_H

#include "TMarker.h"

class KVIDQALine;
class KVIDQAMarker : public TMarker
{

	protected:

		Int_t fA;  //Mass associated to this marker
		KVIDQALine *fParent; //! parent identification line

		void init();
   public:
   KVIDQAMarker();
   KVIDQAMarker( KVIDQALine *parent, Int_t a);
   virtual ~KVIDQAMarker();
   void Copy(TObject& obj) const;

   //-------------------- inline methods ------------------//

   void SetA( Int_t a );
   void SetParent( KVIDQALine *parent );

   ClassDef(KVIDQAMarker,1)//Base class for identification markers corresponding to differents couples of mass and charge state
};

//_____________________________________________________//

inline void KVIDQAMarker::SetA( Int_t a ){ fA = a; }
//_____________________________________________________//

inline void KVIDQAMarker::SetParent( KVIDQALine *parent ){ fParent = parent; }

#endif
