 #include "Siv.h"
#include "KVDataSet.h"
#include <cmath>

//Author: Maurycy Rejmund
ClassImp(Siv)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Siv</h2>
<p>
Part of the VAMOS analysis package kindly contributed by Maurycy Rejmund (GANIL).
</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

Siv::Siv(LogFile *Log)
{
#ifdef DEBUG
  cout << "Si::Constructor" << endl;
#endif
  Ready=kFALSE;
  
  char line[255];
  int len=255;
  int i;
  Float_t tmp1,tmp2;

  L=Log;

  for(i=0;i<2;i++)
    Counter[i] = 0;

  InitRaw();
  Init();

  Rnd = new Random;
  
  ifstream inf;
  
  int tmp=0;
  if(!gDataSet->OpenDataSetFile("Si.cal",inf))
  {
     cout << "Could not open the calibration file Si.cal !" << endl;
     return;
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "-> Reading Si.cal" << endl;
      L->Log << "-> Reading Si.cal" << endl;
      inf.getline(line,len);
      cout << line << endl;
      L->Log << line << endl;
      while(!inf.eof())
	{
	  inf.getline(line,len);
	  if(!inf.eof() && (tmp < 45))
	    {
	      if(tmp<21)
		{
		  sscanf(line,"%f %f %f %f %f %f %f %f", 
		  	ECoef[tmp]+0, ECoef[tmp]+1, ECoef[tmp]+2, ECoef[tmp]+3, ECoef[tmp]+4, ECoef[tmp]+5, ECoef[tmp]+6, ECoef[tmp]+7);

// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	
// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	
		cout << ECoef[tmp][0] <<" " << ECoef[tmp][1] <<" " << ECoef[tmp][2] <<" " 
		<< ECoef[tmp][3] <<" " << ECoef[tmp][4] <<" " << ECoef[tmp][5] <<" " 
		<< ECoef[tmp][6] <<" " << ECoef[tmp][7] <<" " << endl;
// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	
// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	
		  
		  tmp++;
		}
	      else if(tmp<42)
		{
		  sscanf(line,"%f %f", TOff[tmp-21]+0, TOff[tmp-21]+1);
		  //		  cout <<  "TO " <<TOff[tmp-21][0] << " " << TOff[tmp-21][1]<< endl;
		  tmp++;
		}
	      else
		{
		  sscanf(line,"%f %f", TCoef[tmp-42]+0, TCoef[tmp-42]+1);
		  //		  cout <<  "TC " <<TCoef[tmp-42][0] << " " << TCoef[tmp-42][1]<< endl;
		  tmp++;		  
		}
	    }
	}
    }
  inf.close();
  Ready=kTRUE;
}
Siv::~Siv(void)
{
#ifdef DEBUG
  cout << "Si::Destructor" << endl;
#endif
  
  PrintCounters();
}

void Siv::PrintCounters(void)
{
#ifdef DEBUG
  cout << "Si::PrintCounters" << endl;
#endif
  
  cout << endl;
  cout << "Si::PrintCounters" << endl;
  cout << "Called: " << Counter[0] << endl;
  cout << "Present: " << Counter[1] << endl;
  
  L->Log << endl;
  L->Log << "Si::PrintCounters" << endl;
  L->Log << "Called: " << Counter[0] << endl;
  L->Log << "Present: " << Counter[1] << endl;

}


void Siv::InitRaw(void)
{
#ifdef DEBUG
  cout << "Si::InitRaw" << endl;
#endif

  for(Int_t i=0;i<21;i++)
    {
      E_Raw[i] = 0;
      E_Raw_Nr[i] = 0;
    }
  for(Int_t i=0;i<3;i++)
    {
      T_Raw[i] = 0;
    }
  E_RawM = 0;
}

void Siv::Init(void)
{
#ifdef DEBUG
  cout << "Siv::Init" << endl;
#endif
  Present = false; 

  for(Int_t i=0;i<21;i++)
    {
      E[i] = 0.0;
    }
  for(Int_t i=0;i<3;i++)
    {
      T[i] = 0.0;
    }
  Offset[0] = Offset[1]  = 0.0;
  EM = 0;
  ETotal = 0.0;
  Number = 0;

}

void Siv::Calibrate(void)
{
	Int_t i,j,k;
	double x,a,b,c,d,e,f,g,h;

#ifdef DEBUG
	cout << "Si::Calibrate" << endl;
#endif

	for(i=0;i<3;i++)
	{
		if(T_Raw[i] > 200)
		for(j=0;j<2;j++)
		{
			T[i] = (Float_t) T_Raw[i];
			
//			cout << i << " " << j << " " <<TCoef[i][j] << endl;
		}
	}
/*
 *	Energy calibration with Fermi functions
 */
	for(i=0;i<E_RawM;i++)
	{
		Rnd->Next();
		
		x = (double)E_Raw[i]+(double)Rnd->Value() ;
		a = (double)ECoef[E_Raw_Nr[i]][0] ;
		b = (double)ECoef[E_Raw_Nr[i]][1] ;
		c = (double)ECoef[E_Raw_Nr[i]][2] ;
		d = (double)ECoef[E_Raw_Nr[i]][3] ;
		e = (double)ECoef[E_Raw_Nr[i]][4] ;
		f = (double)ECoef[E_Raw_Nr[i]][5] ;
		g = (double)ECoef[E_Raw_Nr[i]][6] ;
		h = (double)ECoef[E_Raw_Nr[i]][7] ;
		
		E[E_Raw_Nr[i]] = (Float_t)( ((a*(1./(exp(b*(x+c))+1.)-1.))+(d+e*x+f*x*x))*(g/h) );		
		
		if(E[E_Raw_Nr[i]] > 0)
		{
			ETotal = E[E_Raw_Nr[i]];
			Number = E_Raw_Nr[i]+1;


			EM++;
		}
	}
// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	
// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	

			cout << Number << " , "<< ETotal << " , "<< E_RawM << endl;  

// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	
// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	

  
	if(EM!=1) 
	{
		ETotal = 0.0;
		Number = 0;
		for(k=0;k<2;k++)
		Offset[k] = 0.0;
	}
      
	if(ETotal > 0) 
	{    
		Present = true;
		Counter[1]++;
	}
  
}

void Siv::Treat(void)
{
#ifdef DEBUG
  cout << "Siv::Treat" << endl;
#endif
  if(!Ready) return;
  
  Counter[0]++;
  Init();
  Calibrate();
#ifdef DEBUG
  Show_Raw();
  Show();
#endif
}

void Siv::inAttach(TTree *inT)
{

#ifdef DEBUG
  cout << "Si::inAttach" << endl;
#endif

#ifdef DEBUG
  cout << "Attaching Si variables" << endl;
#endif

#ifdef ACTIVEBRANCHES
  cout << "Activating Branches: Si" << endl;
  L->Log << "Activating Branches: Si" << endl;

  inT->SetBranchStatus("SIEM",1);
  inT->SetBranchStatus("SIE",1);
  inT->SetBranchStatus("SIENr",1);

  inT->SetBranchStatus("TSI_HF",1);
  inT->SetBranchStatus("TSI_SED",1);
  inT->SetBranchStatus("TSED_HF",1);

  

#endif

  inT->SetBranchAddress("SIEM",&E_RawM);
  inT->SetBranchAddress("SIE",E_Raw);
  inT->SetBranchAddress("SIENr",E_Raw_Nr);

  inT->SetBranchAddress("TSI_HF",T_Raw+0);
  inT->SetBranchAddress("TSI_SED",T_Raw+1);
  inT->SetBranchAddress("TSED_HF",T_Raw+2);

}
void Siv::outAttach(TTree *outT)
{

#ifdef DEBUG
  cout << "Si::inAttach" << endl;
#endif

#ifdef DEBUG
  cout << "Attaching Si variables" << endl;
#endif

   outT->Branch("SiEM",&EM,"SiEM/I");
   outT->Branch("SiE",E,"SiE[21]/F");
   outT->Branch("SiET",&ETotal,"SiET/F");

   outT->Branch("SIE_Raw",E_Raw,"SiE_Raw[21]/s");
   outT->Branch("SIE_Nr",&Number,"SIE_Nr/s");

// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	
// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	

			cout << Number << " , "<< ETotal << endl;  

// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	
// STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID STUPID 		 	

   outT->Branch("TSi_HF",&T[0],"TSi_HF/F");
   outT->Branch("TSi_SeD",&T[1],"TSi_SeD/F");
   outT->Branch("TSeD_HF",&T[2],"TSeD_HF/F");
}
void Siv::CreateHistograms(void)
{

#ifdef DEBUG
  cout << "Si::CreateHistograms" << endl;
#endif

}
void Siv::FillHistograms(void)
{

#ifdef DEBUG
  cout << "Si::FillHistograms" << endl;
#endif

}


void Siv::Show_Raw(void)
{
  Int_t i;
#ifdef DEBUG
  cout << "Si::Show_Raw" << endl;
#endif
  
  cout.setf(ios::showpoint);

  for(i=0;i<E_RawM;i++)
  {
    cout << "CrER: " << E_Raw_Nr[i] << " " << E_Raw[i] << endl; 
  }

}
void Siv::Show(void)
{
  Int_t i;
#ifdef DEBUG
  cout << "Si::Show" << endl;
#endif

  cout.setf(ios::showpoint);
 
  cout << "Present: " << Present << endl;
  if(Present)
    {
      cout << "EM: " << EM << endl;
      for(i=0;i<21;i++)
	{
	  if(E[i]>0.) cout << "SiE: " << i << " " << E[i] << endl; 
	}
    }
}



