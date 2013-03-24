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
  
	fReady=kFALSE;
  
	char line[255];
	int len=255;

	fL = Log;
	fS1 = SeD1;
	fS2 = SeD2;


	for(Int_t i=0;i<14;i++) fCounter[i] = 0;

  	fRnd = new Random;

  	ifstream inf1;
	if(!gDataSet->OpenDataSetFile("SeD12Ref.cal",inf1))
	{
		cout << "Could not open the Reference file SeD12Ref.cal !" << endl;
		return;
	}
	else
	{
		cout.setf(ios::showpoint);
		cout << "Reading SeD12Ref.cal" << endl; fL->Log << "Reading SeD12Ref.cal" << endl;
/*
 *		Reading Reference position
 */
		inf1.getline(line,len);
		cout << line << endl; fL->Log << line << endl;
		for(Int_t i=0;i<2;i++)
		{
			inf1 >> fXRef[i];
			inf1 >> fYRef[i];
/*
 *			Output control
 */
			cout << "  XRef: " << fXRef[i] << "  YRef: " << fYRef[i] << endl ;
		}

/*
 *		Reading Focal position
 */
		for(Int_t i=0;i<2;i++) inf1.getline(line,len);
		cout << line << endl; fL->Log << line << endl;
		inf1 >> fFocalPos;
/*
 *		Output control
 */
		cout <<"  "<< fFocalPos << endl ;
      
/*
 *		Reading Focal Angle
 */
		for(Int_t i=0;i<2;i++) inf1.getline(line,len);
		cout << line << endl; fL->Log << line << endl;
		for(Int_t i=0;i<2;i++)
		{
			inf1 >> fAngleFocal[i];
/*
 *			Output control
 */
			cout <<"  "<< fAngleFocal[i] << " " ; fL->Log <<"  "<< fAngleFocal[i] << " " ;
		}
		cout << endl; 
		fL->Log << endl; 
/*
 *		Reading parameters for Time TSED1-SED2, 6 parameters Float_t, a,b,c,d,e,t0 :
 *		T = -(a * TSED1-SED2 + 2*exp( b * TSED1-SED2)+ c * exp(d * (e + TSED1-SED2))) + t0  [ns]
 *		t0 is determined from file 44, where T-t0 = -79.75
 *		( with the condition: SIE_Nr==5 && EchiNr==4 && SeDX2> -50 && SeDY2 > -50 && SiE>487 && SiE<495 && Echi>7 && Echi<9 && SeDX1>-100 && SeDY1>-50)
 */
		for(Int_t i=0;i<2;i++) inf1.getline(line,len);
		cout << line << endl; fL->Log << line << endl;
		for(Int_t i=0;i<6;i++) inf1 >> fTCoef[i];
/*
 *		Output control
 */
		for(Int_t i=0;i<6;i++) printf("  %10.6g",fTCoef[i]); cout << endl; 	
/*
 *		Reading parameters for the linearisation procedure for the Time TSED1-SED2, 15 parameters Float_t
 */
		for(Int_t i=0;i<2;i++) inf1.getline(line,len);
		cout << line << endl; fL->Log << line << endl;
		for(Int_t i=0;i<15;i++) inf1 >> fTlinearise[i];
/*
 *		Output control
 */
		for(Int_t i=0;i<15;i++) printf("  %10.6g",fTlinearise[i]); cout << endl; 	
		
		cout <<" fin de lecture"<< endl<<flush;
		
	}
	inf1.close();

	SetMatX();
	SetMatY();

	fReady=kTRUE;
}

SeD12v::~SeD12v(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Destructor" << endl;
#	endif

	SafeDelete( fRnd );
	PrintCounters();
}

void SeD12v::PrintCounters(void)
{
#	ifdef DEBUG
	cout << "SeD12v::PrintCounters" << endl;
#	endif
  
	cout << endl;
	cout << "SeD12v::PrintCounters" << endl;
	cout << "Called: " << fCounter[0] << endl;
	cout << "SeD1 Present: " << fCounter[1] << endl;
	cout << "SeD2 Present: " << fCounter[2] << endl;
	cout << "Sed1 and SeD2 Present: " << fCounter[3] << endl;
	cout << "Tf: " << fCounter[4] << " Xf: " << fCounter[5] << endl;
	cout << "Pf: " << fCounter[6] << " Yf: " << fCounter[7] << endl;
	cout << "Present: " << fCounter[8]; 
	fL->Log << endl;
	fL->Log << "SeD12v::PrintCounters" << endl;
	fL->Log << "Called: " << fCounter[0] << endl;
	fL->Log << "SeD1 Present: " << fCounter[1] << endl;
	fL->Log << "SeD2 Present: " << fCounter[2] << endl;
	fL->Log << "Sed1 and SeD2 Present: " << fCounter[3] << endl;
	fL->Log << "Tf: " << fCounter[4] << endl;
	fL->Log << "Tf: " << fCounter[4] << " Xf: " << fCounter[5] << endl;
	fL->Log << "Pf: " << fCounter[6] << " Yf: " << fCounter[7] << endl;
	fL->Log << "Present: " << fCounter[8]; 
}

void SeD12v::Calibrate(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Calibrate" << endl;
#	endif
  
	double a,b,c,d,e,t0, x,t;
	
	fRnd->Next();
	x = (double)fT_Raw + fRnd->Value();
	a = (double)fTCoef[0];
	b = (double)fTCoef[1];
	c = (double)fTCoef[2];
	d = (double)fTCoef[3];
	e = (double)fTCoef[4];
	t0= (double)fTCoef[5];
	fT = 0;
/*
 *	fT = (a * TSED1-SED2 + 2*exp( b * TSED1-SED2)+ c * exp(d * (e + TSED1-SED2))) + t0  [ns]
 */
	if(fT_Raw>0)
	{
		t = (a * x + 2*exp( b * x)+ c * exp(d * (e + x))) + t0;	

		fT = (Float_t)t;
#	ifdef DEBUG
		Info("SeD12v::Calibrate","TSED1_SED2 %d [ch], T %f [ns]",fT_Raw, fT);
#	endif
	}
}

void SeD12v::Linearise(void)
{
#	ifdef DEBUG
	cout << "SeD12v::linearise" << endl;
#	endif

	double c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,  tcorr, x,xx,xxx,xxxx,xxxxx;

	x = fXf; xx=x*x; xxx=xx*x; xxxx=xxx*x; xxxxx=xxxx*x;
	c1 = (double)fTlinearise[0];
	c2 = (double)fTlinearise[1];
	c3 = (double)fTlinearise[2];
	c4 = (double)fTlinearise[3];
	c5 = (double)fTlinearise[4];
	c6 = (double)fTlinearise[5];
	c7 = (double)fTlinearise[6];
	c8 = (double)fTlinearise[7];
	c9 = (double)fTlinearise[8];
	c10= (double)fTlinearise[9];
	c11= (double)fTlinearise[10];
	c12= (double)fTlinearise[11];
	c13= (double)fTlinearise[12];
	c14= (double)fTlinearise[13]; 
	c15= (double)fTlinearise[14];

	tcorr = c1+c2*x+c3*xx+c4*xxx+c5*xxxx+c6*xxxxx 
		+c7 *exp(-(x+c8) *(x+c8) /c9 )
		+c10*exp(-(x+c11)*(x+c11)/c12)
		+c13*exp(-(x+c14)*(x+c14)/c15) ;

//	cout << "  before =====>>>> Xf,T: " << fXf <<" "<< fT << endl;

	fT -= tcorr ;
  
//	cout << "  after  =====>>>> Xf,T,tcorr: " << fXf <<" "<< fT <<" "<< tcorr << endl;
}

void SeD12v::SetMatX(void)
{
	Int_t i,j;
	Double_t A[2][2];
	Double_t Det;
#	ifdef DEBUG
	cout << "SeD12v::SetMatX" << endl;
#	endif

	if(fabs(fAngleFocal[0]) > 0.000001)
		fTanFocal[0] = 1.0/tan(3.1415922654/180.*fAngleFocal[0]);
	else
	fTanFocal[0] = 1.0e08;

	for(i=0;i<2;i++) for(j=0;j<2;j++)
		A[i][j] = fMatX[i][j] = 0.;
      
	for(i=0;i<2;i++)
	{
		A[0][0] += pow(fXRef[i],2.);
		A[0][1] += fXRef[i];
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
		fMatX[0][0] = A[1][1]/Det;
		fMatX[1][1] = A[0][0]/Det;
		fMatX[1][0] = -1.0*A[0][1]/Det;
		fMatX[0][1] = -1.0*A[1][0]/Det;
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

	if(fabs(fAngleFocal[1]) > 0.000001)
	fTanFocal[1] = 1.0/tan(3.1415922654/180.*fAngleFocal[1]);
	else
		fTanFocal[1] = 1.0e08;
 
	for(i=0;i<2;i++) for(j=0;j<2;j++)
		A[i][j] = fMatY[i][j] = 0.;
      
	for(i=0;i<2;i++)
	{
		A[0][0] += pow(fYRef[i],2.);
		A[0][1] += fYRef[i];
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
		fMatY[0][0] = A[1][1]/Det;
		fMatY[1][1] = A[0][0]/Det;
		fMatY[0][1] = -1.0*A[1][0]/Det;
		fMatY[1][0] = -1.0*A[0][1]/Det;
	}
}

void SeD12v::Init(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Init" << endl;
#	endif
	fPresent = false; 

	fXf = fYf = fTf = fPf = -500.0;
	fT=0.;
	fV=0;
}

void SeD12v::Focal(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Focal" << endl;
#	endif
	if(fS1->fPresent) fCounter[1]++;
	if(fS2->fPresent) fCounter[2]++;

	if(fS1->fPresent && fS2->fPresent) // have both SeDs
	{
		fCounter[3]++;
		fX[0] = fS1->fX[0];
		fY[0] = fS1->fX[1];
		fX[1] = fS2->fX[0];
		fY[1] = fS2->fX[1];
		
		FocalX();
		FocalY();
		if( fXf > -500. && fTf > -500. && fYf > -500. && fPf > -500.)
		{
			if(fT!=0.) fV = 100./(cos(0.001*fTf)*cos(0.001*fPf)*fT) ;  
			fPresent = true; 
			fCounter[8]++;
		}
		else fXf = fYf = fTf = fPf = -500.0;         
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
		A[0] += ((Double_t) fX[i])*fXRef[i];
		A[1] += (Double_t) fX[i];
	}
	B[0] =A[0]*fMatX[0][0] + A[1]*fMatX[0][1]; 
	B[1] =A[0]*fMatX[1][0] + A[1]*fMatX[1][1];
/*
 *	Tf in mrad
 */
 	fTf = (Float_t) (1000.*atan(B[0]));
	fCounter[4]++;

	if((fTanFocal[0]-B[0]) != 0.)
	{
		fXf = (Float_t) (B[0]*(fTanFocal[0]*fFocalPos+B[1])/(fTanFocal[0]-B[0])+B[1]);
		fCounter[5]++;
	}
	else  fXf = -500.0;

//	cout << "C " << fTf << ' ' << fXf << endl;
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
		A[0] += ((Double_t) fY[i])*fYRef[i];
		A[1] += fY[i];
	}
	B[0] =A[0]*fMatY[0][0] + A[1]*fMatY[0][1]; 
	B[1] =A[0]*fMatY[1][0] + A[1]*fMatY[1][1]; 

/*
 *	Pf in mrad
 */
	fPf = (Float_t) (1000.*atan(B[0]));
	fCounter[6]++;

	if((fTanFocal[1]-B[0]) != 0.)
	{    
      fYf = (Float_t) (B[0]*(fTanFocal[1]*fFocalPos+B[1])/(fTanFocal[1]-B[0])+B[1]);
      fCounter[7]++;
	}
	else  fYf = -500.0;
}

void SeD12v::Treat(void)
{
#	ifdef DEBUG
	cout << "SeD12v::Treat" << endl;
#	endif

	if(!fReady) return;
	fCounter[0]++;
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
	fL->Log << "Activating Branches: SeD12v" << endl;

	sprintf(strnam,"TSED1_SED2");
	inT->SetBranchStatus(strnam,1);
#	endif
	inT->SetBranchAddress("TSED1_SED2",&fT_Raw);
}

void SeD12v::outAttach(TTree *outT)
{
#	ifdef DEBUG
	cout << "SeD12v::outAttach " << endl;
#	endif

#	ifdef DEBUG
	cout << "Attaching SeD12 variables" << endl;
#	endif

	outT->Branch("Xf",&fXf,"Xf/F");
	outT->Branch("Tf",&fTf,"Tf/F");
	outT->Branch("Yf",&fYf,"Yf/F");
	outT->Branch("Pf",&fPf,"Pf/F");
	outT->Branch("TSeD12",&fT,"TSeD12/F");
	outT->Branch("V",&fV,"V/F");
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

	cout << "X1: " << fS1->fX[0] << " Y1: " << fS1->fX[1] << endl;
	cout << "X2: " << fS2->fX[0] << " Y2: " << fS2->fX[1] << endl;

	cout << "Xf: " << fXf << " Tf: " << fTf << endl;
	cout << "Yf: " << fYf << " Pf: " << fPf << endl;
}
