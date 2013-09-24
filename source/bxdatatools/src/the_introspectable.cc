// -*- mode: c++; -*-
/* the_introspectable.cc
 */

// Standard Library
#include <iostream>

// Ourselves :
#include <datatools/the_introspectable.h>

// Third Party
// - Boost
#include <boost/scoped_ptr.hpp>

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
      std::cerr << "datatools::detail::reflection::dynamic_link_guard: Construction..." << std::endl;
      bool devel = false;
      devel = true;
      static boost::scoped_ptr<dynamic_link_guard> guard (0);
      if ( guard.get () == 0) {
        guard.reset (new dynamic_link_guard);
      }
      if (devel) {
        std::cerr << "datatools::detail::reflection::dynamic_link_guard::instance: Ok." << std::endl;
      }
      std::cerr << "datatools::detail::reflection::dynamic_link_guard: Construction done." << std::endl;
      return *guard.get ();
    }

} // end namespace reflection
} // end namespace detail
} // end namespace datatools

// end of the_introspectable.cc
