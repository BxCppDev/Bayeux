// -*- mode: c++; -*-
//! \file geomtools/helix_3d.ipp

#ifndef __geomtools__helix_3d_ipp
#define __geomtools__helix_3d_ipp 1

#include <geomtools/helix_3d.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/i_serializable.ipp>
#include <geomtools/utils.ipp>

namespace geomtools {

  template<class Archive>
  void helix_3d::serialize (Archive & a_ar , const unsigned int a_version)
  {
    if (a_version > 0)
      {
        /* from version 1 we inherit explicitely from the
         * 'datatools::serialization::i_serializable' abstract class
         */
        a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
    a_ar & boost::serialization::make_nvp ("radius", _radius_);
    if (_radius_ == _radius_)
      {
        a_ar & boost::serialization::make_nvp ("center", _center_);
        a_ar & boost::serialization::make_nvp ("step", _step_);
        a_ar & boost::serialization::make_nvp ("t1", _t1_);
        a_ar & boost::serialization::make_nvp ("t2", _t2_);
      }
    return;
  }

} // end of namespace geomtools

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::helix_3d, 1)

#endif // __geomtools__helix_3d_ipp

// end of helix_3d.ipp
