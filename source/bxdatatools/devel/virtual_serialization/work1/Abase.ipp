 // -*- mode: c++; -*-

#ifndef A_BASE_IPP
#define A_BASE_IPP 1
#include <Abase.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>

namespace A {
  template<class Archive>
  void base::serialize (Archive & ar, const unsigned int file_version)
  {
    return;
  }
}
#endif // A_BASE_IPP
