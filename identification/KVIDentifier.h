/*
$Id: KVIDentifier.h,v 1.3 2009/03/03 14:27:15 franklan Exp $
$Revision: 1.3 $
$Date: 2009/03/03 14:27:15 $
*/

//Created by KVClassFactory on Mon Apr 14 14:25:38 2008
//Author: franklan

#ifndef __KVIDENTIFIER_H
#define __KVIDENTIFIER_H

#include "Riostream.h"
#include "TCutG.h"
#include "KVNucleus.h"

class TF1;
class KVIDGraph;

class KVIDentifier : public TCutG
{
	protected:
	KVIDGraph 	*fParent;	//parent identification map or grid
   KVNucleus 	fIon;    	//ion/nucleus corresponding to this identifier
	
	Int_t fZ;//! dummy variables used by context menu dialog boxes
	Int_t fA;//! dummy variables used by context menu dialog boxes
	Int_t fMassFormula; //*OPTION={GetMethod="GetMassFormula";SetMethod="SetMassFormula";Items=(0="Beta-stability", 1="VEDA mass", 2="EAL mass", 3="EAL residues", 99="2Z+1")}*
	
	// Does nothing. Can be overridden in child classes in order to write any
	// extra information in between the name of the object and the number of points.
   virtual void WriteAsciiFile_extras(ofstream &, const Char_t * name_prefix =""){};
	
	// Does nothing. Can be overridden in child classes in order to read any
	// extra information in between the name of the object and the number of points.
   virtual void ReadAsciiFile_extras(ifstream &){};
   virtual void SetNameFromNucleus() { SetName(Form("Z=%d A=%d", GetZ(), GetA())); };	

	private:
	void init();
	
   public:
   KVIDentifier();
   KVIDentifier(const KVIDentifier&);
   KVIDentifier(const TCutG&);
   KVIDentifier(const TGraph&);
   virtual ~KVIDentifier();
	
	KVIDGraph*  GetParent()     const;
	void  		SetParent(KVIDGraph*);
	
   Int_t Compare(const TObject *) const;

   virtual void WriteAsciiFile(ofstream &, const Char_t * name_prefix ="");
   virtual void ReadAsciiFile(ifstream &);

   virtual Int_t GetID() const {return 0;};
   virtual Int_t GetA() const{ return fIon.GetA();};
   virtual Int_t GetZ() const{ return fIon.GetZ();};
   virtual void SetZ(Int_t atnum){ fIon.SetZ(atnum); SetNameFromNucleus(); }; // *MENU={Hierarchy="SetNucleus.../Z"}*
   virtual void SetA(Int_t atnum){ fIon.SetA(atnum); SetNameFromNucleus(); };  // *MENU={Hierarchy="SetNucleus.../A"}*
   virtual void SetAandZ(Int_t atnum,Int_t ztnum){fIon.SetZ(ztnum);  fIon.SetA(atnum); SetNameFromNucleus();};  // *MENU={Hierarchy="SetNucleus.../A and Z"}*
   virtual void SetMassFormula(Int_t mf){ fIon.SetMassFormula(mf); SetNameFromNucleus(); };   // *SUBMENU={Hierarchy="SetNucleus.../Mass Formula"}* 
   virtual Int_t GetMassFormula()const { return fIon.GetMassFormula(); }
	
   virtual Bool_t TestPoint(Double_t x, Double_t y)
	{
		// Abstract method, should be overridden in child classes.
		// Used to test whether a point (x,y) in the ID map is identifiable.
		
		AbstractMethod("TestMethod(Double_t x, Double_t y");
		return kFALSE;
	}
	
   void CopyGraph(TGraph *);
   void CopyGraph(const TGraph &);
	
	virtual void Scale(Double_t sx = -1, Double_t sy = -1);
	virtual void Scale(TF1 *sx, TF1 *sy);
	
	virtual void Print(Option_t * opt) const;
	
	virtual void WaitForPrimitive();
	
	virtual void ExtendLine(Option_t*,Double_t);  // *MENU*
	
   //---- The following redeclarations are here just to remove the *MENU* tag which
   //---- is present in TGraph.h, to stop these methods appearing in the ID line context menus
   virtual void      SetMaximum(Double_t maximum=-1111){TGraph::SetMaximum(maximum);}; 
   virtual void      SetMinimum(Double_t minimum=-1111){TGraph::SetMinimum(minimum);};
   virtual void      SetTitle(const char *title="") {TGraph::SetTitle(title);};
   virtual void      FitPanel() { TGraph::FitPanel(); };
   virtual void      DrawPanel() { TGraph::DrawPanel(); };
   virtual Int_t     Fit(const char *formula ,Option_t *option="" ,Option_t *goption="", Axis_t xmin=0, Axis_t xmax=0)
   { return TGraph::Fit(formula,option,goption,xmin,xmax);};
   virtual Int_t     Fit(TF1 *f1 ,Option_t *option="" ,Option_t *goption="", Axis_t xmin=0, Axis_t xmax=0)
   { return TGraph::Fit(f1,option,goption,xmin,xmax);};
   //---- The following redeclarations are here just to remove the *MENU* tag which
   //---- is present in TNamed.h, to stop these methods appearing in the ID line context menus
   virtual void     SetName(const char *name){TGraph::SetName(name);}; 
   //---- The following redeclarations are here just to remove the *MENU* tag which
   //---- is present in TObject.h, to stop these methods appearing in the ID line context menus
   //virtual void        Delete(Option_t *option=""){TGraph::Delete(option);}; 
   virtual void        DrawClass() const {TGraph::DrawClass();};
   virtual TObject    *DrawClone(Option_t *option="") const {return TGraph::DrawClone(option);};
   virtual void        Dump() const {TGraph::Dump();}; 
   virtual void        Inspect() const {TGraph::Inspect();};
   virtual void        SaveAs(const char *filename="",Option_t *option="") const {TGraph::SaveAs(filename,option);};
   virtual void        SetDrawOption(Option_t *option="") {TGraph::SetDrawOption(option);};
   virtual void        SetLineAttributes() {TGraph::SetLineAttributes();};
   virtual void        SetFillAttributes() {TGraph::SetFillAttributes();};
   virtual void        SetMarkerAttributes(){TGraph::SetMarkerAttributes();};
	
   ClassDef(KVIDentifier,1)//Base class for graphical cuts used in particle identification
};

#endif
