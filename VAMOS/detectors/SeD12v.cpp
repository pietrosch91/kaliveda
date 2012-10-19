/*
 *	-----------------------------------------------------------------
 *	SeD12v
 *	Initial structure from Maurycy Rejmund (2006)
 *	-----------------------------------------------------------------
 */
#include "SeD12v.h"
#include "KVDataSet.h"
#include <cmath>
#include <cstdlib>

using namespace std;

ClassImp(SeD12v)

SeD12v::SeD12v(LogFile *Log, SeDv *SeD1, SeDv *SeD2)
{
#	ifdef DEBUG
	cout << "SeD12v::Constructor" << endl;
#	endif
  
	Ready=kFALSE;
  
	char line[255];
	int len=255;
	int i,j;

	L = Log;
	S1 = SeD1;
	S2 = SeD2;


	for(i=0;i<14;i++) Counter[i] = 0;

  	Rnd = new Random;

  	ifstream inf1;
	if(!gDataSet->OpenDataSetFile("SeD12Ref.cal",inf1))
	{
		cout << "Could not open the Reference file SeD12Ref.cal !" << endl;
		return;
	}
	else
	{
		cout.setf(ios::showpoint);
		cout << "Reading SeD12Ref.cal" << endl; L->Log << "Reading SeD12Ref.cal" << endl;
/*
 *		Reading Reference position
 */
		inf1.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(i=0;i<2;i++)
		{
			inf1 >> XRef[i];
			inf1 >> YRef[i];
/*
 *			Output control
 */
			cout << "  XRef: " << XRef[i] << "  YRef: " << YRef[i] << endl ;
		}

/*
 *		Reading Focal position
 */
		for(i=0;i<2;i++) inf1.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		inf1 >> FocalPos;
/*
 *		Output control
 */
		cout <<"  "<< FocalPos << endl ;
      
/*
 *		Reading Focal Angle
 */
		for(i=0;i<2;i++) inf1.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(i=0;i<2;i++)
		{
			inf1 >> AngleFocal[i];
/*
 *			Output control
 */
			cout <<"  "<< AngleFocal[i] << " " ; L->Log <<"  "<< AngleFocal[i] << " " ;
		}
		cout << endl; 
		L->Log << endl; 
/*
 *		Reading parameters for Time TSED1-SED2, 6 parameters Float_t, a,b,c,d,e,t0 :
 *		T = -(a * TSED1-SED2 + 2*exp( b * TSED1-SED2)+ c * exp(d * (e + TSED1-SED2))) + t0  [ns]
 *		t0 is determined from file 44, where T-t0 = -79.75
 *		( with the condition: SIE_Nr==5 && EchiNr==4 && SeDX2> -50 && SeDY2 > -50 && SiE>487 && SiE<495 && Echi>7 && Echi<9 && SeDX1>-100 && SeDY1>-50)
 */
		for(i=0;i<2;i++) inf1.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(i=0;i<6;i++) inf1 >> TCoef[i];
/*
 *		Output control
 */
		for(i=0;i<6;i++) printf("  %10.6g",TCoef[i]); cout << endl; 	
/*
 *		Reading parameters for the linearisation procedure for the Time TSED1-SED2, 15 parameters Float_t
 */
		for(i=0;i<2;i++) inf1.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(i=0;i<15;i++) inf1 >> Tlinearise[i];
/*
 *		Output control
 */
		for(i=0;i<15;i++) printf("  %10.6g",Tlinearise[i]); cout << endl; 	
		
		cout <<" fin de lecture"<< endl<<flush;
		
	}
	inf1.close();

	SetMatX();
	SetMatY();

	Ready=kTRUE;
}

SeD12v::~SeD12v(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Destructor" << endl;
#	endif

	PrintCounters();
}

void SeD12v::PrintCounters(void)
{
#	ifdef DEBUG
	cout << "SeD12v::PrintCounters" << endl;
#	endif
  
	cout << endl;
	cout << "SeD12v::PrintCounters" << endl;
	cout << "Called: " << Counter[0] << endl;
	cout << "SeD1 Present: " << Counter[1] << endl;
	cout << "SeD2 Present: " << Counter[2] << endl;
	cout << "Sed1 and SeD2 Present: " << Counter[3] << endl;
	cout << "Tf: " << Counter[4] << " Xf: " << Counter[5] << endl;
	cout << "Pf: " << Counter[6] << " Yf: " << Counter[7] << endl;
	cout << "Present: " << Counter[8]; 
	L->Log << endl;
	L->Log << "SeD12v::PrintCounters" << endl;
	L->Log << "Called: " << Counter[0] << endl;
	L->Log << "SeD1 Present: " << Counter[1] << endl;
	L->Log << "SeD2 Present: " << Counter[2] << endl;
	L->Log << "Sed1 and SeD2 Present: " << Counter[3] << endl;
	L->Log << "Tf: " << Counter[4] << endl;
	L->Log << "Tf: " << Counter[4] << " Xf: " << Counter[5] << endl;
	L->Log << "Pf: " << Counter[6] << " Yf: " << Counter[7] << endl;
	L->Log << "Present: " << Counter[8]; 
}

void SeD12v::Calibrate(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Calibrate" << endl;
#	endif
  
	Int_t i,j;
	double a,b,c,d,e,t0, x,t;
	
	Rnd->Next();
	x = (double)T_Raw + Rnd->Value();
	a = (double)TCoef[0];
	b = (double)TCoef[1];
	c = (double)TCoef[2];
	d = (double)TCoef[3];
	e = (double)TCoef[4];
	t0= (double)TCoef[5];
	T = 0;
/*
 *	T = (a * TSED1-SED2 + 2*exp( b * TSED1-SED2)+ c * exp(d * (e + TSED1-SED2))) + t0  [ns]
 */
	if(T_Raw>0)
	{
		t = (a * x + 2*exp( b * x)+ c * exp(d * (e + x))) + t0;	

		T = (Float_t)t;
#	ifdef DEBUG
		Info("SeD12v::Calibrate","TSED1_SED2 %d [ch], T %f [ns]",T_Raw, T);
#	endif
	}
}

void SeD12v::Linearise(void)
{
#	ifdef DEBUG
	cout << "SeD12v::linearise" << endl;
#	endif

	double c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,  tcorr, x,xx,xxx,xxxx,xxxxx;

	x = Xf; xx=x*x; xxx=xx*x; xxxx=xxx*x; xxxxx=xxxx*x;
	c1 = (double)Tlinearise[0];
	c2 = (double)Tlinearise[1];
	c3 = (double)Tlinearise[2];
	c4 = (double)Tlinearise[3];
	c5 = (double)Tlinearise[4];
	c6 = (double)Tlinearise[5];
	c7 = (double)Tlinearise[6];
	c8 = (double)Tlinearise[7];
	c9 = (double)Tlinearise[8];
	c10= (double)Tlinearise[9];
	c11= (double)Tlinearise[10];
	c12= (double)Tlinearise[11];
	c13= (double)Tlinearise[12];
	c14= (double)Tlinearise[13]; 
	c15= (double)Tlinearise[14];

	tcorr = c1+c2*x+c3*xx+c4*xxx+c5*xxxx+c6*xxxxx 
		+c7 *exp(-(x+c8) *(x+c8) /c9 )
		+c10*exp(-(x+c11)*(x+c11)/c12)
		+c13*exp(-(x+c14)*(x+c14)/c15) ;

//	cout << "  before =====>>>> Xf,T: " << Xf <<" "<< T << endl;

	T -= tcorr ;
  
//	cout << "  after  =====>>>> Xf,T,tcorr: " << Xf <<" "<< T <<" "<< tcorr << endl;
}

void SeD12v::SetMatX(void)
{
	Int_t i,j;
	Double_t A[2][2];
	Double_t Det;
#	ifdef DEBUG
	cout << "SeD12v::SetMatX" << endl;
#	endif

	if(fabs(AngleFocal[0]) > 0.000001)
		TanFocal[0] = 1.0/tan(3.1415922654/180.*AngleFocal[0]);
	else
	TanFocal[0] = 1.0e08;

	for(i=0;i<2;i++) for(j=0;j<2;j++)
		A[i][j] = MatX[i][j] = 0.;
      
	for(i=0;i<2;i++)
	{
		A[0][0] += pow(XRef[i],2.);
		A[0][1] += XRef[i];
		A[1][1] += 1.0;
	}
	A[1][0] = A[0][1];

	Det = A[0][0]*A[1][1] - A[0][1]*A[1][0];

	if(Det == 0.0)
	{
		cout << "SeD12v::SetMatX: Det == 0 !" << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		MatX[0][0] = A[1][1]/Det;
		MatX[1][1] = A[0][0]/Det;
		MatX[1][0] = -1.0*A[0][1]/Det;
		MatX[0][1] = -1.0*A[1][0]/Det;
	}
}

void SeD12v::SetMatY(void)
{
	Int_t i,j;
	Float_t A[2][2];
	Float_t Det;
#	ifdef DEBUG
	cout << "SeD12v::SetMatY" << endl;
#	endif

	if(fabs(AngleFocal[1]) > 0.000001)
	TanFocal[1] = 1.0/tan(3.1415922654/180.*AngleFocal[1]);
	else
		TanFocal[1] = 1.0e08;
 
	for(i=0;i<2;i++) for(j=0;j<2;j++)
		A[i][j] = MatY[i][j] = 0.;
      
	for(i=0;i<2;i++)
	{
		A[0][0] += pow(YRef[i],2.);
		A[0][1] += YRef[i];
		A[1][1] += 1.0;
	}
	A[1][0] = A[0][1];

	Det = A[0][0]*A[1][1] - A[0][1]*A[1][0];

	if(Det == 0.0)
	{
		cout << "SeD12v::SetMatY: Det == 0 !" << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		MatY[0][0] = A[1][1]/Det;
		MatY[1][1] = A[0][0]/Det;
		MatY[0][1] = -1.0*A[1][0]/Det;
		MatY[1][0] = -1.0*A[0][1]/Det;
	}
}

void SeD12v::Init(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Init" << endl;
#	endif
	Present = false; 

	Xf = Yf = Tf = Pf = -500.0;
	T=0.;
	V=0;
}

void SeD12v::Focal(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Focal" << endl;
#	endif
	if(S1->Present) Counter[1]++;
	if(S2->Present) Counter[2]++;

	if(S1->Present && S2->Present) // have both SeDs
	{
		Counter[3]++;
		X[0] = S1->X[0];
		Y[0] = S1->X[1];
		X[1] = S2->X[0];
		Y[1] = S2->X[1];
		
		FocalX();
		FocalY();
		if( Xf > -500. && Tf > -500. && Yf > -500. && Pf > -500.)
		{
			if(T!=0.) V = 100./(cos(0.001*Tf)*cos(0.001*Pf)*T) ;  
			Present = true; 
			Counter[8]++;
		}
		else Xf = Yf = Tf = Pf = -500.0;         
	}
}

void SeD12v::FocalX(void)
{
	Int_t i;
	Double_t A[2];
	Double_t B[2];

#	ifdef DEBUG
	cout << "SeD12v::FocalX" << endl;
#	endif
	for(i=0;i<2;i++) A[i] = B[i] = 0.0;

	for(i=0;i<2;i++)
	{
		A[0] += ((Double_t) X[i])*XRef[i];
		A[1] += (Double_t) X[i];
	}
	B[0] =A[0]*MatX[0][0] + A[1]*MatX[0][1]; 
	B[1] =A[0]*MatX[1][0] + A[1]*MatX[1][1];
/*
 *	Tf in mrad
 */
 	Tf = (Float_t) (1000.*atan(B[0]));
	Counter[4]++;

	if((TanFocal[0]-B[0]) != 0.)
	{
		Xf = (Float_t) (B[0]*(TanFocal[0]*FocalPos+B[1])/(TanFocal[0]-B[0])+B[1]);
		Counter[5]++;
	}
	else  Xf = -500.0;

//	cout << "C " << Tf << ' ' << Xf << endl;
}

void SeD12v::FocalY(void)
{
	Int_t i;
	Float_t A[2];
	Float_t B[2];

#	ifdef DEBUG
	cout << "SeD12v::FocalY" << endl;
#	endif

	for(i=0;i<2;i++) A[i] = B[i] = 0.0;

	for(i=0;i<2;i++)
    {
		A[0] += ((Double_t) Y[i])*YRef[i];
		A[1] += Y[i];
	}
	B[0] =A[0]*MatY[0][0] + A[1]*MatY[0][1]; 
	B[1] =A[0]*MatY[1][0] + A[1]*MatY[1][1]; 

/*
 *	Pf in mrad
 */
	Pf = (Float_t) (1000.*atan(B[0]));
	Counter[6]++;

	if((TanFocal[1]-B[0]) != 0.)
	{    
      Yf = (Float_t) (B[0]*(TanFocal[1]*FocalPos+B[1])/(TanFocal[1]-B[0])+B[1]);
      Counter[7]++;
	}
	else  Yf = -500.0;
}

void SeD12v::Treat(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Treat" << endl;
#	endif

	if(!Ready) return;
	Counter[0]++;
	Init();
	Calibrate();
	Focal();
/*
 *	README README README README README README README README README !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *	The linearisation is for the moment suppressed
 *
 *	README README README README README README README README README !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 */
//	Linearise();

#	ifdef DEBUG
	Show();
#	endif
}

void SeD12v::inAttach(TTree *inT)
{
#	ifdef DEBUG
	cout << "SeD12v::inAttach " << endl;
#	endif

#	ifdef DEBUG
	cout << "Attaching SeD12 variables" << endl;
#	endif
#	ifdef ACTIVEBRANCHES
	cout << "Activating Branches: SeD12v" << endl;
	L->Log << "Activating Branches: SeD12v" << endl;

	sprintf(strnam,"TSED1_SED2");
	inT->SetBranchStatus(strnam,1);
#	endif
	inT->SetBranchAddress("TSED1_SED2",&T_Raw);
}

void SeD12v::outAttach(TTree *outT)
{
#	ifdef DEBUG
	cout << "SeD12v::outAttach " << endl;
#	endif

#	ifdef DEBUG
	cout << "Attaching SeD12 variables" << endl;
#	endif

	outT->Branch("Xf",&Xf,"Xf/F");
	outT->Branch("Tf",&Tf,"Tf/F");
	outT->Branch("Yf",&Yf,"Yf/F");
	outT->Branch("Pf",&Pf,"Pf/F");
	outT->Branch("TSeD12",&T,"TSeD12/F");
	outT->Branch("V",&V,"V/F");
}

void SeD12v::CreateHistograms(void)
{

#	ifdef DEBUG
	cout << "SeD12v::CreateHistograms " << endl;
#	endif
}

void SeD12v::FillHistograms(void)
{
#	ifdef DEBUG
	cout << "SeD12v::FillHistograms " << endl;
#	endif
}

void SeD12v::Show(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Show" << endl;
#	endif
	cout.setf(ios::showpoint);

	cout << "X1: " << S1->X[0] << " Y1: " << S1->X[1] << endl;
	cout << "X2: " << S2->X[0] << " Y2: " << S2->X[1] << endl;

	cout << "Xf: " << Xf << " Tf: " << Tf << endl;
	cout << "Yf: " << Yf << " Pf: " << Pf << endl;
}
