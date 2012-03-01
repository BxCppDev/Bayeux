/* serialization.h */

// useful header to include serialization stuff from boost

#ifndef __datatools__serialization__serialization_h
#define __datatools__serialization__serialization_h 1

//#include <boost/archive/detail/oserializer.hpp>
//#include <boost/archive/detail/iserializer.hpp>

#include <datatools/serialization/io_factory.h>
#include <datatools/serialization/safe_serial.h>
#include <datatools/serialization/i_serializable.h>

#include <boost/cstdint.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>

#endif // __datatools__serialization__serialization_h

/* end of serialization.h */
