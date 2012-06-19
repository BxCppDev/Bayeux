// -*- mode: c++; -*- 
/* i_serializable.ipp */
#ifndef DATATOOLS_SERIALIZATION_I_SERIALIZABLE_IPP_
#define DATATOOLS_SERIALIZATION_I_SERIALIZABLE_IPP_

#include <datatools/serialization/i_serializable.h>

#include <boost/serialization/export.hpp>

namespace datatools {
namespace serialization {
    
template<class Archive>
void i_serializable::serialize(Archive& a_ar, const unsigned int a_file_version) {}
} // end of namespace serialization 
} // end of namespace datatools 

#endif // DATATOOLS_SERIALIZATION_I_SERIALIZABLE_IPP_

