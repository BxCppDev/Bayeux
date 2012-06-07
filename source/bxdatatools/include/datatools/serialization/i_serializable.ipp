// -*- mode: c++; -*- 
/* i_serializable.ipp */

#ifndef __datatools__serialization__i_serializable_ipp__
#define __datatools__serialization__i_serializable_ipp__ 1

#include <datatools/serialization/i_serializable.h>

//#include <boost/serialization/base_object.hpp>
//#include <boost/serialization/assume_abstract.hpp>

#include <boost/serialization/export.hpp>

namespace datatools {
  
  namespace serialization {
    
    template<class Archive>
    void i_serializable::serialize (Archive & a_ar, 
                                    const unsigned int a_file_version)
    {
      return;
    }
    
  } // end of namespace serialization 
  
} // end of namespace datatools 

// Debug :
// #include <boost/serialization/version.hpp>
//BOOST_CLASS_VERSION(datatools::serialization::i_serializable, 8)

#endif // __datatools__serialization__i_serializable_ipp__

/* end of i_serializable.ipp */
