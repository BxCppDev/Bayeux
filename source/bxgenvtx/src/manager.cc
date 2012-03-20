// -*- mode: c++ ; -*- 
/* manager.cc
 */

#include <genvtx/manager.h>

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <algorithm>

#include <geomtools/utils.h>

namespace genvtx {
 
  using namespace std;

  bool manager::is_debug () const
  {
    return _debug_;
  }
  
  void manager::set_debug (bool new_value_)
  {
    _debug_ = new_value_;
    return;
  }
  
  bool manager::is_initialized () const
  {
    return _initialized_;
  }
  
  const string & manager::get_rng_id () const
  {
    return _rng_id_;
  }
  
  // ctor:
  manager::manager ()
  {
    _debug_ = false;
    _initialized_ = false;
    _rng_seed_ = 0; 
    _rng_id_ = "taus2";
    return;
  }
  
  // reset:
  manager::~manager ()
  {
    if (_initialized_) 
      {
        reset ();
      }
    return;
  }
  
  void manager::initialize ()
  {
    if (_initialized_)
      {
        throw logic_error ("genvtx::manager::initialize: Already initialized !");
      }
    _random_.init (_rng_id_, _rng_seed_);

    _initialized_ = true;
    return;
  }
  
  void manager::reset ()
  {
    if (! _initialized_)
      {
        throw logic_error ("genvtx::manager::reset: Not initialized !");
      }
    _initialized_ = false;
    return;
  }

  void manager::shoot_vertex (geomtools::vector_3d & vertex_)
  {
    return;
  }
  
} // end of namespace genvtx

// end of manager.cc
