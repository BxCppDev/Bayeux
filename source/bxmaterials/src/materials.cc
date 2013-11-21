/* materials.cc */

// This project
#include <materials/materials_config.h>

// Ourselves
#include <materials/materials.h>

// Third Party
// - datatools
#include <datatools/kernel.h>

namespace materials {

  void initialize(int argc_, char * argv_[])
  {
    static bool _init = false;
    if (!_init) {
      ::materials::_special_initialize_impl();
      _init = true;
    } else {
#if MATERIALS_WITH_IMPLICIT_INIT_FINI == 0
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "Attempt to initialize the already initialized Bayeux/materials library module !");
#endif
    }
    return;
  }

  void terminate()
  {
    static bool _terminate = false;
    if (!_terminate) {
      ::materials::_special_terminate_impl();
      _terminate = true;
    } else {
#if MATERIALS_WITH_IMPLICIT_INIT_FINI == 0
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "Attempt to terminate the already terminated Bayeux/materials library module !");
#endif
    }
    return;
  }

} // end of namespace datatools
