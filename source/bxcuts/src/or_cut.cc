// or_cut.cc

// Ourselves:
#include <cuts/or_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(or_cut, "cuts::or_cut");

  or_cut::or_cut(datatools::logger::priority logging_priority_)
    : i_binary_cut(logging_priority_)
  {
    return;
  }

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
