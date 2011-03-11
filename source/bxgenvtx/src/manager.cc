// -*- mode: c++ ; -*- 
/* manager.cc
 */

#include <genvtx/manager.h>

namespace genvtx {

  bool manager::is_debug () const
  {
    return __debug;
  }
  
  void manager::set_debug (bool new_value_)
  {
    __debug = new_value_;
    return;
  }
  
  bool manager::is_initialized () const
  {
    return __initialized;
  }
  
  const string & manager::get_rng_id () const
  {
    return __rng_id;
  }
  
  // ctor:
  manager::manager ()
  {
    __debug = false;
    __initialized = false;
    __rng_seed = 0; 
    __rng_id = "taus2";
    return;
  }
  
  // reset:
  manager::~manager ()
  {
    if (__initialized) 
      {
	reset ();
      }
    return;
  }
  
  void manager::initialize ()
  {
    if (__initialized)
      {
	throw runtime_error ("genvtx::manager::initialize: Already initialized !");
      }
    __random.init (__rng_id, __rng_seed);

    __initialized = true;
    return;
  }
  
  void manager::reset ()
  {
    if (! __initialized)
      {
	throw runtime_error ("genvtx::manager::reset: Not initialized !");
      }
    __initialized = false;
    return;
  }

  void manager::shoot_vertex (geomtools::vector_3d & vertex_)
  {
    return;
  }
  
} // end of namespace genvtx

// end of manager.cc
