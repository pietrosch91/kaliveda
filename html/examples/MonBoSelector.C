#define MonBoSelector_cxx
// The class definition in MonBoSelector.h has been generated automatically
// by the KaliVeda utility KVSelector::Make().
//
// This class is derived from the KaliVeda class KVSelector.
// The following members functions are called by the TTree::Process() functions:
//    InitRun():       called everytime a run starts
//    EndRun():        called everytime a run ends
//    InitAnalysis():  called at the beginning of the analysis
//                     a convenient place to create your histograms.
//    Analysis():      called for each event. In this function you 
//                     fill your histograms.
//    EndAnalysis():   called at the end of a loop on the tree,
//                     a convenient place to draw/fit your histograms.
//
//   To use this file, try the following session on your Tree T
//
// Root > T->Process("MonBoSelector.C")
// Root > T->Process("MonBoSelector.C","some options")
// Root > T->Process("MonBoSelector.C+")
//

#include "MonBoSelector.h"
#include "KVINDRAReconNuc.h"
#include "TBenchmark.h"
#include "TLine.h"
#include "TString.h"
#include "KVTrieurLin.h"

ClassImp(MonBoSelector)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>MonBoSelector</h2>
<h4>Very complete example of analysis class using global variables and KVTrieur</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////


int nbevt;
// Variables communes à toutes les methodes
TBenchmark *bench=0;
TString *fnsave=0;	//
KVTrieurLin *triZmax=0; // Un trieur sur la valeur de Zmax

//_____________________________________
void MonBoSelector::InitRun(void)
 {
 // 
 // Initialisations for each run
 // Called at the beginning of each run
 //

 //
 // Enter your code here
 //

 // On affiche le numero du run qui va etre lu
 cout << "Debut du run " << gIndra->GetCurrentRunNumber() << endl;

 //set code mask for events
 GetEvent()->AcceptIDCodes(kIDCode2 | kIDCode3 | kIDCode4 | kIDCode6);
 GetEvent()->AcceptECodes(kECode1 | kECode2);

 }

//_____________________________________
void MonBoSelector::EndRun(void)
 {
 // 
 // Called at the end of each run
 //

 //
 // Enter your code here
 //

 // On indique que l'on a atteint la fin du run
 cout << "Fin du run " << gIndra->GetCurrentRunNumber() << endl;
 
 }

//_____________________________________
void MonBoSelector::InitAnalysis(void)
 {
 // 
 // Declaration of histograms, global variables, etc.
 // Called at the beginning of the analysis
 //

 //
 // Enter your code here
 //

 cout << "Debut de l'analyse" << endl;

 // Variables globales
 // the addresses of all global variables are added to the list of 
 // Global Variables. They will be reset and filled automatically for each
 // event. We only need to fill the histograms.
 
 // Initialisation de la KVGVList
 // Les variables globales etant declarees comme champ de la classe,
 // il suffiet de passer leurs adresses a la liste a l'aide de l'operateur &
 AddGV(&multIMF);
 AddGV(&SEkin);
 AddGV(&SEtrans);
 AddGV(&Zmean);
 AddGV(&tens);
 AddGV(&Zmax);
 AddGV(&Ztot);
 AddGV(&Ptot);
 AddGV(&ZVtot);
 // on affiche le contenu de la liste
 cout << "Liste des variables globales utilisees :" << endl;
 GetGVList()->Print();
 
 // Entrance channel values
 // on recupere les valeurs Ztot, ZVTot et Ptot
 KVDBSystem *kvs=gIndra->GetSystem();
 KV2Body *kv2b=kvs->GetKinematics();
 KVNucleus *proj=kv2b->GetNucleus(1);
 Double_t KEmax=10;
 if(kv2b)
  {
  ZStot=kvs->GetZtot();
  PStot=kvs->GetPtot();
  ZVStot=kvs->GetZVtot();
  KEmax=proj->GetKE()*1.5;
  cout << "Ptot  : " << PStot << endl;
  cout << "Ztot  : " << ZStot << endl; 
  cout << "ZVtot  : " << ZVStot << endl;
  cout << "KEmax : " << KEmax << endl; 
  }

 // Initialisation du trieur sur Zmax
 triZmax=new KVTrieurLin(10,"triZmax");
 triZmax->SetNomVar("Z_{max}");
 triZmax->SetXmin(0.);
 triZmax->SetXmax(kvs->GetZbeam());
 for(Int_t i=0;i<triZmax->GetNbCases();i++)
  {
  cout << i+1 << " : " << triZmax->GetNomCase(i+1) << endl;
  }
 cout << endl;
 
 // Nom du fichier de sauvegarde
 fnsave=new TString("MonBoSel_");
 (*fnsave)+=kv2b->GetNucleus(1)->GetSymbol();
 (*fnsave)+=kv2b->GetNucleus(2)->GetSymbol();
 (*fnsave)+=TMath::Nint(kvs->GetEbeam());
 (*fnsave)+=".root";
 fnsave->ReplaceAll(" ","");
 TFile *file=(TFile*)gROOT->FindObject(fnsave->Data());
 if(file) file->Close();
 file = new TFile(fnsave->Data(),"RECREATE");
 
 // Declaration des histogrammes
 hztzvt=new TH2F("ZVtotPtot","Ztot vs ZVtot",60,0,120,60,0,120);
 hztpt=new TH2F("ZtotPtot","Ztot vs Ptot",60,0,120,60,0,120);
 hEtEk=new TH2F("EtEkin","Etrans vs Ekin",40,0,KEmax,40,0,2000);
 hZmEk=new TH2F("ZmEkin","<Z> vs Ekin",40,0,KEmax,40,0,20);
 hsZEk=new TH2F("sZEkin","#sigma(Z) vs Ekin",40,0,KEmax,40,0,20);
 hZEk=new TH2F("ZEkin","Z vs Ekin",40,0,KEmax,60,0,60);
 hTfEk=new TH2F("TfEkin","Ekin vs #theta_{flot}",45,0,90,40,0,KEmax);
 hZmaxEk=new TH2F("ZmaxEk","Z_{max} vs Ekin",40,0,KEmax,60,0,60);
 hZVparZmax=new TH2F("ZVparZmax","Z vs Vpar pour Z_{max}",40,-5,15,60,0,60);
 hSC=new TH2F("SC","Coplanarite vs Sphericite",50,0,1,50,0,0.5);
 hZmItrieur=new TH2F("ZmItrieur","Itrieur vs Z_{max}",
                     kvs->GetZbeam(),0.5,kvs->GetZbeam()+0.5,
                     triZmax->GetNbCases(),0.5,triZmax->GetNbCases()+0.5);
 hztzvt->SetOption("zcol");
 hztpt->SetOption("zcol");
 hEtEk->SetOption("zcol");
 hZmEk->SetOption("zcol");
 hsZEk->SetOption("zcol");
 hZEk->SetOption("zcol");
 hTfEk->SetOption("zcol");
 hZmaxEk->SetOption("zcol");
 hZVparZmax->SetOption("zcol");
 hSC->SetOption("zcol");
 hZmItrieur->SetOption("zcol");
 TLine *li=new TLine(0,0,0.75,sqrt(3.)/4.);  // l'ajout des lignes a
 hSC->GetListOfFunctions()->Add(li);         // la liste des fonction de 
 li=new TLine(0.75,sqrt(3.)/4.,1,0);         // l'histogramme permet qu'elles
 hSC->GetListOfFunctions()->Add(li);         // se dessine automatiquement en
 					     // meme temps que l'histogramme
   
 nbevt=0;
 
 // benchmark pour avoir une idee du temps de traitement
 bench=new TBenchmark();
 bench->SetName("MonBoSelBench");
 bench->Start("Traitement");
 }

//_____________________________________
Bool_t MonBoSelector::Analysis(void)
 {
 // 
 // Analysis method. The event pointer
 // can be retrieved by a call to GetEvent().
 //

 //
 // Enter your code here
 //

 //
 // Les valeurs des variables globales ont ete deja ajustees. Il n'y a plus qu'a
 // recuper leurs valeurs. On utilisera ici l'operateur ().
 //
 
 nbevt++;
 if(!(nbevt%1000)) 
  {
  cout << "Analyse de l'evenement " << nbevt << " : " 
  << GetEvent()->GetMult() << endl;
  }
  //test event multiplicity >= acquisition trigger multiplicity
 if(!GetEvent()->IsOK()) return kTRUE;
 
 hztzvt->Fill(ZVtot()/ZVStot*100.,Ztot()/ZStot*100.);
 // On aurait aussi pu taper:
 // hztzvt->Fill(ZVtot.GetValue()/ZVStot*100.,Ztot.GetValue()/ZStot*100.);
 
 hztpt->Fill(Ptot()/PStot*100.,Ztot()/ZStot*100.);
 
 if(ZVtot()/ZVStot*100. < 65)
  {
  return kTRUE;
  }
 
 // remplissage des histogrammes
 hEtEk->Fill(SEkin(),SEtrans());
 hZmEk->Fill(SEkin(),Zmean());
 hsZEk->Fill(SEkin(),Zmean("RMS"));
 if(multIMF() >=2 )
  {
  hTfEk->Fill(tens(),SEkin());
  hSC->Fill(tens("Sphericite"),tens("Coplanarite"));
  }
  
 // On recupere le pointeur sur le noyau le plus lourd.
 KVNucleus *lourd=(KVNucleus *)Zmax.GetObject();
 if(lourd)
  {
  hZmaxEk->Fill(SEkin(),Zmax());
  hZVparZmax->Fill(lourd->GetVpar(),Zmax());
  hZmItrieur->Fill(Zmax(),triZmax->GetNumCase(Zmax()));
  }
  
 // une boucle pour remplir l'histogramme hZEk...
 KVINDRAReconNuc *part = 0;
 while( (part = GetEvent()->GetNextParticle("ok")) ){//loop over particles with correct codes
  hZEk->Fill(SEkin(),part->GetZ());
 }
 
 return kTRUE;
 }

//_____________________________________
void MonBoSelector::EndAnalysis(void)
 {
 // 
 // Global calculations, saving of histograms in files
 // Called at the end of the analysis
 //

 //
 // Enter your code here
 //

// On affiche le resultat du temps de traitement
 cout << "Fin de l'analyse : " << nbevt << " evenements lus" << endl;
 bench->Show("Traitement");
 cout << "Real: " << bench->GetRealTime("Traitement")/nbevt << " event/s " << endl; 
 cout << "CPU : " << bench->GetCpuTime("Traitement")/nbevt<< " event/s " << endl;
 TString mes1("Real: ");mes1+=bench->GetRealTime("Traitement")/nbevt;mes1+=" event/s";
 TString mes2("CPU : ");mes2+=bench->GetCpuTime("Traitement")/nbevt;mes2+=" event/s";
 
 // On recupere le nom du fichier de sauvegarde
 TFile *file=(TFile*)gROOT->FindObject(fnsave->Data());
 file->cd();
 TNamed *realtime=new TNamed("Real",mes1.Data());
 TNamed *cputime=new TNamed("CPU",mes2.Data());
 //on sauve tout
 realtime->Write();
 cputime->Write();
 file->Write();
 // on ferme le fichier
 file->Close();
 }
