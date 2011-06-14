// -*- mode: c++; -*-

#ifndef __A__c1_ipp__
#define __A__c1_ipp__ 1

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

#include "Ac1.hpp"
#include "Abase.ipp"

namespace A {

    template<class Archive>
    void c1::serialize (Archive & ar, const unsigned int file_version)
    {
      ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<base>(*this));
      ar & boost::serialization::make_nvp("i", m_i);
      return;
    }

} // end of namespace A

BOOST_CLASS_EXPORT_KEY2 (A::c1, "A::c1")

#endif // __A__c1_ipp__
