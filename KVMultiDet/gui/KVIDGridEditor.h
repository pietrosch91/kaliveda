//Created by KVClassFactory on Fri Feb 17 11:05:30 2012
//Author: dgruyer

#ifndef __KVIDGridEditor_H
#define __KVIDGridEditor_H

#include "TCanvas.h"
#include "TPaveLabel.h"
#include "TStyle.h"
#include "TH2F.h"
#include "TF1.h"
#include "TF2.h"
#include "TROOT.h"
#include "TView.h"
#include "TKey.h"

#include "KVIDGridEditorCanvas.h"
#include "KVBase.h"
#include "KVHashList.h"
#include "KVIDZAGrid.h"
#include "KVIDGridManager.h"
#include "KVIDZALine.h"
#include "KVIDentifier.h"
#include "KVDropDownDialog.h"
#include "KVTestIDGridDialog.h"
#include "KVVirtualIDFitter.h"
#include "TMethod.h"
#include "TContextMenu.h"
#include "TGMsgBox.h"
#include "KVInputDialog.h"

class KVIDGridEditor : public KVBase
{

   private:
   
   Bool_t debug;
   
   KVIDGridEditorCanvas* c1;
   TPad*     mPad;
   TGraph*   pivot;
   
   KVHashList* lplabel;		//contient la liste des TPaveLabel pour les transformations (rouge)
   KVHashList* lplabel2;	//contient la liste des TPaveLabel pour les actions (rouge)
   KVHashList* lplabel3;	//contient la liste des TPaveLabel pour les selections de lignes (vert)
   KVHashList* lplabel4;	//contient la liste des TPaveLabel pour editer la grille (rouge)
   KVHashList* lplabel5;	//contient la liste des TPaveLabel pour moduler les pas (vert)
   
   TPaveLabel* modulator;
   
   TH2*        TheHisto;
   TH2*        TheHistoChoice;
   KVIDZAGrid* TheGrid;
   KVList*     ListOfLines;
   
   Int_t SelectedColor;
   
   Double_t x0;
   Double_t y0;
   Int_t itrans,iact,iopt, imod;
   
   TF1* ft;
   TF1* fs;
   TF1* fsy;
   TF2* frx;
   TF2* fry;
   
   bool ownhisto;
   bool dlmode;
   bool drawmode;
   bool selectmode;
   bool ready;
   
   Double_t xmin,xmax,ymin,ymax;
   Double_t oldx, oldy;
   Bool_t   moved;
   
   public:
   
   KVIDGridEditor();
   KVIDGridEditor (const KVIDGridEditor&) ;
   virtual ~KVIDGridEditor();
   void Copy (TObject&) const;   
   
   void StartViewer();
   void Clear(const Option_t* opt);
   void Close();
   
   void SetHisto(TH2* hh);
   void SetGrid(KVIDZAGrid* gg);
   void SetGrid(TString GridName);
   
   void SelectLabel();
   void MakeTransformation();
   
   void SetSelectedColor(Int_t color){SelectedColor=color;};
   void SetDebug(Bool_t debug_){debug=debug_;};
   
   protected:
   
   void SetDefault();
   void init();
   void AddTransformation (TString label);
   void AddAction         (TString label);
   void AddGridOption     (TString label, KVHashList* thelist);
   void ConstructModulator();
   bool IsClosed();
   
   void TranslateX(int Sign);
   void TranslateY(int Sign);
   void RotateZ   (int Sign);
   void ScaleX    (int Sign);
   void ScaleY    (int Sign);
   void ScaleXY   (int Sign);
   
   void Unzoom      ();
   void SetLogz     ();
   void SetLogy     ();
   void SetLogx     ();
   void ZoomSelected();
   void SetPivot    (double xx0, double yy0);
   
   void SetEditable(TPaveLabel* label);
   void SelectLines(TPaveLabel* label);
   
   void NewLine ();
   void NewCut  ();
   void FitGrid ();
   void TestGrid();
   void SpiderIdentification();
   
   void DeleteLine(KVIDentifier* line);
   void DeleteCut (KVIDentifier* cut);

   void UpdateViewer();
   void ForceUpdate();
   void DrawAtt    (bool piv);
   void ResetColor(KVIDentifier* Ident);
   void ResetColor(KVList* IdentList);
   void ChooseSelectedColor();
   
   void SuggestMoreAction();
   void DispatchOrder(TPaveLabel* label);
   void ChangeStep(const char* title, int dstep=1);
   const char* WhoIsSelected();
   
   TString ListOfHistogramInMemory();
   TString PreselectHistogram(TString ListOfName, Int_t ipert=0);

   ClassDef(KVIDGridEditor,1)// outil de modification de grille.
};

#endif
