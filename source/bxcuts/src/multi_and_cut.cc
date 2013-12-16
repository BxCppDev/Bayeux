// -*- mode: c++ ; -*-
/* multi_and_cut.cc
 */

#include <cuts/multi_and_cut.h>

#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <vector>

#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(multi_and_cut, "cuts::multi_and_cut");

  multi_and_cut::multi_and_cut(datatools::logger::priority logging_priority_)
    : i_multi_cut(logging_priority_)
  {
    return;
  }

  multi_and_cut::~multi_and_cut()
  {
    if (is_initialized()) {
      this->multi_and_cut::reset();
    }
    return;
  }

  int multi_and_cut::_accept()
  {
    int status = SELECTION_ACCEPTED;
    DT_LOG_TRACE(_logging,"Entering: accept for multi-AND cut named  '" << (has_name()? get_name() : "?") << "'");
    DT_THROW_IF(_cuts.size () == 0,
                std::logic_error,
                "Missing cuts !");
    for (cuts_col_type::iterator i = _cuts.begin ();
         i !=  _cuts.end ();
         i++) {
      cut_handle_type & ch = *i;
      status = ch.grab ().process ();
      if (status == SELECTION_INAPPLICABLE) {
        DT_LOG_TRACE(_logging, "Exiting multi-AND cut is inapplicable");
        return status;
      }
      if (status != SELECTION_ACCEPTED) {
        break;
      }
    }
    DT_LOG_TRACE(_logging, "Exiting multi-AND cut with selection status = " << status);
    return status;
  }

} // end of namespace cuts

// end of multi_and_cut.cc
