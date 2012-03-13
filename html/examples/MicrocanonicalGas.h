//Created by KVClassFactory on Thu Jun 24 14:22:17 2010
//Author: John Frankland,,,

#ifndef __MicrocanonicalGas_H
#define __MicrocanonicalGas_H

#include "KVRungeKutta.h"
#include "KVHashList.h"
#include "TTree.h"
#include "TFile.h"
#include <TH1F.h>
#include "Riostream.h"

class MicrocanonicalGas : public KVRungeKutta
{
	Int_t Nparts;
	Double_t *coords;
	Double_t boxradius;
	Double_t sphereradius;
   Double_t xsec;
	Double_t Time;
	Double_t Etot0,Etot;
   
   TFile* TheFile;
   TTree* TheTree;
   
   Double_t collRate;
   Double_t* TheEnergies;//!
   Double_t* TheDistances;//!
   Double_t* TheVelocities;//!
	
   public:
   MicrocanonicalGas(Int_t n, Double_t e0, Double_t boxsize, Double_t spheresize);
   virtual ~MicrocanonicalGas();

	virtual void CalcDerivs(Double_t /*X*/ , Double_t* /*Y*/, Double_t* /*DY/DX*/);
	
	Double_t GetEKin() const;
	Double_t GetR(Int_t i) const;
	Double_t GetV(Int_t i) const;
	Double_t GetE(Int_t i) const;
	void SetRandomCoords(Double_t e);
	void Run(Double_t tmin, Double_t tmax, Double_t dt_coll, Double_t dt_write);
	Int_t Collision(Int_t xi, Int_t xj, Double_t *_dx);
	Int_t CollisionDetection();
	void CheckBoundaries();
	
	void Print(Option_t* opt="") const;
	void WriteResults();
	
   ClassDef(MicrocanonicalGas,1)//Simulate a microcanonical classical gas using KVRungeKutta
};

#endif
