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

  // ctor:
  CUT_CONSTRUCTOR_IMPLEMENT_HEAD(reject_cut,logging_priority_)
  {
    return;
  }

  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(reject_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(reject_cut)
  {
    return SELECTION_REJECTED;
  }

  CUT_INITIALIZE_IMPLEMENT_HEAD(reject_cut,
                                a_configuration,
                                a_service_manager,
                                a_dict)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Reject cut '" << get_name () << "' is already initialized ! ");
    _common_initialize(a_configuration);
    _set_initialized (true);
    return;
  }

  CUT_RESET_IMPLEMENT_HEAD(reject_cut)
  {
    _set_initialized(false);
    this->i_cut::_reset();
    return;
  }

} // end of namespace cuts

// end of reject_cut.cc
