// -*- mode: c++; -*- 
/* materials_version.h configuration header file 
 * 
 *
 */

#ifndef __materials__version_h
#define __materials__version_h 1

//  MATERIALS_VERSION % 100        is the patch level
//  MATERIALS_VERSION / 100 % 1000 is the minor version
//  MATERIALS_VERSION / 100000     is the major version

#define MATERIALS_VERSION 200102

//  MATERIALS_LIB_VERSION must be defined to be the same as MATERIALS_VERSION
//  but as a *string* in the form "x.y[.z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define MATERIALS_LIB_VERSION "2.1.2"

#endif // __materials__version_h

// end of materials_version.h
