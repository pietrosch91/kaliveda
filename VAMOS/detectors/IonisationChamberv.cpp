/*
 *	-----------------------------------------------------------------
 *	IonisationChamberv
 *	Initial structure from Maurycy Rejmund (2006)
 *	-----------------------------------------------------------------
 */
#include "IonisationChamberv.h"
#include "KVDataSet.h"
#include <cmath>

using namespace std;

ClassImp(IonisationChamberv)

IonisationChamberv::IonisationChamberv(LogFile *Log)
{
#	ifdef DEBUG
	cout << "IonisationChamberv::Constructor" << endl;
#	endif
	
	Ready=kFALSE;
  
	char line[255];
	int len=255;
	int i,j,k;

	L=Log;

	for(i=0;i<5;i++) Counter[i] = 0;

	InitRaw();
	Init();

	Rnd = new Random;
  
	ifstream inf;
  
	if(!gDataSet->OpenDataSetFile("IonisationChamber.cal",inf))
	{
		cout << "Could not open the calibration file IonisationChamber.cal !" << endl;
		return;
	}
	else
	{
		cout.setf(ios::showpoint);
		cout << "Reading IonisationChamber.cal" << endl; L->Log << "Reading IonisationChamber.cal" << endl;
		for(i=0;i<3;i++) 
		{
			inf.getline(line,len);
			cout << line << endl; L->Log << line << endl;
			for(j=0;j<7;j++) 
			{
				for(k=0;k<4;k++) inf >> ECoef[i][j][k];
				inf.getline(line,len);
/*
 *				Output control
 */
				if(i==1) {for(k=0;k<4;k++) printf("  %10.4g",ECoef[i][j][k]); cout << endl;} 
			}
		}
		for(j=0;j<3;j++) 
		{
			inf.getline(line,len);
			cout << line << endl; L->Log << line << endl;
			for(k=0;k<2;k++) inf >> ERef[j][k]; 
			inf.getline(line,len);
		}
		for(j=0;j<3;j++) 
		{
			inf.getline(line,len);
			cout << line << endl; L->Log << line << endl;
			inf >> ETresh[j]; 
			cout << "  ETresh " << ETresh[j] << endl;
			inf.getline(line,len);
		}
	}
	inf.close();
	Ready=kTRUE;
}

IonisationChamberv::~IonisationChamberv(void)
{
#	ifdef DEBUG
	cout << "IonisationChamberv::Destructor" << endl;
#	endif
  
	PrintCounters();
}

void IonisationChamberv::PrintCounters(void)
{
#	ifdef DEBUG
	cout << "IonisationChamberv::PrintCounters" << endl;
#	endif
  
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
#	ifdef DEBUG
	cout << "IonisationChamberv::InitRaw" << endl;
#	endif
	
	for(Int_t i=0;i<3;i++) for(Int_t j=0;j<8;j++)
	{
		E_Raw[i*8+j] = 0;
		E_Raw_Nr[i*8+j] = 0;
	}
	E_RawM = 0;
}

void IonisationChamberv::Init(void)
{
#	ifdef DEBUG
	cout << "IonisationChamberv::Init" << endl;
#	endif
	
	Present = false; 
	EM = 0;
	ETotal = 0.0;
	Number = 0;
	for(Int_t i=0;i<3;i++) for(Int_t j=0;j<8;j++) E[i*8+j] = 0.0;
	for(Int_t i =0;i<3;i++) ES[i] = 0.;
}

void IonisationChamberv::Calibrate(void)
{
	double x,a,b,c,d;
	int aa,bb;
	int MRow[3];

#	ifdef DEBUG
	cout << "IonisationChamberv::Calibrate" << endl;
#	endif

	MRow[0] = MRow[1] = MRow[2] = 0;

	for(Int_t i=0;i<E_RawM;i++)
	{
/*
 *		We only take the stage 2 because the others are not defined
 */		
//		aa=(E_Raw_Nr[i]+1)/8;
		aa = 1;
		bb=((E_Raw_Nr[i]+1)%8)-1;		
		Number = bb+1;

		if(bb!=-1)
		{
			Rnd->Next();
	
			x = (double)E_Raw[i]+(double)Rnd->Value() ;
			a = (double)ECoef[aa][bb][0] ;
			b = (double)ECoef[aa][bb][1] ;
			c = (double)ECoef[aa][bb][2] ;
			d = (double)ECoef[aa][bb][3] ;
	  
			E[E_Raw_Nr[i]] = (Float_t)((a + b*x) * (c / d)); 
		
			E[E_Raw_Nr[i]] *= (ERef[aa][1]/ERef[aa][0]);
	  
			if(E[E_Raw_Nr[i]] > ETresh[aa])
			{
				ES[aa] += E[E_Raw_Nr[i]];
				MRow[aa] ++;
				ETotal += E[E_Raw_Nr[i]];
				EM++;
			}
#	ifdef DEBUG
			Info("IonisationChamberv::Calibrate","Telescope %d, ETotal %f, Echannel %d, Mult %d",Number,ETotal,E_Raw[i],E_RawM);
#	endif
		}
    }

#	ifdef DEBUG
	if(E_RawM==0) Info("IonisationChamberv::Calibrate","No signal");  
#	endif

	for(Int_t i=0;i<3;i++) if(ES[i] > 0) Counter[i+1]++;
  
	if(EM!=1)
	{
		ETotal = 0.;
		Number = 0;
	}
	if(ES[0] > 0. &&  ETotal > 0. )
	{
		Present = true; 
		Counter[4]++;
	}
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
	inT->SetBranchStatus("EchiNr",1);
	inT->SetBranchStatus("Echi",  1);
	inT->SetBranchStatus("EchiM", 1); 
	#endif

	inT->SetBranchAddress("EchiNr",E_Raw_Nr);
	inT->SetBranchAddress("Echi",  E_Raw   );
	inT->SetBranchAddress("EchiM", &E_RawM ); 
}

void IonisationChamberv::outAttach(TTree *outT)
{
	#ifdef DEBUG
	cout << "IonisationChamberv::outAttach" << endl;
	#endif

	#ifdef DEBUG
	cout << "Attaching IonisationChamberv variables" << endl;
	#endif

	outT->Branch("EchiNr",&Number,"EchiNr/s" );
	outT->Branch("Echi",  &ETotal,"Echi/F"   );
	outT->Branch("EchiM", &EM,    "EchiM/I"  );
//	outT->Branch("IcE",   E,      "IcE[24]/F");
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
  
	int aa,bb;

	cout << "EEawM: " << E_RawM << endl; 
	for(i=0;i<E_RawM;i++)
	{
		aa=(E_Raw_Nr[i]+1)/8;
		bb=((E_Raw_Nr[i]+1)%8)-1;
		if(bb!=-1)
		{
			cout << E_Raw_Nr[i] << endl;
			cout << "PAD " << aa << " " << bb <<": ";
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
		for(i=0;i<3;i++) cout << "SUM ROW:" << i << " " << ES[i] << endl;
		cout << "ETotal: " << ETotal << endl;
	}
}



