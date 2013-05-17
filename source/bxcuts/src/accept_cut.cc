// -*- mode: c++ ; -*-
/* accept_cut.cc
 */

#include <cuts/accept_cut.h>

#include <stdexcept>
#include <sstream>

namespace cuts {

  using namespace std;

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(accept_cut, "cuts::accept_cut");

  // ctor:
  CUT_CONSTRUCTOR_IMPLEMENT_HEAD(accept_cut,a_logging_priority)
  {
    return;
  }

  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(accept_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(accept_cut)
  {
    return SELECTION_ACCEPTED;
  }

  CUT_INITIALIZE_IMPLEMENT_HEAD(accept_cut,
                                a_configuration,
                                a_service_manager,
                                a_dict)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Accept cut '" << get_name () << "' is already initialized !");
    _common_initialize(a_configuration);
    _set_initialized (true);
    return;
  }

  CUT_RESET_IMPLEMENT_HEAD (accept_cut)
  {
    _set_initialized (false);
    this->i_cut::_reset ();
    return;
  }

} // end of namespace cuts

// end of accept_cut.cc
