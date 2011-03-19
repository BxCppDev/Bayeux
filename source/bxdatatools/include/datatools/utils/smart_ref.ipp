// -*- mode: c++; -*- 
//! \file datatools/utils/smart_ref.ipp

#ifndef __datatools__utils__smart_ref_ipp
#define __datatools__utils__smart_ref_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp> // missing header: fixed 2010-03-16
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/utils/smart_ref.h>
	 
namespace datatools {
		 
  namespace utils {

    // !!! NOT USED YET !!!
    // FUTURE: could we move the (double templatized) serialize function implementation here ?

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__smart_ref_ipp
