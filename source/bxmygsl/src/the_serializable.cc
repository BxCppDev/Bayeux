// -*- mode: c++; -*-  
/* the_serializable.cc  
 */ 

#include <mygsl/the_serializable.h>

// Load the link guard definition :
#include <mygsl/detail/bio_link_guard.h>

#include <cstdlib>
#include <iostream>
#include <boost/scoped_ptr.hpp>

// Load the link guard implementation :

namespace mygsl {
namespace detail {
  namespace serialization {

    bool dynamic_link_guard::_g_devel_ = false;

    dynamic_link_guard::dynamic_link_guard ()
    {
      if (getenv ("MYGSL_SERIALIZATION_DEVEL") != NULL)
        {
          dynamic_link_guard::_g_devel_ = true;
        }
      if (dynamic_link_guard::_g_devel_)
        {
          std::clog << "mygsl::dynamic_link_guard::CTOR: Ok." << std::endl;
        }
      return;
    }

    dynamic_link_guard::~dynamic_link_guard ()
    {
      if (dynamic_link_guard::_g_devel_)
        {
          std::clog << "mygsl::dynamic_link_guard::DTOR: Ok." << std::endl;
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
          std::clog << "mygsl::dynamic_link_guard::instance: Ok." << std::endl;
        }
      return *g_global_guard.get ();
    }

  } // end namespace serialization
} // end namespace detail
} // end namespace mygsl


// end of the_serializable.cc
