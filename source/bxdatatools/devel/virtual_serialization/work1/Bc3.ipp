// -*- mode: c++; -*-

#ifndef B_C3_IPP
#define B_C3_IPP 1

#include <Bc3.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>

namespace B {

  template<class Archive>
  void c3::serialize (Archive & ar, const unsigned int file_version)
  {
    ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<A::base>(*this));
    ar & boost::serialization::make_nvp("c", m_c);
    return;
  }

} // end of namespace B

#endif // B_C3_IPP
