/***************************************************************************
                          KVIDGrid.h  -  description
                             -------------------
    begin                : Nov 10 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDGrid.h,v 1.48 2009/03/13 16:51:08 ebonnet Exp $
***************************************************************************/

#ifndef KVIDGrid_H
#define KVIDGrid_H

#include "KVIDGraph.h"
#include "KVIDLine.h"
#include "Riostream.h"
#include "KVReconstructedNucleus.h"
#include "TList.h"
#include <TPad.h>
#include "TGWindow.h"
#include "TF1.h"
#include "TClass.h"

class KVIDGrid : public KVIDGraph {

 protected:

   void init();
	void ReadIdentifierFromAsciiFile(TString &name, TString &type, TString &cl, ifstream& gridfile);
   
 public:

   KVIDGrid();
   virtual ~ KVIDGrid();
   
   virtual KVIDLine *NewLine(const Char_t * idline_class = "");
   TList *GetIDLinesEmbracingPoint(const Char_t * direction, Double_t x,
                                   Double_t y, Int_t & nlines) const;
   
   virtual KVIDLine *FindNearestIDLine(Double_t x, Double_t y,
                                       const Char_t * position,
                                       Int_t & idx_min,
                                       Int_t & idx_max) const;

   void Initialize();
   void Identify(Double_t /*x*/, Double_t /*y*/, KVReconstructedNucleus * /*nuc*/) const
	{AbstractMethod("Identify");};   

   virtual void SimulateGrid(const Char_t* strz="1-90", Bool_t thickness_factor=1.5, Double_t deltaMasse=0, Bool_t PHD_param=kFALSE, Int_t npoints=20);//  *MENU={Hierarchy="Test.../MakeELossGrid"}*
	virtual void MakeELossGrid(UInt_t Zmax, Bool_t withPHD=kFALSE,
                                Double_t ChIo_press=-1, Int_t npoints=20){};

   virtual void NewGridDialog(const TGWindow * p, const TGWindow * main,
                              UInt_t w = 1, UInt_t h = 1, Option_t * type =
                              "New");
   
	/*
	static KVIDGrid *MakeIDGrid(TH2 *hh,TList*lCutG,Double_t xdeb=-1.,Double_t xfin=-1.,Double_t np=1.,Bool_t save=kFALSE){
		KVIDGrid *idgrid = new KVIDGrid();
		for (Int_t nn=0;nn<lCutG->GetEntries();nn+=1) idgrid->AddLine("ID",KVIDLine::MakeIDLine(hh,(TCutG *)lCutG->At(nn),xdeb,xfin,np,save));
		return idgrid;
	}
	*/
	
	static KVIDGrid *MakeIDGrid(TList*lObject, Double_t xdeb=-1.,Double_t xfin=-1.,Double_t np=1.,Bool_t save=kFALSE){
		// Create an KVIDGrid using a list of objects which are converting in KVIDLine with the static method KVIDLine::MakeIDLine
		// See reference in the KVIDLine doc for the rules of parameters
		KVIDGrid *idgrid = new KVIDGrid();
		for (Int_t nn=0;nn<lObject->GetEntries();nn+=1) idgrid->Add("ID",KVIDLine::MakeIDLine(lObject->At(nn),xdeb,xfin,np,save));
		return idgrid;
	}
	
   virtual void CalculateLineWidths(){};
	
	virtual TClass* DefaultIDLineClass(){
		return TClass::GetClass("KVIDLine");
	};
	virtual TClass* DefaultOKLineClass(){
		return TClass::GetClass("KVIDLine");
	};
      
	
	void FitPanel(); // *MENU*
   
	ClassDef(KVIDGrid, 5)        //Base class for 2D identification grids
};

#endif
