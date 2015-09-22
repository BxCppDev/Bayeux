// -*- mode: c++; -*-

#ifndef DATA_H
#define DATA_H 1

#include <string>

// Portable integral types(mandatory):
#include <boost/cstdint.hpp>

// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>

// Support Boost serialization of STL string objects:
#include <boost/serialization/string.hpp>

// Support Boost serialization of STL vector templatized objects:
#include <boost/serialization/vector.hpp>

namespace datatools {

  namespace test {

    /// \brief A class that is serializable via Boost archives.
    class data
    {
    private:

      bool     _bval_; ///< A boolean
      int8_t   _cval_; ///< A signed char(8 bits) from <boost/cstdint.hpp>
      int32_t  _ival_; ///< A 32 bits signed integral
      float    _fval_; ///< A 32 bits float
      double   _dval_; ///< A 64 bits float
      std::string _sval_; ///< A STL string from <boost/serialization/string.hpp>
      std::vector<double> _dval_vec_; /// a STL vector of 64 bits floats from <boost/serialization/vector.hpp>

    public:

      bool is_bval() const {
        return _bval_;
      }

      // Here we provide a default constructor:
      data() {
        // Initialize members with arbitrary values(this could be randomized for fun !):
        _bval_ = false;
        _cval_ = '\0';
        _ival_ = 0;
        _fval_ = 0.0F;
        _dval_ = 0.0;
        _sval_ = "Hello, world !";
        _dval_vec_.reserve(10); // pre-allocate the vector's capacity for memory optimization
        _dval_vec_.push_back(_dval_);
        _dval_vec_.push_back(_dval_ * 2);
        _dval_vec_.push_back(_dval_ * 3); // only use 3 elements
        return;
      }

      // Destructor:
      virtual ~data() {
        // here '_dval_vec_' is automaticaly cleared
      }

    private:

      /* Boost serialization concept implies to add the following
       * private template `serialize' method in our class. This acts
       * as a streamer.
       *
       * Here each field member is serialized in the archive 'ar_'.
       * Note 'ar_' is templatized, so any class that implements the Boost I/O archive
       * interface will match this method.
       *
       */

      friend class boost::serialization::access;

      template<class Archive>
      void serialize(Archive & ar_,       // an archive type(could be ASCII, XML, binary...)
                     const unsigned int /*version_*/)  // the version number(here not used)
      {
        ar_ & boost::serialization::make_nvp("bval", _bval_); /* the 'make_nvp' ensure XML archive support.
                                                               * NVP stands for name/value pair as expected
                                                               * by XML markup structures.
                                                               */
        ar_ & boost::serialization::make_nvp("cval", _cval_);
        ar_ & boost::serialization::make_nvp("ival", _ival_);
        ar_ & boost::serialization::make_nvp("fval", _fval_);
        ar_ & boost::serialization::make_nvp("dval", _dval_);
        ar_ & boost::serialization::make_nvp("sval", _sval_);
        ar_ & boost::serialization::make_nvp("dval_vec", _dval_vec_);
        return;
      }

    };

  } // end of namespace test

} // end of namespace datatools

#endif // DATA_H
