// -*- mode: c++; -*- 
//! \file materials/materials.h
/* materials.h
 * Author(s):  Benoit Guillon <guillon@lpccaen.in2p3.fr>
 * Creation date: 2008-02-25
 * Last modified: 2008-02-25
 * 
 * License: 
 * 
 * 
 * Description: 
 *   materials package user interface
 *  
 * History: 
 * 
 */

/*! \mainpage The materials package documentation
 *
 * \section intro_section Introduction
 *
 * \subsection pres_subsection Presentation
 *  The materials package provides some general utility to handle materials.
 *
 *  This package depends on:
 *  - datatools
 *
 * \section contents_section Contents
 *
 * \subsection isotope_subsection isotope
 *  The isotope is.... . 
 *
 * \subsection element_subsection element
 *  The element is.... . . 
 *  
 * \subsection material_subsection material
*  The material is.... . . 
 *
 * \section build_section Installation instructions
 *
 *  1- Configure:
 *
 *     shell> ./pkgtools.d/pkgtool configure
 *
 *  2- Build:
 * 
 *     shell> ./pkgtools.d/pkgtool build
 *
 *  3- Build binaries:
 *
 *     shell> ./pkgtools.d/pkgtool build bin
 *
 *  4- Build test binaries:
 *
 *     shell> ./pkgtools.d/pkgtool build bin_test
 *
 *  5- Build documentation:
 *
 *     shell> ./pkgtools.d/pkgtool doc
 *
 *  6- Installation:
 *
 *     shell> ./pkgtools.d/pkgtool install
 *
 * \section geninfo_section General informations
 *
 *  Wiki repository:
 *   - https://nemo.lpc-caen.in2p3.fr/wiki/materials/
 *
 *  SVN repository:
 *   - Base repository: https://nemo.lpc-caen.in2p3.fr/svn/materials/
 *   - Development version: https://nemo.lpc-caen.in2p3.fr/svn/materials/trunk
 *   - Releases: https://nemo.lpc-caen.in2p3.fr/svn/materials/tags/materials-1.0
 *
 *  Author:
 *   - Benoit Guillon <guillon@lpccaen.in2p3.fr>
 *
 *  Mainteners:
 *   - Benoit Guillon <guillon@lpccaen.in2p3.fr>
 *   - François Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef MATERIALS_MATERIALS_H_
#define MATERIALS_MATERIALS_H_ 1

#include <materials/ch_symbol.h>
#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>
#include <materials/factory.h>
#include <materials/manager.h>

#endif // MATERIALS_MATERIALS_H_

// end of materials.h
