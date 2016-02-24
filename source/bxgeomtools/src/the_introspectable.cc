// -*- mode: c++; -*-
/* the_introspectable.cc
 */
// Third Party
// - Boost
#include <boost/scoped_ptr.hpp>

#include <geomtools/the_introspectable.h>

// Load the link guard definition :
#include <geomtools/detail/reflection_link_guard.h>

// Load the link guard implementation :
namespace geomtools {
  namespace detail {
    namespace reflection {
      dynamic_link_guard::dynamic_link_guard()
      {
        return;
      }
      dynamic_link_guard::~dynamic_link_guard()
      {
        return;
      }
      dynamic_link_guard& dynamic_link_guard::instance()
      {
        static boost::scoped_ptr<dynamic_link_guard> g_global_guard (0);
        if ( g_global_guard.get () == 0) {
          g_global_guard.reset (new dynamic_link_guard);
        }
        return *g_global_guard.get ();
      }

    } // end namespace reflection
  } // end namespace detail
} // end namespace geomtools
