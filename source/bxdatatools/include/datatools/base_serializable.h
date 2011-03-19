// -*- mode: c++; -*- 
/* datatools::base_serializable.h */

#ifndef __datatools__base_serializable_h__
#define __datatools__base_serializable_h__ 1

#include <boost/serialization/assume_abstract.hpp>
#include <datatools/serialization/utils.h>
#include <datatools/serialization/archives_instantiation.h>

/********************************************
 * datatools::serialization::i_serializable *
 ********************************************/

#include <datatools/serialization/i_serializable.h>
#include <datatools/serialization/i_serializable.ipp>
BOOST_SERIALIZATION_ASSUME_ABSTRACT(datatools::serialization::i_serializable)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::serialization::i_serializable)

#endif // __datatools__base_serializable_h__

/* end of datatools::base_serializable.h */
