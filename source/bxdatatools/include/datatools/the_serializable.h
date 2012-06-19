// -*- mode: c++; -*- 
/* datatools::the_serializable.h */
#ifndef DATATOOLS_THE_SERIALIZABLE_H_
#define DATATOOLS_THE_SERIALIZABLE_H_

#include <datatools/serialization/archives_instantiation.h>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>

#include <datatools/the_serializable.ipp>

/********************************************
 * datatools::serialization::i_serializable *
 ********************************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::serialization::i_serializable)

/********************************
 * datatools::utils::properties *
 ********************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::properties)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::properties) 

/**************************************
 * datatools::utils::multi_properties *
 **************************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::multi_properties)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::multi_properties) 

/****************************
 * datatools::utils::things *
 ****************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::things)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::things) 

/*********************************
 * datatools::event::event_id *
 *********************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::event::event_id)

#endif // DATATOOLS_THE_SERIALIZABLE_H_
/* end of datatools::the_serializable.h */
