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
  }
  
  // reset:
  manager::~manager ()
  {
    if (__initialized) 
      {
	reset ();
      }
  }
  
  void manager::initialize ()
  {
    if (__initialized)
      {
	throw runtime_error ("genvtx::manager::initialize: Already initialized !");
      }
    __random.init (__rng_id, __rng_seed);

    __initialized = true;
  }
  
  void manager::reset ()
  {
    if (! __initialized)
      {
	throw runtime_error ("genvtx::manager::reset: Not initialized !");
      }

    __initialized = false;
  }

  void manager::shoot_vertex (geomtools::vector_3d & vertex_)
  {
  }
  
} // end of namespace genvtx

// end of manager.cc
