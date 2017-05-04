// multi_or_cut.cc

// Ourselves:
#include <cuts/multi_or_cut.h>

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
  CUT_REGISTRATION_IMPLEMENT(multi_or_cut, "cuts::multi_or_cut")


  multi_or_cut::multi_or_cut(datatools::logger::priority logging_priority_)
    : i_multi_cut(logging_priority_)
  {
    return;
  }

  multi_or_cut::~multi_or_cut()
  {
    if (is_initialized()) {
      this->multi_or_cut::reset();
    }
    return;
  }

  int multi_or_cut::_accept()
  {
    int status = SELECTION_REJECTED;
    DT_LOG_TRACE(_logging,"Entering: accept for multi-OR cut named  '" << (has_name()? get_name() : "?") << "'");
    DT_THROW_IF(_cuts.size () == 0,
                std::logic_error,
                "Missing cuts !");
    for (cuts_col_type::iterator i = _cuts.begin ();
         i != _cuts.end ();
         i++) {
      cut_handle_type & ch = *i;
      DT_LOG_TRACE(_logging,"Check cut named '" << (ch.grab().has_name()? ch.grab ().get_name() : "?") << "'");
      // if (_logging >= datatools::logger::PRIO_TRACE) {
      //   ch.grab().tree_dump(std::cerr,"CURRENT CUT:", std::string("[trace:")+BOOST_CURRENT_FUNCTION+":-]");
      // }
      status = ch.grab().process();
      if (status == SELECTION_ACCEPTED) {
        break;
      } else if (status == SELECTION_INAPPLICABLE) {
        break;
      }
    }
    DT_LOG_TRACE(_logging, "Exiting multi-OR with selection status = " << status);
    return status;
  }

} // end of namespace cuts
