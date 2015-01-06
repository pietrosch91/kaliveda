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
		Double_t fDelta; // normalized position from low closest point and up closest point
		KVIDQALine *fParent; //! parent identification line
		TString fName;

		void init();
		virtual void SetNameFromNucleus();
		virtual void UpdateMarkerStyle();

   	public:
   		KVIDQAMarker();
   		KVIDQAMarker( KVIDQALine *parent, Int_t a);
   		virtual ~KVIDQAMarker();
   		void Copy(TObject& obj) const;
		virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   		void SetParent( KVIDQALine *parent );
   		Int_t GetQ() const;
		virtual void ls(Option_t* option = "") const;
		virtual void UpdateXandY();

   		//-------------------- inline methods ------------------//

		virtual const char*	GetName() const;
		void SetName(const Char_t *name);
   		Int_t GetA() const;
   		void SetA( Int_t a ); // *MENU*
   		Int_t GetPointIndex() const;
   		void SetPointIndex( Int_t idx );
   		void GetPointIndexes( Int_t &idx_low, Int_t &idx_up ) const;
   		void SetPointIndexes( Int_t idx_low, Int_t idx_up, Int_t delta=0 );

   		ClassDef(KVIDQAMarker,1)//Base class for identification markers corresponding to differents couples of mass and charge state
};
//_____________________________________________________//

inline const char* KVIDQAMarker::GetName() const{ return fName.Data(); }
//_____________________________________________________//

inline void KVIDQAMarker::SetName(const Char_t *name){ fName = name; }
//_____________________________________________________//

inline void KVIDQAMarker::SetNameFromNucleus() { 
	SetName(Form("Q=%d A=%d", GetQ(),GetA())); 
	UpdateMarkerStyle();
};
//_____________________________________________________//

inline void KVIDQAMarker::UpdateMarkerStyle(){
	// Set marker style as a function of the fA value
	static Int_t ms[]={ 20, 24, 21, 25, 22, 26, 29, 30};
	Int_t s = ms[fA%8];
	SetMarkerStyle( s );
}
//_____________________________________________________//

inline Int_t KVIDQAMarker::GetA() const{ return fA; }
//_____________________________________________________//

inline void KVIDQAMarker::SetA( Int_t a ){ fA = a; SetNameFromNucleus();}
//_____________________________________________________//

inline Int_t KVIDQAMarker::GetPointIndex() const{ return (fPtIdxLow==fPtIdxUp ? fPtIdxLow : -1 ); }
//_____________________________________________________//

inline void KVIDQAMarker::SetPointIndex( Int_t idx ) { 
	SetPointIndexes( idx, idx );
}
//_____________________________________________________//

inline void KVIDQAMarker::GetPointIndexes( Int_t &idx_low, Int_t &idx_up ) const { idx_low = fPtIdxLow ; idx_up = fPtIdxUp; }


//_____________________________________________________//

inline void KVIDQAMarker::SetPointIndexes( Int_t idx_low, Int_t idx_up, Int_t delta ) { 
	fPtIdxLow = idx_low;
   	fPtIdxUp  = idx_up;
	fDelta    = delta;
	UpdateXandY();
}


#endif
