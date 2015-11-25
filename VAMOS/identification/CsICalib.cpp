//Created by KVClassFactory on Thu Aug 20 16:03:37 2009
//Author: marini

#include "CsICalib.h"
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
#include "KVUnits.h"

#include <stdio.h>

#include <cstdlib>
#include <stdlib.h>
#include <new>
#include <string>
#include <string.h>

ClassImp(CsICalib)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>CsICalib</h2>
<h4>>Best estimations of the total incident energy (=>energy loss in CsI) and particle mass</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

CsICalib::CsICalib(LogFile* Log, Sive503* SiD)
{
   // Default constructor

   LightCsI = 0.;

   eEnergySi = 0.;
   eEnergyCsI = 0.;
   eEnergyGap = 0.;
   sEnergySi = 0.;
   sEnergyCsI = 0.;

   ePied = 0.;
   eZ = 0;
   eA = 0;

   esi1 = 0.;
   esi2 = 0.;
   ecsi1 = 0.;
   ecsi2 = 0.;

   diff1 = 0.;
   diff2 = 0.;
   diffsi = 0.;
   diffcsi = 0.;

   sA = 0;
   iA = 0.;
   right = 0;
   left = 0;
   sRefECsI = 0.;

   Einc = 0.;
   EEsi = 0.;

   L = Log;
   Si = SiD;

   gap = 0;
   si = 0;
   csi = 0;
   kvt_icsi = 0;
   kvt_sicsi = 0;
   lum = 0;

   list = 0;
   list = (KVList*) gIDGridManager->GetGrids();
   if (list == 0) {
      cout << "list = 0 in CsICalib" << endl;
      entries = 0;
   } else {
      cout << "list not null" << endl;
      entries = (Int_t) list->GetEntries();
      cout << "entries : " << entries << endl;
   }
}

CsICalib::~CsICalib()
{
   // Destructor
   //L->Log<<"***Destructor CsICalib***"<<endl;
}

void CsICalib::SetTel1(KVDetector* SI)
{
   ssi = SI;
}

KVDetector* CsICalib::GetTel1(void)
{
   return ssi;
}

void CsICalib::SetTel2(KVDetector* GAP)
{
   ggap = GAP;
}

KVDetector* CsICalib::GetTel2(void)
{
   return ggap;
}

void CsICalib::SetTel3(KVDetector* CSI)
{
   ccsi = CSI;
}

KVDetector* CsICalib::GetTel3(void)
{
   return ccsi;
}

void CsICalib::InitChioV2(Int_t ci_num)
{

   Char_t tel_name [128] = "null";
   sprintf(tel_name, "CI_V2_%02i", ci_num); //1-7

   kvid_chiov2 = 0;
   KVNumberList runList = 0;

   if (list != 0) {

      tmpGrid = 0;

      for (Int_t i = 0; i < entries; i++) {
         tmpGrid = (KVIDGrid*) list->At(i);
         if (tmpGrid != 0) {
            runList = (KVNumberList) tmpGrid->GetRuns();
            runList.Begin();
            while (!runList.End()) {
               UInt_t next_val = (UInt_t) runList.Next();
               if (next_val == gIndra->GetCurrentRunNumber()) {
                  //cout<<"tel name : "<<tmpGrid->GetName()<<endl;
                  if (strcmp(tmpGrid->GetName(), tel_name) == 0) {
                     kvid_chiov2 = tmpGrid;
                     break;
                  }
               }

            }
         }
      }


      if (kvid_chiov2 == 0) {

         printf("Error: 'kvid_chiov2' assignment failed\n");
         cout << "name : " << tel_name << endl;
      }


   }


}

void CsICalib::InitQStraight_chiosi(Int_t chionum)
{

   Char_t tel_name [128] = "null";
   sprintf(tel_name, "Q_AQ_CI%02d", chionum);

   kvid_qaq_chiosi = 0;
   KVNumberList runList = 0;

   if (list != 0) {

      tmpGrid = 0;

      for (Int_t i = 0; i < entries; i++) {
         tmpGrid = (KVIDGrid*) list->At(i);
         if (tmpGrid != 0) {
            runList = (KVNumberList) tmpGrid->GetRuns();
            runList.Begin();
            while (!runList.End()) {
               UInt_t next_val = (UInt_t) runList.Next();
               if (next_val == gIndra->GetCurrentRunNumber()) {
                  //cout<<"tel name : "<<tmpGrid->GetName()<<endl;
                  if (strcmp(tmpGrid->GetName(), tel_name) == 0) {
                     kvid_qaq_chiosi = tmpGrid;
                     break;
                  }
               }

            }
         }
      }


      if (kvid_qaq_chiosi == 0) {

         printf("Error: 'kvid_qaq_chiosi' assignment failed\n");
         cout << "name : " << tel_name << endl;
      }


   }


   return;
}

void CsICalib::InitQStraight(Int_t csinum)
{

   Char_t tel_name [128] = "null";
   sprintf(tel_name, "Q_AQ_CSI%02d", csinum);

   kvid_qaq = 0;
   KVNumberList runList = 0;

   if (list != 0) {

      tmpGrid = 0;

      for (Int_t i = 0; i < entries; i++) {
         tmpGrid = (KVIDGrid*) list->At(i);
         if (tmpGrid != 0) {
            runList = (KVNumberList) tmpGrid->GetRuns();
            runList.Begin();
            while (!runList.End()) {
               UInt_t next_val = (UInt_t) runList.Next();
               if (next_val == gIndra->GetCurrentRunNumber()) {
                  //cout<<"tel name : "<<tmpGrid->GetName()<<endl;
                  if (strcmp(tmpGrid->GetName(), tel_name) == 0) {
                     kvid_qaq = tmpGrid;
                     break;
                  }
               }

            }
         }
      }


      if (kvid_qaq == 0) {

         printf("Error: 'kvid_qaq' assignment failed\n");
         cout << "name : " << tel_name << endl;
      }


   }


   return;
}

void CsICalib::InitCode2Cuts(Float_t brho0)
{

   Char_t tel_name [128] = "null";
   sprintf(tel_name, "SIE_TOF_%04.0f", brho0 * 1000.);
   //sprintf(tel_name, "SIE_TOF_1401");

   kvid_cutscode2 = 0;
   KVNumberList runList = 0;

   if (list != 0) {

      tmpGrid = 0;

      for (Int_t i = 0; i < entries; i++) {
         tmpGrid = (KVIDGrid*) list->At(i);
         if (tmpGrid != 0) {
            runList = (KVNumberList) tmpGrid->GetRuns();
            runList.Begin();
            while (!runList.End()) {
               UInt_t next_val = (UInt_t) runList.Next();
               if (next_val == gIndra->GetCurrentRunNumber()) {
                  //cout<<"tel name : "<<tmpGrid->GetName()<<endl;
                  if (strcmp(tmpGrid->GetName(), tel_name) == 0) {
                     kvid_cutscode2 = tmpGrid;
                     break;
                  }
               }

            }
         }
      }


      if (kvid_cutscode2 == 0) {

         printf("Error: 'kvid_cutscode2' assignment failed\n");
         cout << "name : " << tel_name << endl;
      }


   }


}

void CsICalib::InitTelescopeSiTof(Int_t si_num)
{

   Char_t tel_name [128] = "null";
   sprintf(tel_name, "SIE_%02i_TOF", si_num + 1);

   kvid_sitof = 0;
   KVNumberList runList = 0;

   if (list != 0) {

      tmpGrid = 0;

      for (Int_t i = 0; i < entries; i++) {
         tmpGrid = (KVIDGrid*) list->At(i);
         if (tmpGrid != 0) {
            runList = (KVNumberList) tmpGrid->GetRuns();
            runList.Begin();
            while (!runList.End()) {
               UInt_t next_val = (UInt_t) runList.Next();
               if (next_val == gIndra->GetCurrentRunNumber()) {
                  if (strcmp(tmpGrid->GetName(), tel_name) == 0) {
                     kvid_sitof = tmpGrid;
                     break;
                  }
               }

            }
         }
      }


      if (kvid_sitof == 0) {

         //printf("Error: 'kvid_sitof' assignment failed\n");
         //cout<<"name : "<<tel_name<<endl;
      }
   }

   return;
}

void CsICalib::InitTelescopeChioSi(Int_t ci_num, Int_t si_num)
{

   Char_t si_name [128] = "null";
   Char_t ci_name [128] = "null";

   sprintf(si_name, "SIE_%02i", si_num + 1);
   sprintf(ci_name, "CI_%02i", ci_num);

   Char_t tel_name [128] = "null";
   sprintf(tel_name, "CI_%02i_SIE_%02i", ci_num, si_num + 1);

   kvid_chiosi = 0;
   KVNumberList runList = 0;

   //cout<<"Inside Init Telescope Chio-Si"<<endl;
   if (list != 0) {

      tmpGrid = 0;
      for (Int_t i = 0; i < entries; i++) {
         tmpGrid = (KVIDGrid*) list->At(i);
         if (tmpGrid != 0) {
            runList = (KVNumberList) tmpGrid->GetRuns();
            runList.Begin();

            while (!runList.End()) {
               UInt_t next_val = (UInt_t) runList.Next();
               if (next_val == gIndra->GetCurrentRunNumber()) {
                  if (strcmp(tmpGrid->GetName(), tel_name) == 0) {
                     kvid_chiosi = tmpGrid;

                  }
               }

            }
         } else {
            cout << "tmpGrid =0" << endl;
         }
      }


      if (kvid_chiosi != 0) {
         //cout<<"Found kvid_chiosi in list"<<endl;
         //cout<<"Name : "<<kvid_chiosi->GetName()<<endl;
      } else {
         //printf("Error: 'kvid_chiosi' assignment failed\n");
         //cout<<"ci_num : "<<ci_num<<" si_num : "<<si_num+1<<endl;
      }
   }

   return;
}

void CsICalib::InitTelescope(Int_t si_num , Int_t csi_num)
{

   Char_t si_name [128] = "null";
   Char_t csi_name [128] = "null";

   sprintf(si_name, "SIE_%02i", si_num + 1);
   sprintf(csi_name, "CSI%02i", csi_num + 1);

   Char_t tel_name [128] = "null";
   sprintf(tel_name, "SIE_%02i_CSI%02i", si_num + 1, csi_num + 1);

   kvid = 0;
   KVNumberList runList = 0;

   //list = (KVList*) gIDGridManager->GetGrids();
   //cout<<"Inside Init Telescope Si-CsI"<<endl;
   if (list != 0) {
      //cout<<"Found List"<<endl;

      tmpGrid = 0;

      for (Int_t i = 0; i < entries; i++) {
         tmpGrid = (KVIDGrid*) list->At(i);
         if (tmpGrid != 0) {
            runList = (KVNumberList) tmpGrid->GetRuns();
            runList.Begin();
            while (!runList.End()) {
               UInt_t next_val = (UInt_t) runList.Next();
               if (next_val == gIndra->GetCurrentRunNumber()) {
                  if (strcmp(tmpGrid->GetName(), tel_name) == 0) {
                     kvid = tmpGrid;
                     break;
                  }
               }

            }
         }
      }
      //cout<<"I'm out!!"<<endl<<flush;

      if (kvid != 0) {
         //cout<<"Found kvid in list"<<endl;
         //cout<<"Name : "<<kvid->GetName()<<endl;

      } else {
         //sprintf(tel_name, "null");
         /*printf("Error: 'kvid' assignment failed\n");
         cout<<"si_num : "<<si_num<<" csi_num : "<<csi_num<<endl;
         cout<<"name : "<<tel_name<<endl; */
      }
   } else {
      //printf("Error: 'list' assignment failed\n");
   }

}

void CsICalib::InitSiCsI(Int_t number) // Si-CsI Telescope  # Si : 1 to 18
{

   /********************************************************************************
    TELESCOPE LAYOUT: Using custom built classes IonisationChamber and PlaneAbsorber

    beam >>  | Silicon | C4H10 (Gap) | CsI

    ********************************************************************************/

   GetTel1()->SetThickness(Si->si_thick[number]*KVUnits::um);
   //L->Log<<"Thickness   : "<<GetTel1()->GetThickness()<<endl;
   kvt_sicsi = new KVTelescope();
   kvt_sicsi->Add(GetTel1());
   kvt_sicsi->Add(GetTel2());  // In-active so no 'detected' energy
   kvt_sicsi->Add(GetTel3());

   lum = new KVLightEnergyCsIVamos(GetTel3());
   //lum=new KVLightEnergyCsIVamos(kvd_csi);
}

void CsICalib::SetCalibration(Sive503* Si, CsIv* CsI, Int_t sinum, Int_t csinum)
{

   if (Si->EMSI > 0) {
      a = Si->ECoef[sinum][0];
      b = Si->ECoef[sinum][1];
      c = Si->ECoef[sinum][2];
      alpha = 1;
      //L->Log<<"parametri silicio a="<<a<<" b="<<b<<" c="<<c<<endl;
   } else {
      a = b = c = alpha = 0.0;
   }
   if (CsI->EMCSI > 0) {
      ePied = CsI->Ped[csinum];
      a1 = CsI->ECoef[csinum][0];
      a2 = CsI->ECoef[csinum][1];
      a3 = CsI->ECoef[csinum][2];

      lum = new KVLightEnergyCsIVamos(GetTel3());
      //lum=new KVLightEnergyCsIVamos(kvd_csi);

      lum->SetNumberParams(3);
      lum->SetParameters(a1, a2, a3);
      //L->Log<<"parametri cesio a1="<<a1<<" a2="<<a2<<" a3="<<a3<<endl;
      //L->Log<<"pedestal csi=     "<<ePied<<endl;

   } else {
      ePied = 0.0;

      lum = new KVLightEnergyCsIVamos(GetTel3());
      //lum=new KVLightEnergyCsIVamos(kvd_csi);

      lum->SetNumberParams(3);
      lum->SetParameters(0., 0., 0.);
   }
}

//fragment Z
void CsICalib::SetFragmentZ(Int_t Zin)
{
   eZ = Zin;
   lum->SetZ(eZ);
}

//set fragment A
void CsICalib::SetFragmentA(Int_t Ain)
{
   lum->SetA(Ain);
   sA = Ain;
   //L->Log<<"SetFragment A : "<<sA<<endl;
}

//Complete procedure to get the best estimate of the residual energy
Double_t CsICalib::GetResidualEnergyCsI(Double_t esi, Double_t chcsi)      //UShort_t chsi, UShort_t chcsi
{
   Int_t A;
   A = 2 * eZ;
   eEnergySi = esi;

   LightCsI = chcsi - ePied;
   //L->Log<<"LightCsI = "<<LightCsI<<endl;

   Bisection(A, double(LightCsI));   //Call the bisection method
   Interpolate();        //Interpolation of ECsI and A values
   if (sRefECsI <= 0) {
      cout << "Z : " << eZ << "	A : " << RetrieveA() << "	light : " << LightCsI << endl;
      cout << "sRefECsI : " << sRefECsI << endl;
      cout << "********************" << endl;
   }
   return sRefECsI;
}

//Si calibration function
void CsICalib::CalculateESi(Double_t chan)   //UShort_t chan
{
   Double_t fact = 1.;
   eEnergySi = fact * alpha * (a + chan * b + chan * chan * c);
   if (eEnergySi <= 0) cout << "eEnergySi : " << eEnergySi << endl;
}

//Complete procedure for to get the energy losses from simulation
void  CsICalib::CompleteSimulation()   //UShort_t chan
{

   sEnergySi = 0.0;
   sEnergyCsI = 0.0;
   eEnergyCsI = 0.0;
   eEnergyGap = 0.0;

   //L->Log<<"light before calculating : "<<LightCsI<<endl;
   eEnergyCsI = lum->Compute(LightCsI);          //Get the ECsI from the known (Z,A) and the calibration

   //cout<<"Invert : "<<lum->Invert(eEnergyCsI)<<endl;
   //L->Log<<"Invert : "<<lum->Invert(eEnergyCsI)<<endl;
   //L->Log<<"light csi="<<LightCsI<<" eEnergyCsI=MeV "<<eEnergyCsI<<endl;     //" eEnergyGap=MeV "<<eEnergyGap<<endl; //paola

   part.Clear();
   part.SetZ(eZ);
   part.SetA(sA);
   //Einc = eEnergySi+eEnergyGap+eEnergyCsI;           //taking into account the energy loss in gap

   Double_t einc_isogap2 = 0.0;
   einc_isogap2 = GetTel1()->GetEResFromDeltaE(eZ, sA, eEnergySi);
   eEnergyGap = GetTel2()->GetDeltaE(eZ, sA, einc_isogap2);
   Einc = eEnergySi + eEnergyGap + eEnergyCsI;

   part.SetEnergy(Einc);

   kvt_sicsi->DetectParticle(&part);       //Simulate the fragment through the telescope

   //L->Log<<"Total incident E. : "<<Einc <<endl;
   //L->Log<<"simualed energy Si : "<<GetTel1()->GetEnergy()<<endl;
   //L->Log<<"simualed energy gap : "<<GetTel2()->GetEnergy()<<endl;
   //L->Log<<"simualed energy CsI : "<<GetTel3()->GetEnergy()<<endl;
   //eEnergyGap = GetTel2()->GetEnergy();

   //CanalCsI=lum->Invert(double(eZ),double(sA),GetTel3()->GetEnergy());    //GetTel3()->GetEnergy()); kvd_csi->GetEnergy()
   //L->Log<<"Invert simulation: "<<CanalCsI<<endl;
   //difflum = (CanalCsI-LightCsI);
   //L->Log<<"difflum = "<<difflum<<endl; //paola

   sEnergySi = GetTel1()->GetEnergy();     //simulated ESi obtained from the Einc (ESi(calibration) + ECsI(Z,A known and calibration))
   //L->Log<<"GetESi:: sEnergySi= "<<sEnergySi<<endl;

   sEnergyCsI = GetTel3()->GetEnergy();       //simulated ESi obtained from the Einc (ESi(calibration) + ECsI(Z,A known and calibration))
   //L->Log<<"GetECsI:: sEnergyCsI= "<<sEnergyCsI<<endl;

   part.Clear();
   GetTel1()->Clear();
   GetTel2()->Clear();
   GetTel3()->Clear();
   //kvd_si->Clear();
   //gap->Clear();
   //kvd_csi->Clear();
   kvt_sicsi->Clear();
}

void CsICalib::Bisection(Int_t A, Double_t chan)   //UShort_t chan
{
   Int_t middle, it = 0;
   good_bisection = kTRUE;
   //L->Log<<"------------"<<endl;
   //L->Log<<"Valeur de A : "<<A<<endl;
   //L->Log<<"Energy Si Reference :  "<<eEnergySi<<endl;
   if (A - 5 > 0)
      left = A - 5; //left and right are integer!! A is a double
   else left = 1;
   right = A + 10;
   //L->Log<<"-----------------------------------BISECTION-------------------"<<endl;//paola

//new bisection method
//=== Test
   SetFragmentA(left);
   CompleteSimulation();
   Int_t factor = 1;
   Double_t difference = 0.;
   Double_t diffright = 0.;
   difference = eEnergySi - sEnergySi;
   if (difference > 0) {
      factor = -1;
   }
   difference *= factor;
   //cout<<"A left : "<<left<<" ESi measured : "<<eEnergySi<<" ESi calculated : "<<sEnergySi<<" difference : "<<difference<<" factor : "<<factor<<endl;

   SetFragmentA(right);
   CompleteSimulation();
   difference = eEnergySi - sEnergySi;
   if (difference > 0) good_bisection = kFALSE;
   difference *= factor;
   diffright = difference;
   //cout<<"A right : "<<right<<" ESi measured : "<<eEnergySi<<" ESi calculated : "<<sEnergySi<<" difference : "<<difference<<" factor : "<<factor<<endl;
   if (difference < 0) {
      //cout<<"Bisection error"<<endl;
      while (diffright < 0) {
         //cout<<"Searching for new left value"<<endl;

         if (left - 1 > 0) left = left - 1;
         else left = 1;
         //cout<<"left : "<<left<<endl;

         SetFragmentA(left);
         CompleteSimulation();
         difference = eEnergySi - sEnergySi;
         //cout<<"sEnergySi : "<<sEnergySi<<"   difference : "<<difference<<endl;
         if (difference > 0) {
            factor = -1;
         }
         diffright *= factor;
         //cout<<"diffright : "<<diffright<<endl;
         if (left == 1) {
            cout << "Bisection error : Lower value of A not found" << endl;
            good_bisection = kFALSE;

            break;
         }
      }

   }
// === End test

   //loop: bisection
   while (right - left > 1) {

      //L->Log<<"-------------------loop bisection------------"<<endl;
      //L->Log<<"LightCsI  : "<<LightCsI<<endl;
      //cout<<"LightCsI : "<<LightCsI<<endl;
      middle = (left + right + 1) / 2;

      //simulated energies stored for middle A
      SetFragmentA(middle);
      CompleteSimulation();
      esi2 = sEnergySi;
      ecsi2 = sEnergyCsI;

//new bisection method
//=== Test
      difference = factor * (eEnergySi - sEnergySi);
      //cout<<"difference : "<<difference<<endl;
      if (difference < 0) {
         left = middle;
      } else
         right = middle;
// === End test

      /*if(esi2>eEnergySi){
         right=middle;
      }
      else
      left=middle;*/

      //cout<<"A2 (middle) : "<<middle<<"   ESi2= "<<sEnergySi<<"   ECsI2=   "<<sEnergyCsI<<endl; //paola

      it++;
      //L->Log<<"----------------iteration= "<<it<<endl;
   }    //end of bisection loop
   //cout<<"end of bisection loop"<<endl;
   //cout<<"Aleft : "<<left<<" Aright : "<<right<<endl;

   //simulated energies for two values of A closest to the point of zero
   //difference between the simulated energy for Si and the one from calibration
   SetFragmentA(left);
   CompleteSimulation();
   esi1 = sEnergySi;
   ecsi1 = sEnergyCsI;
   //L->Log<<"esi1 : "<<esi1<<" ecsi1 : "<<ecsi1<<endl;
   //cout<<"esi1 : "<<esi1<<" ecsi1 : "<<ecsi1<<endl;

   SetFragmentA(right);
   CompleteSimulation();
   esi2 = sEnergySi;
   ecsi2 = sEnergyCsI;
   //L->Log<<"esi2 : "<<esi2<<" ecsi2 : "<<ecsi2<<endl;
   //cout<<"esi2 : "<<esi2<<" ecsi2 : "<<ecsi2<<endl;


}


Double_t CsICalib::BisectionLight(Double_t Z, Double_t A, Double_t ECsI)
{
   Double_t middle = 0.;
   Int_t it = 0;
   Double_t leftA, rightA = 0.;
   //L->Log<<"------------"<<endl;
   //L->Log<<"Valeur de A : "<<A<<endl;
   //L->Log<<"Light Reference :   "<<LightCsI<<endl;

   if (A - 0.5 > 0)
      leftA = A - 0.5; //left and right are integer!! A is a double
   else leftA = 0.;
   rightA = A + 0.5;

   //L->Log<<"-----------------------------------BISECTION-LIGHT------------------"<<endl;//paola

//new bisection method
//=== Test

   Double_t calcul_light = lum->Invert(Z, leftA, ECsI);

   Int_t factor = 1;
   Double_t difference = 0.;
   difference = LightCsI - calcul_light;
   if (difference > 0) {
      factor = -1;
   }
   difference *= factor;
   //L->Log<<"A left : "<<leftA<<" Measured Light : "<<LightCsI<<" Calculated  Light : "<<calcul_light<<" difference : "<<difference<<" factor : "<<factor<<endl;

   calcul_light = lum->Invert(Z, rightA, ECsI);
   difference = LightCsI - calcul_light;
   difference *= factor;
   //L->Log<<"A right : "<<rightA<<" Measured Light : "<<LightCsI<<" Calculated  Light : "<<calcul_light<<" difference : "<<difference<<" factor : "<<factor<<endl;

   if (difference < 0) {
      //L->Log<<"Bisection error"<<endl;
   }
// === End test

   //loop: bisection
   while (rightA - leftA > 0.1) {

      //L->Log<<"-------------------loop bisection------------"<<endl;
      //L->Log<<"LightCsI  : "<<LightCsI<<endl;
      //cout<<"LightCsI : "<<LightCsI<<endl;
      middle = (leftA + rightA) / 2;

      //simulated energies stored for middle A
      calcul_light = lum->Invert(Z, middle, ECsI);

//new bisection method
//=== Test
      difference = factor * (LightCsI - calcul_light);
      //L->Log<<"difference : "<<difference<<endl;
      if (difference < 0) {
         leftA = middle;
      } else
         rightA = middle;
// === End test

      /*if(esi2>eEnergySi){
         right=middle;
      }
      else
      left=middle;*/

      //L->Log<<"A2 (middle) :  "<<middle<<"   Measured Light =  "<<LightCsI<<" Calculated Light =   "<<calcul_light<<endl; //paola

      it++;
      //L->Log<<"----------------iteration= "<<it<<endl;
   }    //end of bisection loop

   //simulated energies for two values of A closest to the point of zero
   //difference between the simulated energy for Si and the one from calibration

   Double_t calcul_light_left = lum->Invert(Z, leftA, ECsI);
   Double_t calcul_light_right = lum->Invert(Z, rightA, ECsI);
   if (TMath::Abs(LightCsI - calcul_light_left) > TMath::Abs(LightCsI - calcul_light_right)) {
      return rightA;
   } else
      return leftA;

}


//interpolation usage for to find the right residual energy and right A, using
//the final interval returned by bisection
void CsICalib::Interpolate()
{
//L->Log<<"esi1 : "<<esi1<<" ecsi1 : "<<ecsi1<<endl;
//L->Log<<"esi2 : "<<esi2<<" ecsi2 : "<<ecsi2<<endl;
   sRefECsI = GetInterpolationD(esi1, ecsi1, esi2, ecsi2, eEnergySi);
   //L->Log<<"sRefECsI : "<<sRefECsI<<endl;
   iA = GetInterpolationD(esi1, (Double_t)left, esi2, (Double_t)right, eEnergySi);
   //L->Log<<"iA : "<<iA<<endl;

}

// Interpolation procedure: finding y corresponding to x, under assumption that
//these variables behave linearly within the given ranges x1-x2, and y1-y2.
Double_t CsICalib::GetInterpolationD(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t x)
{
   Double_t slope;
   Double_t y;
   slope = (y1 - y2) / (x1 - x2);
   y = slope * (x - x2) + y2;
   // cout<<"esi1="<<x1<<" ecsi1="<<y1<<" esi2="<<x2<<" ecsi2="<<y2<<" esi mis="<<x<<" ecsi interp="<<y; //paola
   //cout<<"  slope : "<<slope<<endl;

   return y;
}

//--------------------------Retrieve values------------------------//
//Getter for energy loss in silicon
Double_t CsICalib::RetrieveEnergySi()
{
   return eEnergySi;
}

//getter for the best estimate of A
Double_t CsICalib::RetrieveA()
{
   return iA;
}

//getter for light in CsI
Double_t CsICalib::RetrieveLight()
{
   return LightCsI;
}

Double_t CsICalib::RetrieveEnergyCsI()
{
   return sRefECsI;     //was eEnergyCsI
}
