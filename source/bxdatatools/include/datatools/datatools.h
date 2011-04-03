// -*- mode: c++; -*- 
//! \file datatools/datatools.h
/* datatools.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-25
 * Last modified: 2010-11-22
 * 
 * License: 
 * 
 * 
 * Description: 
 *   datatools package user interface
 *  
 * History: 
 * 
 */

/*! \mainpage The datatools package documentation
 *
 * \section intro_section Introduction
 *
 * \subsection pres_subsection Presentation
 *  The datatools package provides some general purpose utility classes
 *  as well as an I/O system based on the Boost serialization library.
 *
 *  This package depends on:
 *  - the Boost C++ library through the boost_help package available at:
 *    https://nemo.lpc-caen.in2p3.fr/svn/boost_help/ (version 1.44 and above)
 *  - the CLHEP C++ library (version 2.0 and above for some mathematics 
 *    and its systemof units)
 *
 * \section contents_section Contents
 *
 * \subsection utils_subsection The utils module
 *  The utils module contains some useful classes and interfaces  
 *  for many possible applications. 
 *
 * \subsection serialization_subsection The serialization module
 *  The serialization module contains some classes and macros 
 *  dedicated to data serializarion
 *  through the Boost.Serialization library. 
 *  
 * \subsection event_subsection The event module
 *  The event module contains some classes dedicated to the representation of
 *  some event model for physics.
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
 *  3- Build test binaries:
 *
 *     shell> ./pkgtools.d/pkgtool test
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
 *   - https://nemo.lpc-caen.in2p3.fr/wiki/datatools/
 *
 *  SVN repository:
 *   - Base repository: https://nemo.lpc-caen.in2p3.fr/svn/datatools/
 *   - Development version: https://nemo.lpc-caen.in2p3.fr/svn/datatools/trunk
 *   - Releases: https://nemo.lpc-caen.in2p3.fr/svn/datatools/tags/datatools-X.Y
 *
 *  Author:
 *   - Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 *  Mainteners:
 *   - Francois Mauger <mauger@lpccaen.in2p3.fr>
 *   - Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 *
 */

#ifndef __datatools__datatools_h
#define __datatools__datatools_h 1

#include <datatools/datatools_version.h>

#include <datatools/serialization/io_factory.h>
#include <datatools/serialization/serialization.h>

#include <datatools/utils/i_clear.h>
#include <datatools/utils/i_named.h>
#include <datatools/utils/i_propinit.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/ioutils.h>
#include <datatools/utils/multi_properties.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/simple_lockfile.h>
#include <datatools/utils/units.h>
#include <datatools/utils/utils.h>

#include <datatools/event/event_id.h>
#include <datatools/event/basic_event.h>

//! \namespace datatools
/* The basic datatools namespace.
 */
namespace datatools {

  //static bool g_debug = false;

} // end of namespace datatools

#endif // __datatools__datatools_h

// end of datatools.h
