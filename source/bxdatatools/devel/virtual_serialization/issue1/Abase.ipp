// -*- mode: c++; -*-

#ifndef __A__base_ipp__
#define __A__base_ipp__ 1


#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>

#include "Abase.hpp"

namespace A {

    template<class Archive>
    void base::serialize (Archive & ar, const unsigned int file_version)
    {
      return;
    }

} // end of namespace A

BOOST_SERIALIZATION_ASSUME_ABSTRACT(A::base)
BOOST_CLASS_EXPORT_KEY2(A::base, "A::base")

#endif // __A__base_ipp__
