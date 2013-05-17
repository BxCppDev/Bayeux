// -*- mode: c++ ; -*-
/* xnor_cut.cc
 */

#include <cuts/xnor_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  using namespace std;

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(xnor_cut, "cuts::xnor_cut");

  // ctor:
  xnor_cut::xnor_cut (datatools::logger::priority logging_priority_)
    : i_binary_cut(logging_priority_)
  {
    return;
  }

  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (xnor_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(xnor_cut)
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

// end of xnor_cut.cc
