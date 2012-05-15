/*
 *	-----------------------------------------------------------------
 *	Siv
 *	Initial structure from Maurycy Rejmund (2006)
 *	-----------------------------------------------------------------
 */
#include "Siv.h"
#include "KVDataSet.h"
#include <cmath>

ClassImp(Siv)

Siv::Siv(LogFile *Log)
{
	#ifdef DEBUG
	cout << "Si::Constructor" << endl;
	#endif
	
	Ready=kFALSE;
  
	char line[255];
	int len=255;
	int i,j;

	L=Log;

	for(i=0;i<2;i++) Counter[i] = 0;

	InitRaw();
	Init();

	Rnd = new Random;
  
	ifstream inf;
  	if(!gDataSet->OpenDataSetFile("Si.cal",inf))
	{
		cout << "Could not open the calibration file Si.cal !" << endl;
		return;
	}
	else
	{
		cout.setf(ios::showpoint);
		cout << "Reading Si.cal" << endl; L->Log << "Reading Si.cal" << endl;		
/*
 *		Silicon calibration parameters
 */
		inf.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(i=0;i<18;i++) 
		{
			for(j=0;j<8;j++)  inf >> ECoef[i][j]; inf.getline(line,len);
/*
 *			Output control
 */
			printf("  silicon %2d: ",i+1);
			for(j=0;j<8;j++) printf("  %10.4g",ECoef[i][j]); cout << endl;	
		}
/*	
 *		Reading parameters for Time TSi_HF, 6 parameters a,b,c,d,e,f :
 */
 		inf.getline(line,len);
 		cout << line << endl; L->Log << line << endl;
 		for(i=0;i<6;i++) inf >> TSi_HF_Coef[i];
 		for(i=0;i<6;i++) printf("  %10.4g",TSi_HF_Coef[i]); cout << endl ;  
/*	
 *		Reading t0 for Time TSi_HF, 18 values, one per silicon :
 */
		for(i=0;i<2;i++) inf.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(i=0;i<18;i++) inf >> TSi_HF_t0[i];
		for(i=0;i<18;i++) printf("  silicon %2d: %10.4g\n",i+1,TSi_HF_t0[i]);
/*	
 *		Reading parameters for Time TSi_SED1, 4 parameters a,b,c,d :
 */
		for(i=0;i<2;i++) inf.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(i=0;i<4;i++) inf >> TSi_SED1_Coef[i];
		for(i=0;i<4;i++) printf("  %10.4g",TSi_SED1_Coef[i]); cout << endl ;  
/*	
 *		Reading t0 for Time TSi_SED1, 18 values, one per silicon :
 */
		for(i=0;i<2;i++) inf.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(i=0;i<18;i++) inf >> TSi_SED1_t0[i];
		for(i=0;i<18;i++) printf("  silicon %2d: %10.4g\n",i+1,TSi_SED1_t0[i]);
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

	for(Int_t i=0;i<21;i++) E_Raw[i] = 0;
	for(Int_t i=0;i<21;i++) E_Raw_Nr[i] = 0;
	for(Int_t i=0;i<3;i++) T_Raw[i] = 0;
	E_RawM = 0;
}

void Siv::Init(void)
{
	#ifdef DEBUG
	cout << "Siv::Init" << endl;
	#endif
	
	Present = false; 
	for(Int_t i=0;i<21;i++) E[i] = 0.0;
	for(Int_t i=0;i<3;i++) T[i] = 0.0;
	EM = 0;
	ETotal = 0.0;
	Number = 0;
}

void Siv::Calibrate(void)
{
	Int_t i,j,k;
	double x,a,b,c,d,e,f,g,h,t0;

	#ifdef DEBUG
	cout << "Si::Calibrate" << endl;
	#endif

	for(i=0;i<3;i++) if(T_Raw[i] > 200) for(j=0;j<2;j++) 
		T[i] = (Float_t) T_Raw[i];
	
	for(i=0;i<E_RawM;i++)
	{
		cout << "  Telescope, Mult : "  << Number << " , "<< E_RawM << endl;  
		Rnd->Next();
/*
 *		Calibration of silicon detectors	
 *		Energy calibration with Fermi functions
 */	
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
		cout << "    ESi[ch], [MeV] : "<< E_Raw[i] << " , "  << E[E_Raw_Nr[i]] << endl;  
		
		if(E[E_Raw_Nr[i]] > 0)
		{
			ETotal += E[E_Raw_Nr[i]];
			Number = E_Raw_Nr[i]+1;
			EM++;
		}
/*
 *		Calibration of time of flight (TSi_HF)
 *		T[ns]= t0 - (a+(b+(c+(d+(e+f*x)*x)*x)*x)*x)*x
 */
		x = (double)T_Raw[0]+(double)Rnd->Value() ;
		a = (double)TSi_HF_Coef[0] ;
		b = (double)TSi_HF_Coef[1] ;
		c = (double)TSi_HF_Coef[2] ;
		d = (double)TSi_HF_Coef[3] ;
		e = (double)TSi_HF_Coef[4] ;
		f = (double)TSi_HF_Coef[5] ;
		t0= (double)TSi_HF_t0[E_Raw_Nr[i]] ;

		T[0] = (Float_t)( t0 -(a+(b+(c+(d+(e+f*x)*x)*x)*x)*x)*x );	
		cout << "    TSi_HF[ch], [ns] : "<< T_Raw[0] << " , "  << T[0] << endl;  
/*
 *		Calibration of time of flight (TSi_SED1)
 *		T[ns]= t0 - (a*x+b*x*x+c*exp(d*x))
 */
		x = (double)T_Raw[1]+(double)Rnd->Value() ;
		a = (double)TSi_SED1_Coef[0] ;
		b = (double)TSi_SED1_Coef[1] ;
		c = (double)TSi_SED1_Coef[2] ;
		d = (double)TSi_SED1_Coef[3] ;
		t0= (double)TSi_SED1_t0[E_Raw_Nr[i]] ;

		T[1] = (Float_t)( t0 - (a*x+b*x*x+c*exp(d*x)) );	
		cout << "    TSi_SED1[ch], [ns] : "<< T_Raw[1] << " , "  << T[1] << endl;  
	}
/*
 *	Output control
 */
	if(E_RawM==0) cout<<"No signal "<<endl;  

	if(EM!=1) 
	{
		ETotal = 0.;
		Number = 0;
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
	cout << "Attaching Si variables" << endl;
	#endif

	#ifdef ACTIVEBRANCHES
	cout << "Activating Branches: Si" << endl;
	L->Log << "Activating Branches: Si" << endl;
	inT->SetBranchStatus("SIENr",  1);
	inT->SetBranchStatus("SIE",    1);
	inT->SetBranchStatus("SIEM",   1);
	inT->SetBranchStatus("TSI_HF", 1);
	inT->SetBranchStatus("TSI_SED",1);
	inT->SetBranchStatus("TSED_HF",1);
	#endif

	inT->SetBranchAddress("SIENr",  E_Raw_Nr);
	inT->SetBranchAddress("SIE",    E_Raw   );
	inT->SetBranchAddress("SIEM",   &E_RawM );
	inT->SetBranchAddress("TSI_HF", T_Raw+0 );
	inT->SetBranchAddress("TSI_SED",T_Raw+1 );
	inT->SetBranchAddress("TSED_HF",T_Raw+2 );
}

void Siv::outAttach(TTree *outT)
{
	#ifdef DEBUG
	cout << "Si::outAttach" << endl;
	#endif

	#ifdef DEBUG
	cout << "Attaching Si variables" << endl;
	#endif

	outT->Branch("SIE_Nr", &Number,"SIE_Nr/s"     );
	outT->Branch("SiE",    E,      "SiE[21]/F"    );
	outT->Branch("SiEM",   &EM,    "SiEM/I"       );
//	outT->Branch("SiET",   &ETotal,"SiET/F"       );
//	outT->Branch("SIE_Raw",E_Raw,  "SiE_Raw[21]/s");
	outT->Branch("TSi_HF", &T[0],  "TSi_HF/F"     );
	outT->Branch("TSi_SeD",&T[1],  "TSi_SeD/F"    );
//	outT->Branch("TSeD_HF",&T[2],  "TSeD_HF/F"    );
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
		cout << "CrER: " << E_Raw_Nr[i] << " " << E_Raw[i] << endl; 
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
		for(i=0;i<21;i++) if(E[i]>0.) 
			cout << "SiE: " << i << " " << E[i] << endl; 
	}
}



