// -*- mode: c++; -*- 
/* genbb_help_config.h header file 
 */

#ifndef __genbb_genbb_help_config_h
#define __genbb_genbb_help_config_h 1

/*** brio versioning information ***/

//  GENBB_HELP_VERSION % 100        is the patch level
//  GENBB_HELP_VERSION / 100 % 1000 is the minor version
//  GENBB_HELP_VERSION / 100000     is the major version

#define GENBB_HELP_VERSION_MAJOR @genbb_help_VERSION_MAJOR@
#define GENBB_HELP_VERSION_MINOR @genbb_help_VERSION_MINOR@
#define GENBB_HELP_PATCH_VERSION @genbb_help_PATCH_VERSION@

#define GENBB_HELP_VERSION @genbb_help_VERSION_NUMERIC@

//  GENBB_HELP_LIB_VERSION must be defined to be the same as GENBB_HELP_VERSION
//  but as a *string* in the form "x.y[.z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define GENBB_HELP_LIB_VERSION "@genbb_help_VERSION_STR@"

/***  genbb_help embedded Boost/Serialization based I/O ***/
#if x1 == x1
#define GENBB_HELP_WITH_BIO @GENBB_HELP_WITH_BIO@
#endif

#endif // __genbb_genbb_help_config_h

// end of genbb_help_config.h
