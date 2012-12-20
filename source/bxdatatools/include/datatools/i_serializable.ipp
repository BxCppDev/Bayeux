// -*- mode: c++; -*- 
/* i_serializable.ipp */
#ifndef DATATOOLS_I_SERIALIZABLE_IPP_
#define DATATOOLS_I_SERIALIZABLE_IPP_

// Ourself
#include <datatools/i_serializable.h>

// Standard Library

// Third Party
// - Boost
#include <boost/serialization/export.hpp>

namespace datatools {
    
DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(i_serializable, archive, version)
{
}

} // end of namespace datatools 

#endif // DATATOOLS_I_SERIALIZABLE_IPP_

