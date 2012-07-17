/*
 *	-----------------------------------------------------------------
 *	ReconstructionSeDv
 *	Initial structure from Maurycy Rejmund (2006)
 *	-----------------------------------------------------------------
 */
#include "ReconstructionSeDv.h"
#include "TVector3.h"
#include "KVDataSet.h"
#include <cmath>
#include "stdlib.h"             // mars 2011 pour drand48

#define PI 3.141592654
#define cPI_OVER_180 0.017453293
#define c180_OVER_PI 57.295779506

using namespace std;

ClassImp(ReconstructionSeDv)

ReconstructionSeDv::ReconstructionSeDv(LogFile *Log, SeD12v *SeD12)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::Constructor" << endl;
#	endif
	
	Ready=kFALSE;
	char line[255];
	char fname[30];
	int len=255;
	int i,j;

	L = Log;
	S12 = SeD12;

	for(i=0;i<6;i++) Counter[i] = 0;

	Init();

	Rnd = new Random;
  
	ifstream inf;
  
	if(!gDataSet->OpenDataSetFile("ReconstructionSeD.cal",inf))
	{
		cout << "Could not open the calibration file ReconstructionSeD.cal !" << endl;
		return;
	}
	else
	{
		cout.setf(ios::showpoint);
		cout << "Reading ReconstructionSeD.cal" << endl;
		L->Log << "Reading ReconstructionSeD.cal" << endl;

		inf.getline(line,len);
		cout << line << endl;
		L->Log << line << endl;
//		inf >> BrhoRef;
//		cout << BrhoRef << endl;
//		L->Log << BrhoRef << endl;

		inf.getline(line,len);
     
		for(j=0;j<4;j++)
		{
			inf.getline(line,len);
			cout << line << endl;
			L->Log << line << endl;
			inf.getline(line,len);
			sscanf(line,"%s",fname);
	
			ifstream inf1;
	  
			if(!gDataSet->OpenDataSetFile(fname,inf1))
			{
				cout << "Could not open the calibration file " << fname << endl;
				return;
			}
			else
			{
				cout.setf(ios::showpoint);
				cout << "Reading " << fname << endl;
				L->Log << "Reading " << fname << endl;
				inf1.getline(line,len);
				cout << line << endl;
				L->Log << line << endl;
	      
				for(i=0;i<330;i++) inf1 >> Coef[j][i];
			}
			inf1.close();
		}
	}
	inf.close();
	Ready=kTRUE;
}

ReconstructionSeDv::~ReconstructionSeDv(void)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::Destructor" << endl;
#	endif

	PrintCounters();
}

void ReconstructionSeDv::PrintCounters(void)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::PrintCounters" << endl;
#	endif
	cout << endl;
	cout << "ReconstructionSeDv::PrintCounters" << endl;
	cout << "Called :" << Counter[0] << endl;
	cout << "Called with SeDs Present :" << Counter[1] << endl;
	cout << "Present :" << Counter[2] << endl;

	L->Log << endl;
	L->Log << "ReconstructionSeDv::PrintCounters" << endl;
	L->Log << "Called :" << Counter[0] << endl;
	L->Log << "Called with SeDs Present :" << Counter[1] << endl;
	L->Log << "Present :" << Counter[2] << endl;
}

void ReconstructionSeDv::Init(void)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::Init" << endl;
#	endif
	Present = false; 

	Brho = -1.;
	Theta = Phi = Path = -500.;
	ThetaL = PhiL = -1.;
}

void ReconstructionSeDv::SetBrhoRef(Double_t BB)
{
	B = BB;
}

void ReconstructionSeDv::SetAngleVamos(Double_t ttheta)
{
	theta = ttheta; 
}

Double_t ReconstructionSeDv::GetBrhoRef(void)
{
	return B;
}

Double_t ReconstructionSeDv::GetAngleVamos(void)
{
	return theta; 
}

void ReconstructionSeDv::Calculate(void)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::Calculate" << endl;
#	endif
	
	Double_t Brhot,Thetat,Phit,Patht;
	Double_t Vec[5],Vecp;
	Double_t theta_refVamos,x,y,z,x_,y_,hipot_rel;
	Double_t ThetaL_,PhiL_;


	Int_t i,j[7];

	Brhot=Thetat=Phit = Patht = 0.00000;
	Vec[0] = 1.000000;
	Vec[1] =(Double_t) (-1. * (S12->Xf)/1000.);
	Vec[2] =0.;//(Double_t) (-1. * (S12->Yf)/1000.);
	Vec[3] =(Double_t) (-1. * (S12->Tf)/1000.);
	Vec[4] =0.;//(Double_t) (-1. * atan(tan((S12->Pf)/1000.)*cos((S12->Tf)/1000.)));
	//goes to zgoubi coordinates

	i = 0;
	for(j[0]=0;j[0]<5;j[0]++)
	for(j[1]=j[0];j[1]<5;j[1]++)
	for(j[2]=j[1];j[2]<5;j[2]++)
	for(j[3]=j[2];j[3]<5;j[3]++)
	for(j[4]=j[3];j[4]<5;j[4]++)
	for(j[5]=j[4];j[5]<5;j[5]++)
	for(j[6]=j[5];j[6]<5;j[6]++)
	{
		Vecp = Vec[j[0]]*Vec[j[1]]*Vec[j[2]]*Vec[j[3]]*Vec[j[4]]*Vec[j[5]]*Vec[j[6]];
		Brhot += Coef[0][i] *Vecp;
		Thetat += Coef[1][i] *Vecp;
		Patht += Coef[3][i] *Vecp;
		i++;
	}

	Vec[0] = 1.000000;
	Vec[1] =(Double_t) (-1. * (S12->Xf)/1000.);
	Vec[2] =(Double_t) (-1. * (S12->Yf)/1000.);
	Vec[3] =(Double_t) (-1. * (S12->Tf)/1000.);
	Vec[4] =(Double_t) (-1. * atan(tan((S12->Pf)/1000.)*cos((S12->Tf)/1000.)));

	i = 0;
	for(j[0]=0;j[0]<5;j[0]++)
	for(j[1]=j[0];j[1]<5;j[1]++)
	for(j[2]=j[1];j[2]<5;j[2]++)
	for(j[3]=j[2];j[3]<5;j[3]++)
	for(j[4]=j[3];j[4]<5;j[4]++)
	for(j[5]=j[4];j[5]<5;j[5]++)
	for(j[6]=j[5];j[6]<5;j[6]++)
	{
		Vecp = Vec[j[0]]*Vec[j[1]]*Vec[j[2]]*Vec[j[3]]*Vec[j[4]]*Vec[j[5]]*Vec[j[6]];
		Phit += Coef[2][i] *Vecp;
		i++;
	}
	cout << i << " Xf:" << S12->Xf << "[mm] Thf:" << S12->Tf << "[mrad] Yf:" << S12->Yf << "[mm] Phf:" << S12->Pf <<"[mrad] " << endl; 
	cout <<"Brhot:"<< Brhot << " Thetat:" << Thetat*c180_OVER_PI/1000. << "[deg] Phit:" << Phit*c180_OVER_PI/1000. << "[deg] Patht:" << Patht<< "[cm]" << endl;  

	if(Brhot>0.001 && Thetat>-300. && Thetat<300. && Phit>-300. && Phit<300. && Patht>0 && Patht<2000.)
	{
		Counter[2]++;
		Present = true;

		BrhoRef = GetBrhoRef();   				// [Tm]
		Brho    = BrhoRef*((Float_t) Brhot);	// [Tm]
 		Path    = (Float_t) Patht;				// [cm]

//		printf("------------>%f %f ",Thetat,Phit);


/*
 *  	Superpose the border of a window, for test.
 */
		int	drawgrid,method;
		double t_min,t_max,t_cut,p_min,p_max,p_cut,r_target_window,theta,phi,theta_offaxis;

		drawgrid=1;
		if(drawgrid==1) if(drand48()>0.5)										/* abscissa */
		{
		t_min = 			-2. ;									/* deg		*/
		t_max = 			 2. ;									/* deg		*/
		p_min = 			-17.2 ;									/* deg		*/
		p_max = 			 17.2 ;									/* deg		*/
		r_target_window =	 139. ;									/* cm		*/
		theta_offaxis =		 0.6 ;									/* deg		*/
		AngleVamos = -(GetAngleVamos());

		t_cut = 			-7.21 ;									/* deg		*/
		t_cut -= theta_offaxis;
		AngleVamos += theta_offaxis;
		t_cut-=AngleVamos; if(t_cut<t_min||t_cut>t_max)t_cut=t_max;
 		Path    = 0;				// in order to select the grid

		if(drand48()>0.5)										/* abscissa */
		  {
			x = drand48();
//			if(x>0.6) phi=p_max;								/* up		*/
//			else if(x>0.3) phi=p_cut;							/* cut		*/
			if(x>0.5) phi=p_max;								/* up		*/
			else phi=p_min;										/* down		*/
			theta = drand48()*(t_max-t_min)+t_min;
		}
		else													/* ordinate */
		{
			x = drand48();
			if(x>0.6) theta=t_max;								/* right	*/
			else if(x>0.3) theta=t_cut;							/* cut	*/
			else theta=t_min; 									/* left		*/
			phi = drand48()*(p_max-p_min)+p_min;		
		}
 		Thetat  = -(theta+theta_offaxis)*1000.*PI/180.  ;		/* mrad		*/
 		Phit    = -phi*1000.*PI/180.    ;						/* mrad		*/

		printf("\t %f %f \n",Thetat,Phit);
		}



		method=2;
/*
 *		Method 1    THIS IS WRONG
 */
		if(method==1)
		{
/*
 *		Change of reference: sit on the target and watch beamward, 
 *		polar coordinates: 
 *		Theta from left(-PI) to right(+PI)
 *		Phi   from bottom(-PI) to top(+PI)
 */ 
 		AngleVamos = -(GetAngleVamos()*1000.*cPI_OVER_180);
 		Theta   = (Float_t)(-Thetat);			// [mrad]
 		Phi     = (Float_t)(-Phit);				// [mrad]
/*
 *		Change of reference: sit on the target and watch beamward, 
 *		cartesian coordinates (normalised to |vector|), ATTENTION: 
 *		x from left to right
 *		y from top to bottom
 */
 		x = sin(Theta/1000.)*cos(Phi/1000.);	// [rel.u.]
		y = sin(Phi/1000.);						// [rel.u.]
		z = cos(Theta/1000.)*cos(Phi/1000.);	// [rel.u.]
 /*
  *		Rotation of the vector around y, of the Vamos rotation angle
  *		and change of reference: sit on the target and watch beamward, 
  *		ThetaL from beam-axis(0) outward(+PI)
  *		PhiL clockwise from top(0) to +PI and counterclockwise from top(0) to -PI
  */
		TVector3 myVec(x,y,z);
		myVec.RotateY(AngleVamos); 		 	
 		ThetaL = myVec.Theta();					// [rad]
 		PhiL   = myVec.Phi();					// [rad]
//		if(PhiL<-PI)PhiL+=2.*PI; if(PhiL>PI)PhiL-=2.*PI;
		}


/*
 *		Method 2 :    THIS SEEMS CORRECT
 */
		if(method==2)
		{
/*
 *		Change of reference: sit on the target and watch beamward, 
 *		polar coordinates: 
 *		Theta from left(-PI) to right(+PI)
 *		Phi   from bottom(-PI) to top(+PI)
  *		Rotation of the vector around x, of the Vamos rotation angle
 */ 
 		AngleVamos = -(GetAngleVamos()*1000.*cPI_OVER_180);
 		Theta   = (Float_t)(-Thetat+AngleVamos);	  		// [mrad]
 		Phi     = (Float_t)(-Phit);				// [mrad]
/*
 *		Change of reference: sit on the target and watch beamward, 
 *		cartesian coordinates (normalised to |vector|), ATTENTION: 
 *		x from top to bottom
 *		y from left to right
 */
		x = sin(Phi/1000.);						// [rel.u.]
 		y = sin(Theta/1000.)*cos(Phi/1000.);	// [rel.u.]
		z = cos(Theta/1000.)*cos(Phi/1000.);	// [rel.u.]
 /*
  *		Change of reference: sit on the target and watch beamward, 
  *		ThetaL from beam-axis(0) outward(+PI)
  *		PhiL clockwise from top(0) to +PI and counterclockwise from top(0) to -PI
  */
		TVector3 myVec(x,y,z);
 		ThetaL = myVec.Theta();					// [rad]
 		PhiL   = myVec.Phi();					// [rad]
		if(PhiL<-PI)PhiL+=2.*PI; if(PhiL>PI)PhiL-=2.*PI;
		}

/*
 *		Method 3:    THIS SEEMS CORRECT
 */
		if(method==3)
		{
		
 		AngleVamos = -(GetAngleVamos()*1000.*cPI_OVER_180);
		Thetat *= -1.; Thetat += AngleVamos;
		Phit   *= -1.;

		x = cos(Phit/1000.)*sin(Thetat/1000.);
		y = sin(Phit/1000.);
		z = cos(Phit/1000.)*cos(Thetat/1000.);
/*
 *      thetaL	[rad]
 */
		if(z==0)              ThetaL_ = 0.5*PI ;
		else if(x==0 && y==0) ThetaL_ = 0. ; 
		else                  ThetaL_ = 0.5*PI-atan(z/sqrt(y*y+x*x)) ;
/*
 *      phiL	[rad]
 */
		if(x==0 && y==0) PhiL_ = 0. ;
		else if(x==0) {if(y>0) PhiL_=0.5*PI; else PhiL_=1.5*PI;}
        else if(y==0) {if(x>0) PhiL_=0.; else PhiL_=PI; }
        else
		{
			if(y>=0) PhiL_ = acos(x/sqrt(y*y+x*x)) ;
			if(y< 0) PhiL_ = 2.*PI-acos(x/sqrt(y*y+x*x)) ;
		}
		PhiL_*=-1.; PhiL_+=PI/2.; 						/* rad		*/
		if(PhiL_<-PI)PhiL_+=2.*PI; if(PhiL_>PI)PhiL_-=2.*PI;
/*
 *		Theta, Phi [mrad]. after the rotation of Vamos
 */			
 		Theta  = (Float_t) Thetat;						/* mrad		*/
 		Phi    = (Float_t) Phit;						/* mrad		*/
		PhiL   = (Float_t) PhiL_;						/* rad		*/
		ThetaL = (Float_t) ThetaL_;						/* rad		*/
		
		}

     
		cout <<" BrhoRef:"<<BrhoRef<<" Brho:"<< Brho << " AngleVamos:"<<AngleVamos*c180_OVER_PI <<"[deg] "<< endl;
		cout <<"Theta :"<< Theta*c180_OVER_PI/1000. << "[deg] Phi :" << Phi*c180_OVER_PI/1000.<<"[deg]" << endl;
		cout <<"ThetaL:"<< ThetaL*c180_OVER_PI << "[deg] PhiL:" << PhiL*c180_OVER_PI<<"[deg]" << endl;
	} 
}

void ReconstructionSeDv::Treat(void)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::Treat" << endl;
#	endif

	if(!Ready) return;
  
	Counter[0]++;
	if(S12->Present)
	{
		Counter[1]++;
		Init();
		Calculate();
		
#		ifdef DEBUG
		Show();
#		endif
	}
}

void ReconstructionSeDv::outAttach(TTree *outT)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::outAttach " << endl;
#	endif

#	ifdef DEBUG
	cout << "Attaching ReconstructionSeDv variables" << endl;
#	endif

	outT->Branch("Brho"  ,&Brho  , "Brho/F");   // [Tm]
	outT->Branch("Theta" ,&Theta , "Theta/F");  // [mrad]
	outT->Branch("Phi"   ,&Phi   , "Phi/F");    // [mrad]
	outT->Branch("Path"  ,&Path  , "Path/F");   // [cm]
	outT->Branch("ThetaL",&ThetaL, "ThetaL/F"); // [rad]
	outT->Branch("PhiL"  ,&PhiL  , "PhiL/F");   // [rad]
}

void ReconstructionSeDv::CreateHistograms(void)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::CreateHistograms " << endl;
#	endif
}

void ReconstructionSeDv::FillHistograms(void)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::FillHistograms " << endl;
#	endif
}

void ReconstructionSeDv::Show(void)
{
#	ifdef DEBUG
	cout << "ReconstructionSeDv::Show" << endl;
#	endif
	cout.setf(ios::showpoint);
}
