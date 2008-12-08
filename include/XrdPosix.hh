#ifndef __XRDPOSIX_H__
#define __XRDPOSIX_H__
/******************************************************************************/
/*                                                                            */
/*                           X r d P o s i x . h h                            */
/*                                                                            */
/* (c) 2005 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/* Modified by Frank Winklmeier to add the full Posix file system definition. */
/******************************************************************************/
  
//           $Id: XrdPosix.hh,v 1.3 2008/10/09 14:20:56 franklan Exp $

// The following defines substitute our names for the common system names. We
// would have liked to use wrappers but each platform uses a different mechanism
// to accomplish this. So, redefinition is the most portable way of doing this.
//

#define close(a)         XrdPosix_Close(a)

#define lseek(a,b,c)     XrdPosix_Lseek(a,b,c)

#define open             XrdPosix_Open

#define read(a,b,c)      XrdPosix_Read(a,b,c)

#define write(a,b,c)     XrdPosix_Write(a,b,c)

// Now define the external interfaces (not C++ but OS compatabile)
//
//#include "XrdPosix/XrdPosixExtern.hh"
#ifndef __XRDPOSIXEXTERN_H__
#define __XRDPOSIXEXTERN_H__
/******************************************************************************/
/*                                                                            */
/*                     X r d P o s i x E x t e r n . h h                      */
/*                                                                            */
/* (c) 2005 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/* Modified by Frank Winklmeier to add the full Posix file system definition. */
/******************************************************************************/
  
//           $Id: XrdPosix.hh,v 1.3 2008/10/09 14:20:56 franklan Exp $

// These OS-Compatible (not C++) externs are included by XrdPosix.hh to
// complete the macro definitions contained therein.

// Use this file directly to define your own macros or interfaces. Note that
// native types are used to avoid 32/64 bit parameter/return value ambiguities
// and to enforce shared library compatability (needed by the preload32 code).

// Only 64-bit interfaces are directly supported. However, the preload library
// supports the old 32-bit interfaces. To use this include you must specify

// -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

// compilation options. This ensures LP64 compatability which defines:
//
// ssize_t ->          long long
//  size_t -> unsigned long long
//   off_t ->          long long

#if (!defined(_LARGEFILE_SOURCE) || !defined(_LARGEFILE64_SOURCE) || \
    _FILE_OFFSET_BITS!=64) && !defined(XRDPOSIXPRELOAD32)
#error Compilation options are incompatible with XrdPosixExtern.hh; \
       Specify -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
#endif

#ifndef OLD_XRDPOSIXEXTERN
// We pre-declare various structure t avoid compilation complaints. We cannot
// include the necessary ".h" files as these would also try to define entry
// points which may conflict with our definitions due to renaming pragmas and
// simple defines. All we want is to make sure we have the right name in the
// loader's symbol table so that the preload library can intercept the call.
// We need these definitions here because the includer may not have included
// all of the includes necessary to support all of the API's.
//
struct iovec;
struct stat;
struct statfs;
struct statvfs;
#endif

#include <dirent.h>
#ifdef OLD_XRDPOSIXEXTERN
#include <unistd.h>
#include <sys/stat.h>
#else
#include <stdio.h>
#include <unistd.h>
#endif
#include <sys/types.h>

//#include "XrdPosix/XrdPosixOsDep.hh"
#ifndef __XRDPOSIXOSDEP_H__
#define __XRDPOSIXOSDEP_H__
/******************************************************************************/
/*                                                                            */
/*                      X r d P o s i x O s D e p . h h                       */
/*                                                                            */
/* (c) 2005 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/* Modified by Frank Winklmeier to add the full Posix file system definition. */
/******************************************************************************/
  
//           $Id: XrdPosix.hh,v 1.3 2008/10/09 14:20:56 franklan Exp $

// Solaris does not have a statfs64 structure. So all interfaces use statfs.
//
#ifdef __solaris__
#define statfs64 statfs
#endif

// We need to avoid using dirent64 for MacOS platforms. We would normally
// include XrdSysPlatform.hh for this but this include file needs to be
// standalone. So, we replicate the dirent64 redefinition here, Additionally,
// off64_t, normally defined in Solaris and Linux, is cast as long long (the
// appropriate type for the next 25 years). The Posix interface only supports
// 64-bit offsets.
//
#if  defined(__macos__)
#if !defined(dirent64)
#define dirent64 dirent
#endif
#if !defined(off64_t)
#define off64_t long long
#endif

#if defined(__DARWIN_VERS_1050) && !__DARWIN_VERS_1050
#if !defined(stat64)
#define stat64 stat
#endif
#if !defined(statfs64)
#define statfs64 statfs
#endif
#endif

#if !defined(statvfs64)
#define statvfs64 statvfs
#endif
#define ELIBACC ESHLIBVERS
#endif

#endif

//handle change in declarations of functions from ROOT v5.20/00 onwards
#ifdef OLD_XRDPOSIXEXTERN
#define OFFT    off_t
#define SSIZET  ssize_t
#define SIZET   size_t
#else
#define OFFT    long long
#define SSIZET  long long
#define SIZET   unsigned long long
#endif

#ifndef OLD_XRDPOSIXEXTERN
#ifdef __cplusplus
extern "C"
{
#endif
#endif
extern int        XrdPosix_Close(int fildes);

extern SSIZET  XrdPosix_Lseek(int fildes, OFFT offset, int whence);

extern int        XrdPosix_Open(const char *path, int oflag, ...);

extern SSIZET  XrdPosix_Read(int fildes, void *buf, SIZET nbyte);
  
extern SSIZET  XrdPosix_Write(int fildes, const void *buf,
                                 SIZET nbyte);
#ifndef OLD_XRDPOSIXEXTERN
#ifdef __cplusplus
};
#endif
#endif

#endif

#endif
