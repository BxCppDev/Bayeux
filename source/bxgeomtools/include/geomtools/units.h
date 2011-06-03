// -*- mode: c++; -*- 
/* units.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2009-03-31
 * 
 * License: 
 * 
 * Description: 
 *  CLHEP units utilities from 'datatools::utils::units' class.
 *
 * History: 
 * 
 */

#ifndef __geomtools__units_h
#define __geomtools__units_h 1

#include <datatools/utils/units.h>

// maybe this is a bad solution:
// here we include the CLHEP headers in such a way the 
// full system of units is available everywhere
//#include <CLHEP/Units/SystemOfUnits.h>
//#include <CLHEP/Units/PhysicalConstants.h>

namespace geomtools {

  /* 2010-03-18 FM:
   * class 'units' is now part of the 'datatools' package:
   * Here I just define an alias for convenience and backward compatibilty.
   */
  // 2010-05-04 FM: removed
  //typedef datatools::utils::units units;

}

#endif // __geomtools__units_h

// end of units.h
