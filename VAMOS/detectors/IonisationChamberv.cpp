#include "IonisationChamberv.h"
#include "KVDataSet.h"
#include <cmath>
//Author: Maurycy Rejmund

ClassImp(IonisationChamberv)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>IonisationChamberv</h2>
<p>
Part of the VAMOS analysis package kindly contributed by Maurycy Rejmund (GANIL).
</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

IonisationChamberv::IonisationChamberv(LogFile *Log)
{
#ifdef DEBUG
  cout << "IonisationChamberv::Constructor" << endl;
#endif
  Ready=kFALSE;
  
  char line[255];
  int len=255;
  int i,j,k;
  Float_t tmp;
  Float_t tmp1;

  L=Log;

  for(i=0;i<5;i++)
    Counter[i] = 0;

  InitRaw();
  Init();

  Rnd = new Random;

   ifstream inf;
   Int_t num=0;
   Float_t dummy1, dummy2, dummy3;
   TString sline;

  if(!gDataSet->OpenDataSetFile("IonisationChamber.cal",inf))
  {
     cout << "Could not open the calibration file IonisationChamber.cal !" << endl;
     return;
  }
   else 
  {
   cout<< "Reading IonisationChamber.cal" <<endl;
   while(!inf.eof()){
       sline.ReadLine(inf);
       if(!inf.eof()){
	   if (!sline.BeginsWith("#")){
	     sscanf(sline.Data(),"%d %f %f %f", &num, &dummy1, &dummy2, &dummy3);
	     a[num] = dummy1;
	     b[num] = dummy2;
	     Vnorm[num] = dummy3;	     
	     	   }
         	}
       	}
}
/*
  else
    {
      cout.setf(ios::showpoint);
      cout << "Reading IonisationChamber.cal" << endl;
      L->Log << "Reading IonisationChamber.cal" << endl;
      for(i=0;i<3;i++) 
	{
	  inf.getline(line,len);
	  cout << line << endl;
	  L->Log << line << endl;
	  for(j=0;j<7;j++) 
	    {
	      for(k=0;k<3;k++) 
		inf >> ECoef[i][j][k];
	      
	      inf >> tmp;
	      inf >> tmp1;

	      for(k=0;k<3;k++) 
		ECoef[i][j][k] *= tmp;

	      inf.getline(line,len);
	    }
	}
      for(j=0;j<3;j++) 
	{
	  inf.getline(line,len);
	  cout << line << endl;
	  L->Log << line << endl;
	  for(k=0;k<2;k++) 
	    inf >> ERef[j][k]; 
	  inf.getline(line,len);
	}
      for(j=0;j<3;j++) 
	{
	  inf.getline(line,len);
	  cout << line << endl;
	  L->Log << line << endl;
	  inf >> ETresh[j]; 
	  cout << "ETresh " << ETresh[j] << endl;
	  inf.getline(line,len);
	}
    }
*/
  inf.close();
  Ready=kTRUE;
  
}
IonisationChamberv::~IonisationChamberv(void)
{
#ifdef DEBUG
  cout << "IonisationChamberv::Destructor" << endl;
#endif
  
  PrintCounters();
}

void IonisationChamberv::PrintCounters(void)
{
#ifdef DEBUG
  cout << "IonisationChamberv::PrintCounters" << endl;
#endif
  
  cout << endl;
  cout << "IonisationChamberv::PrintCounters" << endl;
  cout << "Called: " << Counter[0] << endl;
  for(Int_t i=0;i<3;i++)
    cout << "E" << i << ": " << Counter[i+1] << endl;
  cout << "Present: " << Counter[4] << endl;

  
  L->Log << endl;
  L->Log << "IonisationChamberv::PrintCounters" << endl;
  L->Log << "Called: " << Counter[0] << endl;
  for(Int_t i=0;i<3;i++)
    L->Log << "E" << i << ": " << Counter[1] << endl;
  L->Log << "Present: " << Counter[4] << endl;

}


void IonisationChamberv::InitRaw(void)
{
#ifdef DEBUG
  cout << "IonisationChamberv::InitRaw" << endl;
#endif

  for(Int_t i=0;i<3;i++)
    for(Int_t j=0;j<8;j++)
      {
	E_Raw[i*8+j] = 0;
      	E_Raw_Nr[i*8+j] = 0;
      }
  E_RawM = 0;
}

void IonisationChamberv::Init(void)
{
#ifdef DEBUG
  cout << "IonisationChamberv::Init" << endl;
#endif
  Present = false; 

  ETotal = 0.0;
  EM = 0;
  eloss = 7.54; //Energy loss (MeV) for the elastic pic form the active zone of the IC
  
  for(Int_t i=0;i<3;i++)
    for(Int_t j=0;j<8;j++)
      {
	E[i*8+j] = 0.0;
      }

/*
  for(Int_t i =0;i<3;i++)
    ES[i] = 0.;
*/  
}

void IonisationChamberv::Calibrate(void)
{
  Int_t i,k;

#ifdef DEBUG
  cout << "IonisationChamberv::Calibrate" << endl;
#endif

  for(i=0;i<E_RawM;i++)
    {
    E[E_Raw_Nr[i]] = ((((Float_t) E_Raw[i] + Rnd->Value())*a[E_Raw_Nr[i]])+b[E_Raw_Nr[i]])*(eloss/Vnorm[E_Raw_Nr[i]]);	//The linear fit * k/Vnorm
    ETotal = E[E_Raw_Nr[i]];
    Number = int(E_Raw_Nr[i]);
    //L->Log<<"IC		E_raw : "<<E_Raw[i]<<"	# : "<<Number<<"	E : "<<ETotal<<endl;    		
    		//==Raw_signal class==
    		IC_Raw = E_Raw[i];
    		//====================
    EM++;
    }

/*
  int a,b;
  int MRow[3];

  MRow[0] = MRow[1] = MRow[2] = 0;
  for(i=0;i<E_RawM;i++)
    {
      a=(E_Raw_Nr[i]+1)/8;
      b=((E_Raw_Nr[i]+1)%8)-1;
      if(b!=-1)
	{
	  Rnd->Next();
	  for(k=0;k<3;k++)
	    {
	      E[E_Raw_Nr[i]] += powf((Float_t) E_Raw[i] + Rnd->Value(),
			    (Float_t) k)*ECoef[a][b][k];
	    }
	  E[E_Raw_Nr[i]] *= (ERef[a][1]/ERef[a][0]);
	  if(E[E_Raw_Nr[i]] > ETresh[a])
	    {
	      ES[a] += E[E_Raw_Nr[i]];
	      MRow[a] ++;
	      ETotal += E[E_Raw_Nr[i]];
	      //	      ENr[EM] = E_Raw_Nr[i];
	      Number = int(E_Raw_Nr[i]);
	      EM++;
	    }
	  
	}

    }
*/
 // for(i=0;i<3;i++)
   // if(ES[i] > 0) Counter[i+1]++;
  
 // if(ES[0] > 0. &&  ETotal > 0.0 )//&& MRow[0] == 1 && MRow[1] ==1 )
 if(ETotal > 0.0)
    {
      Present = true; 
      Counter[4]++;
    }
  else
      Init();
}

void IonisationChamberv::Treat(void)
{
#ifdef DEBUG
  cout << "IonisationChamberv::Treat" << endl;
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

void IonisationChamberv::inAttach(TTree *inT)
{

#ifdef DEBUG
  cout << "IonisationChamberv::inAttach" << endl;
#endif

#ifdef DEBUG
  cout << "Attaching IonisationChamberv variables" << endl;
#endif

#ifdef ACTIVEBRANCHES
  cout << "Activating Branches: IonisationChamberv" << endl;
  L->Log << "Activating Branches: IonisationChamberv" << endl;

  inT->SetBranchStatus("EchiM",1);
  inT->SetBranchStatus("Echi",1);
  inT->SetBranchStatus("EchiNr",1);

#endif

      inT->SetBranchAddress("EchiM",&E_RawM);
      inT->SetBranchAddress("Echi",E_Raw);
      inT->SetBranchAddress("EchiNr",E_Raw_Nr);

}
void IonisationChamberv::outAttach(TTree *outT)
{

#ifdef DEBUG
  cout << "IonisationChamberv::outAttach" << endl;
#endif

#ifdef DEBUG
  cout << "Attaching IonisationChamberv variables" << endl;
#endif
  //outT->Branch("IcEA",&ES[0],"IcEA/F");
  //outT->Branch("IcEB",&ES[1],"IcEB/F");
  //outT->Branch("IcEC",&ES[2],"IcEC/F");
  outT->Branch("IcET",&ETotal,"IcET/F");

  outT->Branch("IcM",&EM,"IcM/I");
  outT->Branch("IcE",E,"IcE[24]/F");
  
  outT->Branch("IcNb",&Number,"IcNb/I");

}
void IonisationChamberv::CreateHistograms(void)
{

#ifdef DEBUG
  cout << "IonisationChamberv::CreateHistograms" << endl;
#endif

}
void IonisationChamberv::FillHistograms(void)
{

#ifdef DEBUG
  cout << "IonisationChamberv::FillHistograms" << endl;
#endif

}


void IonisationChamberv::Show_Raw(void)
{
  Int_t i;
#ifdef DEBUG
  cout << "IonisationChamberv::Show_Raw" << endl;
#endif
  
  cout.setf(ios::showpoint);
  
int a,b;

 cout << "EEawM: " << E_RawM << endl; 
  for(i=0;i<E_RawM;i++)
    {
      a=(E_Raw_Nr[i]+1)/8;
      b=((E_Raw_Nr[i]+1)%8)-1;
      if(b!=-1)
	{
	  cout << E_Raw_Nr[i] << endl;
      cout << "PAD " << a << " " << b <<": ";
      cout << E_Raw[i] << " ";
      cout << endl;
	}    
    }


}
void IonisationChamberv::Show(void)
{
  Int_t i;
  int a,b;

#ifdef DEBUG
  cout << "IonisationChamberv::Show" << endl;
#endif

  cout.setf(ios::showpoint);
 
  cout << "Present: " << Present << endl;
  if(Present)
    {
      cout << "EM: " << EM << endl;
      for(i=0;i<24;i++)
	{
	  a=(i+1)/8;
	  b=((i+1)%8)-1;
	  if(b!=-1)
	    {
	      if(E[i] > 0) 
		{
		  cout << "ROW " << a << " PAD " << b << ": "; 
		  cout << E[i] <<endl;
		}	    
	    }
	}
      for(i=0;i<3;i++)
	//cout << "SUM ROW:" << i << " " << ES[i] << endl;
      cout << "ETotal: " << ETotal << endl;
    }
}



