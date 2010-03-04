// -*- mode: c++; -*- 
/* units.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2009-03-31
 * 
 * License: 
 * 
 * Description: 
 *  CLHEP units utilities.
 *
 * History: 
 * 
 */

#ifndef __geomtools__units_h
#define __geomtools__units_h 1

#include <stdexcept>
#include <string>

#include <geomtools/clhep.h>

namespace geomtools {

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

  };

}

#endif // __geomtools__units_h

// end of units.h
