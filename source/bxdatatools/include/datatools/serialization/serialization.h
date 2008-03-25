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

#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>

#include <boost/serialization/utility.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

#include <boost/cstdint.hpp>

#include <datatools/serialization/safe_serial.h>
#include <datatools/serialization/io_factory.h>
#include <datatools/serialization/i_serializable.h>

#endif // __datatools__serialization__serialization_h

/* end of serialization.h */
