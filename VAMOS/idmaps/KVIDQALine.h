//Created by KVClassFactory on Fri Jun 20 12:07:39 2014
//Author: Guilain ADEMARD

#ifndef __KVIDQALINE_H
#define __KVIDQALINE_H

#include "KVIDZALine.h"

#include "KVIDQAMarker.h"
//class KVIDQAMarker;
class KVIDQALine : public KVIDZALine
{

	friend class KVIDQAGrid;

	protected:

		Int_t fQ;//! dummy variable used by context menu dialog boxes
		KVList *fMarkers; //-> list of Q-A identification markers

		void init();
		virtual void SetNameFromNucleus();
		void IncrementPtIdxOfMarkers( Int_t idx, Int_t ival=1 );

   	public:
   		KVIDQALine();
   		virtual ~KVIDQALine();
   		void Copy(TObject& obj) const;

		void IdentA( Double_t x, Double_t y, Int_t &A, Int_t &realA );
		
		virtual Int_t InsertMarker(Int_t a=0); // *MENU={Hierarchy="Modify Line.../InsertMarker"}*

		//---------- inline methods ---------------------//

		virtual Int_t GetQ() const;
   		virtual void SetQ(Int_t qtnum){ SetZ( qtnum ); }; // *MENU={Hierarchy="SetNucleus.../Q"}*
		virtual void AddMarker( KVIDQAMarker *marker );
		KVList *GetMarkers() const;
		//virtual Int_t RemoveMarker(); // *MENU={Hierarchy="Modify Line.../RemoveMarker"}*
		//virtual Int_t RemoveMarker(Int_t a);

   		ClassDef(KVIDQALine,1)//Base class for identification ridge lines and spots corresponding to different masses and charge states respectively
};

//_______________________________________________________________//

inline void KVIDQALine::SetNameFromNucleus() { 
	SetName(Form("Q=%d", GetQ())); 
	fMarkers->R__FOR_EACH(KVIDQAMarker,SetNameFromNucleus)();
};

//_______________________________________________________________//
inline void KVIDQALine::AddMarker( KVIDQAMarker *marker ){
	// Add QA-marker to the line. It will be deleted by the line.
	fMarkers->Add( marker );
	marker->SetParent( this );
}
//_______________________________________________________________//

inline KVList *KVIDQALine::GetMarkers() const{ return fMarkers; }
//_______________________________________________________________//

inline Int_t KVIDQALine::GetQ() const{
	// returns charge states
 	return GetZ();
};
//_______________________________________________________________//
/*
inline Int_t RemoveMarker(){
	Int_t idx = RemovePoint();
   	if( idx>=0 ){ return TCutG::RemovePoint(); } else {return -1;} 
};
//_______________________________________________________________//

inline Int_t RemovePoint(Int_t i){
   	return TCutG::RemovePoint(i);
};
*/

#endif
