/***************************************************************************
$Id: VAMOSLinkDef.h,v 1.4 2009/01/14 15:59:49 franklan Exp $
 ***************************************************************************/

#ifdef __CINT__

#include "RVersion.h"
#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#ifdef __ENABLE_DEPRECATED_VAMOS__
#pragma link C++ class Identificationv+;
#pragma link C++ class CsICalib+;
#endif

#pragma link C++ class KVIDHarpeeICSi+;
#pragma link C++ class KVIDHarpeeICSi_e503+;
#pragma link C++ class KVIDHarpeeSiCsI+;
#pragma link C++ class KVIDQA+;
#pragma link C++ class KVIDQAGrid+;
#pragma link C++ class KVIDQALine-;
#pragma link C++ class KVIDQAMarker+;
#pragma link C++ class KVIDSiCsIVamos+;
#pragma link C++ class KVVAMOSIDTelescope+;

#endif
