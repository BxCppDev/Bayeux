// -*- mode: c++; -*- 
/* brio_config.h header file 
 */

#ifndef __brio__brio_config_h
#define __brio__brio_config_h 1

/*** brio versioning information ***/

//  BRIO_VERSION % 100        is the patch level
//  BRIO_VERSION / 100 % 1000 is the minor version
//  BRIO_VERSION / 100000     is the major version

#define BRIO_VERSION_MAJOR @brio_VERSION_MAJOR@
#define BRIO_VERSION_MINOR @brio_VERSION_MINOR@
#define BRIO_PATCH_VERSION @brio_PATCH_VERSION@

#define BRIO_VERSION @brio_VERSION_NUMERIC@

//  BRIO_LIB_VERSION must be defined to be the same as BRIO_VERSION
//  but as a *string* in the form "x.y[.z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define BRIO_LIB_VERSION "@brio_VERSION_STR@"

#endif // __brio__brio_config_h

// end of brio_config.h
