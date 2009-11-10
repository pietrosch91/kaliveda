/***************************************************************************
                          kvindra5.cpp  -  description
                             -------------------
    begin                : Mon May 20 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Riostream.h"
#include "TROOT.h"
#include "KVINDRA5.h"
#include "KVTelescope.h"
#include "KVSilicon.h"
#include "KVChIo.h"
#include "KVCsI.h"

#define PG_GG_SI_FILE "Si_fit_PG_GG.dat"
#define PG_GG_CHIO_FILE "ChIo_fit_PG_GG.dat"

ClassImp(KVINDRA5);
////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRA5</h2>
<h4>INDRA multidetector array, configuration for 5th campaign experiments</h4>

<img src="images/indra_camp5.png"><br>

<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRA5::KVINDRA5()
{
}

//____________________________________________________________________________________

KVINDRA5::~KVINDRA5()
{
}

//____________________________________________________________________________________

void KVINDRA5::MakeListOfDetectorTypes()
{
   //Add Si150um detectors to standard INDRA list

   KVINDRA4::MakeListOfDetectorTypes();

	KVSilicon *kvsi = (KVSilicon* )KVDetector::MakeDetector(Form("%s.SI",gDataSet->GetName()), 150.0);
	//kvsi->SetThickness(150.0);
	kvsi->SetLabel("SI150");
	fDetectorTypes->Add(kvsi);
}

//________________________________________________________________________________________

void KVINDRA5::PrototypeTelescopes()
{
   //Make Si150-Csixxcm telescopes

   KVINDRA4::PrototypeTelescopes();
   // Si-CsI Ring 2
   KVTelescope *kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si150-CsI Ring2");
   kvt->SetAzimuthalWidth(27.51);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 3
   kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si150-CsI Ring3");
   kvt->SetAzimuthalWidth(14.15);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 4
   kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si150-CsI Ring4");
   kvt->SetAzimuthalWidth(14.01);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 5
   kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI138"));
   kvt->SetType("Si150-CsI Ring5");
   kvt->SetAzimuthalWidth(14.29);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 6
   kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI97"));
   kvt->SetType("Si150-CsI Ring6");
   kvt->SetAzimuthalWidth(14.29);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 7
   kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI97"));
   kvt->SetType("Si150-CsI Ring7");
   kvt->SetAzimuthalWidth(14.48);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 8
   kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI90"));
   kvt->SetType("Si150-CsI Ring8");
   kvt->SetAzimuthalWidth(14.18);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);

   // Si-CsI Ring 9
   kvt = new KVTelescope;
   kvt->AddDetector((KVSilicon *) fDetectorTypes->
                    FindObjectByLabel("SI150"));
   kvt->AddDetector((KVCsI *) fDetectorTypes->FindObjectByLabel("CSI90"));
   kvt->SetType("Si150-CsI Ring9");
   kvt->SetAzimuthalWidth(14.32);
	kvt->SetDepth(1, 0.0);
	kvt->SetDepth(2, 5.0);
   fTelescopes->Add(kvt);
}

//_________________________________________________________________________________________

void KVINDRA5::Build()
{
   //construct specific realisation of INDRA array for 5th campaign
   //start from INDRA4 (with 1st ring Si-Csi) and replaces some Si300 with Si150
  
   KVINDRA4::Build();
   SetName("INDRA");
   SetTitle("5th campaign INDRA detector");
   SetPGtoGGFactors("SI");
   SetPGtoGGFactors("CI");
}

//_______________________________________________________________________________

void KVINDRA5::SetPGtoGGFactors(Option_t * type)
{
   //Read files containing experimental PG to GG conversion factors and set these
   //factors for detectors of type "type"


   UInt_t det_type = ChIo_GG;
   ifstream datfile;

   if (!strcmp(type, "SI")) {
      if (!SearchAndOpenKVFile(PG_GG_SI_FILE, datfile, fDataSet.Data()))
         return;
      det_type = Si_GG;
   } else if (!strcmp(type, "CI")) {
      if (!SearchAndOpenKVFile(PG_GG_CHIO_FILE, datfile, fDataSet.Data()))
         return;
      det_type = ChIo_GG;
   } else {
      return;
   }

   if (!datfile.good()) {
      Error("SetPGtoGGFactors",
            "PG to GG conversion data not found for type %s", type);
      return;
   }

   KVString aline;
   int cou, mod;
   float alpha, beta;

   aline.ReadLine(datfile);
   while (datfile.good()) {

      if (!aline.BeginsWith('#')) {     //skip comments in header

         if (sscanf(aline.Data(), "%d %d %f %f", &cou, &mod, &alpha, &beta)
             != 4) {
            //erreur de lecture
            Error("SetPGtoGGFactors", "Problem reading line :\n%s",
                  aline.Data());
            datfile.close();
            return;
         }
         KVDetector *det =
             GetDetectorByType((UInt_t) cou, (UInt_t) mod, det_type);
         if (!det) {
            //no detector found with cou, mod and type
            Error("SetPGtoGGFactors",
                  "No detector found with cou=%d mod=%d and type=%d", cou,
                  mod, det_type);
            datfile.close();
            return;
         }
         if (det->InheritsFrom("KVSilicon")) {
            ((KVSilicon *) det)->SetPGtoGG(alpha, beta);
         } else if (det->InheritsFrom("KVChIo")) {
            ((KVChIo *) det)->SetPGtoGG(alpha, beta);
         }
      }
      aline.ReadLine(datfile);
   }                            //while( datfile.good()
   datfile.close();
}

//_________________________________________________________________________________________

void KVINDRA5::BuildGeometry()
{
   //place 150um Si-CsI telescopes in INDRA4 array

   KVINDRA4::BuildGeometry();
   // 2-21
   ReplaceTelescope("SI_CSI_0221",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring2"));
   ReplaceTelescope("SI_CSI_0321",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring3"));
   ReplaceTelescope("SI_CSI_0322",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring3"));
   // 4-9
   ReplaceTelescope("SI_CSI_0409",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring4"));
   ReplaceTelescope("SI_CSI_0410",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring4"));
   ReplaceTelescope("SI_CSI_0509",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring5"));
   ReplaceTelescope("SI_CSI_0510",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring5"));
   // 6-3
   ReplaceTelescope("SI_CSI_0603",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring6"));
   ReplaceTelescope("SI_CSI_0604",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring6"));
   ReplaceTelescope("SI_CSI_0703",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring7"));
   ReplaceTelescope("SI_CSI_0704",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring7"));
   // 6-13
   ReplaceTelescope("SI_CSI_0613",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring6"));
   ReplaceTelescope("SI_CSI_0614",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring6"));
   ReplaceTelescope("SI_CSI_0713",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring7"));
   ReplaceTelescope("SI_CSI_0714",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring7"));
   // 8-7
   ReplaceTelescope("SI_CSI_0807",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0808",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0907",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   ReplaceTelescope("SI_CSI_0908",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   // 8-17
   ReplaceTelescope("SI_CSI_0817",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0818",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0917",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   ReplaceTelescope("SI_CSI_0918",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   // 8-23
   ReplaceTelescope("SI_CSI_0823",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0824",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring8"));
   ReplaceTelescope("SI_CSI_0923",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
   ReplaceTelescope("SI_CSI_0924",
                    (KVTelescope *) fTelescopes->
                    FindObjectByType("Si150-CsI Ring9"));
}

//_______________________________________________________________________________________

// void KVINDRA5::SetCsIIDParams()
// {
//    //Read ID and Gamma-line files for CsI R-L identifications.
//    //Initialise grids in KVIDCsI telescopes
// 
//    Info("SetCsIIDParams", "Setting identification parameters");
// 
//    ifstream datfile;
// //   if (!SearchAndOpenKVFile(CSI_ID_PARAMS, datfile, fDataSet.Data()))
//  //     return;
// 
// /*    if (!datfile.good()) {
//       Error("SetCsiIDParams",
//             "Parameter file not found : %s", CSI_ID_PARAMS);
//       return;
//    }
//  */   KVString s;
// 
//    while (datfile.good()) {
// 
//       s.ReadLine(datfile);
//       if (!s.BeginsWith('#')) { //'#' sign signals comments
//          int ring, modu, frun, lrun, totpoints;
//          if (sscanf(s.Data(), "%d %d %d %d", &ring, &modu, &frun, &lrun) !=
//              4) {
// /*             Error("SetCsIIDParams", "Problem reading file %s\nRead: %s",
//                   CSI_ID_PARAMS, s.Data());
//  */            return;
//          };
//          //create new grid
//          KVIDGCsI *grid = new KVIDGCsI;
//          //add to ID Grid Manager
//          gIDGridManager->AddGrid(grid);
//          grid->GetParameters()->SetValue("First run", frun);
//          grid->GetParameters()->SetValue("Last run", lrun);
//          grid->GetParameters()->SetValue("Ring min", ring);
//          grid->GetParameters()->SetValue("Ring max", ring);
//          grid->GetParameters()->SetValue("Mod min", modu);
//          grid->GetParameters()->SetValue("Mod max", modu);
//          int nlines;
//          datfile >> nlines;
//          totpoints = 0;
//          //read Z,A and number of points in each line
//          for (int i = 0; i < nlines; i++) {
//             int z, a, npoints;
//             datfile >> z >> a >> npoints;
//             totpoints += 2 * npoints;
//             if (z > 0) {
//                //identification line
//                KVIDCsIRLLine *line =
//                    (KVIDCsIRLLine *) grid->NewLine("KVIDCsIRLLine");
//                line->SetZ(z);
//                line->SetA(a);
//                line->Set(npoints);
//                grid->AddLine("id", line);
//             } else {
//                //IMF line - always last (?)
//                KVIDLine *line = grid->NewLine();
//                line->SetName("IMF_line");
//                line->Set(npoints);
//                grid->AddLine("ok", line);
//             }
//          }
//          //read coordinates of lines in the order they were created.
//          //ID lines first, IMF_line is last.
//          TIter nextID(grid->GetLines("id"));
//          KVIDLine *line;
//          while ((line = (KVIDLine *) nextID())) {
//             //read in points
//             for (int i = 0; i < line->GetN(); i++) {
//                Double_t x, y;
//                datfile >> x >> y;
//                line->SetPoint(i, x, y);
//             }
//          }
//          //last of all, IMF_line
//          line = grid->GetLine("ok", "IMF_line");
//          //read in points
//          for (int i = 0; i < line->GetN(); i++) {
//             Double_t x, y;
//             datfile >> x >> y;
//             line->SetPoint(i, x, y);
//          }
//          //calculate line widths in grid
//          grid->CalculateLineWidths();
//       }
//    }
// 
//    //Now read gamma lines file & add lines to grids
// 
//    ifstream gamfile;
// //   if (!SearchAndOpenKVFile(CSI_GAMMA_PARAMS, gamfile, fDataSet.Data()))
//  //     return;
// 
// /*    if (!gamfile.good()) {
//       Error("SetCsiIDParams",
//             "Parameter file not found : %s", CSI_GAMMA_PARAMS);
//       return;
//    }
//  */   while (gamfile.good()) {
// 
//       s.ReadLine(gamfile);
//       if (!s.BeginsWith('#')) { //'#' sign signals comments
//          int ring, modu, frun, lrun;
//          if (sscanf(s.Data(), "%d %d %d %d", &ring, &modu, &frun, &lrun) !=
//              4) {
//             Error("ReadOrsayGamma", "Problem reading file");
//             return;
//          };
//          //get grid for this ring,mod
//          TString name;
//          name.
//              Form
//              ("CsI R-L Grid First run=%d Last run=%d Ring min=%d Ring max=%d Mod min=%d Mod max=%d",
//               frun, lrun, ring, ring, modu, modu);
//          KVIDGCsI *grid =
//              (KVIDGCsI *) gIDGridManager->GetGrid(name.Data());
//          if (!grid)
//             Error("SetCsIIDParams", "Can't find grid %s", name.Data());
//          int npoints;
//          gamfile >> npoints;
//          //found gamma line
//          KVIDLine *line = 0;
//          if (grid) {
//             line = grid->NewLine();
//             line->SetName("gamma_line");
//             line->Set(npoints);
//             grid->AddLine("ok", line);
//          }
//          //read in points
//          for (int i = 0; i < npoints; i++) {
//             Double_t x, y;
//             gamfile >> x >> y;
//             if (grid)
//                line->SetPoint(i, x, y);
//          }
//       }
//    }
// }
