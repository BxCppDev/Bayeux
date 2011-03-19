// -*- mode: c++; -*- 
/* datatools::test::the_serializable.h */

#ifndef __datatools__test__the_serializable_h__
#define __datatools__test__the_serializable_h__ 1

#include <boost/serialization/export.hpp>

#include <datatools/serialization/archives_instantiation.h>
#include <datatools/base_serializable.h>

/****************************
 * datatools::test::data_t *
 ****************************/

#include <datatools/test/my_data.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::test::data_t, "datatools::test::data_t")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::test::data_t)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::data_t) 


/********************************
 * datatools::test::more_data_t *
 ********************************/

BOOST_CLASS_EXPORT_KEY2(datatools::test::more_data_t, "datatools::test::more_data_t")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::test::more_data_t)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::more_data_t) 

#endif // __datatools__test__the_serializable_h__

/* end of datatools::test::the_serializable.h */
