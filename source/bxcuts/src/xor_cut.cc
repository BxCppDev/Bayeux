// xor_cut.cc

// Ourselves:
#include <cuts/xor_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(xor_cut, "cuts::xor_cut")

  xor_cut::xor_cut(datatools::logger::priority logging_priority_)
    : i_binary_cut(logging_priority_)
  {
    return;
  }

  xor_cut::~xor_cut()
  {
    if (is_initialized()) {
      this->xor_cut::reset();
    }
    return;
  }

  int xor_cut::_accept()
  {
    int status_1 = _handle_1.grab().process();
    int status_2 = _handle_2.grab().process();
    if ((status_1 == SELECTION_INAPPLICABLE) || (status_2 == SELECTION_INAPPLICABLE)) {
      return SELECTION_INAPPLICABLE;
    }
    if ((status_1 + status_2) == 1) {
      return (SELECTION_ACCEPTED);
    }
    return (SELECTION_REJECTED);
  }

} // end of namespace cuts
