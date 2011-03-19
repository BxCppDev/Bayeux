// -*- mode: c++; -*- 

#ifndef __B__serialization_ipp__
#define __B__serialization_ipp__ 1

#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>

#include "archives.hpp"

#include <Abase.hpp>

/*********
 * B::c3 *
 *********/

#include <Bc3.ipp>
BOOST_CLASS_EXPORT_KEY2 (B::c3, "B::c3")
template void B::c3::serialize (boost::archive::text_iarchive & ar, const unsigned int file_version);
template void B::c3::serialize (boost::archive::text_oarchive & ar, const unsigned int file_version);
template void B::c3::serialize (boost::archive::xml_iarchive & ar, const unsigned int file_version);
template void B::c3::serialize (boost::archive::xml_oarchive & ar, const unsigned int file_version);
template void B::c3::serialize (boost::archive::binary_iarchive & ar, const unsigned int file_version);
template void B::c3::serialize (boost::archive::binary_oarchive & ar, const unsigned int file_version);
BOOST_CLASS_EXPORT_IMPLEMENT (B::c3)

#endif // __B__serialization_ipp__
