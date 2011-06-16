// -*- mode: c++; -*-

#ifndef __B__dd1_ipp__
#define __B__dd1_ipp__ 1

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include "Bdd1.hpp"
#include "Ac1.ipp"

namespace B {

    template<class Archive>
    void dd1::serialize (Archive & ar, const unsigned int file_version)
    {
      ar & boost::serialization::make_nvp("A::c1", boost::serialization::base_object<A::c1>(*this));
      ar & boost::serialization::make_nvp("j", m_j);
      return;
    }

} // end of namespace B

BOOST_CLASS_EXPORT_KEY2 (B::dd1, "B::dd1")

#endif // __B__dd1_ipp__
