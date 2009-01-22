/***************************************************************************************
$Id: KVTGIDFunctions.h,v 1.5 2008/02/06 11:21:30 franklan Exp $
								KVTGIDFunctions.h
								
			Contains Tassan-Got functionals for mass and charge
			identification used for Si-CsI, Si150-CsI and ChIo-CsI, and ChIo-Si matrices.
***************************************************************************************/

#ifndef __KVTGIDFUNCTIONS_H
#define __KVTGIDFUNCTIONS_H

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

namespace KVTGIDFunctions {
   Double_t chiosi_Z(Double_t * x, Double_t * par);
   Double_t tassangot_Z(Double_t * x, Double_t * par);
   Double_t pichon_Z(Double_t * x, Double_t * par);
   Double_t tassangot_A(Double_t * x, Double_t * par);
   Double_t pawlowski_Z(Double_t * x, Double_t * par);
   Double_t pawlowski_A(Double_t * x, Double_t * par);
   Double_t starting_points_Z(Double_t * x, Double_t * par);
   Double_t starting_points_A(Double_t * x, Double_t * par);
}
#endif
