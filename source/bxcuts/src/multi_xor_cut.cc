// multi_xor_cut.cc

// Ourselves:
#include <cuts/multi_xor_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <vector>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(multi_xor_cut, "cuts::multi_xor_cut")

  multi_xor_cut::multi_xor_cut(datatools::logger::priority logging_priority_)
    : i_multi_cut(logging_priority_)
  {
    return;
  }

  multi_xor_cut::~multi_xor_cut()
  {
    if (is_initialized()) {
      this->multi_xor_cut::reset();
    }
    return;
  }

  int multi_xor_cut::_accept()
  {
    int status = SELECTION_REJECTED;
    DT_LOG_TRACE(_logging,"Entering: accept for multi-XOR cut named  '" << (has_name()? get_name() : "?") << "'");
    DT_THROW_IF(_cuts.size () == 0,
                std::logic_error,
                "Missing cuts !");
    /**
     * See : http://en.wikipedia.org/wiki/NOR_logic#XOR
     * Input A  Input B Output Q
     * 0        0       0
     * 0        1       1
     * 1        0       1
     * 1        1       0
     * See also : http://en.wikipedia.org/wiki/One-hot
     */
    int status_sum = 0;
    for (cuts_col_type::iterator i = _cuts.begin ();
         i != _cuts.end ();
         i++) {
      cut_handle_type & ch = *i;
      status = ch.grab ().process ();
      if (status == SELECTION_INAPPLICABLE) {
        DT_LOG_TRACE(_logging, "Exiting: Multi-XOR cut is inapplicable");
        return status;
      }
      if (status == SELECTION_ACCEPTED) {
        status_sum++;
      }
    }
    status = SELECTION_ACCEPTED;
    if (status_sum != 1) status = SELECTION_REJECTED;
    DT_LOG_TRACE(_logging,
                 "Exiting multi-XOR cut with selection status = " << status);
    return status;
  }

} // end of namespace cuts
