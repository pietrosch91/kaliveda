#include "Identificationv.h"
#include "TMath.h"
#include <cmath>
//Author: Maurycy Rejmund
using namespace std;

ClassImp(Identificationv)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Identificationv</h2>
<p>
Part of the VAMOS analysis package kindly contributed by Maurycy Rejmund (GANIL).
</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Identificationv::Identificationv(LogFile *Log, Reconstructionv *Recon,
		  DriftChamberv *Drift, IonisationChamberv *IonCh, Siv *SiD)
{
#ifdef DEBUG
  cout << "Identificationv::Constructor" << endl;
#endif
  char line[255];
  char name[10];
  int len=255;
  int i;
  int tmp;

  L = Log;
  Rec = Recon;
  Dr = Drift;
  Ic = IonCh;
  Si = SiD;

  for(i=0;i<6;i++)
    Counter[i] = 0;

  Init();

  Rnd = new Random;


#ifdef FOLLOWPEAKS

  ifstream inf2("CgLast.cal");
  tmp = 0;
  if(!inf2)
  {
     cout << "Could not open the calibration file CgLast.cal !" << endl;

     ifstream inf3("CgFirst.cal");
     tmp = 0;
     if(!inf3)
       {
	 cout << "Could not open the calibration file CgFirst.cal !" << endl;
	 exit(-1);
       }
     else
       {
	 cout.setf(ios::showpoint);
	 cout << "Reading CgFirst.cal" << endl;
	 L->Log << "Reading CgFirst.cal" << endl;
	 inf3.getline(line,len);
	 cout << line << endl;
	 L->Log << line << endl;
	 while(!inf3.eof())
	   {
	     inf3.getline(line,len);
	     if(!inf3.eof() && (tmp < 21))
	       {
		 sscanf(line,"%f %f", CgM_v+tmp, CgM_Q_v+tmp);
		 tmp++;
	       }
	   }
       }
     inf3.close();
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Reading CgLast.cal" << endl;
      L->Log << "Reading CgLast.cal" << endl;
      inf2.getline(line,len);
      cout << line << endl;
      L->Log << line << endl;
      while(!inf2.eof())
	{
	  inf2.getline(line,len);
	  if(!inf2.eof() && (tmp < 21))
	    {
	      sscanf(line,"%f %f", CgM_v+tmp, CgM_Q_v+tmp);
	      tmp++;
	    }
	}
    }
  inf2.close();


  for(i=0;i<21;i++)
    {
      if( i==5 || i==6 || i==13 || i==14 || i==15)
	{
	  CgM[i] = 48.0;
	  CgM_Q[i] = 48.0/18.0;	  
	  //CgM_Q_v[i] = 48./18.;
	}
	else
	{
	  CgM[i] = 48.0;
	  CgM_Q[i] = 48.0/19.0;
	  //CgM_Q_v[i] = 48./19.;
	}
      //CgM_v[i] = 48.;
      myct[i] = 0;
      myct1[i] = 0;
    }

  for(i=0;i<21;i++)
    {
      //      cout << CgM_Q_v[i] << " " << CgM_v[i] << endl;
      L->Log << CgM_v[i] << " " << CgM_Q_v[i] << endl;
    }
#include "MyCuts.h" 

  for(i=0;i<21;i++)
    {
      if(myct[i])
	{
	  sprintf(name,"MYCUT%02d",i);
	  cout << "Printing " << name << endl;
	  myct[i]->Print();;
	}
    }
  for(i=0;i<21;i++)
    {
      if(myct1[i])
	{
	  sprintf(name,"MYCUT1_%02d",i);
	  cout << "Printing " << name << endl;
	  myct1[i]->Print();;
	}
    }



  ifstream inf("Sicorr.cal");

  
  tmp=0;
  if(!inf)
  {
     cout << "Could not open the calibration file Sicorr.cal !" << endl;
     exit(-1);
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Reading Sicorr.cal" << endl;
      L->Log << "Reading Sicorr.cal" << endl;
      inf.getline(line,len);
      cout << line << endl;
      L->Log << line << endl;
      while(!inf.eof())
	{
	  inf.getline(line,len);
	  if(!inf.eof() && (tmp < 21))
	    {
          Float_t dummy;
	      sscanf(line,"%f %f %f %f %f", M_QCoef[tmp]+0, M_QCoef[tmp]+1, M_QCoef[tmp]+2, &dummy, &dummy);
	      tmp++;
	    }
	}
    }
  inf.close();

  ifstream inf1("Sicorr1.cal");
  tmp=0;
  if(!inf1)
  {
     cout << "Could not open the calibration file Sicorr1.cal !" << endl;
     exit(-1);
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Reading Sicorr1.cal" << endl;
      L->Log << "Reading Sicorr1.cal" << endl;
      inf1.getline(line,len);
      cout << line << endl;
      L->Log << line << endl;
      while(!inf1.eof())
	{
	  inf1.getline(line,len);
	  if(!inf1.eof() && (tmp < 21))
	    {
          Float_t dummy;
	      sscanf(line,"%f %f %f %f %f", MCoef[tmp]+0, MCoef[tmp]+1, MCoef[tmp]+2, &dummy, &dummy);
	      tmp++;
	    }
	}
    }
  inf1.close();


#endif

}
Identificationv::~Identificationv(void)
{
#ifdef DEBUG
  cout << "Identificationv::Destructor" << endl;
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

void Identificationv::PrintCounters(void)
{
#ifdef DEBUG
  cout << "Identificationv::PrintCounters" << endl;
#endif
  cout << endl;
  cout << "Identificationv::PrintCounters" << endl;
  cout << "Called :" << Counter[0] << endl;
  cout << "Called with Rec Present :" << Counter[1] << endl;
  cout << "Present :" << Counter[2] << endl;

  L->Log << endl;
  L->Log << "Identificationv::PrintCounters" << endl;
  L->Log << "Called :" << Counter[0] << endl;
  L->Log << "Called with Rec Present :" << Counter[1] << endl;
  L->Log << "Present :" << Counter[2] << endl;


}

void Identificationv::Init(void)
{
#ifdef DEBUG
  cout << "Identificationv::Init" << endl;
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

void Identificationv::Calculate(void)
{
#ifdef DEBUG
  cout << "Identificationv::Calculate" << endl;
#endif

  if(
     Dr->E[0] > 0 &&
     Dr->E[1] > 0 &&
     Ic->ETotal > 0)
    {
      //      dE += Dr->E[0];
      //      dE += Dr->E[1];
      dE = dE1 = Ic->ETotal;
      if(Dr->Present) dE1 = dE1 / cos(Dr->Tf/1000.);
      dE /= 0.614;
      dE += dE*0.15;
      if(Si->ETotal > 0)
	E = dE + Si->ETotal*0.99;
    }


  if(Si->T[1] >0. && Si->T[2] >0.)
    {
      if(Si->T[2] < Si->T[1] * 1.4+82.)
 	T = Si->T[2] + 116.69;
      else
 	T = Si->T[2]; 
      T -= 3.;
    }

  if(T >0 && Rec->Path>0 && Dr->Present)
    {
      D = Rec->Path + (-1.*(Dr->Yf)/10.*sin(3.14159/4.)/
	cos(3.14159/4. + fabs(Dr->Pf/1000.)))/cos(Dr->Tf/1000.);
      //            cout << Rec->Path << " " << (Dr->Yf)/10. << " " <<
      //      	Rec->Phi/1000.*180/3.1415 << " " << D << endl;
      V = D/T;
      V = V+ V*(1-cos(Dr->Tf/1000.)*cos(Dr->Pf/1000.));
      Beta =V/29.9792; 
      Gamma = 1./sqrt(1.-TMath::Power(Beta,2.));
    }

  if(Beta>0 && Rec->Brho>0&&Gamma>1.&&Si->Present)
    {
      M = 2.* (E/1000.) / 931.5016/TMath::Power(Beta,2.);
      M_Q = Rec->Brho/3.105/Beta;

#ifdef FOLLOWPEAKS
      if(myct[Si->Number]->IsInside(M_Q,M))
	{
	  CgM_v[Si->Number] = 0.99*CgM_v[Si->Number] + 0.01*M;
	  CgM_Q_v[Si->Number] = 0.99*CgM_Q_v[Si->Number] + 0.01*M_Q;
	}
      {
	Mcorr = M + (CgM[Si->Number] - CgM_v[Si->Number]);
	M_Qcorr = M_Q + (CgM_Q[Si->Number] - CgM_Q_v[Si->Number]);
	for(Int_t k=0;k<3;k++)
	  {
	    M_Qcorr1 += powf(M_Qcorr,
			  (Float_t) k)*M_QCoef[Si->Number][k];	    
	  }
	for(Int_t k=0;k<3;k++)
	  {
	    Mcorr1 += powf(Mcorr,
			  (Float_t) k)*MCoef[Si->Number][k];	    
	  }
      }
#endif

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


void Identificationv::Treat(void)
{
#ifdef DEBUG
  cout << "Identificationv::Treat" << endl;
#endif

  Counter[0]++;
  Init();
  if(Rec->Present) Counter[1]++;
  Calculate();
#ifdef DEBUG
  Show();
#endif
  
}
void Identificationv::inAttach(TTree *inT)
{

#ifdef DEBUG
  cout << "Identificationv::inAttach " << endl;
#endif

#ifdef DEBUG
  cout << "Attaching Identification variables" << endl;
#endif
#ifdef ACTIVEBRANCHES
  cout << "Activating Branches: Identificationv" << endl;
  L->Log << "Activating Branches: Identificationv" << endl;

#endif

}

void Identificationv::outAttach(TTree *outT)
{

#ifdef DEBUG
  cout << "Identificationv::outAttach " << endl;
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


void Identificationv::CreateHistograms(void)
{

#ifdef DEBUG
  cout << "Identificationv::CreateHistograms " << endl;
#endif
}
void Identificationv::FillHistograms(void)
{
#ifdef DEBUG
  cout << "Identificationv::FillHistograms " << endl;
#endif

}


void Identificationv::Show(void)
{
#ifdef DEBUG
  cout << "Identificationv::Show" << endl;
#endif
  cout.setf(ios::showpoint);
 
}
