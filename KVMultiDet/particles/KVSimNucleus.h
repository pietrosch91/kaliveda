//Created by KVClassFactory on Mon Jun 28 15:02:00 2010
//Author: bonnet

#ifndef __KVSIMNUCLEUS_H
#define __KVSIMNUCLEUS_H

#include "KVNucleus.h"
#include "TVector3.h"
#include "KVNameValueList.h"

class KVSimNucleus : public KVNucleus
{

	protected:
	TVector3 position; 	// vector position of the particle in fm
	TVector3 angmom; 	// angular momentum of the particle in ???
	KVNameValueList *add_variables;	//additional variables
	Bool_t IsCleared;
	
	public:
	KVSimNucleus(){
		init();
	}
	
	void init(){
		add_variables = new KVNameValueList();
		IsCleared = kFALSE;
	
	}
	
   virtual ~KVSimNucleus(){
		if ( !IsCleared ) Clear();
	}
	
	void Clear(Option_t * opt=""){
		KVNucleus::Clear();
		delete add_variables;
		add_variables=0;
		
		IsCleared = kTRUE;	
	}
	
	void SetPosition(Double_t rx, Double_t ry, Double_t rz){
		position.SetXYZ(rx,ry,rz);
	}
	const TVector3* GetPosition() const {return &position; }
	
	void SetAngMom(Double_t lx, Double_t ly, Double_t lz){
		angmom.SetXYZ(lx,ly,lz);
	}
	const TVector3* GetAngMom() const {return &angmom; }
	
	KVNameValueList* GetParameters() { return add_variables; }
	
   ClassDef(KVSimNucleus,1)//essai_sim

};

#endif
