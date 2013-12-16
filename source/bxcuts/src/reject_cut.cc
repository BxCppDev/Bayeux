// -*- mode: c++ ; -*-
/* reject_cut.cc
 */

#include <cuts/reject_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(reject_cut, "cuts::reject_cut");

  reject_cut::reject_cut(datatools::logger::priority a_logger_priority)
    : i_cut(a_logger_priority)
  {
    return;
  }

  reject_cut::~reject_cut()
  {
    if (is_initialized()) {
      this->reject_cut::reset();
    }
    return;
  }

  int reject_cut::_accept()
  {
    return SELECTION_REJECTED;
  }

  void reject_cut::initialize(const datatools::properties & a_configuration,
                              datatools::service_manager & /*a_service_manager*/,
                              cuts::cut_handle_dict_type & /*a_cut_dict*/)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Reject cut '" << get_name () << "' is already initialized ! ");
    _common_initialize(a_configuration);
    _set_initialized (true);
    return;
  }

  void reject_cut::reset()
  {
    _set_initialized(false);
    this->i_cut::_reset();
    return;
  }

} // end of namespace cuts

// end of reject_cut.cc
