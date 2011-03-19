// -*- mode: c++; -*- 
/* i_serializable.ipp */

#ifndef __datatools__serialization__i_serializable_ipp__
#define __datatools__serialization__i_serializable_ipp__ 1

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/base_object.hpp>

#include <datatools/serialization/i_serializable.h>

namespace datatools {
  
  namespace serialization {
    
    template<class Archive>
    void i_serializable::serialize (Archive & ar_, const unsigned int file_version_)
    {
      return;
    }
    
  } // end of namespace serialization 
  
} // end of namespace datatools 

#endif // __datatools__serialization__i_serializable_ipp__

/* end of i_serializable.ipp */
