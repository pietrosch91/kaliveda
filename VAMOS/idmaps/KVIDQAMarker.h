//Created by KVClassFactory on Wed Jul  9 15:38:39 2014
//Author: Guilain ADEMARD

#ifndef __KVIDQAMARKER_H
#define __KVIDQAMARKER_H

#include "TMarker.h"

class KVIDQALine;
class KVIDQAMarker : public TMarker
{
	friend class KVIDQALine;

	protected:

		Int_t fA;      //Mass associated to this marker
		Int_t fPtIdxLow;//index of the low closest point from this marker 
		Int_t fPtIdxUp; //index of the up closest point from this marker 
		KVIDQALine *fParent; //! parent identification line
		TString fName;

		void init();
		virtual void SetNameFromNucleus();

   	public:
   		KVIDQAMarker();
   		KVIDQAMarker( KVIDQALine *parent, Int_t a);
   		virtual ~KVIDQAMarker();
   		void Copy(TObject& obj) const;
   		void SetParent( KVIDQALine *parent );
   		Int_t GetQ() const;
		virtual void ls(Option_t* option = "") const;

   		//-------------------- inline methods ------------------//

		virtual const char*	GetName() const;
		void SetName(const Char_t *name);
   		Int_t GetA() const;
   		void SetA( Int_t a );
   		Int_t GetPointIndex() const;
   		void SetPointIndex( Int_t idx );
   		void GetPointIndexes( Int_t &idx_low, Int_t &idx_up ) const;
   		void SetPointIndexes( Int_t idx_low, Int_t idx_up );

   		ClassDef(KVIDQAMarker,1)//Base class for identification markers corresponding to differents couples of mass and charge state
};
//_____________________________________________________//

inline const char* KVIDQAMarker::GetName() const{ return fName.Data(); }
//_____________________________________________________//

inline void KVIDQAMarker::SetName(const Char_t *name){ fName = name; }
//_____________________________________________________//

inline void KVIDQAMarker::SetNameFromNucleus() { SetName(Form("Q=%d A=%d", GetQ(),GetA())); };
//_____________________________________________________//

inline Int_t KVIDQAMarker::GetA() const{ return fA; }
//_____________________________________________________//

inline void KVIDQAMarker::SetA( Int_t a ){ fA = a; SetNameFromNucleus();}
//_____________________________________________________//

inline Int_t KVIDQAMarker::GetPointIndex() const{ return (fPtIdxLow==fPtIdxUp ? fPtIdxLow : -1 ); }
//_____________________________________________________//

inline void KVIDQAMarker::SetPointIndex( Int_t idx ) { fPtIdxLow = fPtIdxUp = idx; }
//_____________________________________________________//

inline void KVIDQAMarker::GetPointIndexes( Int_t &idx_low, Int_t &idx_up ) const { idx_low = fPtIdxLow ; idx_up = fPtIdxUp; }


//_____________________________________________________//

inline void KVIDQAMarker::SetPointIndexes( Int_t idx_low, Int_t idx_up ) { fPtIdxLow = idx_low; fPtIdxUp = idx_up; }


#endif
