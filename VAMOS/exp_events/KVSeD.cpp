//Created by KVClassFactory on Mon Sep  3 11:28:25 2012
//Author: Guilain ADEMARD

#include "KVSeD.h"
#include "KVFunctionCal.h"
#include "TH1F.h"
#include "TFitResult.h"
#include "TMatrixD.h"

ClassImp(KVSeD)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSeD</h2>
<h4>Secondary Electron Detector, used at the focal plan of VAMOS</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVString KVSeD::fACQParamTypes("0:Q, 1:T_HF, 2:T, 9:NO_TYPE");
KVString KVSeD::fPositionTypes("0:X, 1:Y, 2:Z, 3:XY, 9:NO_TYPE");

void KVSeD::init()
{
   // Initialise non-persistent pointers

   fQ  = new TH1F** [3];
   for (Int_t i = 0; i < 3; i++) { //[ raw, calibrated, clean ]
      fQ[i] = new TH1F*[2];
      for (Int_t j = 0; j < 2; j++) { //[ X, Y ]
         fQ[i][j]      = NULL;
      }
   }

   fPeakFunction = NULL;
   fRawPos [0] = fRawPos [1] = -500;
   fERawPos[0] = fERawPos[1] = -500;
// fSpectrum     = NULL;
   fPosCalib = NULL;

   //a KVSeD can not be used in a ID telescope
   ResetBit(kOKforID);
}
//________________________________________________________________

KVSeD::KVSeD()
{
   // Default constructor
   init();
   SetType("SED");
   SetLabel("SED");
   SetName(GetArrayName());

}
//________________________________________________________________

KVSeD::KVSeD(const KVSeD& obj)  : KVVAMOSDetector()
{
   // Copy constructor
   // This ctor is used to make a copy of an existing object (for example
   // when a method returns an object), and it is always a good idea to
   // implement it.
   // If your class allocates memory in its constructor(s) then it is ESSENTIAL :-)

   obj.Copy(*this);
}
//________________________________________________________________

KVSeD::~KVSeD()
{
   // Destructor
   for (Int_t i = 0; i < 3; i++) {
      for (Int_t j = 0; j < 2; j++) {
         SafeDelete(fQ[i][j]);
      }
      delete[] fQ[i];
   }
   delete[] fQ;
   SafeDelete(fPeakFunction);
}
//________________________________________________________________

void KVSeD::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVSeD::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   KVVAMOSDetector::Copy(obj);
   //KVSeD& CastedObj = (KVSeD&)obj;
}
//________________________________________________________________

void  KVSeD::Clear(Option_t* option)
{
   // Set the energy losses etc. to zero and reset the histograms used
   // to find the X,Y positions (Qraw and Q)

   KVVAMOSDetector::Clear(option);
   ResetCalculatedData();
}
//________________________________________________________________

const Char_t* KVSeD::GetArrayName()
{
   // Name of detector given in the form
   // SED1 SED2 ...
   // to be compatible GANIL acquisition parameters
   //
   // The root of the name is the detector type + number.

   if (GetNumber() > 0) {
      fFName  = Form("%s%d", GetType(), GetNumber());
      SetLabel(fFName.Data());
   } else fFName = GetType();
   return fFName.Data();
}
//________________________________________________________________

const Char_t* KVSeD::GetTBaseName() const
{
   // Base name of the time of flight parameters used to be compatible
   // GANIL acquisition parameters
   //
   // The root of the name is the detector name.

   return GetName();
}
//________________________________________________________________

Float_t KVSeD::CalculateQThreshold(const Char_t dir)
{
   // Calculate the noise in order to remove it on strips for X and
   // Y positions.

   //Study the noise and the maximum value for the charge.
   //At this stage the quantities noise_*** contain also the signal!
   Float_t QTmp, noise_mean, noise_var, noise_stdev, noise_sum;
   Float_t Q_max;
   Q_max = noise_mean = noise_var = noise_sum = noise_stdev = 0.;

   UShort_t Q_max_Nr;
   Q_max_Nr = 1000 ;

   TH1F* hQcal = GetQHisto(dir);
   if (!hQcal) return 0;

   for (Int_t j = 1; j <= hQcal->GetNbinsX(); j++) { //loop over strips
      QTmp = hQcal->GetBinContent(j);
      if (QTmp > 0) {
         noise_mean += QTmp;
         noise_var  += QTmp * QTmp;
         noise_sum  += 1.;
      }
      if (QTmp > Q_max) {
         Q_max    = QTmp;
         Q_max_Nr = j;
      }
   }

   //Study the surrounding of the maximum charge
   Float_t max_mean, max_var, max_sum;
   max_mean = max_var = max_sum = 0.;
   for (Int_t j = -Border; j <= Border; j++) {
      Int_t k = Q_max_Nr + j;
      if ((0 < k) && (k <= hQcal->GetNbinsX())) {
         QTmp = hQcal->GetBinContent(k);
         if (QTmp > 0) {
            max_mean  += QTmp;
            max_var   += QTmp * QTmp;
            max_sum   += 1.;
         }
      }
   }
   //The signal of the maximum charge is removed from the quantities noise_***
   noise_mean -= max_mean;
   noise_var  -= max_var;
   noise_sum  -= max_sum;
   //Calculate statistical quntities related to the noise and fix the threshold
   //as the mean of the noise + 1 time its standart deviation
   if (noise_sum < 2. || noise_var == 0) {
      noise_mean = noise_var = 0;
   } else {
      noise_mean /= noise_sum;
      noise_var   = noise_var / noise_sum - noise_mean * noise_mean;
      noise_stdev = TMath::Sqrt(noise_var);
   }
   Float_t threshold = noise_mean + noise_stdev;

   return threshold;
}
//________________________________________________________________

TH1F* KVSeD::GetCleanQHisto(const Char_t dir)
{
   // Retruns the X or Y position histogram with calibrated charges (Q)
   // after the substration of noise.

   Int_t i = IDX(dir);
   if ((i < 0) || (i > 2)) return NULL;
   if (!fQ[2][i]) return NULL;
   if (fQ[2][i]->GetEntries()) return fQ[2][i];

   TH1F* hQcal = GetQHisto(dir);
   if (!hQcal) return NULL;

   Float_t threshold = CalculateQThreshold(dir);

   for (Int_t j = 1; j <= hQcal->GetNbinsX(); j++) { //loop over strips
      Double_t QTmp = hQcal->GetBinContent(j);
      if (QTmp > threshold) {
         fQ[2][i]->SetBinContent(j, QTmp - threshold);
      }
   }

   return fQ[2][i];
}
//________________________________________________________________

TH1F* KVSeD::GetQrawHisto(const Char_t dir)
{
   // Retruns the X or Y position histogram with raw charges (Q)

   Int_t i = IDX(dir);
   if ((i < 0) || (i > 2)) return NULL;
   if (!fQ[0][i]) return NULL;
   if (fQ[0][i]->GetEntries()) return fQ[0][i];

   fQ[0][i]->Reset();
   TIter next(GetACQParamList());

   Bool_t ok = kFALSE;
   KVACQParam* par  = NULL;
   while ((par = (KVACQParam*)next())) {

      if ((par->GetLabel()[0] != dir) || !par->Fired("P")) continue;

      Float_t data;
      fQ[0][i]->SetBinContent(par->GetNumber(), data = par->GetData());
      ok = kTRUE;
   }

   if (!ok) Warning("GetQrawHisto", "No ACQ parameter fired for the %c position", dir);

   return fQ[0][i];
}
//________________________________________________________________

TH1F* KVSeD::GetQHisto(const Char_t dir)
{
   // Retruns the X or Y position histogram with calibrated charges (Q)

   Int_t i = IDX(dir);
   if ((i < 0) || (i > 2)) return NULL;
   if (!fQ[1][i]) return NULL;
   if (fQ[1][i]->GetEntries()) return fQ[1][i];

   fQ[1][i]->Reset();
   TIter next(GetListOfCalibrators());

   Bool_t ok = kFALSE;
   KVCalibrator* cal  = NULL;
   KVACQParam*   par  = NULL;
   Int_t num;

   Int_t count_calOK = 0, count_fired = 0;
   while ((cal = (KVCalibrator*)next())) {


      if (!cal->GetStatus() || (GetPositionTypeIdxFromID(cal->GetUniqueID()) != i)) continue;
      count_calOK++;

      KVFunctionCal* calf = (KVFunctionCal*)cal;
      par = calf->GetACQParam();
      if (!par->Fired("P")) continue;
      count_fired++;

      num =  calf->GetNumber();
      fQ[1][i]->SetBinContent(num, calf->Compute());
      ok = kTRUE;
   }

   if (!ok) Warning("GetQHisto", "Impossible to calibrate correctly the charges for %c position of %s, %d calibrators are OK and %d fired ACQ parameters", dir, GetName(), count_calOK, count_fired);

   return fQ[1][i];
}
//________________________________________________________________

void KVSeD::Initialize()
{
   // Initialize the data members. Called by KVVAMOS::Initialize().
   ResetCalculatedData();
}
//________________________________________________________________

Bool_t KVSeD::PositionIsOK()
{
   // Returns true if the position can be given by this SeD, i.e. the
   // position is callibrated, the raw X and Y positions are well
   // determined. In this case the position is given by the method
   // GetPosition(...).
   if (!IsPositionCalibrated()) return kFALSE;
   if (GetRawPosition('X') < 0) return kFALSE;
   if (GetRawPosition('Y') < 0) return kFALSE;
   return kTRUE;
}
//________________________________________________________________

void KVSeD::ResetCalculatedData()
{
   // Reset all X or Y position histograms (raw, calibrated, clean)
   // and raw positions.
   // This method have to be called each time you read a new event (i.e.
   // when the ACQ parameter change) otherwise certain methods
   // (such as GetPosition(), GetQrawHisto() ...) will not return
   // what you expect.
   for (Int_t i = 0; i < 3; i++) {
      for (Int_t j = 0; j < 2; j++) {
         if (fQ[i][j]) fQ[i][j]->Reset();
      }
   }

   fRawPos [0] = fRawPos [1]  = -500;
   fERawPos[0] = fERawPos[1] = -500;
}
//________________________________________________________________

void KVSeD::SetACQParams()
{
   // Setup the position acquistion parameters to the SeD.
   // These ACQ parameters have the type 'Q' (for charge).
   // Their name has the form: [detector type][detector number]_[X,Y]_[128,48]
   // where 128 is the number of strips for the X measurment and 48
   // the number of 48 channels, each connecting 3 wires for the Y
   // measurment (A. Drouart et al Nuc. Instr. Meth A 579 (2007) 1090).
   //
   // Number of each ACQ parameters:
   //       SED1_X_001  --> 1001
   //       SED1_X_002  --> 1002
   //              :
   //       SED1_X_128  --> 1128
   //       SED1_Y_001  --> 2001
   //       SED1_Y_002  --> 2002
   //              :
   //       SED1_Y_128  --> 2128


   // Charge ACQ parameters
   TString name, title;
   KVACQParam* par = NULL;
   Int_t Nstrips[] = { 128,  48 };
   const Char_t* extraname[]  = {    "raw", "calib",        "clean"};
   const Char_t* extratitle[] = {       "",     "", "without noise"};
   const Char_t* units[]      = {"channel", "Volt",         "Volt"};
   for (Int_t i = 0; i < 2; i++) {
      for (Int_t num = 1; num <= Nstrips[i]; num++) {
         par = new KVACQParam;
         name.Form("%s_%c_%03d", GetArrayName(), DIRECTION(i), num);
         par->SetName(name);
         par->SetType("Q");
         par->SetLabel(Form("%c", DIRECTION(i)));
         par->SetNumber(num);
         par->SetUniqueID(CalculateUniqueID(par));
         AddACQParam(par);
      }

      // build  Q histograms for X and Y position
      for (Int_t j = 0; j < 3; j++) {
         // Qraw histogram
         if (fQ[j][i]) SafeDelete(fQ[j][i]);
         name.Form("Q%s_%s_%c", extraname[j], GetArrayName(), DIRECTION(i));
         title.Form("%c position for %s %s; Strip number; Qraw (%s)", DIRECTION(i), extratitle[j], GetArrayName(), units[j]);
         fQ[j][i] = new TH1F(name.Data(), title.Data(), Nstrips[i], .5, Nstrips[i] + .5);
         fQ[j][i]->SetDirectory(0);
         fQ[j][i]->SetLineColor(j + 1);
      }
   }
}
//________________________________________________________________

void KVSeD::SetCalibrators()
{
   // Setup the calibrators for this SeD detector. Call once name
   // has been set.
   // The calibrators are KVFunctionCal (see KVVAMOSDetector::SetCalibrators())
   // except for the position calibration (position->cm) where a
   // KVSeDPositionCal object is used.

   KVVAMOSDetector::SetCalibrators();
   KVSeDPositionCal* c = new KVSeDPositionCal(this);
   c->SetUniqueID(CalculateUniqueID(c));
   if (!AddCalibrator(c)) delete c;
   else fPosCalib = c;

}
//________________________________________________________________

void KVSeD::ShowCleanQHisto(const Char_t dir, Option_t* opt)
{
   TH1F* hh = GetCleanQHisto(dir);
   if (hh) hh->Draw(opt);
}
//________________________________________________________________

void KVSeD::ShowQrawHisto(const Char_t dir, Option_t* opt)
{
   TH1F* hh = GetQrawHisto(dir);
   if (hh) hh->Draw(opt);
}
//________________________________________________________________

void KVSeD::ShowQHisto(const Char_t dir, Option_t* opt)
{
   TH1F* hh = GetQHisto(dir);
   if (hh) hh->Draw(opt);
}
//________________________________________________________________

Double_t KVSeD::GetRawPosition2(const Char_t dir, Double_t min_amp, Double_t min_sigma, Double_t max_sigma, Int_t)
{
   // Return the position (strip) deduced from the histogram representing
   // the calibrated charge versus strip number. First the method searchs
   // peaks. If there is to many peaks (>maxNpeaks) the method returns -1
   // else the function
   //   f(x) = [0]*exp(-0.5*((x-[1])/[2])**2)+[3]
   // is fitted to the histogram by setting initial parameters to
   // fit the main peak found. The position is given by the parameter[1].
   // argument:  min_amp - acceptable minimum for the amplitude
   //                      parameter [0]
   //            min_sigma and max_sigma - acceptable minimum and maximum
   //                                      for the sigma parameter [2]
   //
   // If these two parameter are not in the limits then the method returns -1.
   Int_t idx = IDX(dir);
   if (fRawPos[ idx ] > -500) return fRawPos[ idx ];
   fRawPos [ idx ] = -1;
   fERawPos[ idx ] = -1;
   TH1F* hh = GetQHisto(dir);
   if (!hh) return fRawPos[ idx ];

   ///////////////////////////////////////////////////
// if( !fSpectrum ) fSpectrum = new TSpectrum(15);
   if (!fPeakFunction) {
      fPeakFunction = new TF1(Form("KVSeD:%s:PeakFunction", GetName()), "gaus+[3]", 0, 129);
      fPeakFunction->SetNpx(128);
      fPeakFunction->SetLineColor(kBlue);
   }

   Double_t par[4];
   TFitResultPtr r = hh->Fit("pol0", "QSNC");
   if (!r.Get()) return fRawPos[ idx ];
   par[3] = r->Parameter(0);       // noise
// Int_t nfound = fSpectrum->Search(hh,2,"goff",0.2);
// if( (nfound<1) || (maxNpeaks< nfound) ) return -1;
// Double_t *xpeaks = fSpectrum->GetPositionX();
// Info("GetRawPosition2","Number of peaks found: %d", nfound);
   par[2] = 2.;                   // sigma
   Int_t locmax = hh->GetMaximumBin();
   Double_t max = hh->GetBinContent(locmax);

// par[1] = xpeaks[0];            // position
   par[1] = (Double_t)locmax;            // position
   par[0] = TMath::Abs(max - par[3]); // amplitude
   fPeakFunction->SetParameters(par);

   fPeakFunction->SetParLimits(0, 0., max);
   fPeakFunction->SetParLimits(1, 1., hh->GetNbinsX());
   fPeakFunction->SetParLimits(2, 0.5, max_sigma * 1.1);
   fPeakFunction->SetParLimits(3, 0., max);

   fPeakFunction->SetParError(0, par[0] * 0.2);
   fPeakFunction->SetParError(1, 1);
   fPeakFunction->SetParError(2, 0.5);
   fPeakFunction->SetParError(3, max * .2);

   hh->Fit(fPeakFunction, "QN", "", locmax - Border, locmax + Border);
// hh->Fit(fPeakFunction,"","",locmax-Border,locmax+Border);

   par[0] = fPeakFunction->GetParameter(0);
   par[1] = fPeakFunction->GetParameter(1);
   par[2] = fPeakFunction->GetParameter(2);

   if (par[0] < min_amp) return fRawPos[ idx ];
   if ((par[2] < min_sigma) || (max_sigma < par[2])) return fRawPos[ idx ];

   fRawPos [ idx ] = par[1];
   fERawPos[ idx ] = par[2] * 1.2;
   return fRawPos[ idx ];
}
//________________________________________________________________

Double_t KVSeD::GetRawPosition(const Char_t dir)
{
   // Return the position (strip) deduced from the histogram representing
   // the calibrated charge versus strip number. Faster method compare to
   // the method GetRawPosition2. The resolution is less good but sufficient.

   Int_t idx = IDX(dir);
   if (fRawPos[ idx ] > -500) return fRawPos[ idx ];
   fRawPos [ idx ] = -1;
   fERawPos[ idx ] = -1;
   TH1F* hh = GetCleanQHisto(dir);
   if (!hh) return fRawPos[ idx ];

   Int_t NStrips = 3;
   if (hh->GetEntries() < NStrips) return fRawPos[ idx ];

   ///////////////////////////////////////////////////

   Int_t binMax = hh->GetMaximumBin();
   Int_t min;
   for (min = binMax; min > 1; min--) {
      Double_t deltaQ = hh->GetBinContent(min) - hh->GetBinContent(min - 1);
      if (deltaQ < 0) break;
   }
   Int_t max;
   for (max = binMax; max < hh->GetNbinsX(); max++) {
      Double_t deltaQ = hh->GetBinContent(max) - hh->GetBinContent(max + 1);
      if (deltaQ < 0) break;
   }

   if ((max - min) < NStrips) return fRawPos[ idx ];

   hh->GetXaxis()->SetRange(min, max);

   fRawPos [ idx ] = hh->GetMean();
   fERawPos[ idx ] = hh->GetRMS() * 1.2;
   hh->GetXaxis()->SetRange();
   return fRawPos[ idx ];
}
//________________________________________________________________

UChar_t KVSeD::GetRawPosition(Double_t* XYZf)
{
   // Returns in the 'XYZf' array the X and Y coordinates of the position (strip)
   // deduced from the histogram representing the calibrated charge versus strip
   // number. The bit 0 (1) of the UChar_t returned value is set to 1 if
   // the X (Y) position is correctly deduced.

   UChar_t rval = 3;
   if ((XYZf[0] = GetRawPosition('X')) < 0) rval -= 1;
   if ((XYZf[1] = GetRawPosition('Y')) < 0) rval -= 2;
   return rval;
}
//________________________________________________________________

Double_t KVSeD::GetRawPositionError(const Char_t dir)
{
   // Returns the error on the position (strip) returned by GetRawPosition( dir ).

   Int_t idx = IDX(dir);
   if (fERawPos[ idx ] > -500) return fERawPos[ idx ];

   GetRawPosition(dir);
   return fERawPos[ idx ];
}
//________________________________________________________________

UChar_t KVSeD::GetRawPositionError(Double_t* EXYZf)
{
   // Returns in the 'EXYZf' array the errors of X and Y coordinates of the position
   // returned by GetRawPosition(...).
   // The bit 0 (1) of the UChar_t returned value is set to 1 if
   // the X (Y) position is correctly deduced.

   UChar_t rval = 3;
   if ((EXYZf[0] = GetRawPositionError('X')) < 0) rval -= 1;
   if ((EXYZf[1] = GetRawPositionError('Y')) < 0) rval -= 2;
   return rval;
}
//________________________________________________________________

UChar_t KVSeD::GetPosition(Double_t* XYZf, Int_t)
{
   // Get calibrated and deviation-corrected positions Xf, Yf and Zf (in cm)
   // in the focal plan reference frame from the raw positions in channel
   // obtained with GetRawPosition(...). The argument 'XYZf' has to be an
   // array of size 3 i.e. XYZf[3].
   // The 3 first bits of the returned UChar_t value give an information about
   // the coordinates well determined, calibrated and corrected. For example is the Y
   // coordinate, with the indice 1, is good then the bit 1 is set to 1.
   // See documentation of KVSeDPositionCal for more details about
   // calibration algorithm.

   XYZf[0] = XYZf[1] = XYZf[2] = -666;
   // Nothing is done if there is no calibrator for the position
   if (!IsPositionCalibrated()) return 0;

   Double_t Xraw = GetRawPosition('X');
   Double_t Yraw = GetRawPosition('Y');

   // Calibration is performed with the calibrator KVSeDPositionCal
   // The calibrator return kFALSE if at least one of the raw coordinates is
   // less of equal to zero

   Bool_t  OK     = kFALSE;
   UChar_t rvalue = 0;       // returned value;
   XYZf[2]        = 0.;      // Z = 0 in the reference frame of the
   // active volule

   if ((Xraw > 0) && (Yraw > 0)) {
      OK = (*fPosCalib)(Xraw, Yraw, XYZf[0], XYZf[1]);
      OK = (OK && ActiveVolumeToFocal(XYZf, XYZf));
      rvalue = 7; // X,Y,Z are OK
   } else if (Xraw > 0) {
      Yraw = 24;  // we impose the center in Y (48/2) for the X calibration
      OK = (*fPosCalib)(Xraw, Yraw, XYZf[0], XYZf[1]);
      OK = (OK && ActiveVolumeToFocal(XYZf, XYZf));
      XYZf[1] = -666;
      rvalue = 5; // X and Z are OK
   } else if (Yraw > 0) {
      Xraw = 64;  // we impose the center in X (128/2) for the Y calibration
      OK = (*fPosCalib)(Xraw, Yraw, XYZf[0], XYZf[1]);
      OK = (OK && ActiveVolumeToFocal(XYZf, XYZf));
      XYZf[0] = -666;
      rvalue = 6; // Y and Z are OK
   }

   if (OK) return rvalue;

   // No coordinates are OK
   XYZf[0] = XYZf[1] = XYZf[2] = -666;
   return 0;
}
//________________________________________________________________

void KVSeD::GetDeltaXYZf(Double_t* DXYZf, Int_t)
{
   // Returns in the DXYZf array the errors of each coordinate of the position returned by
   // GetPosition(...) in the focal-plane frame of reference.

   UChar_t  res = GetRawPositionError(DXYZf);
   DXYZf[0] = (res & 1 ?  DXYZf[0] * 0.32 : -1); // width of stips:   0.32cm
   DXYZf[1] = (res & 2 ?  DXYZf[1] * 0.32 : -1); // width of 3 wires: 0.32cm
   DXYZf[2] = DXYZf[1];
}
