// xnor_cut.cc

// Ourselves:
#include <cuts/xnor_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(xnor_cut, "cuts::xnor_cut")

  xnor_cut::xnor_cut (datatools::logger::priority logging_priority_)
    : i_binary_cut(logging_priority_)
  {
    return;
  }

  xnor_cut::~xnor_cut()
  {
    if (is_initialized()) {
      this->xnor_cut::reset();
    }
    return;
  }

  int xnor_cut::_accept()
  {
    int status_1 = _handle_1.grab ().process();
    int status_2 = _handle_2.grab ().process();
    if ((status_1 < 0) || (status_2 < 0)) {
      return SELECTION_INAPPLICABLE;
    }
    if ((status_1 + status_2) == 1) {
      return (SELECTION_REJECTED);
    }
    return (SELECTION_ACCEPTED);
  }

} // end of namespace cuts
