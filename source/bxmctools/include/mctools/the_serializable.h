// -*- mode: c++; -*-
/* the_serializable.h */

#ifndef MCTOOLS_THE_SERIALIZABLE_H_
#define MCTOOLS_THE_SERIALIZABLE_H_ 1

#include <boost/serialization/export.hpp>
#include <datatools/archives_instantiation.h>


/**************************
 * mctools::base_step_hit *
 **************************/

#include <mctools/base_step_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::base_step_hit)


/***************************
 * mctools::simulated_data *
 ***************************/

#include <mctools/simulated_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::simulated_data)
BOOST_CLASS_EXPORT_IMPLEMENT(mctools::simulated_data)


#endif // MCTOOLS_THE_SERIALIZABLE_H_

/* end of the_serializable.h */

