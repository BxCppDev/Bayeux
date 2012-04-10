// -*- mode: c++ ; -*-
/* material.cc
 */

#include <sstream>

#include <geomtools/material.h>

#include <sstream>

namespace geomtools {

  using namespace std;

  // static 
  const material::constants & 
  material::constants::instance ()
  {
    static boost::scoped_ptr<material::constants> g_global_constants (0);
    if ( g_global_constants.get () == 0)
      {
        g_global_constants.reset (new material::constants);
      }
    return *g_global_constants.get ();
  }

  material::constants::constants ()
  {
    MATERIAL_PREFIX       = "material.";
    MATERIAL_REF_PROPERTY = "ref";
    MATERIAL_REF_DEFAULT  = "__default__";
    MATERIAL_REF_UNKNOWN  = "__unknown__";
    MATERIAL_REF_VACUUM   = "vacuum";
    MATERIAL_SOLID_FLAG   = "solid";
    MATERIAL_LIQUID_FLAG  = "liquid";
    MATERIAL_GAS_FLAG     = "gas";
    MATERIAL_GAS_PRESSURE_PROPERTY    = "gas_pressure";
    MATERIAL_GAS_TEMPERATURE_PROPERTY = "gas_temperature";
    return;
  }

  string material::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << material::constants::instance ().MATERIAL_PREFIX << key_;
    return key_oss.str ();
  }

  void material::extract (const datatools::utils::properties & source_,
                          datatools::utils::properties & target_)
  {
    source_.export_starting_with (target_, material::constants::instance ().MATERIAL_PREFIX);
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
    material::has_flag (config_, material::constants::instance ().MATERIAL_GAS_FLAG);
  }

  bool material::is_liquid (const datatools::utils::properties & config_)
  {
    material::has_flag (config_, material::constants::instance ().MATERIAL_LIQUID_FLAG);
  }

  bool material::is_solid (const datatools::utils::properties & config_)
  {
    material::has_flag (config_, material::constants::instance ().MATERIAL_SOLID_FLAG);
  }

} // end of namespace geomtools

// end of material.cc
