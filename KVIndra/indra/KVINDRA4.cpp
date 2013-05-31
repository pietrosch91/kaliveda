/***************************************************************************
$Id: KVINDRA4.cpp,v 1.7 2008/12/17 13:01:26 franklan Exp $
                          KVINDRA4.cpp  -  description
                             -------------------
    begin                : 25th October 2003
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
#include <TF1.h>
#include <TGraph.h>
#include "KVINDRA4.h"
#include "KVTelescope.h"
#include "KVLayer.h"
#include "KVList.h"
#include "KVSilicon.h"
#include "KVChIo.h"
#include "KVCsI.h"
#include "KVNucleus.h"
#include "KVGroup.h"
#include "KVRing.h"

ClassImp(KVINDRA4);
////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRA4</h2>
<h4>INDRA multidetector array, configuration for 4th campaign experiments</h4>

<img src="images/indra_camp4_693x620.png"><br>

<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRA4::KVINDRA4()
{
}

//____________________________________________________________________________________
KVINDRA4::~KVINDRA4()
{
}

//____________________________________________________________________________________

void KVINDRA4::Build()
{
   //construct specific realisation of INDRA array for 4th campaign

   KVINDRA::Build();
   SetName("INDRA");
   SetTitle("4th campaign INDRA@GSI detector");
}

//_________________________________________________________________________________________

void KVINDRA4::BuildGeometry()
{
   //Take 1st campaign array, replace Ring 1 (phoswich) with Si-Csi detectors
   //as a guess, use same prototype telescopes as for Ring 2

   KVINDRA::BuildGeometry();
//   KVTelescope *proto_cour_1 =
//       (KVTelescope *) fTelescopes->FindObjectByType("Si-CsI Ring2");
//    KVRing* rg = ((KVINDRATelescope*)GetTelescope("PHOS_0101"))->GetRing();
//	rg->SetTitle("Si-CsI Ring1");
//	rg->SetPrototype(proto_cour_1);
//   ReplaceTelescope("PHOS_0101", proto_cour_1);
//   ReplaceTelescope("PHOS_0102", proto_cour_1);
//   ReplaceTelescope("PHOS_0103", proto_cour_1);
//   ReplaceTelescope("PHOS_0104", proto_cour_1);
//   ReplaceTelescope("PHOS_0105", proto_cour_1);
//   ReplaceTelescope("PHOS_0106", proto_cour_1);
//   ReplaceTelescope("PHOS_0107", proto_cour_1);
//   ReplaceTelescope("PHOS_0108", proto_cour_1);
//   ReplaceTelescope("PHOS_0109", proto_cour_1);
//   ReplaceTelescope("PHOS_0110", proto_cour_1);
//   ReplaceTelescope("PHOS_0111", proto_cour_1);
//   ReplaceTelescope("PHOS_0112", proto_cour_1);
//	rg->GetTelescopes()->Execute("SetType","\"Si-CsI Ring1\"");
	
}
