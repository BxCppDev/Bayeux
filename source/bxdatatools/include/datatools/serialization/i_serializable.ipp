// -*- mode: c++; -*- 
/* i_serializable.ipp */

#ifndef __datatools__serialization__i_serializable_ipp__
#define __datatools__serialization__i_serializable_ipp__ 1

//#include <boost/serialization/nvp.hpp>
//#include <boost/serialization/base_object.hpp>
//#include <boost/serialization/assume_abstract.hpp>
//#include <boost/serialization/tracking.hpp>
//#include <boost/serialization/export.hpp>

#include <datatools/serialization/i_serializable.h>
#include <boost/serialization/export.hpp>

namespace datatools {
  
  namespace serialization {
    
    template<class Archive>
    void i_serializable::serialize (Archive & ar_, const unsigned int file_version_)
    {
      return;
    }
    
  } // end of namespace serialization 
  
} // end of namespace datatools 

//BOOST_SERIALIZATION_ASSUME_ABSTRACT(datatools::serialization::i_serializable)
//BOOST_CLASS_TRACKING(datatools::serialization::i_serializable, boost::serialization::track_always) 
//BOOST_CLASS_EXPORT_KEY2(datatools::serialization::i_serializable, "datatools::serialization::i_serializable")

#endif // __datatools__serialization__i_serializable_ipp__

/* end of i_serializable.ipp */
