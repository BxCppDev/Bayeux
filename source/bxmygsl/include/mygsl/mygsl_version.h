// -*- mode: c++; -*- 
/* mygsl_version.h version header file 
 * 
 *
 */

#ifndef __mygsl__mygsl_version_h
#define __mygsl__mygsl_version_h 1

//  MYGSL_VERSION % 100        is the patch level
//  MYGSL_VERSION / 100 % 1000 is the minor version
//  MYGSL_VERSION / 100000     is the major version

#define MYGSL_VERSION 200102

//  MYGSL_LIB_VERSION must be defined to be the same as MYGSL_VERSION
//  but as a *string* in the form "x.y[.z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define MYGSL_LIB_VERSION "2.1.2"

#endif // __mygsl__mygsl_version_h

// end of mygsl_version.h
