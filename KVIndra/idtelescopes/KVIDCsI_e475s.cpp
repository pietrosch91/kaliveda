/*
$Id: KVIDCsI_e475s.cpp,v 1.4 2008/12/02 16:43:30 ebonnet Exp $
$Revision: 1.4 $
$Date: 2008/12/02 16:43:30 $
*/

//Created by KVClassFactory on Fri Jul  4 16:36:34 2008
//Author: Eric Bonnet

#include "KVIDCsI_e475s.h"
#include "KVChIo_e475s.h"
#include "KVSilicon_e475s.h"
#include "KVString.h"
#include "KVINDRAReconNuc.h"

ClassImp(KVIDCsI_e475s)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIDCsI_e475s</h2>
<h4>derivation of KVIDCsI class for E475s experiment</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVIDCsI_e475s::KVIDCsI_e475s()
{
   // Default constructor
}

KVIDCsI_e475s::~KVIDCsI_e475s()
{
   // Destructor
}

void KVIDCsI_e475s::CalculateParticleEnergy(KVReconstructedNucleus * nuc)
{
	
	//status code
   fCalibStatus = kCalibStatus_NoCalibrations;

   if ( !(((KVINDRAReconNuc *) nuc)->GetCodes().GetIDCode()==kIDCode2) ) return;

   UInt_t z = nuc->GetZ();
   //uncharged particles
   if(z==0) { return; }
   //telescope csi rl -> un seul detecteur
   KVDetector* d1 = GetDetector(1);
	//pas d implementation pour les couronnes au dessus de 9
	if (GetDetector(1)->GetRingNumber()>9) { return; }

	Bool_t d1_cal = d1->IsCalibrated();
   if (d1_cal){
		//si calibre retour a la methode KVIDTelescope::CalculateParticleEnergy
		this->KVIDTelescope::CalculateParticleEnergy(nuc);
		return;
	}
   
	//no calibrations available for csi
   KVList* ll = nuc->GetIDTelescopes();
	// si il n y a ri1 avant le csi return pas de calibration possible
	if (ll->GetEntries()==1) {
		return;
	}
	
	KVString telname; telname.Form("*_SI_%02d%02d",GetDetector(1)->GetRingNumber(),GetDetector(1)->GetModuleNumber());
	//Recherche du telescope "CI_SI"
	KVIDTelescope* idbefore = (KVIDTelescope* )ll->FindObjectWithMethod(telname,"GetName");
	if (!idbefore) {
		//printf("recherhe d autre idtel a plante\n");
		return;
	}
	else if (!idbefore->GetDetector(1)->IsCalibrated() || !idbefore->GetDetector(2)->IsCalibrated()) {
		return;
	}
	else {}
		
	Double_t etot=0;
	//recuperation des energies chio (gaz+mylar)
	Double_t eci = ((KVChIo_e475s* )idbefore->GetDetector(1))->GetCalibratedEnergy(); 		
	if (eci<0) eci=0; idbefore->GetDetector(1)->SetEnergy(eci);	
	Double_t eci_corr = idbefore->GetDetector(1)->GetCorrectedEnergy(nuc->GetZ(),nuc->GetA());
	if (eci_corr<0) eci_corr=0;
	//ajout a l energie totale
	etot+=eci_corr;
	
	//recuperation des energies si et calcul de l energie residuelle apres le si
	Double_t esi = ((KVSilicon_e475s* )idbefore->GetDetector(2))->GetCalibratedEnergy();		
	if (esi<0) esi=0; idbefore->GetDetector(2)->SetEnergy(esi);
	//ajout a l energie totale
	etot+=esi;
	
	//si aucune energie detecte dans les etages precedents -> neutron
	if (esi==0 && eci==0){
		nuc->SetZ(0); nuc->SetA(1); ((KVINDRAReconNuc *) nuc)->SetIDCode(kIDCode1);
		nuc->SetEnergy(0);
		nuc->GetAnglesFromTelescope();
		return;
	}
	
	//Calcul de l energie residuelle laissee dans le Csi a partir de 
	//celle deposee dans le Si
	Double_t esi_res= idbefore->GetDetector(2)->GetEResFromDeltaE(nuc->GetZ(),nuc->GetA());
	if (esi_res<0) esi_res=0;
	
	//estimation de l energie deposee dans les csi
	Double_t ecsi = GetDetector(1)->GetDeltaE(nuc->GetZ(),nuc->GetA(),esi_res);
	if (ecsi<0) ecsi=0; GetDetector(1)->SetEnergy(ecsi);
	//ajout a l energie totale
	etot+=ecsi;
	
	//affiliation des energies et angles
	nuc->SetEnergy(etot);
	nuc->GetAnglesFromTelescope();
	fCalibStatus = kCalibStatus_Calculated;
	
	return;
}
