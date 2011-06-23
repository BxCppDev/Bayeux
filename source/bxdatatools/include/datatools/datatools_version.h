// -*- mode: c++; -*- 
/* datatools_version.h version header file 
 * 
 *
 */

#ifndef __datatools__datatools_version_h
#define __datatools__datatools_version_h 1

//  DATATOOLS_VERSION % 100        is the patch level
//  DATATOOLS_VERSION / 100 % 1000 is the minor version
//  DATATOOLS_VERSION / 100000     is the major version

#define DATATOOLS_VERSION 300101

//  DATATOOLS_LIB_VERSION must be defined to be the same as DATATOOLS_VERSION
//  but as a *string* in the form "x.y[.z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define DATATOOLS_LIB_VERSION "3.1.1"

#endif // __datatools__datatools_version_h

// end of datatools_version.h
