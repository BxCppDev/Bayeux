/* serialization.h */

// useful header to include serialization stuff from boost

#ifndef __datatools__serialization__serialization_h
#define __datatools__serialization__serialization_h 1

#include <boost/archive/detail/oserializer.hpp>
#include <boost/archive/detail/iserializer.hpp>
/*
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
*/
#include <boost/serialization/serialization.hpp>
  //#include <boost/serialization/access.hpp>
  //#include <boost/serialization/nvp.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/cstdint.hpp>

#endif // __datatools__serialization__serialization_h

/* end of serialization.h */
