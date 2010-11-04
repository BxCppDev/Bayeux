// -*- mode: c++; -*- 
/* brio.h global header file 
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-28
 * Last modified: 
 *
 *
 */

/*! \mainpage The brio package documentation
 *
 * \section intro_section Introduction
 *
 * This section is empty.
 *
 * \section contents_section Contents 
 *
 * This section is empty.
 *
 * \section build_section Installation instructions 
 *
 * Configuration, build, installation and setup are done thanks 
 * to the \c './pkgtools.d/pkgtool' script.
 *
 * Usage: 
 *  \verbatim
bash> ./pkgtools.d/pkgtool --help   
\endverbatim
 *
 * Using bash, a typical install sequence is:
 * 
 *  \verbatim
bash> ./pkgtools.d/pkgtool [options...] check [special options...]
bash> ./pkgtools.d/pkgtool [options...] configure  [special options...]
bash> ./pkgtools.d/pkgtool [options...] build 
bash> ./pkgtools.d/pkgtool [options...] build bin 
bash> ./pkgtools.d/pkgtool [options...] build bin_test 
bash> ./pkgtools.d/pkgtool [options...] doc 
bash> ./pkgtools.d/pkgtool [options...] install [special options...]
\endverbatim
 * 
 * Test: 
 *  \verbatim
bash> source brio.sh
bash> brio-config --help
\endverbatim
 * 
 * Setup from your \c '~/.bashrc': 
 *  \verbatim
export BRIO_ROOT=<installation base directory>
if [ -f ${BRIO_ROOT}/brio.sh ]; then
   source ${BRIO_ROOT}/brio.sh
else
   echo "ERROR: No setup for the brio package !" >&2
fi
\endverbatim
 *
 * \section misc_section Miscellaneous informations
 *
 * This section is empty.
 *
 */

#ifndef __brio__brio_h
#define __brio__brio_h 1

#include <brio/brio_version.h>
#include <brio/writer.h>
#include <brio/reader.h>


#endif // __brio__brio_h

// end of brio.h
