#define KVFortranAnalysis_cxx
// The class definition in KVFortranAnalysis.h has been generated automatically
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
// through the global pointer gIndra, you must AT LEAST add the line
//   #include "KVINDRA.h"
// (in this particular case, if you wish to access some information which
// is specific to a particular campaign, you should use the class declaration
// corresponding to that campaign, e.g.
//	 #include "KVINDRA4.h"
// for the 4th campaign).

#include "KVFortranAnalysis.h"
#include "KVINDRAReconNuc.h"
#include "TRandom.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "Riostream.h"
#include "TString.h"
#include "TObjString.h"
#include "TList.h"
#include "TMacro.h"

ClassImp(KVFortranAnalysis)
////////////////////////////////////////////////////////////////////////////////
// For analysing INDRA data with a fortran file
//
// An intermediate .C file has to be build from the fortran file. This can be
// done by calling the static method:
//    KVFortranAnalysis::BuildKVSelectorFromFortran(const Char_t *f)
//
// where f is a character chain containing the name of the fortran file with its
// suffix ".f". Here is an example to convert the "MyFortranAnalysis.f" fortran file:
//    KVFortranAnalysis::BuildKVSelectorFromFortran("MyFortranAnalysis.f")
//
// If you run an analysis with the KaliVedaGUI of with the "kvdatanalyser" command, 
// type the complete name of the fortran file when needed. The convertion and the 
// compilation of the converted files will be done automatically. If not, edit a new
// text file and write 100 times: "I am a dummy fellow. I will not do it again.". Then
// close the text file and do not move: somebody is watching you.
// 
// If something goes wrong (incomplete conversion, syntax error, dead parrot), an
// error message wil appear indicating what to do. If nothing appears, you can expect 
// that all is ok. If you still feel not safe about it, you can light candels in the 
// closest church and pray as long as you can. If you believe in the Vodoo, you can buy
// a chicken and smash it on your keyboard. Do not hesitate to spread the chicken blood
// on all keys to improve the speed of your analysis.
//
// In case the "Hole in the fishing net" error appears, repair or replace as fast 
// as you can the fishing net. If you wait for a too long time, the April Fish may escape.
//
// If all is good, the generated file can be compiled by the analysis task. The 
// spectra will be saved automatically but who cares?
//
// If you have read this class description so far, we congratulate you. We will thank
// you by telling you a secret: this is an April Fish! If by chance the convertion of
// the fortran file is OK, the generated TH1F histograms will be filled with a gaussian
// distribution and an April Fish will be drawn on all TH2F histograms. We are sorry for
// the disturbance...
// 
////////////////////////////////////////////////////////////////////////////////

//_____________________________________
void KVFortranAnalysis::BuildKVSelectorFromFortran(const Char_t *f)
{
//
// April Fish 2007
// Method to convert fortran file into a C file containing a fake KVSelector
// Only made to generate an InitAnalysis() method.
//
TString fFortran=f;
if(!fFortran.Length())
 {
 cout << "Warning <KVFortranAnalysis::BuilKVSelectorFromFortran(const Char_t *f=\"\")> \n"; 
 cout << "CatchTheAprilFish Error: \nNo fortran file set. Nothing done." << endl;
 return;
 }
TMacro m;
if(!m.ReadFile(fFortran.Data()))
 {
 cout << "Warning <KVFortranAnalysis::BuilKVSelectorFromFortran(const Char_t *f=\"\")> \n"; 
 cout << "CatchTheAprilFish Error: \nThe fortran file \""<< fFortran.Data()<< 
 "\" does not exist. Nothing done" << endl;;
 return;
 }
fFortran.ReplaceAll(".f",".C");
ofstream fout(fFortran.Data());
fFortran.ReplaceAll(".C","");
TIter nextl(m.GetListOfLines());
TObjString *l=0;
Bool_t subrout=kFALSE;
fout << "#include \"" << fFortran.Data() << ".h\" " << endl; 
fout << "#include \"TH1F.h\" " << endl; 
fout << "#include \"TH2F.h\" " << endl << endl; 
while((l=(TObjString *)nextl()))
 {
 TString line=l->GetString();
 if(line.BeginsWith("c"))
  {
  line.Remove(0,1);
  line.Prepend("//");
  }
 else
  {
  line.ToLower();
  if(line.Contains("subroutine"))
   {
   if(line.Contains("ini_hbook"))
    {
    subrout=kTRUE;
    fout << "void " << fFortran.Data() << "::InitAnalysis(){" << endl;
    fout << "// Bad Joke" << endl;
    fout << "// It is an April Fish Routine: You should not try to run it..." <<
    endl;
    fout << "Warning(\"InitAnalysis()\",\"There is a hole in the fishing net: The April fish may escape\" ); " << endl;
    line="";
    fout << "       TH1F *h1=0;" << endl;
    fout << "       TH2F *h2=0;" << endl;
    }
   } 
  else if(line.Contains("return") && subrout)
   {
   line="";
   } 
  else if(line.Contains("end") && subrout)
   {
   fout << "}" << endl;
   subrout=kFALSE;
   } 
  else if(line.Contains("call") && subrout)
   {
   if(line.Contains("hbook1"))
    {
    line.ReplaceAll("call","h1=new ");
    line.ReplaceAll("hbook1","TH1F");
    line.ReplaceAll("'","\"");
    line.Append(";");
    Int_t i=line.Index("(");
    line.Insert(i+1,"\"h");
    i=line.Index(",");
    line.Insert(i,"\"");
    if(line.CountChar(',') == 5)
     {
     Int_t i=line.Index(")")-1;
     while(line(i) != ',') 
      {
      line.Remove(i,1);i--;
      }
     line.Remove(i,1);
     }
    fout << line.Data() << endl;
    fout << "       los.Add(h1);" << endl;
    }
   else if(line.Contains("hbook2"))
    {
    line.ReplaceAll("call","h2=new ");
    line.ReplaceAll("hbook2","TH2F");
    line.ReplaceAll("'","\"");
    line.Append(";");
    Int_t i=line.Index("(");
    line.Insert(i+1,"\"h");
    i=line.Index(",");
    line.Insert(i,"\"");
    if(line.CountChar(',') == 8)
     {
     Int_t i=line.Index(")")-1;
     while(line(i) != ',') 
      {
      line.Remove(i,1);i--;
      }
     line.Remove(i,1);
     }
    fout << line.Data() << endl;
    fout << "       los.Add(h2);" << endl;
    }
   else
    {
    line="";
    }
   }
  else
   {
   line="";
   } 
  }
 }
fout.close();
cout << fFortran.Data() << ".C file generated." <<endl;
fFortran.Append(".h");
ofstream fouth(fFortran.Data());
fFortran.ReplaceAll(".h","");
fouth << "#ifndef " << fFortran.Data() << "_H" << endl;
fouth << "#define " << fFortran.Data() << "_H"<< endl << endl;
fouth << "#include \"KVFortranAnalysis.h\" "<< endl << endl;
fouth << "class " << fFortran.Data() << " : public KVFortranAnalysis {" << endl;
fouth << "  public:" << endl;
fouth << "   " << fFortran.Data() << "(){}" << endl;
fouth << "   ~" << fFortran.Data() << "(){}" << endl << endl;
fouth << "   virtual void InitAnalysis();" << endl <<endl;
fouth << "};" <<  endl;
fouth << "#endif " << endl;
fouth.close();
cout << fFortran.Data() << ".h file generated." <<endl;
}

//_____________________________________
void KVFortranAnalysis::InitAnalysis(void)
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
 
 }

//_____________________________________
void KVFortranAnalysis::InitRun(void)
 {
 // 
 // Initialisations for each run
 // Called at the beginning of each run
 //

 //
 // Enter your code here
 //
 // To define which identification/calibration codes you want
 // to use in your analysis:
 //     GetEvent()->AcceptIDCodes(kIDCode2 | kIDCode3 | kIDCode4 | kIDCode6);//VEDA id-codes code(i)=2,3,4 and 6 accepted
 //     GetEvent()->AcceptECodes( kECode1|kECode2 );//VEDA ecode(i)=1&2 accepted
 //
 // If you want the angles of particles to be calculated using the
 // centre of each detector (instead of the randomised angles used by default):
 //     GetEvent()->UseMeanAngles();

 Warning("InitRun()","CatchTheAprilFish Error:\n\t there is a hole in the fishing net...");
 
 AprilFish=(TCutG *)gROOT->FindObject("AprilFish");
 if(!AprilFish)
  {
  AprilFish = new TCutG("AprilFish",28);
  AprilFish->SetVarX("");
  AprilFish->SetVarY("");
  AprilFish->SetTitle("Graph");
  AprilFish->SetPoint(0,0.149054,0.492882);
  AprilFish->SetPoint(1,0.184543,0.566913);
  AprilFish->SetPoint(2,0.265379,0.629556);
  AprilFish->SetPoint(3,0.42705,0.65803);
  AprilFish->SetPoint(4,0.529574,0.603929);
  AprilFish->SetPoint(5,0.663644,0.470103);
  AprilFish->SetPoint(6,0.705047,0.393223);
  AprilFish->SetPoint(7,0.73265,0.481492);
  AprilFish->SetPoint(8,0.819401,0.575456);
  AprilFish->SetPoint(9,0.880521,0.572608);
  AprilFish->SetPoint(10,0.813486,0.495729);
  AprilFish->SetPoint(11,0.754338,0.370444);
  AprilFish->SetPoint(12,0.72082,0.293565);
  AprilFish->SetPoint(13,0.777997,0.208144);
  AprilFish->SetPoint(14,0.852918,0.154043);
  AprilFish->SetPoint(15,0.886435,0.142654);
  AprilFish->SetPoint(16,0.734621,0.151196);
  AprilFish->SetPoint(17,0.679416,0.219533);
  AprilFish->SetPoint(18,0.675473,0.259396);
  AprilFish->SetPoint(19,0.628155,0.193907);
  AprilFish->SetPoint(20,0.496057,0.151196);
  AprilFish->SetPoint(21,0.336356,0.173975);
  AprilFish->SetPoint(22,0.174685,0.273633);
  AprilFish->SetPoint(23,0.131309,0.376139);
  AprilFish->SetPoint(24,0.265379,0.378986);
  AprilFish->SetPoint(25,0.133281,0.410308);
  AprilFish->SetPoint(26,0.141167,0.46156);
  AprilFish->SetPoint(27,0.149054,0.492882);
  }

 Eye=(TCutG *)gROOT->FindObject("Eye");
 if(!Eye)
  {
  Eye = new TCutG("Eye",8);
  Eye->SetVarX("");
  Eye->SetVarY("");
  Eye->SetTitle("Graph");
  Eye->SetPoint(0,0.235804,0.561219);
  Eye->SetPoint(1,0.225946,0.541287);
  Eye->SetPoint(2,0.227918,0.521355);
  Eye->SetPoint(3,0.243691,0.504271);
  Eye->SetPoint(4,0.259464,0.524203);
  Eye->SetPoint(5,0.257492,0.552677);
  Eye->SetPoint(6,0.249606,0.558371);
  Eye->SetPoint(7,0.235804,0.561219);
 }
}

//_____________________________________
Bool_t KVFortranAnalysis::Analysis(void)
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

 TIter next(&los);
 TObject *o=0;
 while((o=next()))
  {
  if(o->InheritsFrom("TH1") && !o->InheritsFrom("TH2"))
   {
   TH1 *h=(TH1 *)o;
   Double_t xmin=h->GetXaxis()->GetXmin();
   Double_t xmax=h->GetXaxis()->GetXmax();
   Double_t mean=(xmin+xmax)/2.;
   Double_t rms=(xmax-xmin)/10.;
   h->Fill(gRandom->Gaus(mean,rms));
   }
  if(o->InheritsFrom("TH2"))
   {
   TH2 *h=(TH2 *)o;
   Double_t xmin=h->GetXaxis()->GetXmin();
   Double_t xmax=h->GetXaxis()->GetXmax();
   Double_t ymin=h->GetYaxis()->GetXmin();
   Double_t ymax=h->GetYaxis()->GetXmax();
   Double_t x=gRandom->Rndm();
   Double_t y=gRandom->Rndm();
   if(AprilFish->IsInside(x,y) && !Eye->IsInside(x,y))
    {
    x*=(xmax-xmin);
    x+=xmin;
    y*=(ymax-ymin);
    y+=ymin;
    h->Fill(x,y);
    }
   }
  }

 return kTRUE;
 }

//_____________________________________
void KVFortranAnalysis::EndRun(void)
 {
 // 
 // Called at the end of each run
 //

 //
 // Enter your code here
 //

 }

//_____________________________________
void KVFortranAnalysis::EndAnalysis(void)
 {
 // 
 // Global calculations, saving of histograms in files
 // Called at the end of the analysis
 //

 //
 // Enter your code here
 //
 
 Warning("EndAnalysis()",
         "CatchTheAprilFish Error:\n\t Due to the hole in the fishing net, the April Fish did escape.");

 los.ls();
 
 TString fname=gSystem->Getenv("KVBATCHNAME");
 if(!fname.Length()) fname=Form("%s.root",IsA()->GetName()); else fname+=".root";

 Info("EndAnalysis()",
      Form("CatchTheAprilFish Info: spectra saved in \"%s\".",fname.Data()));
 
 TFile *f=new TFile(fname.Data(),"recreate");
 TIter next(&los);
 TObject *o=0;
 while((o=next()))
  {
  o->Write();
  }
 delete f;
 }
