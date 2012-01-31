// -*- mode: c++ ; -*- 
/* i_boxed_model.cc
 */ 

#include <geomtools/i_boxed_model.h>

#include <geomtools/box.h>

namespace geomtools {
  
  using namespace std;  
  
  i_boxed_model::i_boxed_model (const string & a_name) : i_model (a_name)
  {
    return;
  }

  i_boxed_model::~i_boxed_model()
  {
    return;
  }
 
} // end of namespace geomtools

// end of i_boxed_model.cc
