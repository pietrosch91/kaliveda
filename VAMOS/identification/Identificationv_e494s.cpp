//Created by KVClassFactory on Thu Dec 17 19:57:43 2009
//Author: paola,,,

#include "Identificationv_e494s.h"

using namespace std;

ClassImp(Identificationv_e494s)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Identificationv_e494s</h2>
<h4>Identificationv for e494s experiment</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Identificationv_e494s::Identificationv_e494s(LogFile *Log, ReconstructionSeDv *Recon,
		  SeD12v *Sed12, IonisationChamberv *IonCh, Siv *SiD)
{
   // Default constructor
#ifdef DEBUG
  cout << "Identificationv_e494s::Constructor" << endl;
#endif

  L = Log;
  Rec = Recon;
  Se = Sed12;
  Ic = IonCh;
  Si = SiD;

  for(Int_t i=0;i<6;i++)
    Counter[i] = 0;

  Init();

  Rnd = new Random;



}

Identificationv_e494s::~Identificationv_e494s()
{
   // Destructor
#ifdef DEBUG
  cout << "Identificationv_e494s::Destructor" << endl;
#endif

  PrintCounters();

#ifdef FOLLOWPEAKS
  for(Int_t i=0;i<21;i++)
    {
      if(myct[i]) delete myct[i];
      if(myct1[i]) delete myct1[i];
    }
  ofstream ouf("CgLast.cal");
  if(!ouf)
  {
     cout << "Could not open the calibration file CgLast.cal for writing!" << endl;
     exit(-1);
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Writing CgLast.cal" << endl;
      L->Log << "Writing CgLast.cal" << endl;
      ouf << "// CgM_v   CgM_Q_v" << endl;
      for(Int_t i=0;i<21;i++)
	{
	  ouf << CgM_v[i] << " " << CgM_Q_v[i] << endl;
	}
    }
  ouf.close();



#endif

}

void Identificationv_e494s::PrintCounters(void)
{
#ifdef DEBUG
  cout << "Identificationv_e494s::PrintCounters" << endl;
#endif
  cout << endl;
  cout << "Identificationv_e494s::PrintCounters" << endl;
  cout << "Called :" << Counter[0] << endl;
  cout << "Called with Rec Present :" << Counter[1] << endl;
  cout << "Present :" << Counter[2] << endl;

  L->Log << endl;
  L->Log << "Identificationv_e494s::PrintCounters" << endl;
  L->Log << "Called :" << Counter[0] << endl;
  L->Log << "Called with Rec Present :" << Counter[1] << endl;
  L->Log << "Present :" << Counter[2] << endl;


}

void Identificationv_e494s::Init(void)
{
#ifdef DEBUG
  cout << "Identificationv_e494s::Init" << endl;
#endif
  Present = false; 

  dE = dE1 = E = T = V = M_Q = M = Z1 = Z2 =  Beta = Q = D = 0;
  M_Qr = Mr = Qr = 0.0;
  Qc = Mc = 0.0;
  Gamma = 1.; 

#ifdef FOLLOWPEAKS
  Mcorr = M_Qcorr = 0.0;
  M_Qcorr1 = 0.0;
  Mcorr1 = 0.0;
#endif

}

void Identificationv_e494s::Calculate(void)
{
#ifdef DEBUG
  cout << "Identificationv_e494s::Calculate" << endl;
#endif

  if(
//      Se->E[0] > 0 &&
//      Se->E[1] > 0 &&
     Ic->ETotal > 0)
    {
      dE = dE1 = Ic->ETotal;
      //      if(Se->Present) dE1 = dE1 / cos(Se->Tf/1000.);
      dE /= 0.614;
      dE += dE*0.15;
      if(Si->ETotal > 0)
	//	E = dE + Si->ETotal*0.99;
	E = dE + Si->ETotal;
    }


  if(Si->T[1] >0. && Si->T[2] >0.)
    {
//       if(Si->T[2] < Si->T[1] * 1.4+82.)
//  	T = Si->T[2] + 116.69;
//       else
 	T = Si->T[2]; 
      //      T -= 3.;
    }

  if(T >0 && Rec->Path>0 && Se->fPresent)
    {
      D = Rec->Path + (-1.*(Se->fYf)/10.*sin(3.14159/4.)/
	cos(3.14159/4. + fabs(Se->fPf/1000.)))/cos(Se->fTf/1000.);
      //            cout << Rec->Path << " " << (Se->Yf)/10. << " " <<
      //      	Rec->Phi/1000.*180/3.1415 << " " << D << endl;
      V = D/T;
      //      V = V+ V*(1-cos(Se->Tf/1000.)*cos(Se->Pf/1000.));
      Beta =V/29.9792; 
      Gamma = 1./sqrt(1.-TMath::Power(Beta,2.));
    }

  if(Beta>0 && Rec->Brho>0&&Gamma>1.&&Si->Present)
    {
      M = 2.* (E/1000.) / 931.5016/TMath::Power(Beta,2.);
      M_Q = Rec->Brho/3.105/Beta;



      Mr = (E/1000.)/931.5016/(Gamma-1.);
      M_Qr = Rec->Brho/3.105/Beta/Gamma;

      Q = M/M_Q;
      Qr = Mr/M_Qr;
      Qr = (-1.036820+1.042380*Qr +0.4801678e-03*Qr*Qr);
      Qc = int(Qr+0.5);
      Mc = M_Qr*Qc;
    }
  
  Z1 = sqrt(dE*E/pow(931.5016,2.))/pow(29.9792,2.)*100.;
  Z2 = sqrt(dE/931.5016)*TMath::Power(Beta,2.)*100.;



  if( T > 0 && V > 0 && M_Q > 0 && M > 0 && Z1 > 0 && Z2 > 0 && Beta > 0 && Gamma > 1.0) 
    {
      Present=true;
      Counter[2]++;
    }
}


void Identificationv_e494s::Treat(void)
{
#ifdef DEBUG
  cout << "Identificationv_e494s::Treat" << endl;
#endif

  Counter[0]++;
  Init();
  if(Rec->Present) Counter[1]++;
  Calculate();
#ifdef DEBUG
  Show();
#endif
  
}
void Identificationv_e494s::inAttach(TTree *inT)
{

#ifdef DEBUG
  cout << "Identificationv_e494s::inAttach " << endl;
#endif

#ifdef DEBUG
  cout << "Attaching Identification variables" << endl;
#endif
#ifdef ACTIVEBRANCHES
  cout << "Activating Branches: Identificationv" << endl;
  L->Log << "Activating Branches: Identificationv" << endl;

#endif

}

void Identificationv_e494s::outAttach(TTree *outT)
{

#ifdef DEBUG
  cout << "Identificationv_e494s::outAttach " << endl;
#endif

#ifdef DEBUG
  cout << "Attaching Identificationv variables" << endl;
#endif

  outT->Branch("dE",&dE,"dE/F");
  outT->Branch("dE1",&dE1,"dE1/F");
  outT->Branch("E",&E,"E/F");
  outT->Branch("T",&T,"T/F");
  outT->Branch("V",&V,"V/F");
  outT->Branch("D",&D,"D/F");
  outT->Branch("Beta",&Beta,"Beta/F");
  outT->Branch("Gamma",&Gamma,"Gamma/F");
  outT->Branch("M_Q",&M_Q,"M_Q/F");
  outT->Branch("Q",&Q,"Q/F");
  outT->Branch("M",&M,"M/F");
  outT->Branch("M_Qr",&M_Qr,"M_Qr/F");
  outT->Branch("Qr",&Qr,"Qr/F");
  outT->Branch("Mr",&Mr,"Mr/F");
  outT->Branch("Qc",&Qc,"Qc/F");
  outT->Branch("Mc",&Mc,"Mc/F");
  outT->Branch("Z1",&Z1,"Z1/F");
  outT->Branch("Z2",&Z2,"Z2/F");
#ifdef FOLLOWPEAKS
  outT->Branch("M_Qcorr",&M_Qcorr,"M_Qcorr/F");
  outT->Branch("Mcorr",&Mcorr,"Mcorr/F");
  outT->Branch("M_Qcorr1",&M_Qcorr1,"M_Qcorr1/F");
  outT->Branch("Mcorr1",&Mcorr1,"Mcorr1/F");
#endif
}


void Identificationv_e494s::CreateHistograms(void)
{

#ifdef DEBUG
  cout << "Identificationv_e494s::CreateHistograms " << endl;
#endif
}
void Identificationv_e494s::FillHistograms(void)
{
#ifdef DEBUG
  cout << "Identificationv_e494s::FillHistograms " << endl;
#endif

}


void Identificationv_e494s::Show(void)
{
#ifdef DEBUG
  cout << "Identificationv_e494s::Show" << endl;
#endif
  cout.setf(ios::showpoint);
 
}
