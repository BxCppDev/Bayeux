// -*- mode: c++ ; -*-
/* or_cut.cc
 */

#include <cuts/or_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(or_cut, "cuts::or_cut");

  // ctor:
  or_cut::or_cut(datatools::logger::priority logging_priority_)
    : i_binary_cut(logging_priority_)
  {
    return;
  }

  // dtor:
  or_cut::~or_cut()
  {
    if (is_initialized()) {
      this->or_cut::reset();
    }
    return;
  }

  int or_cut::_accept()
  {
    int status_1 = _handle_1.grab ().process ();
    int status_2 = _handle_2.grab ().process ();
    if ((status_1 < 0) || (status_2 < 0)) {
      return SELECTION_INAPPLICABLE;
    }
    if ((status_1 + status_2) > 0) {
      return (SELECTION_ACCEPTED);
    }
    return (SELECTION_REJECTED);
  }

} // end of namespace cuts

// end of or_cut.cc
