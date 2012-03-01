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

#include "TMethod.h"
#include "TContextMenu.h"
#include "TGMsgBox.h"
#include "TGFileDialog.h"

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

   protected:
   
   Bool_t fDebug;
   
   KVIDGridEditorCanvas* fCanvas;
   TGraph*               fPivot;
   
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
   
   Bool_t ownhisto;
   Bool_t dlmode;
   Bool_t drawmode;
   Bool_t selectmode;
   Bool_t ready;
   
   Double_t xmin,xmax,ymin,ymax;
   Double_t oldx, oldy;
   Bool_t   moved;
   
   
   protected:
   
   void SetDefault();
   void init();
   void AddTransformation (TString label);
   void AddAction         (TString label);
   void AddGridOption     (TString label, KVHashList* thelist);
   void ConstructModulator();
   
   void TranslateX(Int_t Sign);
   void TranslateY(Int_t Sign);
   void RotateZ   (Int_t Sign);
   void ScaleX    (Int_t Sign);
   void ScaleY    (Int_t Sign);
   void ScaleXY   (Int_t Sign);
   
   void Unzoom      ();
   void SetLogz     ();
   void SetLogy     ();
   void SetLogx     ();
   void ZoomSelected();
   void SetPivot    (Double_t xx0, Double_t yy0);
   
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
   void DrawAtt    (Bool_t piv);
   void ResetColor(KVIDentifier* Ident);
   void ResetColor(KVList* IdentList);
   
   void SuggestMoreAction();
   void ChooseSelectedColor();
   void OpenRootFile();
   void SaveCurrentGrid();
   
   void SetVarXVarY(char* VarX, char* VarY); 
   void SetRunList(char* RunList);  
   void SetParameter(char* Name, char* Value); 
   
   void DispatchOrder(TPaveLabel* label);
   void ChangeStep(const char* title, Int_t dstep=1);
   const char* WhoIsSelected();
   
   TString ListOfHistogramInMemory();
   TString PreselectHistogram(TString ListOfName, Int_t ipert=0);
   
   
   public:
   
   KVIDGridEditor();
   KVIDGridEditor (const KVIDGridEditor&) ;
   virtual ~KVIDGridEditor();
   void Copy (TObject&) const;   
   
   void StartViewer();
   void Clear(const Option_t* opt);
   void Close();
   Bool_t IsClosed();
   
   void SetHisto(TH2* hh);
   void SetGrid(KVIDZAGrid* gg, Bool_t histo=true);
   void SetGrid(TString GridName);
   
   void SelectLabel();
   void MakeTransformation();
   
   void SetSelectedColor(Int_t color){SelectedColor=color;};
   void SetDebug(Bool_t debug){fDebug=debug;};
   

   ClassDef(KVIDGridEditor,1)// outil de modification de grille.
};

//................  global variable
R__EXTERN KVIDGridEditor *gIDGridEditor;

#endif
