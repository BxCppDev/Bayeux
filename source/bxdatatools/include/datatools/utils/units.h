// -*- mode: c++ ; -*- 
/* units.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-18
 * Last modified: 2010-03-18
 * 
 * License: 
 * 
 * Description: 
 *   Utilities for units (CLHEP)
 * 
 * History: 
 * 
 */

#ifndef __datatools__utils__units_h
#define __datatools__utils__units_h 1

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <list>
#include <limits>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>

namespace datatools {

  namespace utils {

    using namespace std;

    class units
    {
    
    protected:

      static void _throw_bad_unit (const string & type_, 
				   const string & unit_str_);

    public:

      static double get_length_unit_from (const string &);
      static double get_surface_unit_from (const string &);
      static double get_volume_unit_from (const string &);
      static double get_time_unit_from (const string &);
      static double get_angle_unit_from (const string &);
      static double get_solid_angle_unit_from (const string &);
      static double get_energy_unit_from (const string &);
      static double get_mass_unit_from (const string &);
      static double get_pressure_unit_from (const string &);
      static double get_magnetic_field_unit_from (const string &);
      static double get_temperature_unit_from (const string &);
      static double get_density_unit_from (const string &);
      static double get_unit_from (const string & unit_type_, 
				   const string & unit_str_);
      static double get_unit (const string & unit_str_);

      static double get_value_with_unit (const string & word_);
  
    };

  } // end of namespace utils

} // end of namespace datatools

#endif // __datatools__utils__units_h

// end of units.h
