/*
$Id: KVIDGChIoSi.cpp,v 1.31 2008/03/31 12:31:39 ebonnet Exp $
$Revision: 1.31 $
$Date: 2008/03/31 12:31:39 $
$Author: ebonnet $
*/

#include "KVIDGChIoSi.h"
#include "KVIDZLine.h"
#include "KVINDRAReconNuc.h"
#include "KVINDRA.h"
#include "KVChIo.h"
#include "KVSilicon.h"
#include "KVNewGridRRMDialog.h"
#include "TObjString.h"
#include "KVParameter.h"
#include "TObjArray.h"
#include "TROOT.h"

ClassImp(KVIDGChIoSi)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KVIDGChIoSi
//
// Identification grids for INDRA Ionisation chamber-Silicon telescopes.
// After attempting identification with method Identify, GetStatus returns
// one of the following status codes:
//
// 0 KVIDGChIoSi::kOK               "ok",
// 1 KVIDGChIoSi::k_BelowPunchThrough   "point to identify below punch-through line (bruit)",
// 2 KVIDGChIoSi::k_BelowSeuilSi      "point to identify left of Si threshold line (bruit/arret ChIo?)",
// 3 KVIDGChIoSi::k_LeftOfBragg       "point to identify below Bragg curve. Z given is a Zmin",
// 4 KVIDGChIoSi::k_RightOfEmaxSi         "point to identify has E_Si > Emax_Si i.e. codeur is saturated. Unidentifiable",
// 5 KVIDGChIoSi::k_ZgtZmax           "point to identify above largest identifiable Z line. Z given is a Zmin.",
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KVIDGChIoSi::KVIDGChIoSi()
{
   //Default constructor
   init();
}

    KVIDGChIoSi::KVIDGChIoSi(TString pattern)
{
   //constructor with a pattern
   init();
	pattern.ReplaceAll(".","_"); fPattern = pattern;
}

void KVIDGChIoSi::init()
{
   //default initialisations
   fPattern = "";
	fStatus = kOK;
   SetType("ChIo-Si Grid");
   fBragg = fPunch = fSeuil = fEmaxSi = 0;
   fIgnorePunchThrough = kFALSE;
}

KVIDGChIoSi::KVIDGChIoSi(const KVIDGChIoSi & grid)
{
   //Copy constructor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   grid.Copy(*this);
#else
   ((KVIDGChIoSi &) grid).Copy(*this);
#endif
}

#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVIDGChIoSi::Copy(TObject & obj) const
#else
void KVIDGChIoSi::Copy(TObject & obj)
#endif
{
   //Copy characteristics of this grid to the object "obj"
   KVIDGChIoSi & grid = (KVIDGChIoSi &) obj;
   KVIDGrid::Copy(obj);
   if (!const_cast < KVIDGChIoSi * >(this)->GetRunList().IsEmpty())
      grid.GetRunList().SetList(const_cast <
                                KVIDGChIoSi *
                                >(this)->GetRunList().AsString());
   if (!const_cast < KVIDGChIoSi * >(this)->GetRingList().IsEmpty())
      grid.GetRingList().SetList(const_cast <
                                 KVIDGChIoSi *
                                 >(this)->GetRingList().AsString());
   if (!const_cast < KVIDGChIoSi * >(this)->GetModuleList().IsEmpty())
      grid.GetModuleList().SetList(const_cast <
                                   KVIDGChIoSi *
                                   >(this)->GetModuleList().AsString());
   if (!const_cast < KVIDGChIoSi * >(this)->GetPattern().IsNull())
      grid.SetPattern(const_cast <
                                   KVIDGChIoSi *
                                   >(this)->GetPattern());
}

KVIDGChIoSi::~KVIDGChIoSi()
{
   //Dtor.
}

//______________________________________________________________________________________________________//

Bool_t KVIDGChIoSi::IsIdentifiable(Double_t x, Double_t y) const
{
   // Sets fStatus for particle depending on its position in the grid.
   //
   // Returns kTRUE if the particle corresponding to point (x=esi,y=echio) in the ChIo-Si map
   // is identifiable i.e. if it is contained within the region delimited by
   //      - the line indicating the limit of particles stopping in the ChIo (seuil_si)
   //      - the line indicating the limit of particles punching through to the CsI (punch_through)
   //      - the line of the largest Z identified in the map (zmaxline)
   //

   Bool_t can_id = kTRUE;
   const_cast < KVIDGChIoSi * >(this)->fStatus = kOK;
   //must be above punch through line
   if (fPunch) {
      can_id = fPunch->WhereAmI(x, y, "above");
      if (!can_id) {
         const_cast < KVIDGChIoSi * >(this)->fStatus = k_BelowPunchThrough;
         return kFALSE;
      };
   }
   //must be right of arret chio/seuil si line
   if (fSeuil) {
      can_id = fSeuil->WhereAmI(x, y, "right");
      if (!can_id) {
         const_cast < KVIDGChIoSi * >(this)->fStatus = k_BelowSeuilSi;
         return kFALSE;
      };
   }
   //must be left of Emax_Si line
   if (fEmaxSi) {
      can_id =  fEmaxSi->WhereAmI(x, y, "left");
      if (!can_id) {
         const_cast < KVIDGChIoSi * >(this)->fStatus = k_RightOfEmaxSi;
         return kFALSE;
      };
   }
   //must be below max Z line
   can_id =fZMaxLine->WhereAmI(x, y, "below");
   if (!can_id) {
      const_cast < KVIDGChIoSi * >(this)->fStatus = k_ZgtZmax;
      return kFALSE;
   };
   //particles below (left of) Bragg line are identified, but their Z is a Zmin
   if (fBragg) {
      can_id = fBragg->WhereAmI(x, y, "right");
      if (!can_id) {
         const_cast < KVIDGChIoSi * >(this)->fStatus = k_LeftOfBragg;
			return kTRUE;
      };
   }
   return can_id;
}

//_______________________________________________________________________________________________//

void KVIDGChIoSi::MakeELossGrid(UInt_t Zmax, Bool_t withPHD,
                                Double_t ChIo_press, Int_t npoints)
{
   //For a given ChIo-Si ID telescope, construct a new ID grid with Z-lines
   //calculated from energy loss calculations, but with
   //      - different chio pressure if required (give -1 to use current Chio pressure)
   //      - simulation of PHD in silicon detector (withPHD=kTRUE)
   //         *** PHD of detector must be set via its KVPulseHeightDefect calibration object ***
   //         *** i.e. call KVSilicon::SetMoultonPHDParameters() method for detector in question ***
   //
   //The characteristics of the detectors are taken from the ionisation chamber and silicon
   //pad corresponding to the lowest ring and module numbers for which this grid has been
   //declared to be valid. Do not call this method before setting this. Also make sure that you perform
   //e.g. a call to gIndra->SetParameters(some_run_number) beforehand in order to ensure that
   //the gas pressure in the ionisation chamber is correct for the runs for which the grid is valid
   //(this is not done automatically).
   //
   //Lines are calculated from energy at which particle first passes ChIo and stops in silicon
   //to punch-through
   //but Silicon thickness is increased by 50% so that lines continue further than data
   //
   //To change the formula used to calculate fragment A from Z, use method SetMassFormula
   //before calculating grid.
   //
   //More points are used at the beginning than at the end of the line (logarithmic)
   //Bragg "OK"-line is also calculated and added (each point is GetBraggE and GetBraggDE for each Z)
   //
   //If this grid has X/Y scaling factors, we apply them to the results of the calculation.

   Double_t x_scale = GetXScaleFactor();
   Double_t y_scale = GetYScaleFactor();
   //clear old lines from grid (and scaling parameters)
   Clear();
   //set conditions of ChIo and Si in telescope
   KVSilicon *si =
       (KVSilicon *) gIndra->GetDetectorByType(GetMinRing(), GetMinMod(),
                                               Si_GG);
   if (!si) {
      Error("MakeELossGrid",
            "No silicon detector with ring=%d mod=%d", GetMinRing(),
            GetMinMod());
      return;
   }
   //increase Si thickness by 50%
   Double_t Si_thick = si->GetThickness();
   si->SetThickness(1.5 * Si_thick);

   if (withPHD)
      GetParameters()->SetValue("With PHD", 1);
   else
      GetParameters()->RemoveParameter("With PHD");

   KVChIo *chio = si->GetChIo();
   if (!chio) {
      Error("MakeELossGrid", "No ChIo in front of %s", si->GetName());
      return;
   }
   //nominal pressure of ChIo
   Double_t press_nom = chio->GetPressure();
   //change pressure if required
   if (ChIo_press > 0.)
      chio->SetPressure(ChIo_press);
   //keep actual ChIo pressure used for calculation to write in header of ascii file
   GetParameters()->SetValue("ChIo pressure", chio->GetPressure());

   //loop over Z
   KVNucleus part;
   //set mass formula used for calculating A from Z
   part.SetMassFormula(GetMassFormula());

   Info("MakeELossGrid",
        "Calculating dE-E grid: dE detector = %s, E detector = %s",
        chio->GetName(), si->GetName());

   KVIDLine *B_line = (KVIDLine *) AddLine("OK");
   Int_t npoi_bragg = 0;
   B_line->SetName("Bragg_line");

   for (int z = 1; z <= (int) Zmax; z++) {
      part.SetZ(z);

      //loop over energy
      //first find :
      //      ****E1 = energy at which particle passes ChIo and starts to enter Si****
      //      E2 = energy at which particle passes Si
      //then perform npoints calculations between these two energies and use these
      //to construct a KVIDZLine

      Double_t E1, E2;
      //find E1
      //go from 0.1 MeV to chio->GetBraggE(part.GetZ(),part.GetA()))
      Double_t E1min = 0.1, E1max = chio->GetBraggE(part.GetZ(),part.GetA());
      E1 = (E1min + E1max) / 2.;

      while ((E1max - E1min) > 0.1) {

         part.SetEnergy(E1);
         si->Clear();
         chio->DetectParticle(&part);
         si->DetectParticle(&part);
         if (si->GetEnergy() > .1) {
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
      Double_t dE_B = chio->GetBraggDE(z, part.GetA());
      Double_t E_B = chio->GetBraggE(z, part.GetA());
      Double_t Esi_B = chio->GetERes(z, part.GetA(), E_B);
      B_line->SetPoint(npoi_bragg++, Esi_B, dE_B);

      //find E2
      //go from E1 MeV to 6000 MeV
      Double_t E2min = E1, E2max = 6000.;
      E2 = (E2min + E2max) / 2.;

      while ((E2max - E2min > 0.1)) {

         part.SetEnergy(E2);
         chio->DetectParticle(&part);
         si->DetectParticle(&part);
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
      
      cout << "Z=" << z << " E1 = " << E1 << " E2 = " << E2 << endl;
      KVIDZLine *line = (KVIDZLine *) AddLine("ID", "KVIDZLine");
      line->SetZ(z);

      Double_t logE1 = TMath::Log(E1);
      Double_t logE2 = TMath::Log(E2);
      Double_t dLog = (logE2 - logE1) / (npoints - 1.);

      for (int i = 0; i < npoints; i++) {

//                      Double_t E = E1 + i*(E2-E1)/(npoints-1.);
         Double_t E = TMath::Exp(logE1 + i * dLog);

         Double_t Esi = 0.;
         while (Esi < 0.1) {
            chio->Clear();
            si->Clear();
            part.SetEnergy(E);
            chio->DetectParticle(&part);
            si->DetectParticle(&part);
            Esi = si->GetEnergy();
            E += 0.1;
         }
         Double_t Echio = chio->GetEnergy();
         //PHD correction
         if (withPHD)
            Esi -= si->GetPHD(Esi, z);
         line->SetPoint(i, Esi, Echio);
      }
   }
   //reset Si thickness
   si->SetThickness(Si_thick);
   //put real pressure back if changed
   if (ChIo_press > 0.)
      chio->SetPressure(press_nom);
   //if this grid has scaling factors, we need to apply them to the result
   if (x_scale != 1)
      SetXScaleFactor(x_scale);
   if (y_scale != 1)
      SetYScaleFactor(y_scale);
}


//_______________________________________________________________________________________________//

void KVIDGChIoSi::SetRings(Int_t rmin, Int_t rmax)
{
   //Set list of rings for which grid is valid from ring rmin to ring rmax
   GetRingList().SetMinMax(rmin, rmax);
}

void KVIDGChIoSi::SetModules(Int_t rmin, Int_t rmax)
{
   //Set list of modules for which grid is valid from module rmin to module rmax
   GetModuleList().SetMinMax(rmin, rmax);
}

void KVIDGChIoSi::SetRuns(Int_t rmin, Int_t rmax)
{
   //Set list of runs for which grid is valid from run rmin to run rmax
   GetRunList().SetMinMax(rmin, rmax);
}

//_______________________________________________________________________________________________//

void KVIDGChIoSi::ReadAsciiFile(ifstream & gridfile)
{
   //Redefinition of KVIDGrid::ReadAsciiFile.
   //In case we are reading an old (i.e. prior to class version 3) grid, the runs, rings and modules for which it
   //is valid are indicated by <PARAMETER> lines with names "Ring min", "Run max", etc. We have to read these
   //and use them to set up the KVNumberList objects (GetRunList().SetMinMax() etc. etc.)
   //
   //If we are reading a new (class version >= 3) grid, then the KVNumberLists are written on lines such as:
   //<LIST> Runs=1000-1045 1051 1067-1080
   //<LIST> Rings=6-7
   //<LIST> Modules=1-4 7-18 21-24
   //These are read and used directly to initialise the appropriate lists.
   //
   //For the rest (any other <PARAMETER> lines, or the actual lines in the grid), we proceed exactly as in
   //KVIDGrid::ReadAsciiFile:
   //(the following is the doc from KVIDGrid::ReadAsciiFile)
   //Read info from already open ascii file stream containing each line of this grid.
   //Any previous lines in the grid are destroyed.
   //Comment lines begin with #
   //Grid info begins with
   //++ClassName
   //where ClassName is actual class name of instance of this object
   //Parameter lines begin with <PARAMETER>
   //New lines begin with +name_of_class
   //'!' signals end of grid info
   //Any scaling factors are applied once the coordinates have been read

   //reset grid - destroy old lines, parameters, axis limits...
   Clear();

   KVString s;
   int line_no=0;// counter for lines read

   Int_t ring_min, ring_max, mod_min, mod_max, run_min, run_max;
   ring_min = ring_max = mod_min = mod_max = run_min = run_max = -1;

   while (gridfile.good()) {
      //read a line
      s.ReadLine(gridfile);
      if (s.BeginsWith('!')) {
         //end of grid signalled
         break;
      } else if (s.BeginsWith("++")) {  //will only happen if we are reading a file using ReadAsciiFile(const char*)
         //check classname
         s.Remove(0, 2);
         if (s != ClassName())
            Warning("ReadAsciiFile(ofstream&)",
                    "Class name in file %s does not correspond to this class (%s)",
                    s.Data(), ClassName());
      } else if (s.BeginsWith("<PARAMETER>")) { //parameter definition
         s.Remove(0, 11);       // get rid of "<PARAMETER>"
         //split into tokens separated by '='
         TObjArray *toks = s.Tokenize('=');
         TString name =
             ((TObjString *) toks->At(0))->GetString().
             Strip(TString::kBoth);
         KVString value(((TObjString *) toks->At(1))->GetString());
         //look for old "ring min"/"ring max" etc. parameters
         if (name == "Ring min")
            ring_min = value.Atoi();
         else if (name == "Ring max")
            ring_max = value.Atoi();
         else if (name == "Mod min")
            mod_min = value.Atoi();
         else if (name == "Mod max")
            mod_max = value.Atoi();
         else if (name == "First run")
            run_min = value.Atoi();
         else if (name == "Last run")
            run_max = value.Atoi();
         else
            GetParameters()->SetValue(name.Data(), value.Atof());    // any other parameter is treated normally
         delete toks;           //must clean up             
      } else if (s.BeginsWith("<LIST>")) {      // number list definition
         s.Remove(0, 6);        // get rid of "<LIST>"
         //split into tokens separated by '='
         TObjArray *toks = s.Tokenize('=');
         TString name =
             ((TObjString *) toks->At(0))->GetString().
             Strip(TString::kBoth);
         TString list(((TObjString *) toks->At(1))->GetString());
         //set appropriate list
         if (name == "Runs")
            GetRunList().SetList(list);
         else if (name == "Rings")
            GetRingList().SetList(list);
         else if (name == "Modules")
            GetModuleList().SetList(list);
         delete toks;           //must clean up
      } else if (s.BeginsWith('+')) {
         //New line
         line_no++;
         //Get name of class by stripping off the '+' at the start of the line
         s.Remove(0, 1);
         //Make new line using this class
         KVIDLine *line = NewLine(s.Data());
         //next line is type ("ID" or "OK") followed by ':', followed by name of line
         s.ReadLine(gridfile);
         //split into tokens separated by ':'
         TObjArray *toks = s.Tokenize(':');
         TString type = ((TObjString *) toks->At(0))->GetString();
         TString name = "";
         // make sure the line actually has a name !
         if( toks->GetEntries()>1 ) name = ((TObjString *) toks->At(1))->GetString();
         else
         {
            // print warning allowing to find nameless line in file
            Warning("ReadAsciiFile",
                  "In grid : %s\nLine with no name. Line type = %s. Line number in grid = %d",
                  GetName(), type.Data(), line_no);
         }
         delete toks;           //must clean up
         AddLine(type, line);
         line->SetName(name.Data());
         //now use ReadAscii method of class to read coordinates and other informations
         line->ReadAsciiFile(gridfile);
      }
   }
   fLastScaleX = GetXScaleFactor();
   fLastScaleY = GetYScaleFactor();
   if (fLastScaleX != 1 || fLastScaleY != 1) {
      Scale(fLastScaleX, fLastScaleY);
   }
   //set run/ring/module lists according to min/max parameters read in case of old grid (class version < 3)
   if (ring_min != -1 && ring_max != -1)
      GetRingList().SetMinMax(ring_min, ring_max);
   if (mod_min != -1 && mod_max != -1)
      GetModuleList().SetMinMax(mod_min, mod_max);
   if (run_min != -1 && run_max != -1)
      GetRunList().SetMinMax(run_min, run_max);
}

//___________________________________________________________________________________

const Char_t *KVIDGChIoSi::GetName() const
{
   //Overrides KVIDGrid method. Name includes all parameter values, plus ring/module/run lists
   //Gives same name to grid as for old-style (class version < 3) grids.

   Char_t par_value[512], dummy[128];
   sprintf(par_value, "%s : [", GetType());
   if (!const_cast < KVIDGChIoSi * >(this)->GetRingList().IsEmpty()) {
      sprintf(dummy, " Rings:%s",
              const_cast <
              KVIDGChIoSi * >(this)->GetRingList().AsString());
      strcat(par_value, dummy);
   }
   if (!const_cast < KVIDGChIoSi * >(this)->GetModuleList().IsEmpty()) {
      sprintf(dummy, " Mods:%s",
              const_cast <
              KVIDGChIoSi * >(this)->GetModuleList().AsString());
      strcat(par_value, dummy);
   }
   if (!const_cast < KVIDGChIoSi * >(this)->GetRunList().IsEmpty()) {
      sprintf(dummy, " Runs:%s",
              const_cast < KVIDGChIoSi * >(this)->GetRunList().AsString());
      strcat(par_value, dummy);
   }
   strcat(par_value, " ]");
   KVParameter<KVString> *par = 0;
   for( int i=0; i<GetParameters()->GetNPar(); i++) { //loop over all parameters
      par = GetParameters()->GetParameter(i);
      sprintf(dummy, " %s=%s", par->GetName(), par->GetVal().Data());
      strcat(par_value, dummy);
   }
   const_cast < KVIDGChIoSi * >(this)->SetName(par_value);
   return TNamed::GetName();
}

//_______________________________________________________________________________________________//

void KVIDGChIoSi::WriteAsciiFile(ofstream & gridfile)
{
   //Overrides KVIDGrid method in order to write number lists for runs, rings and modules.
   //Format for these lines is
   //<LIST> Runs=1000-1045 1051 1067-1080
   //<LIST> Rings=6-7
   //<LIST> Modules=1-4 7-18 21-24
   //
   //Everything else is as before:
   //(the following is the doc from KVIDGrid)
   //
   //Write info on this grid in already-open ascii file stream 'gridfile'.
   //This begins with the line
   //++ClassName
   //where '"ClassName'" is the name of the class actual instance of this object
   //followed by informations on the grid and its lines.
   //Any associated parameters are written before the lines, starting with
   //<PARAMETER>
   //then the name of the parameter and its value
   //e.e.
   //<PARAMETER> ChIo pressure=45.5
   //A single '!' on a line by itself signals the end of the informations for this grid.
   //
   //Scaling factors
   //Any scaling factors are written in the file as 
   //<PARAMETER> XScaleFactor=...
   //<PARAMETER> YScaleFactor=...
   //However the coordinates written in the file are the unscaled factors
   //When the file is read back again, the unscaled coordinates are read in and
   //then the scaling is applied.

   gridfile << "# ASCII file generated by " << ClassName() <<
       "::WriteAsciiFile" << endl;
   gridfile << "# Grid Name : " << GetName() << endl;
   gridfile << "# This file can be read using " << ClassName() <<
       "::ReadAsciiFile" << endl;
   gridfile << "# " << endl;
   gridfile << "++" << ClassName() << endl;

   //write number lists
   if (!GetRingList().IsEmpty()) {
      gridfile << "<LIST> Rings=" << GetRingList().AsString() << endl;
   }
   if (!GetModuleList().IsEmpty()) {
      gridfile << "<LIST> Modules=" << GetModuleList().AsString() << endl;
   }
   if (!GetRunList().IsEmpty()) {
      gridfile << "<LIST> Runs=" << GetRunList().AsString() << endl;
   }
   //write parameters
   KVParameter<KVString> *par = 0;
   for( int i=0; i<GetParameters()->GetNPar(); i++) { //loop over all parameters
      par = GetParameters()->GetParameter(i);
      gridfile << "<PARAMETER> " << par->GetName() << "=" << par->GetVal().Data() << endl;
   }
   //remove scaling if there is one
   if (fLastScaleX != 1 || fLastScaleY != 1)
      Scale(1. / fLastScaleX, 1. / fLastScaleY);

   //Write ID lines
   TIter next_IDline(GetIDLines());
   KVIDLine *line;
   while ((line = (KVIDLine *) next_IDline())) {
      line->WriteAsciiFile(gridfile, "ID");
   }
   //Write OK lines
   TIter next_OKline(GetOKLines());
   while ((line = (KVIDLine *) next_OKline())) {
      line->WriteAsciiFile(gridfile, "OK");
   }
   gridfile << "!" << endl;

   //restore scaling if there is one
   if (fLastScaleX != 1 || fLastScaleY != 1)
      Scale(fLastScaleX, fLastScaleY);
}

//_______________________________________________________________________________________________//
void KVIDGChIoSi::WriteAsciiFile_WP(Int_t version)
{
// ecriture des grilles avec version suivant un modele de fichier (fPattern)
// fPattern_v%d.grid
// le modele doit etre initialiser par la methode SetPattern(TString)
// la valeur par default version=-1 permet l ecriture de la derniere version
// apres un check des versions existantes (methode CheckVersion)
// la premiere version est indexee v0
Int_t vv=0; if (version==-1) vv = CheckVersion(version)+1; 
WriteAsciiFile(fPattern+"_v"+Form("%d",vv)+".grid");
}
//_______________________________________________________________________________________________//
void KVIDGChIoSi::ReadAsciiFile_WP(Int_t version)
{
// lecture des grilles avec version suivant un modele de fichier
Int_t vv = CheckVersion(version);
if (vv==-1) { 
	if (version==-1) 	cout << "no file "+fPattern+"_v%d.grid" << endl; 
	else 					cout << "no file "+fPattern+Form("_v%d.grid",version) << endl;
}
else ReadAsciiFile(fPattern+"_v"+Form("%d",vv)+".grid");
}
//___________________________________________________________________________________//
Int_t KVIDGChIoSi::CheckVersion(Int_t version)
{
// version>=0 check si la version demandee du fichier grille existe
// version-1 renvoie la derniere version existante
// si la version demandee n existe pas return -1
// creation d un fichier tampon dans le repertoire ou se situe les fichiers grilles
// efface a la fin de la methode
Int_t temoin=-1;
TString pbis = fPattern,chemin = "";
TObjArray *tok=NULL;
if (fPattern.Contains("/")){		// prise en compte de fPattern contenant un chemin pour les fichiers
	tok = fPattern.Tokenize("/");
	Int_t nb = tok->GetEntries();
	pbis = ((TObjString*)(*tok)[nb-1])->GetString();
	chemin = ""; for (Int_t ii=0;ii<nb-1;ii+=1) chemin+=((TObjString*)(*tok)[ii])->GetString(); chemin+="/";
}
gROOT->ProcessLine(".! ls "+fPattern+"_v*.grid >> "+chemin+"listKVIDGChIoSI_"+pbis);
ifstream f_in(chemin+"listKVIDGChIoSI_"+pbis);
TString line="";
Int_t num=-2;
while (f_in.good() && num!=version) {
	line.ReadLine(f_in); if ( line.Contains(fPattern+"_v") && line.Contains(".grid") ){
		tok = line.Tokenize(".");
		line = ((TObjString*)(*tok)[0])->GetString();
		line.ReplaceAll(fPattern+"_v","template");
		tok = line.Tokenize("template");
		num = ((TObjString*)(*tok)[0])->GetString().Atoi();
		if (num>temoin) temoin=num;
	}
}
f_in.close();
gROOT->ProcessLine(".! rm -f "+chemin+"listKVIDGChIoSI_"+pbis);
delete tok;
if (version==-1) return temoin;
else if (temoin!=version) { return -1; }
else return temoin;
}
//___________________________________________________________________________________//
void KVIDGChIoSi::NewGridDialog(const TGWindow * p, const TGWindow * main,
                                UInt_t w, UInt_t h, Option_t * type)
{
   //create and return pointer to dialog box used by KVIDGridManagerGUI to create, modify, and
   //copy grids. for ChIo-Si grids it is a KVNewGridRRMDialog object
   new KVNewGridRRMDialog(p, main, w, h, type, this);
}

//___________________________________________________________________________________

void KVIDGChIoSi::Initialize()
{
   // General initialisation method for identification grid.
   // This method MUST be called once before using the grid for identifications.
   // The ID lines are sorted.
   // The natural line widths of all ID lines are calculated.
   // The line with the largest Z (Zmax line) is found.
   // Pointers to different 'OK lines' are initialised
   
   KVIDZGrid::Initialize();
   fSeuil = GetOKLine("Seuil_Si");
   fBragg = GetOKLine("Bragg_line");
   fPunch = GetOKLine("Punch_through");
   fEmaxSi = GetOKLine("Emax_Si");
}
