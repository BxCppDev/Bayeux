/// \file geomtools/blur_spot.ipp

#ifndef GEOMTOOLS_BLUR_SPOT_IPP
#define GEOMTOOLS_BLUR_SPOT_IPP 1

// Third party:
// - Boost:
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

// This project:
#include <geomtools/blur_spot.h>
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>
#include <geomtools/placement.ipp>

namespace geomtools {

  /// Boost serialization template method
  template<class Archive>
  void blur_spot::serialize(Archive & ar_ , const unsigned int version_)
  {
    if (version_ == 0) {
      if (Archive::is_loading::value) {
        // Special case for the original implementation (backward compatibility):
        ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
        ar_ & boost::serialization::make_nvp("blur_dimension", _blur_dimension_);
        ar_ & boost::serialization::make_nvp("tolerance",  _tolerance_);
        ar_ & boost::serialization::make_nvp("placement",  _placement_);
        datatools::properties aux;
        ar_ & boost::serialization::make_nvp("auxiliaries", aux);
        invalidate_auxiliaries();
        if (aux.size()) {
          grab_auxiliaries() = aux;
        }
        if (is_dimension_two() || is_dimension_three()) {
          ar_ & boost::serialization::make_nvp("x_error",  _x_error_);
          ar_ & boost::serialization::make_nvp("y_error",  _y_error_);
        }
        if (is_dimension_one() || is_dimension_three()) {
          ar_ & boost::serialization::make_nvp("z_error",  _z_error_);
        }
        return;
      }
    }

    // Inherit from the 'base_hit' mother class:
    ar_ & boost::serialization::make_nvp("geomtools__base_hit",
                                         boost::serialization::base_object<geomtools::base_hit>(*this));

    ar_ & boost::serialization::make_nvp("blur_dimension", _blur_dimension_);
    ar_ & boost::serialization::make_nvp("tolerance",  _tolerance_);
    if (_store_check(STORE_PLACEMENT)) {
      // Only load/store if store bit STORE_PLACEMENT is set:
      ar_ & boost::serialization::make_nvp("placement",  _placement_);
    }
    if (is_dimension_two() || is_dimension_three()) {
      ar_ & boost::serialization::make_nvp("x_error",  _x_error_);
      ar_ & boost::serialization::make_nvp("y_error",  _y_error_);
    }
    if (is_dimension_one() || is_dimension_three()) {
      ar_ & boost::serialization::make_nvp("z_error",  _z_error_);
    }

    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_BLUR_SPOT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
