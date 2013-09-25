// -*- mode: c++ ; -*-
/* mapping_utils.cc
 */

#include <sstream>

#include <geomtools/mapping_utils.h>

#include <sstream>

namespace geomtools {

  using namespace std;

  // static
  const string & mapping_utils::mapping_prefix()
  {
    static std::string prefix = "mapping.";
    return prefix;
  }

  const string & mapping_utils::mapping_category_property()
  {
    static std::string property = "category";
    return property;
  }

  const string & mapping_utils::mapping_address_property()
  {
    static std::string property = "address";
    return property;
  }

  const string & mapping_utils::mapping_id_property()
  {
    static std::string property = "id";
    return property;
  }

  const string & mapping_utils::mapping_daughter_id_property()
  {
    static std::string property = "daughter_id";
    return property;
  }

  void mapping_utils::extract (const datatools::properties & source_,
                               datatools::properties & target_)
  {
    source_.export_starting_with (target_, mapping_utils::mapping_prefix());
    return;
  }

  string mapping_utils::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << mapping_utils::mapping_prefix() << key_;
    return key_oss.str ();
  }

  bool mapping_utils::has_key (const datatools::properties & config_,
                               const string & key_)
  {
    return (config_.has_key (mapping_utils::make_key (key_)));
  }

  bool mapping_utils::has_flag (const datatools::properties & config_,
                                const string & key_)
  {
    return (config_.has_flag (mapping_utils::make_key (key_)));
  }

 bool mapping_utils::has_id (const datatools::properties & config_)
 {
   return (config_.has_key (mapping_utils::make_key (mapping_utils::mapping_id_property())));
 }

  void mapping_utils::fetch_id (const datatools::properties & config_,
                                string & id_info_)
  {
    config_.fetch (mapping_utils::make_key (mapping_utils::mapping_id_property()), id_info_);
    return;
  }

  bool mapping_utils::has_daughter_id (const datatools::properties & config_,
                                       const string & daughter_name_)
  {
    ostringstream daughter_id_name;
    daughter_id_name << mapping_utils::make_key (mapping_utils::mapping_daughter_id_property())
                     << "." << daughter_name_;
    return (config_.has_key (daughter_id_name.str ()));
  }

  void mapping_utils::fetch_daughter_id (const datatools::properties & config_,
                                         const string & daughter_name_,
                                         string & id_info_)
  {
    ostringstream daughter_id_name;
    daughter_id_name << mapping_utils::make_key (mapping_utils::mapping_daughter_id_property())
                     << "." << daughter_name_;
    config_.fetch (daughter_id_name.str (), id_info_);
    return;
  }

} // end of namespace geomtools

// end of mapping_utils.cc
