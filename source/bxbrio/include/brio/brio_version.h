// -*- mode: c++; -*- 
/* brio_version.h version header file 
 * 
 *
 */

#ifndef __brio__brio_version_h
#define __brio__brio_version_h 1

//  BRIO_VERSION % 100        is the patch level
//  BRIO_VERSION / 100 % 1000 is the minor version
//  BRIO_VERSION / 100000     is the major version

#define BRIO_VERSION 200001

//  BRIO_LIB_VERSION must be defined to be the same as BRIO_VERSION
//  but as a *string* in the form "x_y[_z]" where x is the major version
//  number, y is the minor version number, and z is the patch level if not 0.

#define BRIO_LIB_VERSION "2.1"

#endif // __brio__brio_version_h

// end of brio_version.h
