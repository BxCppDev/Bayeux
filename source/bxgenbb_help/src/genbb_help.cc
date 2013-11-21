/* genbb_help.cc */

// This project
#include <genbb_help/genbb_help_config.h>

// Ourselves
#include <genbb_help/genbb_help.h>

// Third Party
// - datatools
#include <datatools/kernel.h>

namespace genbb {

  void initialize(int argc_, char * argv_[])
  {
    static bool _init = false;
    if (!_init) {
      ::genbb::_special_initialize_impl();
      _init = true;
    } else {
#if GENBB_HELP_WITH_IMPLICIT_INIT_FINI == 0
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "Attempt to initialize the already initialized Bayeux/genbb_help library module !");
#endif
    }
    return;
  }

  void terminate()
  {
    static bool _terminate = false;
    if (!_terminate) {
      ::genbb::_special_terminate_impl();
      _terminate = true;
    } else {
#if GENBB_HELP_WITH_IMPLICIT_INIT_FINI == 0
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "Attempt to terminate the already terminated Bayeux/genbb_help library module !");
#endif
    }
    return;
  }

} // end of namespace genbb
