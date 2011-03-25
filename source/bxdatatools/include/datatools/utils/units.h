// -*- mode: c++ ; -*- 
//! \file datatools/utils/units.h
/* units.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-18
 * Last modified: 2011-03-25
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

namespace datatools {

  namespace utils {

    using namespace std;
   
    /*! \brief Utilities for units (CLHEP).
     *
     *  The units class provides some static methods to manipulate 
     *  units values through strings.
     *  It is based on the system of units in the CLHEP package.
     *
     */
    class units
    {
    
    protected:
      //! Exception thrown in case of unrecognized unit symbol.
      static void _throw_bad_unit (const string & type_, 
				   const string & unit_str_);

    public:

      //! Get the CLHEP length unit from a string.
      static double get_length_unit_from (const string &);

      //! Get the CLHEP surface unit from a string.
      static double get_surface_unit_from (const string &);

      //! Get the CLHEP volume unit from a string.
      static double get_volume_unit_from (const string &);

      //! Get the CLHEP time unit from a string.
      static double get_time_unit_from (const string &);

      //! Get the CLHEP angle unit from a string.
      static double get_angle_unit_from (const string &);

      //! Get the CLHEP solid angle unit from a string.
      static double get_solid_angle_unit_from (const string &);

      //! Get the CLHEP energy unit from a string.
      static double get_energy_unit_from (const string &);

      //! Get the CLHEP mass unit from a string.
      static double get_mass_unit_from (const string &);

      //! Get the CLHEP pressure unit from a string.
      static double get_pressure_unit_from (const string &);

      //! Get the CLHEP magnetic field unit from a string.
      static double get_magnetic_field_unit_from (const string &);

      //! Get the CLHEP temperature unit from a string.
      static double get_temperature_unit_from (const string &);

      //! Get the CLHEP density unit from a string.
      static double get_density_unit_from (const string &);

      //! Get the CLHEP unit from a string specifiying the unit type.
     /**
       * Example:
       *
       *   double lu = datatools::utils::units::get_unit_from  ("length", "cm");
       */
      static double get_unit_from (const string & unit_type_, 
				   const string & unit_str_);
 
      //! Get the CLHEP unspecified from a string.
      /**
       * Example:
       *
       *   double u = datatools::utils::units::get_unit ("cm");
       */
      static double get_unit (const string & unit_str_);

      //! Get a value from a string taking into account the unit symbol.
      /**
       * Example:
       *
       *   double val = datatools::utils::units::get_value_with_unit ("2.54 cm");
       */
      static double get_value_with_unit (const string & word_);
  
    };

  } // end of namespace utils

} // end of namespace datatools

#endif // __datatools__utils__units_h

// end of units.h
