// -*- mode: c++ ; -*-
/* nand_cut.cc
 */

#include <cuts/nand_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(nand_cut, "cuts::nand_cut");

  // ctor:
  nand_cut::nand_cut(datatools::logger::priority logging_priority_)
    : i_binary_cut(logging_priority_)
  {
    return;
  }

  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (nand_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(nand_cut)
  {
    int status_1 = _handle_1.grab ().process ();
    int status_2 = _handle_2.grab ().process ();
    if ((status_1 < 0) || (status_2 < 0)) {
      return SELECTION_INAPPLICABLE;
    }
    if ((status_1 + status_2) == 2) {
      return (SELECTION_REJECTED);
    }
    return (SELECTION_ACCEPTED);
  }

} // end of namespace cuts

// end of nand_cut.cc
