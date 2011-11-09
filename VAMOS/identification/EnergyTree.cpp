//Created by KVClassFactory on Thu Aug 20 16:03:37 2009
//Author: marini

#include "EnergyTree.h"
#include "Riostream.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TEventList.h"
#include "TROOT.h"
#include "Sive503.h"
#include "CsIv.h"
#include "KVIVReconIdent.h"
#include "KVSeqCollection.h"
#include "KVIDGridManager.h"
#include "KVIDZAGrid.h"
#include "KVIDSiCsIVamos.h"

#include <stdio.h>

#include <cstdlib>
#include <stdlib.h>
#include <new>
#include <string>
#include <string.h>

ClassImp(EnergyTree)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>EnergyTree</h2>
<h4>>Best estimations of the total incident energy (=>energy loss in CsI) and particle mass</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

EnergyTree::EnergyTree(LogFile *Log,Sive503 *SiD)
{
   // Default constructor
  eN=0;
  eLightSi=0;
  LightCsI=0.;
  eLightCsI=0;
  eEnergySi=0.;
  eEnergyCsI=0.;
  ePied=0.;
  eZ=0;
  eA=0;
  sEnergySi=0.;
  sEnergyCsI=0.;
  esi1=0.;
  esi2=0.;
  ecsi1=0.;
  ecsi2=0.;
  diff1=0.;
  diff2=0.;
  diffsi=0.;
  diffcsi=0.;
  sA=0;
  iA=0.;
  right=0;
  left=0;
  sRefECsI=0.;
  Einc=0.;
  EEsi=0.;
  Echio=0.;
  
  L=Log;
  Si = SiD;

    ioCh=0;
    gap=0;
    si=0;
    csi=0;
    kvt_icsi=0; 
    kvt_sicsi=0;
    lum=0; 
}

EnergyTree::~EnergyTree()
{
   // Destructor
   L->Log<<"***Destructor EnergyTree***"<<endl;
}

//-------------------telescope and calibrations initialization----------------------------
// return the thickness in microm of this Silicon number of the VAMOS wall-Si 
/*
Double_t EnergyTree::GetSiliconThickness(Int_t number)
{
    Double_t si_thick[18] =
        {522., 530., 531., 532., 533., 533., 534., 535., 531.,
                535., 524., 531., 529., 524., 533., 537., 519., 530.};

	if (number>18 || number<0){
	    cout<<"No thickness for this detector"<<endl;
	    return 0;
	}

    return si_thick[number];	
}
*/

/*  void EnergyTree::InitDetector(Int_t number)
{
    ioCh = new IonisationChamber();  
    gap = new PlaneAbsorber();
    si = new KVSiliconVamos(Si->si_thick[number]);
    csi = new KVCsIVamos(1.);
    
    kvt_icsi = new KVTelescope();
    kvt_sicsi = new KVTelescope();
    lum=new KVLightEnergyCsI(csi);
}*/

void EnergyTree::SetModuleMap(string id_modulemap[18][80])
{
for(Int_t i=0;i<18;i++){
	for(Int_t j=0;j<80;j++){
		module_map[i][j] = id_modulemap[i][j];
		}
	}
}

string EnergyTree::GetModuleName(Int_t si, Int_t csi)
{
//cout<<" Si and CsI : "<<si<<" "<<csi<<endl;
name = "SI_CSI_";
name += module_map[si][csi];

if(name == "SI_CSI_null"){
cout<<"module not found"<<endl;
cout<<" Si and CsI : "<<si<<" "<<csi<<endl;
//exit(-1);
}

//sprintf(name, "SI_CSI_%04s", module_map[si-1][csi-1]);
return name;
}

void EnergyTree::InitTelescope(Int_t si_num ,Int_t csi_num)
{
    
    Char_t si_name [128] = "null";
    Char_t csi_name [128] = "null";
    string module_name;

    sprintf(si_name, "SIE_%02i", si_num+1); 
    sprintf(csi_name, "CSI%02i", csi_num+1);
	
    //module_name = GetModuleName(si_num, csi_num);		//Original

    Char_t tel_name [128] = "null";
    sprintf(tel_name, "SIE_%02i_CSI%02i", si_num+1, csi_num+1);

    list = 0;        
    KVList *grid_list = 0;
    kvid = 0;
    KVNumberList runList = 0;

    list = (KVList*) gIDGridManager->GetGrids();
		
    if(list != 0){
	
		Int_t entries = (Int_t) list->GetEntries();
		Int_t kHasGrids = 0;

		KVIDGrid *tmpGrid = 0;
		Int_t nGridsForRun = 0;

		for(Int_t i=0; i<entries; i++){
    			tmpGrid = (KVIDGrid*) list->At(i);
    			if(tmpGrid != 0){
        			runList = (KVNumberList) tmpGrid->GetRuns();
        			runList.Begin();
        			while( !runList.End() ){
            				UInt_t next_val = (UInt_t) runList.Next();
            				if(next_val == gIndra->GetCurrentRunNumber()){
						if(strcmp(tmpGrid->GetName(),tel_name)==0){
							kvid = tmpGrid;
							}
						}
					
        				}
    				}
			} 
			
			      				
		if(kvid != 0){

        }else{  
	    printf("Error: 'kvid' assignment failed\n");
	    cout<<"si_num : "<<si_num<<" csi_num : "<<csi_num<<endl; 
	    cout<<"name : "<<tel_name<<endl; 	    
	    
	    L->Log<<"Error: 'kvid' assignment failed"<<endl;
	    L->Log<<"si_num : "<<si_num<<" csi_num : "<<csi_num<<endl; 
	    L->Log<<"name : "<<tel_name<<endl; 
        }
    }else{
        printf("Error: 'list' assignment failed\n");   
    }

}

void EnergyTree::InitIcSi(Int_t number) // Ionisation Chamber to Silicon Telescope
{

   /********************************************************************************
    TELESCOPE LAYOUT: Using custom built classes IonisationChamber and PlaneAbsorber 
    to take into account the more detailed characteristics of the IC and KVMaterials. 
    The IC now has the correct 'dead' and 'active' regions implemented as well 
    as the entrance and exit windows.

    beam >>  | IoCh | C4H10 (Gap) | Si Detector |

    ********************************************************************************/

    ioCh = new IonisationChamber();  
    gap = new PlaneAbsorber();
    si = new KVSiliconVamos(Si->si_thick[number]);

/*
    Double_t si_thick[19] = 
        {0., 522., 530., 531., 532., 533., 533., 534., 535., 531., 
                535., 524., 531., 529., 524., 533., 537., 519., 530.};
*/
    // Remember 'gap' and 'si' are of class 'PlaneAbsorber'
    // see header files for list of functions that can be called

    gap->SetThickness(12.78,"NORM");
    gap->SetMaterial("C4H10");
    gap->SetPressure(40.);

    // Build the Telescope 
    // Need to use the 'GetDetector()' method as they are not
    // of type KVDetector

    kvt_icsi = new KVTelescope();
    kvt_icsi->Add(ioCh->GetDetector());
    kvt_icsi->Add(gap->GetDetector());  // In-active so no 'detected' energy
    kvt_icsi->Add(si);

}

Double_t EnergyTree::GetResidualEnergyIc(Int_t Z, Int_t A, Double_t EEinc)
{	
    KVNucleus nucleus(Z,A);
    nucleus.SetEnergy(EEinc);
    kvt_icsi->DetectParticle(&nucleus);

    // Remember:
    // si is of type 'PlaneAbsorber'
    // ioCh is of type 'IonisationChamber'
    // see header files for list of functions that can be called

	EEsi = si->GetEnergy();
	Echio = ioCh->GetEnergy();

	Double_t Epunch = nucleus.GetEnergy();

    //if(Epunch > .1) printf("Einc: %4.2f ESi: %4.2f EIoCh: %4.2f EPunch %4.2f\n", EEinc, EEsi, Echio, Epunch);

    nucleus.Clear();
    kvt_icsi->Clear();
  
	return Echio;
}

void EnergyTree::InitSiCsI(Int_t number) // Si-CsI Telescope	# Si : 1 to 18	
{

   /********************************************************************************
    TELESCOPE LAYOUT: Using custom built classes IonisationChamber and PlaneAbsorber 

    beam >>  | Silicon | C4H10 (Gap) | CsI 

    ********************************************************************************/
 
    kvd_si = new KVSiliconVamos(Si->si_thick[number]);	//# Si : 1 to 18
    gap = new PlaneAbsorber();
    kvd_csi = new KVCsIVamos(1.);

/*
    Double_t si_thick[19] =
        {0., 522., 530., 531., 532., 533., 533., 534., 535., 531.,
                535., 524., 531., 529., 524., 533., 537., 519., 530.};	//# Si : 1 to 18
*/
    // Remember they are of class 'PlaneAbsorber'
    // see header files for list of methods that can be called
     //L->Log<<"Silicon Thickness	: "<<si_thick[number]<<endl;

    gap->SetThickness(136.5,"NORM");
    gap->SetMaterial("C4H10");
    gap->SetPressure(40.);

    // Build the Telescope 
    // Need to use the 'GetDetector()' method as they are not
    // of type KVDetector

    kvt_sicsi = new KVTelescope();
    kvt_sicsi->Add(kvd_si);
    kvt_sicsi->Add(gap->GetDetector());  // In-active so no 'detected' energy
    kvt_sicsi->Add(kvd_csi);
    
    lum=new KVLightEnergyCsIVamos(kvd_csi);
}

Int_t EnergyTree::ClearEvent(Int_t runFlag){     
if(runFlag == -5){
    if(si != 0){
        delete si;
        si=0;
    }	
    if(csi != 0){
        delete csi;
        csi=0;
    }	
    if(ioCh != 0){
        delete ioCh;
        ioCh=0;
    }
    if(gap != 0){
         delete gap;
        gap=0;
    }
    if(kvt_icsi != 0){
        delete kvt_icsi;
        kvt_icsi=0;
    }
    if(lum != 0){ 
        delete lum;
        lum=0;
    }
}
    return 0;
}

  void EnergyTree::SetSiliconThickness(Int_t number)	//Si->Number goes to 0 to 17
{
	InitSiCsI(number+1);	//InitSiCsI(#)	(1-18)
}
//-------------------------------------------------------------------

//Set Si calibration parameters
void EnergyTree::SetCalSi(Float_t p1, Float_t p2, Float_t p3){
  //cout<<"EnergyTree::SetCalSi"<<endl<<flush;
  a=p1;
  b=p2;
  c=p3;
  alpha=1;
  L->Log<<"parametri silicio a="<<a<<" b="<<b<<" c="<<c<<endl;
}

//Set CsI pedestal
void EnergyTree::SetCsIPed(Float_t pied){
 ePied=pied;
}

//CsI calibration parameters and the piedestal
void EnergyTree::SetCalCsI(Float_t a1, Float_t a2, Float_t a3){
 
 lum=new KVLightEnergyCsIVamos(kvd_csi);
 lum->SetNumberParams(2);
 lum->SetParameters(a1,a2);
 L->Log<<"parametri cesio a1="<<a1<<" a2="<<a2<<" a3="<<a3<<endl;
 L->Log<<"pedestal csi=		"<<ePied<<endl;
}

void EnergyTree::SetCalibration(Sive503 *Si, CsIv* CsI,Int_t sinum, Int_t csinum)
{

  if(Si->E_RawM>0)
  SetCalSi(Si->ECoef[sinum][0],Si->ECoef[sinum][1],Si->ECoef[sinum][2]);
  else
    {
      a=b=c=alpha=0.0;
    }
  if(CsI->E_RawM>0)
    {
      SetCsIPed(CsI->Ped[csinum][0]);
      SetCalCsI(CsI->ECoef[csinum][0],CsI->ECoef[csinum][1],CsI->ECoef[csinum][2]);
    }
  else
    {
      ePied=0.0;
      
      lum=new KVLightEnergyCsIVamos(kvd_csi);
      lum->SetNumberParams(2);
      lum->SetParameters(0.,0.);
    }
}

//fragment Z
void EnergyTree::SetFragmentZ(Int_t Zin){
  eZ=Zin;
  lum->SetZ(eZ);
}

//---------------------------best estimation of Ecsi and A-----------
void EnergyTree::DoIt(UShort_t chsi, UShort_t chcsi, Int_t Aoo){
  CalculateESi(chsi);
  ECsIch(chcsi);
  SetFragmentA(Aoo);
  CompleteSimulation(short(LightCsI));  
}


//Complete procedure to get the best estimate of the residual energy
Double_t EnergyTree::GetResidualEnergyCsI(UShort_t chsi, UShort_t chcsi){
  Int_t A;
  A=2*eZ;
  CalculateESi(chsi);
  ECsIch(chcsi);
  Bisection(A,short(LightCsI));		//Bisection(A,chcsi); 	
  Interpolate();
  
  return sRefECsI; 
}

Double_t EnergyTree::GetResidualEnergyCsI2(UShort_t chsi, UShort_t chcsi, Int_t Zoo, Int_t Aoo){
//Int_t eZ;
Zoo=eZ;
Int_t A;
A=Aoo;

  CalculateESi(chsi);
  ECsIch(chcsi);
  Bisection(A,short(LightCsI));	//Bisection(Aoo,chcsi);  	
  Interpolate();
  
  return sRefECsI; 
}

//Si calibration function
void EnergyTree::CalculateESi(UShort_t chan){
  Double_t fact=1.;
  eEnergySi = fact*alpha*(a+chan*b+chan*chan*c); 
}



//CsI light in channels
void EnergyTree::ECsIch(UShort_t chan){
  LightCsI=chan - ePied; 
  //L->Log<<"LightCsI ECsIch() = "<<LightCsI<<endl;
  //L->Log<<"pedestal csi=	"<<ePied<<endl;
}
//set fragment A
void EnergyTree::SetFragmentA(Int_t Ain){ 
  lum->SetA(Ain);
  sA=Ain; 
}

//Complete procedure for to get the energy losses from simulation
void  EnergyTree::CompleteSimulation(UShort_t chan){
  CalculateECsI();
  SimulateEvent();
  CalculateCanalCsI();

  GetESi();
  GetECsI();
  ClearTelescope();
}

//CsI calibration function
void EnergyTree::CalculateECsI(){
      eEnergyCsI=lum->Compute(LightCsI);
      //L->Log<<"light csi="<<LightCsI<<" eEnergyCsI=MeV "<<eEnergyCsI<<endl; //paola
}

void EnergyTree::CalculateCanalCsI(){
      CanalCsI=lum->Invert(kvd_csi->GetEnergy());
      difflum = (CanalCsI-LightCsI);
      //L->Log<<"Canal csi Invert() = "<<CanalCsI<<endl; //paola
}

//simulation of particle traversing the detector
void EnergyTree::SimulateEvent(){
  part.SetZ(eZ);
  part.SetA(sA);
  Einc=eEnergySi+eEnergyCsI;
  part.SetEnergy(Einc);
  
  kvt_sicsi->DetectParticle(&part);
  //kvd_si->DetectParticle(&part);    
  //kvd_csi->DetectParticle(&part);  
  
  //diffsi = eEnergySi-kvd_si->GetEnergy();
  //diffcsi = RetrieveEnergyCsI()-kvd_csi->GetEnergy();
}

//Energy in Si from simulation
void EnergyTree::GetESi(){
  sEnergySi=kvd_si->GetEnergy();
  //L->Log<<"GetESi:: sEnergySi= "<<sEnergySi<<endl;
}

//Energy in CsI from simulation
void EnergyTree::GetECsI(){
  sEnergyCsI=kvd_csi->GetEnergy();
  //L->Log<<"GetECsI:: sEnergyCsI= "<<sEnergyCsI<<endl;
}

// Reset the telescope in order to prepare for the next event
void EnergyTree::ClearTelescope(){
  part.Clear();
  kvt_sicsi->Clear();
  
  //kvd_si->Clear();		
  //kvd_csi->Clear();
  //kvid->Clear();
}

void EnergyTree::Bisection(Int_t A, UShort_t chan){ 
  Int_t middle, it=0;
  L->Log<<"------------"<<endl;
  L->Log<<"Valeur de A : "<<A<<endl;
  L->Log<<"Energy Si Reference :	"<<eEnergySi<<endl;
  if(A-5>0)
  left=A-5.;  //left and right are integer!! A is a double
  else left=1;
  right=A+10;	
  L->Log<<"-----------------------------------BISECTION-------------------"<<endl;//paola

  //loop: bisection
  while(right-left>1){

    L->Log<<"-------------------loop bisection------------"<<endl;

	middle=(left+right+1)/2;
	
    //simulated energies stored for middle A
    SetFragmentA(middle);
    CompleteSimulation(chan);
    esi2=sEnergySi;
    ecsi2=sEnergyCsI;
    
	if(esi2>eEnergySi){
	   right=middle;	
	}    
	else
	left=middle;
	    
    L->Log<<"A2 (middle) :	"<<middle<<"	ESi2=	"<<sEnergySi<<"	ECsI2=	"<<sEnergyCsI<<endl; //paola

    it++;
    L->Log<<"----------------iteration= "<<it<<endl;
  }    //end of bisection loop
  
  //simulated energies for two values of A closest to the point of zero 
  //difference between the simulated energy for Si and the one from calibration
    SetFragmentA(left);
    CompleteSimulation(chan);
    esi1=sEnergySi;
    ecsi1=sEnergyCsI;
    
    SetFragmentA(right);
    CompleteSimulation(chan);
    esi2=sEnergySi;
    ecsi2=sEnergyCsI;
}

//interpolation usage for to find the right residual energy and right A, using 
//the final interval returned by bisection 
void EnergyTree::Interpolate(){
   sRefECsI= GetInterpolationD(esi1,ecsi1,esi2,ecsi2,eEnergySi);
   iA=GetInterpolationD(esi1,(Double_t)left,esi2,(Double_t)right,eEnergySi);  
}

// Interpolation procedure: finding y corresponding to x, under assumption that
//these variables behave linearly within the given ranges x1-x2, and y1-y2.
Double_t EnergyTree::GetInterpolationD(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t x){
  Double_t slope;
  Double_t y;
  slope=(y1-y2)/(x1-x2);
  y=slope*(x-x2)+y2;
  //  cout<<"esi1="<<x1<<" ecsi1="<<y1<<" esi2="<<x2<<" ecsi2="<<y2<<" esi mis="<<x<<" ecsi interp="<<y<<endl; //paola

  return y;
}

//--------------------------Retrieve values------------------------//
//Getter for energy loss in silicon
Double_t EnergyTree::RetrieveEnergySi(){
  return eEnergySi;
}

//getter for the best estimate of A
Double_t EnergyTree::RetrieveA(){
  return iA;
}

//getter for light in CsI
Double_t EnergyTree::RetrieveLight(){
  return LightCsI;
}

Double_t EnergyTree::RetrieveEnergyCsI(){
	return eEnergyCsI;
}
