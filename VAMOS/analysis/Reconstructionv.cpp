#include "Reconstructionv.h"
#include "TVector3.h"
#include "KVDataSet.h"
#include <cmath>
//Author: Maurycy Rejmund

using namespace std;

ClassImp(Reconstructionv)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Reconstructionv</h2>
<p>
Part of the VAMOS analysis package kindly contributed by Maurycy Rejmund (GANIL).
</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Reconstructionv::Reconstructionv(LogFile *Log, DriftChamberv *Drift)
{
#ifdef DEBUG
  cout << "Reconstructionv::Constructor" << endl;
#endif
  Ready=kFALSE;
  char line[255];
  char fname[30];
  int len=255;
  int i,j;

  L = Log;
  Dr = Drift;


  for(i=0;i<6;i++)
    Counter[i] = 0;

  Init();

  Rnd = new Random;
  


  ifstream inf;
  if(!gDataSet->OpenDataSetFile("Reconstruction.cal", inf))
    {
      cout << "Could not open the calibration file Reconstruction.cal !" << endl;
      return;
    }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Reading Reconstruction.cal" << endl;
      L->Log << "Reading Reconstruction.cal" << endl;


      inf.getline(line,len);
      cout << line << endl;
      L->Log << line << endl;
      inf >> BrhoRef;
      cout << BrhoRef << endl;
      L->Log << BrhoRef << endl;

      inf.getline(line,len);
     
      for(j=0;j<4;j++)
	{
	  inf.getline(line,len);
	  cout << line << endl;
	  L->Log << line << endl;
	  inf.getline(line,len);
	  sscanf(line,"%s",fname);
	  
	  
	  ifstream inf1;
	  
	  if(!gDataSet->OpenDataSetFile(fname,inf1))
	    {
	      cout << "Could not open the calibration file " << fname << endl;
	      return;
	    }
	  else
	    {
	      cout.setf(ios::showpoint);
	      cout << "Reading " << fname << endl;
	      L->Log << "Reading " << fname << endl;
	      inf1.getline(line,len);
	      cout << line << endl;
	      L->Log << line << endl;
	      
	      for(i=0;i<330;i++)
		inf1 >> Coef[j][i];

	    }
	  inf1.close();
	}
	      inf.getline(line,len);
	      cout << line << endl;
	      L->Log << line << endl;
	      inf >> PathOffset;
	      cout << "PathOffset: " << PathOffset << endl;
	      L->Log << "PathOffset: " << PathOffset << endl;
    }
  inf.close();
  Ready=kTRUE;
}
Reconstructionv::~Reconstructionv(void)
{
#ifdef DEBUG
  cout << "Reconstructionv::Destructor" << endl;
#endif

  PrintCounters();
}

void Reconstructionv::PrintCounters(void)
{
#ifdef DEBUG
  cout << "Reconstructionv::PrintCounters" << endl;
#endif
  cout << endl;
  cout << "Reconstructionv::PrintCounters" << endl;
  cout << "Called :" << Counter[0] << endl;
  cout << "Called with Drift Present :" << Counter[1] << endl;
  cout << "Present :" << Counter[2] << endl;

  L->Log << endl;
  L->Log << "Reconstructionv::PrintCounters" << endl;
  L->Log << "Called :" << Counter[0] << endl;
  L->Log << "Called with Drift Present :" << Counter[1] << endl;
  L->Log << "Present :" << Counter[2] << endl;


}

void Reconstructionv::Init(void)
{
#ifdef DEBUG
  cout << "Reconstructionv::Init" << endl;
#endif
  Present = false; 

  Brho = Theta = Phi = Path = -500;
  ThetaL = PhiL = -500.;
}

void Reconstructionv::SetBrhoRef(Double_t BB)
{
	B = BB;
}

void Reconstructionv::SetAngleVamos(Double_t ttheta)
{
	theta = ttheta; 
}

Double_t Reconstructionv::GetBrhoRef(void)
{
	return B;
}

Double_t Reconstructionv::GetAngleVamos(void)
{
	return theta; 
}

void Reconstructionv::Calculate(void)
{
#ifdef DEBUG
  cout << "Reconstructionv::Calculate" << endl;
#endif
  
  Double_t Brhot,Thetat,Phit,Patht;
  Double_t Vec[5],Vecp;

  Int_t i,j[7];
  
  Brhot=Thetat=Phit = Patht = 0.00000;
  Vec[0] = 1.000000;
  Vec[1] =(Double_t) (-1. * (Dr->Xf)/1000.);
  Vec[2] =0.;//(Double_t) (-1. * (Dr->Yf)/1000.);
  Vec[3] =(Double_t) (-1. * (Dr->Tf)/1000.);
  Vec[4] =0.;//(Double_t) (-1. * atan(tan((Dr->Pf)/1000.)*cos((Dr->Tf)/1000.)));
  //goes to zgoubi coordinates

  i = 0;
  for(j[0]=0;j[0]<5;j[0]++)
    for(j[1]=j[0];j[1]<5;j[1]++)
      for(j[2]=j[1];j[2]<5;j[2]++)
	for(j[3]=j[2];j[3]<5;j[3]++)
	  for(j[4]=j[3];j[4]<5;j[4]++)
	    for(j[5]=j[4];j[5]<5;j[5]++)
	      for(j[6]=j[5];j[6]<5;j[6]++)
	      {
		Vecp = Vec[j[0]]*Vec[j[1]]*Vec[j[2]]*Vec[j[3]]*Vec[j[4]]*Vec[j[5]]*Vec[j[6]];
		Brhot += Coef[0][i] *Vecp;
		Thetat += Coef[1][i] *Vecp;
		Patht += Coef[3][i] *Vecp;
		i++;
	      }
  Vec[0] = 1.000000;
  Vec[1] =(Double_t) (-1. * (Dr->Xf)/1000.);
  Vec[2] =(Double_t) (-1. * (Dr->Yf)/1000.);
  Vec[3] =(Double_t) (-1. * (Dr->Tf)/1000.);
  Vec[4] =(Double_t) (-1. * atan(tan((Dr->Pf)/1000.)*cos((Dr->Tf)/1000.)));

  i = 0;
  for(j[0]=0;j[0]<5;j[0]++)
    for(j[1]=j[0];j[1]<5;j[1]++)
      for(j[2]=j[1];j[2]<5;j[2]++)
	for(j[3]=j[2];j[3]<5;j[3]++)
	  for(j[4]=j[3];j[4]<5;j[4]++)
	    for(j[5]=j[4];j[5]<5;j[5]++)
	      for(j[6]=j[5];j[6]<5;j[6]++)
	      {
		Vecp = Vec[j[0]]*Vec[j[1]]*Vec[j[2]]*Vec[j[3]]*Vec[j[4]]*Vec[j[5]]*Vec[j[6]];
		Phit += Coef[2][i] *Vecp;
		i++;
	      }


  //  cout << i << " " << Dr->Xf/10. << " " << Dr->Tf << " " << Dr->Yf/10. << " " << Dr->Pf << endl; 
  if(Brhot >0.001 && Thetat > -300. && Thetat < 300. 
     && Phit > -300. && Phit < 300. && Patht >0 && Patht < 2000.)
    {
      Counter[2]++;
      Present = true;
      //Brho = BrhoRef*((Float_t) Brhot);

// NEW NEW NEW NEW NEW NEW NEW NEW 
	  Brho = GetBrhoRef()*((Float_t) Brhot);
// NEW NEW NEW NEW NEW NEW NEW NEW 


      Theta = (Float_t) Thetat*-1;
      Phi = (Float_t) Phit*-1;
      Path = (Float_t) Patht + PathOffset;

      TVector3 myVec(sin(Theta/1000.)*cos(Phi/1000.),sin(Phi/1000.),cos(Theta/1000.)*cos(Phi/1000.));
/*
 *		Inclusion of rotation angle of Vamos
 */
//      myVec.RotateY(35.*3.141592654/180.);
// NEW NEW NEW NEW NEW NEW NEW NEW 
      myVec.RotateY(GetAngleVamos()*3.141592654/180.);
// NEW NEW NEW NEW NEW NEW NEW NEW 
	  
      ThetaL = myVec.Theta();
      PhiL = myVec.Phi();
      
      //      cout << Brho << " " << Theta << " " << Phi << endl;
    } 

}


void Reconstructionv::Treat(void)
{
#ifdef DEBUG
  cout << "Reconstructionv::Treat" << endl;
#endif

  if( !Ready ) return;
  
  Counter[0]++;
  Init();
  if(Dr->Present)
    {
      Counter[1]++;
      Calculate();
#ifdef DEBUG
      Show();
#endif
    }
}

void Reconstructionv::outAttach(TTree *outT)
{

#ifdef DEBUG
  cout << "Reconstructionv::outAttach " << endl;
#endif

#ifdef DEBUG
  cout << "Attaching Reconstructionv variables" << endl;
#endif

  outT->Branch("Brho",&Brho,"Brho/F");
  outT->Branch("Theta",&Theta,"Theta/F");
  outT->Branch("Phi",&Phi,"Phi/F");
  outT->Branch("Path",&Path,"Path/F");
  outT->Branch("ThetaL",&ThetaL,"ThetaL/F");
  outT->Branch("PhiL",&PhiL,"PhiL/F");

}


void Reconstructionv::CreateHistograms(void)
{

#ifdef DEBUG
  cout << "Reconstructionv::CreateHistograms " << endl;
#endif
}
void Reconstructionv::FillHistograms(void)
{
#ifdef DEBUG
  cout << "Reconstructionv::FillHistograms " << endl;
#endif

}


void Reconstructionv::Show(void)
{
#ifdef DEBUG
  cout << "Reconstructionv::Show" << endl;
#endif
  cout.setf(ios::showpoint);
 
}
