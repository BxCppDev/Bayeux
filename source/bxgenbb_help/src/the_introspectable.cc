// the_introspectable.cc

// Third Party
// - Boost
#include <boost/scoped_ptr.hpp>

// Load the link guard definition :
#include <genbb_help/detail/reflection_link_guard.h>

// Load the link guard implementation :
namespace genbb {
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
} // end namespace genbb

#include <genbb_help/the_introspectable.h>
