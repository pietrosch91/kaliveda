//Created by KVClassFactory on Mon Jun 28 15:02:00 2010
//Author: bonnet

#ifndef __KVSIMNUCLEUS_H
#define __KVSIMNUCLEUS_H

#include "KVNucleus.h"
#include "KVNameValueList.h"

class TVector3;

class KVSimNucleus : public KVNucleus
{

	protected:
	TVector3 position; 	// vector position of the particle in fm
	TVector3 angmom; 	// angular momentum of the particle in ???
	
	public:
	
	KVSimNucleus();
	virtual ~KVSimNucleus();
	
	void Copy(TObject& obj) const;
	
	void SetPosition(Double_t rx, Double_t ry, Double_t rz);
	const TVector3* GetPosition() const;
	
	void SetAngMom(Double_t lx, Double_t ly, Double_t lz);
	const TVector3* GetAngMom() const;
	
	ClassDef(KVSimNucleus,2)//essai_sim

};

#endif
