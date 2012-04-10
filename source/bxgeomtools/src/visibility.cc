// -*- mode: c++ ; -*-
/* visibility.cc
 */

#include <sstream>

#include <geomtools/visibility.h>

#include <sstream>

namespace geomtools {

  using namespace std;

  // static 
  const visibility::constants & 
  visibility::constants::instance ()
  {
    static boost::scoped_ptr<visibility::constants> g_global_constants (0);
    if ( g_global_constants.get () == 0)
      {
        g_global_constants.reset (new visibility::constants);
      }
    return *g_global_constants.get ();
  }

  visibility::constants::constants ()
  {
    
    VISIBILITY_PREFIX = "visibility.";
    
    VISIBILITY_HIDDEN_FLAG           = "hidden";
    VISIBILITY_HIDDEN_ENVELOP_FLAG   = "hidden_envelop";
    VISIBILITY_HIDDEN_ENVELOPE_FLAG  = "hidden_envelope";
    VISIBILITY_DAUGHTERS_HIDDEN_FLAG = "daughters.hidden";
    VISIBILITY_COLOR_PROPERTY        = "color";
    VISIBILITY_WIRED_CYLINDER_FLAG   = "wired_cylinder";
    
    // VISIBILITY_FORCE_SHOWN_FLAG      = "daughters.invisible";
    // VISIBILITY_FORCE_HIDDEN_FLAG     = "daughters.visible";
    return;
  }

  string visibility::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << visibility::constants::instance().VISIBILITY_PREFIX << key_;
    return key_oss.str ();
  }

  void visibility::extract (const datatools::utils::properties & source_,
                            datatools::utils::properties & target_)
  {
    source_.export_starting_with (target_, visibility::constants::instance().VISIBILITY_PREFIX);
    return;
  }

  bool visibility::has_flag (const datatools::utils::properties & config_,
                             const string & flag_)
  {
    return (config_.has_flag (visibility::make_key (flag_)));
  }

  bool visibility::has_key (const datatools::utils::properties & config_,
                            const string & key_)
  {
    return (config_.has_key (visibility::make_key (key_)));
  }

  bool visibility::is_shown (const datatools::utils::properties & config_)
  {
    bool shown = true; // default:
    if (visibility::has_flag (config_, visibility::constants::instance().VISIBILITY_HIDDEN_FLAG))
      {
        shown = false;
      }
    return shown;
  }

  bool visibility::is_hidden (const datatools::utils::properties & config_)
  {
    return ! visibility::is_shown (config_);
  }

  bool visibility::is_hidden_envelop (const datatools::utils::properties & config_)
  {
    return visibility::has_flag (config_, visibility::constants::instance().VISIBILITY_HIDDEN_ENVELOP_FLAG);
  }

  bool visibility::is_hidden_envelope (const datatools::utils::properties & config_)
  {
    return visibility::has_flag (config_, visibility::constants::instance().VISIBILITY_HIDDEN_ENVELOPE_FLAG);
  }

  bool visibility::is_daughters_shown (const datatools::utils::properties & config_)
  {
    return ! visibility::is_daughters_hidden (config_);
  }

  bool visibility::is_daughters_hidden (const datatools::utils::properties & config_)
  {
    return visibility::has_flag (config_, visibility::constants::instance().VISIBILITY_DAUGHTERS_HIDDEN_FLAG);
  }

  bool visibility::has_color (const datatools::utils::properties & config_)
  {
    return visibility::has_key (config_, visibility::constants::instance().VISIBILITY_COLOR_PROPERTY);
  }

  string visibility::get_color (const datatools::utils::properties & config_)
  {
    if (! has_color (config_)) return "";
    return config_.fetch_string (visibility::make_key (visibility::constants::instance().VISIBILITY_COLOR_PROPERTY));
  }

  void visibility::set_color (datatools::utils::properties & config_,
                              const string & color_)
  {
    config_.update (visibility::make_key (visibility::constants::instance().VISIBILITY_COLOR_PROPERTY), color_);
    return;
  }

  bool visibility::is_wired_cylinder (const datatools::utils::properties & config_)
  {
    return visibility::has_flag (config_, visibility::constants::instance().VISIBILITY_WIRED_CYLINDER_FLAG);
  }


} // end of namespace geomtools

// end of visibility.cc
