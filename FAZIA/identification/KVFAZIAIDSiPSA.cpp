//Created by KVClassFactory on Mon Feb 17 13:52:51 2014
//Author: John Frankland,,,

#include "KVFAZIAIDSiPSA.h"
#include "KVDataSet.h"

ClassImp(KVFAZIAIDSiPSA)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIAIDSiPSA</h2>
<h4>PSA identification in first silicon of FAZIA telescopes</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

TGraph* KVFAZIAIDSiPSA::fZThreshold = 0;
TGraph* KVFAZIAIDSiPSA::fAThreshold = 0;

KVFAZIAIDSiPSA::KVFAZIAIDSiPSA()
{
   // Default constructor

   if (!fZThreshold) {
      // Threshold for Z identification by PSA
      // Data from NIM A 701, 145 (2013)
      fZThreshold = new TGraph(8);
      fZThreshold->SetPoint(0, 2.919586798, 1.505566588);
      fZThreshold->SetPoint(1, 4.968677141, 2.393311626);
      fZThreshold->SetPoint(2, 5.963949593, 2.837184146);
      fZThreshold->SetPoint(3, 9.945039402, 4.733730365);
      fZThreshold->SetPoint(4, 14.9799471, 5.984643828);
      fZThreshold->SetPoint(5, 20.0148548, 7.840838);
      fZThreshold->SetPoint(6, 25.10830794, 9.858440361);
      fZThreshold->SetPoint(7, 30.0846702, 14.21646146);
   }
   if (!fAThreshold) {
      // Threshold for A identification by PSA
      // Data from NIM A ?, ? (2017)
      fAThreshold = new TGraph(8);
      fAThreshold->SetPoint(0, 2.919586798, 2 * 1.505566588);
      fAThreshold->SetPoint(1, 4.968677141, 2 * 2.393311626);
      fAThreshold->SetPoint(2, 5.963949593, 2 * 2.837184146);
      fAThreshold->SetPoint(3, 9.945039402, 2 * 4.733730365);
      fAThreshold->SetPoint(4, 14.9799471, 2 * 5.984643828);
      fAThreshold->SetPoint(5, 20.0148548, 2 * 7.840838);
      fAThreshold->SetPoint(6, 25.10830794, 2 * 9.858440361);
      fAThreshold->SetPoint(7, 30.0846702, 2 * 14.21646146);
   }
   SetType("SiPSA");
}

KVFAZIAIDSiPSA::~KVFAZIAIDSiPSA()
{
   // Destructor
}

Bool_t KVFAZIAIDSiPSA::Identify(KVIdentificationResult* idr, Double_t x, Double_t y)
{
   //Particle identification and code setting using identification grid (class KVIDZAFromZGrid).

   idr->SetIDType(GetType());
   idr->IDattempted = kTRUE;

   //perform identification
   Double_t esi = (y < 0. ? GetIDMapY() : y);
   Double_t ima = (x < 0. ? GetIDMapX() : x);

   if (IGrid->IsIdentifiable(ima, esi)) {
      IGrid->Identify(ima, esi, idr);
   } else {
      idr->IDOK = kFALSE;
      idr->IDquality = KVIDZAGrid::kICODE8;
   }

   // set general ID code
   idr->IDcode = GetIDCode();
   return kTRUE;
}

Double_t KVFAZIAIDSiPSA::GetIDMapX(Option_t*)
{
   //X-coordinate for Si PSA identification map :
   //Amplitude of QH1 signal
//    if (!strcmp(opt, "I")) return fSi->GetI1Amplitude();
//    else if (!strcmp(opt, "Q")) return fSi->GetQH1RiseTime();
//    else return -1;
   return fSi->GetI1Amplitude();
}

Double_t KVFAZIAIDSiPSA::GetIDMapY(Option_t*)
{
   //Y-coordinate for Si PSA identification map :
   //Amplitude of I1 signal
   return fSi->GetQH1Amplitude();
}

void KVFAZIAIDSiPSA::Initialize()
{
   // Initialisation of telescope before identification.
   // This method MUST be called once before any identification is attempted.
   // Initialisation of grid is performed here.
   // IsReadyForID() will return kTRUE if a grid is associated to this telescope for the current run.

   IGrid = (KVIDZAGrid*) GetIDGrid(); // for the moment we defined only Igrids -> to be modified latter
   fSi = (KVFAZIADetector*)GetDetector(1);
   if (IGrid) {
      SetHasMassID(IGrid->IsOnlyZId());
      IGrid->Initialize();
      SetBit(kReadyForID);
   } else {
      ResetBit(kReadyForID);
   }
   if (!gDataSet->HasCalibIdentInfos()) SetBit(kReadyForID);
}

Bool_t KVFAZIAIDSiPSA::CheckTheoreticalIdentificationThreshold(KVNucleus* ION, Double_t)
{
   // Return kTRUE if energy of ION is > minimum incident energy required for identification
   // in charge (Z) given by figure 9 of "Comparison of charged particle identification
   // using pulse shape discrimination and DeltaE-E methods between front and rear injection in
   // silicon detector" NIM A 701, 145 (2013) (FAZIA collaboration).
   // Thresholds used are those for rear-injection (red curve, lowest values).
   // **to be updated with G.Pastore NIMA 2017**

   Double_t seuil = fZThreshold->Eval(ION->GetZ());
   return (ION->GetEnergyPerNucleon() > seuil);
}

void KVFAZIAIDSiPSA::SetIdentificationStatus(KVReconstructedNucleus* n)
{
   // For filtering simulations
   // If n->GetEnergy() is above threshold for mass identification, we set
   // n->IsAMeasured(kTRUE) (and n->IsZMeasured(kTRUE)).
   // Otherwise, we just set n->IsZMeasured(kTRUE) and use the A given by
   // the mass formula for the particle

   n->SetZMeasured();
   if (n->GetEnergyPerNucleon() > fAThreshold->Eval(n->GetZ())) {
      n->SetAMeasured();
   } else
      n->SetZ(n->GetZ());
}
