//! \file datatools/things.ipp

#ifndef DATATOOLS_THINGS_IPP
#define DATATOOLS_THINGS_IPP

// Ourselves:
#include <datatools/things.h>

// Third Party:
// - Boost:
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

// This Project:
#include <datatools/i_serializable.ipp>
#include <datatools/utils.h>

namespace datatools {

  /// Boost serialization template method
  template<class Archive>
  void things::entry_type::serialize(Archive & archive, const unsigned int version __attribute__((unused)))
  {
    archive & boost::serialization::make_nvp("description", description);
    archive & boost::serialization::make_nvp("flags",       flags);
    archive & boost::serialization::make_nvp("handle",      handle);
  }

  /// Boost serialization template method
  template<class Archive>
  void things::serialize(Archive & archive, const unsigned int version)
  {
    if (version == 0) {
      archive & DATATOOLS_SERIALIZATION_OLD_I_SERIALIZABLE_BASE_OBJECT_NVP;
    }
    else {
      archive & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    }
    archive & boost::serialization::make_nvp("name",        name_);
    archive & boost::serialization::make_nvp("description", description_);
    archive & boost::serialization::make_nvp("things",      things_);
  }

} // end of namespace datatools

#endif // DATATOOLS_THINGS_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
