#ifndef __VAMOS_DEFINES_H__
#define __VAMOS_DEFINES_H__

// Enable the deprecated VAMOS code (lots of warnings will appear if enabled)
//#define __ENABLE_DEPRECATED_VAMOS__

//----------- Only deprecated definitions occur below this line ----------------

#ifdef __ENABLE_DEPRECATED_VAMOS__

// Multiplepeak rejection mode
//#define SECHIP
//#define MULTIPLEPEAK
#define WEIGHTEDAVERAGE
#define GOCCE
#define ESS
//#define FOLLOWPEAKS
//#define ACTIVEBRANCHES
//#define EGCORR

//Defines DEBUG mode
//#define DEBUG

#endif // __ENABLE_DEPRECATED_VAMOS__ defined

#endif // __VAMOS_DEFINES_H__ not defined
