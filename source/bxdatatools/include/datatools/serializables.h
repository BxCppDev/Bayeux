// -*- mode: c++; -*- 
/* serializables.h */

#ifndef __datatools__serializables__serializables_h__
#define __datatools__serializables__serializables_h__ 1

#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/archives_list.h>
#include <datatools/serialization/archives_instantiation.h>

/********************************************
 * datatools::serialization::i_serializable *
 ********************************************/

#include <datatools/serialization/i_serializable.h>
#include <datatools/serialization/i_serializable.ipp>
BOOST_SERIALIZATION_ASSUME_ABSTRACT(datatools::serialization::i_serializable)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::serialization::i_serializable)


/********************************
 * datatools::utils::properties *
 ********************************/
#include <datatools/utils/properties.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::properties, "datatools::utils::properties")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::properties)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::properties) 


/**************************************
 * datatools::utils::multi_properties *
 **************************************/
#include <datatools/utils/multi_properties.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::multi_properties, "datatools::utils::multi_properties")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::multi_properties)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::multi_properties) 


/****************************
 * datatools::utils::things *
 ****************************/
#include <datatools/utils/things.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::things, "datatools::utils::things")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::things)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::things) 


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


/*********************************
 * datatools::event::event_id *
 *********************************/
#include <datatools/event/event_id.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::event::event_id, "datatools::event::event_id")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::event::event_id)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::event::event_id) 


/*********************************
 * datatools::event::basic_event *
 *********************************/
#include <datatools/event/basic_event.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::event::basic_event, "datatools::event::basic_event")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::event::basic_event)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::event::basic_event) 

#endif // __datatools__serialization__serializables_h__

/* end of serializables.h */
