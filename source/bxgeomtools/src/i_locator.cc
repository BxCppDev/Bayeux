// -*- mode: c++ ; -*-
/* i_locator.cc
 */

#include <geomtools/i_locator.h>

namespace geomtools {

  bool i_locator::find_geom_id (const vector_3d & position_,
                                int type_,
                                geom_id & gid_,
                                double tolerance_) const
  {
    gid_ = get_geom_id (position_, type_, tolerance_);
    return gid_.is_valid ();
  }

  datatools::logger::priority base_locator::get_logging_priority () const
  {
    return _logging_priority;
  }

  void base_locator::set_logging_priority (datatools::logger::priority lp_)
  {
    _logging_priority = lp_;
    return;
  }

  base_locator::base_locator ()
  {
    _logging_priority = datatools::logger::PRIO_WARNING;
    _geo_manager_ = 0;
    return;
  }

  base_locator::~base_locator ()
  {
    _geo_manager_ = 0;
    return;
  }

  void base_locator::set_geo_manager (const manager & mgr_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Locator is already initialized !");
    _geo_manager_ = &mgr_;
    return;
  }

  const manager & base_locator::get_geo_manager() const
  {
    return *_geo_manager_;
  }

  void base_locator::_basic_initialize (const datatools::properties & config_)
  {
    set_logging_priority(datatools::logger::extract_logging_configuration(config_,datatools::logger::PRIO_WARNING));
    return;
  }

} // end of namespace geomtools

// end of i_locator.cc
