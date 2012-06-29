/*
 *	-----------------------------------------------------------------
 *	SeDv
 *	Initial structure from Maurycy Rejmund (2006)
 *  Rewritten by Sylvain Leblond (stage L3 IPN 2010) and Paolo Napolitani
 *	-----------------------------------------------------------------
 */
#include "SeDv.h"
#include "KVDataSet.h"
#include <cmath>
#include <cstdlib>

ClassImp(SeDv)

SeDv::SeDv(LogFile *Log, short nr)
{
#	ifdef DEBUG
	cout << "SeDv::Constructor" << endl;
#	endif
	Ready=kFALSE;
  
	char line[255];
	int len=255;
	int i,j,k;
	char fname[20];

	L = Log;
	lnr = nr;

	for(i=0;i<14;i++) Counter[i] = 0;
	for(i=0;i<4;i++) for(j=0;j<5;j++) Counter1[i][j] = 0;

	InitRaw();
	Zero();

	Rnd = new Random;
  
	NStrips = 3;
	cout << "Number of considered strips: " << NStrips << endl;
	L->Log << "Number of considered strips: " << NStrips << endl;
/*
 *	Open and read "SeD#Ref.cal"
 */
	sprintf(fname,"SeD%1dRef.cal",lnr);
	ifstream inf;
	if(!gDataSet->OpenDataSetFile(fname, inf)) {
		cout << "Could not open the Reference file SeDRef.cal !" << endl; return; }
	else
	{
		cout.setf(ios::showpoint);
		cout << "Reading " << fname << endl; L->Log << "Reading "<< fname << endl;
/*
 *		Reading Q-threshold parameter
 */
		inf.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		inf >> QThresh[0]; inf >> QThresh[1];
		cout <<"  "<< QThresh[0] <<", "<< QThresh[1] << endl; L->Log <<"  "<< QThresh[0] <<", "<< QThresh[1] << endl; 
/*
 *		Reading X geometric transformation matrix(j*k), first and second iteration(i)
 */
		for(i=0;i<2;i++) 
		{
		  	for(j=0;j<2;j++) inf.getline(line,len);
			cout << line << endl; L->Log << line << endl;
			for(j=0;j<=ORDRE_DU_FIT;j++) for(k=0;k<=ORDRE_DU_FIT;k++)
		  	{
				inf >> RecadrageX_SED[i][j][k];
				printf("  %13.8g",RecadrageX_SED[i][j][k]);  if(k==4) cout << endl; 
			}
		}
/*	
 *		Reading Y geometric transformation matrix(j*k), first and second iteration(i)
 */
		for(i=0;i<2;i++) 
		{
		  	for(j=0;j<2;j++) inf.getline(line,len);
			cout << line << endl;
			L->Log << line << endl;
			for(j=0;j<=ORDRE_DU_FIT;j++) for(k=0;k<=ORDRE_DU_FIT;k++)
		  	{
		  		inf >> RecadrageY_SED[i][j][k];
				printf("  %13.8g",RecadrageY_SED[i][j][k]);  if(k==4) cout << endl; 
		  	}
		}
	}
	inf.close();
/*
 *	Open and read "SeD#.cal"
 */
	sprintf(fname,"SeD%1d.cal",lnr);
	if(!gDataSet->OpenDataSetFile(fname,inf)) { 
		cout << "Could not open the calibration file " << fname << endl; return; }
	else
	{
		cout.setf(ios::showpoint);
		cout << "Reading " << fname << endl; L->Log << "Reading " << fname << endl;
/*	
 *		Reading E coefficients :
 */
		inf.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(i=0;i<3;i++) for(j=0;j<2;j++) 
		{
			inf >> ECoef[i][j]; 
			cout <<"  "<< ECoef[i][j] <<" "; if(j==1) cout << endl; 
		} 
/*	
 *		Reading parameters for Time TSED#-HF, 7 parameters Float_t, a,b,c,d,e,f,t0 :
 */
		for(j=0;j<2;j++) inf.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(k=0;k<7;k++)
		{
		  	inf >> TSED_HF_Coef[k];
			cout <<"  "<< TSED_HF_Coef[k] <<" "; if(k>=5) cout << endl ;  
			if(k==5) {cout<<"  t0: ";  for(j=0;j<2;j++) inf.getline(line,len);}	// t0 is in a new line
		}	
/*	
 *		Reading charge calibration of the 128 vertical strips giving the x coordinate :
 */
		for(j=0;j<2;j++) inf.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(k=0;k<1;k++) for(i=0;i<128;i++) for(j=0;j<3;j++) inf >> QCoef[i][k][j];
/*	
 *		Reading charge calibration of the 48 horizontal strips giving the y coordinate :
 */
		for(j=0;j<2;j++) inf.getline(line,len);
		cout << line << endl; L->Log << line << endl;
		for(k=1;k<2;k++) for(i=0;i<48;i++) for(j=0;j<3;j++) inf >> QCoef[i][k][j];
	}
	inf.close();
	Ready=kTRUE;
}

SeDv::~SeDv(void)
{
#	ifdef DEBUG
	cout << "SeDv::Destructor" << endl;
#	endif
	PrintCounters();
}

void SeDv::PrintCounters(void)
{
	Int_t i;
#	ifdef DEBUG
	cout << "SeDv::PrintCounters" << endl;
#	endif

	cout << endl;
	cout << "SeDv::PrintCounters" << endl;
	cout << "Called: " << Counter[0] << endl;
	cout << "E1: " << Counter[1] << endl;
	cout << "E2: " << Counter[2] << endl;
	cout << "T1: " << Counter[3] << endl;
	cout << "T2: " << Counter[4] << endl;
	cout << "PresentWires: " << Counter[5] << endl;
	cout << "STRIP Mult >= " << NStrips << endl;
	for(i=0;i<4;i++) cout << Counter1[i][0] << " ";
	cout << endl;
	cout << "PresentStrips: " << Counter[6] << endl;
	cout << "X: Sequence Hiperbolique" << endl;
	for(i=0;i<2;i++) cout << Counter1[i][1] << " ";
	cout << endl;
	cout << "X: Weighted Average" << endl;
	for(i=0;i<2;i++) cout << Counter1[i][2] << " ";
	cout << endl;
	for(i=0;i<2;i++) cout << Counter1[i][3] << " ";
	cout << endl;
	cout << "PresentSubseqX: " << Counter[7] << endl;
	cout << "PresentSubseqY: " << Counter[8] << endl;
	cout << "Present: " << Counter[9] << endl;

	L->Log << endl;
	L->Log << "SeDv::PrintCounters" << endl;
	L->Log << "Called: " << Counter[0] << endl;
	L->Log << "E1: " << Counter[1] << endl;
	L->Log << "E2: " << Counter[2] << endl;
	L->Log << "T1: " << Counter[3] << endl;
	L->Log << "T2: " << Counter[4] << endl;
	L->Log << "PresentWires: " << Counter[5] << endl;
	L->Log << "STRIP Mult >= " << NStrips << endl;
	for(i=0;i<2;i++) L->Log << Counter1[i][0] << " ";
	L->Log << endl;
	L->Log << "PresentStrips: " << Counter[6] << endl;
	L->Log << "X: Sequence Hiperbolique" << endl;
	for(i=0;i<2;i++) L->Log << Counter1[i][1] << " ";
	L->Log << endl;
	L->Log << "X: Weighted Average" << endl;
	for(i=0;i<2;i++) L->Log << Counter1[i][2] << " ";
	L->Log << endl;
	for(i=0;i<2;i++) L->Log << Counter1[i][3] << " ";
	L->Log << endl;
	L->Log << "PresentSubseqX: " << Counter[7] << endl;
	L->Log << "PresentSubseqY: " << Counter[8] << endl;
	L->Log << "Present: " << Counter[9] << endl;
}

void SeDv::Zero(void)
{
#	ifdef DEBUG
	cout << "SeDv::Zero" << endl;
#	endif
	Present = PresentWires = PresentStrips = false; 

	E[0] = E[1] = E[2] = T[0] = 0.0;
	Mult[0] = Mult[1] = 0; 
	for(Int_t j=0;j<2;j++) for(Int_t i=0;i<128;i++)
	{
		Q[i][j] = 0.0;
		N[i][j] = 0 ;
	}
	X[0]   = X[1]   = -500.0;
	XS[0]  = XS[1]  = -500.0;
	XWA[0] = XWA[1] = -500.0;
}

void SeDv::InitRaw(void)
{
#	ifdef DEBUG
	cout << "SeDv::InitRaw" << endl;
#	endif

	for(Int_t i=0;i<3;i++) E_Raw[i] = 0;
	T_Raw[0] = 0;

	for(Int_t i=0;i<128;i++) for(Int_t j=0;j<2;j++)
	{
		Q_Raw[j*128+i] = 0;
		Q_Raw_Nr[j*128+i] = 0;
	}

	for(Int_t j=0;j<2;j++) Q_RawM[j] = 0;
}

void SeDv::Init(void)
{
#	ifdef DEBUG
	cout << "SeDv::Init" << endl;
#	endif
	Present = PresentWires = PresentStrips = false; 

	E[0] = E[1] = E[2] = T[0] = 0.0;
	Mult[0] = Mult[1] = 0;
	X[0]   = X[1]   = -500.0;
	XS[0]  = XS[1]  = -500.0;
	XWA[0] = XWA[1] = -500.0;
}

void SeDv::Calibrate(void)
{
#	ifdef DEBUG
	cout << "SeDv::Calibrate" << endl;
#	endif
  
	double x,a,b,c,d,e,f,t0,m_run;
	Int_t i,j,k;
	Float_t QTmp,QTmp_array[2][128], noise_mean[2],noise_variance[2],noise_stdeviation_[2],noise_stdeviation,noise_sum;
	Float_t max_mean[2],max_variance[2],max_stdeviation,max_sum;
	UShort_t Q_max_Nr[2];
	Float_t noise_threshold[2],Q_max[2]; 
/*
 *	Initialisations
 */	
	for(i=0;i<2;i++) for(j=0;j<128;j++) QTmp_array[i][j]=0;

	for(i=0;i<2;i++) 
	{
		Q_max[i] = noise_mean[i] = noise_variance[i] = noise_sum = noise_stdeviation_[i] = noise_threshold[i] = 0.;
		max_mean[i] = max_variance[i] = max_sum = 0.;
		Q_max_Nr[i] = 1000 ;
		Accepted_event = false;
	}

	for(i=0;i<3;i++) if(E_Raw[i]>0)
	{
		Rnd->Next();
		for(j=0;j<2;j++) E[i]+=powf((Float_t)E_Raw[i]+Rnd->Value(),(Float_t)j)*ECoef[i][j];
	}
	if(T_Raw[0]>0)
	{
		Rnd->Next();
		
		x = (double)T_Raw[0]+(double)Rnd->Value() ;
		a = (double)TSED_HF_Coef[0] ;
		b = (double)TSED_HF_Coef[1] ;
		c = (double)TSED_HF_Coef[2] ;
		d = (double)TSED_HF_Coef[3] ;
		e = (double)TSED_HF_Coef[4] ;
		f = (double)TSED_HF_Coef[5] ;
		t0= (double)TSED_HF_Coef[6] ;
		
/*
 *		README README README README README README README README README README  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *		
 *		PRELIMINARY CORRECTION: to change later 
 *		this is approximatively valid for the calibration runs 41-46
 *		this should be done more properly by verifying t0 in the parameters from file 
 *		and a multiplication factor m_run as a function of the run.
 *		one should perhaps write
 *		
 *		T[0] = (Float_t)( -(a+(b+(c+(d+(e+f*x)*x)*x)*x)*x)*x * m_run + t0 );
 *		
 *		README README README README README README README README README README  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */		
		m_run = 1./2.5 ;

		T[0] = (Float_t)( -(a+(b+(c+(d+(e+f*x)*x)*x)*x)*x)*x*m_run+t0 );
		
/*
 *		Output control
 */
		cout<<"  Traw[ch], TSED_HF[ns] : " <<T_Raw[0] << " , " <<T[0] <<endl;  
	}
	
	if(E[0] > 0.0) Counter[1]++;
	if(E[1] > 0.0) Counter[2]++;
	if(E[2] > 0.0) Counter[3]++;
	if(T[0] > 0.0) Counter[4]++;

	if((E[0] > 0.0 || E[1] > 0.0 || E[2] > 0.0) && T_Raw[0] > 0.0)
	{
		PresentWires = true;
		Counter[5]++;
	}
	else 
	{
		E[0] = E[1] = E[2] = 0.0;
/*
 *		PRELIMINARY CORRECTION: to change later by the use of t0 
 *		if T[] is negative than it should be rejected 
 */		
//		T[0] = 0.0;
	}
/*
 *	Calculation of the threshold
 */
	if(!PresentWires)
	{	
		for(i=0;i<2;i++)			//0) vertical, giving x. 1) vertical, giving y
		{
/*
 *			Study the noise and the maximum value for the charge.
 *			At this stage the quantities noise_*** contain also the signal!
 */
			for(j=0;j<Q_RawM[i];j++)	//loop over strips
			{
				Rnd->Next();
				for(k=0;k<3;k++)		//loop over coeff
					QTmp_array[i][j] += powf((Float_t)Q_Raw[j+i*128]+Rnd->Value(),(Float_t)k)
						*QCoef[Q_Raw_Nr[j+i*128]][i][k];
				
				QTmp = QTmp_array[i][j];			
				if(QTmp >0)
				{
					noise_mean[i] += QTmp;
					noise_variance[i] += QTmp*QTmp;
					noise_sum += 1.;
				}
				if(QTmp > Q_max[i])	
				{
					Q_max[i] = QTmp;
					Q_max_Nr[i] = Q_Raw_Nr[j+i*128];
				}
			}
/*
 *			Study the surrounding of the maximum charge
 */			
			if(Q_max_Nr[i]>=0) for(j=-Border;j<=Border;j++)
			{
				k = Q_max_Nr[i]+j; if(k<=48+i*128 && k>=0) 
				{
					QTmp=QTmp_array[i][k];  
					if (QTmp>0)
					{
						max_mean[i] += QTmp;
						max_variance[i] += QTmp*QTmp;
						max_sum += 1.;	
					}
				}
			}
/*
 *			The signal of the maximum charge is removed from the quantities noise_***
 */		
			noise_mean[i] -= max_mean[i];
			noise_variance[i] -= max_variance[i];
			noise_sum -= noise_sum;
/*
 *			Calculate statistical quntities related to the noise and fix the threshold
 *			as the mean of the noise + 1 time its standart deviation
 */			
			if(noise_sum<2. || noise_variance[i]==0) 
			{
				noise_mean[i] = noise_variance[i] =0;
			}
			else 
			{
				noise_mean[i] /= noise_sum;
				noise_variance[i] = noise_variance[i]/noise_sum - noise_mean[i]*noise_mean[i];
				noise_stdeviation_[i] = sqrt(noise_variance[i]);
			}
			noise_threshold[i] = noise_mean[i] + noise_stdeviation_[i];
/*
 *			Calculate statistical quntities related to the maximum
 */			
			if(max_sum<2.) 
			{
				max_mean[i] = max_variance[i] =0;
			}
			else 
			{
				max_mean[i] /= max_sum;
				max_variance[i] = max_variance[i]/max_sum - max_mean[i]*max_mean[i];
			}
		}
		max_stdeviation = sqrt(max_variance[0] + max_variance[1]); 
		noise_stdeviation = sqrt(noise_variance[0] + noise_variance[1]); 
		if((noise_sum>3. && noise_stdeviation<2.*max_stdeviation) || (noise_sum<2. && max_sum>2.))
			Accepted_event= true ;
 	}
/*
 *	Select the wires giving a signal above the threshold 
 */
	if(!PresentWires)
	{	
		if(Accepted_event)
		for(i=0;i<2;i++)			//0) vertical, giving x. 1) vertical, giving y
		for(j=0;j<Q_RawM[i];j++)	//loop over strips
		{
			QTmp=QTmp_array[i][j];
			if(QTmp > noise_threshold[i])
			{
				Mult[i]++;
				Q[Mult[i]-1][i] = QTmp;		
				N[Mult[i]-1][i] = Q_Raw_Nr[j+i*128];		
			}
		}
		for(i=0;i<2;i++) if(Mult[i] >= NStrips) Counter1[i][0]++;
	
		if(Mult[0] >= NStrips && Mult[1] >= NStrips)
		{
			PresentStrips = true;
			Counter[6]++;
		}
/*
 *		output control
 */
		cout<<" accepted event          : " << Accepted_event   <<endl;  
 		cout<<" maximum charge x,y      : " << Q_max[0]             <<" "<< Q_max[1]             <<endl;  
		cout<<" maximum charge wire x,y : " << Q_max_Nr[0]          <<" "<< Q_max_Nr[1]          <<endl;  
		cout<<" noise mean x,y          : " << noise_mean[0]        <<" "<< noise_mean[1]        <<endl;  
		cout<<" noise variance x,y      : " << noise_variance[0]    <<" "<< noise_variance[1]    <<endl;  
		cout<<" noise stdeviation x,y   : " << noise_stdeviation_[0]<<" "<< noise_stdeviation_[1]<<endl;  
		cout<<" noise threshold x,y     : " << noise_threshold[0]   <<" "<< noise_threshold[1]   <<endl;  
		cout<<" max mean x,y            : " << max_mean[0]          <<" "<< max_mean[1]          <<endl;  
		cout<<" max variance x,y        : " << max_variance[0]      <<" "<< max_variance[1]      <<endl;  
		cout<<" max stdeviation         : " << max_stdeviation   <<endl;  
	}
}
void SeDv::FocalSubseqX(void)
{
	Int_t i,j,k,m;
	Float_t QTmp[128];
	Float_t QMax;
	Int_t NMax;
	Int_t FStrip[128];
	Int_t StripsWA;
	Float_t v[6];
	Double_t XYp,stk1,stk0;

#	ifdef DEBUG
	cout << "SeDv::FocalSubseqX" << endl;
#	endif

	for(i=0;i<2;i++)							// 0) vertical, giving x. 1) vertical, giving y
	{
/*
 *		Create an array of touched strips of dimension Mult[i]
 */
		for(j=0;j<Mult[i];j++) QTmp[j]=Q[j][i];
/*
 *		searching for a number NStrips of Strips, create an array of labels which order the strips 
 *		according to decreasing charge
 */
		for(k=0;k<NStrips;k++)
		{
			QMax=0.0; NMax=0;
			for(j=0;j<Mult[i];j++) if(QTmp[j] > QMax)
			{ 
				QMax=QTmp[j]; 
				NMax=j; 
			}
			QTmp[NMax] = 0.0;
			FStrip[k] = NMax;
		} 
/*
 *		Just a control
 */       
		if(NStrips != 3) 
		{
			cout << "NStrips != 3 but " << NStrips << " Program it first" << endl;
			exit(EXIT_FAILURE);
		}
/*
 *		Looking for entire peak for weighted average. The Strips are ordered  0-128
 */
		if(Mult[i] > NStrips)	
		{
			StripsWA=0;
			for(j=FStrip[0]-1;j>=0;j--) if(abs(N[j+1][i]-N[j][i])==1)
			{
				if((Q[j][i] <= Q[j+1][i]))
				{
					StripsWA++;
					FStrip[StripsWA]=j;
				}
				else break;
			}
			else break;
			  
			for(j=FStrip[0];j<Mult[i]-1;j++) if(abs(N[j+1][i]-N[j][i])==1)
			{
				if(Q[j][i] >= Q[j+1][i])
				{
					StripsWA++;
					FStrip[StripsWA]=j+1;			  
				}
				else break;
			}
			else break;
	
			if(StripsWA >= NStrips)
			{
				v[0] = v[1] = 0.0;
				for(k=0;k<=StripsWA;k++)
				{		      
					v[0] += Q[FStrip[k]][i]* ((Float_t) N[FStrip[k]][i]);
					v[1] += Q[FStrip[k]][i];
				}
				X[i] = v[0] / v[1];
			}		
		}
	}
/*
 *	Output control
 */
	cout << "  (X,Y)[ch] , (X,Y)[mm] : ("<< X[0]<< " , "  << X[1]<<")";
/*
 *	Recadrage. On utilise les fonctions de corrections pour recadrer l'image. Passage en cm on the SeD plane
 */
	X[0]+=1.; X[1]+=1.; // To have wires starting at 1

	if(X[0]>0 && X[1]>0) for(i=0;i<2;i++)
	{
/*
 *		Correction en X 1
 *		Abscissa calibration on the SeD# plane in cm
 */
		stk0 = (Double_t)X[0];
		stk1 = (Double_t)X[1];
		X[0] = 0.;
		for(j=0;j<=ORDRE_DU_FIT;j++) for(k=0;k<=ORDRE_DU_FIT;k++) if(RecadrageX_SED[i][j][k]!=0) 
		{
			XYp=1.; 
			for(m=0;m<j;m++) XYp*=stk0; 
			for(m=0;m<k;m++) XYp*=stk1;
			X[0] += (Float_t)(RecadrageX_SED[i][j][k] * XYp);
		}
//		cout << "  Wire corrected x : "<< X[0]<< " "  << X[1]<<  endl;
/*
 *		Correction en Y 1
 *		Ordinate calibration on the SeD# plane in cm
 */
		stk0 = (Double_t)X[0];
		stk1 = (Double_t)X[1];
		X[1] = 0.;
		for(j=0;j<=ORDRE_DU_FIT;j++) for(k=0;k<=ORDRE_DU_FIT;k++) if(RecadrageY_SED[i][j][k]!=0) 
		{
			XYp=1.; 
			for(m=0;m<j;m++) XYp*=stk1; 
			for(m=0;m<k;m++) XYp*=stk0;
			X[1] += (Float_t)(RecadrageY_SED[i][j][k]* XYp);
		}
//		cout << "  Wire corrected y : "<< X[0]<< " "  << X[1]<<  endl;
	}
/*
 *	Passage cm -> mm on the projection of SeD#
 */
	X[0] *= 10.;
	X[1] *= 10.*ONEOVERSQRTTWO;
/*
 *	Counters
 */
	XWA[0]=X[0];	XWA[1]=X[1];
	if ((X[0]<Borneminx_SED || X[0] >Bornemaxx_SED) ||
		(X[1]<Borneminy_SED || X[1] >Bornemaxy_SED))
		XWA[0] = X[0] = XWA[1] = X[1] = -500.;
	else Counter1[i][2]++;
	
/*
 *	Output control
 */
	cout << " , ("<< X[0]<< " , "  << X[1]<<")"<<  endl;

	if (X[0]>Borneminx_SED && X[0] <Bornemaxx_SED)	Counter[7]++;
	if (X[1]>Borneminy_SED && X[1] <Bornemaxy_SED)	Counter[8]++;
	
	if ((X[0]>Borneminx_SED && X[0] <Bornemaxx_SED)&&
	    (X[1]>Borneminy_SED && X[1] <Bornemaxy_SED))
	{
		Present = true;
		Counter[9]++;
	}
	else for(i=0;i<2;i++) X[i] = -500.0;
}

void SeDv::Treat(void)
{
#	ifdef DEBUG
	cout << "SeDv::Treat" << endl;
#	endif

	if(!Ready) return;
	Counter[0]++;
	Init();
	Calibrate();
	if(PresentStrips) FocalSubseqX();		//PresentWires by definition

#	ifdef DEBUG
	Show_Raw();
	Show();
#	endif
}

void SeDv::inAttach(TTree *inT)
{
	char strnam[20];
	Int_t i;

#	ifdef DEBUG
	cout << "SeDv::inAttach " << endl;
	cout << "Attaching Sed variables" << endl;
#	endif

	/* for(i=0;i<3;i++)
	{
		sprintf(strnam,"QSED%1d_%1d",lnr,i+1);
		inT->SetBranchAddress(strnam,&E_Raw[i]);
	// }*/
	sprintf(strnam,"TSED%1d_HF",lnr);
	inT->SetBranchAddress(strnam,&T_Raw[0]);
	sprintf(strnam,"SEDMX%1d",lnr);
	inT->SetBranchAddress(strnam,Q_RawM+0);
	sprintf(strnam,"SEDMY%1d",lnr);
	inT->SetBranchAddress(strnam,Q_RawM+1);
	sprintf(strnam,"SEDX%1d",lnr);
	inT->SetBranchAddress(strnam,Q_Raw+0*128);
	sprintf(strnam,"SEDY%1d",lnr);
	inT->SetBranchAddress(strnam,Q_Raw+1*128);
	sprintf(strnam,"SEDNrX%1d",lnr);
	inT->SetBranchAddress(strnam,Q_Raw_Nr+0*128);
	sprintf(strnam,"SEDNrY%1d",lnr);
	inT->SetBranchAddress(strnam,Q_Raw_Nr+1*128);
}

void SeDv::outAttach(TTree *outT)
{
	char strnam[20];
	char strnam1[20];
	Int_t i;

#	ifdef DEBUG
	cout << "SeDv::outAttach " << endl;
	cout << "Attaching SeD variables" << endl;
#	endif

	for(i=0;i<3;i++)
	{
		sprintf(strnam,"SeDEWire%1d_%1d",lnr,i);
		sprintf(strnam1,"SeDEWire%1d_%1d/F",lnr,i);
		outT->Branch(strnam,&E[i],strnam1);
	}
	sprintf(strnam,"SeDTWire%1d",lnr);
	sprintf(strnam1,"SeDTWire%1d/F",lnr);
	outT->Branch(strnam,&T[0],strnam1);

	sprintf(strnam,"SeDX%1d",lnr);
	sprintf(strnam1,"SeDX%1d/F",lnr);
	outT->Branch(strnam,&X[0],strnam1);

	sprintf(strnam,"SeDY%1d",lnr);
	sprintf(strnam1,"SeDY%1d/F",lnr);
	outT->Branch(strnam,&X[1],strnam1);

	sprintf(strnam,"SeDXS%1d",lnr);
	sprintf(strnam1,"SeDXS%1d/F",lnr);
	outT->Branch(strnam,&XS[0],strnam1);

	sprintf(strnam,"SeDYS%1d",lnr);
	sprintf(strnam1,"SeDYS%1d/F",lnr);
	outT->Branch(strnam,&XS[1],strnam1);

	sprintf(strnam,"SeDXWA%1d",lnr);
	sprintf(strnam1,"SeDXWA%1d/F",lnr);
	outT->Branch(strnam,&XWA[0],strnam1);

	sprintf(strnam,"SeDYWA%1d",lnr);
	sprintf(strnam1,"SeDYWA%1d/F",lnr);
	outT->Branch(strnam,&XWA[1],strnam1);
}

void SeDv::CreateHistograms(void)
{
#	ifdef DEBUG
	cout << "SeDv::CreateHistograms " << endl;
#	endif
}

void SeDv::FillHistograms(void)
{
#	ifdef DEBUG
	cout << "SeDv::FillHistograms " << endl;
#	endif
}

void SeDv::Show_Raw(void)
{
	Int_t i,j;
#	ifdef DEBUG
	cout << "SeDv::Show_Raw" << endl;
#	endif
	cout.setf(ios::showpoint);

	cout << "E1_Raw: " << E_Raw[0] << " E2_Raw: " << E_Raw[1] << " E3_Raw: " << E_Raw[2] <<endl;
	cout << "T1_Raw: " << T_Raw[0] << endl;

	for(i=0;i<2;i++)			//0) vertical, giving x. 1) vertical, giving y
	for(j=0;j<Q_RawM[i];j++)	//loop over strips
		cout << "STR: " << Q_Raw_Nr[j+i*128] << " SED: " << i << " Q_Raw: " <<  Q_Raw[j+i*128] << endl;
}

void SeDv::Show(void)
{
	Int_t i,j;

#	ifdef DEBUG
	cout << "SeDv::Show" << endl;
#	endif
	cout.setf(ios::showpoint); 
	
	cout << "PresentWires: " << PresentWires << endl;
	if(PresentWires)
	{
		cout << "E1: " << E[0] << " E2: " << E[1] << " E3: " << E[2] <<endl;
		cout << "T1: " << T[0] << endl;
	}
	cout << "PresentStrips: " << PresentStrips << endl;
	if(PresentStrips) for(i=0;i<2;i++)						//0) vertical, giving x. 1) vertical, giving y
	{
		cout << " Mult " << i << ": " << Mult[i] << endl;
		for(j=0;j<Mult[i];j++)								//loop over strips touched
			cout << "STR: " << N[j][i] << " SED: " << i << " Q: " <<  Q[j][i] << endl;
	}
	cout << "Xi: ";
	for(i=0;i<2;i++) cout << X[i] << " ";					//0) vertical, giving x. 1) vertical, giving y		
	cout << endl;
	cout << "Present: " << Present << endl;
}
