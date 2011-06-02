// -*- mode: c++ ; -*-
/* sensitive.cc
 */

#include <sstream>

#include <geomtools/sensitive.h>

namespace geomtools {

  using namespace std;

  const string sensitive::SENSITIVE_PREFIX            = "sensitive.";
  const string sensitive::SENSITIVE_CATEGORY_PROPERTY = "category";
  const string sensitive::SENSITIVE_RECORD_ALPHA_QUENCHING_FLAG  = "record_alpha_quenching";
  const string sensitive::SENSITIVE_RECORD_TRACK_ID_FLAG         = "record_track_id";
  const string sensitive::SENSITIVE_RECORD_PRIMARY_PARTICLE_FLAG = "record_primary_particle";

  string sensitive::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << sensitive::SENSITIVE_PREFIX << key_;
    return key_oss.str ();
  }

  void sensitive::extract (const datatools::utils::properties & source_,
			   datatools::utils::properties & target_)
  {
    source_.export_starting_with (target_, sensitive::SENSITIVE_PREFIX);
    return;
  }

  bool sensitive::has_flag (const datatools::utils::properties & config_,
			    const string & flag_)
  {
    return (config_.has_flag (sensitive::make_key (flag_)));
  }

  bool sensitive::has_key (const datatools::utils::properties & config_,
			   const string & key_)
  {
    return (config_.has_key (sensitive::make_key (key_)));
  }

  bool sensitive::is_sensitive (const datatools::utils::properties & config_)
  {
    return sensitive::has_key (config_, sensitive::SENSITIVE_CATEGORY_PROPERTY);
  }

  string sensitive::get_sensitive_category (const datatools::utils::properties & config_)
  {
    return config_.fetch_string (make_key (sensitive::SENSITIVE_CATEGORY_PROPERTY));
  }

  void sensitive::set_sensitive_category (datatools::utils::properties & config_,
					  const string & cat_name_)
  {
    config_.update (make_key (sensitive::SENSITIVE_CATEGORY_PROPERTY), cat_name_);
    return;
  }

  bool sensitive::recording_alpha_quenching (const datatools::utils::properties & config_)
  {
    return sensitive::has_flag (config_, sensitive::SENSITIVE_RECORD_ALPHA_QUENCHING_FLAG);
  }

  bool sensitive::recording_track_id (const datatools::utils::properties & config_)
  {
    return sensitive::has_flag (config_, sensitive::SENSITIVE_RECORD_TRACK_ID_FLAG);
  }

  bool sensitive::recording_primary_particle (const datatools::utils::properties & config_)
  {
    return sensitive::has_flag (config_, sensitive::SENSITIVE_RECORD_PRIMARY_PARTICLE_FLAG);
  }

} // end of namespace geomtools

// end of sensitive.cc
