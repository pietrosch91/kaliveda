/***************************************************************************
                          KVIDGChIoSi.h  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDGChIoSi.h,v 1.22 2008/01/17 15:28:27 franklan Exp $
***************************************************************************/

#ifndef KVIDGChIoSi_H
#define KVIDGChIoSi_H

#include "KVIDZGrid.h"
#include "TString.h"
#include "KVINDRARRMValidator.h"

class KVIDGChIoSi:public KVIDZGrid, public KVINDRARRMValidator {

 private:

   TString fPattern;				  //pattern of files to write or read grid !!! AVOID "." in it !!!
										  //they are replaced by "_"
	Int_t fStatus;               //!status of identification
   KVIDLine *fBragg;            //bragg line
   KVIDLine *fPunch;            //punch-through line
   KVIDLine *fSeuil;            //seuil silicium line
   KVIDLine *fEmaxSi;           //saturation codeur silicon

   Bool_t fIgnorePunchThrough;      //when set to kTRUE, points below punch-through line are not rejected by IsIdentifiable
   
   void init();
   
 public:

   void SetIgnorePunchThrough(Bool_t ignore = kTRUE) {
      fIgnorePunchThrough=ignore;
   };
 
   enum {
      kOK,                      //normal identification
      k_BelowPunchThrough,      //point to ID was below punch-through line (bruit)
      k_BelowSeuilSi,           //point to ID was left of "Seuil Si" line (bruit/arret ChIo?)
      k_LeftOfBragg,            //point to ID is between 'Seuil Si' and 'Bragg': Z given is a Zmin.
      k_RightOfEmaxSi,          //point to ID has E_Si > Emax_Si i.e. codeur is saturated. Unidentifiable.
      k_ZgtZmax                 //point to ID is higher (in ChIo) than line with biggest Z. Z given is a Zmin.
   };

    KVIDGChIoSi();
    KVIDGChIoSi(const KVIDGChIoSi &);
    KVIDGChIoSi(TString pattern);
    virtual ~ KVIDGChIoSi();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   void Copy(TObject & obj) const;
#else
   void Copy(TObject & obj);
#endif

   void SetRings(Int_t rmin, Int_t rmax);
   void SetModules(Int_t mmin, Int_t mmax);
   void SetRuns(Int_t rmin, Int_t rmax);
   void SetPattern(TString pattern) { pattern.ReplaceAll(".","_"); fPattern = pattern;}	
   TString GetPattern(void) {return fPattern;}
	Int_t GetMinRing() {
      return GetRingList().First();
   };
   Int_t GetMaxRing() {
      return GetRingList().Last();
   };
   Int_t GetMinMod() {
      return GetModuleList().First();
   };
   Int_t GetMaxMod() {
      return GetModuleList().Last();
   };
   Int_t GetFirstRun() {
      return GetRunList().First();
   };
   Int_t GetLastRun() {
      return GetRunList().Last();
   };
	KVIDLine *GetSeuilSiLine() {
      return (fSeuil);
   };
   KVIDLine *GetBraggLine() {
      return (fBragg);
   };
   KVIDLine *GetPunchThroughLine() {
      return (fPunch);
   };
   KVIDLine *GetEmaxSiLine() {
      return (fEmaxSi);
   };

   virtual Bool_t IsIdentifiable(Double_t x, Double_t y) const;
   void MakeELossGrid(UInt_t Zmax, Bool_t PHD_param =
                      kFALSE, Double_t chio_press = -1.0, Int_t npoints = 20);

   inline Int_t GetStatus() const {
      return fStatus;
   };

   virtual void Initialize();

   virtual const Char_t *GetName() const;
   virtual void ReadAsciiFile(ifstream & gridfile);
   virtual void WriteAsciiFile(ofstream & gridfile);

   virtual void WriteAsciiFile(TString filename){
		ofstream f_out(filename); WriteAsciiFile(f_out); f_out.close();
	}
   virtual void ReadAsciiFile(TString filename){
		ifstream f_in(filename); ReadAsciiFile(f_in); f_in.close();
	}
	virtual void WriteAsciiFile_WP(Int_t version=-1);//WP means with pattern
   virtual void ReadAsciiFile_WP(Int_t version=-1); //WP means with pattern
	virtual Int_t CheckVersion(Int_t version);
	virtual Bool_t ExistVersion(Int_t version){
		if (version!=-1) return (version==CheckVersion(version));
		else return (version!=-1);
	}
	virtual void NewGridDialog(const TGWindow * p, const TGWindow * main,
                              UInt_t w = 1, UInt_t h = 1, Option_t * type =
                              "New");

   ClassDef(KVIDGChIoSi, 5)     //ChIo-Si dE-E Z-identification grid
};

#endif
