/*
$Id: KVIDGraph.h,v 1.2 2009/03/03 13:36:00 franklan Exp $
$Revision: 1.2 $
$Date: 2009/03/03 13:36:00 $
*/

//Created by KVClassFactory on Mon Apr 14 13:42:47 2008
//Author: franklan

#ifndef __KVIDGRAPH_H
#define __KVIDGRAPH_H

#include "TCutG.h"
#include "KVGenParList.h"
#include "KVIDentifier.h"
#include <RQ_OBJECT.h>
#include <TH2.h>
#include "KVNumberList.h"
#include "KVIDTelescope.h"

class KVReconstructedNucleus;
class TVirtualPad;

class KVIDGraph : public TCutG
{
	friend class KVIDGridManager;
	
   RQ_OBJECT("KVIDGraph")

	protected:

   Bool_t 			fOnlyZId;			//set to kTRUE when only to be used to give Z identification of nuclei, no mass info
   KVList 			*fIdentifiers;		//-> list of identification objects
   KVList 			*fCuts;       		//-> cuts used to define area in which identification is possible
   Axis_t 			fXmin, fXmax; 		//!min/max X coordinates of graph
   Axis_t 			fYmin, fYmax; 		//!min/max Y coordinates of graph
   KVGenParList 	*fPar;				//-> parameters associated to grid
   Double_t 		fLastScaleX;		//last applied scaling factor on X
   Double_t 		fLastScaleY;		//last applied scaling factor on Y
	TVirtualPad 	*fPad; 				//!pad in which graph is drawn 
	KVNumberList	fRunlist;			//runs for which grid is valid
	TList 			fTelescopes;		//ID telescopes for which grid is valid
	TString			fDyName; 			//!dynamically generated name
   TString  		fPattern;			//pattern of filenames used to write or read grid
	
   void FindAxisLimits();
   void Scale(Double_t sx = -1, Double_t sy = -1);
   virtual void ReadFromAsciiFile(ifstream & gridfile);
   virtual void WriteToAsciiFile(ofstream & gridfile);
   void init();
	virtual KVIDentifier* New(const Char_t*);
	virtual void ReadIdentifierFromAsciiFile(TString &name, TString &type, TString &cl, ifstream& gridfile);
	void FillListOfIDTelescopes();
	void WriteParameterListOfIDTelescopes();
	virtual void BackwardsCompatibilityFix();
	Bool_t AcceptIDForTest()
	{
   	// Used by TestIdentification.
   	// The result of the identification may be excluded from the histograms of PID
   	// and PID vs. Eres, depending on e.g. some status code of the identification algorithm.
   	// By default, this returns kTRUE (accept all), but may be overridden in child classes.
   	return kTRUE;
	};
	
	
   public:
			
   KVIDGraph();
   KVIDGraph(const KVIDGraph &);
   virtual ~KVIDGraph();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif
	virtual void Clear(Option_t* opt="");
   virtual void Identify(Double_t /*x*/, Double_t /*y*/, KVReconstructedNucleus * /*nuc*/) const = 0;   
   virtual void Initialize()=0;
   virtual Bool_t IsIdentifiable(Double_t /*x*/, Double_t /*y*/) const;

   void Scale(TF1 *sx,TF1 *sy);
	
   KVIDentifier *GetIdentifier(Int_t Z, Int_t A=0) const;
   void RemoveIdentifier(KVIDentifier*);
   void RemoveCut(KVIDentifier*);
	
   void WriteAsciiFile(const Char_t * filename);
   void ReadAsciiFile(const Char_t * filename);
   void SetXScaleFactor(Double_t = 0);
   void SetYScaleFactor(Double_t = 0);
   Double_t GetXScaleFactor();
   Double_t GetYScaleFactor();
   void Add(TString, KVIDentifier *);
   KVIDentifier* Add(TString, TString);
   void DrawAndAdd(TString, TString);
   void Draw(Option_t * opt = "");
   void UnDraw();
   void Print(Option_t * opt = "") const;
   void TestIdentification(TH2F * data, TH1F * id_real,
                           TH2F * id_real_vs_e_res);
   static KVIDGraph *AddGraphs(KVIDGraph * g1, Int_t id1_min, Int_t id1_max,
                             KVIDGraph * g2, Int_t id2_min, Int_t id2_max);
	void SetRuns(const KVNumberList& nl);
	const Char_t* GetName() const;
	virtual void WriteAsciiFile_WP(Int_t version=-1);//WP means with pattern
   virtual void ReadAsciiFile_WP(Int_t version=-1); //WP means with pattern
	virtual Int_t CheckVersion(Int_t version);
	virtual Bool_t ExistVersion(Int_t version){
		if (version!=-1) return (version==CheckVersion(version));
		else return (version!=-1);
	};
   void SetPattern(TString pattern) { pattern.ReplaceAll(".","_"); fPattern = pattern;}	
   TString GetPattern(void) {return fPattern;}
	
   void Increment(Float_t x)  // *SIGNAL*
	{
   	// Used by TestIdentification and KVTestIDGridDialog to send
   	// signals to TGHProgressBar about the progress of the identification test
   	Emit("Increment(Float_t)", x);
	};
	
   // Use this method if the graph is only to be used for Z identification
   // (no isotopic information). Default is to identify both Z & A
   // (fOnlyZid = kFALSE). Note that setting fOnlyZid=kTRUE changes the way line
   // widths are calculated (see KVIDGrid::CalculateLineWidths)
   void SetOnlyZId(Bool_t yes=kTRUE) { fOnlyZId = yes; };
   // Returns kTRUE if the graph is only to be used for Z identification
   // (no isotopic information). Default is to identify both Z & A
   // (fOnlyZid = kFALSE)
   Bool_t OnlyZId() const { return fOnlyZId; };
   
	const KVNumberList& GetRuns() const
	{
		// Get list of runs for which grid is valid
		return fRunlist;
	};
   KVIDentifier *GetIdentifier(const Char_t * name) const
	{
		return (KVIDentifier*)fIdentifiers->FindObjectByName(name);
	};
   KVIDentifier *GetCut(const Char_t * name) const
	{
		return (KVIDentifier*)fCuts->FindObjectByName(name);
	};
   // Return pointer to list of parameters associated to grid
   KVGenParList *GetParameters() const {
      return fPar;
   };
   KVList *GetIdentifiers() const {
		// Returns list of identifier objects (derived from KVIDentifier)
      return fIdentifiers;
   };
   KVList *GetCuts() const {
		// Returns list of cuts (derived from KVIDentifier)
      return fCuts;
   };
   Int_t GetNumberOfIdentifiers() const {
      return (fIdentifiers ? fIdentifiers->GetSize() : 0);
   };
   Int_t GetNumberOfCuts() const {
      return (fCuts? fCuts->GetSize() : 0);
   };
   void AddIdentifier(KVIDentifier *id) {
		// Add identifier to the graph. It will be deleted by the graph.
      fIdentifiers->Add(id);
		id->SetParent(this);
		id->SetVarX(GetVarX());
		id->SetVarY(GetVarY());
   };
   void AddCut(KVIDentifier *cut) {
		// Add cut to the graph. It will be deleted by the graph.
      cut->SetLineColor(kRed);
		cut->SetParent(this);
      fCuts->Add(cut);
		cut->SetVarX(GetVarX());
		cut->SetVarY(GetVarY());
   };
   void SortIdentifiers() {
      fIdentifiers->Sort();
   };
   Bool_t IsSorted() const
	{
		// Return kTRUE if list of identifiers has been sorted
		return fIdentifiers->IsSorted();
	};
   Axis_t GetXmin() const {
      return fXmin;
   };
   Axis_t GetYmin() const {
      return fYmin;
   };
   Axis_t GetXmax() const {
      return fXmax;
   };
   Axis_t GetYmax() const {
      return fYmax;
   };
   // Return quality code related to previously-attempted identification using grid.
   // Redefine in child classes.
   virtual Int_t GetQualityCode() const
   {
      return 0;
   };
	void SetLineColor(Color_t lcolor)
	{
   	//Set line colour of all objects in grid
   	fIdentifiers->Execute("SetLineColor", Form("%d", (Int_t) lcolor));
   	fCuts->Execute("SetLineColor", Form("%d", (Int_t) lcolor));
	};
	void SetLineStyle(Style_t lstyle)
	{
   	//Set line style of all objects in grid
   	fIdentifiers->Execute("SetLineStyle", Form("%d", (Int_t) lstyle));
   	fCuts->Execute("SetLineStyle", Form("%d", (Int_t) lstyle));
	};
	void SetLineWidth(Width_t lwidth)
	{
   	//Set line width of all objects in grid
   	fIdentifiers->Execute("SetLineWidth", Form("%d", (Int_t) lwidth));
   	fCuts->Execute("SetLineWidth", Form("%d", (Int_t) lwidth));
	};
	void AddIDTelescope(KVIDTelescope*t)
	{
		// Associate this graph with an identification telescope, i.e. add the
		// telescope to the list of telescopes which can use this
		// graph for identification purposes.
	
		fTelescopes.Add(t);
	};
	void RemoveIDTelescope(KVIDTelescope*t)
	{
		// Remove telescope from the list of telescopes
		// which can use this graph for identification purposes.
	
		fTelescopes.Remove(t);
	};
	Bool_t HandlesIDTelescope(KVIDTelescope*t) const
	{
		// Returns kTRUE if telescope can use this graph for identification purposes.
	
		return fTelescopes.Contains(t);
	};

   ClassDef(KVIDGraph,1)//Base class for particle identification in a 2D map
};

#endif
