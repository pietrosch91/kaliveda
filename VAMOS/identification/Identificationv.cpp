#include "Identificationv.h"
#include "TMath.h"
#include <cmath>
#include <iostream>
#include "EnergyTree.h"
#include "KVNucleus.h"

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
				 DriftChamberv *Drift, IonisationChamberv *IonCh, Siv *SiD, CsIv *CsID)
{
#ifdef DEBUG
  cout << "Identificationv::Constructor" << endl;
#endif
  char line[255];
  char name[10];
  int len=255;
  int i;
  int tmp;

  L = Log;
  Rec = Recon;
  Dr = Drift;
  Ic = IonCh;
  Si = SiD;
  CsI=CsID;
  	
  for(i=0;i<6;i++)
    Counter[i] = 0;

  Init();

  Rnd = new Random;

  energytree=new EnergyTree(L);
  energytree->Init();

#ifdef FOLLOWPEAKS

  ifstream inf2("CgLast.cal");
  tmp = 0;
  if(!inf2)
  {
     cout << "Could not open the calibration file CgLast.cal !" << endl;

     ifstream inf3("CgFirst.cal");
     tmp = 0;
     if(!inf3)
       {
	 cout << "Could not open the calibration file CgFirst.cal !" << endl;
	 exit(-1);
       }
     else
       {
	 cout.setf(ios::showpoint);
	 cout << "Reading CgFirst.cal" << endl;
	 L->Log << "Reading CgFirst.cal" << endl;
	 inf3.getline(line,len);
	 cout << line << endl;
	 L->Log << line << endl;
	 while(!inf3.eof())
	   {
	     inf3.getline(line,len);
	     if(!inf3.eof() && (tmp < 21))
	       {
		 sscanf(line,"%f %f", CgM_v+tmp, CgM_Q_v+tmp);
		 tmp++;
	       }
	   }
       }
     inf3.close();
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Reading CgLast.cal" << endl;
      L->Log << "Reading CgLast.cal" << endl;
      inf2.getline(line,len);
      cout << line << endl;
      L->Log << line << endl;
      while(!inf2.eof())
	{
	  inf2.getline(line,len);
	  if(!inf2.eof() && (tmp < 21))
	    {
	      sscanf(line,"%f %f", CgM_v+tmp, CgM_Q_v+tmp);
	      tmp++;
	    }
	}
    }
  inf2.close();


  for(i=0;i<21;i++)
    {
      if( i==5 || i==6 || i==13 || i==14 || i==15)
	{
	  CgM[i] = 48.0;
	  CgM_Q[i] = 48.0/18.0;	  
	  //CgM_Q_v[i] = 48./18.;
	}
	else
	{
	  CgM[i] = 48.0;
	  CgM_Q[i] = 48.0/19.0;
	  //CgM_Q_v[i] = 48./19.;
	}
      //CgM_v[i] = 48.;
      myct[i] = 0;
      myct1[i] = 0;
    }

  for(i=0;i<21;i++)
    {
      //      cout << CgM_Q_v[i] << " " << CgM_v[i] << endl;
      L->Log << CgM_v[i] << " " << CgM_Q_v[i] << endl;
    }
#include "MyCuts.h" 

  for(i=0;i<21;i++)
    {
      if(myct[i])
	{
	  sprintf(name,"MYCUT%02d",i);
	  cout << "Printing " << name << endl;
	  myct[i]->Print();;
	}
    }
  for(i=0;i<21;i++)
    {
      if(myct1[i])
	{
	  sprintf(name,"MYCUT1_%02d",i);
	  cout << "Printing " << name << endl;
	  myct1[i]->Print();;
	}
    }



  ifstream inf("Sicorr.cal");

  
  tmp=0;
  if(!inf)
  {
     cout << "Could not open the calibration file Sicorr.cal !" << endl;
     exit(-1);
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Reading Sicorr.cal" << endl;
      L->Log << "Reading Sicorr.cal" << endl;
      inf.getline(line,len);
      cout << line << endl;
      L->Log << line << endl;
      while(!inf.eof())
	{
	  inf.getline(line,len);
	  if(!inf.eof() && (tmp < 21))
	    {
          Float_t dummy;
	      sscanf(line,"%f %f %f %f %f", M_QCoef[tmp]+0, M_QCoef[tmp]+1, M_QCoef[tmp]+2, &dummy, &dummy);
	      tmp++;
	    }
	}
    }
  inf.close();

  ifstream inf1("Sicorr1.cal");
  tmp=0;
  if(!inf1)
  {
     cout << "Could not open the calibration file Sicorr1.cal !" << endl;
     exit(-1);
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Reading Sicorr1.cal" << endl;
      L->Log << "Reading Sicorr1.cal" << endl;
      inf1.getline(line,len);
      cout << line << endl;
      L->Log << line << endl;
      while(!inf1.eof())
	{
	  inf1.getline(line,len);
	  if(!inf1.eof() && (tmp < 21))
	    {
          Float_t dummy;
	      sscanf(line,"%f %f %f %f %f", MCoef[tmp]+0, MCoef[tmp]+1, MCoef[tmp]+2, &dummy, &dummy);
	      tmp++;
	    }
	}
    }
  inf1.close();


#endif

}
Identificationv::~Identificationv(void)
{
#ifdef DEBUG

  cout << "Identificationv::Destructor" << endl;
#endif

  PrintCounters();

#ifdef FOLLOWPEAKS
  for(Int_t i=0;i<21;i++)
    {
      if(myct[i]) delete myct[i];
      if(myct1[i]) delete myct1[i];
    }
  ofstream ouf("CgLast.cal");
  if(!ouf)
  {
     cout << "Could not open the calibration file CgLast.cal for writing!" << endl;
     exit(-1);
  }
  else
    {
      cout.setf(ios::showpoint);
      cout << "Writing CgLast.cal" << endl;
      L->Log << "Writing CgLast.cal" << endl;
      ouf << "// CgM_v   CgM_Q_v" << endl;
      for(Int_t i=0;i<21;i++)
	{
	  ouf << CgM_v[i] << " " << CgM_Q_v[i] << endl;
	}
    }
  ouf.close();



#endif

}

void Identificationv::PrintCounters(void)
{
#ifdef DEBUG
  cout << "Identificationv::PrintCounters" << endl;
#endif
  cout << endl;
  cout << "Identificationv::PrintCounters" << endl;
  cout << "Called :" << Counter[0] << endl;
  cout << "Called with Rec Present :" << Counter[1] << endl;
  cout << "Present :" << Counter[2] << endl;

  L->Log << endl;
  L->Log << "Identificationv::PrintCounters" << endl;
  L->Log << "Called :" << Counter[0] << endl;
  L->Log << "Called with Rec Present :" << Counter[1] << endl;
  L->Log << "Present :" << Counter[2] << endl;


}

void Identificationv::Init(void)
{
  //#ifdef DEBUG
  //cout << "Identificationv::Init" << endl;
  //L->Log<< "Identificationv::Init" << endl;
  //#endif
  Present = false; 

  dE = dE1 = E = T = V = M_Q = M = Z1 = Z2 =  Beta = Q = D = -10;
  M_Qr = Mr = Qr = -10.0;
  Qc = Mc = -10.0;
  Gamma = 1.; 
	      
#ifdef FOLLOWPEAKS
  Mcorr = M_Qcorr = 0.0;
  M_Qcorr1 = 0.0;
  Mcorr1 = 0.0;
#endif
  for(Int_t i=0;i<5;i++)
    {
    dif1[i]=-10.0;
    dif2[i]=-10.0;
    As[i]=-10.0;
    ZZZ[i]=-10.0;
    }
        
zt = ZZ = AA = CsIRaw = SiRaw = DetSi = DetCsI = -10;
ESi = ECsI = -10.0;
}

void Identificationv::SetBrho(Double_t bbrho)
{
brho=bbrho;
}

Double_t Identificationv::GetBrho(void)
{
return brho;
}


void Identificationv::Calculate(void)
{
  //#ifdef DEBUG
  //cout << "Identificationv::Calculate" << endl;
  //L->Log<< "Identificationv::Calculate" << endl;
  //#endif
  
  L->Log<<"num si="<<int(Si->Number)<<endl;
  L->Log<<"num csi="<<int(CsI->Number)<<endl;
  

  energytree->SetSiliconThickness(Si->Number);
  
  L->Log<<"Thick : "<<energytree->thick<<" "<<"Det. Nb. : "<<Si->Number<<endl;
  
  energytree->SetCalibration(Si,CsI,Si->Number,CsI->Number);

  for(Int_t y=0;y< Si->E_RawM ;y++)	 
    {
      //L->Log<<"ch si="<<Si->E_Raw[y]<<endl<<flush;
      for(Int_t j=0;j< CsI->E_RawM;j++)	
	{
	  if(Geometry(Si->Number,CsI->Number)==1)			// if csi is behind the si
	    { 
	      CsIRaw = int(CsI->E_Raw[j]);
	      SiRaw = int(Si->E_Raw[y]);
	      L->Log<<"CsIRaw = "<<CsIRaw<<endl;
	      L->Log<<"SiRaw = "<<SiRaw<<endl;
	      //CsI->CsIRaw[CsI->Number] = CsI->E_Raw[j];			//Associer le canal au # du détecteur CsI
	      //Si->SiRaw[Si->Number] = Si->E_Raw[y];				//Associer le canal au # du détecteur Si
	      
	      //L->Log<<"ch csi="<<CsI->E_Raw[j]<<endl<<flush;
	      /*
	      	Double_t min,max;
  		min = max = rap[0];
		Float_t min2,max2;
  		min2 = max2 = val[0];
		*/
		Float_t min3,max3;
  		min3 = max3 = abs(diffsi[0]);
		Float_t min4,max4;
  		min4 = max4 = abs(difflum[0]);
		Float_t min5,max5;
		min5 = max5 = abs(diffsi[0]*difflum[0]);
		
		//energytree->eEnergySi=Si->ETotal;
/*
	      for(energytree->eZ=3;energytree->eZ<24;energytree->eZ++)
	      {
	      energytree->SetFragmentZ(energytree->eZ);
	      energytree->GetResidualEnergyCsI(Si->E_Raw[y],CsI->E_Raw[j],energytree->eZ);
		dif1[energytree->eZ-3]=energytree->diff1;
		dif2[energytree->eZ-3]=energytree->diff2;
		As[energytree->eZ-3]=energytree->sA;
		
		SiRef[energytree->eZ-3] = energytree->eEnergySi;
		ARetreive[energytree->eZ-3]=energytree->RetrieveA();
		CsIsRef[energytree->eZ-3] = energytree->sRefECsI;
		
		val[energytree->eZ-3]=abs(dif1[energytree->eZ-3]*dif2[energytree->eZ-3]);
		rap[energytree->eZ-3]=double(energytree->sA)/double(energytree->eZ);
		
		if(rap[energytree->eZ-3] < min)
      			min = rap[energytree->eZ-3];
    		else
      			max = rap[energytree->eZ-3];
		
		if(val[energytree->eZ-3] < min2)
      			min2 = val[energytree->eZ-3];
    		else
      			max2 = val[energytree->eZ-3];				
	      }
		
		//L->Log<<"Min : "<<min<<endl;
		
  		for(zt=0;zt<21;zt++)				
		{
			if(val[zt]==min2 && rap[zt]>1.3 && rap[zt]<2.8 && dif1[zt]>0 && dif2[zt]>0) //&& ARetreive[zt]>1 && ARetreive[zt]<100)	//&& CsIsRef[zt]>1e-8 && CsIsRef[zt]<1e+6) //&& (dif1[zt]>0 || dif2[zt]>0)) //|| val[zt]==min2)	
				break;	//rap[zt]==min
			//else if(val[zt]==min2 && )
				//break;	
		}
		
		
		if(ARetreive[zt]>100 || ARetreive[zt]<3 || isnan(double(ARetreive[zt]))==1 || isinf(double(ARetreive[zt]))==1)//(CsIsRef[zt]<1e-10 || CsIsRef[zt]>1e+6)	//ARetreive[zt]>100 || ARetreive[zt]<0 || //Si le min ne correspond pas a la bonne valeur de A
		{
  			for(zt=0;zt<21;zt++)				
			{
			if(rap[zt]<2.8 && (((zt==0 || zt==1) && val[zt]<400) || (((zt>1 && zt<10)||zt==10) &&
			val[zt]<1000) ||(zt>10 && val[zt]<20000)) )	//(CsIsRef[zt]>1e-8 && CsIsRef[zt]<1e+6) &&
				break;
			}
		}
*/
	for(energytree->eZ=3;energytree->eZ<24;energytree->eZ++)	//Modification : Boucle sur les valeurs des Z : 2010-05-11
  		{ 

		Int_t AAA=2*energytree->eZ;
		
		L->Log<<"-------------Cas A=2Z-------------------"<<endl;
		
		energytree->sA=AAA;
		energytree->SetFragmentZ(energytree->eZ);
		energytree->SetFragmentA(AAA);
		energytree->ECsIch(CsIRaw);
		energytree->CompleteSimulation(energytree->LightCsI);			//Simulation for the case A=2*Z
		
		//energytree->GetResidualEnergyCsI(Si->E_Raw[y],CsI->E_Raw[j]);
		
		L->Log<<"eZ = "<<energytree->eZ<<endl<<flush;
		L->Log<<"A = "<<energytree->sA<<endl<<flush;
		L->Log<<"Esi = "<<float(energytree->eEnergySi)<<endl;
		L->Log<<"Ecsi = "<<energytree->RetrieveEnergyCsI()<<endl;
		//L->Log<<"Canal csi Invert() = "<<energytree->CalculateCanalCsI()<<endl;
		L->Log<<"E tot = "<<double(energytree->eEnergySi+energytree->RetrieveEnergyCsI())<<endl;
		L->Log<<"Getincidentenergy = "<<energytree->detsi->GetIncidentEnergy(energytree->eZ,int(energytree->sA),energytree->eEnergySi)<<endl;	//AAA
		L->Log<<"Diffsi = "<<energytree->diffsi<<" "<<"Diffcsi = "<<energytree->diffcsi<<" "<<"Diffetot = "<<energytree->diffetot<<" "<<"Difflum = "<<energytree->difflum<<endl;
		L->Log<<"Diff lum %= "<<(energytree->CanalCsI-energytree->LightCsI)/energytree->LightCsI<<endl;
		L->Log<<"-------------Fin A=2Z-------------------"<<endl;
		
		
		diffsi[energytree->eZ-3]=abs(energytree->diffsi);			//Calculate the difference between the calibrate energy and the simulated one
		diffcsi[energytree->eZ-3]=abs(energytree->diffcsi);
		diffetot[energytree->eZ-3]=abs(energytree->diffetot);
		difflum[energytree->eZ-3]=abs(energytree->difflum);
		diffpro[energytree->eZ-3]=abs(energytree->difflum*energytree->diffsi);
				
		if(diffsi[energytree->eZ-3] < min3)
      			min3 = diffsi[energytree->eZ-3];
    		else
      			max3 = diffsi[energytree->eZ-3];
		.
		if(difflum[energytree->eZ-3] < min4)
      			min4 = difflum[energytree->eZ-3];
    		else
      			max4 = difflum[energytree->eZ-3];
			
		if(diffpro[energytree->eZ-3] < min5)
      			min5 = diffpro[energytree->eZ-3];
    		else
      			max5 = diffpro[energytree->eZ-3];
		}
		
		for(zt=0;zt<21;zt++)				
		{
			if(diffpro[zt]==min5)
				break;
		}
		L->Log<<"Pos min :"<<zt+1<<endl;
		L->Log<<"Z trouvé ="<<zt+3<<endl;
	

	Float_t min6,max6;	
	min6 = max6 = diffpro5[0];
			
	for(energytree->eZ=zt;energytree->eZ<zt+5;energytree->eZ++)			//Form the minimum found, isolate a "Z region"
	      {
	      energytree->SetFragmentZ(energytree->eZ);
	      energytree->SetFragmentA(energytree->sA);
	      energytree->GetResidualEnergyCsI(Si->E_Raw[y],CsI->E_Raw[j]);		//Identification method
	      
	        L->Log<<"eZ = "<<energytree->eZ<<endl<<flush;
		L->Log<<"A = "<<energytree->sA<<endl<<flush;
		
	        diffsi5[energytree->eZ-zt]= abs(energytree->diffsi);
		difflum5[energytree->eZ-zt]= abs(energytree->difflum);
	        diffpro5[energytree->eZ-zt]= abs(energytree->difflum*energytree->diffsi);
		
		ZZZ[energytree->eZ-zt]=energytree->eZ;
		As[energytree->eZ-zt]=energytree->sA;	
		SiRef[energytree->eZ-zt] = energytree->eEnergySi;
		ARetreive[energytree->eZ-zt]=energytree->RetrieveA();
		CsIsRef[energytree->eZ-zt] = energytree->sRefECsI;
			
		if(diffpro5[energytree->eZ-zt] < min6)
      			min6 = diffpro5[energytree->eZ-zt];
    		else
      			max6 = diffpro5[energytree->eZ-zt];
			
		L->Log<<"Ecsi interp="<<energytree->sRefECsI<<" A interp="<<energytree->iA<<endl;
		L->Log<<"E tot = "<<double(energytree->eEnergySi+energytree->RetrieveEnergyCsI())<<endl;
		L->Log<<"Getincidentenergy = "<<energytree->detsi->GetIncidentEnergy(energytree->eZ,int(energytree->sA),energytree->eEnergySi)<<endl;	
		L->Log<<"Diffsi = "<<energytree->diffsi<<" "<<"Diffcsi = "<<energytree->diffcsi<<" "<<"Diffetot = "<<energytree->diffetot<<" "<<"Difflum = "<<energytree->difflum<<endl;
		L->Log<<"Diff lum %= "<<(energytree->CanalCsI-energytree->LightCsI)/energytree->LightCsI<<endl;
		//L->Log<<"diffsi = "<<diffsi5[energytree->eZ-zt]<<" "<<"difflum = "<<difflum5[energytree->eZ-zt]<<" "<<"diffpro = "<<diffpro5[energytree->eZ-zt]<<endl;
		
	      }
	      
		Int_t ztt;
		for(ztt=0;ztt<5;ztt++)				
		{
			if(diffpro5[ztt]==min6)
				break;
		}
		
	        L->Log<<"Position finale : "<<ztt<<endl;
	      	ECsI = double(CsIsRef[ztt]);
		ESi = energytree->eEnergySi;
		ZZ = ZZZ[ztt];
		AA = int(ARetreive[ztt]);
		DetCsI = int(CsI->Number); 
		DetSi = int(Si->Number);
		//L->Log<<"Pos min (final):"<<zt+1<<endl;
		L->Log<<"Z trouvé(final)="<<ZZZ[ztt]<<" A trouvé(final)="<<AA<<endl;
		L->Log<<"ESi = "<<ESi<<" ECsI = "<<ECsI<<endl;
		
		/*		
		if(energytree->eZ==3 && abs(energytree->diff1*energytree->diff2)<40)	//Modification : Condition sur les différentes valeurs de Z : 2010-05-19
			break;
		else if(energytree->eZ==4 && abs(energytree->diff1*energytree->diff2)<55)
			break;
		else if(energytree->eZ==5 && abs(energytree->diff1*energytree->diff2)<300)
			break;
		else if(((energytree->eZ<7 && energytree->eZ>5 || energytree->eZ==7) && abs(energytree->diff1*energytree->diff2)<300)) 
			break;
		else if((((energytree->eZ<10 && energytree->eZ>7) || energytree->eZ==10 ) && abs(energytree->diff1*energytree->diff2)<800))
			break;
		else if((energytree->eZ>10 && abs(energytree->diff1*energytree->diff2)<2000))
		 	break;
		*/	 
		//&& abs(energytree->eEnergySi-energytree->esi1)<30) // && ((energytree->eZ<=6 && energytree->iA<20)||(energytree->eZ>6 && energytree->iA>10))) 
			//Condition sur les differences between the simulated energies in Si and calibrated one
				
		//if( energytree->ecsi1==0 && energytree->ecsi2==0)
			//energytree->iA=-10;			
/*
	      if(energytree->RetrieveA()<-10||energytree->RetrieveA()>200)	//Si A trouvée est -inf ou +inf
	      {
			energytree->GetResidualEnergyCsI(Si->E_Raw[y],CsI->E_Raw[j],energytree->eZ);
			if(abs(energytree->diff1*energytree->diff2)<10000)
				break;	   
	      }
*/
	      //L->Log<<"-----"<<endl;
	      /*
	      //L->Log<<"Einc MeV = "<<energytree->Einc<<endl;
	      L->Log<<"left="<<energytree->left<<" right="<<energytree->right<<endl;
	      L->Log<<"diff1 = "<<energytree->diff1<<" diff2 = "<<energytree->diff2<<endl;	
   	      L->Log<<"esi1="<<energytree->esi1<<" ecsi1="<<energytree->ecsi1<<" esi2="<<energytree->esi2<<" ecsi2="<<energytree->ecsi2<<" esi mis="<<energytree->eEnergySi<<endl;
       	      L->Log<<"Ecsi interp="<<energytree->sRefECsI<<" A interp="<<energytree->iA<<endl;
	      L->Log<<"-----"<<endl;
	      L->Log<<"Z found = "<<energytree->eZ<<endl;
	      L->Log<<"E silicio MeV="<<Si->ETotal<<endl;
	      L->Log<<"E CsI in MeV="<<energytree->sRefECsI<<endl;
	      L->Log<<"-----"<<endl;
	      */
	      
	      //L->Log<<"___________________________________________"<<endl;
 	      //L->Log<<"___________________________________________"<<endl; 
	    }
	}
    }


  //cout<<"I'm back..."<<endl<<flush;
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
      if((ESi+ECsI)>0)	//if(Si->ETotal > 0)		//Originalement : if(Si->ETotal>0)
	E = (dE/1000) + (ESi+ECsI)*0.99;
	//L->Log<<"dE1 = "<<dE1<<endl;
	L->Log<<"dE = "<<dE/1000<<endl;
	L->Log<<"E = "<<E<<endl;
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

T = Si->T[0]+12.69;

  if(T >0 && Rec->Path>0 && Dr->Present)
    {
      D = Rec->Path + (-1.*(Dr->Yf)/10.*sin(3.14159/4.)/
	cos(3.14159/4. + fabs(Dr->Pf/1000.)))/cos(Dr->Tf/1000.);
      //            cout << Rec->Path << " " << (Dr->Yf)/10. << " " <<
      //      	Rec->Phi/1000.*180/3.1415 << " " << D << endl;
      V = D/T;
      V = V+ V*(1-cos(Dr->Tf/1000.)*cos(Dr->Pf/1000.));
      Beta =V/29.9792; 
      Gamma = 1./sqrt(1.-TMath::Power(Beta,2.));
      L->Log<<"D = "<<D<<" Rec->Path = "<<Rec->Path<<" D-Path = "<<(-1.*(Dr->Yf)/10.*sin(3.14159/4.)/cos(3.14159/4. + fabs(Dr->Pf/1000.)))/cos(Dr->Tf/1000.)<<endl;
      L->Log<<"Beta = "<<Beta<<endl;
    }

  if(Beta>0 && Rec->Brho>0&&Gamma>1.&&Si->Present)
    {
      M = 2.* E / 931.5016/TMath::Power(Beta,2.);
      M_Q = Rec->Brho/3.105/Beta;
      L->Log<<"M = "<<M<<endl;
      L->Log<<"M/Q = "<<M_Q<<endl;

#ifdef FOLLOWPEAKS
      if(myct[Si->Number]->IsInside(M_Q,M))
	{
	  CgM_v[Si->Number] = 0.99*CgM_v[Si->Number] + 0.01*M;
	  CgM_Q_v[Si->Number] = 0.99*CgM_Q_v[Si->Number] + 0.01*M_Q;
	}
      {
	Mcorr = M + (CgM[Si->Number] - CgM_v[Si->Number]);
	M_Qcorr = M_Q + (CgM_Q[Si->Number] - CgM_Q_v[Si->Number]);
	for(Int_t k=0;k<3;k++)
	  {
	    M_Qcorr1 += powf(M_Qcorr,
			  (Float_t) k)*M_QCoef[Si->Number][k];	    
	  }
	for(Int_t k=0;k<3;k++)
	  {
	    Mcorr1 += powf(Mcorr,
			  (Float_t) k)*MCoef[Si->Number][k];	    
	  }
      }
#endif

      Mr = (E/1000.)/931.5016/(Gamma-1.);
      M_Qr = Rec->Brho/3.105/Beta/Gamma;

      Q = M/M_Q;
      Qr = Mr/M_Qr;
      Qr = (-1.036820+1.042380*Qr +0.4801678e-03*Qr*Qr);
      Qc = int(Qr+0.5);
      Mc = M_Qr*Qc;
    }
  
  Z1 = sqrt(dE*E/pow(931.5016,2.))/pow(29.9792,2.)*100.;
  Z2 = sqrt(dE/931.5016)*TMath::Power(Beta,2.)*100.;
//L->Log<<"Z1 = "<<Z1<<" Z2 = "<<Z2<<endl;


  if( T > 0 && V > 0 && M_Q > 0 && M > 0 && Z1 > 0 && Z2 > 0 && Beta > 0 && Gamma > 1.0) 
    {
      Present=true;
      Counter[2]++;
    }
}


void Identificationv::Treat(void)
{
#ifdef DEBUG
  cout << "Identificationv::Treat" << endl;
#endif

  Counter[0]++;
  Init();
  //if(Rec->Present) Counter[1]++;		//Non commenter
  Calculate();
  //#ifdef DEBUG
  Show();
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
	outT->Branch("iA",&AA,"iA/D");
   	outT->Branch("ZZ",&ZZ,"ZZ/D");
	outT->Branch("ESiRaw",&SiRaw,"SiRaw/I");
	outT->Branch("ECsIRaw",&CsIRaw,"CsIRaw/I");
	outT->Branch("ESi",&ESi,"ESi/D");
	outT->Branch("ECsI",&ECsI,"ECsI/D");
	outT->Branch("DetSi",&DetSi,"DetSi/I");
	outT->Branch("DetCsI",&DetCsI,"DetCsI/I");
	
	//outT->Branch("V",&V,"V/F");
	//outT->Branch("Beta",&Beta,"Beta/F");
  	//outT->Branch("Gamma",&Gamma,"Gamma/F");
	
	//outT->Branch("Q",&Q,"Q/F");
  	//outT->Branch("M",&M,"M/F");
	//outT->Branch("M_Q",&M_Q,"M_Q/F");
  	//outT->Branch("CsIE",ECsI,"ECsI[80]/F");
	//outT->Branch("SiE",ESi,"ESi[21]/F");
	//outT->Branch();

  outT->Branch("dE",&dE,"dE/F");
  outT->Branch("dE1",&dE1,"dE1/F");
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
  outT->Branch("Z1",&Z1,"Z1/F");
  outT->Branch("Z2",&Z2,"Z2/F");
#ifdef FOLLOWPEAKS
  outT->Branch("M_Qcorr",&M_Qcorr,"M_Qcorr/F");
  outT->Branch("Mcorr",&Mcorr,"Mcorr/F");
  outT->Branch("M_Qcorr1",&M_Qcorr1,"M_Qcorr1/F");
  outT->Branch("Mcorr1",&Mcorr1,"Mcorr1/F");
#endif

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
  int geom[18][6];
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
  geom[17][2]=57;
  geom[17][3]=58;
  geom[17][4]=68;
  geom[17][5]=69;


  for(int i=0;i<6;i++)
    {
      if(geom[sinum][i]==csinum)return 1;
    }
  return 0;
}
