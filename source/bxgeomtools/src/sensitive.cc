// -*- mode: c++ ; -*-
/* sensitive.cc
 */

#include <sstream>

#include <geomtools/sensitive.h>

namespace geomtools {

  using namespace std;

  // static
  const sensitive::constants &
  sensitive::constants::instance ()
  {
    static boost::scoped_ptr<sensitive::constants> g_global_constants (0);
    if ( g_global_constants.get () == 0) {
        g_global_constants.reset (new sensitive::constants);
      }
    return *g_global_constants.get ();
  }

  sensitive::constants::constants ()
  {
    SENSITIVE_PREFIX            = "sensitive.";
    SENSITIVE_CATEGORY_PROPERTY = "category";
    return;
  }

  string sensitive::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << sensitive::constants::instance().SENSITIVE_PREFIX << key_;
    return key_oss.str ();
  }

  void sensitive::extract (const datatools::properties & source_,
                           datatools::properties & target_)
  {
    source_.export_starting_with (target_, sensitive::constants::instance().SENSITIVE_PREFIX);
    return;
  }

  bool sensitive::has_flag (const datatools::properties & config_,
                            const string & flag_)
  {
    return (config_.has_flag (sensitive::make_key (flag_)));
  }

  bool sensitive::has_key (const datatools::properties & config_,
                           const string & key_)
  {
    return (config_.has_key (sensitive::make_key (key_)));
  }

  bool sensitive::is_sensitive (const datatools::properties & config_)
  {
    return sensitive::has_key (config_, sensitive::constants::instance().SENSITIVE_CATEGORY_PROPERTY);
  }

  string sensitive::get_sensitive_category (const datatools::properties & config_)
  {
    return config_.fetch_string (make_key (sensitive::constants::instance().SENSITIVE_CATEGORY_PROPERTY));
  }

  void sensitive::set_sensitive_category (datatools::properties & config_,
                                          const string & cat_name_)
  {
    config_.update (make_key (sensitive::constants::instance().SENSITIVE_CATEGORY_PROPERTY), cat_name_);
    return;
  }

} // end of namespace geomtools

// end of sensitive.cc
