#include "Identificationv.h"
#include "TMath.h"
#include <cmath>
#include <iostream>
#include "CsICalib.h"
#include "KVNucleus.h"
#include "KVDetector.h"

#include "KVIDZAGrid.h"
#include "KVIDGridManager.h"
#include "KVINDRAe503.h"
#include "KVUnits.h"

#include "KVMaterial.h"
#include "KVIonRangeTable.h"
#include "KVIonRangeTableMaterial.h"

//Author: Maurycy Rejmund
using namespace std;

Bool_t DebugIDV = kFALSE;

ClassImp(Identificationv)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Identificationv</h2>
<p>Part of the VAMOS analysis package kindly contributed by Maurycy Rejmund (GANIL).</p>
<body>


</body>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////


class  MyFunctionObject {
public:

   TF1* dedx;
   Int_t fZ, fA;
   Double_t fRho;
   Double_t fEi;
   Double_t fEres;
   Double_t fDeltaE;
   MyFunctionObject(Int_t z, Int_t a, Int_t option)
   {
      fZ = z;
      fA = a;
      //fEi = ei;
      //fDeltaE = deltaE;

      Float_t pressure;
      KVMaterial* gas;

      if (option == 0) { //case DC1-DC2
         pressure = 13.;
         gas = new KVMaterial("C4H10", 2.*13.0);   //en cm - 2DC
         gas->SetPressure(pressure * KVUnits::mbar);
      } else if (option == 1) { //case Chio
         pressure = 40.;
         gas = new KVMaterial("C4H10", 13.8);   //en cm
         gas->SetPressure(pressure * KVUnits::mbar);
      } else {
         // Nasty error state, should not be encountered.
         std::cerr << "MyFunctionObject: Refusing to proceed (uninitialised!)"
                   << std::endl;
         std::abort();
      }

      KVIonRangeTable* t = gas->GetRangeTable();
      KVIonRangeTableMaterial* mat = t->GetMaterial("C4H10");
      mat->SetTemperatureAndPressure(19., pressure * KVUnits::mbar);

      fRho = mat->GetDensity();
      dedx = mat->GetStoppingFunction(z, a);

   }

// use constructor to customize your function object
   double operator()(double* x, double* p)
   {
      UNUSED(p);

      // z:fZ a:fA

      double e = *x;
      double result = 1. / (e * dedx->Eval(e));

      return result;
   }

   Double_t GetRho()
   {
      return fRho;
   }
   Double_t GetDeltaE()
   {
      return fDeltaE;
   }
   Double_t GetEres()
   {
      return fEres;
   }
   Double_t GetEi()
   {
      return fEi;
   }
   Int_t GetA()
   {
      return fA;
   }
   Int_t GetZ()
   {
      return fZ;
   }

};

Double_t DrawDeDx(Int_t z, Int_t a, Int_t option, Double_t Einc, Double_t Eres)
{
   MyFunctionObject* fobj;
   TF1* f;

   fobj = new MyFunctionObject(z, a, option);     // create the function object
   f = new TF1("f", fobj, 1e-5, 2000, 0, "MyFunctionObject"); // create TF1 class.
   f->SetNpx(10000);

   double rho = fobj->GetRho();
   double fact = TMath::Sqrt(0.7192 * (double)fobj->GetA()) * 1. / rho;
   //Double_t Einc = fobj->GetEi();
   //Double_t Eres = fobj->GetEres();
   double timeNS = fact * f->Integral(Eres, Einc);

   //cout<<"timeNS : "<<timeNS<<endl;
   delete fobj;
   delete f;

   return timeNS;
}

Identificationv::Identificationv(LogFile* Log, Reconstructionv* Recon,
                                 DriftChamberv* Drift, IonisationChamberv* IonCh, Sive503* SiD, CsIv* CsID, CsICalib* Ecsicalibrated)
{
   L = Log;
   Rec = Recon;
   Dr = Drift;
   Ic = IonCh;
   Si = SiD;
   CsI = CsID;

   energytree = Ecsicalibrated;

   debug = kFALSE;
   //for(i=0;i<6;i++)
   //Counter[i] = 0;

   NbSilicon =  gEnv->GetValue("VAMOS.NbSilicon", -1);
   NbCsI   = gEnv->GetValue("VAMOS.NbCsI", -1);
   MaxRun = gEnv->GetValue("VAMOS.MaxRun", -1);
   if (NbSilicon < 0) {
      cout << "Not Reading VAMOS.NbSilicon in Siv Class" << endl;
   }
   if (MaxRun < 0) {
      cout << "Not Reading VAMOS.MaxRun in Siv Class" << endl;
   }
   if (NbCsI < 0) {
      cout << "Not Reading VAMOS.NbCsI in CsIv Class" << endl;
   }

   Rnd = new Random;
   kvn = new KVNucleus();

//=======================================
   AA = new Float_t[NbCsI];
   FragECsI = new Double_t[NbCsI];
   FragEGap = new Double_t[NbCsI];
   Stat_Indra = new Float_t[MaxRun];

   PID = new Double_t[NbCsI];
   Z_PID = new Double_t[NbCsI];
   A_PID = new Double_t[NbCsI];
   Z_corr = new Int_t[NbCsI];
   PID_sitof = new Double_t[NbCsI];
   Z_PID_sitof = new Double_t[NbCsI];
   Z_corr_sitof = new Int_t[NbCsI];
   PID_chiov2 = new Double_t[NbCsI];
   Z_PID_chiov2 = new Double_t[NbCsI];
   Z_corr_chiov2 = new Int_t[NbCsI];

   //Q Correction Function according to the CsI detector
   P0 = new Float_t[NbCsI];
   P1 = new Float_t[NbCsI];
   P2 = new Float_t[NbCsI];
   P3 = new Float_t[NbCsI];

   // Correction de M/Q
   P0_mq = new Float_t[MaxRun];
   P1_mq = new Float_t[MaxRun];

   // Correction de M
   P0_m = new Float_t* [MaxRun];
   P1_m = new Float_t* [MaxRun];

// Correction de M Chio-Si
   P0_m_chiosi = new Float_t* [MaxRun];
   P1_m_chiosi = new Float_t* [MaxRun];

   // Correction de M Si-Tof
   P0_m_sitof = new Float_t* [MaxRun];
   P1_m_sitof = new Float_t* [MaxRun];

   for (Int_t i = 0; i < MaxRun; i++) {
      P0_m[i] = new Float_t[25];
      P1_m[i] = new Float_t[25];
      P0_m_chiosi[i] = new Float_t[25];
      P1_m_chiosi[i] = new Float_t[25];
      P0_m_sitof[i] = new Float_t[25];
      P1_m_sitof[i] = new Float_t[25];
   }

   // Correction de M/Q Chio-Si
   P0_mq_chiosi = new Float_t [MaxRun];
   P1_mq_chiosi = new Float_t [MaxRun];

   // Correction de M/Q Si-Tof
   P0_mq_sitof = new Float_t [MaxRun];
   P1_mq_sitof = new Float_t [MaxRun];


   // Tag des events
   Brho_min =  new Float_t[MaxRun];
   Brho_max =  new Float_t[MaxRun];
   /*Brho_min = new Float_t***[25];
   Brho_max = new Float_t***[25];
   for(Int_t i=0; i<25; i++){
      Brho_min[i] = new Float_t**[60];
      Brho_max[i] = new Float_t**[60];
      for(Int_t j=0; j<60; j++){
         Brho_min[i][j] = new Float_t*[21];
         Brho_max[i][j] = new Float_t*[21];
         for(Int_t k=0; k<21; k++){
            Brho_min[i][j][k] = new Float_t[MaxRun];
            Brho_max[i][j][k] = new Float_t[MaxRun];
         }
      }

   }*/

   Code_Ident_Vamos = new Int_t[NbCsI];
   Code_Vamos = new Int_t[NbCsI];

   // Correction de Tof pour identification Chio-Si
   Tof0 = new Float_t[MaxRun];
   Esi0 = new Float_t[MaxRun];
   Brho0 = new Float_t[MaxRun];
   // Correction de Tof pour identification Si-CsI
   Tof0_code1 = new Float_t[MaxRun];
   Deltat_code1 = new Float_t[MaxRun];

   // Correction Double tof Pics

   Fit1 = new Float_t** [25];
   Fit2 = new Float_t** [25];
   Fit3 = new Float_t** [25];
   Fit4 = new Float_t** [25];
   Fit5 = new Float_t** [25];
   Fit6 = new Float_t** [25];
   Fit7 = new Float_t** [25];
   for (Int_t i = 0; i < 25; i++) {
      Fit1[i] = new Float_t* [2];
      Fit2[i] = new Float_t* [2];
      Fit3[i] = new Float_t* [2];
      Fit4[i] = new Float_t* [2];
      Fit5[i] = new Float_t* [2];
      Fit6[i] = new Float_t* [2];
      Fit7[i] = new Float_t* [2];
      for (Int_t j = 0; j < 2 ; j++) {
         Fit1[i][j] = new Float_t [MaxRun];
         Fit2[i][j] = new Float_t [MaxRun];
         Fit3[i][j] = new Float_t [MaxRun];
         Fit4[i][j] = new Float_t [MaxRun];
         Fit5[i][j] = new Float_t [MaxRun];
         Fit6[i][j] = new Float_t [MaxRun];
         Fit7[i][j] = new Float_t [MaxRun];
      }
   }

   // Correction Offet relativiste
   Offset_rela = new Float_t[MaxRun];
   Offset_relativiste_chiosi = new Float_t[MaxRun];

   // Redressement des distributions A/Q
   P0_aq_straight = new Float_t[NbSilicon];
   P1_aq_straight = new Float_t[NbSilicon];
   P2_aq_straight = new Float_t[NbSilicon];

   P0_aq_straight_chiosi = new Float_t[7];
   P1_aq_straight_chiosi = new Float_t[7];
   P2_aq_straight_chiosi = new Float_t[7];

   FragDetSi = new Int_t[NbCsI];
   FragSignalSi = new Int_t[NbCsI];
   FragESi = new Float_t[NbCsI];
   FragTfinal = new Double_t[NbCsI];
   Off_chiosi = new Double_t[NbCsI];
   Off_dc1dc2 = new Double_t[NbCsI];
   TOF_chiosi = new Double_t[NbCsI];

   FragSignalCsI = new Int_t[NbCsI];
   FragDetCsI = new Int_t[NbCsI];

   FragDetChio = new Int_t[NbCsI];
   FragSignalChio = new Int_t[NbCsI];
   FragEChio = new Float_t[NbCsI];

   V = new Float_t[NbCsI];
   Vx = new Float_t[NbCsI];
   Vy = new Float_t[NbCsI];
   Vz = new Float_t[NbCsI];

   Beta = new Float_t[NbCsI];
   M_Q = new Float_t[NbCsI];
   Etot = new Float_t[NbCsI];
   M = new Float_t[NbCsI];
   Q  = new Float_t[NbCsI];

   Beta_chiosi = new Float_t[NbCsI];
   M_Q_chiosi = new Float_t[NbCsI];
   M_chiosi = new Float_t[NbCsI];
   Q_chiosi  = new Float_t[NbCsI];

   RealQ_straight = new  Float_t[NbCsI];
   Q_straight = new  Int_t[NbCsI];
   M_straight = new  Float_t[NbCsI];

   geom = new Int_t*[NbSilicon];
   geom_cross = new Int_t*[NbSilicon];
   geomchiosi = new Int_t*[NbSilicon];
   geomchiocsi = new Int_t*[NbCsI];

   for (Int_t i = 0; i < NbSilicon; i++) {
      geom[i] = new Int_t[6];
      geom_cross[i] = new Int_t[4];
      geomchiosi[i] = new Int_t[2];
   }
   for (Int_t i = 0; i < NbCsI; i++) {
      geomchiocsi[i] = new Int_t[2];
   }


   //=======================================
   InitReadVariables();
   Geometry();
   GeometryChioSi();
   GeometryChioCsI();
   GeometrySiCsICross();

   grid_list = 0;

   if (gIDGridManager != 0) {
      grid_list = (KVList*) gIDGridManager->GetGrids();
      if (grid_list == 0)   printf("Error: gIDGridManager->GetGrids() failed\n");
   } else {
      cout << "gIDGridManager NULL" << endl;
   }

//Reading Files :
   ReadQCorrection();
   ReadAQCorrection();
   ReadACorrection();
   ReadFlagVamos();
   ReadToFCorrectionChioSi();
   ReadToFCorrectionCode1();
   ReadStatIndra();
   ReadStraightAQ();
   ReadDoublingCorrection();
   ReadOffsetsChiosi();

   event_number = 0;

}

void Identificationv::LoadGridForCode2Cuts()
{

   cout << "run : " << gIndra->GetCurrentRunNumber() << " Brho0 : " << Brho0[int(gIndra->GetCurrentRunNumber())] << endl;
   energytree->InitCode2Cuts(Brho0[int(gIndra->GetCurrentRunNumber())]);

   return;
}


Identificationv::~Identificationv(void)
{

   delete [] AA ;
   delete [] FragECsI;
   delete [] FragEGap;
   delete [] Stat_Indra;

   delete [] PID;
   delete [] PID_sitof;
   delete [] PID_chiov2;
   delete [] Z_PID;
   delete [] A_PID;
   delete [] Z_corr;
   delete [] Z_PID_sitof;
   delete [] Z_corr_sitof;
   delete [] Z_PID_chiov2;
   delete [] Z_corr_chiov2;
   //Q Correction Function according to the CsI detector
   delete [] P0;
   delete [] P1;
   delete [] P2;
   delete [] P3;

   // Correction de M/Q
   delete [] P0_mq;
   delete [] P1_mq;

   // Correction de M
   delete [] P0_m;
   delete [] P1_m;

// Correction de M Chio-Si
   delete [] P0_m_chiosi;
   delete [] P1_m_chiosi;

   // Correction de M Si-Tof
   delete [] P0_m_sitof;
   delete [] P1_m_sitof;

   // Correction de M/Q Chio-Si
   delete [] P0_mq_chiosi;
   delete [] P1_mq_chiosi;

   // Correction de M/Q Si-Tof
   delete []P0_mq_sitof;
   delete []P1_mq_sitof;


   // Tag des events
   delete []Brho_min;
   delete []Brho_max;

   delete []Code_Ident_Vamos;
   delete []Code_Vamos;

   // Correction de Tof pour identification Chio-Si
   delete []Tof0;
   delete []Esi0;
   delete []Brho0;

   // Correction de Tof pour identification Si-CsI
   delete []Tof0_code1;
   delete []Deltat_code1;

   // Correction Double tof Pics

   delete []Fit1;
   delete []Fit2;
   delete []Fit3;
   delete []Fit4;
   delete []Fit5;
   delete []Fit6;
   delete []Fit7;

   // Correction Offet relativiste
   delete []Offset_rela;
   delete []Offset_relativiste_chiosi;

   // Redressement des distributions A/Q
   delete []P0_aq_straight;
   delete []P1_aq_straight;
   delete []P2_aq_straight;

   delete []P0_aq_straight_chiosi;
   delete []P1_aq_straight_chiosi;
   delete []P2_aq_straight_chiosi;

   delete []FragDetSi;
   delete []FragSignalSi;
   delete []FragESi;

   delete []FragSignalCsI;
   delete []FragDetCsI;

   delete []FragDetChio;
   delete []FragSignalChio;
   delete []FragEChio;

   delete []FragTfinal;
   delete []Off_chiosi;
   delete []Off_dc1dc2;
   delete []TOF_chiosi;

   delete []V;
   delete []Vx;
   delete []Vy;
   delete []Vz;

   delete []Beta;
   delete []M_Q;
   delete []Etot;
   delete []M;
   delete []Q;

   delete []Beta_chiosi;
   delete []M_Q_chiosi;
   delete []M_chiosi;
   delete []Q_chiosi;


   delete []RealQ_straight;
   delete []Q_straight;
   delete []M_straight;

   delete [] geom;
   delete [] geom_cross;
   delete [] geomchiosi;
   delete [] geomchiocsi;

   delete Rnd;
   delete id;
   delete kvn;
}

void Identificationv::InitReadVariables(void)
{


   for (Int_t i = 0; i < 18; i++) {
      for (Int_t j = 0; j < 6; j++) {
         geom[i][j] = -1;
         if (j < 3)  geomchiosi[i][j] = -1;
      }
   }

   for (Int_t i = 0; i < NbCsI; i++) {
      P0[i] = 0.;
      P1[i] = 0.;
      P2[i] = 0.;
   }
   for (Int_t i = 0; i < 600; i++) {
      P0_mq[i] = 0.;
      P1_mq[i] = 0.;
      P0_mq_chiosi[i] = 0.;
      P1_mq_chiosi[i] = 0.;
      P0_mq_sitof[i] = 0.;
      P1_mq_sitof[i] = 0.;
      Tof0[i] = 0.;
      Esi0[i] = 0.;
      Brho0[i] = 0.;
      Tof0_code1[i] = 0.;
      Deltat_code1[i] = 0.;
      for (Int_t j = 0; j < 25; j++) {
         P0_m[i][j] = 0.;
         P1_m[i][j] = 0.;
         P0_m_chiosi[i][j] = 0.;
         P1_m_chiosi[i][j] = 0.;
         P0_m_sitof[i][j] = 0.;
         P1_m_sitof[i][j] = 0.;

      }
   }

   /*for(Int_t i=0;i<25;i++){
      for(Int_t j=0;j<60;j++){
         for(Int_t k=0;k<10;k++){
            for(Int_t l=0;l<MaxRun;l++){
               Brho_min[i][j][k][l] = -10.0;
               Brho_max[i][j][k][l] = -10.0;
            }
         }
      }
   }*/

   for (Int_t i = 0; i < MaxRun; i++) {
      Stat_Indra[i] = -10.0;
      Stat_Indra[i] = -10.0;

      Brho_min[i] = -10.0;
      Brho_max[i] = -10.0;

      Offset_relativiste_chiosi[i] = 0.0;
   }

   for (Int_t i = 0; i < 18; i++) {
      P0_aq_straight[i] = 0.0;
      P1_aq_straight[i] = 0.0;
      P2_aq_straight[i] = 0.0;
   }
   for (Int_t i = 0; i < 7; i++) {
      P0_aq_straight_chiosi[i] = 0.0;
      P1_aq_straight_chiosi[i] = 0.0;
      P2_aq_straight_chiosi[i] = 0.0;
   }

   for (Int_t i = 0; i < 25; i++) {
      for (Int_t j = 0; j < 2; j++) {
         for (Int_t k = 0; k < MaxRun; k++) {
            Fit1[i][j][k] = -1000.0;
            Fit2[i][j][k] = -1000.0;
            Fit3[i][j][k] = -1000.0;
            Fit4[i][j][k] = -1000.0;
            Fit5[i][j][k] = -1000.0;
            Fit6[i][j][k] = -1000.0;
            Fit7[i][j][k] = -1000.0;
         }
      }
   }

   return;
}

void Identificationv::Init(void)
{
   Present = false;
   dE = dE1 = V_Etot = T_FP = Mass = M_simul = Z1 = Z2 = Z_tot = Z_si = D = -10.0;
   M_Qr = Mr = Qr = -10.0;
   Qc = Mc = -10.0;

   Gamma = 1.;
   initThickness = 0.;

   einc_si = einc_isogap1 = eloss_isogap1 = einc_ic = eloss_ic = einc_dc2 = eloss_dc2 = einc_sed = eloss_sed = einc_dc1 = eloss_dc1 = einc_tgt = eloss_tgt = 0.0;
   E_tgt = E_dc1 = E_dc2 = E_sed = E_gap1 = E_chio = 0.0;

   for (Int_t i = 0; i < 3; i++) {
      fELosLayer_dc1[i] = 0.0;
      fELosLayer_dc2[i] = 0.0;
      fELosLayer_ic[i] = 0.0;
   }
   runNumber = 0;
   runNumber = (Int_t)gIndra->GetCurrentRunNumber();

   for (Int_t i = 0; i < NbCsI; i++) {
      FragDetSi[i] = -10;
      FragSignalSi[i] = -10;
      FragESi[i] = -10.0;
      FragTfinal[i] = -10.0;
      Off_chiosi[i] = -10.0;
      Off_dc1dc2[i] = -10.0;
      TOF_chiosi[i] = -10.0;

      FragSignalCsI[i] = -10;
      FragDetCsI[i] = -10;

      FragDetChio[i] = -10;
      FragSignalChio[i] = -10;
      FragEChio[i] = -10.0;

      A_PID[i] = -10.0;
      Z_PID[i] = -10.0;
      PID[i] = -10.0;
      PID_sitof[i] = -10.0;
      PID_chiov2[i] = -10.0;
      Z_corr[i] = -10;
      Z_PID_sitof[i] = -10.0;
      Z_corr_sitof[i] = -10;
      Z_PID_chiov2[i] = -10.0;
      Z_corr_chiov2[i] = -10;
      FragECsI[i] = -10.0;
      FragEGap[i] = -10.0;
      AA[i] = -10.0;

      Code_Ident_Vamos[i] = -10;
      Code_Vamos[i] = -10;
      V[i] = -10.0;
      Vx[i] = -10.0;
      Vy[i] = -10.0;
      Vz[i] = -10.0;
      Beta[i] = -10.0;
      M_Q[i] = -10.0;
      Etot[i] = -10.0;
      M[i] = -10.0;
      Q[i] = -10.0;

      Beta_chiosi[i] = -10.0;
      M_Q_chiosi[i] = -10.0;
      M_chiosi[i] = -10.0;
      Q_chiosi[i] = -10.0;

      RealQ_straight[i] = -10.0;
      Q_straight[i] = -10;
      M_straight[i] = -10.0;

   }
   MaxM = 0;
   fMultiplicity_case = -10;
   Code_good_event = -10;
   print = kFALSE;

   return;
}

void Identificationv::InitSavedQuantities(void)
{

   for (Int_t i = 0; i < NbCsI; i++) {
      FragDetSi[i] = -10;
      FragSignalSi[i] = -10;
      FragESi[i] = -10.0;
      FragTfinal[i] = -10.0;
      Off_chiosi[i] = -10.0;
      Off_dc1dc2[i] = -10.0;
      TOF_chiosi[i] = -10.0;

      FragSignalCsI[i] = -10;
      FragDetCsI[i] = -10;

      FragDetChio[i] = -10;
      FragSignalChio[i] = -10;
      FragEChio[i] = -10.0;

   }

   return;
}
//===================================================

Int_t Identificationv::GetMaxMultiplicity(void)
{
   if (Rec->Brho < 0) {
      fMultiplicity_case = -3;
      Code_good_event = 3;
      return 0;
   }
   if (Si->EMSI >= Ic->EMIC && Si->EMSI >= CsI->EMCSI) {
      fMultiplicity_case = 1;
      return Si->EMSI;
   } else if (CsI->EMCSI > Si->EMSI && CsI->EMCSI >= Ic->EMIC) {
      fMultiplicity_case = 2;
      return CsI->EMCSI;
   } else if (Ic->EMIC > Si->EMSI && Ic->EMIC > CsI->EMCSI) {
      fMultiplicity_case = 0;
      return Ic->EMIC;
   } else {
      fMultiplicity_case = -10;
      cout << "Not suppose to enter here GetMaxMultiplicty()" << endl;
      return 0;
   }

}

void Identificationv::FragPropertiesInOrder(void)
{

   Int_t nhit_sicsi = 0;
   Int_t nhit_chiosi = 0;
   Int_t nhit_chiocsi = 0;
   MaxM = GetMaxMultiplicity();
   Code_good_event = 0;

   bool used_silicon[Si->EMSI];
   bool not_recoverable = kFALSE;
   for (Int_t i = 0; i < Si->EMSI; i++) {
      used_silicon[i] = kFALSE;
   }

   InitSavedQuantities();

   if (debug)cout << "fMultiplicity_case : " << fMultiplicity_case << endl;
   if (debug)cout << "code good event : " << Code_good_event << endl;
   for (Int_t i = 0; i < GetMaxMultiplicity(); i++) {
      if (debug)cout << "Chio before : " << FragDetChio[i] << "	" << FragSignalChio[i] << "	" << FragEChio[i] << endl;
      if (debug)cout << "Si before : " << FragDetSi[i] << "	" << FragSignalSi[i] << "	" << FragESi[i] << endl;
      if (debug)cout << "CsI before : " << FragDetCsI[i] << "	" << FragSignalCsI[i] << endl;
   }
   if (fMultiplicity_case == 1) { //Si->EMSI greater or equal to the other multiplicities
      Code_good_event = -10;
      for (Int_t i = 0; i < Si->EMSI; i++) {

         if (Si->EMSI == 1) {
            if (Si->DetSi[i] == 16) {
               if (Si->E[i] > ((4.36 * Si->T_final[i]) - 311.)) {
                  if ((CsI->DetCsI[i] == 45 && Si->E[i] < ((-0.0548 * CsI->CsIERaw[i]) + 384.) &&  Si->E[i] > ((-0.0556 * CsI->CsIERaw[i]) + 340.)) ||
                        (CsI->DetCsI[i] == 46 && Si->E[i] < ((-0.0328 * CsI->CsIERaw[i]) + 365.) &&  Si->E[i] > ((-0.0353 * CsI->CsIERaw[i]) + 345.))) {

                     cout << "DetCsI : " << CsI->DetCsI[i] << endl;
                     cout << "ESi : " << Si->E[i] << "	Tof : " << Si->T_final[i] << "	CsICanal : " << CsI->CsIERaw[i] << endl;
                     print = kTRUE;
                  }
               }
            }
         }

         FragDetSi[i] = Si->DetSi[i];
         FragSignalSi[i] = Si->SiERaw[i];
         FragESi[i] = Si->E[i];
         FragTfinal[i] = Si->T_final[i];

         if (debug)cout << "i : " << i << " FragDetSi[i] : " << FragDetSi[i] << " FragSignalSi[i] : " << FragSignalSi[i] << " FragESi[i] : " << FragESi[i] << "	FragTfinal : " << FragTfinal[i] << endl;

         for (Int_t j = 0; j < CsI->EMCSI; j++) {
            if (debug)cout << "j : " << j << " " << CsI->DetCsI[j] << " " << CsI->CsIERaw[j] << endl;
            if (debug)cout << "geometry sicsi : " << IsSiCsITelescope(Si->DetSi[i] - 1, CsI->DetCsI[j] - 1) << endl;
            if (CsI->CsIERaw[j] > 0 && IsSiCsITelescope(Si->DetSi[i] - 1, CsI->DetCsI[j] - 1) == 1) {
               if (nhit_sicsi > 0) {
                  if (debug)cout << "Double Hit In SiCsI" << endl;
               }
               FragSignalCsI[i] = CsI->CsIERaw[j];
               FragDetCsI[i] = CsI->DetCsI[j];
               nhit_sicsi++;

            } //IsSiCsITelescope
         } //Multiplicity CsI

         for (Int_t k = 0; k < Ic->EMIC; k++) {
            if (debug)cout << "k : " << k << " " << Ic->DetChio[k] << " " << Ic->E[k] << endl;
            if (debug)cout << "geometry chiosi : " << IsChioSiTelescope(Ic->DetChio[k] - 1, Si->DetSi[i] - 1) << endl;
            if (Ic->E[k] > 0 && IsChioSiTelescope(Ic->DetChio[k] - 1, Si->DetSi[i] - 1) == 1) {
               if (nhit_chiosi > 0) {
                  if (debug)cout << "Double Hit In ChioSi" << endl;
               }
               FragSignalChio[i] = Ic->IcRaw[k];
               FragDetChio[i] = Ic->DetChio[k];
               FragEChio[i] = Ic->E[k];
               nhit_chiosi++;

            } //IsChioSiTelescope
         } //Multiplicity IC

         if (nhit_sicsi > 1 || nhit_chiosi > 1) {
            Code_good_event = 4;
            break;
         }
         nhit_sicsi = 0;
         nhit_chiosi = 0;

      } //Multiplicity Si

      if (Si->EMSI == CsI->EMCSI && Si->EMSI == 1) { //We restrict to events with multiplicity = 1, because after we analyze only mulitplicity 1 events
         if (FragDetCsI[0] == -10) {
            if (IsCsISiCrossTelescope(FragDetSi[0] - 1, CsI->DetCsI[0] - 1) == 1) {
               Code_good_event = 6; //Fragment hit one Si and enters in the CsI behind a neighbour Si. We can't identify those events in Chio-Si,
               //because the ptcle. is not stopped in Si
            } else {
               Code_good_event = 7; //No relation between the Si and the CsI. Possible? Chio-Si identification if there's a chio, but we lose the information about the CsI.
            }

         }
      }
   }

   else if (fMultiplicity_case == 2) {
      for (Int_t i = 0; i < CsI->EMCSI; i++) {
         FragSignalCsI[i] = CsI->CsIERaw[i];
         FragDetCsI[i] = CsI->DetCsI[i];

         for (Int_t j = 0; j < Si->EMSI; j++) {
            if (Si->E[j] > 0 && IsSiCsITelescope(Si->DetSi[j] - 1, CsI->DetCsI[i] - 1) == 1) {
               if (used_silicon[j] == kTRUE) {
                  not_recoverable = kTRUE;
                  if (debug)cout << "2 CsI behind the same Si" << endl;
               }
               if (nhit_sicsi > 0) {
                  if (debug)cout << "Double Hit In SiCsI" << endl;
               }
               used_silicon[j] = kTRUE;
               FragDetSi[i] = Si->DetSi[j];
               FragSignalSi[i] = Si->SiERaw[j];
               FragESi[i] = Si->E[j];
               FragTfinal[i] = Si->T_final[j];
               nhit_sicsi++;

            } //IsSiCsITelescope
         } //Multiplicity Si

         for (Int_t k = 0; k < Ic->EMIC; k++) {
            if (Ic->E[k] > 0 && IsChioCsITelescope(Ic->DetChio[k] - 1, CsI->DetCsI[i] - 1) == 1) {
               if (nhit_chiocsi > 0) {
                  if (debug)cout << "Double Hit In ChioCsI" << endl;
               }
               FragSignalChio[i] = Ic->IcRaw[k];
               FragDetChio[i] = Ic->DetChio[k];
               FragEChio[i] = Ic->E[k];
               nhit_chiocsi++;

            } //IsChioSiTelescope
         } //Multiplicity IC

         if (nhit_sicsi > 1 || nhit_chiocsi > 1 || not_recoverable == kTRUE) {
            //if(nhit_sicsi>1 || nhit_chiocsi>1 ){
            Code_good_event = 4;
            break;
         }
         nhit_sicsi = 0;
         nhit_chiocsi = 0;
      } //Multiplicity Si

      //if(((CsI->EMCSI==2 && Si->EMSI==1) ||  (CsI->EMCSI==3 && Si->EMSI==1)) && FragDetSi[0]>0){
      if ((CsI->EMCSI == 2 && Si->EMSI == 1) && ((FragDetSi[0] > 0) ^ (FragDetSi[1] > 0))) {
         Code_good_event = -8;
      } else {
         Code_good_event = 9;
      }
      if ((CsI->EMCSI == 3 && Si->EMSI == 1) && ((FragDetSi[0] > 0) ^ (FragDetSi[1] > 0) ^ (FragDetSi[2] > 0))) {
         Code_good_event = -8;
      } else {
         Code_good_event = 9;
      }
      if (CsI->EMCSI == 2 && Si->EMSI == 1 && FragDetSi[0] == -10) { //We restrict to events with multiplicity = 1, because after we analyze only mulitplicity 1 events
         for (Int_t i = 0; i < CsI->EMCSI; i++) {
            if (IsCsISiCrossTelescope(Si->DetSi[0] - 1, CsI->DetCsI[i] - 1) == 1) {
               Code_good_event = 6; //Fragment hit one Si and enters in the CsI behind a neighbour Si. We can't identify those events in Chio-Si,
               //because the ptcle. is not stopped in Si
            }
         }
         if (Code_good_event == 0) {
            Code_good_event = 9;

         }
      }

   }

   else if (fMultiplicity_case == 0) {
      Code_good_event = 10;
      for (Int_t i = 0; i < Ic->EMIC; i++) {
         FragSignalChio[i] = Ic->IcRaw[i];
         FragDetChio[i] = Ic->DetChio[i];
         FragEChio[i] = Ic->E[i];

         for (Int_t j = 0; j < Si->EMSI; j++) {
            if (Si->E[j] > 0 && IsChioSiTelescope(Ic->DetChio[i] - 1, Si->DetSi[j] - 1) == 1) {
               if (nhit_chiosi > 0) {
                  if (debug)cout << "Double Hit In ChioSi" << endl;
               }
               FragDetSi[i] = Si->DetSi[j];
               FragSignalSi[i] = Si->SiERaw[j];
               FragESi[i] = Si->E[j];
               FragTfinal[i] = Si->T_final[j];
               nhit_chiosi++;

            } //IsSiCsITelescope
         } //Multiplicity Si

         for (Int_t j = 0; j < CsI->EMCSI; j++) {
            if (CsI->CsIERaw[j] > 0 && IsChioCsITelescope(Ic->DetChio[i] - 1, CsI->DetCsI[j] - 1) == 1) {
               if (nhit_chiocsi > 0) {
                  if (debug)cout << "Double Hit In ChioCsI" << endl;
               }
               FragSignalCsI[i] = CsI->CsIERaw[j];
               FragDetCsI[i] = CsI->DetCsI[j];
               nhit_chiocsi++;

            } //IsSiCsITelescope
         } //Multiplicity CsI

         if (nhit_sicsi > 1 || nhit_chiocsi > 1) {
            Code_good_event = 4;
            break;
         }
         nhit_chiosi = 0;
         nhit_chiocsi = 0;
      }
   }

   else {
      Code_good_event = 4;
      if (debug)cout << "Should not happen FragPropertiesInOrder()" << endl;
   }
   if (debug)cout << "Code good event at the end : " << Code_good_event << endl;

   if (Code_good_event == 4) {
      if (debug)cout << "Reset everything" << endl;
      for (Int_t i = 0; i < GetMaxMultiplicity(); i++) {
         FragDetSi[i] = -10;
         FragSignalSi[i] = -10;
         FragESi[i] = -10.0;
         FragTfinal[i] = -10.0;

         FragSignalCsI[i] = -10;
         FragDetCsI[i] = -10;

         FragDetChio[i] = -10;
         FragSignalChio[i] = -10;
         FragEChio[i] = -10.0;
      }
   }
   if (debug) {
      for (Int_t i = 0; i < GetMaxMultiplicity(); i++) {
         cout << "Chio : " << FragDetChio[i] << "	" << FragSignalChio[i] << "	" << FragEChio[i] << endl;
         cout << "Si : " << FragDetSi[i] << "	" << FragSignalSi[i] << "	" << FragESi[i] << endl;
         cout << "CsI : " << FragDetCsI[i] << "	" << FragSignalCsI[i] << endl;
         cout << "Tof : " << FragTfinal[i] << endl;
      }
   }

   return;
}


void Identificationv::Calculate(void)
{

//=================================================
// Z Identification
   bool identified = kFALSE;

   if (Code_good_event == -10  && Si->EMSI == 1) {
      for (Int_t y = 0; y < Si->EMSI ; y++) {
         identified = kFALSE;
         if (FragSignalCsI[y] > 0) { // if the channel is above the pedestal
            //IdentificationSi-CsI
            energytree->InitTelescope(FragDetSi[y] - 1, FragDetCsI[y] - 1);
            energytree->InitSiCsI(FragDetSi[y]);
            energytree->SetCalibration(Si, CsI, FragDetSi[y] - 1, FragDetCsI[y] - 1);
            id = new KVIdentificationResult();
            id_sitof = new KVIdentificationResult();
            char scope_name [256];
            char scope_name_sitof [256];
            sprintf(scope_name, "null");
            sprintf(scope_name_sitof, "null");
            if (energytree->kvid == NULL) {
               if (debug)cout << "See you!!" << endl << flush;
               Code_Ident_Vamos[y] = 31;  //ID failed because no Si-CsI grid
               if (debug)cout << "Code 31 : " << Code_Ident_Vamos[y] << endl;
               identified = kTRUE;
            } else {
               sprintf(scope_name, "%s", energytree->kvid->GetName());
               if (debug)cout << "Si-CsI name : " << energytree->kvid->GetName() << endl;
               grd = 0;
               if ((grd = (KVIDGraph*) grid_list->FindObjectByName(scope_name)) != 0) {
                  if (energytree->kvid->IsIdentifiable(double(FragSignalCsI[y]), double(FragESi[y]))) {
                     //Code_Ident_Vamos[y]=1;
                     //if(debug)cout<<"Code1 : "<<Code_Ident_Vamos[y]<<endl;
                     energytree->kvid->Identify(double(FragSignalCsI[y]), double(FragESi[y]), id);         //energytree->kvid : KVIDGraph  Identify(x,y)
                     A_PID[y] = id->A;
                     Z_PID[y] = id->Z;
                     Z_corr[y] = int(Z_PID[y]);
                     PID[y] = id->PID;
                     energytree->SetFragmentZ(Z_corr[y]);
                     energytree->GetResidualEnergyCsI(double(FragESi[y]), double(FragSignalCsI[y]));       //Method called for guessing A value by bissection method and getting CsI energy
                     if (energytree->good_bisection == kFALSE) Code_Ident_Vamos[y] = 37;
                     FragECsI[y] = energytree->RetrieveEnergyCsI();
                     FragEGap[y] = energytree->eEnergyGap;

                     AA[y] = energytree->RetrieveA();
                     identified = kTRUE;

                     energytree->InitTelescopeSiTof(FragDetSi[y] - 1);
                     if (energytree->kvid_sitof == NULL) {
                        Code_Ident_Vamos[y] = 38;

                     } else {
                        sprintf(scope_name_sitof, "%s", energytree->kvid_sitof->GetName());
                        if ((grd = (KVIDGraph*) grid_list->FindObjectByName(scope_name_sitof)) != 0) {
                           if (energytree->kvid_sitof->IsIdentifiable(double(FragTfinal[y]), double(FragESi[y]))) {
                              //cout<<"Identifiable"<<endl;
                              energytree->kvid_sitof->Identify(double(FragTfinal[y]), double(FragESi[y]), id_sitof);
                              PID_sitof[y] = id_sitof->PID;
                              Z_PID_sitof[y] = id_sitof->Z;
                              Z_corr_sitof[y] = int(Z_PID_sitof[y]);
                              if ((id_sitof->PID >= (PID[y] - 0.7)) && (id_sitof->PID <= (PID[y] + 0.7))) {
                                 Code_Ident_Vamos[y] = 1;
                                 //cout<<"Good Z. Youppi!"<<endl;

                              } else {
                                 Code_Ident_Vamos[y] = 39;
                                 //cout<<"Bad Z. Bou!"<<endl;
                                 //cout<<"TOF : "<<FragTfinal[y]<<"  ESI : "<<FragESi[y]<<"  DetSi : "<<FragDetSi[y]<<" Z Si-CsI : "<<PID[y]<<
                                 //"   Z Chio-Si : "<<id_sitof->PID<<endl;
                              }
                           } else {
                              Code_Ident_Vamos[y] = 40;
                              //cout<<"Non identifiable Si-TOF"<<endl;
                              //cout<<"TOF : "<<FragTfinal[y]<<"  ESI : "<<FragESi[y]<<"  DetSi : "<<FragDetSi[y]<<" Z Si-CsI : "<<PID[y]<<endl;
                           }

                        } else {
                           cout << "grd NULL, but I don't know why!!!" << endl;
                        }


                     }

                     if (print) {
                        cout << "DetCsI : " << CsI->DetCsI[y] << endl;
                        cout << "ESi : " << Si->E[y] << "	Tof : " << Si->T_final[y] << "	CsICanal : " << CsI->CsIERaw[y] << endl;
                        cout << "Z : " << PID[y] << "	ECsI : " << FragECsI[y] << endl;
                     }

                  }  //Is Identifiable
                  else {
                     Code_Ident_Vamos[y] = 35;  //Signal in CsI, but noise...
                     identified = kTRUE;
                  }
               } else {
                  Code_Ident_Vamos[y] = 37;  //Signal in CsI, but noise...
                  identified = kTRUE;
               }

               delete id;
               delete id_sitof;

            }
         } //Signal CsI>piedestal

         if (FragEChio[y] > 0 && FragSignalCsI[y] <= 0) {
            //Identification Chio-Si
            energytree->InitTelescopeChioSi(FragDetChio[y], FragDetSi[y] - 1); //Ic->Number (1-7), Si->Number(0,17)
            id_chiosi = new KVIdentificationResult();
            char scope_name_chiosi [256];
            sprintf(scope_name_chiosi, "null");
            if (energytree->kvid_chiosi == NULL) {
               if (debug)cout << "See you Baby!!" << endl;
               Code_Ident_Vamos[y] = 32;  //ID failed because no Chio-Si grid
               identified = kTRUE;
            } else {
               sprintf(scope_name_chiosi, "%s", energytree->kvid_chiosi->GetName());
               if (debug)cout << "Chio-Si name : " << energytree->kvid_chiosi->GetName() << endl;
               grd = 0;
               if ((grd = (KVIDGraph*) grid_list->FindObjectByName(scope_name_chiosi)) != 0) {
                  if (energytree->kvid_chiosi->IsIdentifiable(FragESi[y], FragEChio[y])) {
                     Code_Ident_Vamos[y] = 2;
                     energytree->kvid_chiosi->Identify(FragESi[y], FragEChio[y] , id_chiosi);   //energytree->kvid : KVIDGraph   Identify(x,y)
                     A_PID[y] = id_chiosi->A;

                     Z_PID[y] = (id_chiosi->Z);
                     Z_corr[y] = int(Z_PID[y]);
                     PID[y] = (id_chiosi->PID);


                     identified = kTRUE;

                  }  //IsIdentificable
                  else {
                     Code_Ident_Vamos[y] = 36;  //Signal in Chio, but noise...
                     identified = kTRUE;
                  }
               }  //grid exist
               delete id_chiosi;
            }
         } //identified==kFALSE && FragEChio[y]>0
         if (identified == kFALSE) {
            Code_Ident_Vamos[y] = 3;
         }
      } //Multiplicity Si

      for (Int_t y = 0; y < Si->EMSI ; y++) {
         if (Code_Ident_Vamos[y] < 0) {
            if (debug)cout << "**************Code_Ident_Vamos=-10 OUT**************" << endl;

         }
      }
   } //good event

   if (Code_good_event == -8) {
      for (Int_t y = 0; y < CsI->EMCSI ; y++) {
         identified = kFALSE;
         if (FragESi[y] > 0) { // if the channel is above the pedestal
            //IdentificationSi-CsI
            energytree->InitTelescope(FragDetSi[y] - 1, FragDetCsI[y] - 1);
            energytree->InitSiCsI(FragDetSi[y]);
            energytree->SetCalibration(Si, CsI, FragDetSi[y] - 1, FragDetCsI[y] - 1);
            id = new KVIdentificationResult();
            id_sitof = new KVIdentificationResult();
            char scope_name [256];
            char scope_name_sitof [256];
            sprintf(scope_name, "null");
            sprintf(scope_name_sitof, "null");
            if (energytree->kvid == NULL) {
               if (debug)cout << "See you!!" << endl << flush;
               Code_Ident_Vamos[y] = 31;  //ID failed because no Si-CsI grid
               if (debug)cout << "Code 31 : " << Code_Ident_Vamos[y] << endl;
            } else {
               sprintf(scope_name, "%s", energytree->kvid->GetName());
               if (debug)cout << "Si-CsI name : " << energytree->kvid->GetName() << endl;
               grd = 0;
               if ((grd = (KVIDGraph*) grid_list->FindObjectByName(scope_name)) != 0) {
                  if (energytree->kvid->IsIdentifiable(double(FragSignalCsI[y]), double(FragESi[y]))) {
                     Code_Ident_Vamos[y] = 1;
                     if (debug)cout << "Code1 : " << Code_Ident_Vamos[y] << endl;
                     energytree->kvid->Identify(double(FragSignalCsI[y]), double(FragESi[y]), id);         //energytree->kvid : KVIDGraph  Identify(x,y)
                     A_PID[y] = id->A;
                     Z_PID[y] = id->Z;
                     Z_corr[y] = int(Z_PID[y]);
                     PID[y] = id->PID;
                     energytree->SetFragmentZ(Z_corr[y]);
                     energytree->GetResidualEnergyCsI(double(FragSignalSi[y]), double(FragSignalCsI[y]));        //Method called for guessing A value by bissection method and getting CsI energy
                     if (energytree->good_bisection == kFALSE) Code_Ident_Vamos[y] = 37;
                     FragECsI[y] = energytree->RetrieveEnergyCsI();
                     FragEGap[y] = energytree->eEnergyGap;

                     AA[y] = energytree->RetrieveA();
                     identified = kTRUE;

                     energytree->InitTelescopeSiTof(FragDetSi[y] - 1);
                     if (energytree->kvid_sitof == NULL) {
                        Code_Ident_Vamos[y] = 38;
                     } else {
                        sprintf(scope_name_sitof, "%s", energytree->kvid_sitof->GetName());
                        if ((grd = (KVIDGraph*) grid_list->FindObjectByName(scope_name_sitof)) != 0) {
                           if (energytree->kvid_sitof->IsIdentifiable(double(FragTfinal[y]), double(FragESi[y]))) {
                              energytree->kvid_sitof->Identify(double(FragTfinal[y]), double(FragESi[y]), id_sitof);
                              PID_sitof[y] = id_sitof->PID;
                              Z_PID_sitof[y] = id_sitof->Z;
                              Z_corr_sitof[y] = int(Z_PID_sitof[y]);
                              if ((id_sitof->PID >= (PID[y] - 0.5)) && (id_sitof->PID <= (PID[y] + 0.5))) {
                                 Code_Ident_Vamos[y] = 1;
                              } else {
                                 Code_Ident_Vamos[y] = 39;
                              }
                           } else {
                              Code_Ident_Vamos[y] = 40;
                           }
                        }
                     }

                  }  //Is Identifiable
                  else {
                     Code_Ident_Vamos[y] = 36;
                  }

               } else {
                  if (debug)cout << "**************************************************************************************************************************" << endl;
               }
               delete id;
               delete id_sitof;
            }
         } //Energy Si
         else {
            Code_Ident_Vamos[y] = 34;
            if (debug)cout << "Code 34 : " << Code_Ident_Vamos[y] << endl;
         }
         /*if(identified == kFALSE){
            Code_Ident_Vamos[y]=3;
            cout<<"Code Ident : "<<Code_Ident_Vamos[y]<<endl;
         }*/
      }//CsI Multiplicity

      for (Int_t y = 0; y < CsI->EMCSI ; y++) {
         if (Code_Ident_Vamos[y] < 0) {
            if (debug)cout << "**************Code_Ident_Vamos=-10 OUT**************" << endl;

         }
      }

   }// Code good event

//==========================================================================================
// Offset for Si-CsI events (Code_Ident_Vamos=1).
// We observe a shift of 40ns that we can't understand...Shift them back!

   D = (1 / TMath::Cos(Dr->Pf / 1000.)) * (Rec->Path + (((Rec->DSI - Dr->FocalPos) / 10.) / TMath::Cos(Dr->Tf / 1000.))); //Distance : 9423mm(si layer position) - 8702.5mm(focal plane position) = 720.50 mm

   if ((Code_good_event == -10  && Si->EMSI == 1) || (Code_good_event == -8)) {
      for (Int_t y = 0; y < MaxM ; y++) {
         if (Code_Ident_Vamos[y] == 1) {
            if (FragTfinal[y] < Tof0_code1[runNumber]) {
               FragTfinal[y] = FragTfinal[y] + Deltat_code1[runNumber];
               Code_Ident_Vamos[y] = 40;
            }
         }
      }
   }


// Reconstruct Tof for Chio-si events adding the RF period
// Frequency for the two beams : 8.5500 MHz

   if ((Code_good_event == -10  && Si->EMSI == 1) || (Code_good_event == -8)) {
      for (Int_t y = 0; y < MaxM ; y++) {
         if (Code_Ident_Vamos[y] == 2) {
            if (energytree->kvid_cutscode2 == NULL) {
               cout << "kvid_cutscode2 NULL" << endl;
            } else {
               if ((runNumber > 510 && runNumber < 515) || runNumber == 424 || runNumber == 420 || runNumber == 423 || runNumber == 360 || runNumber == 516 || runNumber == 517 ||
                     (runNumber > 501 && runNumber < 507)) { //Runs with a cut line, nothing to reject

                  if (energytree->kvid_cutscode2->IsIdentifiable(FragTfinal[y], FragESi[y])) {
                     if (runNumber == 360) {
                        FragTfinal[y] = FragTfinal[y] + (2.*(10.0E+09 / (8.5500 * 10.0E06)));
                        Code_Ident_Vamos[y] = 22;
                        if (Z_corr[y] < 10)  Code_Ident_Vamos[y] = 41;
                     } else {
                        FragTfinal[y] = FragTfinal[y] + (10.0E+09 / (8.5500 * 10.0E06));
                        Code_Ident_Vamos[y] = 22;
                        if (runNumber == 424 || runNumber == 420 || runNumber == 423 || runNumber == 360 || runNumber == 516 || runNumber == 517) {
                           if (Z_corr[y] < 10)  Code_Ident_Vamos[y] = 41;
                        } else if ((runNumber > 501 && runNumber < 505)) {
                           if (Z_corr[y] < 7)   Code_Ident_Vamos[y] = 41;
                        } else if ((runNumber > 504 && runNumber < 507)) {
                           if (Z_corr[y] < 9)   Code_Ident_Vamos[y] = 41;
                        }
                     }
                  } else {
                     if (runNumber == 360)   FragTfinal[y] = FragTfinal[y] + (10.0E+09 / (8.5500 * 10.0E06));
                     else  FragTfinal[y] = FragTfinal[y];
                  }
               }  //runs with a cut line, nothing to reject
               else { //runs where we reject something, we don't understand...
                  if (energytree->kvid_cutscode2->IsIdentifiable(FragTfinal[y], FragESi[y])) {
                     if (runNumber > 516 && runNumber < 520 && FragTfinal[y] > Tof0[runNumber]) {
                        //FragTfinal[y] = FragTfinal[y]-(10.0E+09/(8.5500*10.0E06));
                        FragTfinal[y] = FragTfinal[y];
                        Code_Ident_Vamos[y] = 22;
                     }  //for those 2 runs, we have to substract the HF period, check done on Total Energy-ToF spectras
                     else {
                        if (FragTfinal[y] < Tof0[runNumber]) {
                           FragTfinal[y] = FragTfinal[y] + (10.0E+09 / (8.5500 * 10.0E06));
                           Code_Ident_Vamos[y] = 22;
                        }
                     }
                  }  //fragment identifiable, good fragments!
                  else {
                     FragTfinal[y] = FragTfinal[y] + Deltat_code1[runNumber];
                     Code_Ident_Vamos[y] = 40;  //Rejected, Si-ToF we don't understand
                  }  // if not identifiable, fragment inside the contour
               }  //runs where we reject something
            }  //grid not NULL


            //Comparaison de l'identification en Z pour l'identification Chio-Si (Code_Ident_Vamos=2)
            id_chiov2 = new KVIdentificationResult();
            char scope_name_chiov2 [256];
            sprintf(scope_name_chiov2, "null");

            energytree->InitChioV2(FragDetChio[y]);
            if (energytree->kvid_chiov2 == NULL) {
               Code_Ident_Vamos[y] = 38;
            } else {
               sprintf(scope_name_chiov2, "%s", energytree->kvid_chiov2->GetName());
               if ((grd = (KVIDGraph*) grid_list->FindObjectByName(scope_name_chiov2)) != 0) {
                  /*cout<<"DetChio : "<<FragDetChio[y]<<endl;
                  cout<<"Echio : "<<FragEChio[y]<<endl;
                  cout<<"V2 : "<<double((D/FragTfinal[y])*(D/FragTfinal[y]))<<endl;*/

                  if (energytree->kvid_chiov2->IsIdentifiable(double((D / FragTfinal[y]) * (D / FragTfinal[y])), double(FragEChio[y]))) {
                     energytree->kvid_chiov2->Identify(double((D / FragTfinal[y]) * (D / FragTfinal[y])), double(FragEChio[y]), id_chiov2);

                     PID_chiov2[y] = (id_chiov2->PID);
                     Z_PID_chiov2[y] = (id_chiov2->Z);
                     Z_corr_chiov2[y] = int(Z_PID_chiov2[y]);
                     //cout<<"PID chiov2 : "<<PID_chiov2[y]<<endl;
                     if ((PID_chiov2[y] >= (PID[y] - 0.5)) && (PID_chiov2[y] <= (PID[y] + 0.5))) {
                        //cout<<"diff : "<<id_chiov2->PID+1.-PID[y]<<endl;
                        Code_Ident_Vamos[y] = 2;
                     } else {
                        Code_Ident_Vamos[y] = 39;
                     }
                  } else {
                     Code_Ident_Vamos[y] = 40;
                  }
               }
            }
            delete   id_chiov2;

         }  //code_ident_vamos==2
      }  //for MaxM
   }  //code_good_event

//==========================================================================================
// V, A/Q, A and Q definitions for both cases
   Double_t AMU = 931.494061;
   Double_t m_elec = 0.510998928;
   Double_t cte_tesla = (1.660538921E-27 * 299792458) / 1.602176565E-19;

   if ((Code_good_event == -10  && Si->EMSI == 1) || (Code_good_event == -8)) {

      Double_t x_aq = 0.0;
      Double_t Gamma = 0.0;

      for (Int_t y = 0; y < MaxM ; y++) {
         if (FragDetSi[y] < 0)   continue;

         Code_Vamos[y] = -10;
         if ((Code_Ident_Vamos[y] == 1 || Code_Ident_Vamos[y] == 2 || Code_Ident_Vamos[y] == 22) &&  Rec->Brho >= Brho_min[runNumber] && Rec->Brho <= Brho_max[runNumber]) {
            Code_Vamos[y] = 1;
         } else {
            Code_Vamos[y] = 0;
         }


         V[y] = D / FragTfinal[y];  //Velocity given in cm/ns
         //cout<<"V : "<<V[y]<<endl;
         Vx[y] = V[y] * sin(Rec->ThetaL) * cos((Rec->PhiL) + (270.*(TMath::Pi() / 180.)));
         Vy[y] = V[y] * sin(Rec->ThetaL) * sin((Rec->PhiL) + (270.*(TMath::Pi() / 180.)));
         Vz[y] = V[y] * cos(Rec->ThetaL);

         Beta[y] = V[y] / 29.9792458;
         x_aq = TMath::Sqrt((1 - (Beta[y] * Beta[y]))) / Beta[y];
         M_Q[y] = ((1. / AMU) * ((Rec->Brho * (AMU / cte_tesla) * x_aq) + m_elec)) - (P0_aq_straight[FragDetSi[y]] + (P1_aq_straight[FragDetSi[y]] * PID[y]) + (P2_aq_straight[FragDetSi[y]] * TMath::Power(PID[y], 2)));

         if (Code_Ident_Vamos[y] == 1) {
            if ((M_Q[y] > Fit1[Z_corr[y]][0][runNumber] && M_Q[y] < Fit1[Z_corr[y]][1][runNumber]) || (M_Q[y] > Fit2[Z_corr[y]][0][runNumber] && M_Q[y] < Fit2[Z_corr[y]][1][runNumber]) ||
                  (M_Q[y] > Fit3[Z_corr[y]][0][runNumber] && M_Q[y] < Fit3[Z_corr[y]][1][runNumber]) || (M_Q[y] > Fit4[Z_corr[y]][0][runNumber] && M_Q[y] < Fit4[Z_corr[y]][1][runNumber]) ||
                  (M_Q[y] > Fit5[Z_corr[y]][0][runNumber] && M_Q[y] < Fit5[Z_corr[y]][1][runNumber]) || (M_Q[y] > Fit6[Z_corr[y]][0][runNumber] && M_Q[y] < Fit6[Z_corr[y]][1][runNumber]) ||
                  (M_Q[y] > Fit7[Z_corr[y]][0][runNumber] && M_Q[y] < Fit7[Z_corr[y]][1][runNumber])) {

               FragTfinal[y] = FragTfinal[y] + 1.235;
               V[y] = D / FragTfinal[y];  //Velocity given in cm/ns
               Vx[y] = V[y] * sin(Rec->ThetaL) * cos((Rec->PhiL) + (270.*(TMath::Pi() / 180.)));
               Vy[y] = V[y] * sin(Rec->ThetaL) * sin((Rec->PhiL) + (270.*(TMath::Pi() / 180.)));
               Vz[y] = V[y] * cos(Rec->ThetaL);

               Beta[y] = V[y] / 29.9792458;
               x_aq = TMath::Sqrt((1 - (Beta[y] * Beta[y]))) / Beta[y];
               M_Q[y] = ((1. / AMU) * ((Rec->Brho * (AMU / cte_tesla) * x_aq) + m_elec)) - (P0_aq_straight[FragDetSi[y]] + (P1_aq_straight[FragDetSi[y]] * PID[y]) + (P2_aq_straight[FragDetSi[y]] * TMath::Power(PID[y], 2)));
               Code_Ident_Vamos[y] = 50;
            }
         } else if (Code_Ident_Vamos[y] == 2 || Code_Ident_Vamos[y] == 22) {
            FragTfinal[y] = FragTfinal[y] + Offset_relativiste_chiosi[runNumber];
            V[y] = D / FragTfinal[y];  //Velocity given in cm/ns
            Vx[y] = V[y] * sin(Rec->ThetaL) * cos((Rec->PhiL) + (270.*(TMath::Pi() / 180.)));
            Vy[y] = V[y] * sin(Rec->ThetaL) * sin((Rec->PhiL) + (270.*(TMath::Pi() / 180.)));
            Vz[y] = V[y] * cos(Rec->ThetaL);

            Beta[y] = V[y] / 29.9792458;
            x_aq = TMath::Sqrt((1 - (Beta[y] * Beta[y]))) / Beta[y];
            M_Q[y] = ((1. / AMU) * ((Rec->Brho * (AMU / cte_tesla) * x_aq) + m_elec)) - (P0_aq_straight_chiosi[FragDetChio[y]] + (P1_aq_straight_chiosi[FragDetChio[y]] * PID[y]) + (P2_aq_straight_chiosi[FragDetChio[y]] * TMath::Power(PID[y], 2)));

         }

         //cout<<"Code : "<<Code_good_event<<endl;
         //cout<<"PID : "<<PID[y]<<endl;
         //cout<<"DetSi : "<<FragDetSi[y]<<" "<<P0_aq_straight[FragDetSi[y]]<<"  "<<P1_aq_straight[FragDetSi[y]]<<"  "<<P2_aq_straight[FragDetSi[y]]<<endl;
         //cout<<"Brho : "<<Rec->Brho<<" V : "<<V[y]<<" D : "<<D<<" T : "<<FragTfinal[y]<<" AQ : "<<  M_Q[y]<<endl;
         E_csi = GetEnergyLossCsI(FragDetSi[y], Z_corr[y], M_Q[y], FragESi[y]);
         E_gap2 = GetEnergyLossGap2(FragDetSi[y], Z_corr[y], M_Q[y], FragESi[y]); //gap Si-CsI
         E_gap1 = GetEnergyLossGap1(FragDetSi[y], Z_corr[y], M_Q[y], FragESi[y]); //gap Chio-Si
         E_chio = GetEnergyLossChio(Z_corr[y], M_Q[y]);
         E_dc2 = GetEnergyLossDC2(Z_corr[y], M_Q[y]);
         E_sed = GetEnergyLossSed(Z_corr[y], M_Q[y]);
         E_dc1 = GetEnergyLossDC1(Z_corr[y], M_Q[y]);
         E_tgt = GetEnergyLossTarget(Z_corr[y], M_Q[y]);

         if (Code_Ident_Vamos[y] == 1 || Code_Ident_Vamos[y] == 38 || Code_Ident_Vamos[y] == 39 || Code_Ident_Vamos[y] == 40) { //Identification Si-CsI
            if (FragEChio[y] <= 0)  FragEChio[y] = E_chio;
            if (FragEGap[y] <= 0)   FragEGap[y] = 0.0;
            if (FragECsI[y] <= 0)   FragECsI[y] = 0.0;

            Etot[y] = float(FragEChio[y] + FragESi[y] + FragEGap[y] + FragECsI[y]);
            //cout<<"EChio : "<<FragEChio[y]<<" ESi : "<<FragESi[y]<<"  Egap : "<<FragEGap[y]<<"   ECsI : "<<FragECsI[y]<<endl;
         } else if (Code_Ident_Vamos[y] == 2 || Code_Ident_Vamos[y] == 22) { //Identification Chio-Si
            Etot[y] = FragEChio[y] + FragESi[y];
         }

         Etot[y] += float(E_tgt + E_dc1 + E_dc2 + E_sed + E_gap1);
         //cout<<"Etot : "<<Etot[y]<<endl;

         Gamma = 1. / TMath::Sqrt(1. - TMath::Power(Beta[y], 2.));
         M[y] = Etot[y] / (AMU * (Gamma - 1));
         Q[y] = M[y] / M_Q[y];

         if (((Code_Ident_Vamos[y] == 2) && Code_good_event == -10 && Si->EMSI == 1)) {
            //DrawDeDx(Z_VAMOS[y], int(AQ[y]*Z_VAMOS[y]), FragEChio[y]);
            //cout<<"Z : "<<Z_corr[y]<<" A : "<<M_Q[y]*Z_corr[y]<<" DeltaE Chio : "<<FragEChio[y]<<endl;

            Off_chiosi[y] = DrawDeDx(Z_corr[y], int(M_Q[y] * Z_corr[y]), 1, FragESi[y] + E_gap1 + FragEChio[y], E_gap1 + FragESi[y]);
            //cout<<"offset chio-si : "<<Off_chiosi[y]<<endl;

            Off_dc1dc2[y] = DrawDeDx(Z_corr[y], int(M_Q[y] * Z_corr[y]), 0, FragESi[y] + E_gap1 + FragEChio[y] + E_dc1 + E_dc2 + E_sed, FragESi[y] + E_gap1 + FragEChio[y]);
            //cout<<"offset dc1-dc2 : "<<Off_dc1dc2[y]<<endl;
            //cout<<"================="<<endl;

            TOF_chiosi[y] = FragTfinal[y] - Off_chiosi[y] - Off_dc1dc2[y] ;

            if (TOF_chiosi[y] > 0) {
               Beta_chiosi[y] = (D / TOF_chiosi[y]) / 29.9792458;
               x_aq = TMath::Sqrt((1 - (Beta_chiosi[y] * Beta_chiosi[y]))) / Beta_chiosi[y];
               //M_Q_chiosi[y] = (1./AMU)*( (Rec->Brho*(AMU/cte_tesla)*x_aq)+m_elec);
               M_Q_chiosi[y] = (1. / AMU) * ((Rec->Brho * (AMU / cte_tesla) * x_aq) + m_elec) - (P0_aq_straight[FragDetSi[y]] + (P1_aq_straight[FragDetSi[y]] * PID[y]) + (P2_aq_straight[FragDetSi[y]] * TMath::Power(PID[y], 2)));
               Gamma = 1. / TMath::Sqrt(1. - TMath::Power(Beta_chiosi[y], 2.));
               M_chiosi[y] = Etot[y] / (AMU * (Gamma - 1));
               Q_chiosi[y] = M_chiosi[y] / M_Q_chiosi[y];
            }
         }

         if (Code_Ident_Vamos[y] == 1 || Code_Ident_Vamos[y] == 40 || Code_Ident_Vamos[y] == 50) {
            //Redressement des distributions en Q
            id_qaq = new KVIdentificationResult();
            char scope_name_qaq [256];
            sprintf(scope_name_qaq, "null");

            energytree->InitQStraight(FragDetCsI[y]);
            if (energytree->kvid_qaq == NULL) {
               RealQ_straight[y] = Q[y];
               Q_straight[y] = int(TMath::Floor(Q[y] + 0.5));
               M_straight[y] = Q_straight[y] * M_Q[y];
               Code_Ident_Vamos[y] = 45;
            } else {
               sprintf(scope_name_qaq, "%s", energytree->kvid_qaq->GetName());
               if ((grd = (KVIDGraph*) grid_list->FindObjectByName(scope_name_qaq)) != 0) {

                  if (energytree->kvid_qaq->IsIdentifiable(double(M_Q[y]), double(Q[y]))) {
                     energytree->kvid_qaq->Identify(double(M_Q[y]), double(Q[y]), id_qaq);

                     //cout<<"Redressement"<<endl;
                     RealQ_straight[y] = (id_qaq->PID);
                     Q_straight[y] = int(id_qaq->Z);
                     M_straight[y] = Q_straight[y] * M_Q[y];

                  } else {
                     Code_Ident_Vamos[y] = 46;
                  }
               }
            }
            delete   id_qaq;

         }//Code_Ident_Vamos[y]=1
         else if (Code_Ident_Vamos[y] == 2 || Code_Ident_Vamos[y] == 22) {
            //Redressement des distributions en Q
            id_qaq_chiosi = new KVIdentificationResult();
            char scope_name_qaq_chiosi [256];
            sprintf(scope_name_qaq_chiosi, "null");

            energytree->InitQStraight_chiosi(FragDetChio[y]);
            if (energytree->kvid_qaq_chiosi == NULL) {
               RealQ_straight[y] = Q[y];
               Q_straight[y] = int(TMath::Floor(Q[y] + 0.5));
               M_straight[y] = Q_straight[y] * M_Q[y];
               Code_Ident_Vamos[y] = 55;
            } else {
               sprintf(scope_name_qaq_chiosi, "%s", energytree->kvid_qaq_chiosi->GetName());
               if ((grd = (KVIDGraph*) grid_list->FindObjectByName(scope_name_qaq_chiosi)) != 0) {

                  if (energytree->kvid_qaq_chiosi->IsIdentifiable(double(M_Q[y]), double(Q[y]))) {
                     energytree->kvid_qaq_chiosi->Identify(double(M_Q[y]), double(Q[y]), id_qaq_chiosi);

                     //cout<<"Redressement"<<endl;
                     RealQ_straight[y] = (id_qaq_chiosi->PID);
                     Q_straight[y] = int(id_qaq_chiosi->Z);
                     M_straight[y] = Q_straight[y] * M_Q[y];

                  } else {
                     Code_Ident_Vamos[y] = 56;
                  }
               }
            }
            delete   id_qaq_chiosi;
         }//Code_Ident_Vamos[y]=2


      }//Loop MaxM
   }

   NormVamos = gIndraDB->GetRun(gIndra->GetCurrentRunNumber())->Get("NormVamos");
//Correction du temps mort : 1.0 / (1.0-DT(%)) (/ 6.21295660000000000e+07 pour que correction soit de l'ordre de l'unité)
   DT = 1.0 / (1.0 - (gIndraDB->GetRun(gIndra->GetCurrentRunNumber())->Get("DT") / 100.0));
   FC_Indra = gIndraDB->GetRun(gIndra->GetCurrentRunNumber())->GetScaler("INDRA");
   stat_tot = 0.;
   stat_tot = Stat_Indra[gIndra->GetCurrentRunNumber()];

}


void Identificationv::Treat(void)
{
#ifdef DEBUG
   cout << "Identificationv::Treat" << endl;
#endif

   //Counter[0]++;
   //if(Rec->Present) Counter[1]++;     //Non commenter

   //cout<<"******************"<<endl;
   //cout<<"Event Number : "<<event_number<<endl;
   Init();
   FragPropertiesInOrder();
   Calculate();

   event_number++;
   //#ifdef DEBUG
   //Show();
   //#endif

}
void Identificationv::inAttach(TTree* inT)
{
   UNUSED(inT);

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

void Identificationv::outAttach(TTree* outT)
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
   outT->Branch("DT",&DT,"DT/D");
   outT->Branch("Brho_mag",&Brho_mag,"Brho_mag/D");
   outT->Branch("FC_Indra",&FC_Indra,"FC_Indra/D");

   outT->Branch("E_corr",&E_corr,"E_corr/F");

   outT->Branch("Gamma",&Gamma,"Gamma/F");*/

   //outT->Branch("ESiRaw",&SiRaw,"SiRaw/I");
   //outT->Branch("ECsIRaw",&CsIRaw,"CsIRaw/I");
   //outT->Branch("ESi",&ESi,"ESi/D");

   outT->Branch("event_number", &event_number, "event_number/L");
   outT->Branch("MaxM", &MaxM, "MaxM/I");

   outT->Branch("FragDetChio", FragDetChio, "FragDetChio[MaxM]/I");
   outT->Branch("FragSignalChio", FragSignalChio, "FragSignalChio[MaxM]/I");
   outT->Branch("FragEChio", FragEChio, "FragEChio[MaxM]/F");

   outT->Branch("FragDetSi", FragDetSi, "FragDetSi[MaxM]/I");
   outT->Branch("FragSignalSi", FragSignalSi, "FragSignalSi[MaxM]/I");
   outT->Branch("FragESi", FragESi, "FragESi[MaxM]/F");
   outT->Branch("FragTfinal", FragTfinal, "FragTfinal[MaxM]/D");

   outT->Branch("FragDetCsI", FragDetCsI, "FragDetCsI[MaxM]/I");
   outT->Branch("FragSignalCsI", FragSignalCsI, "FragSignalCsI[MaxM]/I");
   outT->Branch("FragECsI", FragECsI, "FragECsI[MaxM]/D");
   outT->Branch("Code_good_event", &Code_good_event, "Code_good_event/I");
   outT->Branch("fMultiplicity_case", &fMultiplicity_case, "fMultiplicity_case/I");

   outT->Branch("Brho_0", &Brho_mag, "Brho_mag/D");
   outT->Branch("DTcorr", &DT, "DT/D");
   outT->Branch("Stat_Indra", &stat_tot, "Stat_Indra/F");
   outT->Branch("NormVamos", &NormVamos, "NormVamos/D");
   outT->Branch("Code_Vamos", Code_Vamos, "Code_Vamos[MaxM]/I");
   outT->Branch("Code_Ident_Vamos", Code_Ident_Vamos, "Code_Ident_Vamos[MaxM]/I");

   outT->Branch("E_VAMOS", Etot, "Etot[MaxM]/F");
   //outT->Branch("E",&E_VAMOS,"E_VAMOS/F");
   //outT->Branch("T",&T,"T/F");

   outT->Branch("D", &D, "D/F");
   outT->Branch("V_VAMOS", V, "V[MaxM]/F");
   outT->Branch("Vx_VAMOS", Vx, "Vx[MaxM]/F");
   outT->Branch("Vy_VAMOS", Vy, "Vy[MaxM]/F");
   outT->Branch("Vz_VAMOS", Vz, "Vz[MaxM]/F");
   outT->Branch("Beta", Beta, "Beta[MaxM]/F");

   //outT->Branch("AQ_VAMOS", &M_Qcorr, "M_Qcorr/F");

   outT->Branch("RealZ_VAMOS", PID, "PID[MaxM]/D");
   outT->Branch("Z_VAMOS", Z_corr, "Z_corr[MaxM]/I");
   outT->Branch("RealZ_VAMOS_sitof", PID_sitof, "PID_sitof[MaxM]/D");
   outT->Branch("Z_VAMOS_sitof", Z_corr_sitof, "Z_corr_sitof[MaxM]/I");
   outT->Branch("RealZ_VAMOS_chiov2", PID_chiov2, "PID_chiov2[MaxM]/D");
   outT->Branch("Z_VAMOS_chiov2", Z_corr_chiov2, "Z_corr_chiov2[MaxM]/I");

   outT->Branch("A_VAMOS", M_straight, "M_straight[MaxM]/F");
   outT->Branch("Q_VAMOS", Q_straight, "Q_straight[MaxM]/I");
   outT->Branch("RealQ_VAMOS", RealQ_straight, "RealQ_straight[MaxM]/F");

   outT->Branch("AQ", M_Q, "M_Q[MaxM]/F");
   outT->Branch("Q", Q, "Q[MaxM]/F");
   outT->Branch("A", M, "M[MaxM]/F");

   outT->Branch("AQ_chiosi", M_Q_chiosi, "M_Q_chiosi[MaxM]/F");
   outT->Branch("Q_chiosi", Q_chiosi, "Q_chiosi[MaxM]/F");
   outT->Branch("A_chiosi", M_chiosi, "M_chiosi[MaxM]/F");
   outT->Branch("Off_chiosi", Off_chiosi, "Off_chiosi[MaxM]/D");
   outT->Branch("Off_dc1dc2", Off_dc1dc2, "Off_dc1dc2[MaxM]/D");


   outT->Branch("Abisec", AA, "AA[MaxM]/F");

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
//=========================================================================
Int_t* Identificationv::GetListOfCsICrossFromSi(Int_t sinum)
{


   return geom_cross[sinum];
}


void Identificationv::GeometrySiCsICross()
{
   Int_t  num;
   Int_t csi1 = 0, csi2 = 0, csi3 = 0, csi4 = 0;
   ifstream in;
   TString sline;

   if (!gDataSet->OpenDataSetFile("geom_sicsi_biggercsi.dat", in)) {
      cout << "Could not open the calibration file geom_sicsi_biggercsi.dat !!!" << endl;
      return;
   } else {
      while (!in.eof()) {
         sline.ReadLine(in);
         if (!in.eof()) {
            if (!sline.BeginsWith("#")) {
               sscanf(sline.Data(), "%d %d %d %d %d", &num, &csi1, &csi2, &csi3, &csi4);
               geom_cross[num][0] = csi1;
               geom_cross[num][1] = csi2;
               geom_cross[num][2] = csi3;
               geom_cross[num][3] = csi4;

            }
         }
      }
   }
   in.close();

   return;
}

Int_t Identificationv::IsCsISiCrossTelescope(Int_t sinum, Int_t csinum)
{

   for (int i = 0; i < 4; i++) {
      if (geom_cross[sinum][i] == csinum)return 1;
   }
   return 0;

}

//=========================================================================
Int_t* Identificationv::GetListOfChioFromSi(Int_t sinum)
{


   return geomchiosi[sinum];
}

//method to reconstruct VAMOS telescopes
void Identificationv::GeometryChioSi()
{
   Int_t  num;
   Int_t chio1 = 0, chio2 = 0;
   ifstream in;
   TString sline;

   if (!gDataSet->OpenDataSetFile("geom_chiosi.dat", in)) {
      cout << "Could not open the calibration file geom_chiosi.dat !!!" << endl;
      return;
   } else {
      while (!in.eof()) {
         sline.ReadLine(in);
         if (!in.eof()) {
            if (!sline.BeginsWith("#")) {
               sscanf(sline.Data(), "%d %d %d", &num, &chio1, &chio2);
               geomchiosi[num][0] = chio1;
               geomchiosi[num][1] = chio2;
            }
         }
      }
   }
   in.close();

   return;

}

Int_t Identificationv::IsChioSiTelescope(Int_t cinum, Int_t sinum)
{

   for (int i = 0; i < 2; i++) {
      if (geomchiosi[sinum][i] == cinum)return 1;
   }
   return 0;

}

//=========================================================================
Int_t* Identificationv::GetListOfChioFromCsI(Int_t csinum)
{

   return geomchiocsi[csinum];
}

void Identificationv::GeometryChioCsI()
{
   Int_t  num;
   Int_t chio1 = 0, chio2 = 0;
   ifstream in;
   TString sline;

   if (!gDataSet->OpenDataSetFile("geom_chiocsi.dat", in)) {
      cout << "Could not open the calibration file geom_chiocsi.dat !!!" << endl;
      return;
   } else {
      while (!in.eof()) {
         sline.ReadLine(in);
         if (!in.eof()) {
            if (!sline.BeginsWith("#")) {
               sscanf(sline.Data(), "%d %d %d", &num, &chio1, &chio2);
               geomchiocsi[num][0] = chio1;
               geomchiocsi[num][1] = chio2;
            }
         }
      }
   }
   in.close();


   return;
}

Int_t Identificationv::IsChioCsITelescope(Int_t chionum, Int_t csinum)
{

   for (int i = 0; i < 2; i++) {
      if (geomchiocsi[csinum][i] == chionum)return 1;
   }
   return 0;
}

//=========================================================================
Int_t* Identificationv::GetListOfCsIFromSi(Int_t sinum)
{


   return geom[sinum];
}

void Identificationv::Geometry()
{
   Int_t  num;
   Int_t csi1 = 0, csi2 = 0, csi3 = 0, csi4 = 0, csi5 = 0, csi6 = 0;
   ifstream in;
   TString sline;

   if (!gDataSet->OpenDataSetFile("geom.dat", in)) {
      cout << "Could not open the calibration file geom.dat !!!" << endl;
      return;
   } else {
      while (!in.eof()) {
         sline.ReadLine(in);
         if (!in.eof()) {
            if (!sline.BeginsWith("#")) {
               sscanf(sline.Data(), "%d %d %d %d %d %d %d", &num, &csi1, &csi2, &csi3, &csi4, &csi5, &csi6);
               geom[num][0] = csi1;
               geom[num][1] = csi2;
               geom[num][2] = csi3;
               geom[num][3] = csi4;
               geom[num][4] = csi5;
               geom[num][5] = csi6;
               /*for(Int_t i=0; i<6; i++){
                cout<<"geom Si-CsI : "<<geom[num][i]<<" num : "<<num<<endl;
               }*/

            }
         }
      }
   }
   in.close();

   return;
}

Int_t Identificationv::IsSiCsITelescope(Int_t sinum, Int_t csinum)
{
//cout<<"IsSiCsITelescope : "<<sinum<<" "<<csinum<<endl;

   for (int i = 0; i < 6; i++) {
      //cout<<"geom : "<<geom[sinum][i]<<endl;
      if (geom[sinum][i] == csinum)return 1;

   }
   return 0;

}


void Identificationv::ReadQCorrection()
{
//==========================================================================
// Correction de l'état de charge en fonction du détecteur CsI
// Charge state correction according to the CsI detector

   Int_t csi = 0;
   Float_t p0 = 0.;
   Float_t p1 = 0.;
   Float_t p2 = 0.;
   Float_t p3 = 0.;

   ifstream file;
   TString sline;

   if (!gDataSet->OpenDataSetFile("q_function.dat", file)) {
      L->Log << "Could not open the calibration file q_function !!!" << endl;
      return;
   } else {
      L->Log << "Reading q_function" << endl;
      while (file.good()) {         //reading the file
         sline.ReadLine(file);
         if (!file.eof()) {          //fin du fichier
            if (sline.Sizeof() > 1 && !sline.BeginsWith("#")) {
               sscanf(sline.Data(), "%u %f %f %f %f",
                      &csi, &p0, &p1, &p2, &p3);
               P0[csi] = p0;
               P1[csi] = p1;
               P2[csi] = p2;
               P3[csi] = p3;
               L->Log << p0 << "	" << p1 << "	" << p2 << "	" << p3 << endl;
            }
         }
      }
   }
   file.close();

   return;
}

void Identificationv::ReadAQCorrection()
{
//==========================================================================

   Int_t run1 = 0;
   Int_t run2 = 0;
   Float_t brho = 0.;
   Float_t chi2 = 0.;
   Float_t p0mq = 0.;
   Float_t p1mq = 0.;

//==========================================================================
// Correction du rapport M/Q en fonction du Brho (ou RunNumber...)
// M/Q correction according to the Brho value (or the RunNumber...)
// Identification Si-CsI

   ifstream file2;
   TString sline2;
   if (!gDataSet->OpenDataSetFile("mq_function.dat", file2)) {
      L->Log << "Could not open the calibration file q_function !!!" << endl;
      return;
   } else {
      L->Log << "Reading mq_function" << endl;
      while (file2.good()) {         //reading the file
         sline2.ReadLine(file2);
         if (!file2.eof()) {          //fin du fichier
            if (sline2.Sizeof() > 1 && !sline2.BeginsWith("#")) {
               sscanf(sline2.Data(), "%f %u %u %f %f %f",
                      &brho, &run1, &run2, &p0mq, &p1mq, &chi2);
               for (Int_t i = run1; i < run2 + 1; i++) {
                  P0_mq[i] = p0mq;
                  P1_mq[i] = p1mq;
                  //L->Log<<p0mq<<" "<<p1mq<<endl;
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
   if (!gDataSet->OpenDataSetFile("mq_function_chiosi.dat", file22)) {
      L->Log << "Could not open the calibration file mq_function_chiosi !!!" << endl;
      return;
   } else {
      L->Log << "Reading mq_function_chiosi" << endl;
      while (file22.good()) {         //reading the file
         sline22.ReadLine(file22);
         if (!file22.eof()) {          //fin du fichier
            if (sline22.Sizeof() > 1 && !sline22.BeginsWith("#")) {
               sscanf(sline22.Data(), "%f %u %u %f %f %f",
                      &brho, &run1, &run2, &p0mq, &p1mq, &chi2);
               for (Int_t i = run1; i < run2 + 1; i++) {
                  P0_mq_chiosi[i] = p0mq;
                  P1_mq_chiosi[i] = p1mq;
                  //L->Log<<p0mq<<" "<<p1mq<<endl;
               }
            }
         }
      }
   }

   file22.close();
//==========================================================================

   return;
}

void Identificationv::ReadACorrection()
{
//==========================================================================
// Correction de la masse en fonction du Brho (ou RunNumber...) et de l'état de charge entier Q
// Mass correction according to the Brho value et charge state value Q (which is an interger in that case...)
// Identification Si-CsI
   ifstream file3;
   TString sline3;

   Int_t q;
   Float_t brho;
   Int_t run1;
   Int_t run2;
   Float_t p0m;
   Float_t p1m;
   Float_t chi2;

   if (!gDataSet->OpenDataSetFile("m_function.dat", file3)) {
      L->Log << "Could not open the calibration file m_function !!!" << endl;
      return;
   } else {
      L->Log << "Reading m_function" << endl;
      while (file3.good()) {         //reading the file
         sline3.ReadLine(file3);
         if (!file3.eof()) {          //fin du fichier
            if (sline3.Sizeof() > 1 && !sline3.BeginsWith("#")) {
               sscanf(sline3.Data(), "%f %u %u %u %f %f %f",
                      &brho, &run1, &run2, &q, &p0m, &p1m, &chi2);
               for (Int_t i = run1; i < run2 + 1; i++) {
                  P0_m[i][q] = p0m;
                  P1_m[i][q] = p1m;
                  //L->Log<<p0m<<"  "<<p1m<<endl;
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

   if (!gDataSet->OpenDataSetFile("m_function_chiosi.dat", file33)) {
      L->Log << "Could not open the calibration file m_function_chiosi !!!" << endl;
      return;
   } else {
      L->Log << "Reading m_function_chiosi" << endl;
      while (file33.good()) {         //reading the file
         sline33.ReadLine(file33);
         if (!file33.eof()) {          //fin du fichier
            if (sline33.Sizeof() > 1 && !sline33.BeginsWith("#")) {
               sscanf(sline33.Data(), "%f %u %u %u %f %f %f",
                      &brho, &run1, &run2, &q, &p0m, &p1m, &chi2);
               for (Int_t i = run1; i < run2 + 1; i++) {
                  P0_m_chiosi[i][q] = p0m;
                  P1_m_chiosi[i][q] = p1m;
                  //L->Log<<p0m<<"  "<<p1m<<endl;
               }
            }
         }
      }
   }
   file33.close();
//==========================================================================
   return;
}

void Identificationv::ReadFlagVamos()
{

//==========================================================================
// Tag the event according to the fragment identification (Z,A,Q),
// Brho0 (or RunNumber) and system

   //Int_t z;
   //Int_t a;
   //Int_t q;
   Float_t brho0;
   Int_t run1;
   Int_t run2;
   Float_t brhomin;
   Float_t brhomax;

   ifstream file4;
   TString sline4;

   if (!gDataSet->OpenDataSetFile("Flag_CodeVamos.dat", file4)) {
      L->Log << "Could not open the calibration file Flag_CodeVamos !!!" << endl;
      return;
   } else {
      //L->Log<< "Reading Flag_CodeVamos" <<endl;
      cout << "Reading Flag_CodeVamos" << endl;
      while (file4.good()) {         //reading the file
         sline4.ReadLine(file4);
         if (!file4.eof()) {          //fin du fichier
            if (sline4.Sizeof() > 1 && !sline4.BeginsWith("#")) {
               sscanf(sline4.Data(), "%f %u %u %f %f",
                      &brho0, &run1, &run2, &brhomin, &brhomax);
               for (Int_t i = run1; i < run2 + 1; i++) {
                  Brho_min[i] = brhomin;
                  Brho_max[i] = brhomax;
               }
               if (run1 == run2) {
                  Brho_min[run1] = brhomin;
                  Brho_max[run2] = brhomax;
               }
               if (TMath::Abs(run2 - run1) == 1) {
                  Brho_min[run1] = brhomin;
                  Brho_max[run1] = brhomax;
                  Brho_min[run2] = brhomin;
                  Brho_max[run2] = brhomax;
               }

            }
         }
      }
   }
   //cout<<"Done Reading Flag_CodeVamos.dat"<<endl;
   // Brho_min[z][a][q][i] = brhomin;
   // Brho_max[z][a][q][i] = brhomax;
   file4.close();
//==========================================================================

   return;
}

void Identificationv::ReadDoublingCorrection()
{
//==========================================================================
   Int_t z;
   Int_t system, run1, run2;
   Float_t min1, max1;
   Float_t min2, max2;
   Float_t min3, max3;
   Float_t min4, max4;
   Float_t min5, max5;
   Float_t min6, max6;
   Float_t min7, max7;

   ifstream file5;
   TString sline5;

   cout << "Reading doublepeak_mins.dat" << endl;

   if (!gDataSet->OpenDataSetFile("doublepeak_mins.dat", file5)) {
      L->Log << "Could not open the calibration file doublepeak_mins.dat !!!" << endl;
      return;
   } else {
      while (file5.good()) {         //reading the file
         sline5.ReadLine(file5);
         if (!file5.eof()) {          //fin du fichier
            if (sline5.Sizeof() > 1 && !sline5.BeginsWith("#")) {
               sscanf(sline5.Data(), "%u %u %u %u %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                      &system, &z, &run1, &run2, &min1, &max1, &min2, &max2, &min3, &max3, &min4, &max4, &min5, &max5, &min6, &max6, &min7, &max7);

               for (Int_t i = run1; i < run2 + 1; i++) {
                  Fit1[z][0][i] = min1;
                  Fit1[z][1][i] = max1;

                  Fit2[z][0][i] = min2;
                  Fit2[z][1][i] = max2;

                  Fit3[z][0][i] = min3;
                  Fit3[z][1][i] = max3;

                  Fit4[z][0][i] = min4;
                  Fit4[z][1][i] = max4;

                  Fit5[z][0][i] = min5;
                  Fit5[z][1][i] = max5;

                  Fit6[z][0][i] = min6;
                  Fit6[z][1][i] = max6;

                  Fit7[z][0][i] = min7;
                  Fit7[z][1][i] = max7;
               }
            }
         }
      }
   }
   file5.close();
   cout << "End Reading doublepeak_mins.dat" << endl;
//==========================================================================

   return;
}


void Identificationv::ReadToFCorrectionChioSi()
{
//==========================================================================
// Correction du temps de vol pour les events Chio-Si (Code_Ident_Vamos==2)
   Float_t brho0;
   Int_t run1;
   Int_t run2;
   Int_t t0;
   Int_t esi0;

   ifstream file5;
   TString sline5;

   if (!gDataSet->OpenDataSetFile("Tof_corr.dat", file5)) {
      L->Log << "Could not open the calibration file Tof_corr.dat !!!" << endl;
      return;
   } else {
      while (file5.good()) {         //reading the file
         sline5.ReadLine(file5);
         if (!file5.eof()) {          //fin du fichier
            if (sline5.Sizeof() > 1 && !sline5.BeginsWith("#")) {
               sscanf(sline5.Data(), "%f %u %u %u %u",
                      &brho0, &run1, &run2, &t0, &esi0);
               for (Int_t i = run1; i < run2 + 1; i++) {
                  Brho0[i] = brho0;
                  Tof0[i] = t0;
                  Esi0[i] = esi0;
               }
            }
         }
      }
   }
   /*for(Int_t i=0; i<MaxRun; i++){
   cout<<"i : "<<i<<" Brho0 : "<<Brho0[i]<<endl;
   }*/

   file5.close();
//==========================================================================
   return;
}

void Identificationv::ReadToFCorrectionCode1()
{

//==========================================================================
// Correction du temps de vol pour les events Si-CsI (Code_Ident_Vamos=1)
   Int_t run1;
   Int_t run2;
   Int_t system;
   Float_t t0, biny1, biny2, toflow, err_toflow, tofgood, err_tofgood, deltat;

   ifstream file5;
   TString sline5;

   if (!gDataSet->OpenDataSetFile("Tof_corr_code1.dat", file5)) {
      L->Log << "Could not open the calibration file Tof_corr_code1.dat !!!" << endl;
      return;
   } else {
      while (file5.good()) {         //reading the file
         sline5.ReadLine(file5);
         if (!file5.eof()) {          //fin du fichier
            if (sline5.Sizeof() > 1 && !sline5.BeginsWith("#")) {
               sscanf(sline5.Data(), "%d %d %d %f %f %f %f %f %f %f %f",
                      &run1, &run2, &system, &t0, &biny1, &biny2, &toflow, &err_toflow, &tofgood, &err_tofgood, &deltat);
               for (Int_t i = run1; i < run2 + 1; i++) {
                  Tof0_code1[i] = t0;
                  Deltat_code1[i] = deltat;

               }
            }
         }
      }
   }
   /*for(Int_t i=0; i<MaxRun; i++){
   cout<<"i : "<<i<<" Brho0 : "<<Brho0[i]<<endl;
   }*/

   file5.close();
//==========================================================================


}

void Identificationv::ReadStatIndra()
{
//==========================================================================
// Calculate the total statistic for each nominal Brho.
   Float_t brho0;
   Int_t run1;
   Int_t run2;

//Float_t brho0;
   ifstream file6;
   TString sline6;

   stat_indra = -10.0;

   if (!gDataSet->OpenDataSetFile("Stat_Indra.dat", file6)) {
      L->Log << "Could not open the calibration file  Stat_Indra!!!" << endl;
      return;
   } else {
      while (file6.good()) {         //reading the file
         sline6.ReadLine(file6);
         if (!file6.eof()) {          //fin du fichier
            if (sline6.Sizeof() > 1 && !sline6.BeginsWith("#")) {
               sscanf(sline6.Data(), "%f %d %d %f", &brho0, &run1, &run2, &stat_indra);
               //cout<<"stat indra : "<<stat_indra<<endl;
               for (Int_t i = run1; i < run2 + 1; i++) {
                  Stat_Indra[i] = stat_indra;
               }
               if (run1 == run2) {
                  Stat_Indra[run1] = stat_indra;
               }
               if (TMath::Abs(run2 - run1) == 1) {
                  Stat_Indra[run1] = stat_indra;
                  Stat_Indra[run2] = stat_indra;
               }

            }
         }
      }
   }
   file6.close();
//==========================================================================
   return;
}

void Identificationv::ReadOffsetsChiosi()
{

   TString sline3;
   ifstream in3;
   Float_t brho0, aq, sig_aq, off_relativiste;
   Int_t run1, run2;


   //ToF Offset for each run
   if (!gDataSet->OpenDataSetFile("Offset_relativiste_chiosi.dat", in3)) {
      cout << "Could not open the calibration file Offset_relativiste.cal !!!" << endl;
      return;
   } else {
      cout << "Reading Offset_relativiste.dat" << endl;
      L->Log << "Reading Offset_relativiste.dat" << endl;
      while (!in3.eof()) {
         sline3.ReadLine(in3);
         if (!in3.eof()) {
            if (!sline3.BeginsWith("+") && !sline3.BeginsWith("#")) {
               sscanf(sline3.Data(), "%f %d %d %f %f %f", &brho0, &run1, &run2, &off_relativiste, &aq, &sig_aq);
               for (Int_t i = run1; i < run2 + 1; i++) {
                  Offset_relativiste_chiosi[i] = off_relativiste;
                  //cout<<"offset relativiste : "<<i<<"  "<<Offset_relativiste[i]<<endl;
               }
            }
         }
      }
   }
   in3.close();


   return;
}

void Identificationv::ReadStraightAQ()
{

//==========================================================================
   ifstream file8;
   TString sline8;
   ifstream file80;
   TString sline80;

   Int_t si;
   Float_t p0, err_p0;
   Float_t p1, err_p1;
   Float_t p2, err_p2;

   si = 0;
   p0 = err_p0 = 0.0;
   p1 = err_p1 = 0.0;
   p2 = err_p2 = 0.0;

   if (!gDataSet->OpenDataSetFile("straight_fct.dat", file8)) {
      L->Log << "Could not open the calibration file  straight_fct!!!" << endl;
      return;
   } else {
      while (file8.good()) {         //reading the file
         sline8.ReadLine(file8);
         if (!file8.eof()) {          //fin du fichier
            if (sline8.Sizeof() > 0 && !sline8.BeginsWith("#")) {
               sscanf(sline8.Data(), "%u %f %f %f %f %f %f", &si, &p0, &err_p0, &p1, &err_p1, &p2, &err_p2);
               P0_aq_straight[si] = p0;
               P1_aq_straight[si] = p1;
               P2_aq_straight[si] = p2;
            }
         }
      }
   }
   file8.close();

   if (!gDataSet->OpenDataSetFile("straight_fct_chiosi.dat", file80)) {
      L->Log << "Could not open the calibration file  straight_fct_chiosi!!!" << endl;
      return;
   } else {
      while (file80.good()) {         //reading the file
         sline80.ReadLine(file80);
         if (!file80.eof()) {          //fin du fichier
            if (sline80.Sizeof() > 0 && !sline80.BeginsWith("#")) {
               sscanf(sline80.Data(), "%u %f %f %f %f %f %f", &si, &p0, &err_p0, &p1, &err_p1, &p2, &err_p2);
               P0_aq_straight_chiosi[si] = p0;
               P1_aq_straight_chiosi[si] = p1;
               P2_aq_straight_chiosi[si] = p2;
            }
         }
      }
   }
   file80.close();
//==========================================================================


   return;
}

//===================================================
void Identificationv::SetTarget(KVTarget* tgt)
{
   ttgt = tgt;
}
void Identificationv::SetDC1(KVDetector* dcv1)
{
   ddcv1 = dcv1;
}
void Identificationv::SetSed(KVMaterial* sed)
{
   ssed = sed;
}
void Identificationv::SetDC2(KVDetector* dcv2)
{
   ddcv2 = dcv2;
}
void Identificationv::SetIC(KVDetector* ic)
{
   iic = ic;
}
void Identificationv::SetGap1(KVMaterial* isogap1)
{
   iisogap1 = isogap1;
}
void Identificationv::SetSi(KVMaterial* si)
{
   ssi = si;
}
void Identificationv::SetGap2(KVMaterial* isogap2)
{
   iisogap2 = isogap2;
}
void Identificationv::SetCsI(KVMaterial* csi)
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
Double_t Identificationv::GetEnergyLossCsI(Int_t number, Int_t Z, Double_t AQ, Double_t ESi)
{
   GetSi()->SetThickness(Si->si_thick[number]*KVUnits::um);

   //Calcul de l'énergie perdue dans csi
   einc_isogap2 = GetSi()->GetEResFromDeltaE(Z, int(Z * AQ), ESi);
   eloss_isogap2 = GetGap2()->GetDeltaE(Z, int(Z * AQ), einc_isogap2);

   einc_csi = GetGap2()->GetEResFromDeltaE(Z, int(Z * AQ), eloss_isogap2);
   eloss_csi = GetCsI()->GetDeltaE(Z, int(Z * AQ), einc_isogap2);
   GetSi()->Clear();

   return eloss_csi;
}

Double_t Identificationv::GetEnergyLossGap2(Int_t number, Int_t Z, Double_t AQ, Double_t ESi)
{
   GetSi()->SetThickness(Si->si_thick[number]*KVUnits::um);

   //Calcul de l'énergie perdue dans isogap2
   einc_isogap2 = GetSi()->GetEResFromDeltaE(Z, int(Z * AQ), ESi);
   eloss_isogap2 = GetGap2()->GetDeltaE(Z, int(Z * AQ), einc_isogap2);

   GetSi()->Clear();

   return eloss_isogap2;
}

Double_t Identificationv::GetEnergyLossGap1(Int_t number, Int_t Z, Double_t AQ, Double_t ESi)
{
   GetSi()->SetThickness(Si->si_thick[number]*KVUnits::um);
   einc_si = GetSi()->GetIncidentEnergy(Z, int(Z * AQ), ESi);

   //Calcul de l'énergie perdue dans isogap1
   einc_isogap1 = GetGap1()->GetIncidentEnergyFromERes(Z, int(Z * AQ), einc_si);
   eloss_isogap1 = GetGap1()->GetDeltaEFromERes(Z, int(Z * AQ), einc_si);

   GetSi()->Clear();

   return eloss_isogap1;
}

Double_t Identificationv::GetEnergyLossChio(Int_t Z, Double_t AQ)
{
   //Calcul de l'énergie perdue dans la chio
   einc_ic = GetIC()->GetIncidentEnergyFromERes(Z, int(Z * AQ), einc_isogap1);

   kvn->SetZ(Z);
   kvn->SetA(int(AQ * Z));
   kvn->SetEnergy(einc_ic);
   KVMaterial* kvm_ic = 0;

   for (Int_t i = 0; i < 2; i++) { //Take account only the first two layers, because the calibration gives the energy for the active layer, which is the last layer...
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
Double_t Identificationv::GetEnergyLossDC2(Int_t Z, Double_t AQ)
{
   //Calcul de l'énergie perdue dans la DC2
   einc_dc2 = GetDC2()->GetIncidentEnergyFromERes(Z, int(AQ * Z), einc_ic);

   kvn->SetZ(Z);
   kvn->SetA(int(AQ * Z));
   kvn->SetEnergy(einc_dc2);
   KVMaterial* kvm_dc2 = 0;

   for (Int_t i = 0; i < 3; i++) {
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

Double_t Identificationv::GetEnergyLossSed(Int_t Z, Double_t AQ)
{
   //Calcul de l'énergie perdue dans la SED
   TVector3 rot(0, 1, -1); //45 deg
   einc_sed = GetSed()->GetIncidentEnergyFromERes(Z, int(AQ * Z), einc_dc2);

   kvn->SetZ(Z);
   kvn->SetA(int(AQ * Z));
   kvn->SetEnergy(einc_sed);

   GetSed()->DetectParticle(kvn, &rot);
   eloss_sed = GetSed()->GetEnergyLoss();

   //einc_sed = GetSed()->GetIncidentEnergyFromERes(int(Z_PID),int(M_Q*Z_PID),einc_dc2);
   //eloss_sed = GetSed()->GetDeltaEFromERes(int(Z_PID),int(M_Q*Z_PID),einc_dc2);

   GetSed()->Clear();

   return eloss_sed;
}

Double_t Identificationv::GetEnergyLossDC1(Int_t Z, Double_t AQ)
{
   //Calcul de l'énergie perdue dans la DC1
   einc_dc1 = GetDC1()->GetIncidentEnergyFromERes(Z, int(AQ * Z), einc_sed);

   kvn->SetZ(int(Z));
   kvn->SetA(int(AQ * Z));
   kvn->SetEnergy(einc_dc1);
   KVMaterial* kvm_dc1 = 0;

   for (Int_t i = 0; i < 3; i++) {
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

Double_t Identificationv::GetEnergyLossTarget(Int_t Z, Double_t AQ)
{
   //Calcul de l'énergie perdue dans la cible
   einc_tgt = GetTarget()->GetIncidentEnergyFromERes(Z, int(AQ * Z), einc_dc1);
   eloss_tgt = GetTarget()->GetDeltaEFromERes(Z, int(AQ * Z), einc_dc1);

   GetTarget()->Clear();

   return einc_tgt - einc_dc1;
   //return eloss_tgt;
}

