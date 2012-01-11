// -*- mode: c++; -*- 
/* genvtx_config.h header file 
 */

#ifndef __genvtx_genvtx_config_h
#define __genvtx_genvtx_config_h 1

/*** brio versioning information ***/

//  GENVTX_VERSION % 100        is the patch level
//  GENVTX_VERSION / 100 % 1000 is the minor version
//  GENVTX_VERSION / 100000     is the major version

#define GENVTX_VERSION_MAJOR @genvtx_VERSION_MAJOR@
#define GENVTX_VERSION_MINOR @genvtx_VERSION_MINOR@
#define GENVTX_PATCH_VERSION @genvtx_PATCH_VERSION@

#define GENVTX_VERSION @genvtx_VERSION_NUMERIC@

//  GENVTX_LIB_VERSION must be defined to be the same as GENVTX_VERSION
//  but as a *string* in the form "x.y[.z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define GENVTX_LIB_VERSION "@genvtx_VERSION_STR@"

/***  genvtx embedded Boost/Serialization based I/O ***/
#if x0 == x1
#define GENVTX_WITH_BIO @_PACKNAME___WITH_BIO@
#endif

#endif // __genvtx_genvtx_config_h

// end of genvtx_config.h
