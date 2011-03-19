// -*- mode: c++; -*- 
/* datatools::test::serializables.h */

#ifndef __datatools__test__serializables_h__
#define __datatools__test__serializables_h__ 1

#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/archives_instantiation.h>

#ifndef DATATOOLS_SERIALIZATION_BASE_CLASS_DONE
#define DATATOOLS_SERIALIZATION_BASE_CLASS_DONE
/********************************************
 * datatools::serialization::i_serializable *
 ********************************************/

#include <datatools/serialization/i_serializable.h>
#include <datatools/serialization/i_serializable.ipp>
BOOST_SERIALIZATION_ASSUME_ABSTRACT(datatools::serialization::i_serializable)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::serialization::i_serializable)

#endif // DATATOOLS_SERIALIZATION_BASE_CLASS_DONE

/****************************
 * datatools::test::data_t *
 ****************************/

#include <datatools/test/my_data.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::test::data_t, "datatools::test::data_t")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::test::data_t)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::data_t) 

BOOST_CLASS_EXPORT_KEY2(datatools::test::more_data_t, "datatools::test::more_data_t")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::test::more_data_t)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::more_data_t) 

#endif // __datatools__test__serializables_h__

/* end of datatools::test::serializables.h */
