//Created by KVClassFactory on Tue Aug 11 16:38:54 2009
//Author: marini

#include "CsIv.h"
#include <cmath>
#include "TRandom3.h"

ClassImp(CsIv)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>CsIv</h2>
<h4>CsI VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

CsIv::CsIv(LogFile *Log)
{

	L=Log;
	
  InitRaw();
  Init();
   // Default constructor


  //initialization of calibration parameter
  for(Int_t i=0;i<80;i++)
    {
      Ped[i][0]=-1.;
    }
  for(Int_t j=0;j<80;j++)
    {
      for(Int_t k=0;k<3;k++)
	{
	  ECoef[j][k]=-1.;
	}
    }

  //pedestal reading
   Int_t num=0;
   Float_t pied=0.;
   Float_t dummy1, dummy2, dummy3;
   TString sline;
   ifstream in2;


   //reading the piedestal for CsI
   in2.open("/sps/indra/ganil/mark/Ident/Param_pied_WCsI.out");
   //in2.open("$KVROOT/KVFiles/INDRA_e503/Param_pied_WCsI.out");
   if(in2!=NULL) L->Log<<"Reading CsI pedestal file..."<<endl;
   else L->Log<<"ERROR: CsI pedestal file not found!!"<<endl;
   while(!in2.eof()){
       sline.ReadLine(in2);
       if(!in2.eof()){
	   if (!sline.BeginsWith("#")){
	     sscanf(sline.Data(),"%d %f %f %f %f", &num, &dummy1, &dummy2, &dummy3, &pied);
	     Ped[num-1][0]=pied;	   
	     	     //L->Log<<"Ped["<<num-1<<"][0]="<<Ped[num-1][0]<<endl<<flush;
	   }
         }
       }
     //end of reading the piedestal for CsI

   in2.close();

   //    cout<<"piedestal= "<<pied<<endl;


   Int_t  num2;
   Float_t a1=0., a2=0., a3=0.;
   ifstream in3;

   //reading the CsI calibration parameters
   in3.open("/sps/indra/ganil/mark/Ident/new.dat");
   //in3.open("$KVROOT/KVFiles/INDRA_e503/new.dat");
   if(in3!=NULL) L->Log<<"Reading CsI calibration parameters..."<<endl;
   else L->Log<<"ERROR: CsI calibration file not found!!"<<endl;
     while(!in3.eof()){
       sline.ReadLine(in3);
       if(!in3.eof()){
	   if (!sline.BeginsWith("+")&&!sline.BeginsWith("|")){
	     sscanf(sline.Data(),"%d %d %f %f %f %f %f", &num, &num2, &a1, &a2, &a3, &dummy1, &dummy2);
	     ECoef[num2-1][0]=a1;
	     ECoef[num2-1][1]=a2;
	     ECoef[num2-1][2]=a3;
	     	     //L->Log<<"num "<<num2-1<<" a1= "<<a1<<"a2= "<<a2<<"a3= "<<a3<<endl; 
	   }
       }
     }
     //end of reading the calibration parameters of CsI
     
     in3.close();

 
}


CsIv::~CsIv()
{
   // Destructor
}


void CsIv::InitRaw(void)
{
  cout << "CsI::InitRaw" << endl;

  for(Int_t i=0;i<80;i++)
    {
      E_Raw[i] = 0;
      E_Raw_Nr[i] = 0;
      CsIRaw[i] = 0;
    }
  E_RawM = 0;
}


void CsIv::Init()
{
  cout<<"Init()"<<endl;

  //Initialization of calibrated parameter
  for(Int_t i=0;i<80;i++)
    {
      //E[i]=-100;
      ENr[i]=-1;
    }
  EM=0;
  ETotal=-1;
  Number=-10;
}

void CsIv::Calibrate(void)
{
  Int_t i,j,k;

  // #ifdef DEBUG
  //cout << "CSI::Calibrate" << endl;
  // #endif
  
    //cout<<"E_RawM "<<E_RawM<<endl;
  
  for(i=0;i<E_RawM;i++)
      {
	//cout<<"CsI : E_Raw["<<i<<"] = "<<E_Raw[i]<<endl;
	
		//cout<<"E_Raw_Nr[i] "<<E_Raw_Nr[i]<<endl;
	if(E_Raw > 0)
 	  {
	    //cout<<"E_Raw "<<E_Raw[i]<<endl;
	    Number = E_Raw_Nr[i];
	    EM++;
	  }
      }

  
}

void CsIv::Treat(void)
{
  //#ifdef DEBUG
  //cout << "CsIv::Treat" << endl;
  //#endif
  
    //  Init();
  Calibrate();
}

void CsIv::inAttach(TTree *inT)
{

#ifdef DEBUG
  cout << "CsI::inAttach" << endl;
#endif

#ifdef DEBUG
  cout << "Attaching CsI variables" << endl;
#endif

  inT->SetBranchAddress("WCSIEM",&E_RawM);
  inT->SetBranchAddress("WCSIE",E_Raw);
  inT->SetBranchAddress("WCSIENr",E_Raw_Nr);

}

void CsIv::outAttach(TTree *outT)
{

#ifdef DEBUG
  cout << "CsI::inAttach" << endl;
#endif

#ifdef DEBUG
  cout << "Attaching CsI variables" << endl;
#endif

   outT->Branch("CsIEM",&EM,"CsIEM/I");
   //outT->Branch("CsIE",E,"CsIE[80]/F");
   //outT->Branch("CsIET",&ETotal,"CsIET/F");

   //outT->Branch("CSIERaw",&E_Raw[0],"CsIERaw/S");
   //outT->Branch("CSIERaw",CsIRaw,"CsIERaw[80]/S");
}

void CsIv::CreateHistograms(void)
{

#ifdef DEBUG
  cout << "CsI::CreateHistograms" << endl;
#endif

}
void CsIv::FillHistograms(void)
{

#ifdef DEBUG
  cout << "CsI::FillHistograms" << endl;
#endif

}
