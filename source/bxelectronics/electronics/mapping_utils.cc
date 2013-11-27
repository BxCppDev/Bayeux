// -*- mode: c++ ; -*-
/* mapping_utils.cc
 */

// Standard library
#include <sstream>

// Third party
// - Bayeux/datatools
#include <datatools/properties.h>

// This project
#include <electronics/mapping_utils.h>

namespace electronics {

  // static
  const std::string & mapping_utils::mapping_prefix()
  {
    static std::string prefix = "mapping.";
    return prefix;
  }

  const std::string & mapping_utils::mapping_category_property()
  {
    static std::string property = "category";
    return property;
  }

  const std::string & mapping_utils::mapping_address_property()
  {
    static std::string property = "address";
    return property;
  }

  const std::string & mapping_utils::mapping_id_property()
  {
    static std::string property = "id";
    return property;
  }

  const std::string & mapping_utils::mapping_embedded_id_property()
  {
    static std::string property = "embedded_id";
    return property;
  }

  void mapping_utils::extract (const datatools::properties & source_,
                               datatools::properties & target_)
  {
    source_.export_starting_with (target_, mapping_utils::mapping_prefix());
    return;
  }

  std::string mapping_utils::make_key (const std::string & key_)
  {
    std::ostringstream key_oss;
    key_oss << mapping_utils::mapping_prefix() << key_;
    return key_oss.str ();
  }

  bool mapping_utils::has_key (const datatools::properties & config_,
                               const std::string & key_)
  {
    return (config_.has_key (mapping_utils::make_key (key_)));
  }

  bool mapping_utils::has_flag (const datatools::properties & config_,
                                const std::string & key_)
  {
    return (config_.has_flag (mapping_utils::make_key (key_)));
  }

 bool mapping_utils::has_id (const datatools::properties & config_)
 {
   return (config_.has_key (mapping_utils::make_key (mapping_utils::mapping_id_property())));
 }

  void mapping_utils::fetch_id (const datatools::properties & config_,
                                std::string & id_info_)
  {
    config_.fetch (mapping_utils::make_key (mapping_utils::mapping_id_property()), id_info_);
    return;
  }

  bool mapping_utils::has_embedded_id (const datatools::properties & config_,
                                       const std::string & embedded_name_)
  {
    std::ostringstream embedded_id_name;
    embedded_id_name << mapping_utils::make_key (mapping_utils::mapping_embedded_id_property())
                     << "." << embedded_name_;
    return (config_.has_key (embedded_id_name.str ()));
  }

  void mapping_utils::fetch_embedded_id (const datatools::properties & config_,
                                         const std::string & embedded_name_,
                                         std::string & id_info_)
  {
    std::ostringstream embedded_id_name;
    embedded_id_name << mapping_utils::make_key (mapping_utils::mapping_embedded_id_property())
                     << "." << embedded_name_;
    config_.fetch (embedded_id_name.str (), id_info_);
    return;
  }

} // end of namespace electronics

// end of mapping_utils.cc
