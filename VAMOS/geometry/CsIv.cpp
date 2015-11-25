//Created by KVClassFactory on Tue Aug 11 16:38:54 2009
//Author: marini
//2010-10-04 : Files CsI.cal and CsI.pied have been modified to take account the right distances between detectors

#include "CsIv.h"
#include <cmath>
#include "TRandom3.h"

ClassImp(CsIv)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>CsIv</h2>
<h4>CsI VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

CsIv::CsIv(LogFile* Log)
{

   L = Log;

   InitRaw();
   Init();
   // Default constructor


   //initialization of calibration parameter
   for (Int_t i = 0; i < 80; i++) {
      Ped[i][0] = -1.;
   }
   for (Int_t j = 0; j < 80; j++) {
      for (Int_t k = 0; k < 3; k++) {
         ECoef[j][k] = -1.;
      }
   }

   //pedestal reading
   Int_t num = 0;
   Float_t pied = 0.;
   Float_t dummy1, dummy2, dummy3;
   TString sline;
   ifstream in2;

   //reading the piedestal for CsI from P.Wigg work - 2010-10-04
   if (!gDataSet->OpenDataSetFile("CsI.pied", in2)) {
      cout << "Could not open the calibration file CsI.pied !!!" << endl;
      return;
   } else {
      cout << "Reading CsI.pied" << endl;
      while (!in2.eof()) {
         sline.ReadLine(in2);
         if (!in2.eof()) {
            if (!sline.BeginsWith("#")) {
               sscanf(sline.Data(), "%d %f %f %f %f", &num, &dummy1, &dummy2, &dummy3, &pied);
               Ped[num - 1][0] = pied;
            }
         }
      }
   }
   in2.close();

   //    cout<<"piedestal= "<<pied<<endl;


   Int_t  num2;
   Float_t a1 = 0., a2 = 0., a3 = 0.;
   ifstream in3;

   //reading the CsI calibration parameters from P.Wigg work - 2010-10-04
   if (!gDataSet->OpenDataSetFile("CsI.cal", in3)) {
      cout << "Could not open the calibration file CsI.cal !!!" << endl;
      return;
   } else {
      cout << "Reading CsI.cal" << endl;
      while (!in3.eof()) {
         sline.ReadLine(in3);
         if (!in3.eof()) {
            if (!sline.BeginsWith("+") && !sline.BeginsWith("|")) {
               sscanf(sline.Data(), "%d %d %f %f %f %f %f", &num, &num2, &a1, &a2, &a3, &dummy1, &dummy2);
               ECoef[num2 - 1][0] = a1;
               ECoef[num2 - 1][1] = a2;
               ECoef[num2 - 1][2] = a3;
            }
         }
      }
   }
   in3.close();

}

CsIv::~CsIv()
{
   // Destructor
}


void CsIv::InitRaw(void)
{
   //cout << "CsI::InitRaw" << endl;

   for (Int_t i = 0; i < 80; i++) {
      E_Raw[i] = 0;
      E_Raw_Nr[i] = 0;
      CsIRaw[i] = 0;
   }
   E_RawM = 0;
}


void CsIv::Init()
{
   //cout<<"Init()"<<endl;

   //Initialization of calibrated parameter
   for (Int_t i = 0; i < 80; i++) {
      //E[i]=-100;
      ENr[i] = -1;
   }
   EM = 0;
   ETotal = -1;
   Number = -10;
   DetCsI = 0;
}

void CsIv::Calibrate(void)
{
   // #ifdef DEBUG
   //cout << "CSI::Calibrate" << endl;
   // #endif

   //L->Log<<"E_RawM CsI"<<E_RawM<<endl;

   for (Int_t i = 0; i < E_RawM; i++) {
      //cout<<"CsI : E_Raw["<<i<<"] = "<<E_Raw[i]<<endl;

      //cout<<"E_Raw_Nr[i] "<<E_Raw_Nr[i]<<endl;
      if (float(E_Raw[i]) > Ped[int(E_Raw_Nr[i])][0]) {
         //cout<<"E_Raw "<<E_Raw[i]<<endl;

         //==Raw_signal class==
         CSI_Raw = E_Raw[i];
         //====================
         Number = E_Raw_Nr[i];
         DetCsI = int(Number) + 1; //numérotation 1-80
         EM++;
      }
   }


}

void CsIv::Treat(void)
{
   //#ifdef DEBUG
   //cout << "CsIv::Treat" << endl;
   //#endif

   Init();
   Calibrate();
}

void CsIv::inAttach(TTree* inT)
{

#ifdef DEBUG
   cout << "CsI::inAttach" << endl;
#endif

#ifdef DEBUG
   cout << "Attaching CsI variables" << endl;
#endif

   inT->SetBranchAddress("WCSIEM", &E_RawM);
   inT->SetBranchAddress("WCSIE", E_Raw);
   inT->SetBranchAddress("WCSIENr", E_Raw_Nr);

}

void CsIv::outAttach(TTree* outT)
{

#ifdef DEBUG
   cout << "CsI::inAttach" << endl;
#endif

#ifdef DEBUG
   cout << "Attaching CsI variables" << endl;
#endif

   outT->Branch("CsIEM", &EM, "CsIEM/I");
   outT->Branch("DetCsI", &DetCsI, "DetCsI/I");
   //outT->Branch("CsIE",E,"CsIE[80]/F");
   //outT->Branch("CsIET",&ETotal,"CsIET/F");

   //outT->Branch("CSIERaw",&E_Raw[0],"CsIERaw/S");
   //outT->Branch("CSIERaw",CsIRaw,"CsIERaw[80]/S");
}

void CsIv::CreateHistograms(void)
{

#ifdef DEBUG
   cout << "CsI::CreateHistograms" << endl;
#endif

}
void CsIv::FillHistograms(void)
{

#ifdef DEBUG
   cout << "CsI::FillHistograms" << endl;
#endif

}
