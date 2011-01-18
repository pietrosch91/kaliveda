/***************************************************************************
$Id: KVIDTelescope.cpp,v 1.52 2009/05/05 15:54:04 franklan Exp $
Author : $Author: franklan $
                          KVIDTelescope.cpp  -  description
                             -------------------
    begin                : Wed Jun 18 2003
    copyright            : (C) 2003 by J.D Frankland
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
#include "TROOT.h"
#include "KVIDTelescope.h"
#include "KVTelescope.h"
#include "KVGroup.h"
#include "KVNucleus.h"
#include "KVReconstructedNucleus.h"
#include "KVIDGraph.h"
#include "KVIDGrid.h"
#include "Riostream.h"
#include "TPluginManager.h"
#include "KVMultiDetArray.h"
#include "KVDataSet.h"
#include "KVIDGridManager.h"
#include "KVIDZALine.h"
#include "KVIdentificationResult.h"
#include "TMath.h"
#include "TClass.h"

ClassImp(KVIDTelescope)
/////////////////////////////////////////////////////////////////////////////////////////
//KVIDTelescope
//
//Charged particle identification is handled by KVIDTelescope and derived classes.
//A KVIDTelescope is an association of one or more detectors which is capable of particle
//identification.
//Although initially conceived in terms of DE-E two-stage telescopes, the identification
//method can be quite different (see KVIDCsI for example).
//
//Visualising Identification (ID) Grids & Maps
//--------------------------------------------
//Identification in such 'telescopes' is often associated with a 'grid' or set of
//graphical cuts permitting to associate the informations measured in the 'telescope'
//with certain types of particles. For any class derived from KVIDTelescope, the
//identification grid (KVIDGraph), if one exists, can be obtained and visualised using :
//
//      idtelescope->GetIDGrid()->Draw();
//
//Note that if no grid has been defined for the telescope, GetIDGrid() returns a null
//pointer, so you should only use the previous method if you are absolutely sure that
//the ID telescope in question has a grid.
//
//Visualising identifications based on fitted functionals
//--------------------------------------------------------
//The identification functionals devised by L. Tassan-Got and defined in KVTGIDFunctions
//can be visualised as identification grids for classes having KVTGIDManager as a base
//class. In this case a grid can be created and drawn using :
//
//      KVIDGrid* g = idtelescope->GetTGIDGrid("name_of_functional", max_X_coord_of_lines);
//      g->Draw();
//
//You should keep a pointer to the grid because it is the user's responsibility to delete it after
//use.
//
//Particle Identification and Calibration/Coherency
//-------------------------------------------------
//Particles reconstructed from data measured in the detectors of a multidetector array
//(KVReconstructedNucleus) are identified using the KVReconstructedNucleus::Identify()
//method. This takes the list of ID telescopes through whose detectors the particle passed,
//beginning with the telescope in which the particle stopped, and calls the Identify()
//method of each ID telescope in turn. This continues until identification is achieved.
//Thus the identification method, specific to each type of ID telescope, can be defined
//by overriding the Identify() method in child classes.
//
//Similarly, calibration of identified particles is achieved by KVReconstructedNucleus::
//Calibrate() which uses the Calibrate() method of the 'identifying telescope' (i.e. the
//KVIDTelescope in which identification was achieved). Again, this may be redefined in
//child classes (see KVIDCsI for example), but the default procedure defined here is to
//calculate the energy losses of the particle in all the detectors preceding the ID
//telescope and compare them to the measured energy losses in these detectors, in order
//to check for double-hits. The results of this procedure may then be used by some
//coherency algorithm.
//
//Setting identification parameters
//---------------------------------------------
//The method SetIdentificationParameters(const KVMultiDetArray*) will be used by the
//KVMultiDetArray to which this telescope belongs in order to set the parameters of
//ALL telescopes of this type in the array.
//By default, this method looks for the file with name given by the environment variable
//[dataset name].IdentificationParameterFile.[telescope label]:       [filename]
//which is assumed to contain identification grids. The file will be read in by gIDGridManager
//and the grids added to its list.
/////////////////////////////////////////////////////////////////////////////////////////
    KVIDTelescope::KVIDTelescope()
{
   init();
}

void KVIDTelescope::init()
{
   //default init
   fDetectors = new KVList(kFALSE);
   fDetectors->SetCleanup(kTRUE);
   fGroup = 0;
   fIDCode = 0;                 //default
   fIDGrids = new KVList(kFALSE);
   fIDGrids->SetCleanup(kTRUE);
}

KVIDTelescope::~KVIDTelescope()
{
   //delete this ID telescope
   if (fDetectors && fDetectors->TestBit(kNotDeleted)) {
      fDetectors->Clear();
      delete fDetectors;
   }
   fDetectors = 0;
   fGroup = 0;
   fIDGrids->Clear();
   SafeDelete(fIDGrids);
}

//___________________________________________________________________________________________
void KVIDTelescope::AddDetector(KVDetector * d)
{
   //Add a detector to the telescope.
   //The first detector added is the "DeltaE" member, the second the "Eresidual" member.

   if (d) {
      fDetectors->Add(d);
      d->AddIDTelescope(this);
   } else {
      Warning("AddDetector", "Called with null pointer");
   }
}

//___________________________________________________________________________________________
void KVIDTelescope::Print(Option_t * opt) const
{
// print out telescope structure
//if opt="fired" only fired detectors are printed

   TIter next(fDetectors);
   KVDetector *obj;

   if (!strcmp(opt, "fired")) {
      while ((obj = (KVDetector *) next())) {

         if (obj->Fired() || obj->GetEnergy())
            obj->Print("data");
      }
   } else {
      cout << "\n" << opt << "Structure of KVIDTelescope object: " <<
          GetName() << " " << GetType() << endl;
      cout << opt <<
          "--------------------------------------------------------" <<
          endl;
      while ((obj = (KVDetector *) next())) {
         cout << opt << "Detector: " << obj->GetName() << endl;
      }
   }
}

//____________________________________________________________________________________
const Char_t *KVIDTelescope::GetName() const
{
   // Name of telescope given in the form Det1_Det2_Ring-numberTelescope-number
   // The detectors are signified by their TYPE names i.e. KVDetector::GetType
   //
   //Just a wrapper for GetArrayName to allow polymorphism
   return ((KVIDTelescope *) this)->GetArrayName();
}
const Char_t *KVIDTelescope::GetArrayName()
{
   // Name of telescope given in the form Det1_Det2_Ring-numberTelescope-number
   //where ring and telescope numbers are those of the smallest (in angular terms)
   //detector of the telescope (if both are the same size, either one will do).
   // The detectors are signified by their TYPE names i.e. KVDetector::GetType

   //in order to access angular dimensions of detectors, we need their KVTelescopes
   KVTelescope *de_det = GetDetector(1)->GetTelescope();
   KVTelescope *e_det = 0;
   if (GetSize() > 1)
      e_det = GetDetector(2)->GetTelescope();
   UInt_t ring, mod;
   if (!e_det || de_det->IsSmallerThan(e_det)) {
      ring = de_det->GetRingNumber();
      mod = de_det->GetNumber();
   } else {
      ring = e_det->GetRingNumber();
      mod = e_det->GetNumber();
   }
   SetName(GetDetector(1)->GetType());
   if (e_det) {
      fName.Append("_");
      fName.Append(GetDetector(2)->GetType());
   }
   SetType(fName.Data());
   fName.Append("_");
   TString num;
   num.Form("%02d%02d", ring, mod);
   fName += num;

   return fName.Data();
}

//____________________________________________________________________________________
UInt_t KVIDTelescope::GetDetectorRank(KVDetector * kvd)
{
   //returns position (1=front, 2=next, etc.) detector in the telescope structure

   TIter next(fDetectors);
   KVDetector *d;
   UInt_t i = 1;
   while ((d = (KVDetector *) next())) {
      if (d == kvd)
         return i;
      i++;
   }
   return 0;
}

//____________________________________________________________________________________

KVDetector *KVIDTelescope::GetDetector(const Char_t * name) const
{
   // Return a pointer to the detector in the telescope with the name "name".

   KVDetector *tmp = (KVDetector *) fDetectors->FindObject(name);
   if (!tmp)
      Warning("GetDetector(const Char_t *name)",
              "Detector %s not found in telescope %s", name, GetName());
   return tmp;
}

//____________________________________________________________________________________

KVGroup *KVIDTelescope::GetGroup() const
{
   return fGroup;
}

//____________________________________________________________________________________

void KVIDTelescope::SetGroup(KVGroup * kvg)
{
   fGroup = kvg;
}

//____________________________________________________________________________________
UInt_t KVIDTelescope::GetGroupNumber()
{
   return (GetGroup()? GetGroup()->GetNumber() : 0);
}

//____________________________________________________________________________________

TGraph *KVIDTelescope::MakeIDLine(KVNucleus * nuc, Float_t Emin,
                                  Float_t Emax, Float_t Estep)
{
   //For a given nucleus, generate a TGraph representing the line in the deltaE-E
   //plane of the telescope which can be associated with nuclei of this (A,Z) with total
   //incident energies between the two limits.
   //NOTE: if there are other absorbers/detectors placed before this telescope,
   //the energy losses of the particle in these will be taken into account.
   //If the step in energy is not given, it is taken equal to 100 equal steps between min and max.
   //The TGraph should be deleted by the user after use.


   if (!Estep)
      Estep = (Emax - Emin) / 100.;

   Int_t nsteps = 1 + (Int_t) ((Emax - Emin) / Estep);

   if (nsteps < 1)
      return 0;

   Float_t *y = new Float_t[nsteps];
   Float_t *x = new Float_t[nsteps];
   Int_t step = 0;

   //get list of all detectors through which particle must pass in order to reach
   //2nd member of ID Telescope
   TList *detectors =
       GetGroup()->GetAlignedDetectors(GetDetector(2), KVGroup::kForwards);
   //detectors->ls();
   TIter next_det(detectors);
   //cout << "nsteps =" << nsteps << endl;

   for (Float_t E = Emin; E <= Emax; E += Estep) {
      //Set energy of nucleus
      nuc->SetEnergy(E);
      // cout << "Einc=" << E << endl;

      //Calculate energy loss in each member and stock in arrays x & y
      //first member
      KVDetector *det = 0;
      while ((det = (KVDetector *) next_det())) {
         //det->Print();
         Float_t eloss = det->GetELostByParticle(nuc);
         if (det == GetDetector(1))
            y[step] = eloss;
         else if (det == GetDetector(2))
            x[step] = eloss;
         //  cout << "Eloss=" << eloss << endl;
         //cout << "Enuc=" << nuc->GetEnergy() << endl;
      }

      //make sure that some energy is lost in each member
      //otherwise miss a step and reduce number of points in graph
      if (x[step] && y[step]) {
         step++;
      } else {
         nsteps--;
      }

      //cout << "nsteps =" << nsteps << endl;
      //reset iterator ready for next loop on detectors
      next_det.Reset();
   }
   TGraph *tmp = 0;
   if (nsteps > 1)
      tmp = new TGraph(nsteps, x, y);
   delete[]x;
   delete[]y;
   delete detectors;
   return tmp;
}

//____________________________________________________________________________________

Bool_t KVIDTelescope::Identify(KVIdentificationResult*)
{
   //Default identification method.
   //Identification of experimental data needs to be implemented in specific
   //child classes. Returns kFALSE.

   return kFALSE;
}

//____________________________________________________________________________________

void KVIDTelescope::SetIDGrid(KVIDGraph * grid)
{
   //Add an identification grid to the list of grids used by this telescope.
   fIDGrids->Add(grid);
}

//____________________________________________________________________________________

KVIDGraph *KVIDTelescope::GetIDGrid()
{
   //Return the first in the list of identification grids used by this telescope
   //(this is for backwards compatibility with ID telescopes which had only one grid).
   return GetIDGrid(1);
}

//____________________________________________________________________________________

KVIDGraph *KVIDTelescope::GetIDGrid(Int_t index)
{
   //Return pointer to grid using position in list. First grid has index = 1.
   return (KVIDGraph*)fIDGrids->At(index-1);
}

//____________________________________________________________________________________

KVIDGraph *KVIDTelescope::GetIDGrid(const Char_t* label)
{
   //Return pointer to grid using "label" to search in list of grids associated
   //to this telescope.
   return (KVIDGraph*)fIDGrids->FindObjectByLabel(label);
}

//____________________________________________________________________________________

Double_t KVIDTelescope::GetIDMapX(Option_t * opt)
{
   //This method should define how to calculate the X-coordinate in a 2D identification map
   //associated with the ID telescope based on the current state of the detectors.
   //By default, we assume a dE - E telescope, and so 'X' is equal to the energy measured in the
   //second detector of the telescope (or zero if there is not a 2nd detector in the telescope)
   //
   //However, if the detector is not calibrated, GetEnergy will return 0, therefore
   //if 'opt' is not an empty string (default) we return the value of the acquisition
   //parameter of type 'opt' associated to the detector

   if (GetSize() < 2)
      return 0.0;

   if (strcmp(opt, "")) {
      return GetDetector(2)->GetACQData(opt);
   }
   return GetDetector(2)->GetEnergy();
}

Double_t KVIDTelescope::GetPedestalX(Option_t * opt)
{
   // Returns the pedestal associated with the 2nd detector of the telescope,
   // optionally depending on the given option string.
   // By default this returns 0, and should be overridden in specific implementations.

   return 0.;
}

Double_t KVIDTelescope::GetPedestalY(Option_t * opt)
{
   // Returns the pedestal associated with the 1st detector of the telescope,
   // optionally depending on the given option string.
   // By default this returns 0, and should be overridden in specific implementations.

   return 0.;
}

//____________________________________________________________________________________

Double_t KVIDTelescope::GetIDMapY(Option_t * opt)
{
   //This method should define how to calculate the Y-coordinate in a 2D identification map
   //associated with the ID telescope based on the current state of the detectors.
   //By default, we assume a dE - E telescope, and so 'Y' is equal to the energy measured in the
   //first detector of the telescope.
   //
   //However, if the detector is not calibrated, GetEnergy will return 0, therefore
   //if 'opt' is not an empty string (default) we return the value of the acquisition
   //parameter of type 'opt' associated to the detector
   if (strcmp(opt, "")) {
      return GetDetector(1)->GetACQData(opt);
   }
   return GetDetector(1)->GetEnergy();
}

//____________________________________________________________________________________

void KVIDTelescope::RemoveGrids()
{
   //Remove all identification grids for this ID telescope
   //Grids are not deleted as this is handled by gIDGridManager
   fIDGrids->Clear();
}

//____________________________________________________________________________________

KVIDTelescope *KVIDTelescope::MakeIDTelescope(const Char_t * uri)
{
   //Static function which will create an instance of the KVIDTelescope-derived class
   //corresponding to 'name'
   //These are defined as 'Plugin' objects in the file $KVROOT/KVFiles/.kvrootrc :
// # The KVMultiDetArray::GetIDTelescopes(KVDetector*de, KVDetector*e) method uses these plugins to
// # create KVIDTelescope instances adapted to the specific array geometry and detector types.
// # For each pair of detectors we look for a plugin with one of the following names:
// #    [name_of_dataset].de_detector_type[de detector thickness]-e_detector_type[de detector thickness]
// # Each characteristic in [] brackets may or may not be present in the name; first we test for names
// # with these characteristics, then all combinations where one or other of the characteristics is not present.
// # In addition, we first test all combinations which begin with [name_of_dataset].
// # The first plugin found in this order will be used.
// # In addition, if for one of the two detectors there is a plugin called
// #    [name_of_dataset].de_detector_type[de detector thickness]
// #    [name_of_dataset].e_detector_type[e detector thickness]
// # then we add also an instance of this 1-detector identification telescope.

   //check and load plugin library
   TPluginHandler *ph;
   if (!(ph = LoadPlugin("KVIDTelescope", uri)))
      return 0;

   //execute constructor/macro for identification telescope
   KVIDTelescope *mda = (KVIDTelescope *) ph->ExecPlugin(0);
   if(mda){
      //set label of telescope with URI used to find plugin (minus dataset name)
      mda->SetLabelFromURI(uri);
   }

   return mda;
}

//____________________________________________________________________________________

void KVIDTelescope::SetLabelFromURI(const Char_t* uri)
{
   //PRIVATE METHOD
   //Sets label of telescope based on URI of plugin describing child class for this telescope

   TString _uri(uri);
   if(_uri.BeginsWith( gDataSet->GetName() )) _uri.Remove(0, strlen(gDataSet->GetName())+1);
   SetLabel(_uri.Data());
}

//____________________________________________________________________________________

Bool_t KVIDTelescope::SetIdentificationParameters(const KVMultiDetArray* MDA)
{
   // Initialise the identification parameters (grids, etc.) of ALL identification telescopes of this
   // kind (label) in the multidetector array. Therefore this method need only be called once, and not
   // called for each telescope. The kind/label (returned by GetLabel) of the telescope corresponds
   // to the URI used to find the plugin class in $KVROOT/KVFiles/.kvrootrc.
   // By default, this method looks for the file with name given by the environment variable
   //
   // [dataset name].IdentificationParameterFile.[telescope label]:       [filename]
   //
   // which is assumed to contain identification grids. The file will be read in by gIDGridManager
   // and the grids added to its list.

   TString filename = gDataSet->GetDataSetEnv( Form("IdentificationParameterFile.%s",GetLabel()) );
   if( filename == "" ){
      Warning("SetIdentificationParameters",
            "No filename defined. Should be given by %s.IdentificationParameterFile.%s",
            gDataSet->GetName(), GetLabel());
      return kFALSE;
   }
   TString path;
   if (!SearchKVFile(filename.Data(), path, gDataSet->GetName())){
      Error("SetIdentificationParameters",
            "File %s not found. Should be in $KVROOT/KVFiles/%s",
            filename.Data(), gDataSet->GetName());
      return kFALSE;
   }
   //read grids from file
   Info("SetIdentificationParameters", "Using file %s", path.Data());
   gIDGridManager->ReadAsciiFile(path.Data());
   return kTRUE;
}

//______________________________________________________________________________

void KVIDTelescope::Streamer(TBuffer &R__b)
{
   // Stream an object of class KVIDTelescope.
   // Handles backwards compatibility for objects with version number < 3
   // for which a single KVIDGrid was associated with the telescope instead
   // of a list of grids.

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v < 4) {
         KVBase::Streamer(R__b);
         //ID telescopes previous to V.4 had no label corresponding to plugin URI
         //here we add the label if it is missing
         if(!strcmp(GetLabel(),"")) {
            SetLabelFromURI( GetPluginURI("KVIDTelescope", ClassName()) );
         }
         KVIDSubCodeManager::Streamer(R__b);
         fDetectors->Streamer(R__b);
         R__b >> fGroup;
         KVIDGrid* fIDGrid=0;
         R__b >> fIDGrid;
         //clear any previous grids from list and add the read-in grid as sole member
         fIDGrids->Clear();
         if(fIDGrid){
            fIDGrids->Add(fIDGrid);
         }
         R__b.CheckByteCount(R__s, R__c, KVIDTelescope::IsA());
      }
      else
      {
         R__b.ReadClassBuffer(KVIDTelescope::Class(), this, R__v, R__s, R__c);
      }
   } else {
      R__b.WriteClassBuffer(KVIDTelescope::Class(), this);
   }
}

//______________________________________________________________________________

void KVIDTelescope::RemoveIdentificationParameters()
{
   //Remove identification parameters from telescope in such a way that they
   //can subsequently be reset e.g. with a new version.
   //This is used by KVMultiDetArray::UpdateIdentifications.
   //Child classes with specific SetIdentificationParameters methods should
   //also redefine this method in order to remove (destroy) cleanly the objects
   //created in SetIdentificationParameters.
   //
   //This default method takes the list of grids associated to the telescope,
   //and for each one: 1) checks if it is still in the gIDGridManager's list
   //2) if yes, delete the grid and remove it from gIDGridManager

   TIter next_grid( fIDGrids );
   KVIDGrid* grid;
   while( (grid = (KVIDGrid*)next_grid()) ){

      if ( gIDGridManager->GetGrids()->FindObject( grid ) ){//this only works if KVIDTelescope uses TObject:IsEqual method (i.e. compares pointers)

         gIDGridManager->DeleteGrid( grid );

      }
   }
   //clear list of grids
   fIDGrids->Clear();
   SetHasMassID(kFALSE);
}

//____________________________________________________________________________________

void KVIDTelescope::CalculateParticleEnergy(KVReconstructedNucleus * nuc)
{
   // The energy of each particle is calculated as follows:
   //
   //      E = dE_1 + dE_2 + ... + dE_N
   //
   // dE_1, dE_2, ... = energy losses measured in each detector through which
   //                          the particle has passed (or stopped, in the case of dE_N).
   //                         These energy losses are corrected for (Z,A)-dependent effects
   //                          such as pulse-heigth defect in silicon detectors, losses in
   //                          windows of gas detectors, etc.
   //
   // Whenever possible, the energy loss for fired detectors which are uncalibrated
   // or not functioning is calculated. In this case the status returned by GetCalibStatus()
   // will be KVIDTelescope::kCalibStatus_Calculated.
   // If none of the detectors is calibrated, the particle's energy cannot be calculated &
   // the status will be KVIDTelescope::kCalibStatus_NoCalibrations.
   // Otherwise, the status code will be KVIDTelescope::kCalibStatus_OK.

   //status code
   fCalibStatus = kCalibStatus_NoCalibrations;

   UInt_t z = nuc->GetZ();
   //uncharged particles
   //if(z==0) return;

   KVDetector* d1 = GetDetector(1);
   KVDetector* d2 = (fDetectors->GetSize()>1 ? GetDetector(2) : 0);
   Bool_t d1_cal = d1->IsCalibrated();
   Bool_t d2_cal = (d2 ? d2->IsCalibrated() : kFALSE);

   //no calibrations
   if (!d1_cal && !d2)
      return;
   if((d1&&d2) && !d1_cal && !d2_cal)
      return;

   //status code
   fCalibStatus = kCalibStatus_OK;

   UInt_t a = nuc->GetA();

   // particles stopped in first member of telescope
   if( nuc->GetStatus() == 3 ) {
      if(d1_cal){
         nuc->SetEnergy( d1->GetCorrectedEnergy(z, a, -1, kFALSE) );//N.B.: transmission=kFALSE because particle stop in d1
         //set angles from the dimensions of the telescope in which particle detected
         //N.B. should use dimensions of stopping detector, but this could be inconsistent
         //with the association of this ID telescope as the "identifying telescope" of the particle
         nuc->GetAnglesFromTelescope();
      }
      return;
   }

   Double_t e1, e2, einc;
   e1 = e2 = einc = 0.0;

   if (!d1_cal) {//1st detector not calibrated - calculate from residual energy in 2nd detector

      //second detector must exist and have all acquisition parameters fired with above-pedestal value
      if(d2 && d2->Fired("Pall")) e2 = d2->GetCorrectedEnergy(z, a, -1, kFALSE);//N.B.: transmission=kFALSE because particle stop in d2
      if( e2 <= 0.0 ){
         // zero energy loss in 2nd detector ? can't do anything...
         fCalibStatus = kCalibStatus_NoCalibrations;
         return;
      }
      //calculate & set energy loss in dE detector
      //N.B. using e2 for the residual energy after detector 1 means
      //that we are assuming the particle stops in detector 2.
      //if this is not true, we will underestimate the energy of the particle.
      e1 = d1->GetDeltaEFromERes(z,a,e2);
      if( e1< 0.0 ) e1 = 0.0;
      else{
         d1->SetEnergyLoss(e1);
         e1 = d1->GetCorrectedEnergy(z,a);
         //status code
         fCalibStatus = kCalibStatus_Calculated;
      }
   } else {//1st detector is calibrated too: get corrected energy loss

      e1 = d1->GetCorrectedEnergy(z, a);

   }

   if (d2 && !d2_cal) {//2nd detector not calibrated - calculate from energy loss in 1st detector

      e1 = d1->GetCorrectedEnergy(z, a);
      if( e1 <= 0.0 ){
         // zero energy loss in 1st detector ? can't do anything...
         fCalibStatus = kCalibStatus_NoCalibrations;
         return;
      }
      //calculate & set energy loss in 2nd detector
      e2 = d1->GetEResFromDeltaE(z,a);
      if( e2< 0.0 ) e2 = 0.0;
      else{
         e2 = d2->GetDeltaE(z,a,e2);
         d2->SetEnergyLoss(e2);
         e2 = d2->GetCorrectedEnergy(z,a);
         //status code
         fCalibStatus = kCalibStatus_Calculated;
      }
   }
   else if(d2){//2nd detector is calibrated too: get corrected energy loss

      e2 = d2->GetCorrectedEnergy(z, a, -1, kFALSE);//N.B.: transmission=kFALSE because particle assumed to stop in d2

   }

   //incident energy of particle (before 1st member of telescope)
   einc = e1 + e2;

   Double_t coherence_tolerance = gEnv->GetValue("KVIDTelescope.CoherencyTolerance", 1.05);
   if(coherence_tolerance < 1) coherence_tolerance += 1.00;

   //Now we have to work our way up the list of detectors from which the particle was
   //reconstructed. For each fired & calibrated detector which is only associated with
   //one particle in the events, we add the corrected measured energy loss
   //to the particle. For uncalibrated, unfired detectors and detectors through which
   //more than one particle has passed, we calculate the corrected energy loss and add it
   //to the particle.
   int ndets = nuc->GetNumDet();
   if (ndets > (int)GetSize()) { //particle passed through other detectors before this idtelesocpe
      //look at detectors not in this id telescope
      int idet = GetSize();//next detector after delta-e member of IDTelescope (stopping detector = 0)
      while (idet < ndets) {

         KVDetector *det = nuc->GetDetector(idet);
         if( det->Fired() && det->IsCalibrated() && det->GetNHits() == 1 ){
            Double_t dE = det->GetEnergy();
            //in order to check if particle was really the only one to
            //hit each detector, we calculate the particle's energy loss
            //from its residual energy. if the measured energy loss is
            //significantly larger, there may be a second particle.
            e1 = det->GetDeltaEFromERes(z,a,einc);
            if( e1< 0.0 ) e1 = 0.0;
            dE = det->GetCorrectedEnergy(z,a);
            einc += dE;
         }
         else{
            // Uncalibrated/unfired/multihit detector. Calculate energy loss.
            //calculate energy of particle before detector from energy after detector
            e1 = det->GetDeltaEFromERes(z,a,einc);
            if( e1< 0.0 ) e1 = 0.0;
            if( det->GetNHits() > 1 ){
               //Info("CalculateParticleEnergy",
                 //    "Detector %s was hit by %d particles. Calculated energy loss for particle %f MeV",
                 //    det->GetName(), det->GetNHits(), e1);
               if(!( det->Fired() && det->IsCalibrated()) )
					{
                  det->SetEnergyLoss(e1 + det->GetEnergy());// sum up calculated energy losses in uncalibrated detector
               }
               //status code
               fCalibStatus = kCalibStatus_Multihit;
            }
            else if( !det->Fired() || !det->IsCalibrated() ){
               //Info("CalculateParticleEnergy",
                 //    "Detector %s uncalibrated/not fired. Calculated energy loss for particle %f MeV",
                 //    det->GetName(), e1);
               det->SetEnergyLoss(e1);
               //status code
               fCalibStatus = kCalibStatus_Calculated;
            }
            e1 = det->GetCorrectedEnergy(z,a,e1);
            einc += e1;
         }
         idet++;
      }
   }
   //einc is now the energy of the particle before crossing the first detector
   nuc->SetEnergy(einc);
   //set angles from the dimensions of the telescope in which particle detected
   nuc->GetAnglesFromTelescope();
}

//_____________________________________________________________________________________________________//

const Char_t* KVIDTelescope::GetDefaultIDGridClass()
{
	// Returns name of default ID grid class for this ID telescope.
	// This is defined in a .kvrootrc configuration file by one of the following:
	// KVIDTelescope.DefaultGrid:
	// KVIDTelescope.DefaultGrid.[type]:
	// where [type] is the type of this identification telescope (which is given
	// by the character string returned by method GetLabel()... sorry :( )
	// If no default grid is defined for the specific type of this telescope,
	// the default defined by KVIDTelescope.DefaultGrid is used.

	TString general = gEnv->GetValue("KVIDTelescope.DefaultGrid", "KVIDGraph");
	TString specific; specific.Form("KVIDTelescope.DefaultGrid.%s", GetLabel());
	return gEnv->GetValue(specific.Data(), general.Data());
}

//_____________________________________________________________________________________________________//

KVIDGrid* KVIDTelescope::CalculateDeltaE_EGrid(const Char_t* Zrange,Int_t deltaMasse,Int_t npoints)
{

	if (GetSize()<=1) return 0;

	KVNumberList nlz(Zrange);

	TClass* cl = new TClass(GetDefaultIDGridClass());
	KVIDGrid* idgrid = (KVIDGrid* )cl->New();
	delete cl;

	idgrid->AddIDTelescope(this);
	idgrid->SetOnlyZId((deltaMasse!=0));

	KVDetector* det_de = GetDetector(1);	if (!det_de)		return 0;
	KVDetector* det_eres = GetDetector(2);	if (!det_eres) 	return 0;

	KVNucleus part;
	Info("SimulateGrid",
        "Calculating dE-E grid: dE detector = %s, E detector = %s",
        det_de->GetName(), det_eres->GetName());

	KVIDLine *B_line = (KVIDLine *)idgrid->Add("OK", "KVIDCutLine");
   Int_t npoi_bragg = 0;
	B_line->SetName("Bragg_line");

   nlz.Begin(); while (!nlz.End()){
		Int_t zzz = nlz.Next();
		part.SetZ(zzz);
		Int_t aref = part.GetAWithMaxBindingEnergy(zzz);
		printf("%d\n",zzz);
		for (Int_t aaa=aref-deltaMasse; aaa<=aref+deltaMasse; aaa+=1){
      	part.SetA(aaa);
			printf("+ %d %d %d\n",aaa,aref,part.IsKnown());
			if (part.IsKnown()){

				//loop over energy
      		//first find :
      		//      ****E1 = energy at which particle passes ChIo and starts to enter Si****
      		//      E2 = energy at which particle passes Si
      		//then perform npoints calculations between these two energies and use these
      		//to construct a KVIDZLine

      		Double_t E1, E2;
      		//find E1
      		//go from 0.1 MeV to chio->GetBraggE(part.GetZ(),part.GetA()))
      		Double_t E1min = 0.1, E1max = det_de->GetBraggE(zzz,aaa);
      		E1 = (E1min + E1max) / 2.;

				while ((E1max - E1min) > 0.1) {
					//printf("1ere iteration %lf>0.1\n",E1max - E1min);
         		part.SetEnergy(E1);
         		det_de->Clear();
					det_eres->Clear();

         		det_de->DetectParticle(&part);
         		det_eres->DetectParticle(&part);
         		if (det_eres->GetEnergy() > .1) {
            		//particle got through - decrease energy
            		E1max = E1;
            		E1 = (E1max + E1min) / 2.;
         		} else {
            		//particle stopped - increase energy
            		E1min = E1;
            		E1 = (E1max + E1min) / 2.;
         		}
      		}

      		//add point to Bragg line
      		Double_t dE_B = det_de->GetBraggDE(zzz, aaa);
      		Double_t E_B = det_de->GetBraggE(zzz, aaa);
      		Double_t Eres_B = det_de->GetERes(zzz, aaa, E_B);
      		B_line->SetPoint(npoi_bragg++, Eres_B, dE_B);

      		//find E2
      		//go from E1 MeV to 6000 MeV
      		Double_t E2min = E1, E2max = 6000;
      		E2 = (E2min + E2max) / 2.;

      		while ((E2max - E2min > 0.1)) {
					//printf("2ere iteration %lf>0.1\n",E2max - E2min);
         		part.SetEnergy(E2);
         		det_de->Clear();
					det_eres->Clear();

					det_de->DetectParticle(&part);
         		det_eres->DetectParticle(&part);
         		if (part.GetEnergy() > .1) {
            		//particle got through - decrease energy
            		E2max = E2;
            		E2 = (E2max + E2min) / 2.;
         		} else {
            		//particle stopped - increase energy
            		E2min = E2;
            		E2 = (E2max + E2min) / 2.;
         		}
      		}

      		cout << "Z=" << zzz << " E1 = " << E1 << " E2 = " << E2 << endl;
      		KVIDZALine *line = (KVIDZALine *)idgrid->Add("ID", "KVIDZALine");
      		if (TMath::Even(zzz)) line->SetLineColor(4);
				line->SetZ(zzz);
				line->SetA(aaa);

      		Double_t logE1 = TMath::Log(E1);
      		Double_t logE2 = TMath::Log(E2);
      		Double_t dLog = (logE2 - logE1) / (npoints - 1.);

      		for (Int_t i = 0; i < npoints; i++) {
//                      Double_t E = E1 + i*(E2-E1)/(npoints-1.);
         		Double_t E = TMath::Exp(logE1 + i * dLog);
//					printf("points %d sur %d - %lf\n",i,npoints,E);

         		Double_t Eres = 0.;
         		Int_t niter=0;
					while (Eres < 0.1 && niter<=20) {
            		det_de->Clear();
            		det_eres->Clear();

						part.SetEnergy(E);

						det_de->DetectParticle(&part);
            		det_eres->DetectParticle(&part);

						Eres = det_eres->GetEnergy();
            		E += 0.1;

						//printf("3eme iteration %lf<0.1 - %lf\n",Eres,E);
						niter+=1;
					}
         		if (!(niter>20)){
         			Double_t dE = det_de->GetEnergy();
         			//PHD correction
						line->SetPoint(i, Eres, dE);
      			}
				}
				//printf("sort de boucle points");
			}
		}
   }

	return idgrid;

}

//_________________________________________________________________________________________

Double_t KVIDTelescope::GetMeanDEFromID(Int_t &status, Int_t Z, Int_t A)
{
	// Returns the Y-axis value in the 2D identification map containing isotope (Z,A)
	// corresponding to the current X-axis value given by GetIDMapX.
	// If no mass information is available, just give Z.
	//
	// In this (default) implementation this means scanning the ID grids associated with
	// this telescope until we find and identification line Z or (Z,A), and then interpolating
	// the Y-coordinate for the current X-coordinate value.
	//
	// Status variable can take one of following values:
	//
    //  KVIDTelescope::kMeanDE_OK                    all OK
    //   KVIDTelescope::kMeanDE_XtooSmall      X-coordinate is smaller than smallest X-coordinate of ID line
    //   KVIDTelescope::kMeanDE_XtooLarge     X-coordinate is larger than largest X-coordinate of ID line
    //   KVIDTelescope::kMeanDE_NoIdentifie    No identifier found for Z or (Z,A)

    status = kMeanDE_OK;
	// loop over grids
	TIter next(fIDGrids);
	KVIDGrid* grid;
	KVIDLine *idline=0;
	while( (grid = (KVIDGrid*)next()))
	{
	    idline = (KVIDLine*)grid->GetIdentifier(Z,A);
	    if(idline) break;
	}
	if(!idline) {
	    status = kMeanDE_NoIdentifier;
	    return -1.;
	}
	Double_t x,x1,y1,x2,y2;
	x = GetIDMapX();
	idline->GetEndPoint(x2,y2);
	if(x>x2){
        status = kMeanDE_XtooLarge;
        return -1;
	}
	idline->GetStartPoint(x1,y1);
	if(x<x1){
        status = kMeanDE_XtooSmall;
        return -1.;
	}
	return idline->Eval(x);
}
