// -*- mode: c++ ; -*-
/* mapping_utils.cc
 */

#include <sstream>

#include <geomtools/mapping_utils.h>

#include <sstream>

namespace geomtools {

  using namespace std;

  const string mapping_utils::MAPPING_PREFIX                = "mapping.";
  const string mapping_utils::MAPPING_CATEGORY_PROPERTY     = "category";
  const string mapping_utils::MAPPING_ADDRESS_PROPERTY      = "address";
  const string mapping_utils::MAPPING_ID_PROPERTY           = "id";
  const string mapping_utils::MAPPING_DAUGHTER_ID_PROPERTY  = "daughter_id";

  void mapping_utils::extract (const datatools::utils::properties & source_,
			       datatools::utils::properties & target_)
  {
    source_.export_starting_with (target_, mapping_utils::MAPPING_PREFIX);
    return;
  }

  string mapping_utils::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << mapping_utils::MAPPING_PREFIX << key_;
    return key_oss.str ();
  }

  bool mapping_utils::has_key (const datatools::utils::properties & config_,
			       const string & key_)
  {
    return (config_.has_key (mapping_utils::make_key (key_)));
  }

  bool mapping_utils::has_flag (const datatools::utils::properties & config_,
				const string & key_)
  {
    return (config_.has_flag (mapping_utils::make_key (key_)));
  }

 bool mapping_utils::has_id (const datatools::utils::properties & config_)
 {
    return (config_.has_key (mapping_utils::make_key (mapping_utils::MAPPING_ID_PROPERTY)));
 }

  void mapping_utils::fetch_id (const datatools::utils::properties & config_,
				string & id_info_)
  {
    config_.fetch (mapping_utils::make_key (mapping_utils::MAPPING_ID_PROPERTY), id_info_);
    return;
  }

  bool mapping_utils::has_daughter_id (const datatools::utils::properties & config_,
				       const string & daughter_name_)
  {
    ostringstream daughter_id_name;
    daughter_id_name << mapping_utils::make_key (mapping_utils::MAPPING_DAUGHTER_ID_PROPERTY)
		     << "." << daughter_name_;
    return (config_.has_key (daughter_id_name.str ()));
  }

  void mapping_utils::fetch_daughter_id (const datatools::utils::properties & config_,
					 const string & daughter_name_,
					 string & id_info_)
  {
    ostringstream daughter_id_name;
    daughter_id_name << mapping_utils::make_key (mapping_utils::MAPPING_DAUGHTER_ID_PROPERTY)
		     << "." << daughter_name_;
    config_.fetch (daughter_id_name.str (), id_info_);
    return;
  }

} // end of namespace geomtools

// end of mapping_utils.cc
