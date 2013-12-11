// -*- mode: c++ ; -*-
/* material.cc
 */

#include <sstream>

#include <geomtools/material.h>

#include <sstream>

namespace geomtools {

  using namespace std;

  // static
  std::string & material::material_prefix()
  {
    static std::string prefix = "material.";
    return prefix;
  }

  std::string & material::material_ref_property()
  {
    static std::string prefix = "ref";
    return prefix;
  }

  std::string & material::material_ref_default()
  {
    static std::string token = "__default__";
    return token;
  }

  std::string & material::material_ref_unknown()
  {
    static std::string token = "__unknown__";
    return token;
  }

  std::string & material::material_ref_vacuum()
  {
    static std::string token = "__vacuum__";
    return token;
  }

  std::string & material::material_solid_flag()
  {
    static std::string token = "solid";
    return token;
  }

  std::string & material::material_liquid_flag()
  {
    static std::string token = "liquid";
    return token;
  }

  std::string & material::material_gas_flag()
  {
    static std::string token = "gas";
    return token;
  }

  std::string & material::material_gas_pressure_property()
  {
    static std::string token = "gas_pressure";
    return token;
  }

  std::string & material::material_gas_temperature_property()
  {
    static std::string token = "gas_temperature";
    return token;
  }

  string material::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << material::material_prefix() << key_;
    return key_oss.str ();
  }

  void material::extract (const datatools::properties & source_,
                          datatools::properties & target_)
  {
    source_.export_starting_with (target_, material::material_prefix());
    return;
  }

  bool material::has_flag (const datatools::properties & config_,
                              const string & flag_)
  {
    return (config_.has_flag (material::make_key (flag_)));
  }

  bool material::has_key (const datatools::properties & config_,
                             const string & key_)
  {
    return (config_.has_key (make_key (key_)));
  }

  bool material::is_gas (const datatools::properties & config_)
  {
    return material::has_flag (config_, material::material_gas_flag());
  }

  bool material::is_liquid (const datatools::properties & config_)
  {
    return material::has_flag (config_, material::material_liquid_flag());
  }

  bool material::is_solid (const datatools::properties & config_)
  {
    return material::has_flag (config_, material::material_solid_flag());
  }

} // end of namespace geomtools

// end of material.cc
