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
    ::genbb::_special_initialize_impl();
    return;
  }

  void terminate()
  {
    ::genbb::_special_terminate_impl();
    return;
  }

} // end of namespace datatools
