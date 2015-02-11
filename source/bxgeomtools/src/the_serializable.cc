// the_serializable.cc

// Standard library:
#include <cstdlib>
#include <iostream>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
// Load the Boost/Serialization instantiation and registration code :
#include <geomtools/the_serializable.h>
// Load the link guard definition :
#include <geomtools/detail/bio_link_guard.h>

// Load the link guard implementation :
namespace geomtools {
  namespace detail {
    namespace serialization {

      dynamic_link_guard::dynamic_link_guard ()
      {
        return;
      }

      dynamic_link_guard::~dynamic_link_guard ()
      {
        return;
      }

      dynamic_link_guard & dynamic_link_guard::instance ()
      {
        static boost::scoped_ptr<dynamic_link_guard> g_global_guard (0);
        if ( g_global_guard.get () == 0)
          {
            g_global_guard.reset (new dynamic_link_guard);
          }
        return *g_global_guard.get ();
      }

    } // end namespace serialization
  } // end namespace detail
} // end namespace geomtools
