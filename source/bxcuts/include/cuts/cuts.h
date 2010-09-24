// -*- mode: c++; -*- 
/* cuts.h global header file 
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 
 *
 *
 */

/*! \mainpage The cuts package documentation
 *
 * \section intro_section Introduction
 *
 * The 'cuts' library provides some classes to create, 
 * compose and apply cuts on arbitrary data.
 *
 * Given an arbitrary data model, the user can derived his/her 
 * own cut algorithms using the \c i_cut class interface 
 * (include/cuts/i_cut.h). Such 
 * cuts may be combined using boolean logic in order to apply 
 * more complex criteria:
 *
 * Example:
 *    (CUT1 or CUT2) and (not CUT3) and (CUT6 nor CUT7)
 * where each CUTX instances implement the virtual \c _accept() method
 * that applies a specific selection criterion on the data model.
 *
 * Basic boolean cuts are provided:
 *  \li unary cut:   \c not_cut, 
 *  \li binary cuts: \c and_cut, \c or_cut, \c not_cut, \c xor_cut... 
 *  \li more:        \c multi_cut (general and for many cuts)
 *  
 * A mechanism is implemented within a \e factory class in order
 * to auto-register new cut classes in the library and make
 * possible to allocate and setup cut instances through the factory.
 * 
 * A \e cut \e manager is provided. It enables:
 *  \li the description of collection of cuts (and combinaison of them) 
 *    using ASCII files (through the 'datatools::utils::properties' class), 
 *  \li the use of any of such cut instances to be applied to user 
 *    data (selection).
 *
 * \section contents_section Contents 
 *
 * Practically, the user should implement his/her own cuts and register
 * them in the factory. Each cut should at least implement the
 * protected \c _accept method and some registered \e creator method (see 
 * example in \c include/cuts/test/test_cuts.h sample file).
 * 
 * Then the user may provide some ASCII files that describe the
 * properties of the cuts to be used (see \c ${CUTS_ROOT}/resources/test_cuts_def.conf as an example). Such files are loaded within a cut manager 
 * (see \c programs/test_manager.cxx) and the user can extract the 
 * \e selector of his/her choice and apply it to some data sample.
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
bash> source cuts.sh
bash> cuts-config --help
\endverbatim
 * 
 * Setup from your \c '~/.bashrc': 
 *  \verbatim
export CUTS_ROOT=<installation base directory>
if [ -f ${CUTS_ROOT}/cuts.sh ]; then
   source ${CUTS_ROOT}/cuts.sh
else
   echo "ERROR: No setup for the cuts package !" >&2
fi
\endverbatim
 *
 * \section misc_section Miscellaneous informations
 *
 * This section is empty.
 *
 */

#ifndef __cuts__cuts_h
#define __cuts__cuts_h 1

#include <cuts/cuts_version.h>
#include <cuts/i_cut.h>
#include <cuts/accept_cut.h>
#include <cuts/reject_cut.h>
#include <cuts/not_cut.h>
#include <cuts/i_binary_cut.h>
#include <cuts/nand_cut.h>
#include <cuts/nor_cut.h>
#include <cuts/or_cut.h>
#include <cuts/xnor_cut.h>
#include <cuts/xor_cut.h>
#include <cuts/and_cut.h>
#include <cuts/multi_cut.h>
#include <cuts/factory.h>
#include <cuts/manager.h>

#endif // __cuts__cuts_h

// end of cuts.h
