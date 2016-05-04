/* utils.cc
 */

// Ourselves:
#include <genvtx/utils.h>

// Third party:
// - Bayeux/datatools
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
// - Bayeux/geomtools
#include <geomtools/geom_info.h>

namespace genvtx {

  // static
  const int utils::MODE_INVALID;
  const int utils::MODE_BULK;
  const int utils::MODE_SURFACE;
  const int utils::MODE_DEFAULT;

  bool origin_definition::is_defined() const
  {
    return !config.empty();
  }

  void origin_definition::load(const datatools::properties & config_)
  {
    config.clear();
    if (config_.has_key("origin")) {
      config.store("rules", config_.fetch_string("origin"));
    } else {
      config_.export_and_rename_starting_with(config, "origin.", "");
    }
    return;
  }

  void origin_definition::reset()
  {
    config.reset();
    return;
  }

  void utils::origin_invalidate (std::string & origin_)
  {
    origin_.clear();
    return;
  }

  bool utils::origin_is_invalid (const std::string & origin_)
  {
    return origin_.empty ();
  }

  weight_entry_type::weight_entry_type ()
  {
    weight = 0.0;
    cumulated_weight = 0.0;
    ginfo = 0;
    return;
  }

  bool weight_entry_type::is_valid () const
  {
    return weight >= 0.0
      && cumulated_weight >= 0.0
      && ginfo != 0;
  }

  void weight_entry_type::reset ()
  {
    weight = 0.0;
    cumulated_weight = 0.0;
    ginfo = 0;
    return;
  }

  void weight_entry_type::invalidate ()
  {
    weight = -1.0;
    cumulated_weight = -1.0;
    ginfo = 0;
  }

  double weight_entry_type::get_weight() const
  {
    return weight;
  }

  double weight_entry_type::get_cumulated_weight() const
  {
    return cumulated_weight;
  }

  bool weight_entry_type::has_ginfo() const
  {
    return ginfo != 0;
  }

  const geomtools::geom_info & weight_entry_type::get_ginfo() const
  {
    return *ginfo;
  }

  void weight_info::invalidate ()
  {
    value = -1.0;
    mass = -1.0;
    type = WEIGHTING_NONE;
    return;
  }

  int weight_info::get_type() const
  {
    return type;
  }

  double weight_info::get_value() const
  {
    return value;
  }

  bool weight_info::has_type () const
  {
    return type > WEIGHTING_NONE && type <= WEIGHTING_MASS;
  }

  bool weight_info::has_surface () const
  {
    return value > 0.0 && type == WEIGHTING_SURFACE;
  }

  bool weight_info::has_volume () const
  {
    return value > 0.0 && type == WEIGHTING_VOLUME;
  }

  bool weight_info::has_mass () const
  {
    return mass > 0.0;
  }

  bool weight_info::is_valid () const
  {
    return type != WEIGHTING_NONE;
  }

  double weight_info::get_surface () const
  {
    return weight_info::value;
  }

  double weight_info::get_volume () const
  {
    return weight_info::value;
  }

  double weight_info::get_mass () const
  {
    return mass;
  }

  void weight_info::set_mass(double m)
  {
    mass = m;
  }

  void weight_info::set_value(double v)
  {
    value = v;
  }

  void weight_info::set_type(int t)
  {
    type = t;
  }

  weight_info::weight_info ()
  {
    type = WEIGHTING_NONE;
    value = -1.0;
    mass = -1.0;
    return;
  }

  void weight_info::dump(std::ostream & out_, const std::string & title_) const
  {
    if (! title_.empty()) {
      out_ << title_ << " : \n";
    }
    out_ << "|-- " << "Type  = " << type << '\n';
    out_ << "|-- " << "Value = " << value << '\n';
    out_ << "`-- " << "Mass  = " << mass/ CLHEP::gram << " g" << '\n';
    return;
  }

} // end of namespace genvtx
