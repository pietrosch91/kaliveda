/***************************************************************************
                          KVIDSiCorrCsI.cpp  -  description
                             -------------------
    begin                : Fri Feb 20 2004
    copyright            : (C) 2004 by J.D. Frankland
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

#include "KVIDSiCorrCsI.h"

ClassImp(KVIDSiCorrCsI)

/////////////////////////////////////////////////////////////////////////////////////
//KVIDSiCorrCsI
//
// Identification in SiCorrelated:CsI(total light) matrices for INDRA_E503. 
// Each fit has an associated Zmin & ZMax (entered when generating the fit)
// Z identification is considered possible between max(Zmin-0.5, 0.5) and min(Zmax+0.5, 100.5)
//
// All of the grids were fitted using the LTG fit functional "fede" (10 fit parameters, extended functional, total light)

// ID SUB-CODES:
// -------------
//  9  KVIDSiCorrCsI::kZOK_AnoTGID       "Z ok, A identification tried but not available",
// 10  KVIDSiCorrCsI::kZOK_AOutOfIDRange "Z ok, A identification tried but point to identify outside of identification range of function",
// 11  KVIDSiCorrCsI::kZOK_A_ZtooSmall   "Z ok, A identification tried but IdentA called with Z<1",
// 12  KVIDSiCorrCsI::kZOK_A_ZtooLarge   "Z ok, A identification tried but IdentA called with Z larger than max Z defined for KVTGIDZA isotopic identification object"
/////////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
Bool_t KVIDSiCorrCsI::Identify(KVIdentificationResult* IDR, Double_t x, Double_t y)
{

    Double_t xVar; xVar = x; // not used (Keeps the compiler quiet)
    Double_t yVar; yVar = y; // not used

    //Identification of particles using SiCorrelated-CsI matrices for E503/E494s
    //First of all, Z identification is attempted with KVIDSiCorrCsI::IdentZ.
    //If successful, if this telescope has mass identification capabilities
    //(HasMassID()=kTRUE), then if the identified Z is not too large for the
    //GG grid Z&A identification, we identify the mass of the particle.
    //In this case, a further check is made that the resulting nucleus is not
    //too bizarre, i.e. it must be in the KVNucleus mass table (KVNucleus::IsKnown = kTRUE).
    //If not, or if mass identification is not possible, the mass of the nucleus
    //is deduced from Z using the default mass formula of class KVNucleus.
    //
    // Note that optional arguments (x,y) for testing identification are not used.

    Double_t funLTG_Z = -1;
    Double_t funLTG_A = -1;
    Double_t mass = -1;
    Int_t ia = -1;
    Int_t iz = -1;

    IDR->SetIDType( GetType() );
    IDR->IDattempted = kTRUE;

    Double_t Z = -1.;

    //PrintFitParameters();
    
    Bool_t inRange = (GetIDMapY("") < 4090.) 
                        && (GetIDMapY("") > 0.) 
                        && (GetIDMapX("") > 0.);

    if(inRange == 1){
        Z = IdentZ(this, funLTG_Z, "", "");
    }else{
        return kFALSE;
    }

    //use KVTGIDManager::GetStatus value for IdentZ as identification subcode
    IDR->IDquality = GetStatus();

    if(GetStatus() == KVTGID::kStatus_NotBetween_IDMin_IDMax){
        return kFALSE;
    }

    if (GetStatus() != KVTGIDManager::kStatus_OK){
        return kFALSE;            // no ID
    }

    iz = TMath::Nint(Z);

    IDR->IDOK = kTRUE;

    //is mass identification a possibility ?
    if(iz < 9){
       
        mass = IdentA(this, funLTG_A, "", "", iz);

        if (GetStatus() != KVTGIDManager::kStatus_OK){     //mass ID not good ?

            //only Z identified
            IDR->Z = iz;
            IDR->PID = Z;
            IDR->Zident = kTRUE;
            //subcode says "Z ok but A failed because..."
            IDR->IDquality = GetStatus();

        }else{                    //both Z and A successful ?

            ia = TMath::Nint(mass);
            // fix 16B
            if(iz==5 && ia==16) ia = (mass>16.0 ? 17 : 15);
            // fix 9B
            if(iz==5 && ia==9) ia = (mass>9.0 ? 10 : 8);
            // fix 8Be
            if(iz==4 && ia==8) ia = (mass>8.0 ? 9 : 7);
            // fix 13Be
            if(iz==4 && ia==13) ia = (mass>13.0 ? 14 : 12);
            // fix 10Li
            if(iz==3 && ia==10) ia = (mass>10.0 ? 11 : 9);
            // fix 5He
            if(iz==2 && ia==5) ia = (mass>5.0 ? 6 : 4);
            // fix 7He
            if(iz==2 && ia==7) ia = (mass>7.0 ? 8 : 6);
            // check that mass is not too bizarre
            KVNucleus tmp(iz,ia);
            if(!tmp.IsKnown()){
                // if ia seems too light, we try with iz-1
                // if ia seems too heavy, we try with iz+1
                Info("Identify","%s : initial ID Z=%d A=%d funLTG=%g", GetName(), iz, ia, funLTG_A);
                Int_t iz2 = (ia < 2*iz ? iz-1 : iz+1);
                if(iz2>0){
                    Double_t old_funLTG_A = funLTG_A;
                    Double_t new_mass = IdentA(this, funLTG_A, "", "", iz2);
                    // is this a better solution ?
                    if(GetStatus() == KVTGIDManager::kStatus_OK){
                        Int_t new_ia = TMath::Nint(new_mass);
                        tmp.SetZ(iz2); tmp.SetA(new_ia);
                        Info("Identify","%s : new ID Z=%d A=%d funLTG=%g", GetName(), iz2, new_ia, funLTG_A);
                        if(tmp.IsKnown() && (TMath::Abs(funLTG_A)<TMath::Abs(old_funLTG_A))){
                            // new nucleus is known and point is closer to line
                            iz = iz2; ia=new_ia; mass=new_mass;
                            Info("Identify","%s : accepted new ID", GetName());
                        }else{
                            Info("Identify","%s : rejected new ID", GetName());
                        }
                    }else{
                        Info("Identify","%s : failed to obtain new ID with Z=%d", GetName(), iz2);
                    }
                }
            }
            IDR->Z = iz;
            IDR->A = ia;
            IDR->PID = mass;
            IDR->Zident = kTRUE;
            IDR->Aident = kTRUE;
        }
    }else{
        //only Z identified
        //ID subcode remains 'Zstatus'
        IDR->Z = iz;
        IDR->PID = Z;
        IDR->Zident = kTRUE;
    
    }

    // set general ID code
    IDR->IDcode = kIDCode3;
    return kTRUE;
}

//__________________________________________________________________________//

void KVIDSiCorrCsI::Print(Option_t * opt) const
{
    Option_t *tmp; tmp = opt;     // not used (keeps the compiler quiet)

    //cout << "KVIDSiCorrCsI : " << GetName() << endl;

    if (GetListOfIDFunctions().GetSize() > 0) {

        if (HasMassID()){
            //cout << "  A and Z identification" << endl;
        }else{
           // cout << "  Z identification only" << endl;
        }

        //cout << "Identification objects :" << endl;
        GetListOfIDFunctions().Print();
    }
}

//__________________________________________________________________________//

Double_t KVIDSiCorrCsI::GetIDMapX(Option_t * opt)
{
    Option_t *tmp; tmp = opt; // not used (keeps the compiler quiet)

    //Calculates current X coordinate for identification.

    Double_t h = (Double_t)fCsI->GetLumiereTotale();

    return h;
}
//__________________________________________________________________________//

void KVIDSiCorrCsI::Initialize()
{

    // Initialisation of telescope before identification.
    // This method MUST be called once before any identification is attempted.
    // IsReadyForID() will return kTRUE if KVTGID objects are associated
    // to this telescope for the current run.

    fSi = (KVSilicon*)GetDetector(1);

    fSiPGPedestal = fSi->GetPedestal("PG");
    fSiGGPedestal = fSi->GetPedestal("GG");

    fCsI = (KVCsI*)GetDetector(2);
    fCsIRPedestal = fCsI->GetPedestal("R");
    fCsILPedestal = fCsI->GetPedestal("L");

    if(GetListOfIDFunctions().GetEntries()){
        SetBit(kReadyForID);
    }else{
      ResetBit(kReadyForID);
    }

}

//__________________________________________________________________________//

Double_t KVIDSiCorrCsI::GetIDMapY(Option_t * opt)
{

    Option_t *tmp; tmp = opt; // not used (keeps the compiler quiet)

    //Calculates current Y coordinate for identification (Silicon Correlation)

    Double_t si = -1.;
    Double_t siPG = -1.;
    Double_t siPG_ped = -1.;
    Double_t siGG = -1.;
    Double_t siGG_ped = -1.;

    if(fSi != 0){

        if(fSi->GetGG() < 3900.){

            siPG = (Double_t)fSi->GetPG();
            siPG_ped = fSiPGPedestal;

            siGG = (Double_t)fSi->GetGG();
            siGG_ped = fSiGGPedestal;

            si = fSi->GetPGfromGG(siGG) - siPG_ped;

        }else{

            siPG = (Double_t)fSi->GetPG();
            siPG_ped = fSiPGPedestal;

            si = siPG - siPG_ped;
        }
    
    }else{

        return 10000.;
    }

    return si;
}

//__________________________________________________________________________//

const Char_t *KVIDSiCorrCsI::GetIDSubCodeString(KVIDSubCode &concat) const
{
    //Returns string explaining subcodes for this telescope

    static TString messages[] = {
        "no Z identification available",
        "point outside of range of possible Z identification",
        "Z ok, A identification tried but not available",
        "Z ok, A identification tried but point to identify outside of identification range of function",
        "Z ok, A identification tried but IdentA called with Z<1",
        "Z ok, A identification tried but IdentA called with Z larger than max Z defined for KVTGIDZA isotopic identification object"
    };
   
    Int_t code = GetIDSubCode(concat);
    if (code == -1)
        return SubCodeNotSet();
    return messages[code];
}

//____________________________________________________________________________________

Bool_t KVIDSiCorrCsI::SetIdentificationParameters(const KVMultiDetArray* MDA)
{
    //Initialise the identification parameters (grids, etc.) of ALL identification telescopes of this
    //kind (label) in the multidetector array. Therefore this method need only be called once, and not
    //called for each telescope. The kind/label (returned by GetLabel) of the telescope corresponds
    //to the URI used to find the plugin class in $KVROOT/KVFiles/.kvrootrc.
    //Parameters are read from the file with name given by the environment variable
    //INDRA_camp5.IdentificationParameterFile.SI-CSI:       [filename]

    TString filename = gDataSet->GetDataSetEnv("IdentificationParameterFile.SI-CSI");

    if( filename == "" ){
        Warning("SetIdentificationParameters",
            "No filename defined. Should be given by %s.IdentificationParameterFile.SI-CSI",
            gDataSet->GetName());
        return kFALSE;
    }

    TString path;
    if (!SearchKVFile(filename.Data(), path, gDataSet->GetName())){
        Error("SetIdentificationParameters",
            "File %s not found. Should be in $KVROOT/KVFiles/%s",
            filename.Data(), gDataSet->GetName());
        return kFALSE;
    }
    Info("SetIdentificationParameters", "Using file %s", path.Data());

    //cout << "Setting identification parameters..." << endl;

    ifstream datfile;
    datfile.open(path.Data());

    KVString line;

    int zOrA = -1;
    int zmin = -1;
    int zmax = -1;

    Double_t param[10];

    int ring = -1;
    int module = -1;

    KVIDSiCorrCsI *idt = 0;

    while(datfile.good()){

        line.ReadLine(datfile);

        if(line.BeginsWith("+")){

            sscanf(line.Data(), "++KVTGID::SI_CSI_%02d%02d_fit", &ring, &module);

            stringstream name;
            name << "SI_CSI_" << setfill('0') << setw(2) << ring 
                    << setfill('0') << setw(2) << module;
                
            idt = (KVIDSiCorrCsI*) MDA->GetIDTelescope(name.str().c_str());
        }

        sscanf(line.Data(), "ZorA=%i", &zOrA); 
        sscanf(line.Data(), "ZMIN=%i ZMAX=%i", &zmin, &zmax);

        if(line.BeginsWith("ZMIN=")){

            for(int i=0; i<10; i++){    // read 10 fit parameters 

                line.ReadLine(datfile);
                string thisLine = line.Data();

                int equalsLocation = thisLine.find('=');
                line.Remove(0, equalsLocation + 1);

                param[i] = line.Atof();
            }
        }

        if(line.BeginsWith("!")){
            //create new Tassan-Got ID object for telescope

            // tgidA and tgidZ have EXACTLY the same fit parameters.
            KVTGID *_tgidZ = 0;
            KVTGID *_tgidA = 0;

            _tgidZ = KVTGID::MakeTGID(idt->GetTGIDName(idt->GetName(), "Z", ""), 1, 1, 1, KVNucleus::kEALResMass);
            _tgidA = KVTGID::MakeTGID(idt->GetTGIDName(idt->GetName(), "A", ""), 1, 1, 0, KVNucleus::kEALResMass);
            // 10 parameters, 1 = extended formula, 1 = CsI total light

            _tgidZ->SetIDmin((Double_t) zmin);
            _tgidZ->SetIDmax((Double_t) zmax);

            _tgidA->SetIDmin((Double_t) zmin);
            _tgidA->SetIDmax((Double_t) zmax);

            //read line with parameters on it from file
            _tgidZ->SetLTGParameters(param);
            _tgidA->SetLTGParameters(param);

            KVNumberList runList("1-700");

            _tgidZ->SetValidRuns(runList);
            _tgidZ->AddIDTelescope(idt);

            _tgidA->SetValidRuns(runList);
            _tgidA->AddIDTelescope(idt);

            //add identification object to telescope's ID manager
            idt->AddTGID(_tgidZ);
            idt->AddTGID(_tgidA);
        }
    }

    datfile.close();

    //SetSiCorrelationParameters(MDA);

    return kTRUE;
}

Bool_t KVIDSiCorrCsI::SetSiCorrelationParameters(const KVMultiDetArray* MDA){

    //cout << "Setting silicon correlation parameters..." << endl;

    TString filename = "ParamFitPol1_Sipg_gg.dat_new.dat";
    
    TString path;
    if (!SearchKVFile(filename.Data(), path, gDataSet->GetName())){
        Error("SetSiCorrelationParameters",
            "File %s not found. Should be in $KVROOT/KVFiles/%s",
            filename.Data(), gDataSet->GetName());

        return kFALSE;
    }

    Info("SetSiCorrelationParameters", "Using file %s", path.Data());

    ifstream file;
    file.open(path.Data());

    Int_t ringNum = -1;
    Int_t moduleNum = -1;

    string line;
    Double_t par0 = -1.;
    Double_t par1 = -1.;
    Double_t par2 = -1.;

    KVIDSiCorrCsI  *idt;

    while (file.good() == 1) {
        std::getline(file, line);
        sscanf(line.c_str(), "SI_%02d%02d %lf %lf %lf", &ringNum, &moduleNum, &par0, &par1, &par2);

        stringstream name;
        name << "SI_CSI_" << setfill('0') << setw(2) << ringNum
                << setfill('0') << setw(2) << moduleNum;

        idt = (KVIDSiCorrCsI*) MDA->GetIDTelescope(name.str().c_str());

        idt->AddParamsToTelescope(par0, par1, par2);

    }

    return kTRUE;
}

void KVIDSiCorrCsI::RemoveIdentificationParameters()
{
   //Delete any KVTGID objects associated with this telescope
   RemoveAllTGID();
}

void KVIDSiCorrCsI::PrintFitParameters(){

    cout << endl;
    cout << "___Fit Parameters___" << endl;

    KVTGID *tgid = 0;
    cout << "KVTGID Name(2): " << GetTGIDName(GetName(), "", "") << endl;
    tgid = GetTGID(GetTGIDName(GetName(), "", ""));
            
    if(tgid != 0){
    
       cout << "Functional Type: " << tgid->GetFunctionalType() << endl;
       cout << "Light Energy Dependence: " << tgid->GetLightEnergyDependence() << endl;    
       cout << endl;
       cout << "Lambda: " << tgid->GetLambda() << endl;
       cout << "Alpha: " << tgid->GetAlpha() << endl;
       cout << "Beta: "  << tgid->GetBeta() << endl;
       cout << "Mu: " << tgid->GetMu() << endl;
       cout << "Nu: " << tgid->GetNu() << endl;
       cout << "Xi: " << tgid->GetXi() << endl;
       cout << "G: " << tgid->GetG() << endl;
       cout << "Pdx: " << tgid->GetPdx() << endl;
       cout << "Pdy: " << tgid->GetPdy() << endl;
       cout << "Eta: " << tgid->GetEta() << endl;
       cout << endl;
   }else{
       cout << "No fit parameters present" << endl;
   }

    cout << endl;

}

void KVIDSiCorrCsI::PrintSiCorrelationParameters(){

    cout << endl;
    cout << "___Silicon Correlation Parameters___" << endl;

    for(int i=4; i<10; i++){
        for(int j=1; j<25; j++){
    
            cout << "SI_CSI_" << setfill('0') << setw(2) << i 
                                << setfill('0') << setw(2)  << j << ":"
                                << " p0: " << param0 
                                << " p1: " << param1 
                                << " p2: " << param2 << endl;
        }
    }
}


Double_t KVIDSiCorrCsI::GetSiCorrParam(Int_t npar){

    if(npar == 0) return param0;
    else if(npar == 1) return param1;
    else if(npar == 2) return param2;

    else
        return -5.;

}
