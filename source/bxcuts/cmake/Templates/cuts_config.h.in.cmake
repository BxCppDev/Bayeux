// -*- mode: c++; -*- 
/* cuts_config.h header file 
 */

#ifndef __cuts_cuts_config_h
#define __cuts_cuts_config_h 1

/*** brio versioning information ***/

//  CUTS_VERSION % 100        is the patch level
//  CUTS_VERSION / 100 % 1000 is the minor version
//  CUTS_VERSION / 100000     is the major version

#define CUTS_VERSION_MAJOR @cuts_VERSION_MAJOR@
#define CUTS_VERSION_MINOR @cuts_VERSION_MINOR@
#define CUTS_PATCH_VERSION @cuts_PATCH_VERSION@

#define CUTS_VERSION @cuts_VERSION_NUMERIC@

//  CUTS_LIB_VERSION must be defined to be the same as CUTS_VERSION
//  but as a *string* in the form "x.y[.z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define CUTS_LIB_VERSION "@cuts_VERSION_STR@"

/***  cuts embedded Boost/Serialization based I/O ***/
#if x0 == x1
#define CUTS_WITH_BIO @_PACKNAME___WITH_BIO@
#endif

#endif // __cuts_cuts_config_h

// end of cuts_config.h
