// -*- mode: c++ ; -*- 
/* sensitive_hit.cc
 */

#include <stdexcept>
#include <sstream>
#include <iostream>

#include <mctools/g4/sensitive_hit.h>

//#include <G4THitsCollection.hh>

namespace mctools {

  namespace g4 {

    const base_step_hit & sensitive_hit::get_hit_data () const
    {
      return _hit_data_;
    }
    
    base_step_hit & sensitive_hit::grab_hit_data ()
    {
      return _hit_data_;
    }
 
    // ctor:
    sensitive_hit::sensitive_hit () 
    { 
       return;
    }
    
    // dtor:
    sensitive_hit::~sensitive_hit ()
    {
      //reset ();
      return;
    }
    
    void sensitive_hit::reset ()
    {
      _hit_data_.reset ();
      return;
    }
    
  } // end of namespace g4

} // end of namespace mctools

// end of sensitive_hit.cc
