//Created by KVClassFactory on Wed Nov  9 10:01:49 2011
//Author: Peter Wigg

#include "KVIVSelectorPCW.h"
#include "KVINDRAReconNuc.h"
#include "KVBatchSystem.h"
#include "KVINDRA.h"
#include "KVReconIdent.h"
#include "KVIDGridManager.h"
#include "KVSiliconVamos.h"
#include "KVMaterial.h"
#include "KVCsIVamos.h"

ClassImp(KVIVSelectorPCW)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIVSelectorPCW</h2>
<h4>User analysis class</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

// This class is derived from the KaliVeda class KVIVSelector.
// The following members functions are called by the TTree::Process() functions:
//    InitRun():       called everytime a run starts
//    EndRun():        called everytime a run ends
//    InitAnalysis():  called at the beginning of the analysis
//                     a convenient place to create your histograms.
//    Analysis():      called for each event. In this function you 
//                     fill your histograms.
//    EndAnalysis():   called at the end of a loop on the tree,
//                     a convenient place to draw/fit your histograms.

void KVIVSelectorPCW::InitAnalysis(void){

    workOnSps = 1;

    if(workOnSps == 1){
        dir += "/sps/indra/ganil/peter/test";
    }else{
        dir += ".";
    }

    evNum = 0;

    l = new LogFile;

    if(gBatchSystem){
        l->Open(Form("%s/%s.log", dir.c_str(), gBatchSystem->GetJobName()));
    }else{
        l->Open(Form("%s/Analysis_e503.log", dir.c_str()));
    }

    inTree = (TTree*)gROOT->FindObject("ReconstructedEvents");
    //inTree->Print();

    si = 0;
    dr = 0;
    rcn = 0;
    ic = 0;
    csi = 0;
    et = 0;
    id = 0;

    si = new Sive503(l);
    dr = new DriftChamberv(l,si);
    rcn = new Reconstructionv(l, dr);

    ic = new IonisationChamberv(l);
    csi = new CsIv(l);
    et = new EnergyTree(l, si);

    id = new Identificationv(l, rcn, dr, ic, si, csi, et);

    l->Log << "Pointer Addresses: " << endl;
    l->Log << " + inTree: " << inTree << endl;
    l->Log << " + dr: " << dr << endl;
    l->Log << " + rcn: " << rcn << endl;
    l->Log << " + si: " << si << endl;

    l->Log << " + ic: " << ic << endl;
    l->Log << " + csi: " << csi << endl;
    l->Log << " + et: " << et << endl;
    l->Log << " + id: " << id << endl;

    runNumber = 0;

    this->LoadGrids();
    this->BuildFocalPlane();

}

//_____________________________________
void KVIVSelectorPCW::InitRun(void){

    runNumber = (Int_t) gIndra->GetCurrentRunNumber();

    outTree = new TTree("ReconstructedEvents","ReconstructedEvents");

    if(gBatchSystem){
        f = new TFile(Form("%s/%s.root", dir.c_str(), gBatchSystem->GetJobName()), "recreate");
    }else{
        f = new TFile(Form("%s/Analysis_e503_r%03i.root", dir.c_str(), runNumber), "recreate");
    }

    l->Log << endl << "******" << endl;
    l->Log << "*** START OF RUN " << gIndra->GetCurrentRunNumber() << endl;
    l->Log << "******" << endl << endl;

    l->Log << "*** Attaching input tree..." << endl;
    dr->inAttach(inTree);
    si->inAttach(inTree);
    ic->inAttach(inTree);
    csi->inAttach(inTree);
    id->inAttach(inTree);

    l->Log << "*** Attaching output tree..." << endl;
    l->Log << " + outTree: " << outTree << endl;

    dr->outAttach(outTree);
    rcn->outAttach(outTree);
    si->outAttach(outTree);

    ic->outAttach(outTree);
    csi->outAttach(outTree);
    id->outAttach(outTree);

    evNum = 0;

}

//_____________________________________
Bool_t KVIVSelectorPCW::Analysis(void){

    l->Log << endl << "*** EventNumber: " << evNum << endl;

    dr->Treat();

    rcn->Init();
    rcn->Calculate();

    si->Treat();   
    this->PassTelescope(si->Number);

    ic->Treat();
    csi->Treat();
    id->Treat();

    outTree->Fill();

    evNum++;

    return kTRUE;

}

//_____________________________________
void KVIVSelectorPCW::EndRun(void){
    l->Log << "******" << endl;
    l->Log << "*** End of Run: " << evNum << " events" << endl;
    l->Log << "******" << endl;
   
    f->cd(); 
    outTree->Write();
    f->Close();
    outTree->Delete();
}

//_____________________________________
void KVIVSelectorPCW::EndAnalysis(void){

    //f->cd();
    //outTree->Write();
    //f->Close();

    l->Log << endl << "KVIVSelectorPCW::EndAnalysis()" << endl;

    this->DeleteFocalPlane();

    l->Log << "Deleting analysis stuff..." << endl;

    if(si != 0){
        delete si;
        si = 0;
    }

    if(dr != 0){
        delete dr;
        dr = 0;
    }

    if(rcn != 0){
        delete rcn;
        rcn = 0;
    }

    if(ic != 0){
        delete ic;
        ic = 0;
    }

    if(csi != 0){
        delete csi;
        csi = 0;
    }

    if(et != 0){
        delete et;
        et = 0;
    }

    if(id != 0){
        delete id;
        id = 0;
    }

    l->Close();

    if(l != 0){
        delete l;
        l = 0;
    }
}

Bool_t KVIVSelectorPCW::LoadGrids(){

    const char *grid_map= "grid_vamos.dat";
    const char *grid_map2= "SI_CSI_C4-9.dat";

    l->Log << "Loading grids from " << grid_map << endl;
    l->Log << "Loading grids from " << grid_map2 << endl;

    if(gIDGridManager == 0){
        l->Log << "gIDGridManager not running" << endl;
        return 1;
    }

    char ds_path[256];
    sprintf(ds_path, "null");
    char ds_path2[256];
    sprintf(ds_path2, "null");

    if(gDataSet->GetDataSetDir() == 0){
        l->Log << "Failed to retrieve data set directory" << endl;
        return 1;
    }

    sprintf(ds_path, "%s/%s", gDataSet->GetDataSetDir(), grid_map);
    sprintf(ds_path2, "%s/%s", gDataSet->GetDataSetDir(), grid_map2);

    l->Log << "Reading grid map: " << grid_map << endl;
    l->Log << "Reading grid map: " << grid_map2 << endl;

    if(gIDGridManager->ReadAsciiFile(ds_path) != 0 && gIDGridManager->ReadAsciiFile(ds_path2) != 0){
        l->Log << "All grids from Indra and Vamos have been loaded" << endl;
    }else{
        return 1;
    }

    return 0;
}


void KVIVSelectorPCW::BuildFocalPlane(){

    l->Log << " * Constructing the focal plane detectors..." << endl;

    siVamos = 0;
    deadLayer = 0;
    csiVamos = 0;

    siVamos = new KVSiliconVamos*[18];
    fpTelescope = new KVTelescope*[18];

    Double_t siVamosThickness[18] = {522., 530., 531., 532., 533., 533., 534., 535., 531.,
                                    535., 524., 531., 529., 524., 533., 537., 519., 530.};

    isoButane = new KVMaterial("C4H10", 136.5*KVUnits::mm);
    isoButane->SetPressure(40.*KVUnits::mbar);

    deadLayer = new KVDetector();
    deadLayer->AddAbsorber(isoButane);
    
    csiVamos = new KVCsIVamos(1.*KVUnits::cm);

    for(Int_t i=0; i<18; i++){
        siVamos[i] = new KVSiliconVamos(siVamosThickness[i]); // Input in um

        fpTelescope[i] = new KVTelescope();
        fpTelescope[i]->Add(siVamos[i]);
        fpTelescope[i]->Add(deadLayer);
        fpTelescope[i]->Add(csiVamos);
        fpTelescope[i]->SetName(Form("SI_%02i", i+1));
        fpTelescope[i]->Print();
    }

    l->Log << " * Focal plane detectors have been built (Not verified yet)" << endl;

    return;
}

void KVIVSelectorPCW::DeleteFocalPlane(){

    l->Log << "KVIVSelectorPCW::DeleteFocalPlane()" << endl;

    if(siVamos != 0){
        delete[] siVamos;
        siVamos = 0;
    }

    if(deadLayer != 0){
        delete deadLayer;
        deadLayer = 0;
    }

    // Causes Segv
    //if(isoButane != 0){
    //     delete isoButane;
    //    isoButane = 0;
    //}

    if(csiVamos != 0){
        delete csiVamos;
        csiVamos = 0;   
    }

    if(fpTelescope != 0){
        delete[] fpTelescope;
        fpTelescope = 0;
    }

}

KVTelescope* KVIVSelectorPCW::GetTelescope(Int_t siNumber){
    return fpTelescope[siNumber];
}

void KVIVSelectorPCW::PassTelescope(Int_t siNumber){
    et->PassTelescope(fpTelescope[siNumber]);
    return;
}
