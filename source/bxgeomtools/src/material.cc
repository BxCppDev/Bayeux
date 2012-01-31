// -*- mode: c++ ; -*-
/* material.cc
 */

#include <sstream>

#include <geomtools/material.h>

#include <sstream>

namespace geomtools {

  using namespace std;

  const string material::MATERIAL_PREFIX       = "material.";
  const string material::MATERIAL_REF_PROPERTY = "ref";
  const string material::MATERIAL_REF_DEFAULT  = "__default__";
  const string material::MATERIAL_REF_UNKNOWN  = "__unknown__";
  const string material::MATERIAL_REF_VACUUM   = "vacuum";
  const string material::MATERIAL_SOLID_FLAG   = "solid";
  const string material::MATERIAL_LIQUID_FLAG  = "liquid";
  const string material::MATERIAL_GAS_FLAG     = "gas";
  const string material::MATERIAL_GAS_PRESSURE_PROPERTY    = "gas_pressure";
  const string material::MATERIAL_GAS_TEMPERATURE_PROPERTY = "gas_temperature";

  string material::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << material::MATERIAL_PREFIX << key_;
    return key_oss.str ();
  }

  void material::extract (const datatools::utils::properties & source_,
			  datatools::utils::properties & target_)
  {
    source_.export_starting_with (target_, material::MATERIAL_PREFIX);
    return;
  }

  bool material::has_flag (const datatools::utils::properties & config_,
			      const string & flag_)
  {
    return (config_.has_flag (material::make_key (flag_)));
  }

  bool material::has_key (const datatools::utils::properties & config_,
			     const string & key_)
  {
    return (config_.has_key (make_key (key_)));
  }

  bool material::is_gas (const datatools::utils::properties & config_)
  {
    material::has_flag (config_, material::MATERIAL_GAS_FLAG);
  }

  bool material::is_liquid (const datatools::utils::properties & config_)
  {
    material::has_flag (config_, material::MATERIAL_LIQUID_FLAG);
  }

  bool material::is_solid (const datatools::utils::properties & config_)
  {
    material::has_flag (config_, material::MATERIAL_SOLID_FLAG);
  }

} // end of namespace geomtools

// end of material.cc
