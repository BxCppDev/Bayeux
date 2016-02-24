// the_introspectable.cc

// Ourselves :
#include <datatools/the_introspectable.h>

// Third Party:
// - Boost:
#include <boost/smart_ptr/scoped_ptr.hpp>

// This project:
// Load the link guard definition :
#include <datatools/detail/reflection_link_guard.h>

// Load the link guard implementation :
namespace datatools {
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
        static boost::scoped_ptr<dynamic_link_guard> _guard(0);
        if (_guard.get() == 0) {
          _guard.reset(new dynamic_link_guard);
        }
        return *_guard.get();
      }

    } // end namespace reflection
  } // end namespace detail
} // end namespace datatools
