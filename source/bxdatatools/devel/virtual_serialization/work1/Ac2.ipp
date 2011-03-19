// -*- mode: c++; -*-

#ifndef A_C2_IPP
#define A_C2_IPP 1

#include <Ac2.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>

namespace A {
  template<class Archive>
  void c2::serialize (Archive & ar, const unsigned int file_version)
  {
    ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<base>(*this));
    ar & boost::serialization::make_nvp("name", m_name);
    return;
  }
} // end of namespace A

#endif // A_C2_IPP