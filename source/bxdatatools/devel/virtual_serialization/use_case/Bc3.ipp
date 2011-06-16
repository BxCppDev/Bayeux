// -*- mode: c++; -*-

#ifndef __B__c3_ipp__
#define __B__c3_ipp__ 1

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include "Abase.ipp"
#include "Bc3.hpp"

namespace B {
  
  template<class Archive>
  void c3::serialize (Archive & ar, const unsigned int file_version)
  {
    ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<A::base>(*this));
    ar & boost::serialization::make_nvp("c", m_c);
    return;
  }
  
} // end of namespace B

BOOST_CLASS_EXPORT_KEY2 (B::c3, "B::c3")

#endif // __B__c3_ipp__
