// -*- mode: c++; -*-
//! \file datatools/test/my_data.ipp

#ifndef DATATOOLS_TEST_MY_DATA_IPP
#define DATATOOLS_TEST_MY_DATA_IPP

// Ourselves:
#include <datatools_test_my_data.h>

// Third Party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/vector.hpp> // missing header: fixed 2010-03-16
#include <boost/serialization/string.hpp>

// This project:
#include <datatools/utils.h>
#include <datatools/i_serializable.ipp>

namespace datatools {

  namespace test {

    template<class Archive>
    void data_t::serialize (Archive            & a_ar,
                            const unsigned int   /*a_version*/)
    {
      a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      a_ar & boost::serialization::make_nvp ("flags", _flags_);
      a_ar & boost::serialization::make_nvp ("ival",  _ival_);
      a_ar & boost::serialization::make_nvp ("dval",  _dval_);
      a_ar & boost::serialization::make_nvp ("dvals", _dvals_);
      return;
    }

    template<class Archive>
    void more_data_t::serialize (Archive &          a_ar,
                                 const unsigned int /*a_version*/)
    {
      a_ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP (data_t);
      a_ar & boost::serialization::make_nvp ("name", _name_);
      return;
    }

  } // end of namespace test

} // end of namespace datatools

#endif // DATATOOLS_TEST_MY_DATA_IPP
