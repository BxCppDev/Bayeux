// -*- mode: c++ ; -*- 
/* utils.cc
 */

#include <genvtx/utils.h>

#include <datatools/utils.h>
#include <geomtools/geom_info.h>

namespace genvtx {
      
  void utils::origin_invalidate (std::string & origin_)
  {
    origin_.clear ();
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
  
  void weight_info::invalidate ()
  {
    value = -1.0;
    mass = -1.0;
    type = WEIGHTING_NONE;
    return;
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

  weight_info::weight_info ()
  {
    type = WEIGHTING_NONE;
    value = -1.0;
    mass = -1.0;
    return;
  }

} // end of namespace genvtx

// end of utils.cc
