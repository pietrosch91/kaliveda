/***************************************************************************
$Id: KVDetector.cpp,v 1.87 2009/03/03 14:27:15 franklan Exp $
                          kvdetector.cpp  -  description
                             -------------------
    begin                : Thu May 16 2002
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
#include "TRandom3.h"
#include "TBuffer.h"
#include "KVDetectorBrowser.h"
#include "KVModule.h"
#include "KVGroup.h"
#include "KVTelescope.h"
#include "KVIDTelescope.h"
#include "KVCalibrator.h"
#include "KVACQParam.h"
#include "KVMultiDetArray.h"
#include "KVDetector.h"
#include "TPluginManager.h"
#include "TObjString.h"
#include "KVDataBranchHandler.h"
                /*** geometry ***/
#include "TGeoVolume.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

ClassImp(KVDetector)
///////////////////////////////////////////////////////////////////////////////////////
//Base class for the description of detectors in multidetector arrays.
//
//Begin_Html
//<img src="images/kvdetector_structure.gif">
//End_Html
//
//A detector is composed of one or more absorber layers (KVMaterial objects) in which the energy loss of charged particles can be calculated. One of these layers
//is set as "active" (by default it is the last added layer) which means that only the energy loss in this layer can actually be "read", e.g. an ionisation chamber is composed of an
//"active" gas layer sandwiched between two "inactive" windows :
//
//      KVDetector chio("Myl",2.5);                                             //first layer - 2.5 micron mylar window
//      KVMaterial *gas = new KVMaterial("C3F8",50.0);
//      chio.AddAbsorber(gas);                                                  //second layer - 50mm of C3F8
//      chio.SetActiveLayer(gas);                                               //make gas layer "active"
//      gas->SetPressure( 50 );                                                  //set pressure of gas to 50mbar
//      KVMaterial *win = new KVMaterial("Myl",2.5);    //exit window
//      chio.AddAbsorber(win);
//(This is how the INDRA ionisation chambers are built in class KVChIo).
//
//A detector is created either with the constructor taking the material type as argument:
//      KVDetector det("Si");
//or using SetMaterial:
//      KVDetector det;
//      det.SetMaterial("Si");
//
//For available material types see KVMaterial.
//
//Set the "thickness" of the detector:
//___________________________________________
//      det.SetThickness(300);
//or in the constructor:
//      KVDetector det("Si",300.0);
//
//Calculate the energy loss of a charged particle in a detector:
//_____________________________________________________________________
//Two methods are available: one simply calculates the energy lost by the particle
//in the detector, but does not modify either the particle or the detector (GetELostByParticle);
//the other simulates the passage of the particle through the detector, the particle's energy
//is reduced by the amount lost in the detector's absorbers and the total energy lost in the
//detector is increased, e.g.:
//      KVNucleus alpha(2,4);           //an alpha-particle
//      alpha.SetEnergy(100);           //with 100MeV kinetic energy
//      det.DetectParticle(&alpha);     //simulate passage of particle in the detector/target
//      det.GetEnergy();                        //energy lost by particle in detector/target
//      alpha.GetEnergy();                      //residual energy of particle
//      det.Clear();                            //reset detector ready for a new detection
//
//WARNING: KVMaterial methods not explicitely redefined here act only on the
//active absorber in the detector. It is the case for e.g. KVMaterial::GetIncidentEnergy
//which does not take into account any absorbers placed in front of the active layer.
void KVDetector::init()
{
   //default initialisations
   fModules = 0;
   fCalibrators = 0;
   fBrowser = 0;
   fACQParams = 0;
   fParticles = 0;
   fSegment = 0;
   fGain = 1.;
   fCalWarning = 0;
   fAbsorbers = new KVList;
   fActiveLayer = 0;
   fTelescope = 0;
   fIDTelescopes = new KVList(kFALSE);
   fIDTelescopes->SetCleanup(kTRUE);
   fIDTelAlign = new KVList(kFALSE);
   fIDTelAlign->SetCleanup(kTRUE);
   fIDTele4Ident=0;
   fReanalyse = kFALSE;
   fECalc = 0.0;
   fIdentP = fUnidentP = 0;
   npar_loss = npar_res = 0;
   par_loss = par_res = 0;
	fTotThickness = 0.;
	fDepthInTelescope = 0.;
	fFiredMask.Set("0");
}

KVDetector::KVDetector()
{
//default ctor
   init();
}

//________________________________________________________________________________________
KVDetector::KVDetector(const Char_t * type,
                       const Float_t thick):KVMaterial()
{
   //Create a new detector of a given material and thickness (default value = 0.0)

   init();
   SetType("DET");
   AddAbsorber(new KVMaterial(type, thick));
}

//_______________________________________________________________
KVDetector::KVDetector(const KVDetector & obj)
{
//copy ctor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   obj.Copy(*this);
#else
   ((KVDetector &) obj).Copy(*this);
#endif
}

//_______________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVDetector::Copy(TObject & obj) const
#else
void KVDetector::Copy(TObject & obj)
#endif
{
   //copy 'this' to 'obj'
   //The structure of the detector is copied, with new cloned objects for
   //each absorber layer. The active layer is set in the new detector.

   TIter next(fAbsorbers);
   KVMaterial *mat;
   while ((mat = (KVMaterial *) next())) {
      ((KVDetector &) obj).AddAbsorber((KVMaterial *) mat->Clone());
   }
   //set active layer
   ((KVDetector &) obj).SetActiveLayer(fActiveLayer);
}

//______________________________________________________________________________
void KVDetector::Streamer(TBuffer & R__b)
{
   // Customised streamer for backwards compatibility with multidetectors written to file
   // before use of gROOT->GetListOfCleanups()
   // Calls SetFiredBitmask after reading detectors with version < 7

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      KVDetector::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
      if (R__v < 6) {
         {
            fIDTelescopes->SetCleanup(kTRUE);
         }
         {
            fIDTelAlign->SetCleanup(kTRUE);
         }
      }
      if (R__v < 7) {
         // 'fired' bitmask added in version 7. 
         SetFiredBitmask();
      }
   } else {
      KVDetector::Class()->WriteBuffer(R__b, this);
   }
}

//_______________________________________________________________
KVDetector::~KVDetector()
{
   if (fIDTelescopes && fIDTelescopes->TestBit(kNotDeleted)) {
      //all ID telescopes to which this detector belonged now cease to exist.
      fIDTelescopes->Delete();
      delete fIDTelescopes;
   }
   fIDTelescopes = 0;

   SafeDelete(fModules);
   SafeDelete(fCalibrators);
   SafeDelete(fBrowser);
   SafeDelete(fParticles);
   delete fAbsorbers;

   SafeDelete(fACQParams);
   init();
   fActiveLayer = -1;
   if (fIDTelAlign && fIDTelAlign->TestBit(kNotDeleted)) {
      fIDTelAlign->Clear();
      delete fIDTelAlign;
   }
   fIDTelAlign = 0;
   if(par_loss) delete [] par_loss;
   if(par_res) delete [] par_res;
   SafeDelete(fIDTele4Ident);
}

//________________________________________________________________
void KVDetector::SetMaterial(const Char_t * type)
{
   //Set material of active layer.
   //If no absorbers have been added to the detector, create and add
   //one (active layer by default)
   if (!GetActiveLayer())
      AddAbsorber(new KVMaterial(type));
   else
      GetActiveLayer()->SetMaterial(type);
}

//________________________________________________________________
void KVDetector::DetectParticle(KVNucleus * kvp, TVector3 * norm)
{
   //Calculate the energy loss of a charged particle traversing the detector,
   //the particle is slowed down, it is added to the list of all particles hitting the
   //detector.
   //Do nothing if particle has zero (or -ve) energy.
   //
   //If the optional argument 'norm' is given, it is supposed to be a vector
   //normal to the detector, oriented from the origin towards the detector.
   //In this case the effective thicknesses of the detector's absorbers 'seen' by the particle
   //depending on its direction of motion is used for the calculation.

   if (kvp->GetKE() <= KVDETECTOR_MINIMUM_E)
      return;

   AddHit(kvp);                 //add nucleus to list of particles hitting detector in the event

   //composite detector - calculate losses in all layers
   KVMaterial *abs;
   TIter next(fAbsorbers);
   while ((abs = (KVMaterial *) next())
          && kvp->GetKE() > KVDETECTOR_MINIMUM_E) {
      abs->DetectParticle(kvp, norm);
   }
}

//_______________________________________________________________________________

Double_t KVDetector::GetELostByParticle(KVNucleus * kvp, TVector3 * norm)
{
   //Calculate the energy loss of a charged particle traversing all layers of the detector,
   //returning only the energy lost in the "active" layer. This does not affect the "stored"
   //energy loss value of the detector, nor its ACQData, nor the energy of the particle.
   //
   //If the optional argument 'norm' is given, it is supposed to be a vector
   //normal to the detector, oriented from the origin towards the detector.
   //In this case the effective thicknesses of the detector's absorbers 'seen' by the particle
   //depending on its direction of motion is used for the calculation.

   Double_t eloss, ElossActive = 0.0;
   //make 'clone' of particle
   KVNucleus* clone_part = new KVNucleus(kvp->GetZ(), kvp->GetA());
   clone_part->SetMomentum(kvp->GetMomentum());
   if (clone_part->GetEnergy() > KVDETECTOR_MINIMUM_E) {
      //composite detector - calculate losses in all layers
      KVMaterial *abs; int i=0;
      TIter next(fAbsorbers);
      while ((abs = (KVMaterial *) next())
          && clone_part->GetKE() > KVDETECTOR_MINIMUM_E) {
            eloss = abs->GetELostByParticle(clone_part, norm);
            if (i++ == fActiveLayer)
               ElossActive = eloss;
            clone_part->SetKE(clone_part->GetKE() - eloss);
      }
   }
   //delete clone
   delete clone_part;
   return ElossActive;
}

//_______________________________________________________________________________

Double_t KVDetector::GetParticleEIncFromERes(KVNucleus * kvp, TVector3 * norm)
{
   //Calculate the energy of particle 'kvn' before its passage through the detector,
   //based on the current kinetic energy, Z & A of nucleus 'kvn', supposed to be
   //after passing through the detector.
   //
   //If the optional argument 'norm' is given, it is supposed to be a vector
   //normal to the detector, oriented from the origin towards the detector.
   //In this case the effective thicknesses of the detector's absorbers 'seen' by the particle
   //depending on its direction of motion is used for the calculation.

   Double_t Einc = 0.0;
   //make 'clone' of particle
   KVNucleus* clone_part = new KVNucleus(kvp->GetZ(), kvp->GetA());
   clone_part->SetMomentum(kvp->GetMomentum());
      //composite detector - calculate losses in all layers
      KVMaterial *abs;
      TIter next(fAbsorbers, kIterBackward); //work through list backwards
      while ((abs = (KVMaterial *) next())) {

         //calculate energy of particle before current absorber
            Einc = abs->GetParticleEIncFromERes(clone_part, norm);

            //set new energy of particle
            clone_part->SetKE(Einc);
      }
   //delete clone
   delete clone_part;
   return Einc;
}

//_______________________________________________________________________________

void KVDetector::ConnectModule(KVModule * p, const int fcon)
{
//
//Associate an electronic acquisition or control module to the detector.
//(Not Yet Implemented)
//
   if (!fModules) {
      fModules = new KVList();
      fModules->SetOwner(kFALSE);
   }
   fModules->Add(p);
   if (fcon == KVD_RECPRC_CNXN)
      p->ConnectDetector(this, KVD_NORECPRC_CNXN);
}


//__________________________________________________________________________________
void KVDetector::Print(Option_t * opt) const
{
   //Print info on this detector
   //if option="data" the energy loss and coder channel data are displayed

   if (!strcmp(opt, "data")) {
      cout << ((KVDetector *) this)->
          GetName() << " -- E=" << ((KVDetector *) this)->
          GetEnergy();
      if(Reanalyse()) cout << " (" << GetECalc() << ")";
      cout << "  ";
      TIter next(fACQParams);
      KVACQParam *acq;
      while ((acq = (KVACQParam *) next())) {
         cout << acq->GetName() << "=" << (Short_t) acq->
             GetCoderData();
         if(Reanalyse()) cout << " (" << GetCalcACQParam(acq) << ")";
         cout << "  ";
      }
      if (Reanalyse())cout << " ---> UP FOR REANALYSIS";
      cout << endl;
      if (BelongsToUnidentifiedParticle())
         cout << "(Belongs to an unidentified particle)" << endl;
   } else if (!strcmp(opt, "all")) {
      //Give full details of detector
      //
      TString option("    ");
      cout << option << ClassName() << " : " << ((KVDetector *) this)->
          GetName() << endl;
      //composite detector - print all layers
      KVMaterial *abs;
      TIter next(fAbsorbers);
      while ((abs = (KVMaterial *) next())) {
         if (GetActiveLayer() == abs)
            cout << " ### ACTIVE LAYER ### " << endl;
         cout << option << option;
         abs->Print();
         if (GetActiveLayer() == abs)
            cout << " ### ACTIVE LAYER ### " << endl;
      }
      cout << option << "Gain:      " << GetGain() << endl;
      if (fParticles) {
         cout << option << " --- Detected particles:" << endl;
         fParticles->Print();
      }
      if(fIDTelescopes){
         cout << option << " --- Detector belongs to the following Identification Telescopes:" << endl;
         fIDTelescopes->ls();
      }
      if(fIDTelAlign){
         cout << option << " --- Identification Telescopes in front of detector:" << endl;
         fIDTelAlign->ls();
      }
      if(fIDTele4Ident){
         cout << option << " --- Identification Telescopes used to identify particles stopping in this detector:" << endl;
         fIDTele4Ident->ls();
      }
   } else {
      //just print name
      cout << ClassName() << ": " << ((KVDetector *) this)->
          GetName() << endl;
   }
}


//___________________________________________________________________________________

const Char_t *KVDetector::GetArrayName()
{
   // This method is called by KVMultiDetArray::MakeListOfDetectors
	// after the array geometry has been defined (i.e. all detectors have
	// been placed in the array). The string returned by this method
	// is used to set the name of the detector.
	//
	// Override this method in child classes in order to define a naming
	// convention for specific detectors of the array.
	//
	// By default we return an empty string.

   fFName = "";
   return fFName.Data();
}


//___________________________________________________________________________________
Double_t KVDetector::GetEnergy()
{
//
//Returns energy lost in active layer by particles.
//
   if (GetActiveLayer())
      return GetActiveLayer()->GetEnergyLoss();
   return KVMaterial::GetEnergyLoss();
}

void KVDetector::SetEnergy(Double_t e)
{
//
//Set value of energy lost in active layer
//
   if (GetActiveLayer())
      GetActiveLayer()->SetEnergyLoss(e);
   KVMaterial::SetEnergyLoss(e);
}

//_____________________________________________________________________________________
UInt_t KVDetector::GetTelescopeNumber() const
{
    //The number of the detector's telescope in its ring.
   return (GetTelescope()? GetTelescope()->GetNumber() : 0);
}

//_____________________________________________________________________________________
Bool_t KVDetector::AddCalibrator(KVCalibrator * cal)
{
   //Associate a calibration with this detector.
   //If the calibrator object has the same class and type
   //as an existing object in the list (see KVCalibrator::IsEqual),
   //it will not be added to the list
   //(avoids duplicate calibrators) and the method returns kFALSE.

   if (!fCalibrators)
      fCalibrators = new KVList();
   if(fCalibrators->FindObject(cal)) return kFALSE;
   fCalibrators->Add(cal);
   return kTRUE;
}

//______________________________________________________________________________
void KVDetector::StartBrowser()
{
//
//open graphical configuration tool for this detector
//
   if (gROOT->IsBatch()) {
      fprintf(stderr, "Browser cannot run in batch mode\n");
   }
   if (!fBrowser)
      fBrowser = new KVDetectorBrowser(gClient->GetRoot(), this, 10, 10);
}

//______________________________________________________________________________
void KVDetector::CloseBrowser()
{
//
//close graphical configuration tool for this detector
//
   if (fBrowser) {
      Warning("CloseBrowser", "Closing browser for %s", GetName());
      delete fBrowser;
      fBrowser = 0;
   }
}

//_______________________________________________________________________________
void KVDetector::AddACQParam(KVACQParam* par)
{
   // Add given acquisition parameter to this detector.

   if (!fACQParams) {
      fACQParams = new KVList();
      fACQParams->SetName(Form("List of acquisition parameters for detector %s", GetName()));
   }
   par->SetDetector(this);
   fACQParams->Add(par);
}

//________________________________________________________________________________
KVACQParam *KVDetector::GetACQParam(const Char_t *name)
{
   // Look for acquisition parameter with given name in list
	// of parameters associated with this detector.

   if (!fACQParams) {
      return 0;
   }
   return ((KVACQParam *) fACQParams->FindObject(name));
}

//__________________________________________________________________________________
Float_t KVDetector::GetACQData(const Char_t * name)
{
   // Access acquisition data value associated to parameter with given name.
   // Returns value as a floating-point number which is the raw channel number
	// read from the coder plus a random number in the range [-0.5,+0.5].
   // If the detector has no DAQ parameter of the given type,
	// or if the raw channel number = 0, the value returned is -1.

   KVACQParam *par = GetACQParam(name);
   return (par ? par->GetData() :  -1.);
}

//__________________________________________________________________________________
Float_t KVDetector::GetPedestal(const Char_t *name)
{
   // Access pedestal value associated to parameter with given name.

   KVACQParam *par = GetACQParam(name);
   return (par ? par->GetPedestal() : 0);
}

//__________________________________________________________________________________
void KVDetector::SetPedestal(const Char_t *name, Float_t ped)
{
   // Set value of pedestal associated to parameter with given name.

   KVACQParam *par = GetACQParam(name);
   if (par) {
      par->SetPedestal(ped);
   }
}

//_______________________________________________________________

void KVDetector::Clear(Option_t * opt)
{
   //Set energy loss(es) etc. to zero

   SetAnalysed(kFALSE);
   SetReanalyse(kFALSE);
   fIdentP = fUnidentP = 0;
   ResetBit(kIdentifiedParticle);
   ResetBit(kUnidentifiedParticle);
   fECalc = 0.0;
   if (fACQParams) {
      TIter next(fACQParams);
      KVACQParam *par;
      while ((par = (KVACQParam *) next())) {
         par->Clear();
      }
   }
   if (fParticles) {
      fParticles->Clear();
   }
   //reset all layers in detector
   KVMaterial *mat;
   TIter next(fAbsorbers);
   while ((mat = (KVMaterial *) next())) {
      mat->Clear();
   }
}

//______________________________________________________________________________
Bool_t KVDetector::IsCalibrated() const
{
   //Returns true if the detector has been calibrated
   //i.e. if
   //  -  it has associated calibrators
   //  -  ALL of the calibrators are ready
   if (GetListOfCalibrators()) {
      TIter next(GetListOfCalibrators());
      KVCalibrator *cal;
      while ((cal = (KVCalibrator *) next())) {
         if (!cal->GetStatus())
            return kFALSE;
      }
   }
   return kTRUE;
}

void KVDetector::AddAbsorber(KVMaterial * mat)
{
   // Add a layer of absorber material to the detector
   // By default, the first layer added is set as the "Active" layer.
   // Call SetActiveLayer to change this.
   fAbsorbers->Add(mat);
   if (!TestBit(kActiveSet))
      SetActiveLayer((Short_t) (fAbsorbers->GetSize() - 1));
}

void KVDetector::SetActiveLayer(KVMaterial * mat)
{
   //Set reference to the "active" layer in the detector,
   //i.e. the one in which energy losses are measured
   //By default the active layer is the first layer added

   if( fAbsorbers->IndexOf(mat) > -1 ) SetActiveLayer((Short_t) (fAbsorbers->IndexOf(mat) ));
}

KVMaterial *KVDetector::GetActiveLayer() const
{
   //Get pointer to the "active" layer in the detector, i.e. the one in which energy losses are measured

   return GetAbsorber(fActiveLayer);
}

//_________________________________________________________________________________
void KVDetector::AddToTelescope(KVTelescope * T, const int fcon)
{
//Add this detector to a telescope.
//
   if (T) {
      SetTelescope(T);
      if (fcon == KVD_RECPRC_CNXN)
         T->AddDetector(this, KVD_NORECPRC_CNXN);
   } else {
      Warning("AddToTelescope", KVDETECTOR_ADD_TO_UNKNOWN_TELESCOPE);
   }
}

//_____________________________________________________________________________________
KVTelescope *KVDetector::GetTelescope() const
{
//The telescope to which the detector belongs.
//
   return fTelescope;
}

void KVDetector::SetTelescope(KVTelescope * kvt)
{
   //Set telescope to which detector belongs

   fTelescope = kvt;
}

KVMaterial *KVDetector::GetAbsorber(const Char_t* name) const
{
   // Return absorber with given name
   return (KVMaterial*)(fAbsorbers ? fAbsorbers->FindObject(name) : 0);
}

KVMaterial *KVDetector::GetAbsorber(Int_t i) const
{
   //Returns pointer to the i-th absorber in the detector (i=0 first absorber, i=1 second, etc.)

   if (i < 0) {
      //special case of reading old detectors
      //no warning
      return 0;
   }
   if (!fAbsorbers) {
      Error("GetAbsorber", "No absorber list");
      return 0;
   }
   if (fAbsorbers->GetSize() < 1) {
      Error("GetAbsorber", "Nothing in absorber list");
      return 0;
   }
   if (i >= fAbsorbers->GetSize()) {
      Error("GetAbsorber", "i=%d but only %d absorbers in list", i,
            fAbsorbers->GetSize());
      return 0;
   }

   return (KVMaterial *) fAbsorbers->At(i);
}

void KVDetector::SetACQParams()
{
   //Attribute acquisition parameters to this detector.
   //This method does nothing; it should be overridden in child classes to attribute
   //parameters specific to each detector.
   ;
}

void KVDetector::SetCalibrators()
{
   //Attribute calibrators to this detector.
   //This method does nothing; it should be overridden in child classes to attribute
   //parameters specific to each detector.
   ;
}

void KVDetector::RemoveCalibrators()
{
   //Removes all calibrations associated to this detector: in other words, we delete all
   //the KVCalibrator objects in list fCalibrators.
   if(fCalibrators) fCalibrators->Delete();
}

KVGroup *KVDetector::GetGroup() const
{
   //return pointer to KVGroup to which detector belongs
   if (GetTelescope())
      return GetTelescope()->GetGroup();
   else
      return 0;
};

Float_t KVDetector::GetTheta() const
{
   //get position of detector from parent telescope
   if (GetTelescope())
      return GetTelescope()->GetTheta();
   return 0.0;
}

Float_t KVDetector::GetPhi() const
{
   //get position of detector from parent telescope
   if (GetTelescope())
      return GetTelescope()->GetPhi();
   return 0.0;
}

//___________________________________________________________________________//

void KVDetector::AddIDTelescope(KVIDTelescope * idt)
{
   //Add ID telescope to list of telescopes to which detector belongs
   fIDTelescopes->Add(idt);
}

//___________________________________________________________________________//

KVList *KVDetector::GetIDTelescopes()
{
   //Return list of IDTelescopes to which detector belongs
   return fIDTelescopes;
}

//___________________________________________________________________________//

TList *KVDetector::GetTelescopesForIdentification()
{
   //Returns list of identification telescopes to be used in order to try to identify
   //particles stopping in this detector. This is the same as GetAlignedIDTelescopes
   //but only including the telescopes of which this detector is a member.
   if(fIDTele4Ident) return fIDTele4Ident;
   if(!fIDTelescopes || !fIDTelAlign) return 0;
   fIDTele4Ident=new TList;
   TIter next(fIDTelAlign); KVIDTelescope* idt;
   while( (idt = (KVIDTelescope*)next()) ){
      if( fIDTelescopes->FindObject(idt) ) fIDTele4Ident->Add(idt);
   }
   return fIDTele4Ident;
}

//___________________________________________________________________________//

KVList *KVDetector::GetAlignedIDTelescopes()
{
   //return list of ID telescopes made of this detector
   //and all aligned detectors placed in front of it
   return fIDTelAlign;
}

//___________________________________________________________________________//

void KVDetector::GetAlignedIDTelescopes(TCollection * list)
{
   //Create and add to list all ID telescopes made of this detector
   //and the aligned detectors placed in front of it.
   //
   //If list=0 then we store pointers to the ALREADY EXISTING ID telescopes
   //in fIDTelAlign. (first clear fIDTelAlign)


   TList *aligned = GetGroup()->GetAlignedDetectors(this);      //delete after use

   Bool_t list_zero = kFALSE;

   if (!list) {
      list_zero = kTRUE;
      //the IDTelescopes created will be destroyed at the end,
      //once we have used their names to find pointers to the corresponding
      //telescopes in gMultiDetArray
      list = new KVList;
      //clear any existing list of aligned telescopes
      fIDTelAlign->Clear();
   }
   //The following line is in case there are no detectors aligned
   //with 'this', but 'this' acts as an IDTelescope all by itself.
   //In this case we expect KVMultiDetArray::GetIDTelescopes
   //to define the appropriate ID telescope whenever one of the
   //two detector arguments (or both!) corresponds to 'this''s type.
   gMultiDetArray->GetIDTelescopes(this, this, list);

   if (aligned->GetSize() > 1) {
      //pairwise looping through list
      for (int i = 0; i < (aligned->GetSize() - 1); i++) {
         KVDetector *det1 = (KVDetector *) aligned->At(i + 1);
         KVDetector *det2 = (KVDetector *) aligned->At(i);

         gMultiDetArray->GetIDTelescopes(det1, det2, list);
      }
   }

   delete aligned;              //clean up

   if (list_zero) {
      //now we use the created ID telescopes to find pointers to the already
      //existing identical telescopes in gMultiDetArray, stock them in
      //fIDTelAlign
      TIter next_tel(list);
      KVIDTelescope *tel;
      while ((tel = (KVIDTelescope *) next_tel())) {
         KVIDTelescope *trash =
             gMultiDetArray->GetIDTelescope(tel->GetName());
         if (trash) {
            fIDTelAlign->Add(trash);
         }
      }
      //destroy the superfluous copy telescopes we just created
      list->Delete(); delete list;
   }
}

//______________________________________________________________________________//

Double_t KVDetector::GetCorrectedEnergy(UInt_t z, UInt_t a, Double_t e, Bool_t transmission)
{
   //This function should be redefined in specific detector child classes in order
   //to return the total energy loss in the detector for a given nucleus
   //including inactive absorber layers, and any particle-dependent correction to
   //the 'raw' energy calibration.
   //
   //If not redefined, this just returns the same as GetEnergy()

   if (e > 0.)
      return e;
   return GetEnergy();
}

//______________________________________________________________________________//

void KVDetector::SetECalc(Double_t epart)
{
   //Used in particle reconstruction.
   //If the primary identification of a particle passing through this detector
   //reveals an inconsistency between the measured energy loss and the calculated
   //energy loss of the particle, then this may mean that another particle
   //also passed through or stopped in this detector.
   //In this case the fECalc member should be set to the calculated energy loss
   //in the detector after subtraction of the contributions from already-identified
   //particles.
   //The "reanalysis" flag is set to true (SetReanalyse(kTRUE)).
   //A secondary identification procedure should then be used to identify particles
   //based on the calculated residual energy losses.
   //
   //"epart" is the calculated energy loss of an identified particle in the active layer
   //of this detector. This contribution is subtracted from the measured energy loss
   //in the detector (or from the calculated residual energy loss fECalc if SetECalc
   //has already been called i.e. if Reanalyse() returns kTRUE).

   if(epart>0){
      Double_t eres = (fReanalyse ? fECalc : GetEnergy());
      fECalc = TMath::Max((eres - epart),0.0);
      SetReanalyse( fECalc>0.0 );
   }
}

//______________________________________________________________________________//

UInt_t KVDetector::FindZmin(Double_t ELOSS, Char_t mass_formula)
{
   //For particles which stop in the first stage of an identification telescope,
   //we can at least estimate a minimum Z value based on the energy lost in this
   //detector.
   //
   //This is based on the KVMaterial::GetBraggDE method, giving the maximum
   //energy loss in the detector for a given nucleus. For e.g. single-layer silicon
   //detectors this energy corresponds to the threshold or punch-through energy
   //(including correction for pulse-height deficit, if known), whereas
   //for e.g. gas detectors with mylar windows this energy is slightly higher than that
   //corresponding to the limit of the particle stopping in the detector.
   //The "Zmin" is the Z of the particle which gives a maximum energy loss just greater
   //than that measured in the detector. Particles with Z<Zmin could not lose as much
   //energy and so are excluded.
   //
   //If ELOSS is not given, we use the current value of GetEnergy()
   //Use 'mass_formula' to change the formula used to calculate the A of the nucleus
   //from its Z. Default is valley of stability value. (see KVNucleus::GetAFromZ).
   //
   //If the value of ELOSS or GetEnergy() is <=0 we return Zmin=0

   ELOSS = (ELOSS < 0. ? GetEnergy() : ELOSS);
   if(ELOSS <= 0) return 0;

   UInt_t z = 40;
   UInt_t zmin, zmax;
   zmin = 1;
   zmax = 92;
   Double_t difference, last_difference;
   UInt_t last_positive_difference_z = 1;
   KVNucleus particle;
   if (mass_formula>-1)
		particle.SetMassFormula((UChar_t)mass_formula);

   last_difference = 1.e+07;
   difference = 0.;

   while (zmax > zmin + 1) {

      particle.SetZ(z);

      difference = GetBraggDE(z,particle.GetA()) - ELOSS;
      //if difference < 0 the z is too small
      if (difference < 0.0) {

         zmin = z;
         z += (UInt_t) ((zmax - z) / 2 + 0.5);

      } else {

         zmax = z;
         last_positive_difference_z = z;
         z -= (UInt_t) ((z - zmin) / 2 + 0.5);
         last_difference = difference;

      }
   }
   if (difference < 0) {
      //if the last z we tried actually made the difference become negative again
      //(i.e. z too small) we return the last z which gave a +ve difference
      z = last_positive_difference_z;
   }
   return z;
}

//_____________________________________________________________________________________//

Double_t ELossActive(Double_t * x, Double_t * par)
{
   //Calculates energy loss in active layer of detector, taking into account preceding layers
   //First parameter is number of active layer, first layer is numbered 1
   //Following parameters (by sets of 19) are parameters for energy loss in different layers
   //Argument x[0] is incident energy in MeV

   Int_t n_active = (Int_t) par[0];
   if (n_active < 2)
      return ELossSaclay(x, &par[1]);
   else {
      Int_t par_ind = 1;
      Double_t e = x[0];
      for (Int_t layer = 1; layer < n_active; layer++) {

         e = EResSaclay(&e, &par[par_ind]);     //residual energy after layer
         if (e < KVDETECTOR_MINIMUM_E)
            return 0.;          // return 0 if particle stops in layers before active layer
         par_ind += 19;

      }
      //calculate energy loss in active layer
      return ELossSaclay(&e, &par[par_ind]);
   }
   return 0.;
}

//_____________________________________________________________________________________//

Double_t EResDet(Double_t * x, Double_t * par)
{
   //Calculates residual energy of particle after traversing all layers of detector.
   //Returned value is 0 if particle stops in one of the layers of the detector.
   //First parameter par[0]=number of layers of detector
   //Following parameters (by sets of 19) are parameters for energy loss in different layers
   //Argument x[0] is incident energy in MeV

   Int_t n_layers = (Int_t) par[0];
   if (n_layers < 2)
      return EResSaclay(x, &par[1]);
   else {
      Int_t par_ind = 1;
      Double_t e = x[0];
      for (Int_t layer = 1; layer <= n_layers; layer++) {

         e = EResSaclay(&e, &par[par_ind]);     //residual energy after layer
         if (e < KVDETECTOR_MINIMUM_E)
            return 0.;          // return 0 if particle stops in one of layers in detector
         par_ind += 19;

      }
      return e;
   }
   return 0.;
}

//______________________________________________________________________________________________//

void KVDetector::SetELossParams(Int_t Z, Int_t A)
{
   //Initialise energy loss coefficients for this detector and a given incident nucleus (Z,A)

   //do we need to set up the ELoss function ?
   //only if it has never been done or if the index of the active layer has changed...
   if( !npar_loss || (npar_loss != 1 + ((Int_t) fActiveLayer + 1) * 19) ){
      //number of params for eloss function = 1 (index of active layer) + 19 params for each layer up to & including active layer
      npar_loss = 1 + ((Int_t) fActiveLayer + 1) * 19;
      if( par_loss ) delete [] par_loss; //delete previous parameter array
      par_loss = new Double_t[npar_loss];
      par_loss[0] = (Int_t) fActiveLayer + 1;
      //find/create function
      TString name_eloss; name_eloss.Form("ELoss_nact%d", ((Int_t) fActiveLayer + 1));
      //search in list of functions for one corresponding to this detector
      //the name of the required function is ELoss_nactx with x = number of active layer (1, 2, etc.)
      ELoss =
          (TF1 *) gROOT->GetListOfFunctions()->FindObject(name_eloss.Data());
      if (!ELoss)
         ELoss = new TF1(name_eloss.Data(), ELossActive, 0.1, 5000., npar_loss);
   }

   //loop over layers to fill parameter arrays
   TIter next_abs(fAbsorbers); int i=0; KVMaterial*abs;
   while( (abs = (KVMaterial*)next_abs()) ){
      if (i <= (int) fActiveLayer)
         abs->GetELossParams(Z, A, (par_loss + 1 + 19 * i));
      else break;
      i++;
   }

   //set parameters of energy loss function
   ELoss->SetParameters(par_loss);
}

//______________________________________________________________________________________________//

void KVDetector::SetEResParams(Int_t Z, Int_t A)
{
   //Initialise coefficients of residual energy function for this detector and a given incident nucleus (Z,A)

   //do we need to set up the ERes function ?
   //only if it has never been done or if the number of layers has changed...
   if( !npar_res || (npar_res != 1 + fAbsorbers->GetSize() * 19) ){
      //number of params for eres function = 1 (number of layers) + 19 params for each layer in detector
      npar_res = 1 + fAbsorbers->GetSize() * 19;
      if( par_res ) delete [] par_res; //delete previous parameter array
      par_res = new Double_t[npar_res];
      par_res[0] = fAbsorbers->GetSize();
      //find/create function
      TString name_eres; name_eres.Form("ERes_nlay%d", fAbsorbers->GetSize());
      //search in list of functions for one corresponding to this material
      //the name of the required function is ERes_nlayx with x= number of absorbers in detector
      ERes =
          (TF1 *) gROOT->GetListOfFunctions()->FindObject(name_eres.Data());
      if (!ERes)
         ERes =
             new TF1(name_eres.Data(), EResDet, 0.1, 5000., npar_res);
   }

   //loop over layers to fill parameter arrays
   TIter next_abs(fAbsorbers); int i=0; KVMaterial*abs;
   while( (abs = (KVMaterial*)next_abs()) ){
      abs->GetELossParams(Z, A, (par_res + 1 + 19 * i));
      i++;
   }

   //set parameters of residual energy function
   ERes->SetParameters(par_res);
}

//____________________________________________________________________________________

KVDetector *KVDetector::MakeDetector(const Char_t * name, Float_t thickness)
{
   //Static function which will create an instance of the KVDetector-derived class
   //corresponding to 'name'
   //These are defined as 'Plugin' objects in the file $KVROOT/KVFiles/.kvrootrc :
	//[name_of_dataset].detector_type or detector_type
	//first we check if there is a special plugin for the DataSet
	//if not we take the default one
   //
   //'thickness' is passed as argument to the constructor for the detector plugin

	//check and load plugin library
   TPluginHandler *ph=NULL;
   KVString nom(name);
	if ( !nom.Contains(".") && !(ph = LoadPlugin("KVDetector",name)) ) return 0;
	if ( nom.Contains(".") ) {
		TObjArray *toks = nom.Tokenize("."); Int_t nt = toks->GetEntries();
		if ( nt==2 ) {
			if ( !(ph = LoadPlugin("KVDetector",name)) ){
				if ( !(ph = LoadPlugin("KVDetector",((TObjString*)(*toks)[1])->GetString().Data())) ) { delete toks; return 0; }
			}
		}
		else if ( nt==1 ) { if ( !(ph = LoadPlugin("KVDetector",((TObjString*)(*toks)[0])->GetString().Data())) ) {delete toks; return 0;}  }
		else {
         delete toks; return 0;
      }
      delete toks;
	}

	//execute constructor/macro for detector
	return (KVDetector *) ph->ExecPlugin(1, thickness);
}

//____________________________________________________________________________________

const TVector3& KVDetector::GetNormal()
{
   // Return unit vector normal to surface of detector. The vector points from the target (origin)
   // towards the detector's entrance window. It can be used with GetELostByParticle and
   // GetParticleEIncFromERes.
   // The vector is generated from the theta & phi of the centre of the detector, obtained from
   // the KVTelescope used to position the detector in a multidetector array.
   // If this detector does not belong to a KVTelescope it will have no defined orientation
   // and this method returns (0,0,1)

   fNormToMat.SetMagThetaPhi(1, GetTheta()*TMath::DegToRad(), GetPhi()*TMath::DegToRad());
   return fNormToMat;
}

//____________________________________________________________________________________

void KVDetector::GetVerticesInOwnFrame(TVector3 *corners, Double_t depth, Double_t layer_thickness)
{
	// This will fill the array corners[8] with the coordinates of the vertices of the
	// front (corners[0],...,corners[3]) & back (corners[4],...,corners[7]) sides of the volume
	// representing either a single absorber or the whole detector.
	//
	// depth = depth of detector/absorber inside the KVTelescope it belongs to (in centimetres)
	// layer_thickness = thickness of absorber/detector (in centimetres)
	//
	// Positioning information is taken from the KVTelescope to which this detector
	// belongs; if this is not the case, nothing will be done.

	// relative distance to back of detector
	Double_t dist_to_back = depth + layer_thickness;

	// get coordinates
	fTelescope->GetCornerCoordinatesInOwnFrame(corners, depth);
	fTelescope->GetCornerCoordinatesInOwnFrame(&corners[4], dist_to_back);
}

//____________________________________________________________________________________

TGeoVolume* KVDetector::GetGeoVolume()
{
	// Construct a TGeoVolume shape to represent this detector in the current
	// geometry managed by gGeoManager.
	// Positioning information is taken from the KVTelescope to which this detector
	// belongs; if this is not the case, nothing will be done.
	//
	// Making the volume requires:
	//    - to construct a 'mother' volume (TGeoArb8) defined by the (theta-min/max, phi-min/max)
	//      and the total thickness of the detector (all layers)
	//    - to construct and position volumes (TGeoArb8) for each layer of the detector inside the
	//      'mother' volume. Each layer is represented by a TGeoArb8 whose two parallel faces correspond
	//      to the front and back surfaces of the layer.
	//
	// If the detector is composed of a single absorber, we do not create a superfluous "mother" volume.
	//
	// gGeoManager must point to current instance of geometry manager.

	if(!fTelescope) return NULL;

	if(!gGeoManager) return NULL;

	if(fTotThickness == 0) GetTotalThicknessInCM(); // calculate sum of absorber thicknesses in centimetres
	// get from telescope info on relative depth of detector i.e. depth inside telescope
	if(fDepthInTelescope == 0)
		fDepthInTelescope = fTelescope->GetDepthInCM( fTelescope->GetDetectorRank(this) );

	TVector3 coords[8];
	Double_t vertices[16];
	Double_t tot_thick_det = fTotThickness;
	TGeoMedium *med;
	TGeoVolume *mother_vol = 0;

	Bool_t multi_layer = fAbsorbers->GetSize()>1;

	if( multi_layer ){
		/**** BUILD MOTHER VOLUME : big Vacuum-filled box to put all absorbers in ****/
		GetVerticesInOwnFrame(coords, fDepthInTelescope, tot_thick_det);
		for(register int i=0;i<8;i++){
			vertices[2*i] = coords[i].X();
			vertices[2*i+1] = coords[i].Y();
		}
		Double_t dz = tot_thick_det/2.;
		med = GetGeoMedium("Vacuum");
		mother_vol = gGeoManager->MakeArb8(GetName(), med, dz, vertices);
	}

	/**** BUILD & ADD ABSORBER VOLUMES ****/
	TIter next(fAbsorbers); KVMaterial *abs; Double_t depth_in_det = 0.; Int_t no_abs=1;
	while( (abs = (KVMaterial*)next()) ){
		// get medium for absorber
		med = abs->GetGeoMedium();
		Double_t thick = abs->GetThicknessInCM();
		GetVerticesInOwnFrame(coords, fDepthInTelescope+depth_in_det, thick);
		for(register int i=0;i<8;i++){
			vertices[2*i] = coords[i].X();
			vertices[2*i+1] = coords[i].Y();
		}
		Double_t dz = thick/2.;
		TGeoVolume *vol =
				gGeoManager->MakeArb8( Form("%s_%d_%s", GetName(), no_abs, abs->GetType()), med, dz, vertices);
		vol->SetLineColor( med->GetMaterial()->GetDefaultColor() );
		if( multi_layer ){
			/*** position absorber in mother ***/
			Double_t trans_z = -tot_thick_det/2. + depth_in_det + dz;// (note: reference is CENTRE of absorber)
			TGeoTranslation *tr = new TGeoTranslation(0., 0., trans_z);
			mother_vol->AddNode(vol, 1, tr);
		}
		else
		{
			// single absorber: mother is absorber is detector is mother is ...
			mother_vol = vol;
		}
		depth_in_det += thick; no_abs++;
	}

	return mother_vol;
}

//____________________________________________________________________________________

void KVDetector::AddToGeometry()
{
	// Construct and position a TGeoVolume shape to represent this detector in the current
	// geometry managed by gGeoManager.
	// Positioning information is taken from the KVTelescope to which this detector
	// belongs; if this is not the case, nothing will be done.
	//
	// Adding the detector to the geometry requires:
	//    - to construct a 'mother' volume (TGeoArb8) defined by the (theta-min/max, phi-min/max)
	//      and the total thickness of the detector (all layers)
	//    - to construct and position volumes (TGeoArb8) for each layer of the detector inside the
	//      'mother' volume
	//    - to position 'mother' inside the top-level geometry
	//
	// gGeoManager must point to current instance of geometry manager.

	if(!fTelescope) return;

	if(!gGeoManager) return;

	// get volume for detector
	TGeoVolume* vol = GetGeoVolume();

	// rotate detector to orientation corresponding to (theta,phi)
	KVTelescope* tl=GetTelescope();
	Double_t theta = tl->GetTheta(); Double_t phi = tl->GetPhi();
	TGeoRotation rot1, rot2;
	rot2.SetAngles(phi+90., theta, 0.);
	rot1.SetAngles(-90., 0., 0.);
	// distance to detector centre = distance to telescope + depth of detector in telescope + half total thickness of detector
	Double_t dist = tl->GetDistance()/10. + fDepthInTelescope + fTotThickness/2.;
	// translate detector to correct distance from target (note: reference is CENTRE of detector)
	Double_t trans_z = dist;

	TGeoTranslation tran(0,0,trans_z);
	TGeoHMatrix h = rot2 * tran * rot1;
	TGeoHMatrix *ph = new TGeoHMatrix(h);

	// add detector volume to geometry
	gGeoManager->GetTopVolume()->AddNode(vol, 1, ph);
}

void KVDetector::SetFiredBitmask()
{
   // Set bitmask used to determine which acquisition parameters are
   // taken into account by KVDetector::Fired based on the environment variables
   //          [dataset].KVACQParam.[par name].Working:    NO
   //          [dataset].KVDetector.Fired.ACQParameterList.[type]: PG,GG,T
   // The first allows to define certain acquisition parameters as not functioning;
   // they will not be taken into account.
   // The second allows to "fine-tune" what is meant by "all" or "any" acquisition parameters
   // (i.e. when using Fired("all"), Fired("any"), Fired("Pall", etc.).
   // For each detector type, give a comma-separated list of the acquisition
   // parameter types to be taken into account in the KVDetector::Fired method.
   // Only those parameters which appear in the list will be considered:
   //  then "all" means => all parameters in the list
   //  and  "any" means => any of the parameters in the list
   // These lists are read during construction of multidetector arrays (KVMultiDetArray::Build),
   // the method KVMultiDetArray::SetACQParams uses them to define a mask for each detector
   // of the array.
   // Bits are set/reset in the order of the acquisition parameter list of the detector.
   // If no variable [dataset].KVDetector.Fired.ACQParameterList.[type] exists,
   // we set a bitmask authorizing all acquisition parameters of the detector, e.g.
   // if the detector has 3 acquisition parameters the bitmask will be "111"

	KVString inst; inst.Form("KVDetector.Fired.ACQParameterList.%s",GetType());
	KVString lpar = gDataSet->GetDataSetEnv(inst);
	TObjArray *toks = lpar.Tokenize(",");
	TIter next(fACQParams);
	Bool_t no_variable_defined = (toks->GetEntries()==0);
	KVACQParam* par; Int_t id = 0;
	while( (par = (KVACQParam*)next()) ){
	    if( !par->IsWorking() ) fFiredMask.ResetBit(id); // ignore non-working parameters
	    else{
	        if( no_variable_defined || toks->FindObject( par->GetType() ) ) fFiredMask.SetBit(id);
	        else fFiredMask.ResetBit(id);
	    }
	    id++;
	}
	delete toks;
}

	void printvec(TVector3& v)
	{
		cout << "(" << v.X() << "," << v.Y() << "," << v.Z() << ")";
	};

Double_t KVDetector::GetEntranceWindowSurfaceArea()
{
	// Return surface area of first layer of detector in mm2.

	if(!fTelescope) return 0;

	if(fDepthInTelescope == 0)
		fDepthInTelescope = fTelescope->GetDepthInCM( fTelescope->GetDetectorRank(this) );

	TVector3 coords[4];

	fTelescope->GetCornerCoordinates(coords,fDepthInTelescope);
	cout << "DETECTOR COORDINATES (in cm):" <<endl;
	cout << "=================================" << endl;
	cout << " A : "; printvec(coords[0]); cout << endl;
	cout << " B : "; printvec(coords[1]); cout << endl;
	cout << " C : "; printvec(coords[2]); cout << endl;
	cout << " D : "; printvec(coords[3]); cout << endl;

	cout << "DETECTOR DIMENSIONS (in mm):" << endl;
	cout << "================================" << endl;
	Double_t c = 10*(coords[0]-coords[1]).Mag();
	Double_t b = 10*(coords[1]-coords[2]).Mag();
	Double_t d = 10*(coords[2]-coords[3]).Mag();
	Double_t a = 10*(coords[0]-coords[3]).Mag();
	cout << " AB = " << c << endl;
	cout << " BC = " << b << endl;
	cout << " CD = " << d << endl;
	cout << " AD = " << a << endl;

	cout << "DETECTOR SURFACE AREA = ";
	Double_t surf = pow((a+b),2.0)*(a-b+2.0*c)*(b-a+2.0*c);
	surf = sqrt(surf)/4.0;
	cout << surf << " mm2" << endl;

	return surf;
}

