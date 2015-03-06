//Created by KVClassFactory on Wed Mar  4 16:50:19 2015
//Author: John Frankland,,,

#include "Simple2DModel.h"

ClassImp(BackTrack::Simple2DModel)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>Simple2DModel</h2>
<h4>Simple model to test backtrack procedures</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

namespace BackTrack {

   Simple2DModel::Simple2DModel()
      : fPar1("PAR1","Model parameter",0.,50.),
        fObs1("OBS1","Model observable",-20,70.),
        fPar2("PAR2","Model parameter",0.,50.),
        fObs2("OBS2","Model observable",-20,70.),
        a10("a10","a10",0,0,50),a11("a11","a11",1,-1,1),
        a00("a00","a00",50,0,50),a01("a01","a01",-1,-1,1),
        MEAN1("MEAN1","MEAN1",fPar1,RooArgSet(a00,a01)),
      MEAN2("MEAN2","MEAN2",fPar2,RooArgSet(a10,a11))
   {
      // Default constructor
      AddParameter(fPar1,5);
      AddParameter(fPar2,5);
      AddObservable(fObs1);
      AddObservable(fObs2);

      // conditional distribution of observables
      gauss1 = new RooGaussian("g1","observable distribution for fixed parameter",fObs1,MEAN1,RooConst(5)) ;
      gauss2 = new RooGaussian("g2","observable distribution for fixed parameter",fObs2,MEAN2,RooConst(5)) ;

      theModel = new RooProdPdf("Simple2DModel", "gaussian product", RooArgSet(*gauss1,*gauss2));
   }

   Simple2DModel::~Simple2DModel()
   {
      // Destructor
   }

}
