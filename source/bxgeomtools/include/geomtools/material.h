// -*- mode: c++; -*-
/* material.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-13
 * Last modified: 2010-03-13
 *
 * License:
 *
 * Description:
 *  Utilities for material reference.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_MATERIAL_H_
#define GEOMTOOLS_MATERIAL_H_ 1

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <cmath>
#include <list>

#include <datatools/properties.h>

namespace geomtools {

  class material
  {
  public:

    static std::string & material_prefix();
    static std::string & material_ref_property();
    static std::string & material_ref_default();
    static std::string & material_ref_unknown();
    static std::string & material_ref_vacuum();
    static std::string & material_solid_flag();
    static std::string & material_liquid_flag();
    static std::string & material_gas_flag();
    static std::string & material_gas_pressure_property();
    static std::string & material_gas_temperature_property();

    static std::string make_key (const std::string & flag_);

    static void extract (const datatools::properties & source_,
                         datatools::properties & target_);

    static bool has_flag (const datatools::properties & config_,
                          const std::string & flag_);

    static bool has_key (const datatools::properties & config_,
                         const std::string & key_);

    static bool is_gas (const datatools::properties & config_);

    static bool is_liquid (const datatools::properties & config_);

    static bool is_solid (const datatools::properties & config_);

  };


}  // end of namespace geomtools

#endif // GEOMTOOLS_MATERIAL_H_

// end of material.h
