// and_cut.cc

// Ourselves:
#include <cuts/and_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(and_cut, "cuts::and_cut")

  and_cut::and_cut(datatools::logger::priority logging_priority_)
    : i_binary_cut(logging_priority_)
  {
    return;
  }

  and_cut::~and_cut()
  {
    if (is_initialized()) {
      this->and_cut::reset();
    }
    return;
  }

  int and_cut::_accept()
  {
    int status = SELECTION_REJECTED;
    DT_LOG_TRACE(_logging,"Entering: accept for AND cut named  '" << (has_name()? get_name() : "?") << "'");
    int status_1 = _handle_1.grab ().process ();
    int status_2 = _handle_2.grab ().process ();
    if ((status_1 < 0) || (status_2 < 0)) {
      status = SELECTION_INAPPLICABLE;
      DT_LOG_TRACE(_logging, "Exiting: AND cut is inapplicable");
      return status;
    }
    if (status_1 == SELECTION_ACCEPTED && status_2 == SELECTION_ACCEPTED) {
      status = SELECTION_ACCEPTED;
    }
    DT_LOG_TRACE(_logging, "Exiting.");
    return status;
  }

} // end of namespace cuts
