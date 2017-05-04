// nand_cut.cc

// Ourselves:
#include <cuts/nand_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(nand_cut, "cuts::nand_cut")

  nand_cut::nand_cut(datatools::logger::priority logging_priority_)
    : i_binary_cut(logging_priority_)
  {
    return;
  }

  nand_cut::~nand_cut()
  {
    if (is_initialized()) {
      this->nand_cut::reset();
    }
    return;
  }

  int nand_cut::_accept()
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
