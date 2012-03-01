// genbb_help_config.cc
// -*- mode: c++; -*-

#include <genbb_help/genbb_help_config.h>

namespace genbb {

  bool library_config::g_use_old_serialization_tag = false; 

  void library_config::set_use_old_serialization_tag ( bool u_ )
  {
    library_config::g_use_old_serialization_tag = u_;
    return;
  }
  
} // end of namespace genbb

// end of genbb_help_config.cc
