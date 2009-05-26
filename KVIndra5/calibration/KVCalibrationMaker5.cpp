/***************************************************************************
$Id: KVCalibrationMaker5.cpp,v 1.3 2003/10/16 09:24:22 franklan Exp $
                          kvcalibrationmaker5.cpp  -  description
                             -------------------
    begin                : mer mar 26 2003
    copyright            : (C) 2003 by Alexis Mignon
    email                : mignon@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "KVCalibrationMaker5.h"

ClassImp(KVCalibrationMaker5);
//__________________________________________________________________________
//
//  Class used to do calibration processes
//  We need to have at least indra declared to perform a complete
//  build
// 


//__________________________________________________________________________
KVCalibrationMaker5::KVCalibrationMaker5(){
  if (!gIndra) {
    Warning("KVCalibrationMaker5()","No KVINDRA object found I will create it now !");
    gIndra=new KVINDRA5();
    gIndra->Build();
  }
  fIndra=(KVINDRA5*)gIndra;
  
  if (!gDataBase) {
    Warning("KVCalibrationMaker5()","No Database object found I will create it now !");
    gDataBase=new KVDataBase5();
  }
  fDataBase=gDataBase;
}
//___________________________________________________________________________
void KVCalibrationMaker5::MakeAlphaCalibration(KVPeakDB5*kvpdb,Char_t*outfile)
{
// Méthode pour la calibration alpha
// On utilise la base des pics et la calibratioin Canal-Volt
// pour obtenir la calibration Volt-Energie alpha
// il s'agit d'une simple regression linéaire sans ordonnée
// à l'origine puisque la calibration Canal-Volt prend en compte
// ( ou devrait ) les piedestaux.
// Pour la calibration Volt-Elpha nous n'utilisons que les pics
// en signal grand gain puisqu'ils ont la meilleurs solution
//
// Version temporaire du fichier de lecture qui devrais à terme lire dans
// le fichier des pics les valeurs des run ranges pour lesquels les pics
// sont valables
//

  // ouverture du fichier de sortie
  ofstream ofile(outfile);
  
  //Formule pour le fit
  TF1 formula("formula","pol1");
  formula.FixParameter(0,0);
  
  TString coef_name; //nom du coefficient de calibration
  KVChannelVolt*kvcal=0;
  
  // Ecritures des commentaires relatifs au format du fichier
  ofile<<"############################################################"<<endl
       <<"#$Id: KVCalibrationMaker5.cpp,v 1.3 2003/10/16 09:24:22 franklan Exp $#"<<endl
       <<"# Fichier des paramètres de calibration Volt-E_alpha       #"<<endl
       <<"# Detector_type coef error first last                      #"<<endl
       <<"# Detector_type est construit sur le schema suivant :      #"<<endl
       <<"# type.de.detector_courmodul_signal                        #"<<endl
       <<"# ex : SI_0205_GG                                          #"<<endl
       <<"# coef : valeur trouvé du coefficient de calibration       #"<<endl
       <<"# error : erreur sur ce coefficient                        #"<<endl
       <<"# first, last : premier et dernier run de validité du      #"<<endl
       <<"# coef                                                     #"<<endl
       <<"############################################################"<<endl;
     
  
   UInt_t peaks_found_gg=0;
   
   Double_t raw_peaks_gg[4];// Pics en canaux grand gain
   Double_t volt_peaks[4];// pics en volts grand gain
   Double_t ener_peaks[4];// pics en energie
   
   // Les pics seront ordonnés comme suit
   // 0 1er pic Thoron avec gaz
   // 1 2e pic Thoron avec gaz
   // 2 1er pic Thoron sans gaz
   // 3 2e pic Thoron sans gaz
   // Seuls les deux premiers seront utilisés pour les ChIo
   
  KVChIo* kvchio;
  KVSilicon* kvsi;
  KVPeak* kvp;
  //_______________________________________________________________________________
  // Calibration  des ChIos
  // il n'y a que deux points de calibration
  
  TIter next_chio(fChIo);
  while( (kvchio=(KVChIo*)next_chio()) )
  {
    peaks_found_gg=0;
        
    TString peak_name,peak_type;
    // On cherche les pics grand gain
    peak_name=kvchio->GetFullName();
    peak_name.Append("_GG");
    
    peak_type="Thoron_1_AG";
    kvp=(KVPeak*)kvpdb->FindPeak(peak_name.Data(),peak_type.Data());
    if (!kvp) WarningPeakNotFound(peak_name.Data(),peak_type.Data());
    else 
    {
      raw_peaks_gg[0]=kvp->GetMean();
      kvcal=(KVChannelVolt*)kvchio->GetListOfCalibrators()->GetObject(peak_name.Data(),"Channel-Volt");
      if(kvcal&&kvcal->GetStatus())
      {
        volt_peaks[0]=kvchio->GetVoltsFromCanalGG(raw_peaks_gg[0]);
        peaks_found_gg++;
      }      
    }
    
    peak_type="Thoron_2_AG";
    kvp=(KVPeak*)kvpdb->FindPeak(peak_name.Data(),peak_type.Data());
    if (!kvp) WarningPeakNotFound(peak_name.Data(),peak_type.Data());
    else 
    {
      raw_peaks_gg[1]=kvp->GetMean();
      kvcal=(KVChannelVolt*)kvchio->GetListOfCalibrators()->GetObject(peak_name.Data(),"Channel-Volt");
      if(kvcal&&kvcal->GetStatus())
      {
        volt_peaks[1]=kvchio->GetVoltsFromCanalGG(raw_peaks_gg[1]);
        peaks_found_gg++;
      }      
    }
    
    //Simulate Peaks in ChIo
    if ( peaks_found_gg==2)
    { // Au moins une des deux gammes de gain a été trouvée
      KVNucleus n(2,4);
      TString det_name=kvchio->GetFullName();
       //enleve le debut du nom de la Chio
       //si les chios s'appellent "CI_0813" (format de l'acquisition GANIL)
       //alors il faut enlever les 2 premieres caracteres: Remove(0,2)
      det_name.Remove(0,2);
      det_name.Prepend("SI"); // On cherche le Si derrière la chio
      kvsi=(KVSilicon*)GetDetector(det_name.Data());
      if(kvsi)
      {
        //1er pic
        n.SetKE(KV_THORON_1);
        kvchio->Reset();
        kvsi->Reset();
        DetectParticleIn(kvsi->GetFullName(),&n);
        ener_peaks[0]=kvchio->GetEnergy();
        
        //2e pic
        n.SetKE(KV_THORON_2);
        kvchio->Reset();
        kvsi->Reset();
        DetectParticleIn(kvsi->GetFullName(),&n);
        ener_peaks[1]=kvchio->GetEnergy();
      }
      else 
      {
        cout<<"void KVINDRA5::MakeAlphaCalibration(KVPeakDB5*,Char_t*): Error : No CsI behind the ChIo."<<endl;
        exit(1);
      }
    }
    //Détermination des coefficients d'étalonnage
    TString coef_name;
    if (peaks_found_gg==2)
    {
      TGraph gr_gg(2,volt_peaks,ener_peaks);
      coef_name=kvchio->GetFullName();
      gr_gg.Fit(&formula,"NQW");
      ofile<<coef_name.Data()<<" "<<formula.GetParameter(1)<<" "<<formula.GetParError(1)<<endl;
      if(formula.GetParameter(1)==0&& formula.GetParError(1)!=0){
        cout<<coef_name.Data()<<" "<<kvchio->GetPressure()<<endl
            <<ener_peaks[0]<<" \t "<<ener_peaks[1]<<endl
            <<volt_peaks[0]<<" \t "<<volt_peaks[1]<<endl
            <<raw_peaks_gg[0]<<" \t "<<raw_peaks_gg[1]<<endl;
      }
    }
  }// fin de la boucle sur les ChIo
  
  //_________________________________________________________________________________
  //Calibration des Si
  // il y a quatre points de calibration
  
  TIter next_si(fSi);
  while( (kvsi=(KVSilicon*)next_si()) )
  {
    peaks_found_gg=0;
    
    TString peak_name,peak_type;
    // On cherche les pics grand gain
    peak_name=kvsi->GetFullName();
    peak_name.Append("_GG");
    
    peak_type="Thoron_1_AG";
    kvp=(KVPeak*)kvpdb->FindPeak(peak_name.Data(),peak_type.Data());
    if (!kvp) WarningPeakNotFound(peak_name.Data(),peak_type.Data());
    else 
    {
      raw_peaks_gg[peaks_found_gg]=kvp->GetMean();
      kvcal=(KVChannelVolt*)kvsi->GetListOfCalibrators()->GetObject(peak_name.Data(),"Channel-Volt");
      if(kvcal&&kvcal->GetStatus())
      {
        volt_peaks[peaks_found_gg]=kvsi->GetVoltsFromCanalGG(raw_peaks_gg[peaks_found_gg]);
        KVNucleus n(2,4);
        kvchio=(KVChIo*)GetChIoOf(kvsi->GetFullName());
        if(kvchio)
        {
         //1er pic
          n.SetKE(KV_THORON_1);
          kvchio->Reset();
          kvsi->Reset();
          DetectParticleIn(kvsi->GetFullName(),&n);
          ener_peaks[peaks_found_gg]=kvsi->GetEnergy();
         } 
        peaks_found_gg++;
      }      
    }
    
    peak_type="Thoron_2_AG";
    kvp=(KVPeak*)kvpdb->FindPeak(peak_name.Data(),peak_type.Data());
    if (!kvp) WarningPeakNotFound(peak_name.Data(),peak_type.Data());
    else 
    {
      raw_peaks_gg[peaks_found_gg]=kvp->GetMean();
      kvcal=(KVChannelVolt*)kvsi->GetListOfCalibrators()->GetObject(peak_name.Data(),"Channel-Volt");
      if(kvcal&&kvcal->GetStatus())
      {
        volt_peaks[peaks_found_gg]=kvsi->GetVoltsFromCanalGG(raw_peaks_gg[peaks_found_gg]);
        KVNucleus n(2,4);
        kvchio=(KVChIo*)GetChIoOf(kvsi->GetFullName());
        if(kvchio)
        {
         //2er pic
          n.SetKE(KV_THORON_2);
          kvchio->Reset();
          kvsi->Reset();
          DetectParticleIn(kvsi->GetFullName(),&n);
          ener_peaks[peaks_found_gg]=kvsi->GetEnergy();
         } 
        peaks_found_gg++;
      }      
    }

    peak_type="Thoron_1_SG";
    kvp=(KVPeak*)kvpdb->FindPeak(peak_name.Data(),peak_type.Data());
    if (!kvp) WarningPeakNotFound(peak_name.Data(),peak_type.Data());
    else 
    {
      raw_peaks_gg[peaks_found_gg]=kvp->GetMean();
      kvcal=(KVChannelVolt*)kvsi->GetListOfCalibrators()->GetObject(peak_name.Data(),"Channel-Volt");
      if(kvcal&&kvcal->GetStatus())
      {
        volt_peaks[peaks_found_gg]=kvsi->GetVoltsFromCanalGG(raw_peaks_gg[peaks_found_gg]);
        KVNucleus n(2,4);
        kvchio=(KVChIo*)GetChIoOf(kvsi->GetFullName());
        Double_t gaz_pressure;
        gaz_pressure=kvchio->GetPressure();
        kvchio->SetPressure(0.);
        if(kvchio)
        {
         //1er pic
          n.SetKE(KV_THORON_1);
          kvchio->Reset();
          kvsi->Reset();
          DetectParticleIn(kvsi->GetFullName(),&n);
          ener_peaks[peaks_found_gg]=kvsi->GetEnergy();
        }
        kvchio->SetPressure(gaz_pressure); 
        peaks_found_gg++;
      }      
    }
    
    peak_type="Thoron_2_SG";
    kvp=(KVPeak*)kvpdb->FindPeak(peak_name.Data(),peak_type.Data());
    if (!kvp) WarningPeakNotFound(peak_name.Data(),peak_type.Data());
    else 
    {
      raw_peaks_gg[peaks_found_gg]=kvp->GetMean();
      kvcal=(KVChannelVolt*)kvsi->GetListOfCalibrators()->GetObject(peak_name.Data(),"Channel-Volt");
      if(kvcal&&kvcal->GetStatus())
      {
        volt_peaks[peaks_found_gg]=kvsi->GetVoltsFromCanalGG(raw_peaks_gg[peaks_found_gg]);
        KVNucleus n(2,4);
        kvchio=(KVChIo*)GetChIoOf(kvsi->GetFullName());
        Double_t gaz_pressure;
        gaz_pressure=kvchio->GetPressure();
        kvchio->SetPressure(0.);
        if(kvchio)
        {
         //2er pic
          n.SetKE(KV_THORON_2);
          kvchio->Reset();
          kvsi->Reset();
          DetectParticleIn(kvsi->GetFullName(),&n);
          ener_peaks[peaks_found_gg]=kvsi->GetEnergy();
        }
        kvchio->SetPressure(gaz_pressure); 
        peaks_found_gg++;
      }      
    }
    
    //Détermination des coefficients d'étalonnage
    TString coef_name;
    if (peaks_found_gg>=2)
    {
      TGraph gr_gg(peaks_found_gg,volt_peaks,ener_peaks);
      coef_name=kvsi->GetFullName();
      gr_gg.Fit(&formula,"NQW");
      ofile<<coef_name.Data()<<" "<<formula.GetParameter(1)<<" "<<formula.GetParError(1)<<endl; 
    }
  }// fin de la boucle sur les Si
}
//________________________________________________________________________________
void KVCalibrationMaker5::MakeAlphaCalibration(Char_t*outfile)
{
// Méthode pour la calibration alpha
// On utilise la base des pics et la calibratioin Canal-Volt
// pour obtenir la calibration Volt-Energie alpha
// il s'agit d'une simple regression linéaire sans ordonnée
// à l'origine puisque la calibration Canal-Volt prend en compte
// ( ou devrait ) les piedestaux.
// Pour la calibration Volt-Elpha nous n'utilisons que les pics
// en signal grand gain puisqu'ils ont la meilleurs solution
//
// Version temporaire du fichier de lecture qui devrais à terme lire dans
// le fichier des pics les valeurs des run ranges pour lesquels les pics
// sont valables
//

  // ouverture du fichier de sortie
  ofstream ofile(outfile);
  
  //Formule pour le fit
  TF1 formula("formula","pol1");
  formula.FixParameter(0,0);
  
  TString coef_name; //nom du coefficient de calibration
  KVChannelVolt*kvcal=0;
  
  const Char_t* gains[2]={"GG","PG"};
  
  // Ecritures des commentaires relatifs au format du fichier
  ofile<<"############################################################"<<endl
       <<"#$Id: KVCalibrationMaker5.cpp,v 1.3 2003/10/16 09:24:22 franklan Exp $#"<<endl
       <<"# Fichier des paramètres de calibration Volt-E_alpha       #"<<endl
       <<"# Detector_type coef error first last                      #"<<endl
       <<"# Detector_type est construit sur le schema suivant :      #"<<endl
       <<"# type.de.detector_courmodul_signal                        #"<<endl
       <<"# ex : SI_0205_GG                                          #"<<endl
       <<"# coef : valeur trouvé du coefficient de calibration       #"<<endl
       <<"# error : erreur sur ce coefficient                        #"<<endl
       <<"# first, last : premier et dernier run de validité du      #"<<endl
       <<"# coef                                                     #"<<endl
       <<"############################################################"<<endl;       
  
  //Calibration des ChIos
  KVList *peaks;
  KVRList* ChIos=gIndra->GetListOfChIos();
  
  for (UInt_t i=0;i<2;i++){//boucle sur GG et PG
    
  
  
  
  }
