//Created by KVClassFactory on Thu Aug 20 16:03:37 2009
//Author: marini

#include "EnergyTree.h"
#include "Riostream.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TEventList.h"
#include "TROOT.h"
#include "Siv.h"
#include "CsIv.h"

#include <stdio.h>


ClassImp(EnergyTree)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>EnergyTree</h2>
<h4>>Best estimations of the total incident energy (=>energy loss in CsI) and particle mass</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

EnergyTree::EnergyTree(LogFile *Log)
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
  diffetot=0.;
  difflum=0.;
  sA=0;
  iA=0.;
  right=0;
  left=0;
  sRefECsI=0.;
  Einc=0.;
  EEsi=0.;
  Echio=0.;
  L=Log;
  
}

EnergyTree::~EnergyTree()
{
   // Destructor
}

//-------------------telescope and calibrations initialization----------------------------
// return the thickness in microm of this Silicon number of the VAMOS wall-Si 
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

  void EnergyTree::InitDetector()
{
 	detsi=new KVDetector ("Si",thick);	//define a Silicon detector of 500 microm thick
	detcsi=new KVDetector ("CsI",1.);	//define a CsI detector of 1 cm thick
	kvt = new KVTelescope;				
	kvt->Add(detsi);			// define a telescope composed of a Si(500)+CsI(1cm)  
	kvt->Add(detcsi);
	//lum=new KVLightEnergyCsI(detcsi);
	//kvt->Print();
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
    si = new PlaneAbsorber();

    Double_t si_thick[19] = 
        {0., 522., 530., 531., 532., 533., 533., 534., 535., 531., 
                535., 524., 531., 529., 524., 533., 537., 519., 530.};

    // Remember 'gap' and 'si' are of class 'PlaneAbsorber'
    // see header files for list of functions that can be called

    gap->SetThickness(12.78,"NORM");
    gap->SetMaterial("C4H10");
    gap->SetPressure(40.);

    si->SetThickness(si_thick[number],"NORM");
    si->SetMaterial("Si");
    si->SetActive();

    // Build the Telescope 
    // Need to use the 'GetDetector()' method as they are not
    // of type KVDetector

    kvt_icsi = new KVTelescope();
    kvt_icsi->Add(ioCh->GetDetector());
    kvt_icsi->Add(gap->GetDetector());  // In-active so no 'detected' energy
    kvt_icsi->Add(si->GetDetector());

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

void EnergyTree::InitSiCsI(Int_t number) // Si-CsI Telescope
{

   /********************************************************************************
    TELESCOPE LAYOUT: Using custom built classes IonisationChamber and PlaneAbsorber 

    beam >>  | Silicon | C4H10 (Gap) | CsI 

    ********************************************************************************/

    si = new PlaneAbsorber();
    gap = new PlaneAbsorber();
    csi = new PlaneAbsorber();

    Double_t si_thick[19] =
        {0., 522., 530., 531., 532., 533., 533., 534., 535., 531.,
                535., 524., 531., 529., 524., 533., 537., 519., 530.};

    // Remember they are of class 'PlaneAbsorber'
    // see header files for list of methods that can be called

    si->SetThickness(si_thick[number],"NORM");
    si->SetMaterial("Si");
    si->SetActive();

    gap->SetThickness(136.5,"NORM");
    gap->SetMaterial("C4H10");
    gap->SetPressure(40.);

    csi->SetThickness(1.,"NORM");
    csi->SetMaterial("CsI");
    csi->SetActive();

    // Build the Telescope 
    // Need to use the 'GetDetector()' method as they are not
    // of type KVDetector

    kvt_sicsi = new KVTelescope();
    kvt_sicsi->Add(si->GetDetector());
    kvt_sicsi->Add(gap->GetDetector());  // In-active so no 'detected' energy
    kvt_sicsi->Add(csi->GetDetector());
    lum=new KVLightEnergyCsI(csi->GetDetector());

}


  void EnergyTree::SetSiliconThickness(Int_t number)
{
	thick=GetSiliconThickness(number);
	InitSiCsI(number);
	//detsi->SetThickness(thick);
}
//-------------------------------------------------------------------


//Set Si calibration parameters
void EnergyTree::SetCalSi(Float_t p1, Float_t p2, Float_t p3){
  //cout<<"EnergyTree::SetCalSi"<<endl<<flush;
  a=p1;
  b=p2;
  c=p3;
  alpha=1;
}

//Set CsI pedestal
void EnergyTree::SetCsIPed(Float_t pied){
 ePied=pied;
}

//CsI calibration parameters and the piedestal
void EnergyTree::SetCalCsI(Float_t a1, Float_t a2, Float_t a3){
 lum=new KVLightEnergyCsI(csi->GetDetector());
 lum->SetNumberParams(3);
 lum->SetParameters(a1,a2,a3);
 //cout<<"parametri cesio a1="<<a1<<" a2="<<a2<<" a3="<<a3<<endl;
}

/*   
void EnergyTree::Init(){
L->Log<<"EnergyTree::Init() "<<endl;	
   //InitDetector();
}
*/

void EnergyTree::SetCalibration(Siv *Si, CsIv* CsI,Int_t sinum, Int_t csinum)
{

  if(Si->E_RawM>0)
  SetCalSi(Si->ECoef[sinum][0],Si->ECoef[sinum][1],Si->ECoef[sinum][2]);
  else
    {
      a=b=c=alpha=0.0;
    }
  if(CsI->E_RawM>0)
    {
      //cout<<" SetCalibration:: csinum="<<csinum<<" ped "<<CsI->Ped[csinum][0]<<endl;
      SetCsIPed(CsI->Ped[csinum][0]);
      SetCalCsI(CsI->ECoef[csinum][0],CsI->ECoef[csinum][1],CsI->ECoef[csinum][2]);
    }
  else
    {
      ePied=0.0;
      lum=new KVLightEnergyCsI(csi->GetDetector());
      lum->SetNumberParams(3);
      lum->SetParameters(0.,0.,0.);
    }
}

//fragment Z
void EnergyTree::SetFragmentZ(Int_t Zin){
  eZ=Zin;
  lum->SetZ(eZ);
}

//---------------------------best estimation of Ecsi and A-----------

//Complete procedure to get the best estimate of the residual energy
Double_t EnergyTree::GetResidualEnergyCsI(UShort_t chsi, UShort_t chcsi){
//Int_t eZ;
//Zoo=eZ;
  Int_t A;
  A=2*eZ;
  CalculateESi(chsi);
  ECsIch(chcsi);
  Bisection(A,short(LightCsI));		//Bisection(A,chcsi);
  //cout<<"diff1 = "<<diff1<<endl;
  //cout<<"diff2 = "<<diff2<<endl;
//cout <<"eZ = "<<eZ<<endl;
//cout <<"A = "<<RetrieveA()<<endl;  	
  Interpolate();
  
  return sRefECsI; 
}

Double_t EnergyTree::GetResidualEnergyCsI2(UShort_t chsi, UShort_t chcsi, Int_t Zoo){
//Int_t eZ;
Zoo=eZ;
Int_t A;
A=2*eZ;

  CalculateESi(chsi);
  ECsIch(chcsi);
  Bisection(A,short(LightCsI));	//Bisection(Aoo,chcsi);
  //cout<<"diff1 = "<<diff1<<endl;
  //cout<<"diff2 = "<<diff2<<endl;
//cout <<"eZ = "<<eZ<<endl;
//cout <<"A = "<<RetrieveA()<<endl;  	
  Interpolate();
  
  return sRefECsI; 
}

//Si calibration function

void EnergyTree::CalculateESi(UShort_t chan){
  Double_t fact=1.;
  //  cout<<"eEnergy Si ch="<<chan<<endl;
  eEnergySi = fact*alpha*(a+chan*b+chan*chan*c);
  //eEnergySi = Si->ETotal;
  //cout<<"eEnergySi=MeV "<<eEnergySi<<endl;	//" a="<<a<<" b="<<b<<" c="<<c<<" alpha="<<alpha<<endl;  
}



//CsI light in channels
void EnergyTree::ECsIch(UShort_t chan){
  LightCsI=chan - ePied; 
  //cout<<"pedestal csi="<<ePied<<" chan="<<chan<<endl; //paola
  //L->Log<<"LightCsI ECsIch() = "<<LightCsI<<endl;
}
//set fragment A
void EnergyTree::SetFragmentA(Int_t Ain){ 
  lum->SetA(Ain);
  sA=Ain;
  //cout<<"A="<<sA<<endl; 
}

//Complete procedure for to get the energy losses from simulation
void  EnergyTree::CompleteSimulation(UShort_t chan){
  CalculateECsI();
  SimulateEvent();
  CalculateCanalCsI();
  //  cout<<"Event simulated"<<endl;

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
      CanalCsI=lum->Invert(csi->GetEnergy());
      difflum = (CanalCsI-LightCsI);
      //L->Log<<"Canal csi Invert() = "<<CanalCsI<<endl; //paola
}

//simulation of particle traversing the detector
void EnergyTree::SimulateEvent(){
//Double_t EnergyTree::SimulateEvent(){
  part.SetZ(eZ);
  part.SetA(sA);
  Einc=eEnergySi+eEnergyCsI;
  //L->Log<<"Einc = "<<Einc<<endl;
  //cout<<"Zsim="<<eZ<<" Asim="<<sA<<" Einc="<<Einc<<endl; //paola
  part.SetEnergy(Einc);
  kvt_sicsi->DetectParticle(&part);	//Change the telescope to tqke account the gap between the two detectors
  
  //L->Log<<"Edetsi = "<<detsi->GetEnergy()<<endl;
  //L->Log<<"Edetcsi = "<<detcsi->GetEnergy()<<endl;
  
  diffsi = eEnergySi-si->GetEnergy();
  diffcsi = RetrieveEnergyCsI()-csi->GetEnergy();
  //diffetot = si->GetIncidentEnergy(eZ,sA,eEnergySi)-(eEnergySi+RetrieveEnergyCsI());
  
  //L->Log<<"diff Si = "<<eEnergySi-detsi->GetEnergy()<<endl;
  //L->Log<<"diff CsI = "<<RetrieveEnergyCsI()-detcsi->GetEnergy()<<endl;
  //L->Log<<"diff Etot = "<<detsi->GetIncidentEnergy(eZ,sA,eEnergySi)-(eEnergySi+RetrieveEnergyCsI())<<endl;
  //part.Print();
  //return part.GetA()*part.GetAMeV();
}
//Energy in Si from simulation
void EnergyTree::GetESi(){
  sEnergySi=si->GetEnergy();
  //L->Log<<"GetESi:: sEnergySi= "<<sEnergySi<<endl;
  //cout<<"Z= "<<eZ<<endl;
}

//Energy in CsI from simulation
void EnergyTree::GetECsI(){
  sEnergyCsI=csi->GetEnergy();
  //cout<<"GetECsI:: sEnergyCsI= "<<sEnergyCsI<<endl;
}

// Reset the telescope in order to prepare for the next event
void EnergyTree::ClearTelescope(){
  part.Clear();
  si->Clear();		
  csi->Clear();
  kvt_sicsi->Clear();	
}

void EnergyTree::Bisection(Int_t A, UShort_t chan){ 
  Int_t middle, it=0;
  //L->Log<<"------------"<<endl;
  //L->Log<<"Valeur de A ; "<<A<<endl;
  if(A-5>0)
  left=A-5.;  //left and right are integer!! A is a double
  else left=1;
  right=A+10.;
  //L->Log<<"-----------------------------------BISECTION-------------------"<<endl;//paola

  //loop: bisection
  while(right-left!=1){

    //L->Log<<"-------------------loop bisection------------"<<endl;

    if(TMath::Even(right+left))
      {middle=(left+right)/2;}
    else
      {middle=(left+right+1)/2;}

       //L->Log<<"middle="<<middle<<endl; //paola

    //simulated energies stored for left A
    //L->Log<<"simulated energies stored for left A"<<endl; //paola
    //L->Log<<"left : "<<left<<endl;

    SetFragmentA(left);
    CompleteSimulation(chan);
    esi1=sEnergySi;
    ecsi1=sEnergyCsI;
    //L->Log<<"esi1="<<sEnergySi<<" ecsi1="<<sEnergyCsI<<endl; //paola

    //simulated energies stored for middle A
    //cout<<"simulated energies stored for middle A"<<endl; //paola
    //L->Log<<"middle : "<<middle<<endl;
    SetFragmentA(middle);
    CompleteSimulation(chan);
    esi2=sEnergySi;
    ecsi2=sEnergyCsI;
    //L->Log<<"esi2="<<sEnergySi<<" ecsi2="<<sEnergyCsI<<endl; //paola

    
    //difference between the simulated energies in Si and calibrated one
    diff1=eEnergySi-esi1;
    diff2=eEnergySi-esi2;
    //L->Log<<"diff1="<<diff1<<" diff2="<<diff2<<endl; //paola

    //condition for finding the right interval
    if (diff1*diff2>0)
      {left=middle;}
    else
      {right=middle;}
    //L->Log<<"left="<<left<<" right="<<right<<endl; //paola

    it++;
    //L->Log<<"----------------iteration= "<<it<<endl;
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

    //L->Log<<"esi1="<<esi1<<" ecsi1="<<ecsi1<<" esi2="<<esi2<<" ecsi2="<<ecsi2<<" esi mis="<<eEnergySi<<endl;
    //L->Log<<"A left="<<left<<" A right="<<right<<endl;
    //L->Log<<"----------------------END BISECTION-----------------"<<endl;
 
}
//interpolation usage for to find the right residual energy and right A, using 
//the final interval returned by bisection 
void EnergyTree::Interpolate(){
   sRefECsI= GetInterpolationD(esi1,ecsi1,esi2,ecsi2,eEnergySi);
   iA=GetInterpolationD(esi1,(Double_t)left,esi2,(Double_t)right,eEnergySi);  
    //L->Log<<"Ecsi interp="<<sRefECsI<<" A interp="<<iA<<endl; //paolaA
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

/*
//----------------------------MAIN----------------------------------//
void ident(Int_t sinum, Int_t csinum, Int_t term){
  Char_t filename[100]="filename";
  Char_t filename2[100]="filename2";
  Char_t ofilename[100]="ofilename";
  Char_t treename[30]="treename";
  Char_t bname1[30]="SIE_01";
  Char_t bname2[30]="CSI01";
  Char_t cut[30]="CUTG";
  //structure of the tree
  struct MyPoint{
    UShort_t ch_si;
    Double_t light_csi;
    Int_t A;
    Int_t Z;
    Double_t err_A;
    Double_t energy_si;
    Double_t energy_csi;
  };
  MyPoint point;
  Int_t eN;
  Int_t entry;
  Int_t A,Z;
  Float_t Cmin, Cmax;

  UShort_t sichan=0, csichan=0;

  //the queries from the program
  cout<<"Range file: "<<endl;
  cin.width(100);
  cin>>filename;  
  cout<<"filename read!"<<endl;
  cout<<"Root file: "<<endl;
  cin>>filename2;
  cin.width(30);
  cout<<"Tree name: "<<endl;
  cin>>treename;
  cout<<"Output root file:  "<<endl;
  cin>>ofilename;

  //input file with ranges for particular Z
  ifstream in;
  in.open(filename);

  //input root file with VAMOS data for one Brho
  TFile* f=new TFile(filename2,"READ");

  //preparing the name of the branches according to Si and Csi module number 
  //selected as arguments of main function ident_tree
  if(sinum<10){
    sprintf(bname1,"SIE_O%d", sinum);
  }
  else
    {
      sprintf(bname1,"SIE_%d", sinum);}

  if(csinum<10){
    sprintf(bname2,"CSIO%d", csinum);
  }
  else
    {
      sprintf(bname2,"CSI%d", csinum);}

   TTree* tree=(TTree*)f->Get(treename);

   //setting the branches of the input tree to fill in the channel variables
  tree->SetBranchAddress(bname1,&sichan);
  tree->SetBranchAddress(bname2,&csichan);

  //output root file
  TFile* f2=new TFile(ofilename,"RECREATE");

  //setting the branches of output root tree
  TTree* eTree=new TTree("ETree","Calculated values");
  eTree->Branch("ChannelSi",&point.ch_si,"ChannelSi/s");
  eTree->Branch("LightCsI",&point.light_csi,"LightCsI/D");
  eTree->Branch("ATrue",&point.A,"ATrue/I");
  eTree->Branch("ZTrue",&point.Z,"ZTrue/I");
  eTree->Branch("AErr",&point.err_A,"AErr/D");
  eTree->Branch("ESiTrue",&point.energy_si,"ESiTrue/D");
  eTree->Branch("ECsITrue",&point.energy_csi,"ECsITrue/D");

  //initialisation of the telescope and its calibration
  EnergyTree* ident=new EnergyTree(Siv *SiD);
    Si = SiD;
  ident->Init();
  //name of the gate applied to the channels in Si and CsI
  cout<<"Name of cut: "<<endl;
  cin>>cut;  
 

  //loop:reading the file with the ranges for a particular Z
  TString sline;
  char selection[100];
  Int_t j=0;
  while (in.good()){
    sline.ReadLine(in);
    if(!in.eof()){
      cout<<"#line read="<<j<<endl;
      j++;
      sscanf(sline.Data(), "%d %f %f",&Z,&Cmin,&Cmax);
      cout<<"Z="<<Z<<", Cmin="<<Cmin<<", Cmax="<<Cmax<<endl;
      cout<<"line scanned!"<<endl;
      A=2*Z;

      //"true" A and Z of the fragment, for to fill in the tree
      point.A=A+term;
      point.Z=Z;

      //setting the KNOWN Z
      ident->SetFragmentZ(Z);


      //choosing the entries corresponding to selected Z
      sprintf(selection,"%s && %s>%f && %s<%f",cut,bname1, Cmin, bname1, Cmax);
       cout<<"selection printed!: "<<selection;
       tree->Draw(">>MyList",selection);
      cout<<"list drawn!"<<endl;
      TEventList* list=(TEventList*)gROOT->FindObject("MyList");
      cout<<"list found!"<<endl;

      //number of entries within the selection
      eN=list->GetN();
      cout<<"#entries="<<eN<<endl;


      //loop: entries wihin the selection
      //      for(Int_t i=0;i<1;i++){
       for(Int_t i=0;i<eN;i++){  //paola
	entry=list->GetEntry(i);
	tree->GetEntry(entry);

	// if(Z==14)
	//	cout<<"sichan="<<sichan<<endl;
	//  if(Z==14)
	//	cout<<"csichan="<<csichan<<endl;

	//filling the tree...
	//calculation of the best estimate of residual energy for known Z
	point.energy_csi=ident->GetResidualEnergyCsI(sichan,csichan,Z);

	//Si energy
	point.energy_si=ident->RetrieveEnergySi();

	//best estimate of A
	point.err_A=ident->RetrieveA();

	//channel in Si
	point.ch_si=sichan;

	//light in CsI
	point.light_csi=ident->RetrieveLight();

	//  if(Z==14)	cout<<"A vera="<<point.A<<" Z vera="<<point.Z<<"best estimation A="<<point.err_A<<" E si="<<point.energy_si<<" Ecsi mis="<<point.light_csi<<" best estim E csi="<<point.energy_csi<<endl; //paola

	eTree->Fill();

       }//end del for
    }//end if
  }//end while

  eTree->Write();
  f2->Close();
  f->Close();
  in.close();
}
*/
