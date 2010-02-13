// -*- mode: c++ ; -*- 
/* i_locator.cc
 */

#include <geomtools/i_locator.h>

namespace geomtools {
 
  bool i_locator::is_debug () const
  {
    return __debug;
  }
  
  bool i_locator::is_initialized () const
  {
    return __initialized;
  }
  
  const properties & i_locator::parameters () const
  {
    return __parameters;
  }
  
  properties & i_locator::parameters ()
  {
    return __parameters;
  }
  
  i_locator::i_locator ()
  {
    __debug = false;
    __initialized = false;
  }
  
  i_locator::~i_locator ()
  {
    if (__initialized)
      {
	reset ();
      }
  }
  

  void i_locator::initialize (const properties & params_)
  {
    if (__initialized)
      {
	throw runtime_error ("i_locator::initialize: Already initialized !");
      }
    _initialize (params_);
    __initialized = true;
  }
 
  void i_locator::reset ()
  {
    if (! __initialized)
      {
	throw runtime_error ("i_locator::initialize: Not initialized yet !");
      }
    _reset ();
    __initialized = false;
  }
  
  void i_locator::_initialize (const properties & params_)
  {
  }
 
  void i_locator::_reset ()
  {
  }

} // end of namespace geomtools

// end of i_locator.cc
