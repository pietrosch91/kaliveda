/***************************************************************************
                          KVIDGrid.h  -  description
                             -------------------
    begin                : Nov 10 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDGrid.h,v 1.44 2008/10/13 13:52:29 franklan Exp $
***************************************************************************/

#ifndef KVIDGrid_H
#define KVIDGrid_H

#include "KVBase.h"
#include "KVIDLine.h"
#include "KVGenParList.h"
#include "Riostream.h"
#include "KVReconstructedNucleus.h"
#include "TAttLine.h"
#include "TList.h"
#include <TH2.h>
#include <TPad.h>
#include <RQ_OBJECT.h>
#include "TGWindow.h"
#include "TF1.h"
#include "TClass.h"

class KVTGID;

class KVIDGrid:public KVBase, public TAttLine {

   RQ_OBJECT("KVIDGrid")

 private:

   KVList * fIDLines;           //-> list of identification lines
   KVList *fOKLines;            //-> cuts used to define area in which identification is possible
   Axis_t fXmin, fXmax;         //!min/max X coordinates of grid
   Axis_t fYmin, fYmax;         //!min/max Y coordinates of grid
   KVList* fParameters;      //obsolete parameter list, kept for backwards compatibility
   KVGenParList *fPar;      //-> parameters associated to grid

 protected:
       
       UChar_t fMassFormula;//!mass formula used for calculating identification from energy losses

    Double_t fLastScaleX;       //!last applied scaling factor on X
   Double_t fLastScaleY;        //!last applied scaling factor on Y
   void init();
   inline KVList *GetIDLines() const {
      return fIDLines;
   };
   inline KVList *GetOKLines() const {
      return fOKLines;
   };
   inline UInt_t NumberOfIDLines() const {
      return (GetIDLines()? GetIDLines()->GetSize() : 0);
   };
   inline UInt_t NumberOfOKLines() const {
      return (GetOKLines()? GetOKLines()->GetSize() : 0);
   };
   KVIDLine *GetIDLine(const Char_t * name) const;
   KVIDLine *GetOKLine(const Char_t * name) const;
   KVIDLine *GetIDLine(UInt_t index) const;
   KVIDLine *GetOKLine(UInt_t index) const;
   virtual void AddIDLine(Double_t x1, Double_t y1, Double_t x2,
                          Double_t y2, const Char_t * name,
                          const Char_t * classname = "");
   virtual void AddOKLine(Double_t x1, Double_t y1, Double_t x2,
                          Double_t y2, const Char_t * name,
                          const Char_t * classname = "");
   inline void AddIDLine(KVIDLine * line) {
      GetIDLines()->Add(line);
   };
   inline void AddOKLine(KVIDLine * line) {
      line->SetLineColor(kRed);
      GetOKLines()->Add(line);
   };
   inline void SortIDLines() {
      GetIDLines()->Sort();
   };
   Bool_t IsSorted() const;
   inline Axis_t GetXmin() const {
      return fXmin;
   };
   inline Axis_t GetYmin() const {
      return fYmin;
   };
   inline Axis_t GetXmax() const {
      return fXmax;
   };
   inline Axis_t GetYmax() const {
      return fYmax;
   };
   void FindAxisLimits();
   TList *GetIDLinesEmbracingPoint(const Char_t * direction, Double_t x,
                                   Double_t y, Int_t & nlines) const;
   void Scale(Double_t sx = -1, Double_t sy = -1);
   void Scale(TF1 *sx,TF1 *sy);

   virtual Bool_t AcceptIDForTest();
   
 public:

   KVIDGrid();
   KVIDGrid(const KVIDGrid &);
   virtual ~ KVIDGrid();

   virtual void WriteAsciiFile(const Char_t * filename);
   virtual void ReadAsciiFile(const Char_t * filename);
   virtual void ReadAsciiFile(ifstream & gridfile);
   virtual void WriteAsciiFile(ofstream & gridfile);

   // Return pointer to list of parameters associated to grid
   inline KVGenParList *GetParameters() const {
      return fPar;
   };
   
   inline virtual void SetXScaleFactor(Double_t = 0);
   inline virtual void SetYScaleFactor(Double_t = 0);
   inline virtual Double_t GetXScaleFactor();
   inline virtual Double_t GetYScaleFactor();

   virtual KVIDLine *NewLine(const Char_t * idline_class = "");
   virtual void AddLine(const Char_t * type, Double_t x1, Double_t y1,
                        Double_t x2, Double_t y2, const Char_t * name,
                        const Char_t * classname = "KVIDLine");
   void AddLine(const Char_t * type, KVIDLine * line);
   virtual void DrawAndAddLine(const Char_t * type =
                               "ID", const Char_t * classname =
                               "KVIDLine");
   KVIDLine *AddLine(const Char_t * type, const Char_t * idline_class =
                     "KVIDLine");
   KVList *GetLines(const Char_t * type) const;
   KVIDLine *GetLine(const Char_t * type, const Char_t * name) const;
   KVIDLine *GetLine(const Char_t * type, UInt_t index) const;
   void RemoveLine(const Char_t * type, const Char_t * name);
   void RemoveLine(const Char_t * type, KVIDLine * line);

   virtual KVIDLine *FindNearestIDLine(Double_t x, Double_t y,
                                       const Char_t * position,
                                       Int_t & idx_min,
                                       Int_t & idx_max) const;

   virtual void Draw(Option_t * opt = "");
   virtual void UnDraw();

   virtual void Identify(Double_t /*x*/, Double_t /*y*/,
                         KVReconstructedNucleus * /*nuc*/) const {
      AbstractMethod("Identify");
   };
   
   virtual void Initialize();

   virtual Bool_t IsIdentifiable(Double_t /*x*/, Double_t /*y*/) const {
      return kTRUE;
   };

   void Print(Option_t * opt = "") const;

   virtual void SetLineColor(Color_t lcolor);
   virtual void SetLineStyle(Style_t lstyle);
   virtual void SetLineWidth(Width_t lwidth);

   virtual void Clear(Option_t * opt = "");

   virtual void Fit(KVTGID *);
   virtual void fcnk0(Int_t & npar, Double_t * gin, Double_t & f,
                      Double_t * par, Int_t iflag);
   virtual Double_t TGIDChiSquare(KVTGID * _tgid, Double_t * params = 0);

   void FitStartingPoints(Double_t & lambda, Double_t & alpha,
                          Double_t & beta, Double_t & g);

   virtual const Char_t *GetName() const;
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif
   virtual void MakeELossGrid(UInt_t, Bool_t = kFALSE , Double_t =
                              -1.0, Int_t = 20) {
   };
   virtual void TestIdentification(TH2F * data, TH1F * id_real,
                           TH2F * id_real_vs_e_res);
   void Increment(Float_t);     // *SIGNAL*

   static KVIDGrid *AddGrids(KVIDGrid * g1, Int_t id1_min, Int_t id1_max,
                             KVIDGrid * g2, Int_t id2_min, Int_t id2_max);

   virtual void NewGridDialog(const TGWindow * p, const TGWindow * main,
                              UInt_t w = 1, UInt_t h = 1, Option_t * type =
                              "New");

   // Set mass formula used for calculating masses of nuclei when calculating
   // Z-identification lines for ID grids. See KVNucleus::GetAFromZ
   virtual void SetMassFormula(UChar_t mf){ fMassFormula = mf; };
   
   // Returns mass formula used for calculating masses of nuclei when calculating
   // Z-identification lines for ID grids. See KVNucleus::GetAFromZ
   virtual UChar_t GetMassFormula(){ return fMassFormula; };
   
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
		for (Int_t nn=0;nn<lObject->GetEntries();nn+=1) idgrid->AddLine("ID",KVIDLine::MakeIDLine(lObject->At(nn),xdeb,xfin,np,save));
		return idgrid;
	}
	
   virtual void CalculateLineWidths(){};
	
	virtual TClass* DefaultIDLineClass(){
		return TClass::GetClass("KVIDLine");
	};
	virtual TClass* DefaultOKLineClass(){
		return TClass::GetClass("KVIDLine");
	};
   
	ClassDef(KVIDGrid, 4)        //Base class for 2D identification grids
};

inline void KVIDGrid::SetXScaleFactor(Double_t s)
{
   //Set scaling factor for X-axis - rescales all lines of grid with this factor
   //SetXScaleFactor() or SetXScaleFactor(0) removes scale factor
   if (s > 0) {
      fPar->SetValue("XScaleFactor", s);
      Scale(s / fLastScaleX);
      fLastScaleX = s;
   } else {
      fPar->RemoveParameter("XScaleFactor");
      Scale(1.0 / fLastScaleX);
      fLastScaleX = 1.0;
   }
}

inline void KVIDGrid::SetYScaleFactor(Double_t s)
{
   //Set scaling factor for Y-axis - rescales all lines of grid with this factor
   //SetYScaleFactor() or SetYScaleFactor(0) removes scale factor
   if (s > 0) {
      fPar->SetValue("YScaleFactor", s);
      Scale(-1.0, s / fLastScaleY);
      fLastScaleY = s;
   } else {
      fPar->RemoveParameter("YScaleFactor");
      Scale(-1.0, 1.0 / fLastScaleY);
      fLastScaleY = 1.0;
   }
}

inline Double_t KVIDGrid::GetXScaleFactor()
{
   //Return scaling factor for X-axis
   //If factor not defined, returns 1
   Double_t s = fPar->GetDoubleValue("XScaleFactor");
   if (s > 0)
      return s;
   return 1.0;
}

inline Double_t KVIDGrid::GetYScaleFactor()
{
   //Return scaling factor for Y-axis
   //If factor not defined, returns 1
   Double_t s = fPar->GetDoubleValue("YScaleFactor");
   if (s > 0)
      return s;
   return 1.0;
}
#endif
