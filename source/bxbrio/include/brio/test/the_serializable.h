// -*- mode: c++; -*- 
/* brio::test::the_serializable.h */

#ifndef __brio__test__the_serializable_h__
#define __brio__test__the_serializable_h__ 1

#include <boost/serialization/export.hpp>

#include <datatools/serialization/archives_instantiation.h>
#include <datatools/base_serializable.h>

/****************************
 * brio::test::data_t *
 ****************************/

#include <brio/test/data.ipp>
BOOST_CLASS_EXPORT_KEY2(brio::test::data, "brio::test::data")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(brio::test::data)
BOOST_CLASS_EXPORT_IMPLEMENT(brio::test::data) 

#endif // __brio__test__the_serializable_h__

/* end of brio::test::the_serializable.h */
