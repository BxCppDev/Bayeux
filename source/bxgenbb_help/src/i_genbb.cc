// -*- mode: c++; -*- 
/* i_genbb.cc
 */

#include <genbb_help/i_genbb.h>

namespace genbb {
 
  i_genbb::i_genbb ()
  {
  }
  
  i_genbb::~i_genbb ()
  {
  }

  void i_genbb::load_next (primary_event & event_, 
			   bool compute_classification_)
  {
    _load_next (event_, compute_classification_);
    return;
  }

} // end of namespace genbb

// end of i_genbb.cc
