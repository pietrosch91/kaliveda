//Created by KVClassFactory on Fri Feb 17 11:05:30 2012
//Author: dgruyer

#include "KVIDGridEditor.h"
#include <iostream>
#include <sstream>

ClassImp(KVIDGridEditor)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDGridEditor</h2>
<h4>Editeur de grille d'identification</h4>
note a moi même : écrire une doc...
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDGridEditor *gIDGridEditor = 0x0;

KVIDGridEditor::KVIDGridEditor()
{
   // Default constructor
   
  gIDGridEditor = this;
     
  SetName("gIDGridEditor");
  SetDefault();
  
  ft  = new TF1("tranlation","(x+[0])",0,4096);
  fs  = new TF1("scale","(x-[0])*[1]+[0]",0,4096);
  fsy = new TF1("scale_y","(x-[0])*[1]+[0]",0,4096);
  
  frx = new TF2("rotation_x","(x-[0])*TMath::Cos([2])-(y-[1])*TMath::Sin([2])+[0]",0,4096);
  fry = new TF2("rotation_y","(x-[0])*TMath::Sin([2])+(y-[1])*TMath::Cos([2])+[1]",0,4096);
  
  ListOfLines = new KVList;
  ListOfLines->SetOwner(kFALSE);
  
  lplabel = new KVHashList(); 
  lplabel->SetOwner(kTRUE); 
  lplabel2 = new KVHashList(); 
  lplabel2->SetOwner(kTRUE);
  lplabel3 = new KVHashList(); 
  lplabel3->SetOwner(kTRUE);
  lplabel4 = new KVHashList(); 
  lplabel4->SetOwner(kTRUE);
  lplabel5 = new KVHashList(); 
  lplabel5->SetOwner(kTRUE);
  
  return;
}

//________________________________________________________________
void KVIDGridEditor::StartViewer()
{    
  Close();
  
  fCanvas = new KVIDGridEditorCanvas(Form("%sCanvas",GetName()),Form("%sCanvas",GetName()),800,600);
  fCanvas->AddExec("transform","gIDGridEditor->MakeTransformation()");
  fCanvas->AddExec("recommence","gIDGridEditor->SelectLabel()");

  fCanvas->cd();
  
  if(!ready) init();
  if(TheHisto) TheHisto->Draw("col");
  if(TheGrid)  TheGrid->Draw();
  
  DrawAtt(false);  
  return;
}

//________________________________________________________________
void KVIDGridEditor::SetDefault()
{
  TheHisto = 0;
  TheGrid  = 0;
  fPivot  = 0;
  x0 = y0 = 0.;
  fCanvas = 0;
  
  itrans = iact = iopt = 0;
  imod = 20;
  
  ownhisto   = false;
  dlmode     = false;
  drawmode   = false;
  selectmode = false;
  moved      = false;
  fDebug     = false;
  ready      = false;

  SelectedColor = kOrange+1;
}

//________________________________________________________________
Bool_t KVIDGridEditor::IsClosed()
{
  if(gROOT->FindObject(Form("%sCanvas",GetName()))) return false;
  else return true;
}

//________________________________________________________________
void KVIDGridEditor::Close()
{
  if(!IsClosed())
    {
    fCanvas->Close();
    delete fCanvas;
    fCanvas = 0;
    }
  return;
}

//________________________________________________________________
void KVIDGridEditor::Clear(const Option_t* opt)
{  
  TString option(opt);
  option.ToUpper();
  
  dlmode     = false;
  drawmode   = false;
  selectmode = false;
  moved      = false;
  
  SelectedColor = kOrange+1;
  
  ResetColor(ListOfLines);
  ListOfLines->Clear();

  if(option.Contains("AL"))
    {
    if((TheHisto)) 
      {
      if(ownhisto)
        {
	TheHisto->Delete();
        delete TheHisto;
        ownhisto = false;
	}
      TheHisto->Delete();
      TheHisto = 0;
      }
    if(TheGrid)
      {
      if(!IsClosed()) TheGrid->UnDraw();
      TheGrid = 0;
      }
    gPad->Clear();
    }
    
  SetPivot(0.,0.);
  
  lplabel->Execute ("SetTextSize","0.625");
  lplabel->Execute ("SetFillColor","kWhite");
  lplabel2->Execute("SetTextSize","0.625");
  lplabel2->Execute("SetFillColor","kWhite");
  lplabel3->Execute("SetTextSize","0.6");
  lplabel3->Execute("SetFillColor","kWhite");
  lplabel4->Execute("SetTextSize","0.6");
  lplabel4->Execute("SetFillColor","kWhite");
  
  lplabel->Execute ("SetBorderSize","1");
  lplabel2->Execute("SetBorderSize","1");
  lplabel3->Execute("SetBorderSize","1");
  lplabel4->Execute("SetBorderSize","1");
  lplabel5->Execute("SetBorderSize","1");
  
  DrawAtt(true);
  UpdateViewer();
}

//________________________________________________________________
void KVIDGridEditor::UpdateViewer()
{
  if(!IsClosed())
    {
    gPad->Modified();
    gPad->Update();
    }
}

//________________________________________________________________
KVIDGridEditor::KVIDGridEditor (const KVIDGridEditor& obj)  : KVBase()
{
  // Copy constructor
  // This ctor is used to make a copy of an existing object (for example
  // when a method returns an object), and it is always a good idea to
  // implement it.
  // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

  obj.Copy(*this);
}

//________________________________________________________________
KVIDGridEditor::~KVIDGridEditor()
{
  // Destructor
  
  if(ownhisto) delete TheHisto;
  if(fCanvas) delete fCanvas;
  if(lplabel)
    {
    lplabel->Delete("all");
    delete lplabel;
    }
  if(lplabel2)
    {
    lplabel2->Delete("all");
    delete lplabel2;
    }
  if(lplabel3)
    {
    lplabel3->Delete("all");
    delete lplabel3;
    }
  if(lplabel4)
    {
    lplabel4->Delete("all");
    delete lplabel4;
    }
  if(lplabel5)
    {
    lplabel5->Delete("all");
    delete lplabel5;
    }
  if(gIDGridEditor==this) gIDGridEditor=0x0;
  
}

//________________________________________________________________
void KVIDGridEditor::Copy (TObject& obj) const
{
  // This method copies the current state of 'this' object Int_to 'obj'
  // You should add here any member variables, for example:
  //	(supposing a member variable KVIDGridEditor::fToto)
  //	CastedObj.fToto = fToto;
  // or
  //	CastedObj.SetToto( GetToto() );

  KVBase::Copy(obj);
  //KVIDGridEditor& CastedObj = (KVIDGridEditor&)obj;
}

//________________________________________________________________
void KVIDGridEditor::init()
{  
  AddTransformation("T_{X}");
  AddTransformation("T_{Y}");
  AddTransformation("R_{Z}");
  AddTransformation("S_{X}");
  AddTransformation("S_{Y}");
  AddTransformation("S_{XY}");
  
  AddAction("0");
  AddAction("#odot");
  AddAction("#Leftarrow");
  AddAction("Lz");
  AddAction("Ly");
  AddAction("Lx");
  
  AddGridOption("All",lplabel3);
  AddGridOption("Select",lplabel3);
  
  AddGridOption("Edit",lplabel4);
  AddGridOption("Delete",lplabel4);
  AddGridOption("Line",lplabel4);
  AddGridOption("Cut",lplabel4);
  AddGridOption("Fit",lplabel4);
  AddGridOption("Test",lplabel4);
  AddGridOption("More",lplabel4);
    
  ConstructModulator();
      
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  fCanvas->Clear();
      
  SetPivot(0.,0.);
  Clear("ALL");
  ready = true;
  
  return;
}

//________________________________________________________________
void KVIDGridEditor::ConstructModulator()
{
  Int_t i = 0;
  
  Double_t x1 = 0.92;
  Double_t x2 = 0.99;
  
  Double_t y1 = 0.90-i*0.06;
  Double_t y2 = y1-0.05;
  
  TPaveLabel* templabel = 0;
  
  templabel = new TPaveLabel(x1,y1,x2,y2, "+","NDC");
  templabel->SetName(templabel->GetTitle());
  templabel->SetFillColor(kWhite);
  lplabel5->Add(templabel);
  
  i++;
  y1 = 0.90-i*0.06;
  y2 = y1-0.05;
  
  modulator = new TPaveLabel(x1,y1,x2,y2, Form("%d",imod),"NDC");
  modulator->SetName(templabel->GetTitle());
  modulator->SetFillColor(kGreen);
  lplabel5->Add(modulator);
  
  i++;
  y1 = 0.90-i*0.06;
  y2 = y1-0.05;
  
  templabel = new TPaveLabel(x1,y1,x2,y2, "-","NDC");
  templabel->SetName(templabel->GetTitle());
  templabel->SetFillColor(kWhite);
  lplabel5->Add(templabel);
  
  lplabel5->Execute("SetTextSize","0.625");  
}

//________________________________________________________________
void KVIDGridEditor::AddTransformation(TString label)
{
  Double_t y1 = 0.92;
  Double_t y2 = 0.99;
  
  Double_t x1 = 0.10+itrans*0.06;
  Double_t x2 = x1+0.05;
  
  TPaveLabel* templabel = 0;
  
  templabel = new TPaveLabel(x1,y1,x2,y2, label.Data(),"NDC");
  templabel->SetName(templabel->GetTitle());
  lplabel->Add(templabel);
  
  itrans++;
  return;
}

//________________________________________________________________
void KVIDGridEditor::AddAction(TString label)
{
  Double_t y1 = 0.92;
  Double_t y2 = 0.99;
  
  Double_t x2 = 0.90-iact*0.06;
  Double_t x1 = x2-0.05;
  
  TPaveLabel* templabel = 0;
  
  templabel = new TPaveLabel(x1,y1,x2,y2, label.Data(),"NDC");
  templabel->SetName(templabel->GetTitle());
  lplabel2->Add(templabel);
  
  iact++;
  return;
}

//________________________________________________________________
void KVIDGridEditor::AddGridOption(TString label, KVHashList* thelist)
{
  Double_t x1 = 0.92;
  Double_t x2 = 0.99;
  
  Double_t y1 = 0.10+iopt*0.06;
  Double_t y2 = y1+0.05;
  
  TPaveLabel* templabel = 0;
  
  templabel = new TPaveLabel(x1,y1,x2,y2, label.Data(),"NDC");
  templabel->SetName(templabel->GetTitle());
  thelist->Add(templabel);
  
  iopt++;
  return;
}

//________________________________________________________________
TString KVIDGridEditor::ListOfHistogramInMemory()
{
  if(!gFile) return "";

  TString HistosNames = "";

  TList* KeyList = gFile->GetListOfKeys();

  TKey* key = 0;
  TIter nextkey(KeyList);
  while((key=(TKey*)nextkey()))
    {
    TString classname = key->GetClassName();
    if(classname.Contains("TH2"))
      {
      HistosNames += Form(" %s", key->GetName());
      }
    else if(classname.Contains("List"))
      {
      TList* sublist = (TList*)gFile->Get(key->GetName());
      if(sublist->IsEmpty()) continue;
      TObject* subobj = 0;
      TIter nextobj(sublist);
      while((subobj=(TObject*)nextobj()))
	{
	if(subobj->InheritsFrom("TH2"))
          {
          HistosNames += Form(" %s", subobj->GetName());
          }
        }
      }
    }
  return HistosNames;
}

//________________________________________________________________
TString KVIDGridEditor::PreselectHistogram(TString ListOfName, Int_t ipert)
{
  ipert = 2;
  if(!TheGrid) return "";
  TString result = "";
  TString Iter;
  
  KVString str(ListOfName.Data());
  str.Begin(" ");
  while(!str.End())
    {
    Iter = str.Next();
    if(Iter.Contains(TheGrid->GetName())) result = Iter.Data();
    }

  return result;
}

//________________________________________________________________
void KVIDGridEditor::SetHisto(TH2* hh)
{
  if(!hh)
    {
    TString Listo   = ListOfHistogramInMemory();
    TString Select  = PreselectHistogram(Listo);
        
    TString Choices;
    TString Default;
    Choices = "Dummy ";
    if(TheHisto)
      {
      Default = "Current";
      Choices += "Current ";
      }
    else Default = "Dummy";
    
    if(!strcmp(Listo.Data(),"")) 
      {
      }
    else if(!strcmp(Select.Data(),"")) 
      {
      Choices += Listo;
      }
    else
      {
      Default = Select;
      Choices += Select.Data();
      Choices += " ";
      Choices += Listo.ReplaceAll(Select.Data(),"");
      }
    
    TString Answer;
    Bool_t okpressed;

    if (Choices.Contains(" ")) 
      {
      new KVDropDownDialog(gClient->GetDefaultRoot(), "Choose an histogram :", Choices.Data(), Default.Data(), &Answer, &okpressed);
      if (!okpressed) 
        {
	Answer = "Current";
        return;
        }
      } 
    else Answer = Default;
    
    if(!Answer.Contains("Current")&&ownhisto)
      {
      delete TheHisto;
      TheHisto = 0;
      ownhisto = false;
      }
      
    if((!Answer.Contains("Current"))&&(!Answer.Contains("Dummy")))
      {
      TheHistoChoice = 0;
      if((TheHistoChoice=(TH2*)gFile->Get(Answer.Data()))) TheHisto = TheHistoChoice;
      else Answer = "Dummy";
      }
         
    if(Answer.Contains("Dummy"))
      {
      TString hname = Form("%sDefaultHistogram",GetName());
      Double_t Xmax = 4096.;
      Double_t Ymax = 4096;
      if(TheGrid)
        {
	         TheGrid->Initialize();
            // pour pouvoir utiliser un pointeur KVIDGraph* au lieu de KVIDZAGrid*
// 	         Xmax = TheGrid->GetZmaxLine()->GetXaxis()->GetXmax();
//             Ymax = TheGrid->GetZmaxLine()->GetYaxis()->GetXmax();
            TheGrid->FindAxisLimits();
            Xmax = TheGrid->GetXmax();
            Ymax = TheGrid->GetYmax();
	      }
      TH2* TmpH = 0;
      if((TmpH=(TH2*)gROOT->FindObject(hname.Data()))) delete TmpH;
      TheHisto = new TH2F(hname.Data(),hname.Data(),2048,0,Xmax,2048,0,Ymax);
      ownhisto = true;
      }
    }
  else if(!hh->InheritsFrom("TH2"))
    {
    cout << "ERROR: KVIDGridEditor::SetHisto(): '" << hh->GetName() << "' must be a 2D histogram !" << endl;
    return;
    }
  else 
    {
    if((ownhisto)&&(TheHisto)) 
      {
      delete TheHisto;
      TheHisto = 0;
      }
    TheHisto = hh; 
    ownhisto = false;
    }
  
  if(!IsClosed()&&(TheHisto))
    {
    TheHisto->Draw("col");
    gPad->SetLogz(true);
    }
  DrawAtt(true);
  return;
}

//________________________________________________________________
void KVIDGridEditor::DrawAtt(Bool_t piv)
{
  if((!ready)||IsClosed()) return;

  lplabel->Execute("Draw","");
  lplabel2->Execute("Draw","");
  lplabel3->Execute("Draw","");
  lplabel4->Execute("Draw","");
  lplabel5->Execute("Draw","");
  if(!piv) SetPivot(0.,0.);
  else fPivot->Draw("P");
  
  UpdateViewer();
  return;
}

//________________________________________________________________
void KVIDGridEditor::SetGrid(KVIDGraph* gg, Bool_t histo)
{
  if(!gg)
    {
    cout << "ERROR: KVIDGridEditor::SetHisto(): invalid poInt_ter on the grid !" << endl;
    return;
    }
  if((TheGrid)&&(!IsClosed())) TheGrid->UnDraw();
  
  TheGrid = gg;
  if(histo) SetHisto(0);
  if(!IsClosed()) TheGrid->Draw();
  
  DrawAtt(true);
  UpdateViewer();
  return;
}

//________________________________________________________________
void KVIDGridEditor::SetGrid(TString GridName)
{
  Bool_t sethisto = true;

  if(!strcmp(GridName.Data(),""))
    {
    TString Answer;
    Bool_t proposename = false;
    if(TheGrid) Answer = TheGrid->GetName();
    else if(TheHisto) 
      {
      Answer = TheHisto->GetName();
      proposename = true;
      }
    Bool_t okpressed;
    new KVInputDialog(gClient->GetDefaultRoot(),"Enter the name of your grid :",&Answer,&okpressed);
    if(!okpressed) return;
    GridName = Answer.Data();
    if(proposename&&(!strcmp(TheHisto->GetName(),Answer.Data()))) sethisto = false;
    }

  KVIDGraph* tempgrid = 0;
  if(!gIDGridManager) return;
  if(!(tempgrid = (KVIDGraph*)gIDGridManager->GetGrids()->FindObject(GridName.Data()))) 
    {
    cout << "WARNING: KVIDGridEditor::SetGrid(): Unknown grid named '" << GridName.Data() << "' !" << endl;
    return;
    }
  else SetGrid(tempgrid,sethisto);
  return;
}

//________________________________________________________________
void KVIDGridEditor::SetPivot(Double_t xx0, Double_t yy0)
{
  if(!fPivot) 
    {
    fPivot = new TGraph;
    fPivot->SetMarkerStyle(2);
    fPivot->SetMarkerSize(2);
    fPivot->SetMarkerColor(kRed);
    fPivot->SetName("ThePivot");
    }
  else fPivot->SetPoint(0,xx0,yy0);
}

//________________________________________________________________
void KVIDGridEditor::SelectLabel()
{
  Int_t event = gPad->GetEvent();
  if(event==kMouseMotion) return;  
  TObject *select = gPad->GetSelected();
  
  
  if(!select) return;  
  if(!select->InheritsFrom("TPaveLabel")) return;  
  
  TPaveLabel* label = (TPaveLabel*)select;  
  if(event==kButton1Down)
    {
    Int_t color = label->GetFillColor();
    if(lplabel->Contains(label))
      {      
      lplabel->Execute("SetFillColor","kWhite");
      if(color==kWhite) label->SetFillColor(kRed);
      else if(color==kRed) label->SetFillColor(kWhite);
      UpdateViewer();
      }
    else if(lplabel2->Contains(label))
      {
      label->SetFillColor(kRed);
      UpdateViewer();
      DispatchOrder(label);
      }
    else if(lplabel4->Contains(label))
      {
      DispatchOrder(label);
      }
    else if(lplabel3->Contains(label))
      {
      lplabel3->Execute("SetFillColor","kWhite");
      if(color==kWhite) label->SetFillColor(kGreen);
      if(color==kGreen)  label->SetFillColor(kWhite);
      SelectLines(label);
      UpdateViewer();
      }
    else if(lplabel5->Contains(label)&&(label!=modulator))
      {
      label->SetFillColor(kGreen);
      ChangeStep(label->GetTitle());
      UpdateViewer();
      }
    }
  else if(event==kButton1Up)
    {
    if(lplabel2->Contains(label))
      {
      label->SetFillColor(kWhite);
      UpdateViewer();
      }    
    else if(lplabel5->Contains(label)&&(label!=modulator))
      {
      label->SetFillColor(kWhite);
      UpdateViewer();
      }    
    }    
  else if(event==kButton1Double)
    {
    if(lplabel5->Contains(label)&&(label!=modulator))
      {
      label->SetFillColor(kGreen);
      ChangeStep(label->GetTitle(),9);
      UpdateViewer();
      }
    }
  else if(event==kButton1Shift)
    {
    if(lplabel5->Contains(label)&&(label!=modulator))
      {
      label->SetFillColor(kGreen);
      ChangeStep(label->GetTitle(),100);
      UpdateViewer();
      }
    }
}

//________________________________________________________________
void KVIDGridEditor::MakeTransformation()
{
  Int_t event = gPad->GetEvent();
  if(event==kMouseMotion) return;
  TObject *select = gPad->GetSelected();
    
  if (!select){}
  else 
    {
    if(select->InheritsFrom("TPaveLabel")) return;
    }
  
  if((event==kButton1Up)&&(dlmode)&&(select))
    {
    if(select->InheritsFrom("KVIDZALine"))
      {
      DeleteLine((KVIDentifier*)select);
      }
    else if(select->InheritsFrom("KVIDCutLine")||select->InheritsFrom("KVIDCutContour"))
      {
      DeleteCut((KVIDentifier*)select);
      }
    }
  if((event==kButton1Up)&&(select)&&(!dlmode))
    {
    if(select->InheritsFrom("KVIDentifier"))
      {
      KVIDentifier* line = (KVIDentifier*)select;
      if(selectmode)
        {
        if(!ListOfLines->Contains(line))
          {
          line->SetLineColor(SelectedColor);
          ListOfLines->Add(line);
	  UpdateViewer();
	  }
        else
          {
          ListOfLines->Remove(line);
	  ResetColor(line);
	  UpdateViewer();
	  }
	}
      else if(ListOfLines->Contains(line))
        {
        ListOfLines->Remove(line);
	ResetColor(line);
	TPaveLabel* tmplabel = (TPaveLabel*)lplabel3->FindObject("All");
	tmplabel->SetFillColor(kWhite);
	tmplabel = (TPaveLabel*)lplabel3->FindObject("Select");
	tmplabel->SetFillColor(kGreen);
	SelectLines(tmplabel);
	UpdateViewer();
	}
      }
    }
  if((event==kButton1Double)&&(!drawmode))
    {
    Int_t xx = gPad->GetEventX();
    Int_t yy = gPad->GetEventY();

    x0 = gPad->AbsPixeltoX(xx);
    y0 = gPad->AbsPixeltoY(yy);
    
    SetPivot(x0,y0);
    fPivot->Draw("P");
    UpdateViewer();
    }
  if((event==kWheelUp)||(event==kWheelDown))
    {
    Int_t sign = (event==kWheelUp ? 1:-1);
    const char* who = WhoIsSelected();
    
    if(!strcmp(who,"")) DynamicZoom(sign,gPad->GetEventX(),gPad->GetEventY());
    else if(!strcmp(who,"T_{X}"))  TranslateX(sign);
    else if(!strcmp(who,"T_{Y}"))  TranslateY(sign);
    else if(!strcmp(who,"R_{Z}"))  RotateZ   (sign);
    else if(!strcmp(who,"S_{X}"))  ScaleX    (sign);
    else if(!strcmp(who,"S_{Y}"))  ScaleY    (sign);
    else if(!strcmp(who,"S_{XY}")) ScaleXY   (sign);
    }
  
  //if(event==kButton2Up) ForceUpdate();
  if((event==kESC)&&(TheHisto)) Unzoom();

  return;
}
   
//________________________________________________________________
void KVIDGridEditor::ZoomSelected()
{
  if(!TheHisto) return;
  TAxis* ax = TheHisto->GetXaxis();
  
  Double_t ratio1 = (xmin - gPad->GetUxmin())/(gPad->GetUxmax() - gPad->GetUxmin());
  Double_t ratio2 = (xmax - gPad->GetUxmin())/(gPad->GetUxmax() - gPad->GetUxmin());
  
  if((ratio2-ratio1 > 0.05))
    {
    ax->SetRangeUser(xmin, xmax);
    }
  
  ax = TheHisto->GetYaxis();
  
  ratio1 = (ymin - gPad->GetUymin())/(gPad->GetUymax() - gPad->GetUymin());
  ratio2 = (ymax - gPad->GetUymin())/(gPad->GetUymax() - gPad->GetUymin());
  
  if((ratio2-ratio1 > 0.05))
    {
    ax->SetRangeUser(ymin, ymax);
    }
  
  xmax = xmin = ymax = ymin = 0.;
  return;
}

//________________________________________________________________
void KVIDGridEditor::DispatchOrder(TPaveLabel* label)
{
  TString commande(label->GetName());
  
  if(commande.Contains("#Leftarrow"))  Undo();
  else if(commande.Contains("0"))      SetPivot(0,0);
  else if(commande.Contains("#odot"))  Unzoom();
  else if(commande.Contains("Lz"))     SetLogz();
  else if(commande.Contains("Ly"))     SetLogy();
  else if(commande.Contains("Lx"))     SetLogx();
  else if(commande.Contains("Cut"))    NewCut();
  else if(commande.Contains("Line"))   NewLine();
  else if(commande.Contains("Edit"))   SetEditable(label);
  else if(commande.Contains("Fit"))
    {
    label->SetFillColor(kRed);
    UpdateViewer();
    FitGrid();
    label->SetFillColor(kWhite);
    UpdateViewer();
    }
  else if(commande.Contains("Test"))
    {
    label->SetFillColor(kRed);
    UpdateViewer();
    TestGrid();
    label->SetFillColor(kWhite);
    UpdateViewer();
    }
  else if(commande.Contains("More"))   
    {
    label->SetFillColor(kRed);
    UpdateViewer();
    SuggestMoreAction();
    label->SetFillColor(kWhite);
    UpdateViewer();
    }
  else if(commande.Contains("Delete"))
    {
    if(!TheGrid) return;
    Int_t color = label->GetFillColor();
    if(color==kRed)
      {
      label->SetFillColor(kWhite);
      dlmode = false;
      UpdateViewer();
      }
    else if(color==kWhite)
      {
      label->SetFillColor(kRed);
      dlmode = true;
      UpdateViewer();
      }
    }
  else cout << "WARNING: KVIDGridEditor::DispatchOrder(): unknown order '" << commande << "' !" << endl;
}

//________________________________________________________________
void KVIDGridEditor::SetEditable(TPaveLabel* label)
{  
  if(TheGrid)
    {
    Bool_t iseditable = TheGrid->IsEditable();
    TheGrid->SetEditable(!iseditable);
    if(iseditable) label->SetFillColor(kWhite);
    else label->SetFillColor(kRed);
    }
  else label->SetFillColor(kWhite);
  UpdateViewer();
  return;
}

//________________________________________________________________
void KVIDGridEditor::SelectLines(TPaveLabel* label)
{  
  if(!TheGrid) return;
  TString title(label->GetTitle());
  Int_t color = label->GetFillColor();
  
  if(title.Contains("All"))
    {
    if(color==kWhite) 
      {
      ResetColor(ListOfLines);
      ListOfLines->Clear();
      }
    else if(color==kGreen) 
      {
      ListOfLines->AddAll(TheGrid->GetIdentifiers());
      ListOfLines->AddAll(TheGrid->GetCuts());
      ListOfLines->R__FOR_EACH(KVIDentifier, SetLineColor) (SelectedColor);
      }
    selectmode = false;
    }
  if(title.Contains("Select"))
    {
    if(color==kWhite) 
      {
      selectmode = false;
      ResetColor(ListOfLines);
      ListOfLines->Clear();
      }
    if(color==kGreen)
      {
      ListOfLines->R__FOR_EACH(KVIDentifier, SetLineColor) (SelectedColor);
      selectmode = true;
      }
    }
  UpdateViewer();
  return;  
}

//________________________________________________________________
void KVIDGridEditor::NewLine()
{  
  if (!TheGrid) return;
  TPaveLabel* label = (TPaveLabel*)lplabel4->FindObject("Line");
  label->SetFillColor(kRed);
  UpdateViewer();
  
  drawmode = true;
  
  TString resname;
  resname.Form("%s.IDClass", TheGrid->ClassName());
  TString cut_choices = gEnv->GetValue(resname.Data(), "");
  resname.Form("%s.DefaultIDClass", TheGrid->ClassName());
  TString cut_default = gEnv->GetValue(resname.Data(), "");
  TString cut_class;
  Bool_t okpressed;
  
  if(cut_choices.Contains(" ")) 
    {
    new KVDropDownDialog(gClient->GetDefaultRoot(),
			 "Choose class of new identifier :",
			 cut_choices.Data(),
			 cut_default.Data(),
			 &cut_class,
			 &okpressed);
    if (!okpressed) return;
    } 
  else cut_class = cut_choices;
 
  TheGrid->DrawAndAdd("ID", cut_class.Data());
    
  label->SetFillColor(kWhite);  
  UpdateViewer();
  
  drawmode = false;
  if(fDebug) cout << "INFO: KVIDGridEditor::NewLine(): New Line has been added to the current grid..." << endl;
  return;   
}

//________________________________________________________________
void KVIDGridEditor::NewCut()
	
{  
  if (!TheGrid) return;
  TPaveLabel* label = (TPaveLabel*)lplabel4->FindObject("Cut");
  label->SetFillColor(kRed);
  UpdateViewer();
  
  drawmode = true;
  
  TString resname;
  resname.Form("%s.CutClass", TheGrid->ClassName());
  TString cut_choices = gEnv->GetValue(resname.Data(), "");
  resname.Form("%s.DefaultCutClass", TheGrid->ClassName());
  TString cut_default = gEnv->GetValue(resname.Data(), "");
  cut_default.ReplaceAll(" ", "");
  cut_default.ReplaceAll("KVIDCut", "");
  TString cut_class;
  KVString cut_types = cut_choices;
  cut_types.ReplaceAll("KVIDCut", "");
  Bool_t okpressed;
  
  if (cut_choices.Contains(" ")) 
    {
    if(!strcmp(cut_default,""))
      {
      cut_types.Begin(" ");
      cut_default = cut_types.Next();
      }
    new KVDropDownDialog(gClient->GetDefaultRoot(),
        		 "Choose class of new cut :",
        		 cut_types.Data(),
        		 cut_default.Data(),
        		 &cut_class,
        		 &okpressed);
    if (!okpressed) 
      {
      label->SetFillColor(kWhite);  
      UpdateViewer();
      drawmode = false;
      return;
      }
    } 
  else cut_class = cut_types;
  
  cut_class.Prepend("KVIDCut");
  TheGrid->DrawAndAdd("CUT", cut_class.Data());
  
  label->SetFillColor(kWhite);  
  UpdateViewer();
  
  drawmode = false;
  if(fDebug) cout << "INFO: KVIDGridEditor::NewCut(): New Cut has been added to the current grid..." << endl;
  return;   
}

//________________________________________________________________
void KVIDGridEditor::SpiderIdentification()
{
  cout << "INFO: KVIDGridEditor::SpiderIdentification(): to be implemented..." << endl;
}

//________________________________________________________________
void KVIDGridEditor::TestGrid()
{  
  if(!TheGrid) return;
  if(!TheHisto) return;

  new KVTestIDGridDialog(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), 10, 10, TheGrid, TheHisto);

  if(fDebug) cout << "INFO: KVIDGridEditor::TestGrid(): testing the current grid..." << endl;
}

//________________________________________________________________
void KVIDGridEditor::FitGrid()
{  
  if(!TheGrid) return;

  KVVirtualIDFitter * fitter = KVVirtualIDFitter::GetDefaultFitter();
  fitter->SetGrid(TheGrid);
  fitter->SetPad(TheGrid->GetPad());
  TMethod* m = fitter->IsA()->GetMethodAny("FitPanel");
  TContextMenu * cm = new TContextMenu("FitPanel", "Context menu for KVVirtualIDFitter::FitPanel");
  cm->Action(fitter,m);
  delete cm;

  if(fDebug) cout << "INFO: KVIDGridEditor::FitGrid(): fitting grid '" << TheGrid->GetName() << "'..." << endl;
}

//________________________________________________________________
void KVIDGridEditor::SuggestMoreAction()
{  
  TString Default = "SaveCurrentGrid";
  TString Choices = Default;
  Choices += " SetVarXVarY";
  Choices += " SetRunList";
  Choices += " AddParameter";
  Choices += " SetSelectedColor";
  Choices += " SpiderIdentification";
  
  TString Answer;
  Bool_t okpressed;
  new KVDropDownDialog(gClient->GetDefaultRoot(), "Choose an action :", Choices.Data(), Default.Data(), &Answer, &okpressed);
  if(!okpressed) 
    {
    return;
    } 
      
  if(!strcmp(Answer.Data(),"")) cout << "INFO: KVIDGridEditor::SuggestMoreAction(): Nothing has been done..." << endl;
  else if(!strcmp(Answer.Data(),"SaveCurrentGrid"))      SaveCurrentGrid();
  else if(!strcmp(Answer.Data(),"SetSelectedColor"))     ChooseSelectedColor();
  else if((!strcmp(Answer.Data(),"SetVarXVarY"))||(!strcmp(Answer.Data(),"SetRunList"))||(!strcmp(Answer.Data(),"AddParameter")))
    {  
    if(!TheGrid) return;
    TMethod* m = TheGrid->IsA()->GetMethodAllAny(Answer.Data());
    TContextMenu * cm = new TContextMenu(Answer.Data(), Form("Context menu for KVIDGridEditor::%s",Answer.Data()));
    cm->Action(TheGrid,m);
    delete cm;
    }
  else if(!strcmp(Answer.Data(),"SpiderIdentification")) SpiderIdentification();
  else cout << "INFO: KVIDGridEditor::SuggestMoreAction(): '" << Answer << "' not implemented..." << endl;

}

//________________________________________________________________
void KVIDGridEditor::SetVarXVarY(char* VarX, char* VarY)
{
  cout << "DEBUG: KVIDGridEditor::SetVarXVarY(): varx and vary will be set..." << endl;
  if(!TheGrid) return;
  TheGrid->SetVarX(VarX);
  TheGrid->SetVarX(VarY);
  cout << "DEBUG: KVIDGridEditor::SetVarXVarY(): varx and vary set..." << endl;
  return;
}

//________________________________________________________________
void KVIDGridEditor::SetRunList(char* RunList)
{
  if(!TheGrid) return;
  TheGrid->SetRunList(RunList);
  return;
}

//________________________________________________________________
void KVIDGridEditor::SetParameter(char* Name, char* Value)
{
  if(!TheGrid) return;
  TheGrid->GetParameters()->SetValue(Name,Value);
  
  return;
}

//________________________________________________________________
void KVIDGridEditor::SaveCurrentGrid()
{
  if(!TheGrid) return;
  TString currentdir(gSystem->ExpandPathName("."));
  
  static TString dir(gSystem->ExpandPathName("."));
  const char *filetypes[] = {
     "ID Grid files", "*.dat",
     "All files", "*",
     0, 0
  };
  TGFileInfo fi;
  fi.fFileTypes = filetypes;
  fi.fIniDir = StrDup(dir);
  new TGFileDialog(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), kFDSave, &fi);
  if (fi.fFilename) 
    {
    //if no ".xxx" ending given, we add ".dat"
    TString filenam(fi.fFilename);
    if(filenam.Contains("toto")) filenam.ReplaceAll("toto",TheGrid->GetName());
    if (!filenam.Contains('.')) filenam += ".dat";
    TheGrid->WriteAsciiFile(filenam.Data());
    }
  dir = fi.fIniDir;
  gSystem->cd(currentdir.Data());
}

//________________________________________________________________
void KVIDGridEditor::OpenRootFile()
{
  static TString dir("$HISTOROOT");
  TString currentdir(gSystem->ExpandPathName("."));
  
  if(gSystem->ExpandPathName(dir)) dir = ".";
  
  const char *filetypes[] = {"Root files", "*.root","All files", "*", 0, 0};
  TGFileInfo fi;
  fi.fFileTypes = filetypes;
  fi.fIniDir = StrDup(dir);
  new TGFileDialog(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), kFDOpen, &fi);
  if (fi.fFilename)
    {
    if(!(TFile::Open(fi.fFilename))) 
      {
      new TGMsgBox(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), "ID Grid Editor", Form("Could not open file %s", fi.fFilename), 0, kMBOk);
      }
    }
  dir = fi.fIniDir;
  gSystem->cd(currentdir.Data());
}

//________________________________________________________________
void KVIDGridEditor::ChooseSelectedColor()
{
  TString Default = "kOrange";
  TString Choices = Default;
  Choices += " ";
  Choices += "kBlack kGreen kBlue kRed kYellow kCyan kMagenta";
  
  TString Answer;
  Bool_t okpressed;
  new KVDropDownDialog(gClient->GetDefaultRoot(), "Choose an action :", Choices.Data(), Default.Data(), &Answer, &okpressed);
  if(!okpressed) return;
  
  if(!strcmp(Answer.Data(),"kOrange")) SetSelectedColor(kOrange+1);
  else if(!strcmp(Answer.Data(),"kGreen")) SetSelectedColor(kGreen);
  else if(!strcmp(Answer.Data(),"kBlack")) SetSelectedColor(kBlack);
  else if(!strcmp(Answer.Data(),"kBlue")) SetSelectedColor(kBlue);
  else if(!strcmp(Answer.Data(),"kRed")) SetSelectedColor(kRed);
  else if(!strcmp(Answer.Data(),"kYellow")) SetSelectedColor(kYellow);
  else if(!strcmp(Answer.Data(),"kCyan")) SetSelectedColor(kCyan);
  else if(!strcmp(Answer.Data(),"kMagenta")) SetSelectedColor(kMagenta);
  
//   TMethod* m = this->IsA()->GetMethodAny("SetSelectedColor");
//   TContextMenu * cm = new TContextMenu("SetSelectedColor", "Context menu for KVIDGridEditor::SetSelectedColor()");
//   cm->Action(this,m);
//   delete cm;
  
  return;
}

//________________________________________________________________
void KVIDGridEditor::DeleteLine(KVIDentifier* line)
{  
  if(!TheGrid) return;
  if(!line) return;
  if(!TheGrid->GetIdentifiers()->Contains(line)) return;
  
  Int_t ret_val;
  new TGMsgBox(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), "ID Grid Editor",
                Form("This will delete the line '%s'. Are you sure ?",line->GetName()),
                kMBIconExclamation, kMBOk | kMBCancel, &ret_val);
		
  if (ret_val & kMBOk) 
    {
    if(ListOfLines->Contains(line)) ListOfLines->Remove(line);
    TheGrid->RemoveIdentifier(line);
    }
  return;
}

//________________________________________________________________
void KVIDGridEditor::DeleteCut(KVIDentifier* cut)
{  
  if(!TheGrid) return;
  if(!TheGrid->GetCuts()->Contains(cut)) return;
  
  Int_t ret_val;
  new TGMsgBox(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), "ID Grid Editor",
                Form("This will delete the cut '%s'. Are you sure ?",cut->GetName()),
                kMBIconExclamation, kMBOk | kMBCancel, &ret_val);
		
  if (ret_val & kMBOk) 
    {
    if(ListOfLines->Contains(cut)) ListOfLines->Remove(cut);
    TheGrid->RemoveCut(cut);
    }
  return;
}

//________________________________________________________________
void KVIDGridEditor::ChangeStep(const char* title, Int_t dstep)
{
  TString commande(title);
  if(commande.Contains("+")) 
    {
    imod+=dstep;
    }
  else if(commande.Contains("-"))
    {
    imod-=dstep;
    if(imod<=0)imod=1;
    }
  modulator->SetLabel(Form("%d",imod));
  UpdateViewer();
  return;
}

//________________________________________________________________
void KVIDGridEditor::SetLogz()
{
  if(fCanvas->IsLogz()) gPad->SetLogz(0);
  else gPad->SetLogz(1);
}

//________________________________________________________________
void KVIDGridEditor::SetLogy()
{
  if(fCanvas->IsLogy()) gPad->SetLogy(0);
  else gPad->SetLogy(1);
}

//________________________________________________________________
void KVIDGridEditor::SetLogx()
{
  if(fCanvas->IsLogx()) gPad->SetLogx(0);
  else gPad->SetLogx(1);
}

//________________________________________________________________
void KVIDGridEditor::Unzoom()
{
  if(TheHisto)
    {
    TheHisto->GetXaxis()->UnZoom();
    TheHisto->GetYaxis()->UnZoom();
    }
}

//________________________________________________________________
void KVIDGridEditor::Undo()
{
   // Revert current grid to its last saved version
   // Normally this will be the state of the grid before starting
   // the current editing session
   
   if(!TheGrid) return;
   
   TString username = gSystem->GetUserInfo()->fRealName;
   username.ReplaceAll(",","");
   Int_t ret_val;
   new TGMsgBox(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(), "ID Grid Editor",
                Form("This will undo all changes to the grid. Are you sure, %s?", username.Data()),
                kMBIconExclamation, kMBOk | kMBCancel, &ret_val);
   
   if (ret_val & kMBOk) {
      if(ListOfLines && ListOfLines->GetEntries()){
         // unselect any previously selected lines
//          TPaveLabel* selectLabel=0;
// 	      TPaveLabel* tmplabel = (TPaveLabel*)lplabel3->FindObject("All");
// 	      if(tmplabel->GetFillColor()==kGreen) selectLabel=tmplabel;
//          if(selectLabel) SelectLines(selectLabel);
// 	      tmplabel = (TPaveLabel*)lplabel3->FindObject("Select");
// 	      if(tmplabel->GetFillColor()==kGreen) selectLabel=tmplabel;
//          if(selectLabel) SelectLines(selectLabel);
         Clear();
      }
      TheGrid->RevertToLastSavedVersion();
   } 
	TPaveLabel* tmplabel = (TPaveLabel*)lplabel2->FindObject("#Leftarrow");
   tmplabel->SetFillColor(kWhite);
   UpdateViewer();
}

//________________________________________________________________
const char* KVIDGridEditor::WhoIsSelected()
{
  TPaveLabel* label = 0;
  TIter nextlabel(lplabel);
  while((label = (TPaveLabel*)nextlabel()))
    {
    if(label->GetFillColor()==kRed) return label->GetName();
    }
  return "";
}

//________________________________________________________________
void KVIDGridEditor::TranslateX(Int_t Sign)
{
  if(!TheGrid) return;
  if(!ListOfLines) return;
  if(ListOfLines->IsEmpty()) return;

  Double_t step   = TheHisto->GetXaxis()->GetBinWidth(1)*(imod)*0.1;
  Double_t factor = Sign*step;
  
  ft->SetParameter(0,factor);
  ListOfLines->R__FOR_EACH(KVIDentifier, Scale) (ft,0);
    
  UpdateViewer();  	     
  if(fDebug) cout << "INFO: KVIDGridEditor::TranslateX(): translation on the X axis (" << (Sign>0 ? "+":"-") << step << ") !" << endl;
  return;
}

//________________________________________________________________
void KVIDGridEditor::TranslateY(Int_t Sign)
{
  if(!TheGrid) return;
  if(!ListOfLines) return;
  if(ListOfLines->IsEmpty()) return;

  Double_t step   = TheHisto->GetXaxis()->GetBinWidth(1)*(imod)*0.1;
  Double_t factor = Sign*step;
  
  ft->SetParameter(0,factor);
  ListOfLines->R__FOR_EACH(KVIDentifier, Scale) (0,ft);
    
  UpdateViewer();  	     
  if(fDebug) cout << "INFO: KVIDGridEditor::TranslateY(): translation on the Y axis (" << (Sign>0 ? "+":"-") << step << ") !" << endl;
  return;
}

//________________________________________________________________
void KVIDGridEditor::DynamicZoom(Int_t Sign, Int_t px, Int_t py)
{
   // Zoom in or out of histogram with mouse wheel
   
  if(!TheHisto) return;

  // use modulator value as the percentage of the number of bins
  // currently displayed on each axis 
  Double_t percent = TMath::Min(imod/100.,1.0);
  percent = 0.15-Sign*0.05;
  
  Int_t dX = 0;
  Int_t dY = 0;
  
  px = gPad->AbsPixeltoX(px);
  py = gPad->AbsPixeltoY(py);
  
  TAxis* ax = TheHisto->GetXaxis();
  Int_t NbinsX = ax->GetNbins();
  Int_t X0 = ax->GetFirst();
  Int_t X1 = ax->GetLast();
  Int_t step = TMath::Min(TMath::Max(1, (Int_t)(percent*(X1-X0))),NbinsX/2);
  step*=Sign;
  X0 = TMath::Min(TMath::Max(X0+step,1),X1-step);
  X1 = TMath::Max(TMath::Min(X1-step,NbinsX),X0);
  if(X0>=X1) X0=X1-1;
  if(Sign>0)dX = (Int_t) (X0 + (X1-X0)*0.5 - ax->FindBin(px));
   ax->SetRange(X0-dX,X1-dX);
  
  ax = TheHisto->GetYaxis();
  Int_t NbinsY = ax->GetNbins();
  Int_t Y0 = ax->GetFirst();
  Int_t Y1 = ax->GetLast();  
  step = TMath::Min(TMath::Max(1, (Int_t)(percent*(Y1-Y0))),NbinsY/2);
  step*=Sign;
  Y0 = TMath::Min(TMath::Max(Y0+step,1),Y1-step);
  Y1 = TMath::Max(TMath::Min(Y1-step,NbinsY),Y0);
  if(Y0>=Y1) Y0=Y1-1;
  if(Sign>0) dY = (Int_t) (Y0 + (Y1-Y0)*0.5 - ax->FindBin(py));
  ax->SetRange(Y0-dY,Y1-dY);
  
  UpdateViewer();  	     
  return;
}

//________________________________________________________________
void KVIDGridEditor::RotateZ(Int_t Sign)
{  
  if(!TheGrid) return;
  if(!ListOfLines) return;
  if(ListOfLines->IsEmpty()) return;
  
  Double_t step  = 1.*(imod/100.);
  if(step>=45.) step = 45.;
  Double_t theta = Sign*step*TMath::DegToRad();
  
  x0 = fPivot->GetX()[0];
  y0 = fPivot->GetY()[0];
  
  frx->SetParameters(x0,y0,theta);
  fry->SetParameters(x0,y0,theta);

  ListOfLines->R__FOR_EACH(KVIDentifier, Scale) (frx,fry);
    
  UpdateViewer();
  if(fDebug) cout << "INFO: KVIDGridEditor::RotateZ(): rotation around the Z axis (" << (Sign>0 ? "+":"-") << step << ") !" << endl;
  return;
}

//________________________________________________________________
void KVIDGridEditor::ScaleX(Int_t Sign)
{
  if(!TheGrid) return;
  if(!ListOfLines) return;
  if(ListOfLines->IsEmpty()) return;

  Double_t step   = 0.01*(imod/100.);
  Double_t factor = 1.+Sign*step;
  
  x0 = fPivot->GetX()[0];
  
  fs->SetParameters(x0,factor);
  ListOfLines->R__FOR_EACH(KVIDentifier, Scale) (fs,0);
  
  UpdateViewer();
  if(fDebug) cout << "INFO: KVIDGridEditor::ScaleX(): scaling on the X axis (*" << factor << ") !" << endl;
  return;
}

//________________________________________________________________
void KVIDGridEditor::ScaleY(Int_t Sign)
{
  if(!TheGrid) return;
  if(!ListOfLines) return;
  if(ListOfLines->IsEmpty()) return;

  Double_t step   = 0.01*(imod/100.);
  Double_t factor = 1.+Sign*step;
  
  y0 = fPivot->GetY()[0];
  
  fs->SetParameters(y0,factor);
  ListOfLines->R__FOR_EACH(KVIDentifier, Scale) (0,fs);
    
  UpdateViewer();
  if(fDebug) cout << "INFO: KVIDGridEditor::ScaleY(): scaling on the Y axis (*" << factor << ") !" << endl;
  return;
}

//________________________________________________________________
void KVIDGridEditor::ScaleXY(Int_t Sign)
{
  if(!TheGrid) return;
  if(!ListOfLines) return;
  if(ListOfLines->IsEmpty()) return;

  Double_t step   = 0.01*(imod/100.);
  Double_t factor = 1.+Sign*step;
  
  x0 = fPivot->GetX()[0];
  y0 = fPivot->GetY()[0];
  
  fs->SetParameters(x0,factor);
  fsy->SetParameters(y0,factor);
  
  ListOfLines->R__FOR_EACH(KVIDentifier, Scale) (fs,fsy);
    
  UpdateViewer();
  if(fDebug) cout << "INFO: KVIDGridEditor::ScaleXY(): scaling (*" << factor << ") !" << endl;
  return;
}

//________________________________________________________________
void KVIDGridEditor::ResetColor(KVIDentifier* Ident)
{
  if(!TheGrid) return;
  if(!(TheGrid->GetCuts()->Contains(Ident))) Ident->SetLineColor(kBlack);
  else Ident->SetLineColor(kRed);
  return;
}

//________________________________________________________________
void KVIDGridEditor::ResetColor(KVList* IdentList)
{
  KVIDentifier* idd = 0;
  TIter nextidd(IdentList);
  while((idd=(KVIDentifier*)nextidd()))
    {
    ResetColor(idd);
    }
}

//________________________________________________________________
void KVIDGridEditor::ForceUpdate()
{
  if(IsClosed()) return;
  fCanvas->cd();
  fCanvas->Clear();
  if(TheHisto)
    {
    TheHisto->Draw("col");
    }
  if(TheGrid) 
    {
    TheGrid->Draw();
    }
  if(fPivot) fPivot->Draw("P");  
  DrawAtt(false);
  
  gPad->Modified();
  gPad->Update();	     
  if(fDebug) cout << "INFO: KVIDGridEditor::ForceUpdate(): Canvas and Co has been updated !" << endl; 
}
