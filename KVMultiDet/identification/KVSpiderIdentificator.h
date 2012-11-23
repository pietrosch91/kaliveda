
#ifndef __KVSPIDERIDENTIFICATOR_H
#define __KVSPIDERIDENTIFICATOR_H

#include <TH2F.h>
#include <TGraph.h>
#include <TFile.h>
#include <TMath.h>
#include <TNamed.h>
#include <TRandom3.h>
#include "TSpectrum.h"
#include <TROOT.h>
#include "KVDroite.h"
#include "KVSpiderLine.h"

#include <Riostream.h>
#include <list>

class KVSpiderIdentificator : public TNamed
{
  protected:
  
  bool _is_initialized;
  bool _debug;
  bool _auto;
  bool _sicsi;
  bool _useFit;
  
  double _bfactor;
  double _ftheta;
  double _otheta;
  double _x0;
  double _y0;
  double _xm;
  double _ym;
  double _xmax;
  double _ymax;
  double _alpha;
  
  TList  _llist;
  TList  _dlist;
  TList  _hlist;
  TList  _plist;
  
  TRandom   _alea;
  TSpectrum _ss;
  
  KVSpiderLine* _spline;  
  TGraph*       _invalid;
  KVDroite*     _dtemp;
  
  TH2F* _htot;
  TH1F* _hfound;
  TH1F* _hvalid;
  TH2F* _htemp;
  

  public:
  
  KVSpiderIdentificator();
  KVSpiderIdentificator(TH2F* h_, Double_t Xm=-1, Double_t Ym=-1);
  virtual ~KVSpiderIdentificator();
  
  void Init(TH2F* h_=0, Double_t Xm=-1, Double_t Ym=-1);
  void Close();
  void Clear(Option_t* option = "");
  void SetMode(bool sicsi_=true){_sicsi=sicsi_;};
  void SetParameters(double bining_=1.);
  
  void SetHistogram(TH2F* h_=0, Double_t Xm=-1, Double_t Ym=-1);
  TH1F* GetProjection(TH2F* h_, KVDroite* d_, int rebin_=10);
  bool SearchPeack(TH1F* h1_, double theta_, int create_, double sigma_=2., double peakmin_=1., int rebin_=10, int smooth_=5, TString opt_="goff");
  
  bool ProcessIdentification();
  bool GetLines(int npoints_=1, double alpha_=1.);
  
  TList* CreateHistograms(double thmin_, double thmax_, int nth_, bool cos_=true, double alpha_=-1.);
  TH2F* CreateHistogram(double th_, double alpha_=-1.);
  
  void Draw(Option_t* opt_ = "");
  void SaveAsPdf(Option_t* opt_="", char* path_=".");
  
  TList* GetListOfLines();
  KVSpiderLine* GetLine(int z_);

  void SetX0(double x0_){_x0=x0_;};
  void SetY0(double y0_){_y0=y0_;};
  void SetXm(double xm_){_xm=xm_;};
  void SetYm(double ym_){_ym=ym_;};
  void UseFit(bool fit){_useFit=fit;};

  double GetX0(){return _x0;};
  double GetY0(){return _y0;};
  double GetXm(){return _xm;};
  double GetYm(){return _ym;};
  double GetTheta0(){return _ftheta;};
  void CalculateTheta();

  protected :
  
  void SetDefault();
  bool TestHistogram(TH1* h_=0);
  bool CheckPath(char* path_);
      
  ClassDef(KVSpiderIdentificator,1) // a faire !  
};

#endif









