// -*- mode: c++; -*- 
/* genbb_help.cc
 */

#include <genbb_help/genbb_help.h>

namespace genbb {

  bool library_config::g_use_old_serialization_tag = false; 

  void library_config::set_use_old_serialization_tag (bool u_ )
  {
    library_config::g_use_old_serialization_tag = u_;
    return;
  }
  
} // end of namespace genbb

// end of genbb_help.cc
