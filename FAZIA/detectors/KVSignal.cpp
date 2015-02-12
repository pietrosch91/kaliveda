//Created by KVClassFactory on Mon Dec 22 15:46:46 2014
//Author: ,,,

#include "KVSignal.h"
#include "KVString.h"
#include "TMath.h"
#define LOG2 (double)6.93147180559945286e-01
# define M_PI		3.14159265358979323846	/* pi */

ClassImp(KVSignal)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSignal</h2>
<h4>simple class to store TArray in a list</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVSignal::KVSignal()
{
   // Default constructor
   fYmin = fYmax = 0;
}

//________________________________________________________________

KVSignal::KVSignal(const char* name, const char* title) : TGraph()
{
   // Write your code here
   fYmin = fYmax = 0;
   SetNameTitle(name,title);
}

//________________________________________________________________

KVSignal::KVSignal(const TString& name, const TString& title) : TGraph()
{
   // Write your code here
   fYmin = fYmax = 0;
   SetNameTitle(name,title);
}

KVSignal::~KVSignal()
{
   // Destructor
}

//________________________________________________________________

void KVSignal::Copy(TObject& obj) const
{
   // This method copies the current state of 'this' object into 'obj'
   // You should add here any member variables, for example:
   //    (supposing a member variable KVSignal::fToto)
   //    CastedObj.fToto = fToto;
   // or
   //    CastedObj.SetToto( GetToto() );

   //TGraph::Copy((TGraph&)obj);
    //KVSignal& CastedObj = (KVSignal&)obj;
}

void KVSignal::Init()
{
    if(!GetN())
    {
        Error("Init","Please fill me (%s) before initialisation ...",GetName());
        return;
    }
    if(fType.Contains("I")||fType.Contains("QL1")) fChannelWidth = 4.;
    else                                           fChannelWidth = 10.;
    fAdc.Set(GetN());
    for(int ii=0; ii<GetN(); ii++) fAdc.AddAt(fY[ii],ii);
}

//________________________________________________________________

void KVSignal::SetData(Int_t nn, Double_t* xx, Double_t* yy)
{
	Set(nn);
   if (nn==0) return;
   Int_t np=0;
   fYmin=fYmax=yy[np];
   for (np=1;np<nn;np+=1){
   	SetPoint(np,xx[np],yy[np]);
      if (yy[np]<fYmin) fYmin = yy[np];
      if (yy[np]>fYmax) fYmax = yy[np];
	}
}

//________________________________________________________________
void KVSignal::DeduceFromName()
{

	fBlock = fQuartet = fTelescope = -1;
   fType = fDet = fTelName = fQuartetName = "";
	KVString tmp = GetName();
   KVString part = "";
   if (tmp.BeginsWith("B"))
   {
   	//FAZIA telescope denomination
   	tmp.Begin("-");
      part = tmp.Next(); part.ReplaceAll("B",""); fBlock = part.Atoi();
		part = tmp.Next(); part.ReplaceAll("Q",""); fQuartet = part.Atoi();
		part = tmp.Next(); part.ReplaceAll("T",""); fTelescope = part.Atoi();
		fType = tmp.Next();
      fDet = GetTitle(); fDet.ToUpper();
      
      fDetName.Form("%s-T%d-Q%d-B%03d",fDet.Data(),fTelescope,fQuartet,fBlock);
		fTelName.Form("B%03d-Q%d-T%d",fBlock,fQuartet,fTelescope);
     fQuartetName.Form("B%03d-Q%d",fBlock,fQuartet);
   }
   
}
//________________________________________________________________
void KVSignal::Print(Option_t* chopt) const
{
	Info("Print","\nName: %s - Title: %s",GetName(),GetTitle());
	if (fBlock!=-1){
   	printf("\tBlock# %d - Quartet# %d - Telescope# %d\n",fBlock,fQuartet,fTelescope);
   	printf("\tType: %s - Detecteur: %s\n",fType.Data(),fDet.Data());
   }
}
//________________________________________________________________

KVPSAResult* KVSignal::TreateSignal(void)
{
	//to be implemented in child class
   Info("TreateSignal","To be implemented in child classes");
	return 0;
}

//________________________________________________________________

void KVSignal::ComputeGlobals(void)
{
   Double_t xx,yy;
   Int_t np=0;
   GetPoint(np++,xx,yy);
   fYmin=fYmax=yy;
   
   for (np=1;np<GetN();np+=1){
   	GetPoint(np,xx,yy);
      if (yy<fYmin) fYmin = yy;
      if (yy>fYmax) fYmax = yy;
   }
}

double KVSignal::FindMedia(double tsta, double tsto)
{
// Calcolo della media nel tratto tra tsta e tsto.
// NOTA: questo ha senso solo se il segnale e' piatto in quella regione!!

//#ifdef DEBUG
//  printf("--- FSignal::FindMedia(double tsta, double tsto) ---\n");
//#endif
  int n1=(int)(tsta/fChannelWidth);// Non molto preciso, ma tant'e'...
  int n2=(int)(tsto/fChannelWidth);

  int N=n2-n1+1;
 //// printf("n1=%d, n2=%d, n=%d, fChannelWidth=%e \n",n1, n2, N, fChannelWidth);
  if(n1<0 || n1>=fAdc.GetSize() ||
     n2<n1|| n2>=fAdc.GetSize() ||
     N<=0 || N>=fAdc.GetSize() )
    {
      printf("--- FSignal::FindMedia:  tsta=%f, tsto=%f ??\n",tsta,tsto);
      return -1E10;//non cambiare, serve a FindSigma2!!
    }
  double media=0;
  for(int i=n1;i<=n2;i++)
    media += fAdc.At(i);
  media /= N;
  return media;
}

double KVSignal::FindSigma2(double tsta, double tsto)
{
// Calcolo della varianza nel tratto tra tsta e tsto.
// NOTA: questo ha senso solo se il segnale e' piatto in quella regione!!

//#ifdef DEBUG
//  printf("--- FSignal::FindSigma2(double tsta, double tsto) ---\n");
//#endif
  int n1=(int)(tsta/fChannelWidth);// Non molto preciso, ma tant'e'...
  int n2=(int)(tsto/fChannelWidth);

  int N=n2-n1+1;
  double sigma2=0;
  double media=FindMedia(tsta, tsto);
  if(media==-1E10)
    {
      printf("--- FSignal::FindSigma2(double tsta, double tsto) ---: errore nella media\n");
      return -1;
    }

  for(int i=n1;i<=n2;i++)
    sigma2+=(media-fAdc.At(i))*(media-fAdc.At(i));

  sigma2 /= N-1;

  return sigma2;
}


void KVSignal::FIR_ApplyTrapezoidal(double trise, double tflat) // trise=sqrt(12)*tausha di CR-RC^4 se tflat=trise/2
{
  if(tflat<0) tflat=trise/2.;
  int irise=(int)(1e3*trise/fChannelWidth);
  int iflat=(int)(1e3*tflat/fChannelWidth);

  TArrayF sorig(fAdc);
  float *data  = fAdc.GetArray();
  float *datao = sorig.GetArray();
  int N        = sorig.GetSize();

  // 1
  for(int i=irise;i<N;i++)         data[i]-=datao[i-irise];
  for(int i=irise+iflat;i<N;i++)   data[i]-=datao[i-irise-iflat];
  for(int i=2*irise+iflat;i<N;i++) data[i]+=datao[i-2*irise-iflat];

  // normalizzazione
  double amp=1e3*trise/fChannelWidth;
  data[0] /=  amp;
  for(int i=1;i<N;i++) data[i] = data[i]/amp+data[i-1];
}


Double_t KVSignal::GetAmplitude()
{
    //Compute and return the absolute value of the signal amplitude
  int i_max=0;
  for(int i=0;i<fAdc.GetSize();i++)
    {
      if(fAdc.At(i_max) <  fAdc.At(i) ) i_max = i;
    }
  fIMax = i_max;
  fTMax = fIMax*fChannelWidth;
  fAmplitude = fAdc.At(i_max);
  return fAmplitude;
}

Double_t KVSignal::GetRiseTime()
{
    Multiply(-1);
    fAmplitude*=-1;
    Double_t qt70 = FindTzeroCFDCubic(0.7,3);
    Double_t qt20 = FindTzeroCFDCubic_rev(0.2,qt70,3);
    Double_t qtrise72=qt70-qt20;
    Multiply(-1);
    fAmplitude*=-1;
    return qtrise72;
}

Double_t KVSignal::FindTzeroCFDCubic(double level, int Nrecurr)
{
  // recurr=1 means: linear + 1 approx
  // recurr=0 == FindTzeroCFD
  float *data=fAdc.GetArray();
  int NSamples=fAdc.GetSize();
  double fmax=level*fAmplitude;
  /**** 1) ricerca del punto x2 tale che x2 e' il precedente di tcfd ****/
  int x2=0;
  for(;x2<NSamples;x2++) if(data[x2]<fmax) break;
  x2--;

  return CubicInterpolation(data, x2, fmax, Nrecurr);
}

/***************************************************************************************************/
inline unsigned int ReverseBits(unsigned int p_nIndex, unsigned int p_nBits)
{

    unsigned int i, rev;

    for(i=rev=0; i < p_nBits; i++)
    {
        rev = (rev << 1) | (p_nIndex & 1);
        p_nIndex >>= 1;
    }

    return rev;

}


/***************************************************************************************************/
void KVSignal::ApplyWindowing(int window_type) // 0: barlett, 1:hanning, 2:hamming, 3: blackman
{
  // vedi pag. 468 oppenheim-shafer
  int N=fAdc.GetSize()-1;
  switch(window_type){
  case 0:/*-------------------- BARLETT ----------------------*/
    for(int n=0;n<=N;n++)
      fAdc.GetArray()[n]*= ( n<N/2 ? 2.*n/(double)N : 2.- 2.*n/(double)N );
    break;
  case 1:/*-------------------- HANNING ----------------------*/
    for(int n=0;n<=N;n++)
      fAdc.GetArray()[n]*= 0.5-0.5*cos(2*M_PI*n/(double)N);
    break;
  case 2:/*-------------------- HAmmING ----------------------*/
    for(int n=0;n<=N;n++)
      fAdc.GetArray()[n]*= 0.54-0.46*cos(2*M_PI*n/(double)N);
    break;
  case 3:/*-------------------- BLACKMAN --------------------*/
    for(int n=0;n<=N;n++)
      fAdc.GetArray()[n]*= 0.42-0.5*cos(2*M_PI*n/(double)N)+0.08*cos(4*M_PI*n/(double)N);
    break;
  default:
    printf("ERROR in %s: windowtype %d not valid!\n",__PRETTY_FUNCTION__, window_type);
  };
}




/***************************************************************************************************/
int KVSignal::FFT(unsigned int p_nSamples, bool p_bInverseTransform,
          double *p_lpRealIn, double *p_lpImagIn,
          double *p_lpRealOut, double *p_lpImagOut)
{
  // copiata e adattata da: http://www.codeproject.com/audio/waveInFFT.asp
  // l'unico vettore che puo' essere NULL e' p_lpImagIn

  if(!p_lpRealIn || !p_lpRealOut || !p_lpImagOut){
    printf("ERROR in %s: NULL vectors!\n", __PRETTY_FUNCTION__);
    return -1;
  }


  unsigned int NumBits;
  unsigned int i, j, k, n;
  unsigned int BlockSize, BlockEnd;

  double angle_numerator = 2.0 * M_PI;
  double tr, ti;

  //     if( !IsPowerOfTwo(p_nSamples) )
  //     {
  //         return;
  //     }
  if(p_nSamples <2 || p_nSamples & (p_nSamples-1) )
    {
      printf("ERROR in %s: %d not a power of two!\n", __PRETTY_FUNCTION__,p_nSamples );
      return -1;
    }

  if( p_bInverseTransform ) angle_numerator = -angle_numerator;

  NumBits =0;// NumberOfBitsNeeded ( p_nSamples );
  for(NumBits=0; ; NumBits++ )
    {
      if( p_nSamples & (1 << NumBits) ) break;
    }

  for( i=0; i < p_nSamples; i++ )
    {
      j = ReverseBits ( i, NumBits );
      p_lpRealOut[j] = p_lpRealIn[i];
      p_lpImagOut[j] = (p_lpImagIn == NULL) ? 0.0 : p_lpImagIn[i];
    }


  BlockEnd = 1;
  for( BlockSize = 2; BlockSize <= p_nSamples; BlockSize <<= 1 )
    {
      double delta_angle = angle_numerator / (double)BlockSize;
      double sm2 = sin ( -2 * delta_angle );
      double sm1 = sin ( -delta_angle );
      double cm2 = cos ( -2 * delta_angle );
      double cm1 = cos ( -delta_angle );
      double w = 2 * cm1;
      double ar[3], ai[3];

      for( i=0; i < p_nSamples; i += BlockSize )
        {

      ar[2] = cm2;
      ar[1] = cm1;

      ai[2] = sm2;
      ai[1] = sm1;

      for ( j=i, n=0; n < BlockEnd; j++, n++ )
            {

          ar[0] = w*ar[1] - ar[2];
          ar[2] = ar[1];
          ar[1] = ar[0];

          ai[0] = w*ai[1] - ai[2];
          ai[2] = ai[1];
          ai[1] = ai[0];

          k = j + BlockEnd;
          tr = ar[0]*p_lpRealOut[k] - ai[0]*p_lpImagOut[k];
          ti = ar[0]*p_lpImagOut[k] + ai[0]*p_lpRealOut[k];

          p_lpRealOut[k] = p_lpRealOut[j] - tr;
          p_lpImagOut[k] = p_lpImagOut[j] - ti;

          p_lpRealOut[j] += tr;
          p_lpImagOut[j] += ti;

            }
        }

      BlockEnd = BlockSize;

    }


  if( p_bInverseTransform )
    {
      double denom = (double)p_nSamples;

      for ( i=0; i < p_nSamples; i++ )
        {
      p_lpRealOut[i] /= denom;
      p_lpImagOut[i] /= denom;
        }
    }
  return 0;
}





int KVSignal::FFT(bool p_bInverseTransform, double *p_lpRealOut, double *p_lpImagOut)
{
  // returns the lenght of FFT( power of 2)
  static double *buffer=NULL;
  static int bufflen=0;
  int NSA=fAdc.GetSize();
  int ibits=(int)ceil(log((double)NSA)/LOG2);
  NSA=1<<ibits;

  if(buffer!=NULL && bufflen < NSA )
    {
      delete [] buffer;
      buffer=NULL;
    }


  if(buffer==NULL)
    {
      bufflen=NSA;
      buffer= new double [NSA];
    }
  unsigned int N=fAdc.GetSize();
  float *data=fAdc.GetArray();
  for(unsigned int i=0;i<N;i++)
    buffer[i]=data[i];
  // 0 padding
  memset(buffer+N, 0, (NSA-N)*sizeof(double));
  int r=FFT(NSA, p_bInverseTransform, buffer, NULL, p_lpRealOut, p_lpImagOut);
  if(r<0) return r;
  return NSA;
}
TH1* KVSignal::FFT2Histo(int output, TH1 *hh)  // 0 modulo, 1 modulo db (normalized), 2, re, 3 im
{
  unsigned int N=fAdc.GetSize();
  double *re=new double [2*N];
  double *im=new double [2*N];
  int NFFT=FFT(0, re, im);
  if(NFFT<0)
    {
      printf("ERROR in %s: FFT returned %d!\n", __PRETTY_FUNCTION__, NFFT);
      return NULL;
    }
  int NF=NFFT/2;
  TH1 *h= 0;
  if(!hh) h = new TH1F("hfft","FFT of FSignal",NF,0, 1./fChannelWidth*1000/2);
  else h=hh;

  h->SetStats(kFALSE);
  for(int i=0;i<NF;i++)
    {
      switch(output){
      case 0: // modulo
    h->Fill(h->GetBinCenter(i+1),sqrt(re[i]*re[i]+im[i]*im[i]));
    break;
      case 1: // modulo db
    h->Fill(h->GetBinCenter(i+1),log(sqrt(re[i]*re[i]+im[i]*im[i]))*8.68588963806503500e+00); // numero=20./log(10.)
    break;
      case 2:
    h->Fill(h->GetBinCenter(i+1),re[i]);
    break;
      case 3:
    h->Fill(h->GetBinCenter(i+1),im[i]);
    break;
      default:
    printf("ERROR in %s: output=%d not valid!!\n", __PRETTY_FUNCTION__, output);
    break;
      }
    }
  h->GetXaxis()->SetTitle("Frequency");
  delete [] re;
  delete [] im;

  if(output != 1) return h;
  /*** normalizzazione a 0 db ****/
  int imax=0;
  for(int i=1;i<NF;i++)
    if(h->GetBinContent(i+1)>h->GetBinContent(imax+1))
      imax=i;
  double dbmax=h->GetBinContent(imax+1);
  for(int i=0;i<NF;i++)
    h->SetBinContent(i+1, h->GetBinContent(i+1)-dbmax);
  h->GetYaxis()->SetTitle("Modulo (dB)");
  return h;
}
Double_t KVSignal::CubicInterpolation(float *data, int x2, double fmax, int Nrecurr)
{
 /*
    NOTA: tutti i conti fatti con i tempi in "canali". aggiungero' il *fChannelWidth
    solo nel return.
  */
  /**** 2) normal CFD ***************************************************/
  double xi0=(fmax-data[x2])/(data[x2+1]-data[x2]);
  if(Nrecurr<=0) return fChannelWidth*(x2+xi0);

  /**** 3) approx successive. *******************************************/
  // scrivo il polinomio come a3*xi**3+a2*xi**2+a1*xi+a0
  // dove xi=tcfd-x2 (ovvero 0<xi<1)
  // con maple:
  //                                         3
  //   (1/2 y2 - 1/6 y1 + 1/6 y4 - 1/2 y3) xi
  //
  //                                      2
  //          + (-y2 + 1/2 y3 + 1/2 y1) xi
  //
  //          + (- 1/2 y2 - 1/6 y4 + y3 - 1/3 y1) xi + y2

  double a3=0.5*data[x2]-(1./6.)*data[x2-1]+(1./6.)*data[x2+2]-0.5*data[x2+1];
  double a2=(-data[x2] + 0.5*data[x2+1] + 0.5*data[x2-1]);
  double a1=(- 0.5* data[x2] - 1./6. *data[x2+2]+ data[x2+1] - 1./3.* data[x2-1]);
  double a0=data[x2];
  double xi=xi0;
  for(int rec=0;rec<Nrecurr;rec++)
    {
      xi += (fmax-a0-a1*xi-a2*xi*xi-a3*xi*xi*xi)/(a1+2*a2*xi+3*a3*xi*xi);
    }
  return fChannelWidth*(x2+xi);
}


double KVSignal::FindTzeroCFDCubic_rev(double level, double tend, int Nrecurr)
{
  // recurr=1 means: linear + 1 approx
  // recurr=0 == FindTzeroCFD
  /**************************************/
  float *data=fAdc.GetArray();
  int NSamples=fAdc.GetSize();
  double fmax=level*fAmplitude;
  /**** 1) ricerca del punto x2 tale che x2 e' il precedente di tcfd ****/
  int x2=(int)(tend/fChannelWidth);
  if(x2<=0 || x2>=NSamples) return -1;
  for(; x2>0 ; x2--)
    if(data[x2]>fmax)
      break;
//  x2--;
  return CubicInterpolation(data, x2, fmax, Nrecurr);
}

void KVSignal::FIR_ApplySemigaus(double tau_usec)
{
  FIR_ApplyRCLowPass(tau_usec);
  FIR_ApplyRCLowPass(tau_usec);
  FIR_ApplyRCLowPass(tau_usec);
  FIR_ApplyRCLowPass(tau_usec);

  FIR_ApplyRCHighPass(tau_usec);

  Multiply(1./(32./3.*TMath::Exp(-4.))); // normalizzazione
}


void KVSignal::FIR_ApplyRCLowPass(double time_usec, int reverse)
{
  // f=1/(2*pi*tau) se tau[ns] allora f->[GHz]
  // fsampling=1/fChannelWidth [GHz]
#define DUEPI 6.28318530717958623
  //    printf("fChannelWidth=%f\n",fChannelWidth);
  double f=1./(DUEPI*time_usec*1000.)*fChannelWidth;
  double x=TMath::Exp(-DUEPI*f);
  double a0=1-x;
  double b=x;
  double a=0;
  //    printf("f=%f, x=%f\n",x,f);
  FIR_ApplyRecursiveFilter(a0, 1, &a, &b  , reverse);
}

void KVSignal::FIR_ApplyRCHighPass(double time_usec, int reverse)
{
  // f=1/(2*pi*tau) se tau[ns] allora f->[GHz]
  // fsampling=1/fChannelWidth [GHz]

  //    printf("fChannelWidth=%f\n",fChannelWidth);
  double f=1./(DUEPI*time_usec*1000.)*fChannelWidth;
  double x=TMath::Exp(-DUEPI*f);
  double a0=(1+x)/2.;
  double a1=-(1+x)/2.;
  double b1=x;
  //    printf("f=%f, x=%f\n",x,f);
  FIR_ApplyRecursiveFilter(a0, 1, &a1, &b1 , reverse);
}
#undef DUEPI

void KVSignal::FIR_ApplyRecursiveFilter(double a0, int N, double *a, double *b, int reverse)
{
  // signal will be: y[n]=a0*x[n]+sum a[k] x[k] + sum b[k] y[k]
  int NSamples=fAdc.GetSize();
  double *datay=new double[NSamples];
  float *datax=fAdc.GetArray();
  //    memset(datay, 0, NSamples*sizeof(float)); //azzero l'array.
  /*----------------------------------------------*/
  int i=0, k=0;
  switch(reverse){
  case 0:// direct
    for(i=0;i<N;i++) // primo loop su Npunti
      {
    datay[i]=a0*datax[i];
    for(k=0;k<i;k++)
      datay[i] += a[k]*datax[i-k-1]+b[k]*datay[i-k-1];
      }
    for(i=N;i<NSamples;i++) //secondo loop. cambia l'indice interno.
      {
    datay[i]=a0*datax[i];
    for(k=0;k<N;k++)
      datay[i] += a[k]*datax[i-k-1]+b[k]*datay[i-k-1];
      }
    break; // end of direct
  case 1: //reverse: cut & paste from direct and NSamples-1-
    for(i=0;i<N;i++) // primo loop su Npunti
      {
    datay[NSamples-1-i]=a0*datax[NSamples-1-i];
    for(k=0;k<i;k++)
      datay[NSamples-1-i] += a[k]*datax[NSamples-1-(i-k-1)]
        +b[k]*datay[NSamples-1-(i-k-1)];
      }
    for(i=N;i<NSamples;i++) //secondo loop. cambia l'indice interno.
      {
    datay[NSamples-1-i]=a0*datax[NSamples-1-i];
    for(k=0;k<N;k++)
      datay[NSamples-1-i] += a[k]*datax[NSamples-1-(i-k-1)]
        +b[k]*datay[NSamples-1-(i-k-1)];
      }
    break;
  case -1: // bidirectional
    FIR_ApplyRecursiveFilter(a0,N,a,b,0);
    FIR_ApplyRecursiveFilter(a0,N,a,b,1);
    return;
  default:
    printf("ERROR in %s: reverse=%d not supported\n",__PRETTY_FUNCTION__, reverse);
  }// end of reverse switch.
  /*----------------------------------------------*/
  //   void *memcpy(void *dest, const void *src, size_t n);
  /// non con double! memcpy(datax, datay, NSamples*sizeof(float));
  for(int i=0;i<NSamples;i++)
    datax[i]=(float)datay[i];
  delete [] datay;
}

void KVSignal::ApplyModifications(TGraph *newSignal)
{
    if(!newSignal) newSignal = this;
    for(int ii=0; ii<GetN(); ii++)
    {
        newSignal->SetPoint(ii,fX[ii],fAdc.At(ii));
    }
}


void KVSignal::Multiply(Double_t fact)
{
    for(int i=0;i<fAdc.GetSize();i++) fAdc.AddAt(fAdc.At(i)*fact,i);
//    if(value>0)
//      {
//        BaseLine=0;
//        Max *= value;
//        Integral *=value;
//      }
//    else
//      Update();
//    return *this;
}
