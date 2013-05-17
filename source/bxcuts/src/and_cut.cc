// -*- mode: c++ ; -*-
/* and_cut.cc
 */

#include <cuts/and_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  using namespace std;

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(and_cut, "cuts::and_cut");

  and_cut::and_cut(datatools::logger::priority logging_priority_)
    : i_binary_cut(logging_priority_)
  {
    return;
  }

  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(and_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(and_cut)
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

// end of and_cut.cc
