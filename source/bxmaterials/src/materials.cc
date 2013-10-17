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
    ::materials::_special_initialize_impl();
    return;
  }

  void terminate()
  {
    ::materials::_special_terminate_impl();
    return;
  }

} // end of namespace datatools
