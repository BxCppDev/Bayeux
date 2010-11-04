// -*- mode: c++; -*- 
/* brio.h global header file 
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-28
 * Last modified: 2010-11-04
 *
 *
 */

/*! \mainpage The brio package documentation
 *
 * \section intro_section Introduction
 *
 *  The \t brio package implements some \e writer and \e reader classes that enable
 *   to store/load arbitrary \e serializable objects using (\e  quasi-)\e portable 
 *   \e binary \e archives from the Boost/Serialization system and ROOT files and trees 
 *   as storage media. 
 *   
 *   While loading back the data, serialized objects can then be accessed sequentially or 
 *   non-sequentially while serialized archives (ASCII, XML or binary) using standard
 *   I/O filestreams can only be accessed sequentially (see the \t datatools::serialization::data_reader
 *   and \  datatools::serialization::data_writer classes).
 *
 *   More, files may contains several \e stores of archived data that can be accessed independantly.
 *
 * \section contents_section Contents 
 *
 * This version contains the \t brio::writer and the \t brio::reader classes.
 *
 * Sample programs can be found in the \t programs directory:
 * - \t programs/test_data_io.cxx: show how to use a sample serializable \t data class 
 *   (\t include/brio/test/data.h and \t src/data.cc) and a \e brio \e writer to 
 * store a list of \t data instances in a ROOT files then reload these instances 
 * from the file using a \e brio \e reader.
 * This example makes use of the so-called \e automatic \e store dynamically created 
 * within the ROOT file.
 * 
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
