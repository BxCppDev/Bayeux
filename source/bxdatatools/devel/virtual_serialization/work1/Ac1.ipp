// -*- mode: c++; -*-

#ifndef A_C1_IPP
#define A_C1_IPP 1
#include <Ac1.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>

namespace A {
  template<class Archive>
  void c1::serialize (Archive & ar, const unsigned int file_version)
  {
    ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<base>(*this));
    ar & boost::serialization::make_nvp("i", m_i);
    return;
  }
}
#endif // A_C1_IPP
