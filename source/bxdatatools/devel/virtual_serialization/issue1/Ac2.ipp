// -*- mode: c++; -*-

#ifndef __A__c2_ipp__
#define __A__c2_ipp__ 1

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>

#include "Ac2.hpp"
#include "Abase.ipp"

namespace A {

    template<class Archive>
    void c2::serialize (Archive & ar, const unsigned int file_version)
    {
      ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<base>(*this));
      ar & boost::serialization::make_nvp("name", m_name);
      return;
    }

} // end of namespace A

BOOST_CLASS_EXPORT_KEY2 (A::c2, "A::c2")

#endif // __A__c2_ipp__
