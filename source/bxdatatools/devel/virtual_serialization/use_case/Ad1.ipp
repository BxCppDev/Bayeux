// -*- mode: c++; -*-

#ifndef __A__d1_ipp__
#define __A__d1_ipp__ 1

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include "Ad1.hpp"
#include "Ac1.ipp"

namespace A {

    template<class Archive>
    void d1::serialize (Archive & ar, const unsigned int file_version)
    {
      ar & boost::serialization::make_nvp("A::c1", boost::serialization::base_object<c1>(*this));
      ar & boost::serialization::make_nvp("j", m_j);
      return;
    }

} // end of namespace A

BOOST_CLASS_EXPORT_KEY2 (A::d1,  "A::d1")

#endif // __A__d1_ipp__
