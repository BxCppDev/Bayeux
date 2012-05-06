// -*- mode: c++; -*-  
/* the_serializable.cc  
 */ 

#include <geomtools/serialization/the_serializable.h>

#include <iostream>
#include <boost/scoped_ptr.hpp>

namespace geomtools {
  namespace serialization {
    
    _link_guard_::_link_guard_ ()
    {
      clog << "geomtools::serialization::_link_guard_::CTOR: Ok." << std::endl;
      return;
    }

    _link_guard_::~_link_guard_ ()
    {
      clog << "geomtools::serialization::_link_guard_::DTOR: Ok." << std::endl;
      return;
    }

    _link_guard_ & _link_guard_::instance ()
    {
      static boost::scoped_ptr<_link_guard_> g_global_guard (0);
      if ( g_global_guard.get () == 0)
        {
          g_global_guard.reset (new _link_guard_);
        }
      clog << "geomtools::serialization::_link_guard_::instance: Ok." << std::endl;
      return *g_global_guard.get ();
    }

  } // end namespace serialization
} // end namespace geomtools

// end of the_serializable.cc
