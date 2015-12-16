/***************************************************************************
$Id: VAMOSLinkDef.h,v 1.4 2009/01/14 15:59:49 franklan Exp $
 ***************************************************************************/

#ifdef __CINT__

#include "RVersion.h"
#include "Defines.h" // __ENABLE_DEPRECATED_VAMOS__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class LogFile+;
#pragma link C++ class Random+;

#ifdef __ENABLE_DEPRECATED_VAMOS__
#pragma link C++ class GridLoader+;
#endif

#endif
