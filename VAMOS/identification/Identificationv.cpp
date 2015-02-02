#include "Identificationv.h"
#include "TMath.h"
#include <cmath>
#include <iostream>
#include "CsICalib.h"
#include "KVNucleus.h"
#include "KVDetector.h"

#include "KVIDZAGrid.h"
#include "KVIDGridManager.h" 
#include "KVINDRADB_e503.h"
#include "KVINDRAe503.h"
#include "KVUnits.h"

//Author: Maurycy Rejmund
using namespace std;

Bool_t DebugIDV = kFALSE;

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
				 DriftChamberv *Drift, IonisationChamberv *IonCh, Sive503 *SiD, CsIv *CsID, CsICalib *E)
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
  kvn = new KVNucleus();
  
for(Int_t i=0;i<80;i++){
	P0[i] = 0.;
	P1[i] = 0.;
	P2[i] = 0.;
}
for(Int_t i=0;i<600;i++){	
	P0_mq[i] = 0.;
	P1_mq[i] = 0.;
	P0_mq_chiosi[i] = 0.;
	P1_mq_chiosi[i] = 0.;
	Tof0[i] = 0;
	Esi0[i] = 0;	
	for(Int_t j=0;j<25;j++){
		P0_m[i][j] = 0.;
		P1_m[i][j] = 0.;
		P0_m_chiosi[i][j] = 0.;
		P1_m_chiosi[i][j] = 0.;		
		
	}
}

//==========================================================================
// Correction de l'état de charge en fonction du détecteur CsI
// Charge state correction according to the CsI detector

Int_t csi=0;
Float_t p0=0.;
Float_t p1=0.;
Float_t p2=0.;
Float_t p3=0.;

ifstream file;	
TString sline;

   if(!gDataSet->OpenDataSetFile("q_function.dat",file))
  {	
     L->Log << "Could not open the calibration file q_function !!!" << endl;
     return;
  }
  else 
  {
  	L->Log<< "Reading q_function" <<endl;
	while (file.good()) {         //reading the file
      		sline.ReadLine(file);
      		if (!file.eof()) {          //fin du fichier
			if (sline.Sizeof() > 1 && !sline.BeginsWith("#")){
				sscanf(sline.Data(), "%u %f %f %f %f",
            				&csi, &p0, &p1, &p2, &p3);
				P0[csi] = p0;
				P1[csi] = p1;
				P2[csi] = p2;
				P3[csi] = p3;
				L->Log<<p0<<"	"<<p1<<"	"<<p2<<"	"<<p3<<endl;										
				}
			}
		}
  }
file.close();   
//==========================================================================

Int_t run1=0;
Int_t run2=0;
Float_t brho=0.;
Float_t chi2=0.;
Float_t p0mq=0.;
Float_t p1mq=0.;
Float_t p0m=0.;
Float_t p1m=0.;
Int_t q=0;

//==========================================================================
// Correction du rapport M/Q en fonction du Brho (ou RunNumber...)
// M/Q correction according to the Brho value (or the RunNumber...)
// Identification Si-CsI

ifstream file2;	
TString sline2;
   if(!gDataSet->OpenDataSetFile("mq_function.dat",file2))
  {	
     L->Log << "Could not open the calibration file q_function !!!" << endl;
     return;
  }
  else 
  {
  	L->Log<< "Reading mq_function" <<endl;
	while (file2.good()) {         //reading the file
      		sline2.ReadLine(file2);
      		if (!file2.eof()) {          //fin du fichier
			if (sline2.Sizeof() > 1 && !sline2.BeginsWith("#")){
				sscanf(sline2.Data(), "%f %u %u %f %f %f",
            				&brho, &run1, &run2, &p0mq, &p1mq, &chi2);
					for(Int_t i=run1; i<run2+1; i++){
						P0_mq[i] = p0mq;
						P1_mq[i] = p1mq;
						//L->Log<<p0mq<<"	"<<p1mq<<endl;
					}										
				}
			}
		}
 }
	
file2.close();
//==========================================================================
//==========================================================================
// Correction du rapport M/Q en fonction du Brho (ou RunNumber...)
// M/Q correction according to the Brho value (or the RunNumber...)
// Identification Chio-Si

ifstream file22;	
TString sline22;
   if(!gDataSet->OpenDataSetFile("mq_function_chiosi.dat",file22))
  {	
     L->Log << "Could not open the calibration file mq_function_chiosi !!!" << endl;
     return;
  }
  else 
  {
  	L->Log<< "Reading mq_function_chiosi" <<endl;
	while (file22.good()) {         //reading the file
      		sline22.ReadLine(file22);
      		if (!file22.eof()) {          //fin du fichier
			if (sline22.Sizeof() > 1 && !sline22.BeginsWith("#")){
				sscanf(sline22.Data(), "%f %u %u %f %f %f",
            				&brho, &run1, &run2, &p0mq, &p1mq, &chi2);
					for(Int_t i=run1; i<run2+1; i++){
						P0_mq_chiosi[i] = p0mq;
						P1_mq_chiosi[i] = p1mq;
						//L->Log<<p0mq<<"	"<<p1mq<<endl;
					}										
				}
			}
		}
 }
	
file22.close();
//==========================================================================


//==========================================================================
// Correction de la masse en fonction du Brho (ou RunNumber...) et de l'état de charge entier Q
// Mass correction according to the Brho value et charge state value Q (which is an interger in that case...) 
// Identification Si-CsI
ifstream file3;	
TString sline3;   

if(!gDataSet->OpenDataSetFile("m_function.dat",file3))
  {	
     L->Log<< "Could not open the calibration file m_function !!!" << endl;
     return;
  }
  else 
  {
  	L->Log<< "Reading m_function" <<endl;  
	while (file3.good()) {         //reading the file
      		sline3.ReadLine(file3);
      			if (!file3.eof()) {          //fin du fichier
				if (sline3.Sizeof() > 1 && !sline3.BeginsWith("#")){
					sscanf(sline3.Data(), "%f %u %u %u %f %f %f",
            					&brho, &run1, &run2, &q, &p0m, &p1m, &chi2);
						for(Int_t i=run1; i<run2+1; i++){
							P0_m[i][q] = p0m;
							P1_m[i][q] = p1m;
							//L->Log<<p0m<<"	"<<p1m<<endl;
						}										
					}
				}
			}
 }
file3.close();
//==========================================================================
//==========================================================================
// Correction de la masse en fonction du Brho (ou RunNumber...) et de l'état de charge entier Q
// Mass correction according to the Brho value et charge state value Q (which is an interger in that case...) 
// Identification Chio-Si

ifstream file33;	
TString sline33;   

if(!gDataSet->OpenDataSetFile("m_function_chiosi.dat",file33))
  {	
     L->Log<< "Could not open the calibration file m_function !!!" << endl;
     return;
  }
  else 
  {
  	L->Log<< "Reading m_function_chiosi" <<endl;  
	while (file33.good()) {         //reading the file
      		sline33.ReadLine(file33);
      			if (!file33.eof()) {          //fin du fichier
				if (sline33.Sizeof() > 1 && !sline33.BeginsWith("#")){
					sscanf(sline33.Data(), "%f %u %u %u %f %f %f",
            					&brho, &run1, &run2, &q, &p0m, &p1m, &chi2);
						for(Int_t i=run1; i<run2+1; i++){
							P0_m_chiosi[i][q] = p0m;
							P1_m_chiosi[i][q] = p1m;
							//L->Log<<p0m<<"	"<<p1m<<endl;
						}										
					}
				}
			}
 }
file33.close();
//==========================================================================

//==========================================================================
// Tag the event according to the fragment identification (Z,A,Q),
// Brho0 (or RunNumber) and system

Int_t z;
Int_t a;
//Int_t q;
Float_t brho0;
//Int_t run1;
//Int_t run2;
Float_t brhomin;
Float_t brhomax;

for(Int_t i=0;i<25;i++){
	for(Int_t j=0;j<60;j++){
		for(Int_t k=0;k<10;k++){
			for(Int_t l=0;l<600;l++){
				Brho_min[i][j][k][l] = -10.0;
				Brho_max[i][j][k][l] = -10.0;
			}
		}
	}
}


ifstream file4;	
TString sline4;

if(!gDataSet->OpenDataSetFile("Flag_CodeVamos.dat",file4))
  {	
     L->Log<< "Could not open the calibration file Flag_CodeVamos !!!" << endl;
     return;
  }
  else 
  {
  	L->Log<< "Reading Flag_CodeVamos" <<endl; 
	while (file4.good()) {         //reading the file
		sline4.ReadLine(file4);
			if (!file4.eof()) {          //fin du fichier
				if (sline4.Sizeof() > 1 && !sline4.BeginsWith("#")){
					sscanf(sline4.Data(), "%u %u %u %f %u %u %f %f",
    					&z,&a,&q,&brho0,&run1,&run2,&brhomin,&brhomax);
					for(Int_t i=run1; i<run2+1; i++){			
						Brho_min[z][a][q][i] = brhomin;
						Brho_max[z][a][q][i] = brhomax;			
						}
					if(run1==run2)
						{
						Brho_min[z][a][q][run1] = brhomin;
						Brho_max[z][a][q][run2] = brhomax;					
						}
					if(TMath::Abs(run2-run1)==1)
						{
						Brho_min[z][a][q][run1] = brhomin;
						Brho_max[z][a][q][run1] = brhomax;
						Brho_min[z][a][q][run2] = brhomin;
						Brho_max[z][a][q][run2] = brhomax;										
						}				
														
					}
				}
			}
	}
file4.close();
//==========================================================================

//==========================================================================
// Correction du temps de vol pour les events Chio-Si (Code_Ident_Vamos==2)
Int_t t0;
Int_t esi0;

ifstream file5;	
TString sline5;

if(!gDataSet->OpenDataSetFile("Tof_corr.dat",file5) )
  {	
     L->Log<< "Could not open the calibration file Tof_corr.dat !!!" << endl;
     return;
  }
else{
	while (file5.good()) {         //reading the file
		sline5.ReadLine(file5);
      		if (!file5.eof()) {          //fin du fichier
			if (sline5.Sizeof() > 1 && !sline5.BeginsWith("#")){
				sscanf(sline5.Data(), "%f %u %u %u %u",
            				&brho0, &run1, &run2, &t0, &esi0);
					for(Int_t i=run1; i<run2+1; i++){
						Tof0[i] = t0;
						Esi0[i] = esi0;
						}									
					}
				}
			}
  }
file5.close();
//==========================================================================

//==========================================================================
// Calculate the total statistic for each nominal Brho.

//Float_t brho0;
ifstream file6;	
TString sline6;

stat_indra = -10.0;
for(Int_t i=0;i<600;i++){
	Stat_Indra[i] = -10.0;
	Stat_Indra[i] = -10.0;
}

if(!gDataSet->OpenDataSetFile("Stat_Indra.dat",file6))
  {	
     L->Log<< "Could not open the calibration file  Stat_Indra!!!" << endl;
     return;
  }
  else 
  {
   while (file6.good()) {         //reading the file
      sline6.ReadLine(file6);
      if (!file6.eof()) {          //fin du fichier
		if (sline6.Sizeof() > 1 && !sline6.BeginsWith("#")){		  	
			sscanf(sline6.Data(),"%f %d %d %f", &brho0, &run1, &run2, &stat_indra);
				//cout<<"stat indra : "<<stat_indra<<endl;				
				for(Int_t i=run1; i<run2+1; i++){			
					Stat_Indra[i] = stat_indra;			
					}
				if(run1==run2)
					{
					Stat_Indra[run1] = stat_indra;					
					}
				if(TMath::Abs(run2-run1)==1)
					{
					Stat_Indra[run1] = stat_indra;
					Stat_Indra[run2] = stat_indra;										
					}		
		
			}
		}
	}
}
file6.close();				
//==========================================================================  
}

Identificationv::~Identificationv(void)
{

delete Rnd;
delete id;
delete kvn;
}

void Identificationv::Init(void)
{
  Present = false; 

  dE = dE1 = E = T = V = Vx = Vy = Vz = V2 = V_Etot = T_FP = M_Q = M = Mass = M_simul = Z1 = Z2 = Z_tot = Z_si = DetCsI = Beta = Q = D = -10;
  M_Qr = Mr = Qr = -10.0;
  Qc = Mc = -10.0;
  Gamma = 1.; 
 initThickness=0.;      
zt = ZZ  = CsIRaw = SiRaw = i =  -10;
ESi = ECsI = ECsI_corr = E_corr = EEtot  = AA =  ZR = -10.0;
PID = PID_chiosi = Z_PID = A_PID = -10.0;

Code_Vamos = -10;	//Flag on Brho distributions
Code_Ident_Vamos = -10;	//Vamos Identification Code : Chio-Si, Si-CsI, other...

einc_si = einc_isogap1 = eloss_isogap1 = einc_ic = eloss_ic = einc_dc2 = eloss_dc2 = einc_sed = eloss_sed = einc_dc1 = eloss_dc1 = einc_tgt = eloss_tgt = 0.0;
E_tgt = E_dc1 = E_dc2 = E_sed = E_gap1 = E_chio = 0.0;

for(Int_t i=0; i<3; i++){
 fELosLayer_dc1[i]= 0.0;
 fELosLayer_dc2[i]= 0.0;
 fELosLayer_ic[i]= 0.0;
}
    runNumber = 0;
    runNumber = (Int_t)gIndra->GetCurrentRunNumber();
}

//===================================================
// Gates en Brho et état de charge pour la détermination des états de charges
// Brho/Q gates to determine the charge state value (interger)
// Identification Si-CsI
void Identificationv::SetFileCut(TList *list)
{
	llist = list;	
}
void Identificationv::GetFileCut()
{	
q20 =NULL;
q19 =NULL;
q18 =NULL;
q17 =NULL;
q16 =NULL;
q15 =NULL;
q14 =NULL;
q13 =NULL;
q12 =NULL;
q11 =NULL;
q10 =NULL;
q9 =NULL;
q8 =NULL; 
q7 =NULL; 
q6 =NULL; 
q5 =NULL; 

	if(llist->IsZombie()==0){
		//L->Log<<"reading cuts..."<<endl;	
		//llist->Print();
		
		q20 = (TCutG *)llist->FindObject("q20");
		q19 = (TCutG *)llist->FindObject("q19");
		q18 = (TCutG *)llist->FindObject("q18");
		q17 = (TCutG *)llist->FindObject("q17");
		q16 = (TCutG *)llist->FindObject("q16");
		q15 = (TCutG *)llist->FindObject("q15");
		q14 = (TCutG *)llist->FindObject("q14");
		q13 = (TCutG *)llist->FindObject("q13");				
		q12 = (TCutG *)llist->FindObject("q12");
		q11 = (TCutG *)llist->FindObject("q11");
		q10 = (TCutG *)llist->FindObject("q10");
		q9 = (TCutG *)llist->FindObject("q9");
		q8 = (TCutG *)llist->FindObject("q8");
		q7 = (TCutG *)llist->FindObject("q7");
		q6 = (TCutG *)llist->FindObject("q6");
		q5 = (TCutG *)llist->FindObject("q5");

	}
	else{
		L->Log<<"not reading cuts (Si-CsI)..."<<endl;
	}
}
//===================================================

//===================================================
// Gates en Brho et état de charge pour la détermination des états de charges
// Brho/Q gates to determine the charge state value (interger)
// Identification Chio-Si
void Identificationv::SetFileCutChioSi(TList *list2)
{
	llist2 = list2;	
}
void Identificationv::GetFileCutChioSi()
{
q20cs =NULL;
q19cs =NULL;
q18cs =NULL;
q17cs =NULL;
q16cs =NULL;
q15cs =NULL;
q14cs =NULL;
q13cs =NULL;
q12cs =NULL;
q11cs =NULL;
q10cs =NULL;
q9cs =NULL;
q8cs =NULL;
q7cs =NULL;

	if(llist2->IsZombie()==0){
		//L->Log<<"reading cuts..."<<endl;	
		//llist->Print();
		
		if(((TCutG *)llist2->FindObject("q20"))!=0) q20cs = (TCutG *)llist2->FindObject("q20");
		if(((TCutG *)llist2->FindObject("q19"))!=0) q19cs = (TCutG *)llist2->FindObject("q19");
		if(((TCutG *)llist2->FindObject("q18"))!=0) q18cs = (TCutG *)llist2->FindObject("q18");
		if(((TCutG *)llist2->FindObject("q17"))!=0) q17cs = (TCutG *)llist2->FindObject("q17");
		if(((TCutG *)llist2->FindObject("q16"))!=0) q16cs = (TCutG *)llist2->FindObject("q16");
		if(((TCutG *)llist2->FindObject("q15"))!=0) q15cs = (TCutG *)llist2->FindObject("q15");
		if(((TCutG *)llist2->FindObject("q14"))!=0) q14cs = (TCutG *)llist2->FindObject("q14");
		if(((TCutG *)llist2->FindObject("q13"))!=0) q13cs = (TCutG *)llist2->FindObject("q13");				
		if(((TCutG *)llist2->FindObject("q12"))!=0) q12cs = (TCutG *)llist2->FindObject("q12");
		if(((TCutG *)llist2->FindObject("q11"))!=0) q11cs = (TCutG *)llist2->FindObject("q11");
		if(((TCutG *)llist2->FindObject("q10"))!=0) q10cs = (TCutG *)llist2->FindObject("q10");
		if(((TCutG *)llist2->FindObject("q9"))!=0) q9cs = (TCutG *)llist2->FindObject("q9");
		if(((TCutG *)llist2->FindObject("q8"))!=0) q8cs = (TCutG *)llist2->FindObject("q8");
		if(((TCutG *)llist2->FindObject("q7"))!=0) q7cs = (TCutG *)llist2->FindObject("q7");

	}
	else{
		L->Log<<"not reading cuts (Chio-Si)..."<<endl;
	}
}
//===================================================

//===================================================
void Identificationv::SetTarget(KVTarget *tgt)
{
	ttgt = tgt;
}
void Identificationv::SetDC1(KVDetector *dcv1)
{
	ddcv1 = dcv1; 	
}
void Identificationv::SetSed(KVMaterial *sed)
{
	ssed = sed;	
}
void Identificationv::SetDC2(KVDetector *dcv2)
{
	ddcv2 = dcv2;	
}
void Identificationv::SetIC(KVDetector *ic)
{
	iic = ic;	
}
void Identificationv::SetGap1(KVMaterial *isogap1)
{
	iisogap1 = isogap1;	
}
void Identificationv::SetSi(KVMaterial *si)
{
	ssi = si;	
}
void Identificationv::SetGap2(KVMaterial *isogap2)
{
	iisogap2 = isogap2;	
}
void Identificationv::SetCsI(KVMaterial *csi)
{
	ccsi = csi;	
}
//===================================================

//===================================================
KVTarget* Identificationv::GetTarget()
{
	return ttgt;
}
KVDetector* Identificationv::GetDC1()
{
	return ddcv1; 	
}
KVMaterial* Identificationv::GetSed()
{
	return ssed;	
}
KVDetector* Identificationv::GetDC2()
{
	return ddcv2;	
}
KVDetector* Identificationv::GetIC()
{
	return iic;	
}
KVMaterial* Identificationv::GetGap1()
{
	return iisogap1;	
}
KVMaterial* Identificationv::GetSi()
{
	return ssi;	
}
KVMaterial* Identificationv::GetGap2()
{
	return iisogap2;	
}
KVMaterial* Identificationv::GetCsI()
{
	return ccsi;	
}
//===================================================


//===================================================

Double_t Identificationv::GetEnergyLossCsI(Int_t number)
{
	GetSi()->SetThickness(Si->si_thick[number]*KVUnits::um);		

	//Calcul de l'énergie perdue dans csi
	einc_isogap2 = GetSi()->GetEResFromDeltaE(int(Z_PID),int(M_Q*Z_PID),ESi);
	eloss_isogap2 = GetGap2()->GetDeltaE(int(Z_PID),int(M_Q*Z_PID),einc_isogap2);	
	
	einc_csi = GetGap2()->GetEResFromDeltaE(int(Z_PID),int(M_Q*Z_PID),eloss_isogap2);
	eloss_csi = GetCsI()->GetDeltaE(int(Z_PID),int(M_Q*Z_PID),einc_isogap2);	
	GetSi()->Clear();
	
	return eloss_csi;
}

Double_t Identificationv::GetEnergyLossGap2(Int_t number)
{
	GetSi()->SetThickness(Si->si_thick[number]*KVUnits::um);		

	//Calcul de l'énergie perdue dans isogap2
	einc_isogap2 = GetSi()->GetEResFromDeltaE(int(Z_PID),int(M_Q*Z_PID),ESi);
	eloss_isogap2 = GetGap2()->GetDeltaE(int(Z_PID),int(M_Q*Z_PID),einc_isogap2);
	
	GetSi()->Clear();
	
	return eloss_isogap2;
}

Double_t Identificationv::GetEnergyLossGap1(Int_t number)
{
	GetSi()->SetThickness(Si->si_thick[number]*KVUnits::um);	
	einc_si = GetSi()->GetIncidentEnergy(int(Z_PID),int(M_Q*Z_PID),ESi);

	//Calcul de l'énergie perdue dans isogap1
	einc_isogap1 = GetGap1()->GetIncidentEnergyFromERes(int(Z_PID),int(M_Q*Z_PID),einc_si);
	eloss_isogap1 = GetGap1()->GetDeltaEFromERes(int(Z_PID),int(M_Q*Z_PID),einc_si);
	
	GetSi()->Clear();
	
	return eloss_isogap1;
}

Double_t Identificationv::GetEnergyLossChio()	
{
	//Calcul de l'énergie perdue dans la chio
	einc_ic = GetIC()->GetIncidentEnergyFromERes(int(Z_PID),int(M_Q*Z_PID),einc_isogap1);

	kvn->SetZ(int(Z_PID));
	kvn->SetA(int(M_Q*Z_PID));
	kvn->SetEnergy(einc_ic); 
    	KVMaterial *kvm_ic = 0; 
	      
    for(Int_t i=0; i<2; i++){	//Take account only the first two layers, because the calibration gives the energy for the active layer, which is the last layer...
    	fELosLayer_ic[i] = 0.;
	kvm_ic = (KVMaterial*) GetIC()->GetAbsorber(i);
        kvm_ic->DetectParticle(kvn);
	
        fELosLayer_ic[i] = kvm_ic->GetEnergyLoss();
        eloss_ic += fELosLayer_ic[i];	
	}
	
	kvn->Clear();
	GetIC()->Clear();
	
	return eloss_ic;
}

Double_t Identificationv::GetEnergyLossDC2()
{
	//Calcul de l'énergie perdue dans la DC2
	einc_dc2 = GetDC2()->GetIncidentEnergyFromERes(int(Z_PID),int(M_Q*Z_PID),einc_ic);

	kvn->SetZ(int(Z_PID));
	kvn->SetA(int(M_Q*Z_PID));
	kvn->SetEnergy(einc_dc2); 
    	KVMaterial *kvm_dc2 = 0;  
		     
    for(Int_t i=0; i<3; i++){
    	fELosLayer_dc2[i] = 0.;
	kvm_dc2 = (KVMaterial*) GetDC2()->GetAbsorber(i);
        kvm_dc2->DetectParticle(kvn);
	
        fELosLayer_dc2[i] = kvm_dc2->GetEnergyLoss();
        eloss_dc2 += fELosLayer_dc2[i];	
	}
	
	kvn->Clear();
	GetDC2()->Clear();
	
	return eloss_dc2;
}

Double_t Identificationv::GetEnergyLossSed()
{
	//Calcul de l'énergie perdue dans la SED    
	TVector3 rot(0,1,-1); //45 deg 
	einc_sed = GetSed()->GetIncidentEnergyFromERes(int(Z_PID),int(M_Q*Z_PID),einc_dc2);
	
	kvn->SetZ(int(Z_PID));
	kvn->SetA(int(M_Q*Z_PID));
	kvn->SetEnergy(einc_sed);

    	GetSed()->DetectParticle(kvn,&rot);
    	eloss_sed = GetSed()->GetEnergyLoss();
	
	//einc_sed = GetSed()->GetIncidentEnergyFromERes(int(Z_PID),int(M_Q*Z_PID),einc_dc2);
	//eloss_sed = GetSed()->GetDeltaEFromERes(int(Z_PID),int(M_Q*Z_PID),einc_dc2);

	GetSed()->Clear();
	
	return eloss_sed;
}

Double_t Identificationv::GetEnergyLossDC1()
{
	//Calcul de l'énergie perdue dans la DC1
	einc_dc1 = GetDC1()->GetIncidentEnergyFromERes(int(Z_PID),int(M_Q*Z_PID),einc_sed);

	kvn->SetZ(int(Z_PID));
	kvn->SetA(int(M_Q*Z_PID));
	kvn->SetEnergy(einc_dc1); 
    	KVMaterial *kvm_dc1 = 0;  
		     
    for(Int_t i=0; i<3; i++){
    	fELosLayer_dc1[i] = 0.;
	kvm_dc1 = (KVMaterial*)GetDC1()->GetAbsorber(i);
        kvm_dc1->DetectParticle(kvn);
	
        fELosLayer_dc1[i] = kvm_dc1->GetEnergyLoss();
        eloss_dc1 += fELosLayer_dc1[i];	
	}
	
	kvn->Clear();
	GetDC1()->Clear();
	
	return eloss_dc1;
}

Double_t Identificationv::GetEnergyLossTarget()
{
	//Calcul de l'énergie perdue dans la cible
	einc_tgt = GetTarget()->GetIncidentEnergyFromERes(int(Z_PID),int(M_Q*Z_PID),einc_dc1);
	eloss_tgt = GetTarget()->GetDeltaEFromERes(int(Z_PID),int(M_Q*Z_PID),einc_dc1);
	
	GetTarget()->Clear();
	
	return einc_tgt-einc_dc1;
	//return eloss_tgt;
}

//===================================================

void Identificationv::Calculate(void)
{
	
//=================================================
// Identification Chio-Si

if(CsI->EM==0 && (GeometryChioSi(Ic->Number,Si->Number)==1 && ((Ic->Number!=0 && Si->Number!=0) ||(Ic->Number==0 && Ic->Number==0))) ){ // if si is behind the ic
	energytree->InitTelescopeChioSi(Ic->Number,Si->Number);	//Ic->Number (1-7), Si->Number(0,17)
	//L->Log<<"====="<<endl;
	//L->Log<<"IC Number : "<<Ic->Number<<"	SI Number : "<<Si->Number<<endl;
	//L->Log<<"IC M : "<<Ic->EM<<"	SI M : "<<Si->E_RawM<<"	CSI M : "<<CsI->EM<<endl;		    

 for(Int_t y=0;y< Si->E_RawM ;y++)	 
 	{
	for(Int_t j=0;j< Ic->EM;j++)		
		{	
   		KVList *grid_list_chiosi = 0;
   		id_chiosi = new KVIdentificationResult();
		id_sitof = new KVIdentificationResult();
   		char scope_name_chiosi [256];
  		sprintf(scope_name_chiosi, "null");
   		sprintf(scope_name_chiosi,"%s", energytree->kvid_chiosi->GetName());
   		char scope_name_sitof [256];
  		sprintf(scope_name_sitof, "null");
   		sprintf(scope_name_sitof,"SIE%d_TOF", Si->Number+1);
		
   	   	if(gIDGridManager != 0){
   			grid_list_chiosi = (KVList*) gIDGridManager->GetGrids();

   			if(grid_list_chiosi == 0)	printf("Error: gIDGridManager->GetGrids() failed\n");
			
			else{
				KVIDGraph *grd = 0;

   			if( (grd = (KVIDGraph*) grid_list_chiosi->FindObjectByName(scope_name_chiosi)) != 0){

                		if(grd != 0 && double(Si->E_Raw[y])>0 && double(Ic->E_Raw[j])>0 && Si->E_RawM==1 && Ic->EM==1){
					ESi = Si->ETotal;
					EChio = Ic->ETotal;
					//L->Log<<"EChio : "<<EChio<<" ESi : "<<ESi<<endl;
					if(energytree->kvid_chiosi->IsIdentifiable(ESi, EChio) ){
						Code_Ident_Vamos = 2;
						energytree->kvid_chiosi->Identify(ESi, EChio, id_chiosi);		//energytree->kvid : KVIDGraph	Identify(x,y)
                                        	A_PID = id_chiosi->A;
                        			Z_PID = id_chiosi->Z;
                       				PID = id_chiosi->PID;

						//L->Log<<"PID : "<<id_chiosi->PID<<endl;     
						//L->Log<<"====="<<endl;
						delete id_chiosi;
							}	//IsIdentifiable
					else{	//Punch-through region.
						Code_Ident_Vamos = 4;
						
						if(grid_list_chiosi == 0)	printf("Error: gIDGridManager->GetGrids() failed\n");
	
						else{
							energytree->InitTelescopeSiTof(Si->Number);	//Si->Number(0,17)	
							if( (grd = (KVIDGraph*) grid_list_chiosi->FindObjectByName(scope_name_sitof)) != 0){

								if(grd != 0 && energytree->kvid_sitof->IsIdentifiable(Si->Tfrag, ESi) ){	//T Correspondant au tof sans correction du pid
									energytree->kvid_sitof->Identify(Si->Tfrag, ESi, id_sitof);		//energytree->kvid : KVIDGraph	Identify(x,y)
                        						A_PID = id_sitof->A;
        								Z_PID = id_sitof->Z;
       									PID = id_sitof->PID;
									Code_Ident_Vamos=10;
									//Verification Reconstruction
									/*L->Log<<"=================================="<<endl;
									L->Log<<"Code_Ident_Vamos : "<<Code_Ident_Vamos<<endl;
									L->Log<<"Brho : "<<Rec->Brho<<endl;
									L->Log<<"Thetadeg : "<<Rec->Thetadeg<<"	Phideg : "<<Rec->Phideg<<endl;
									L->Log<<"ThetaLdeg : "<<Rec->ThetaLdeg<<"	PhiLdeg : "<<Rec->PhiLdeg<<endl;
									L->Log<<"=================================="<<endl;*/
				
									delete id_sitof;
								}	//IsIdentifiable
							else{	//Punch-through region.
									Code_Ident_Vamos=11;
								}							

							}	//grd !=0
						}	//else							
						
					}						
					/*L->Log<<"PID : "<<id_chiosi->PID<<endl;
					L->Log<<"Code_Ident_Vamos : "<<Code_Ident_Vamos<<endl;     
					L->Log<<"====="<<endl;*/
						}	//grd!=0 && Signals>0
					}	//grd !=0
				}	//else	
			}	//gIDGridManager!=0
		}	//Ic M
	}	//Si M
}	//Geometry
//=================================================


//================================================= 
// Identification Si-CsI

if(Geometry(Si->Number,CsI->Number)==1 && ((Si->Number!=0 && CsI->Number!=0) ||(Si->Number==0 && CsI->Number==0))){ // if csi is behind the si
	energytree->InitTelescope(Si->Number,CsI->Number);
	energytree->InitSiCsI(Si->Number+1);
	energytree->SetCalibration(Si,CsI,Si->Number,CsI->Number);
}

 for(Int_t y=0;y< Si->E_RawM ;y++)	 
 	{
	for(Int_t j=0;j< CsI->E_RawM;j++)	
		{		      
		CsIRaw = int(CsI->E_Raw[j]);
		SiRaw = int(Si->E_Raw[y]);
		if(SiRaw>0){
			energytree->CalculateESi(double(Si->E_Raw[y]));		
			ESi = double(energytree->RetrieveEnergySi());	      
	      		}
			if(Geometry(Si->Number,CsI->Number)==1 && energytree->kvid != 0) // if csi is behind the si
   				{				   
   				//L->Log<<"name : "<<energytree->kvid->GetName()<<endl;
   				//L->Log<<"Runs : "<<energytree->kvid->GetRuns()<<endl;
  				
   				KVList *grid_list = 0;
   				id = new KVIdentificationResult();
   				char scope_name [256];
  				sprintf(scope_name, "null");
   				sprintf(scope_name,"%s", energytree->kvid->GetName());//COMMENT THIS OUT TO AVOID SEG FAULT 17/09/12 Paola

   	   			if(gIDGridManager != 0){
   		   			grid_list = (KVList*) gIDGridManager->GetGrids();

   		   			if(grid_list == 0){
			   			printf("Error: gIDGridManager->GetGrids() failed\n");
   		   				}
					else{
			   			KVIDGraph *grd = 0;

   					if( (grd = (KVIDGraph*) grid_list->FindObjectByName(scope_name)) != 0){

                    				if(grd != 0 && double(Si->E_Raw[y])>0 && double(CsI->E_Raw[j])>0 && CsI->EM==1 && Si->E_RawM==1 && Ic->EM==1){
                                 
                                if(DebugIDV) 
		    					energytree->CalculateESi(double(Si->E_Raw[y]));						//Si calibration (signal->energy)
							
							//Identification according to the grid (csi,si)
                         	if(DebugIDV) cout<<"Now identify from kvid"<<endl<<flush;
							//L->Log<<"ECsIRaw_signal : "<<CsIRaw<<"	ESi : "<<energytree->eEnergySi<<endl;
							if(energytree->kvid->IsIdentifiable(double(CsIRaw), double(energytree->eEnergySi) ) ){
								Code_Ident_Vamos = 1;
								energytree->kvid->Identify(double(CsIRaw), double(energytree->eEnergySi), id);		//energytree->kvid : KVIDGraph
 				if(DebugIDV) cout<<"Identify done"<<endl<<flush;
                                             			A_PID = id->A;
                        					Z_PID = id->Z;
                       						PID = id->PID;
								delete id;
							
								Int_t Z_PIDI = int(Z_PID);					
								energytree->SetFragmentZ(Z_PIDI);
	      							energytree->GetResidualEnergyCsI(double(Si->E_Raw[y]),double(CsI->E_Raw[j]));		//Method called for guessing A value by bissection method and getting CsI energy

	        						ECsI = energytree->RetrieveEnergyCsI();
								ESi = energytree->RetrieveEnergySi();
								EGap = energytree->eEnergyGap;
			
								AA = energytree->RetrieveA();													
								a_bisec = energytree->BisectionLight(PID,AA,ECsI);
								e_bisec = (ECsI*a_bisec)/AA;
								}	//IsIdentifiable
							else{
								Code_Ident_Vamos = 3;
							}	
							/*L->Log<<"IC Number : "<<Ic->Number<<"	SI Number : "<<Si->Number+1<<"	CSI Number : "<<CsI->Number+1<<endl;
							L->Log<<"IC M : "<<Ic->EM<<"	SI M : "<<Si->E_RawM<<"	CSI M : "<<CsI->EM<<endl;					
							L->Log<<"PID : "<<id->PID<<endl;
							L->Log<<"Code_Ident_Vamos : "<<Code_Ident_Vamos<<endl;     
							L->Log<<"====="<<endl;*/
										
                    						}	//grd!=0 && Signals>0 && Multiplicities=1
                				}	//grd!=0
						else{  
                    					//printf("No object named %s in grid list\n", scope_name);
                				}
            				}
        			}	//gIDGridManager != 0
	    		}	//geometry
		}	//CsI M
    	}	//Si M
//=================================================

  if(Dr->E[0] > 0 && Dr->E[1] > 0 && Ic->ETotal > 0)
    {
      //      dE += Dr->E[0];      
      //      dE += Dr->E[1];
      dE = dE1 = Ic->ETotal;
      if(Dr->Present) dE1 = dE1 / cos(Dr->Tf/1000.);
      dE /= 0.614;
      dE += dE*0.15;
	if((dE1+ESi+ECsI)>0 && CsI->EM==1 && Si->E_RawM==1 && Ic->EM==1 && (Code_Ident_Vamos==1 || Code_Ident_Vamos==3) ){					//if(Si->ETotal > 0)
		E = dE1 + ESi + EGap + ECsI;				//Total energy (MeV)	(ChIo, Si, estimated gap energy, CsI)
	}
	if(CsI->EM==0 && Si->E_RawM==1 && Ic->EM==1 && (Code_Ident_Vamos==2 || Code_Ident_Vamos==4 || Code_Ident_Vamos==10) ){
		E = dE1 + ESi;
	}
    }

//=================================================
// ToF correction for Chio-Si Identification
T = Si->Tfrag*(125.42/((-0.18343*PID)+127.9573));		// ToF * a Correction added on the ToF distribution to get a straight M/Q=2 distribution

// Correction tof pour identification chio-si
if( Code_Ident_Vamos==2 && T<Tof0[runNumber] && ESi<Esi0[runNumber] ){
	T+=116.69;
}
if( Code_Ident_Vamos==4 && T<Tof0[runNumber] && ESi<Esi0[runNumber] ){
	T+=113.69;
}
//================================================= 

//=================================================     
  if(T >0 && Rec->Path>0 && Dr->Present)
    {
 	//Distance between silicon and the target in cm		    
 	D = (1/TMath::Cos(Dr->Pf/1000.))*(Rec->Path + (((Rec->DSI-Dr->FocalPos)/10)/TMath::Cos(Dr->Tf/1000.)));   //Distance : 9423mm(si layer position) - 8702.5mm(focal plane position) = 720.50 mm

 	V = D/T;	   //Velocity given in cm/ns	  
 	//V2 = V + V*(1.-(TMath::Cos(Dr->Tf/1000.)*TMath::Cos(Dr->Pf/1000.)));	   
 	Beta = V/29.9792458; 
 	Gamma = 1./TMath::Sqrt(1.-TMath::Power(Beta,2.));
 	//V_Etot = 1.39*sqrt(E/AA);
 	T_FP = ((Dr->FocalPos)/10.) / V_Etot;
		   
 	Vx = V*sin(Rec->ThetaL)*cos(Rec->PhiL);
 	Vy = V*sin(Rec->ThetaL)*sin(Rec->PhiL);
 	Vz = V*cos(Rec->ThetaL);

   	kin = gIndraDB->GetRun(gIndra->GetCurrentRunNumber())->GetSystem()->GetKinematics();
   	kin->SetOutgoing(kin->GetNucleus(1));
   	kin->CalculateKinematics();

    }
//=================================================


  if(Beta>0 && Rec->Brho>0 && Si->Present)	//Modification (2012-02-10) Original : Beta>0 && Rec->Brho>0 && Gamma>1. && Si->Present
    {
             
      M_Q = Rec->Brho/(3.105*Beta);
            
      //===============================================================
      // Correction on the total energy Si-CsI Identification
	if( CsI->EM==1 && Si->E_RawM==1 && Ic->EM==1 && (Code_Ident_Vamos==1 || Code_Ident_Vamos==3) ){      
		E_csi = GetEnergyLossCsI(Si->Number+1);      
		E_gap2 = GetEnergyLossGap2(Si->Number+1); //gap Si-CsI     
		E_gap1 = GetEnergyLossGap1(Si->Number+1); //gap Chio-Si	
		E_chio = GetEnergyLossChio();
		E_dc2 = GetEnergyLossDC2();
 		E_sed = GetEnergyLossSed();
 		E_dc1 = GetEnergyLossDC1();
 		E_tgt = GetEnergyLossTarget();
		
		E += E_tgt + E_dc1 + E_dc2 + E_sed + E_gap1;	//Correction on the total energy based on the Silicon energy (E_tgt + E_dc1 + E_dc2 + E_sed + E_chio + E_gap1)
	}
	// Correction on the total energy Chio-Si Identification
	// A/Q and A corrections don't depend on corrected energy, but only on EChio+ESi
	/*if( CsI->EM==0 && Si->E_RawM==1 && Ic->EM==1 && (Code_Ident_Vamos==2 || Code_Ident_Vamos==4 || Code_Ident_Vamos==10) ){  
		E_gap2 = GetEnergyLossGap2(Si->Number+1); //gap Si-CsI     
		E_gap1 = GetEnergyLossGap1(Si->Number+1); //gap Chio-Si	
		E_chio = GetEnergyLossChio();
		E_dc2 = GetEnergyLossDC2();
 		E_sed = GetEnergyLossSed();
 		E_dc1 = GetEnergyLossDC1();
 		E_tgt = GetEnergyLossTarget();	
		
		E += E_tgt + E_dc1 + E_dc2 + E_sed + E_gap1 + E_gap2;	
	}*/ 					
      //===============================================================
      
      M = 2.* E / (931.5016*TMath::Power(Beta,2.));
      Q = M/M_Q; 
      
      /*if(Ic->EM==1 && Si->E_RawM==1 && CsI->EM==0 ){
      	L->Log<<"M_Q Raw: "<<M_Q<<endl;
	L->Log<<"M Raw: "<<M<<endl;
	L->Log<<"Q Raw: "<<Q<<endl;
      }*/
            
      //Mass = M_Q*PID;           
      //Mr = (E/1000.)/931.5016/(Gamma-1.);
      //M_Qr = Rec->Brho/3.105/Beta/Gamma;
      //Qr = Mr/M_Qr;
      //Qr = (-1.036820+1.042380*Qr +0.4801678e-03*Qr*Qr);
      //Qc = int(Qr+0.5);
      //Mc = M_Qr*Qc;
      
   M_Qcorr = 0.0;
   M_corr_D2 = 0.0;
   Q_corr = 0;
   Q_corr_D = 0.0;
   realQ = 0;
   realQ_D = 0;
   Qid = 0;
   Q2 = 0;
   Z_corr=0;
   
   Z_corr = int(TMath::Floor(PID+0.5));      
   }
//====================================================================================================
// Première fonction de correction de Q
// First Q correction, according to the CsI detector (Si-CsI Identification)
	if( CsI->EM==1 && Si->E_RawM==1 && Ic->EM==1 && Code_Ident_Vamos==1){   
		DetCsI = int(CsI->Number)+1;   
		Q_corr = int(TMath::Floor((P0[DetCsI]+(P1[DetCsI]*Q)+(P2[DetCsI]*Q*Q)+(P3[DetCsI]*Q*Q*Q))+0.5));
		Q_corr_D = P0[DetCsI]+(P1[DetCsI]*Q)+(P2[DetCsI]*Q*Q)+(P3[DetCsI]*Q*Q*Q);

	}
// End of the fisrt Q correction       
//====================================================================================================

//==========================================================================
// Gates en Brho et état de charge pour la détermination des états de charges
// Brho/Q gates to determine the charge state value (interger)
// Identifications Si-CsI and Chio-Si

GetFileCut();
GetFileCutChioSi();
 
//====================================================================================================
// Second Q correction, by correcting the M and M/Q value
// Case Si-CsI Identification
    if(Q_corr>0 && CsI->EM==1 && Si->E_RawM==1 && Ic->EM==1 && Code_Ident_Vamos==1)	
    {                 
	M_corr = Q_corr*M_Q;
	M_corr_D = Q_corr_D*M_Q;
	
	// Correction de M/Q en fonction du Brho
	// M/Q correction according to the Brho		
	M_Qcorr = P0_mq[runNumber] + (P1_mq[runNumber]*M_Q); 
    }	
    if(Q_corr>0 && llist->IsZombie()==0 && CsI->EM==1 && Si->E_RawM==1 && Ic->EM==1 && Code_Ident_Vamos==1)
    {   		
	// Correction de M en fonction du Brho et de l'état de charge Q
	// M correction according the Brho value and the charge state Q

	if(q5!=NULL && q5->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 5;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);
	}
	else if(q6!=NULL && q6->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 6;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);	
	}
	else if(q7!=NULL && q7->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 7;	
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);	
	}
	else if(q8!=NULL && q8->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 8;	
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);	
	}
	else if(q9!=NULL && q9->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 9;	
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);	
	}	
	else if(q10!=NULL && q10->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 10;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);		
	}
	else if(q11!=NULL && q11->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 11;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D); 		
	}
	else if(q12!=NULL && q12->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 12;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);	
	}
	else if(q13!=NULL && q13->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 13;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);		
	}
	else if(q14!=NULL && q14->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 14;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);		
	}	
	else if(q15!=NULL && q15->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 15;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);
	}
	else if(q16!=NULL && q16->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 16;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);	
	}	
	else if(q17!=NULL && q17->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 17;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D); 
	}
	else if(q18!=NULL && q18->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 18;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D); 		
	}
	else if(q19!=NULL && q19->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 19;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D); 		
	}
	else if(q20!=NULL && q20->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 20;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D); 		
	}
	else if(q21!=NULL && q21->IsInside(Rec->Brho,Q_corr_D)==1)
	{
	Qid = 21;
	M_corr_D2 = P0_m[runNumber][Qid] + (P1_m[runNumber][Qid]*M_corr_D);		
	}	
	else 
	{
	Qid = 0;
	M_corr_D2 = 0.0;
	}
	
	Q_corr_D2 = M_corr_D2 / M_Qcorr;
	Q2 = TMath::Floor(Q_corr_D2+0.5);	
	M_realQ = TMath::Floor(Q_corr_D2+0.5)*M_Qcorr;   	
    	}	            
// End of the second Q correction : Si-CsI Identification
//====================================================================================================
// Second Q correction : Chio-Si Identification    
if(Q>0 && CsI->EM==0 && Si->E_RawM==1 && Ic->EM==1 && (Code_Ident_Vamos==2 || Code_Ident_Vamos==10) )	
    { 
	M_Qcorr = P0_mq_chiosi[runNumber] + (P1_mq_chiosi[runNumber]*M_Q);     
    }
    
    if(Q>0 && llist2->IsZombie()==0 && CsI->EM==0 && Si->E_RawM==1 && Ic->EM==1 && (Code_Ident_Vamos==2 || Code_Ident_Vamos==10) )
    {   		
	// Correction de M en fonction du Brho et de l'état de charge Q
	// M correction according the Brho value and the charge state Q

	if(q8cs!=NULL && q8cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 8;	
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);	 
	}
	else if(q9cs!=NULL && q9cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 9;	
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);	 
	}	
	else if(q10cs!=NULL && q10cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 10;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);		 
	}
	else if(q11cs!=NULL && q11cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 11;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);		 
	}
	else if(q12cs!=NULL && q12cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 12;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);	 
	}
	else if(q13cs!=NULL && q13cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 13;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);		 
	}
	else if(q14cs!=NULL && q14cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 14;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);		 
	}	
	else if(q15cs!=NULL && q15cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 15;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);
	}
	else if(q16cs!=NULL && q16cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 16;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);	 
	}	
	else if(q17cs!=NULL && q17cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 17;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M); 
	}
	else if(q18cs!=NULL && q18cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 18;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);		 
	}
	else if(q19cs!=NULL && q19cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 19;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);		 
	}
	else if(q20cs!=NULL && q20cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 20;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);		 
	}
	else if(q21cs!=NULL && q21cs->IsInside(Rec->Brho,Q)==1)
	{
	Qid = 21;
	M_corr_D2 = P0_m_chiosi[runNumber][Qid] + (P1_m_chiosi[runNumber][Qid]*M);		
	}
	else 
	{
	Qid = 0;
	M_corr_D2 = 0.0;
	}
	
	Q_corr_D2 = M_corr_D2 / M_Qcorr;	
	Q2 = TMath::Floor(Q_corr_D2+0.5);	
	M_realQ = TMath::Floor(Q_corr_D2+0.5)*M_Qcorr;      
   	
    	}	

//====================================================================================================

//====================================================================================================
// Flag the events

// Code_Vamos : 
// Code_Vamos = 1 : Fragment à l'intérieur de la région d'intérêt et Q correct (Z-4<Q<Z+1)
// Code_Vamos = 2 : Fragment à l'extérieur de la région d'intérêt, mais dont Q est correct
// Code_Vamos = 3 : Fragment à l'intérieur de la région d'intérêt et Q>Z
// Code_Vamos = 4 : Fragment à l'intérieur de la région d'intérêt et Q<Z-4
// Code_Vamos = 5 : Fragment à l'extérieur de la région d'intérêt et Q>Z
// Code_Vamos = 6 : Fragment à l'extérieur de la région d'intérêt et Q<Z-4
// Code_Vamos = 13 : Else
    
  if(Z_corr>4 && Z_corr<21 && Qid>0 && M_realQ>0){	// Flag les events pour Z = 5-20 et Qid>0
    if( Rec->Brho >= Brho_min[Z_corr][int(M_realQ)][Qid][runNumber] && Rec->Brho <= Brho_max[Z_corr][int(M_realQ)][Qid][runNumber] && Qid<Z_corr+1 && Qid>Z_corr-4){
		Code_Vamos = 1;
	}
    else if( ((Rec->Brho < Brho_min[Z_corr][int(M_realQ)][Qid][runNumber]) || (Rec->Brho > Brho_max[Z_corr][int(M_realQ)][Qid][runNumber])) && Qid<Z_corr+1 && Qid>Z_corr-4){
    		Code_Vamos = 2;
    	}	
    else if( Rec->Brho >= Brho_min[Z_corr][int(M_realQ)][Qid][runNumber] && Rec->Brho <= Brho_max[Z_corr][int(M_realQ)][Qid][runNumber] && Qid>Z_corr){
		Code_Vamos = 3;
	}
    else if( Rec->Brho >= Brho_min[Z_corr][int(M_realQ)][Qid][runNumber] && Rec->Brho <= Brho_max[Z_corr][int(M_realQ)][Qid][runNumber] && Qid<Z_corr-4){
    		Code_Vamos = 4;
    	}
    else if( ((Rec->Brho < Brho_min[Z_corr][int(M_realQ)][Qid][runNumber]) || (Rec->Brho > Brho_max[Z_corr][int(M_realQ)][Qid][runNumber])) && Qid>Z_corr){
		Code_Vamos = 5;
	}
    else if( ((Rec->Brho < Brho_min[Z_corr][int(M_realQ)][Qid][runNumber]) || (Rec->Brho > Brho_max[Z_corr][int(M_realQ)][Qid][runNumber])) && Qid<Z_corr-4 && Qid>0){
    		Code_Vamos = 6;
    	}		
	else{
		Code_Vamos = 13;
	}
  }
//====================================================================================================
    
NormVamos = gIndraDB->GetRun(gIndra->GetCurrentRunNumber())->Get("NormVamos");
DT = gIndraDB->GetRun(gIndra->GetCurrentRunNumber())->Get("DT");
FC_Indra = gIndraDB->GetRun(gIndra->GetCurrentRunNumber())->GetScaler("INDRA");
Brho_mag = gIndraDB->GetRun(gIndra->GetCurrentRunNumber())->Get("Brho");   
stat_tot = 0.;
stat_tot = Stat_Indra[gIndra->GetCurrentRunNumber()]; 
   
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

  //Counter[0]++;
  Init();
  //if(Rec->Present) Counter[1]++;		//Non commenter
  Calculate();
  //#ifdef DEBUG
  //Show();
  //#endif
  
}
void Identificationv::inAttach(TTree *)
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
	
	/*outT->Branch("ESiRaw",&SiRaw,"SiRaw/I");
	outT->Branch("ECsIRaw",&CsIRaw,"CsIRaw/I");
	
	outT->Branch("E_tgt",&E_tgt,"E_tgt/D");	
	outT->Branch("E_dc1",&E_dc1,"E_dc1/D");	
	outT->Branch("E_sed",&E_sed,"E_sed/D");
	outT->Branch("E_dc2",&E_dc2,"E_dc2/D");
	outT->Branch("E_chio",&E_chio,"E_chio/D");
	outT->Branch("E_gap1",&E_gap1,"E_gap1/D");						
	
	outT->Branch("ESi",&ESi,"ESi/D");
	outT->Branch("EGap",&EGap,"EGap/D");
	outT->Branch("ECsI",&ECsI,"ECsI/D");
	outT->Branch("ECsI_corr",&ECsI_corr,"ECsI_corr/D");
	outT->Branch("NormVamos",&NormVamos,"NormVamos/D");	
	outT->Branch("DT",&DT,"DT/D");
	outT->Branch("Brho_mag",&Brho_mag,"Brho_mag/D");		
	outT->Branch("FC_Indra",&FC_Indra,"FC_Indra/D");

  	outT->Branch("E_corr",&E_corr,"E_corr/F");

	outT->Branch("Gamma",&Gamma,"Gamma/F");*/
	
	outT->Branch("ESi",&ESi,"ESi/D");
	outT->Branch("ECsI",&ECsI,"ECsI/D");
	outT->Branch("Brho_0",&Brho_mag,"Brho_mag/D");
	outT->Branch("Stat_Indra", &stat_tot, "Stat_Indra/F");
	outT->Branch("NormVamos",&NormVamos,"NormVamos/D");
	outT->Branch("Code_Vamos", &Code_Vamos, "Code_Vamos/I");
	outT->Branch("Code_Ident_Vamos", &Code_Ident_Vamos, "Code_Ident_Vamos/I");	
	outT->Branch("E",&E,"E/F");
	outT->Branch("T",&T,"T/F");	
	outT->Branch("D",&D,"D/F");
	outT->Branch("V_VAMOS",&V,"V/F");
	outT->Branch("Beta",&Beta,"Beta/F");
	outT->Branch("AQ_VAMOS", &M_Qcorr, "M_Qcorr/F");
	outT->Branch("RealZ_VAMOS",&PID,"PID/D");
	outT->Branch("Z_VAMOS", &Z_corr, "Z_corr/I");
	outT->Branch("A_VAMOS", &M_realQ, "M_realQ/D");
	outT->Branch("Q_VAMOS", &Q2, "Q2/I");
	outT->Branch("RealQ_VAMOS", &Q_corr_D2,"Q_corr_D2/D");
		
	outT->Branch("AQ", &M_Q, "M_Q/F");	
	outT->Branch("Q",&Q,"Q/F");
	outT->Branch("A",&M,"M/F");
	/*outT->Branch("Q_corr", &Q_corr, "Q_corr/I");
	outT->Branch("Q_corr_D", &Q_corr_D,"Q_corr_D/D");
	outT->Branch("A_corr", &M_corr, "M_corr/D");
	outT->Branch("A_corr_D", &M_corr_D,"M_corr_D/D");*/
	/*outT->Branch("A_realQ", &M_realQ, "M_realQ/D");
	outT->Branch("A_realQ_D", &M_realQ_D,"M_realQ_D/D");
	outT->Branch("A_corr_D2", &M_corr_D2,"M_corr_D2/D");
	outT->Branch("Q_corr_D2", &Q_corr_D2,"Q_corr_D2/D");
	outT->Branch("Qid", &Qid, "Qid/I");*/

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

//method to reconstruct VAMOS telescopes
int Identificationv::GeometryChioSi(UShort_t cinum, UShort_t sinum)
{
   Int_t  num;
   Int_t si1=0, si2=0, si3=0, si4=0, si5=0, si6=0;
   ifstream in;
   TString sline;
   
   if(!gDataSet->OpenDataSetFile("geom_chiosi.dat",in))
  {
     cout << "Could not open the calibration file geom_chiosi.dat !!!" << endl;
     return 0;
  }
  else 
  {
	while(!in.eof()){
       sline.ReadLine(in);
       if(!in.eof()){
	   if (!sline.BeginsWith("#")){
	     sscanf(sline.Data(),"%d %d %d %d %d %d %d", &num, &si1, &si2, &si3, &si4, &si5, &si6);
	     geomchiosi[num][0]=si1;
	     geomchiosi[num][1]=si2;
	     geomchiosi[num][2]=si3;
	     geomchiosi[num][3]=si4;
	     geomchiosi[num][4]=si5;
	     geomchiosi[num][5]=si6;	     
	     	   }
       		}
     	}
  }
  in.close();

  for(int i=0;i<6;i++)
    {
      if(geomchiosi[cinum][i]==sinum)return 1;
    }
  return 0;



}

int Identificationv::Geometry(UShort_t sinum, UShort_t csinum)
{  
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
	     sscanf(sline.Data(),"%d %d %d %d %d %d %d", &num, &csi1, &csi2, &csi3, &csi4, &csi5, &csi6);
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

  for(int i=0;i<6;i++)
    {
      if(geom[sinum][i]==csinum)return 1;
    }
  return 0;
}

