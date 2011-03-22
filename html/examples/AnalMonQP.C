#define AnalMonQP_cxx
// The class definition in AnalMonQP.h has been generated automatically
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
// Modify these methods as you wish in order to create your analysis class.
// Don't forget that for every class used in the analysis, you must put a
// line '#include' at the beginning of this file.
// E.g. if you want to access informations on the INDRA multidetector
// through the global pointer gIndra, you must add the line
//   #include "KVINDRA.h"

#include "AnalMonQP.h"
#include "KVTrieurLin.h"
#include "TH2F.h"
#include "TFile.h"
#include "KVINDRAReconNuc.h"
#include "KVBatchSystem.h"

//
// Ces variables sont declarees ici afin qu'elles soient "vues" par toutes les
// methodes. On aurait aussi put les mettre declarer dans le fichier AnalMonQP.h
// en tant que variables (champs) de la classe AnalMonQP.
//
KVTrieurLin *triE=0;
Double_t vproj=0;
Double_t vcm=0;
Double_t zproj=0;
TVector3 Vcm;
TFile *fsave=0;

ClassImp(AnalMonQP)
      
////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>AnalMonQP</h2>
<h4>Example of analysis class with a 'user' global variable, KVQPSource</h4>
This example shows how to use a global variable class to reconstruct
the quasi-projectile source in heavy-ion collisions.
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

//_____________________________________
void AnalMonQP::InitAnalysis(void)
 {
 // 
 // Declaration of histograms, global variables, etc.
 // Called at the beginning of the analysis
 //

 //
 // Enter your code here
 //
 // To access some information about INDRA, use the global pointer gIndra
 // e.g. to access the list of silicon detetors
 //		KVList *sil_list = gIndra->GetListOfSi();
 
 
 // On ajoute notre variable globale a la liste des variables globales
 //
 AddGV("KVQPsource","MonQP");
 AddGV("KVMultAv","MultAv");
 
 // Declaration d'un KVTrieurLin pour classer les evenements selon l'energie
 // d'excitation a l'avant
 cout << "Declaration du KVTrieurLin " << endl;
 triE=new KVTrieurLin();
 triE->SetNbCases(10);
 triE->SetNomVar("E*/A");
 triE->SetXmin(0.);
 triE->SetXmax(10.);
 
 // On va sauvegarder les histogrammes dans le fichier MonQP_Projection.root
 // (si l'analyse s'effectue en mode interactif) ou dans le fichier 'Nom du Job'.root
 // (si l'analyse s'effectue en mode batch)
 if( !gBatchSystem ) { // en interactif
   fsave=new TFile("MonQP_Projection.root","recreate");
 } else {  // en batch
   fsave = new TFile ( Form( "%s.root", gBatchSystem->GetJobName()), "recreate");
 }
 // Declaration d'un histo E*/A vs Vqp
 TH2F *h2=new TH2F("hEvsV","E*/A vs V_{red}",60,0,1.2,60,0,15.);
 h2->SetXTitle("V_{red}");
 h2->SetYTitle("E*/A (MeV)");
 h2->SetOption("zcol");
 
 // Declaration des histogrammes Vpar-VPer dans le repère du QP pour les alphas
 // a l'avant
 for(Int_t i=0;i<triE->GetNbCases();i++)
  {
  h2=new TH2F(Form("hVparVperAlpha[%d]",i),
  	      Form("Vper vs Vpar alpha, %s",triE->GetNomCase(i+1)),
	      50,-10.,10.,50,0.,20.);
  h2->SetXTitle("V_{//} (cm/ns)");	      
  h2->SetYTitle("V_{#perp} (cm/ns)");
  h2->SetOption("zcol");	      
  }

 // Histogramme E*/A
 TH1F *h1=new TH1F("hEstar","E*/A QP",1000,0,10);
 h1->SetXTitle("E*/A (MeV)");
 
 // Listing des histogrammes declares
 fsave->ls();
  
 }

//_____________________________________
void AnalMonQP::InitRun(void)
 {
 // 
 // Initialisations for each run
 // Called at the beginning of each run
 //

 //
 // Enter your code here
 //
  cout << "Debut du run " << gIndra->GetCurrentRunNumber() << endl;
 // On récupère les valeurs de Vproj et Vcm pour le calcul de Vred
  vproj=gIndra->GetSystem()->GetKinematics()->GetNucleus(1)->GetVpar();
  zproj=gIndra->GetSystem()->GetZproj();
  Vcm=gIndra->GetSystem()->GetKinematics()->GetCMVelocity();
  vcm=Vcm.Z();
 // To define which identification/calibration codes you want
 // to use in your analysis:
    GetEvent()->AcceptIDCodes(kIDCode2 | kIDCode3 | kIDCode4 | kIDCode6);//VEDA id-codes code(i)=2 and 6 accepted
    GetEvent()->AcceptECodes( kECode1 | kECode2 );//VEDA ecode(i)=2 accepted
 //
 // If you want the angles of particles to be calculated using the
 // centre of each detector (instead of the randomised angles used by default):
 //     GetEvent()->UseMeanAngles();

 }

//_____________________________________
Bool_t AnalMonQP::Analysis(void)
 {
 // 
 // Analysis method.
 // The event pointer can be retrieved by a call to GetEvent().
 // See KVINDRAReconEvent documentation for the available methods.
 // In order to loop over all correctly identified particles in the event
 // (assuming the corresponding code masks were set in InitRun):
 //		KVINDRAReconNuc* particle;
 //		while ( (particle = GetEvent()->GetNextParticle("ok")) ){
 //            int z = particle->GetZ();
 //            ...etc. etc.
 //     }

 //
 // Enter your code here
 //
 // On récupère le pointeur sur la variable globale MonQP
 
 KVVarGlob *monqp=GetGV("MonQP");
 
 // On recupere la multiplicite a l'avant du QP
 Double_t mav=GetGV("MultAv")->GetValue();
 
 Double_t estar=monqp->GetValue("E*")/monqp->GetValue("A");
 
 // Si le Z du QP est inferieur a 60% du Z du projectile, ou que la multiplicite
 // a l'avant du CM est inferieure a 2, on sort
 if((monqp->GetValue("Z")/zproj )< 0.6 || mav < 2)
  {
  return kTRUE;
  } 

 TH1F *h1=(TH1F *)gROOT->FindObject("hEstar"); // on recupere le pointeur sur l'histogramme
 h1->Fill(estar);
 
 // On remplit l'histogramme E*/A vs Vred
 Double_t vred=(monqp->GetValue("Vz")-vcm)/(vproj-vcm);
 TH2F *h2=(TH2F *)gROOT->FindObject("hEvsV");
 h2->Fill(vred,estar);
 
 // On va rechercher le numero de case correspondant a E*/A
 Int_t ie=triE->GetNumCase(estar);
 
 // On defini le repere du QP
 KVNucleus *QP=(KVNucleus *)monqp->GetObject();
 TVector3 vqp=QP->GetV();
 GetEvent()->SetFrame("QP",vqp);

 // On recupere la vitesse du QP dans le referentiel du centre de masses
 QP->SetFrame("CM",Vcm);
 TVector3 vqpcm=QP->GetFrame("CM")->GetV();

 // On va recuperer le pointeur du Vpar-Vper
 TH2F *h2a=(TH2F *)gROOT->FindObject(Form("hVparVperAlpha[%d]",ie-1));
 
 // On va faire la boucle sur les particules associees au QP
 KVINDRAReconNuc *part=0;
  
 while( (part=GetEvent()->GetNextParticle("QP"))) // On peut utiliser ceci car on a appele "QP" toutes les particules associees
                                                  // au QP dans le KVQPsource.
  {
  // On verifie que c'est un alpha
  if((part->GetZ() == 2 && part->GetA() == 4))
   {
   //On calcule les composantes paralleles et perpendiculaires dans le repere
   //du QP
   TVector3 v=part->GetFrame("QP")->GetV(); // on recupere le vecteur vitesse dans le repere du QP
   Double_t vper=v.Perp(vqpcm);             // composante perpendiculaire par rapport au vecteur vqpcm
   Double_t vpar=v.Dot(vqpcm.Unit());	    // composante parallele par rapport au vecteur vqpcm
//    // autre possibilite
//    //Double_t vpar=v.Dot(vqpcm)/vqpcm.Mag();
   if(TMath::Abs(vper) > 0)
    {
    h2a->Fill(vpar,TMath::Abs(vper),1./TMath::Abs(vper)); // on remplit l'histogramme
    }
   }
  }


 return kTRUE;
 }

//_____________________________________
void AnalMonQP::EndRun(void)
 {
 // 
 // Called at the end of each run
 //

 //
 // Enter your code here
 //
 cout << "Fin du run " << gIndra->GetCurrentRunNumber() << endl;

 }

//_____________________________________
void AnalMonQP::EndAnalysis(void)
 {
 // 
 // Global calculations, saving of histograms in files
 // Called at the end of the analysis
 //

 //
 // Enter your code here
 //

 // On va ecrire et fermer le fichier MonQP.root
 cout << "Sauvegarde des histogrammes."<<endl;
 fsave->Write(); // On ecrit sur le disque tous les histogrammes
 fsave->Close(); // on ferme le fichier
 }
