// -*- mode: c++ ; -*-
/// \file geomtools/display_data.ipp

#ifndef GEOMTOOLS_DISPLAY_DATA_IPP
#define GEOMTOOLS_DISPLAY_DATA_IPP 1

// Ourselves:
#include <geomtools/display_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.h>
#include <datatools/properties.ipp>

// This project:
#include <geomtools/polyline_3d.ipp>

namespace geomtools {

  /// Boost serialization template method
  template<class Archive>
  void display_data::display_item::serialize (Archive & ar,
                                              const unsigned int version)
  {
    ar & boost::serialization::make_nvp ("frame_info", frame_info);
    if (version > 0)
      {
        ar & boost::serialization::make_nvp ("style",      style);
      }
    else
      {
        if (Archive::is_saving::value)
          {
            ar & boost::serialization::make_nvp ("style",      style);
         }
        else
          {
            style.clear();
          }
      }
    ar & boost::serialization::make_nvp ("color",      color);
    ar & boost::serialization::make_nvp ("paths",      paths);
    return;
  }


  template<class Archive>
  void display_data::display_entry::serialize (Archive & ar,
                                               const unsigned int /*version*/)
  {
    ar & boost::serialization::make_nvp ("entry_type",  entry_type);
    ar & boost::serialization::make_nvp ("group",       group);
    ar & boost::serialization::make_nvp ("items",       items);
    ar & boost::serialization::make_nvp ("auxiliaries", auxiliaries);
    return;
  }


  template<class Archive>
  void display_data::serialize (Archive & ar,
                                const unsigned int version)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("groups",      _groups_);
    ar & boost::serialization::make_nvp ("entries",     _entries_);
    if (version > 0) {
      ar & boost::serialization::make_nvp ("frames", _frames_);
    } else {
      if (Archive::is_saving::value) {
        ar & boost::serialization::make_nvp ("frames", _frames_);
      } else {
        _frames_.clear();
      }
    }
    ar & boost::serialization::make_nvp ("colors",      _colors_);
    ar & boost::serialization::make_nvp ("auxiliaries", _auxiliaries_);
    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_DISPLAY_DATA_IPP
