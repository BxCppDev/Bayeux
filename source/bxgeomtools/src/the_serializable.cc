// -*- mode: c++; -*-  
/* the_serializable.cc  
 */ 

// Load the Boost/Serialization instantiation and registration code :
#include <geomtools/serialization/the_serializable.h>

// Load the link guard definition :
#include <geomtools/serialization/link_guard.h>

#include <cstdlib>
#include <iostream>
#include <boost/scoped_ptr.hpp>

// Load the link guard implementation :
namespace geomtools {
  namespace serialization {

    bool dynamic_link_guard::_g_devel_ = false;

    dynamic_link_guard::dynamic_link_guard ()
    {
      if (getenv ("GEOMTOOLS_SERIALIZATION_DEVEL") != NULL)
        {
          dynamic_link_guard::_g_devel_ = true;
        }
      if (dynamic_link_guard::_g_devel_)
        {
          std::clog << "geomtools::serialization::dynamic_link_guard::CTOR: Ok." << std::endl;
        }
      return;
    }

    dynamic_link_guard::~dynamic_link_guard ()
    {
      if (dynamic_link_guard::_g_devel_)
        {
          std::clog << "geomtools::serialization::dynamic_link_guard::DTOR: Ok." << std::endl;
        }
      return;
    }

    dynamic_link_guard & dynamic_link_guard::instance ()
    {
      static boost::scoped_ptr<dynamic_link_guard> g_global_guard (0);
      if ( g_global_guard.get () == 0)
        {
          g_global_guard.reset (new dynamic_link_guard);
        }
      if (dynamic_link_guard::_g_devel_)
        {
          std::clog << "geomtools::serialization::dynamic_link_guard::instance: Ok." << std::endl;
        }
      return *g_global_guard.get ();
    }

  } // end namespace serialization
} // end namespace geomtools

// end of the_serializable.cc
