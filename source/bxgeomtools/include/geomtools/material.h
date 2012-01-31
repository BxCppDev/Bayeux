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

#include <datatools/utils/properties.h>

namespace geomtools {

  using namespace std;

  class material
  {
  public:

    static const string MATERIAL_PREFIX;
    static const string MATERIAL_REF_PROPERTY;
    static const string MATERIAL_REF_DEFAULT;
    static const string MATERIAL_REF_UNKNOWN;
    static const string MATERIAL_REF_VACUUM;
    static const string MATERIAL_SOLID_FLAG;
    static const string MATERIAL_LIQUID_FLAG;
    static const string MATERIAL_GAS_FLAG;
    static const string MATERIAL_GAS_PRESSURE_PROPERTY;
    static const string MATERIAL_GAS_TEMPERATURE_PROPERTY;

    static string make_key (const string & flag_);

    static void extract (const datatools::utils::properties & source_,
			 datatools::utils::properties & target_);

    static bool has_flag (const datatools::utils::properties & config_,
			  const string & flag_);

    static bool has_key (const datatools::utils::properties & config_,
			 const string & key_);

    static bool is_gas (const datatools::utils::properties & config_);

    static bool is_liquid (const datatools::utils::properties & config_);

    static bool is_solid (const datatools::utils::properties & config_);

  };


}  // end of namespace geomtools

#endif // __geomtools__material_h

// end of material.h
