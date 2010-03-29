// -*- mode: c++ ; -*- 
/* sensitive.cc
 */

#include <geomtools/sensitive.h>

namespace geomtools {

  using namespace std;

  const string sensitive::SENSITIVE_PREFIX            = "sensitive.";
  const string sensitive::SENSITIVE_CATEGORY_PROPERTY = "category";

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
    return (config_.has_key (make_key (key_)));
  }

  bool sensitive::is_sensitive (const datatools::utils::properties & config_)
  {
    return sensitive::has_key (config_, sensitive::SENSITIVE_CATEGORY_PROPERTY);
  }

  string sensitive::get_sensitive_category (const datatools::utils::properties & config_)
  {
    return config_.fetch_string (make_key (sensitive::SENSITIVE_CATEGORY_PROPERTY));
  }

} // end of namespace geomtools

// end of sensitive.cc
