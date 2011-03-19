// -*- mode: c++; -*- 

#ifndef __A__serialization_ipp__
#define __A__serialization_ipp__ 1

#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include "archives.hpp"

#include <Abase.ipp>
BOOST_SERIALIZATION_ASSUME_ABSTRACT(A::base)
template void A::base::serialize (boost::archive::text_iarchive & ar, const unsigned int file_version);
template void A::base::serialize (boost::archive::text_oarchive & ar, const unsigned int file_version);
template void A::base::serialize (boost::archive::xml_iarchive & ar, const unsigned int file_version);
template void A::base::serialize (boost::archive::xml_oarchive & ar, const unsigned int file_version);
template void A::base::serialize (boost::archive::binary_iarchive & ar, const unsigned int file_version);
template void A::base::serialize (boost::archive::binary_oarchive & ar, const unsigned int file_version);

// registration of classes:

/*********
 * A::c1 *
 *********/

#include <Ac1.ipp>
BOOST_CLASS_EXPORT_KEY2 (A::c1, "A::c1")
template void A::c1::serialize (boost::archive::text_iarchive & ar, const unsigned int file_version);
template void A::c1::serialize (boost::archive::text_oarchive & ar, const unsigned int file_version);
template void A::c1::serialize (boost::archive::xml_iarchive & ar, const unsigned int file_version);
template void A::c1::serialize (boost::archive::xml_oarchive & ar, const unsigned int file_version);
template void A::c1::serialize (boost::archive::binary_iarchive & ar, const unsigned int file_version);
template void A::c1::serialize (boost::archive::binary_oarchive & ar, const unsigned int file_version);
BOOST_CLASS_EXPORT_IMPLEMENT (A::c1)

/*********
 * A::c2 *
 *********/

#include <Ac2.ipp>
BOOST_CLASS_EXPORT_KEY2 (A::c2, "A::c2")
template void A::c2::serialize (boost::archive::text_iarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::text_oarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::xml_iarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::xml_oarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::binary_iarchive & ar, const unsigned int file_version);
template void A::c2::serialize (boost::archive::binary_oarchive & ar, const unsigned int file_version);
BOOST_CLASS_EXPORT_IMPLEMENT (A::c2)

#endif // __A__serialization_ipp__
