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
	protected:
   Int_t fBlock;
   Int_t fQuartet;
   TString fQuartetName;
   Int_t fTelescope;
   TString fTelName;
   TString fDetName;
   TString fType;
   TString fDet;

   TArrayF fAdc;                    //! needed to use the psa methods copied from FClasses of Firenze
   Double_t fAmplitude;             // amplitude of the signal
   Double_t fIMax;                  // position of this maximum in channel
   Double_t fTMax;                  // position of this maximum in ns

   Double_t fYmin,fYmax;
   Double_t fChannelWidth;          //  channel width in ns
   Int_t fFirstBL, fLastBL;         //  first and last channel number to compute the base line

   public:
   KVSignal();
   KVSignal(const char* name, const char* title);
   KVSignal(const TString& name, const TString& title);
   virtual ~KVSignal();
   void Copy(TObject& obj) const;
   void Init();
	
   void SetData(Int_t nn, Double_t* xx, Double_t* yy);
   virtual KVPSAResult* TreateSignal(void);
   void DeduceFromName();
   
   void Print(Option_t* chopt = "") const;
   Int_t GetBlockNumber() const { return fBlock; }
   Int_t GetQuartetNumber() const { return fQuartet; }
   const Char_t* GetQuartetName() const { return fQuartetName.Data(); }
   Int_t GetTelescopeNumber() const { return fTelescope; }
   const Char_t* GetTelescopeName() const { return fTelName.Data(); }
   const Char_t* GetDetectorName() const { return fDetName.Data(); }
   const Char_t* GetType() const { return fType.Data(); }
   const Char_t* GetDetector() const { return fDet.Data(); }
   
   void SetType(const Char_t* type) {fType=type;}

   virtual void ComputeGlobals(void);
   Double_t GetYmin() const { return fYmin; }
   Double_t GetYmax() const { return fYmax; }
   
   TArrayF* GetArray(){return &fAdc;}
   Double_t GetChannelWidth(){return fChannelWidth;}
   Int_t GetNSamples(){return fAdc.GetSize();}

   void SetChannelWidth(double tau){fChannelWidth=tau;}
   void SetMaxT(double t){fAdc.Set((int)(t/fChannelWidth));}
   void SetNSamples(int nn){fAdc.Set(nn);}
   void Multiply(Double_t fact);

   double FindMedia(double tsta, double tsto);
   double FindSigma2(double tsta, double tsto);

//   Double_t ComputeGlobals();
   Double_t GetAmplitude();
   Double_t GetRiseTime();

   Double_t FindTzeroCFDCubic(double level, int Nrecurr);
   double FindTzeroCFDCubic_rev(double level, double tend, int Nrecurr);
   Double_t CubicInterpolation(float *data, int x2, double fmax, int Nrecurr);

   void FIR_ApplyTrapezoidal(double trise, double tflat); // trise=sqrt(12)*tausha di CR-RC^4 se tflat=trise/2
   void FIR_ApplySemigaus(double tau_usec);
   void FIR_ApplyRCLowPass(double time_usec, int reverse=0);
   void FIR_ApplyRCHighPass(double time_usec, int reverse=0);
   void FIR_ApplyRecursiveFilter(double a0, int N, double *a, double *b, int reverse);

   void ApplyWindowing(int window_type=3); // 0: barlett, 1:hanning, 2:hamming, 3: blackman

   static int FFT(unsigned int p_nSamples, bool p_bInverseTransform,
          double *p_lpRealIn, double *p_lpImagIn,
          double *p_lpRealOut, double *p_lpImagOut); // nsamples: power of 2
   int FFT(bool p_bInverseTransform, double *p_lpRealOut, double *p_lpImagOut);
   TH1* FFT2Histo(int output, TH1* hh=0);  // 0 modulo, 1 modulo db (normalized), 2, re, 3 im

   void ApplyModifications(TGraph* newSignal=0);

   ClassDef(KVSignal,2)//simple class to store TArray in a list
};

#endif
