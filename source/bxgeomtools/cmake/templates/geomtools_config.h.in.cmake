// -*- mode: c++; -*- 
/* geomtools_config.h header file 
 */

#ifndef __geomtools__geomtools_config_h
#define __geomtools__geomtools_config_h 1

/*** geomtools versioning information ***/

//  GEOMTOOLS_VERSION % 100        is the patch level
//  GEOMTOOLS_VERSION / 100 % 1000 is the minor version
//  GEOMTOOLS_VERSION / 100000     is the major version

#define GEOMTOOLS_VERSION_MAJOR @geomtools_VERSION_MAJOR@
#define GEOMTOOLS_VERSION_MINOR @geomtools_VERSION_MINOR@
#define GEOMTOOLS_PATCH_VERSION @geomtools_PATCH_VERSION@

#define GEOMTOOLS_VERSION @geomtools_VERSION_NUMERIC@

//  GEOMTOOLS_LIB_VERSION must be defined to be the same as GEOMTOOLS_VERSION
//  but as a *string* in the form "x.y[.z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define GEOMTOOLS_LIB_VERSION "@geomtools_VERSION_STR@"

/***  geomtools embedded Boost/Serialization based I/O ***/
#define GEOMTOOLS_WITH_BIO @GEOMTOOLS_WITH_BIO@

#endif // __geomtools__geomtools_config_h

// end of geomtools_config.h
