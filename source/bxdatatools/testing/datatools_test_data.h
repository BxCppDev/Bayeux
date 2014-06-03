// -*- mode: c++; -*-

#ifndef __data_h
#define __data_h 1

#include <string>

// Portable integral types (mandatory):
#include <boost/cstdint.hpp>

// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>

// Support Boost serialization of STL string objects:
#include <boost/serialization/string.hpp>

// Support Boost serialization of STL vector templatized objects:
#include <boost/serialization/vector.hpp>

namespace datatools {

  namespace test {

    using namespace std;

    /** A class that is serializable via Boost archives.
     */
    class data
    {
    private:

      bool     __bval; // A boolean
      int8_t   __cval; /* A signed char (8 bits)
                        * implies #include <boost/cstdint.hpp>
                        */
      int32_t  __ival; // A 32 bits signed integral
      float    __fval; // A 32 bits float
      double   __dval; // A 64 bits float
      string   __sval; /** A STL string
                        * implies #include <boost/serialization/string.hpp>
                        */
      std::vector<double> __dval_vec; /** a STL vector of 64 bits floats
                                       * implies #include <boost/serialization/vector.hpp>
                                       */

    public:

      // Here we provide a default constructor:
      data ()
        {
          // Initialize members with arbitrary values (this could be randomized for fun !):
          __bval = false;
          __cval = '\0';
          __ival = 0;
          __fval = 0.0F;
          __dval = 0.0;
          __sval = "Hello world !";
          __dval_vec.reserve (10); // pre-allocate the vector's capacity for memory optimization
          __dval_vec.push_back (__dval);
          __dval_vec.push_back (__dval * 2);
          __dval_vec.push_back (__dval * 3); // only use 3 elements
          return;
        }

      // Destructor:
      virtual ~data ()
        {
          // here '__dval_vec' is automaticaly cleared
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
        void serialize (Archive            & ar_,       // an archive type (could be ASCII, XML, binary...)
                        const unsigned int   /*version_*/)  // the version number (here not used)
        {
          ar_ & boost::serialization::make_nvp ("bval", __bval); /* the 'make_nvp' ensure XML archive support.
                                                                  * NVP stands for name/value pair as expected
                                                                  * by XML markup structures.
                                                                  */
          ar_ & boost::serialization::make_nvp ("cval", __cval);
          ar_ & boost::serialization::make_nvp ("ival", __ival);
          ar_ & boost::serialization::make_nvp ("fval", __fval);
          ar_ & boost::serialization::make_nvp ("dval", __dval);
          ar_ & boost::serialization::make_nvp ("sval", __sval);
          ar_ & boost::serialization::make_nvp ("dval_vec", __dval_vec);
          return;
        }

    };

  } // end of namespace test

} // end of namespace datatools

#endif // __data_h
