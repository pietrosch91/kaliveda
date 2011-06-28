#include "Identificationv.h"
#include "TMath.h"
#include <cmath>
#include <iostream>
#include "EnergyTree.h"
#include "KVNucleus.h"
#include "KVDetector.h"

#include "KVIDZAGrid.h"
#include "KVIDGridManager.h"

//Author: Maurycy Rejmund
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
				 DriftChamberv *Drift, IonisationChamberv *IonCh, Siv *SiD, CsIv *CsID, EnergyTree *E)
{
  L = Log;
  Rec = Recon;
  Dr = Drift;
  Ic = IonCh;
  Si = SiD;
  CsI=CsID;
  
  energytree = E;
  	
  //for(i=0;i<6;i++)
    //Counter[i] = 0;

  Init();

  Rnd = new Random; 
}

Identificationv::~Identificationv(void)
{

delete Rnd;
delete id;
}

void Identificationv::Init(void)
{
  Present = false; 

  dE = dE1 = E = T = V = M_Q = M = Z1 = Z2 = Z_tot = Z_si =  Beta = Q = D = -10;
  M_Qr = Mr = Qr = -10.0;
  Qc = Mc = -10.0;
  Gamma = 1.; 
 initThickness=0.;      
zt = ZZ = AA = CsIRaw = SiRaw = DetSi = DetCsI = i =  -10;
ESi = ECsI = EEtot = AA2 = ZR = -10.0;
PID = Z_PID = A_PID = -10.0;

    runNumber = 0;
    runNumber = (Int_t)gIndra->GetCurrentRunNumber();
}



void Identificationv::SetBrho(Double_t bbrho)
{
brho=bbrho;
}

Double_t Identificationv::GetBrho(void)
{
return brho;
}

void Identificationv::SetRunFlag(Int_t rrunFlag)
{
runFlag=rrunFlag;
}

Int_t Identificationv::GetRunFlag(void)
{
return runFlag;
}

void Identificationv::Calculate(void)
{

/*if(GetRunFlag() == 0){
	runFlag = 1;
	L->Log<<"***RunFlag : ***"<<runFlag<<endl;
	//L->Log<<"***Focal Plan***"<<endl;
	//GetFocalPlan()->Print();
	//KVSeqCollection* l;
	//l = v->GetListOfIDTelescopes();
	//L->Log<<"***	***"<<endl;
	
	energytree->SetSiliconThickness(int(Si->Number));
	initThickness = Si->si_thick[int(Si->Number)];
}else{
	if(Si->si_thick[int(Si->Number)]!=initThickness && initThickness!=0.){
		initThickness = Si->si_thick[int(Si->Number)];
		energytree->SetSiliconThickness(int(Si->Number));
	}
}*/

if(Geometry(Si->Number,CsI->Number)==1 && ((Si->Number!=0 && CsI->Number!=0) ||(Si->Number==0 && CsI->Number==0))) // if csi is behind the si
{ 
energytree->InitTelescope(energytree->GetFocalPlan(),Si->Number,CsI->Number);
energytree->SetCalibration(Si,CsI,Si->Number,CsI->Number);
}	

  //energytree->SetSiliconThickness(int(Si->Number));		//Initialise the Si-gap-CsI telescope (numérotation : 0-17)
  
  L->Log<<"num si (0-17)=	"<<int(Si->Number)<<endl;
  L->Log<<"num csi(0-79)=	"<<int(CsI->Number)<<endl;	    
 // L->Log<<"Thick : "<<energytree->thick<<" "<<"Det. Nb. : "<<Si->Number<<endl;
  
 //energytree->SetCalibration(Si,CsI,Si->Number,CsI->Number);	//Apply the calibration parameters for the Si and the CsI
	
  for(Int_t y=0;y< Si->E_RawM ;y++)	 
    {
      for(Int_t j=0;j< CsI->E_RawM;j++)	
	{
	  if(Geometry(Si->Number,CsI->Number)==1) // if csi is behind the si
	    { 
	      CsIRaw = int(CsI->E_Raw[j]);
	      SiRaw = int(Si->E_Raw[y]);
	      L->Log<<"CsIRaw	= "<<CsIRaw<<endl;
	      L->Log<<"SiRaw	= "<<SiRaw<<endl;
	            
	      //CsI->CsIRaw[CsI->Number] = CsI->E_Raw[j]; //Associer le canal au # du détecteur CsI
	      //Si->SiRaw[Si->Number] = Si->E_Raw[y];	 //Associer le canal au # du détecteur Si
			
		Double_t min3,max3;
  		min3 = max3 = abs(dif1[0]);
		
		Double_t min11,max11;
  		min11 = max11 = abs(dif11[0]);
		
		Double_t min12,max12;
  		min12 = max12 = abs(dif12[0]);
				
		Double_t min4,max4;
  		min4 = max4 = abs(dif2[0]);
		
		//energytree->eEnergySi=Si->ETotal;
			
	      for(energytree->eZ=3;energytree->eZ<24;energytree->eZ++)
	      {
	      energytree->SetFragmentZ(energytree->eZ);
	      energytree->GetResidualEnergyCsI(Si->E_Raw[y],CsI->E_Raw[j]);
	      
		dif1[energytree->eZ-3]=energytree->diffsi;
		dif2[energytree->eZ-3]=energytree->diffcsi;
		
		As[energytree->eZ-3]=energytree->sA;		
		SiRef[energytree->eZ-3] = energytree->eEnergySi;
		ARetreive[energytree->eZ-3]=energytree->RetrieveA();
		CsIsRef[energytree->eZ-3] = energytree->eEnergyCsI;				//energytree->sRefECsI;
			
		if(abs(dif1[energytree->eZ-3]) < min3)
      			min3 = abs(dif1[energytree->eZ-3]); //Calcul du min et max de la diff. d'énergie du Si
    		else
      			max3 = abs(dif1[energytree->eZ-3]);
		
		if(abs(dif2[energytree->eZ-3]) < min4)
      			min4 = abs(dif2[energytree->eZ-3]); //Calcul du min et max de la diff. d'énergie du CsI
    		else
      			max4 = abs(dif2[energytree->eZ-3]);						
	      }
		
  		for(zt=0;zt<21;zt++)				
		{	
			if(abs(dif1[zt])== min3)		//Minimizing the silicon energy
			//if(abs(dif2[zt])== min4)		//Minimizing the cesium iodide energy
				break;		
		}
		
		//if((ARetreive[zt]<=41 && 455<=gIndra->GetCurrentRunNumber()<593) || (ARetreive[zt]<=49 && 379<=gIndra->GetCurrentRunNumber()<424))
		//{
		L->Log<<"==========================="<<endl;
		L->Log<<"dif1(Si)	= "<<dif1[zt]<<endl;
		L->Log<<"dif2(CsI)	= "<<dif2[zt]<<endl;
		
		L->Log<<"eZ	= "<<zt+3<<"	Zreel	= "<<((energytree->eEnergySi)*(zt+3))/(energytree->eEnergySi-dif1[zt])<<endl;		//(energytree->eEnergySi-dif1[zt])<<endl;	
		L->Log<<"sigma	= "<<(zt+4)-(((energytree->eEnergySi)*(zt+4))/(energytree->eEnergySi-dif1[zt]))<<endl;
		
		L->Log<<"A	= "<<ARetreive[zt]<<endl;
		L->Log<<"sA	= "<<As[zt]<<endl;
		L->Log<<"Esi	= "<<float(SiRef[zt])<<endl;
		L->Log<<"Ecsi	= "<<CsIsRef[zt]<<endl;
		L->Log<<"E tot	= "<<double(SiRef[zt]+CsIsRef[zt])<<endl;
		L->Log<<"==========================="<<endl;		
				
	        ECsI = double(CsIsRef[zt]);
		ESi = double(SiRef[zt]);
		ZZ = zt+3;
		ZR = ((energytree->eEnergySi)*(zt+3))/(energytree->eEnergySi-dif1[zt]);
		//ZR = ((energytree->eEnergySi)*(zt+3))/(energytree->sEnergySi);
		AA = int(ARetreive[zt]);
		AA2 = ARetreive[zt];											
		DetCsI = int(CsI->Number)+1;	// Numérotation : (1-80)
		DetSi = int(Si->Number)+1;	// Numérotation : (1-18)
		
		L->Log<<"name : "<<energytree->kvid->GetName()<<endl;
				    
		//if(!(KVIDZAGrid*)gIDGridManager->GetGrid(energytree->kvid->GetName()))
		if(!(KVIDZAGrid*)energytree->kvid->GetListOfIDGrids()->At(0))
		{
		L->Log<<"NO GRID..."<<endl;
		}
		else
		{
		/*
		cout<<"==============="<<endl;	    
		KVIDZAGrid* idg = (KVIDZAGrid* )energytree->kvid->GetListOfIDGrids()->At(0);
            	idg->Print();
		cout<<"==============="<<endl;
		*/
		id = new KVIdentificationResult();
		
		Double_t x = ESi;
		Double_t y = double(CsI->E_Raw[j]);

		energytree->kvid->Identify(x, y, id);
		
		A_PID = id->A;
		Z_PID = id->Z;
		PID = id->PID;

		L->Log<<"A : "<<id->A<<endl;
		L->Log<<"Z : "<<id->Z<<endl;
		L->Log<<"PID : "<<id->PID<<endl;
		L->Log<<"Zident : "<<id->Zident<<endl;
		L->Log<<"Aident : "<<id->Aident<<endl;		
		L->Log<<"==========================="<<endl;
		}
														
		//}
		
		/*if((ARetreive[zt]>41 && 455<=gIndra->GetCurrentRunNumber()<593) || (ARetreive[zt]>49 && 379<=gIndra->GetCurrentRunNumber()<424))
		{
		
		for(Int_t a=5;a<60;a++)
		{
		energytree->SetFragmentZ(zt+4);	//Set Z at : eZ+1
		energytree->SetFragmentA(a);
		energytree->DoIt(Si->E_Raw[y],CsI->E_Raw[j],a);
		
		dif11[a-5]=energytree->diffsi;
		dif12[a-5]=energytree->diffcsi;
		As11[a-5]=energytree->sA;		
		SiRef11[a-5] = energytree->eEnergySi;
		ARetreive11[a-5]=energytree->RetrieveA();
		CsIsRef11[a-5] = energytree->eEnergyCsI;
		
		if(abs(dif11[a-5]) < min11)
      			min11 = abs(dif11[a-5]);
    		else
      			max11 = abs(dif11[a-5]);
		
		if(abs(dif12[a-5]) < min12)
      			min12 = abs(dif12[a-5]);
    		else
      			max12 = abs(dif12[a-5]);
		}
		
		for(aa=0;aa<55;aa++)
		{
			if(abs(dif11[aa])== min11)
				break;
		}
		L->Log<<"==========================="<<endl;
		L->Log<<"dif11(Si)	= "<<dif11[aa]<<endl;
		L->Log<<"dif12(Csi)	= "<<dif12[aa]<<endl;
		
		L->Log<<"eZ	= "<<zt+4<<"	Zreel	= "<<((energytree->eEnergySi)*(zt+4))/(energytree->eEnergySi-dif11[aa])<<endl;	
		L->Log<<"sigma	= "<<(zt+4)-(((energytree->eEnergySi)*(zt+4))/(energytree->eEnergySi-dif11[aa]))<<endl;
				
		L->Log<<"aa	= "<<aa+5<<endl;
		L->Log<<"sA	= "<<As11[aa]<<endl;
		L->Log<<"Esi	= "<<float(SiRef11[aa])<<endl;
		L->Log<<"Ecsi	= "<<CsIsRef11[aa]<<endl;
		L->Log<<"E tot	= "<<double(SiRef[zt]+CsIsRef[zt])<<endl;
		 
		ECsI = double(CsIsRef11[aa]);
		ESi = double(SiRef11[aa]);
		ZZ = zt+4;
		ZR = ((energytree->eEnergySi)*(zt+4))/(energytree->eEnergySi-dif11[aa]);
		AA = aa+5;
		AA2 = ARetreive11[aa];	
		DetCsI = int(CsI->Number)+1; 
		DetSi = int(Si->Number)+1;				
		}*/		
				
	    }
	}
    }
	
  L->Log<<"Dr->E[0] : "<<Dr->E[0]<<" "<<"Dr->E[1] : "<<Dr->E[1]<<" "<<"Ic->ETotal : "<<Ic->ETotal<<endl;
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
      if((dE1+ESi+ECsI)>0)	//if(Si->ETotal > 0)		//Originalement : if(Si->ETotal>0)
	//E = (dE/1000) + (ESi+ECsI)*0.99;
	E = dE1 + ESi + ECsI;		//Total energy (MeV)	dE1+ESi+ECsI
	E *= 1.03;		//Correction for the different dead layers (about 3%)
	//L->Log<<"dE1 = "<<dE1<<endl;
	L->Log<<"dE1	(MeV)= "<<dE1<<endl;
	L->Log<<"E	(MeV)= "<<E<<endl;
	L->Log<<"ESi	(MeV)= "<<ESi<<"	ECsI	(MeV)= "<<ECsI<<"	Ic	(MeV)= "<<dE1<<endl;
    }

/*
  if(Si->T[1] >0. && Si->T[2] >0.)
    {
      if(Si->T[2] < Si->T[1] * 1.4+82.)
 	T = Si->T[2] + 116.69;		//116.69 : Fréquence HF
      else
 	T = Si->T[2]; 
      T -= 3.;
    }
*/

T = Si->T[0];

  if(T >0 && Rec->Path>0 && Dr->Present)
    {
      //Distance between silicon and the target in cm	
      //D = Rec->Path + (-1.*(Dr->Yf)/10.*sin(3.14159/4.)/ cos(3.14159/4. + fabs(Dr->Pf/1000.)))/cos(Dr->Tf/1000.);	//Distance correction from the target to the SED
      
      D = (Rec->Path + (72.05/cos(Dr->Tf/1000.)))/cos(Dr->Pf/1000.);		//Distance correction from the target to the silicon wall

      V = D/T;		//Velocity given in cm/ns
      V = V+ V*(1-cos(Dr->Tf/1000.)*cos(Dr->Pf/1000.));	//Velocity correction based on the angles 
      Beta =V/29.9792; 
      Gamma = 1./sqrt(1.-TMath::Power(Beta,2.));
      //L->Log<<"D = "<<D<<" Rec->Path = "<<Rec->Path<<" D-Path = "<<(-1.*(Dr->Yf)/10.*sin(3.14159/4.)/cos(3.14159/4. + fabs(Dr->Pf/1000.)))/cos(Dr->Tf/1000.)<<endl;
      L->Log<<"D = "<<D<<"	V = "<<V<<"	Beta = "<<Beta<<"	D/T = "<<D/T<<endl;
      //L->Log<<"brho = "<<Rec->GetBrhoRef()<<"	angle = "<<Rec->GetAngleVamos()<<endl;
    }

  if(Beta>0 && Rec->Brho>0&&Gamma>1.&&Si->Present)
    {
      M = 2.* E / 931.5016/TMath::Power(Beta,2.);
      M_Q = Rec->Brho/3.105/Beta;
      L->Log<<"M = "<<M<<endl;
      L->Log<<"M/Q = "<<M_Q<<endl;

      Mr = (E/1000.)/931.5016/(Gamma-1.);
      M_Qr = Rec->Brho/3.105/Beta/Gamma;

      Q = M/M_Q;
      Qr = Mr/M_Qr;
      Qr = (-1.036820+1.042380*Qr +0.4801678e-03*Qr*Qr);
      Qc = int(Qr+0.5);
      Mc = M_Qr*Qc;
    }
/*  
  Z1 = sqrt(dE1*E/pow(931.5016,2.))/pow(29.9792,2.)*100.;
  Z2 = sqrt(dE/931.5016)*TMath::Power(Beta,2.)*100.;
  
  Z_tot = sqrt(E/931.5016)*TMath::Power(Beta,2.);
  Z_si = sqrt(ESi/931.5016)*TMath::Power(Beta,2.);
L->Log<<"Z_tot = "<<Z_tot<<" 		Z_si = "<<Z_si<<endl;
*/
  Z1 = (sqrt((dE1*E)/931.5016)*29.9792)/7;		//(1/7) : Correction for the mass (1/sqrt(A))
  Z2 = sqrt(dE/931.5016)*TMath::Power(Beta,2.)*100.;
  
    Z_tot = (sqrt((ESi*E)/931.5016)*29.9792)/7;
    Z_si = (sqrt((dE1*E)/931.5016)*29.9792)/7;
    
NormVamos = gIndraDB->GetRun(gIndra->GetCurrentRunNumber())->Get("NormVamos");
    
L->Log<<"Z1 = "<<Z1<<" 		Z2 = "<<Z2<<endl;
L->Log<<"Z_tot = "<<Z_tot<<" 		Z_si = "<<Z_si<<endl;
  if( T > 0 && V > 0 && M_Q > 0 && M > 0 && Z1 > 0 && Z2 > 0 && Beta > 0 && Gamma > 1.0) 
    {
      Present=true;
      Counter[2]++;
    }
    energytree->ClearEvent(runFlag);
}


void Identificationv::Treat(void)
{
#ifdef DEBUG
  cout << "Identificationv::Treat" << endl;
#endif

  //Counter[0]++;
  Init();
  //if(Rec->Present) Counter[1]++;		//Non commenter
  Calculate();
  //#ifdef DEBUG
  //Show();
  //#endif
  
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
    outT->Branch("RunNumber", &runNumber, "runNumber/I");
	outT->Branch("A",&AA,"A/I");
	//outT->Branch("A2",&AA2,"A2/F");
   	outT->Branch("Z",&ZZ,"Z/I");
	outT->Branch("ZR",&ZR,"ZR/D");
	outT->Branch("ESiRaw",&SiRaw,"SiRaw/I");
	outT->Branch("ECsIRaw",&CsIRaw,"CsIRaw/I");
	outT->Branch("ESi",&ESi,"ESi/D");
	outT->Branch("ECsI",&ECsI,"ECsI/D");
	outT->Branch("DetSi",&DetSi,"DetSi/I");
	outT->Branch("DetCsI",&DetCsI,"DetCsI/I");
	outT->Branch("NormVamos",&NormVamos,"NormVamos/D");	
	
	outT->Branch("Z_PID",&Z_PID,"Z_PID/D");
	outT->Branch("A_PID",&A_PID,"A_PID/D");
	outT->Branch("PID",&PID,"PID/D");
			
	//outT->Branch("Q",&Q,"Q/F");
  	//outT->Branch("M",&M,"M/F");
	//outT->Branch("M_Q",&M_Q,"M_Q/F");
  	//outT->Branch("CsIE",ECsI,"ECsI[80]/F");
	//outT->Branch("SiE",ESi,"ESi[21]/F");

  //outT->Branch("dE",&dE,"dE/F");
  //outT->Branch("dE1",&dE1,"dE1/F");
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
  
  /*
  outT->Branch("Z1",&Z1,"Z1/F");
  outT->Branch("Z2",&Z2,"Z2/F");
  outT->Branch("Z_tot",&Z_tot,"Z_tot/F");
  outT->Branch("Z_si",&Z_si,"Z_si/F");
  */
/*  
#ifdef FOLLOWPEAKS
  outT->Branch("M_Qcorr",&M_Qcorr,"M_Qcorr/F");
  outT->Branch("Mcorr",&Mcorr,"Mcorr/F");
  outT->Branch("M_Qcorr1",&M_Qcorr1,"M_Qcorr1/F");
  outT->Branch("Mcorr1",&Mcorr1,"Mcorr1/F");
#endif
*/
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
  cout << "Identificationv::Show__prova" << endl;
 #endif
  cout.setf(ios::showpoint);
 
}


//temporary method to reconstruct VAMOS telescopes
int Identificationv::Geometry(UShort_t sinum, UShort_t csinum)
{
  //int geom[18][6];
  //Int_t geom[18][6];
  
   Int_t  num;
   Int_t csi1=0, csi2=0, csi3=0, csi4=0, csi5=0, csi6=0;
   ifstream in;
   TString sline;
   
   if(!gDataSet->OpenDataSetFile("geom.dat",in))
  {
     cout << "Could not open the calibration file geom.dat !!!" << endl;
     return 0;
  }
  else 
  {
	while(!in.eof()){
       sline.ReadLine(in);
       if(!in.eof()){
	   if (!sline.BeginsWith("#")){
	     sscanf(sline.Data(),"%d %d %d %d %d %d", &num, &csi1, &csi2, &csi3, &csi4, &csi5, &csi6);
	     geom[num][0]=csi1;
	     geom[num][1]=csi2;
	     geom[num][2]=csi3;
	     geom[num][3]=csi4;
	     geom[num][4]=csi5;
	     geom[num][5]=csi6;
	     	   }
       		}
     	}
  }
  in.close();

/*  
  geom[0][0]=0;
  geom[0][1]=1;
  geom[0][2]=12;
  geom[0][3]=13;
  geom[0][4]=24;
  geom[0][5]=25;

  geom[1][0]=2;
  geom[1][1]=3;
  geom[1][2]=14;
  geom[1][3]=15;
  geom[1][4]=26;
  geom[1][5]=27;

  geom[2][0]=4;
  geom[2][1]=5;
  geom[2][2]=16;
  geom[2][3]=17;
  geom[2][4]=28;
  geom[2][5]=29;

  geom[3][0]=6;
  geom[3][1]=7;
  geom[3][2]=18;
  geom[3][3]=19;
  geom[3][4]=30;
  geom[3][5]=31;

  geom[4][0]=8;
  geom[4][1]=9;
  geom[4][2]=20;
  geom[4][3]=21;
  geom[4][4]=32;
  geom[4][5]=33;

  geom[5][0]=10;
  geom[5][1]=11;
  geom[5][2]=22;
  geom[5][3]=23;
  geom[5][4]=34;
  geom[5][5]=35;

  geom[6][0]=36;
  geom[6][1]=37;
  geom[6][2]=-1;
  geom[6][3]=-1;
  geom[6][4]=-1;
  geom[6][5]=-1;

  geom[7][0]=38;
  geom[7][1]=39;
  geom[7][2]=-1;
  geom[7][3]=-1;
  geom[7][4]=-1;
  geom[7][5]=-1;

  geom[8][0]=-1;
  geom[8][1]=-1;
  geom[8][2]=-1;
  geom[8][3]=-1;
  geom[8][4]=-1;
  geom[8][5]=-1;

  geom[9][0]=-1;
  geom[9][1]=-1;
  geom[9][2]=-1;
  geom[9][3]=-1;
  geom[9][4]=-1;
  geom[9][5]=-1;

  geom[10][0]=54;
  geom[10][1]=55;
  geom[10][2]=-1;
  geom[10][3]=-1;
  geom[10][4]=-1;
  geom[10][5]=-1;

  geom[11][0]=52;
  geom[11][1]=53;
  geom[11][2]=-1;
  geom[11][3]=-1;
  geom[11][4]=-1;
  geom[11][5]=-1;

  geom[12][0]=50;
  geom[12][1]=51;
  geom[12][2]=66;
  geom[12][3]=67;
  geom[12][4]=78;
  geom[12][5]=79;

  geom[13][0]=48;
  geom[13][1]=49;
  geom[13][2]=64;
  geom[13][3]=65;
  geom[13][4]=76;
  geom[13][5]=77;

  geom[14][0]=46;
  geom[14][1]=47;
  geom[14][2]=62;
  geom[14][3]=63;
  geom[14][4]=74;
  geom[14][5]=75;

  geom[15][0]=44;
  geom[15][1]=45;
  geom[15][2]=60;
  geom[15][3]=61;
  geom[15][4]=72;
  geom[15][5]=73;

  geom[16][0]=42;
  geom[16][1]=43;
  geom[16][2]=58;
  geom[16][3]=59;
  geom[16][4]=70;
  geom[16][5]=71;

  geom[17][0]=40;
  geom[17][1]=41;
  geom[17][2]=56;
  geom[17][3]=57;
  geom[17][4]=68;
  geom[17][5]=69;
*/

  for(int i=0;i<6;i++)
    {
      if(geom[sinum][i]==csinum)return 1;
    }
  return 0;
}


//-------------------------------------------------------------------

/*void Identificationv::SetSiliconThickness(Int_t number)	//Si->Number goes to 0 to 17
{
	InitSiCsI(number+1);	//InitSiCsI(#)	(1-18)
}*/

//void Identificationv::InitSiCsI(Int_t number) // Si-CsI Telescope
//{

   /********************************************************************************
    TELESCOPE LAYOUT: Using custom built classes IonisationChamber and PlaneAbsorber 

    beam >>  | Silicon | C4H10 (Gap) | CsI 

    ********************************************************************************/
 
    /*si = new KVSiliconVamos(Si->si_thick[number]);
    gap = new PlaneAbsorber();
    csi = new KVCsIVamos(1.);*/

    // Remember they are of class 'PlaneAbsorber'
    // see header files for list of methods that can be called
     //L->Log<<"Silicon Thickness	: "<<si_thick[number]<<endl;

    /*gap->SetThickness(136.5,"NORM");
    gap->SetMaterial("C4H10");
    gap->SetPressure(40.);*/

    // Build the Telescope 
    // Need to use the 'GetDetector()' method as they are not
    // of type KVDetector

    /*kvt_sicsi = new KVTelescope();
    //KVTelescope kvt_sicsi();
    kvt_sicsi->Add(si);
    kvt_sicsi->Add(gap->GetDetector());  // In-active so no 'detected' energy
    kvt_sicsi->Add(csi);*/
    
    //lum=new KVLightEnergyCsIVamos(csi);
    //lum=new KVLightEnergyCsI(csi);
//}
