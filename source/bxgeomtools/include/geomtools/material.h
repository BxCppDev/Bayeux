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

#ifndef __geomtools__material_h
#define __geomtools__material_h 1

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

    struct constants
    {
      std::string MATERIAL_PREFIX;
      std::string MATERIAL_REF_PROPERTY;
      std::string MATERIAL_REF_DEFAULT;
      std::string MATERIAL_REF_UNKNOWN;
      std::string MATERIAL_REF_VACUUM;
      std::string MATERIAL_SOLID_FLAG;
      std::string MATERIAL_LIQUID_FLAG;
      std::string MATERIAL_GAS_FLAG;
      std::string MATERIAL_GAS_PRESSURE_PROPERTY;
      std::string MATERIAL_GAS_TEMPERATURE_PROPERTY;

      constants ();

      static const constants & instance ();
    };

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

#endif // __geomtools__material_h

// end of material.h
