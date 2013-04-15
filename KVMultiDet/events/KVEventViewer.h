//Created by KVClassFactory on Fri Apr 12 15:48:03 2013
//Author: John Frankland,,,

#ifndef __KVEVENTVIEWER_H
#define __KVEVENTVIEWER_H

#include "KVBase.h"
#include "KVEvent.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"
#include "Riostream.h"
using namespace std;

class TBranch;
class TTree;

class KVEventViewer : public KVBase
{
   protected:
   Int_t fproton_color;//proton colour
   Int_t fneutron_color;//neutron colour
   Int_t fProton_color;//proton colour for highlighted nuclei
   Int_t fNeutron_color;//neutron colour for highlighted nuclei
   Double_t free_nucleon_radius;//radius of free nucleons
   Double_t nucleon_radius;//radius of nucleons in nuclei
   Double_t nuclear_radius_parameter;//to calculate radii of nuclei

   Int_t ivol;//geovolume counter
   Bool_t fSavePicture;//kTRUE to save in GIF file
   
   TGeoMaterial *matEmptySpace;
   TGeoMaterial *matNuc;
   TGeoMaterial *matBox;

   TGeoMedium *EmptySpace;
   TGeoMedium *Nuc;
   TGeoMedium *Box;
   TGeoVolume *top;
   TGeoManager *geom;
   
   KVEvent* theEvent;
   
   ifstream eventFile;
   Int_t eventCounter;
   Bool_t textInput;

   TTree* theTree;
   Long64_t NtreeEntries;

   Double_t maxV;//largest velocity of event
   Int_t maxZ;//largest Z of event

   Int_t fHighlightMode;
   Bool_t fAxesMode;
    
   public:
   enum EHighlightMode {
       kNoHighlight,
       kHighlightZmax
   };
   KVEventViewer(Int_t protoncolor=kRed-2, Int_t neutroncolor=kBlue-2,
                 Int_t highlightprotoncolor=kOrange-2,
                 Int_t highlightneutroncolor=kGreen-2,
                 Double_t freenucleonradius=0.2,
                 Double_t nucleonradius=0.25,
                 Double_t nuclearradiusparameter=0.2);
   virtual ~KVEventViewer();
   
   void SetProtonColor(Int_t c) { fproton_color=c; }
   void SetNeutronColor(Int_t c) { fneutron_color=c; }
   void SetHighlightProtonColor(Int_t c) { fProton_color=c; }
   void SetHighlightNeutronColor(Int_t c) { fNeutron_color=c; }
   void SetSavePicture(Bool_t yes=kTRUE) { fSavePicture=yes; }

   void DrawNucleus(KVNucleus*, const Char_t* frame="");
   void DrawEvent(KVEvent*, const Char_t *frame="");
   
   void SetInput(TBranch* eventbranch);
   void SetInput(const char* filename);

   void ReadEvent();
   void ReadTextEvent();
   void ReadTreeEvent();
   
   void DrawNextEvent();

   virtual Bool_t SetHighlight(KVNucleus* n);
   virtual void SetHighlightMode(Int_t m)
   {
       // Select nuclei to highlight
       // m = KVEventViewer::kHighlightZmax : highlight biggest fragment (Z)
       // m = KVEventViewer::kNoHighlight   : turn off highlighting
       fHighlightMode=m;
   }
   
   void SetAxesMode(Bool_t on=kTRUE)
   {
       // Show axes or not
       fAxesMode=on;
   }

   void DumpCurrentEvent() { if(theEvent) theEvent->Print("ok"); }

   ClassDef(KVEventViewer,1)//Draw events in 3D using OpenGL
};

#endif
