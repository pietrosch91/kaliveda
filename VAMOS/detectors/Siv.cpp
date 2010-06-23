 #include "Siv.h"
#include "KVDataSet.h"
#include <cmath>
#include "TRandom3.h"

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

  Rnd = new TRandom3();

  ifstream inf;

  int tmp=0;
  //  if(!gDataSet->OpenDataSetFile("Si_calib2.dat",inf))
  if(!gDataSet->OpenDataSetFile("Si.cal",inf))
  {
     cout << "Could not open the calibration file Si.cal !" << endl;
     return;
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Reading Si.cal" << endl;
      L->Log << "Reading Si.cal" << endl;
      inf.getline(line,len);
      //cout << line << endl;
      //L->Log << line << endl;
      while(!inf.eof())
	{
	  inf.getline(line,len);
	  if(!inf.eof() && (tmp < 45))	//originalement tmp<45
	    {
	      if(tmp<21)
		{
		  sscanf(line,"%f %f %f %f %f", ECoef[tmp]+0, ECoef[tmp]+1, ECoef[tmp]+2, &tmp1,&tmp2);
		  for(i=0;i<3;i++)
		    ECoef[tmp][i] *= tmp1;
		    		//L->Log<<"ECoef : "<<ECoef[tmp][0]<<" "<<"-524 : "<<ECoef[0][0]<<endl;
		  tmp++;
		}
	      else if(tmp<42)	//originalement tmp<42
		{
		  sscanf(line,"%f %f", TOff[tmp-21]+0, TOff[tmp-21]+1);
		  		  //L->Log<<  "TO " <<TOff[tmp-21][0] << " " << TOff[tmp-21][1]<< endl;
		  tmp++;
		}
	      else
		{
		  sscanf(line,"%f %f %f %f %f", TCoef[tmp-42]+0, TCoef[tmp-42]+1, TCoef[tmp-42]+2, TCoef[tmp-42]+3, TCoef[tmp-42]+4); // originalement tmp-42
		  		  //L->Log <<  "TC " <<TCoef[tmp-42][0] << " " << TCoef[tmp-42][1]<< endl;
		  tmp++;		  
		}
	    }
	}
    }
#ifdef DEBUG
  for(int h=0;h<21;h++)
    {
      cout<<"Ecoef["<<h<<"][0]="<<ECoef[h][0]<<" Ecoef["<<h<<"][1]="<<ECoef[h][1]<<"Ecoef["<<h<<"][2]="<<ECoef[h][2]<<endl<<flush;
    }
#endif

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
      SiRaw[i] = 0;
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
  Number = -10;
  /*
  AA = 0;
  ZZ = 0;
  Err_E = 0;
  */
}

void Siv::Calibrate(void)
{
  Int_t i,j,k;

#ifdef DEBUG
  cout << "Si::Calibrate" << endl;
#endif
  
  for(i=0;i<3;i++)
    {
      //if(T_Raw[i] > 200)
	//for(j=0;j<2;j++)
	  //{
	   if(T_Raw[i]>TCoef[i][4])
	    	T[i] = (Float_t) TCoef[i][1]+(T_Raw[i]*TCoef[i][2]);
	   if(T_Raw[i]<=TCoef[i][4])
	    	T[i] = (Float_t) TCoef[i][1]+(T_Raw[i]*TCoef[i][2])+(TCoef[i][3]*powf((T_Raw[i]-TCoef[i][4]),2));	   
		//cout<< "T["<<i<<"] = "<<T[i]<<endl;	
	    	//cout << i << " " << j << " " <<TCoef[i][j] << endl;
	  //}
    }
  	L->Log<<"T_Raw[0] : "<<T_Raw[0]<<" "<<"T[0] : "<<T[0]<<endl;
	//L->Log<<"TCoef[0][1] : "<<TCoef[0][1]<<" "<<"TCoef[0][2] : "<<TCoef[0][2]<<" "<<"TCoef[0][3] : "<<TCoef[0][3]<<" "<<"TCoef[0][4] : "<<TCoef[0][4]<<endl;
  //cout<< "T[0] = "<<T[0]<<endl;
  //cout<< "T[1] = "<<T[1]<<endl;
  //cout<< "T[2] = "<<T[2]<<endl;    
  
  for(i=0;i<E_RawM;i++)
      {
	//cout<<"Si : E_Raw["<<i<<"] = "<<E_Raw[i]<<endl;
	//	Rnd->Next();
	Rnd->SetSeed(clock());

	for(k=0;k<3;k++)
	  {
// 	    E[E_Raw_Nr[i]] += powf((Float_t) E_Raw[i] + Rnd->Value(),
// 			  (Float_t) k)*ECoef[E_Raw_Nr[i]][k];
	    E[E_Raw_Nr[i]] += powf((Float_t) E_Raw[i] +	Rnd->Uniform(0,1),	
			  (Float_t) k)*ECoef[E_Raw_Nr[i]][k];
			  		  
	    //	    cout<<"RANDOM-------------"<<Rnd->Uniform(0,1)<<endl<<flush;
	    //	    cout<<"ECoeff="<<ECoef[E_Raw_Nr[i]][k]<<endl<<flush;
	  }
	if(E[E_Raw_Nr[i]] > 0)
 	  {
	    ETotal = E[E_Raw_Nr[i]];
	    	    //cout<<"ETotal Si = "<<E[E_Raw_Nr[i]]<<endl;
		    //cout<<"E Si = "<<E<<endl;
	    Number = E_Raw_Nr[i];

	    EM++;
	  }
      }

  
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
  //inT->SetBranchAddress("TSI_SED",T_Raw+1);
  //inT->SetBranchAddress("TSED_HF",T_Raw+2);

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
   //outT->Branch("SiE",E,"SiE[21]/F");
   //outT->Branch("SiET",&ETotal,"SiET/F");
   
   //outT->Branch("SiERaw",&E_Raw[0],"SiERaw/S");
   //outT->Branch("SiERaw",SiRaw,"SiERaw[21]/S");
   
   //outT->Branch("iA",&AA,"iA/D");
   //outT->Branch("ZZ",&ZZ,"ZZ/D");

   //outT->Branch("TSi_HF",&T[0],"TSi_HF/F");
   //outT->Branch("TSi_SeD",&T[1],"TSi_SeD/F");
   //outT->Branch("TSeD_HF",&T[2],"TSeD_HF/F");

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



