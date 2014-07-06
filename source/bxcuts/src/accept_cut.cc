// -*- mode: c++ ; -*-
/* accept_cut.cc
 */

#include <cuts/accept_cut.h>

#include <stdexcept>
#include <sstream>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(accept_cut, "cuts::accept_cut");

  // ctor:
  accept_cut::accept_cut(datatools::logger::priority a_logger_priority)
    : i_cut(a_logger_priority)
  {
    return;
  }

  // dtor:
  accept_cut::~accept_cut()
  {
    if (is_initialized()) {
      this->accept_cut::reset();
    }
    return;
  }

  int accept_cut::_accept()
  {
    return SELECTION_ACCEPTED;
  }

  void accept_cut::initialize(const datatools::properties & a_configuration,
                              datatools::service_manager & /*a_service_manager*/,
                              cuts::cut_handle_dict_type & /*a_cut_dict*/)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Accept cut '" << get_name () << "' is already initialized !");
    _common_initialize(a_configuration);
    _set_initialized (true);
    return;
  }

  void accept_cut::reset()
  {
    _set_initialized (false);
    this->i_cut::_reset ();
    return;
  }

} // end of namespace cuts

// end of accept_cut.cc
