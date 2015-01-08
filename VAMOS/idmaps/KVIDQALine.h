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
		Int_t fNextA;//! next A value for the next call of InsertMarker
		Bool_t fNextAinc;//! true if the next A value should increase

		KVList *fMarkers; //-> list of Q-A identification markers

		void init();
		virtual void SetNameFromNucleus();
		virtual void UpdateLineStyle();

   	public:
   		KVIDQALine();
   		virtual ~KVIDQALine();
   		void Copy(TObject& obj) const;
		virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);
		void IdentA( Double_t x, Double_t y, Int_t &A, Int_t &realA );
		
		virtual Int_t InsertMarker(Int_t A=0); // *MENU* *ARGS={A=>fNextA}
		virtual Int_t RemoveMarker(Int_t a=0); //*MENU*
		virtual Int_t InsertPoint();//*MENU*
		virtual Int_t InsertPoint(Int_t i, Double_t x, Double_t y);
		using KVIDZALine::RemovePoint;//*MENU*
		virtual Int_t RemovePoint(Int_t i);
		void IncrementPtIdxOfMarkers( Int_t idx, Int_t ival=1 );

		void FindAMarkers(TH1 *h);

		Int_t GetNextA() const { return fNextA; }
		void  SetNextA(Int_t A) { fNextA = A; }

		//---------- inline methods ---------------------//

		virtual Int_t GetQ() const;
   		virtual void SetQ(Int_t qtnum){ SetZ( qtnum ); }; // *MENU={Hierarchy="SetNucleus.../Q"}*
		virtual void AddMarker( KVIDQAMarker *marker );
		KVList *GetMarkers() const;
		//virtual Int_t RemoveMarker(); // *MENU={Hierarchy="Modify Line.../RemoveMarker"}*
		//virtual Int_t RemoveMarker(Int_t a);

   		ClassDef(KVIDQALine,1)//Base class for identification ridge lines and spots corresponding to different charge states and masses respectively
};

//_______________________________________________________________//

inline void KVIDQALine::SetNameFromNucleus() { 
	SetName(Form("Q=%d", GetQ())); 
	fMarkers->R__FOR_EACH(KVIDQAMarker,SetNameFromNucleus)();
	UpdateLineStyle();
};
//_______________________________________________________________//

inline void KVIDQALine::UpdateLineStyle(){
	// Set line style and color as a function of the fQ value
	static Int_t lc[] ={ kGray+1, kGray+2, kGray+3, kBlack};
	Int_t c = lc[fA%4];
	SetLineColor( c );

}
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
