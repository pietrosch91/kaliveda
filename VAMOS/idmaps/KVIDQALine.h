//Created by KVClassFactory on Fri Jun 20 12:07:39 2014
//Author: Guilain ADEMARD

#ifndef __KVIDQALINE_H
#define __KVIDQALINE_H

#include "KVIDZALine.h"

class KVIDQALine : public KVIDZALine
{

	friend class KVIDQAGrid;

	protected:

		Int_t fQ;//! dummy variable used by context menu dialog boxes

		virtual void SetNameFromNucleus() { SetName(Form("Q=%d", GetQ())); };

   	public:
   		KVIDQALine();
   		virtual ~KVIDQALine();
   		void Copy(TObject& obj) const;

		void IdentA( Double_t x, Double_t y, Int_t &A, Int_t &realA );

		virtual Int_t GetQ() const{
			// returns charge states
 		   	return GetZ();
		};
   		virtual void SetQ(Int_t qtnum){ SetZ( qtnum ); }; // *MENU={Hierarchy="SetNucleus.../Q"}*

   		ClassDef(KVIDQALine,1)//Base class for identification ridge lines and spots corresponding to different masses and charge states respectively
};

#endif
