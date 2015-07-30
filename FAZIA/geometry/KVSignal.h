//Created by KVClassFactory on Mon Dec 22 15:46:46 2014
//Author: ,,,

#ifndef __KVSIGNAL_H
#define __KVSIGNAL_H

#include "TGraph.h"
#include "TArrayF.h"
#include "TH1F.h"

class KVPSAResult;

class KVSignal : public TGraph
{
public:
    enum SignalType
      {
          kQH1,
          kI1,
          kQL1,
          kQ2,
          kI2,
          kQ3,
          kADC,
          kUNKDT
      };


protected:
   Int_t fBlock;
   Int_t fQuartet;
   TString fQuartetName;
   Int_t fTelescope;
   Int_t fChannel;                 //! signal type (see KVSignal::SignalType enum)
   TString fTelName;
   TString fDetName;
   TString fType;
   TString fDet;

   TArrayF fAdc;                    //! needed to use the psa methods copied from FClasses of Firenze
   Double_t fAmplitude;             // amplitude of the signal
   Double_t fRiseTime;              // rise time of the signal
   Double_t fIMax;                  // position of the maximum in channel
   Double_t fTMax;                  // position of the maximum in ns
   Double_t fYmin,fYmax;            // raw min/max of the signal
   Double_t fBaseLine;              // base line mean value
   Double_t fSigmaBase;             // base line rms

   Double_t fChannelWidth;          // channel width in ns
   Double_t fChannelWidthInt;       // channel width of interpolated signal in ns
   Int_t fFirstBL, fLastBL;         // first and last channel number to compute the base line
   Double_t fTauRC;                 // tau_rc of the electronics. Used for pole zero cancellation.
   Double_t fTrapRiseTime;          // rise time of the trapezoidal shaper
   Double_t fTrapFlatTop;           // flat top of the trapezoidal shaper
   Double_t fGaussSigma;            // sigma of the semi-gaussian shaper
   Bool_t   fWithPoleZeroCorrection;// use or nor pole zero correction

 void init();
 
public:
   KVSignal();
   KVSignal(const char* name, const char* title);
   KVSignal(const TString& name, const TString& title);
   virtual ~KVSignal();  
   void Copy(TObject& obj) const;
	void SetDetectorName(const Char_t* detname);
   void DeduceFromName();
   void SetType(const Char_t* type) {fType=type;}
   void Print(Option_t* chopt = "") const;
   void SetData(Int_t nn, Double_t* xx, Double_t* yy);
   virtual void Set(Int_t n);
	
	void SetADCData();
   virtual KVPSAResult* TreateSignal();
   virtual void ComputeGlobals(void);
	Double_t GetRawAmplitude() const { return fYmax-fYmin; }
   virtual void LoadPSAParameters(const Char_t* ){  }
 virtual void SetDefaultValues(){}
	
   Double_t ComputeBaseLine();
   Double_t ComputeAmplitude();
   Double_t ComputeRiseTime();

   Int_t GetBlockNumber()           const { return fBlock; }
   Int_t GetQuartetNumber()         const { return fQuartet; }
   const Char_t* GetQuartetName()   const { return fQuartetName.Data(); }
   Int_t GetTelescopeNumber()       const { return fTelescope; }
   const Char_t* GetTelescopeName() const { return fTelName.Data(); }
   const Char_t* GetDetectorName()  const { return fDetName.Data(); }
   const Char_t* GetType()          const { return fType.Data(); }
   const Char_t* GetDetector()      const { return fDet.Data(); }

   void SetChannelWidth(double tau)                           {fChannelWidth=tau; fChannelWidthInt=tau;}
   void SetMaxT(double t)                                     {fAdc.Set((int)(t/fChannelWidth));}
   void SetNSamples(int nn)                                   {fAdc.Set(nn);}
   void SetBaseLineLength(Int_t length, Int_t first=0)        {fFirstBL=first; fLastBL=length-first;}
   void SetTauRC(Int_t taurc)                                 {fTauRC=taurc;}
   void SetTrapShaperParameters(Double_t rise, Double_t flat) {fTrapRiseTime=rise; fTrapFlatTop=flat;}
   void SetSemiGaussParameter(Double_t sig)                   {fGaussSigma=sig;}
   void SetPoleZeroCorrection(Bool_t with=kTRUE)              {fWithPoleZeroCorrection=with;}

   TArrayF* GetArray()       {return &fAdc;}
   Double_t GetChannelWidth(){return fChannelWidth;}
   Int_t    GetNSamples()    {return fAdc.GetSize();}
   Double_t GetTauRC()       {return fTauRC;}
   Double_t GetBLFirst()     {return fFirstBL;}
   Double_t GetBLLength()    {return fLastBL-fFirstBL;}
   Double_t GetTrapRiseTime(){return fTrapRiseTime;}
   Double_t GetTrapFlatTop() {return fTrapFlatTop;}
   Double_t GetGaussSigma()  {return fGaussSigma;}

   Double_t GetYmin() const   {return fYmin;}
   Double_t GetYmax() const   {return fYmax;}
   Double_t GetAmplitude()    {return fAmplitude;}
   Double_t GetRiseTime()     {return fRiseTime;}
   Double_t GetBaseLine()     {return fBaseLine;}
   Double_t GetSigmaBaseLine(){return fSigmaBase;}

   // compute mean value and rms of a subset of samples
   double FindMedia(double tsta, double tsto);
   double FindMedia(int tsta, int tsto);
   double FindSigma2(double tsta, double tsto);
   double FindSigma2(int tsta, int tsto);

   // multiply the signal  (modify only fAdc)
   void Multiply(Double_t fact);
   void Add(Double_t fact);

   // Interpolations
   Double_t FindTzeroCFDCubic(double level, int Nrecurr);
   double FindTzeroCFDCubic_rev(double level, double tend, int Nrecurr);
   Double_t CubicInterpolation(float *data, int x2, double fmax, int Nrecurr);
   virtual void BuildCubicSignal(double taufinal=1); //Interpolazione mediante cubic
   virtual double GetDataInter(double t);
   virtual double GetDataInterCubic(double t);

   // different shapers (modify only fAdc)
   void FIR_ApplyTrapezoidal(double trise, double tflat); // trise=sqrt(12)*tausha di CR-RC^4 se tflat=trise/2
   void FIR_ApplySemigaus(double tau_usec);
   void FIR_ApplyRCLowPass(double time_usec, int reverse=0);
   void FIR_ApplyRCHighPass(double time_usec, int reverse=0);
   void FIR_ApplyRecursiveFilter(double a0, int N, double *a, double *b, int reverse);
   void PoleZeroSuppression(Double_t tauRC);

   // fast fourier transform and windowing of the signal (modify only fAdc)
   void ApplyWindowing(int window_type=3); // 0: barlett, 1:hanning, 2:hamming, 3: blackman
   static int FFT(unsigned int p_nSamples, bool p_bInverseTransform, double *p_lpRealIn, double *p_lpImagIn, double *p_lpRealOut, double *p_lpImagOut); // nsamples: power of 2
   int FFT(bool p_bInverseTransform, double *p_lpRealOut, double *p_lpImagOut);
   TH1* FFT2Histo(int output, TH1* hh=0);  // 0 modulo, 1 modulo db (normalized), 2, re, 3 im

   // apply modifications of fAdc to the original signal
   void ApplyModifications(TGraph* newSignal=0, Int_t nsa=-1);

   ClassDef(KVSignal,2)//simple class to store TArray in a list
};

#endif
