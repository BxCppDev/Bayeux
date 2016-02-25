// -*- mode: c++; -*-
/** brio_test_data.h
 *
 * A serializable sample class
 *
 */

#ifndef BRIO_TEST_DATA_H
#define BRIO_TEST_DATA_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - Boost - Portable integral types (mandatory):
#include <boost/cstdint.hpp>

// - Bayeux/datatools - Interface base class from datatools to support serialization tools:
#include <datatools/i_serializable.h>

namespace brio {

  namespace test {

    // using namespace std;

    /** A sample serializable class using datatools and/or brio serialization
     *  concept.
     */
    class data_t : public datatools::i_serializable
      {

      public:

        // Default constructor:
        data_t ();

        // Destructor:
        virtual ~data_t ();

        void randomize ();

        void dump (std::ostream & a_out = std::clog, const std::string & a_title = "") const;

      private:

        bool     __bval; // A boolean
        int8_t   __cval; /* A signed char (8 bits)
                          * imples #include <boost/cstdint.hpp>
                          */
        int32_t  __ival; // A 32 bits signed integral
        float    __fval; // A 32 bits float
        double   __dval; // A 64 bits float
        std::string __sval; /** A STL string
                             * implies #include <boost/serialization/string.hpp>
                             */
        std::vector<double> __dval_vec; /** a STL vector of 64 bits floats
                                         * implies #include <boost/serialization/vector.hpp>
                                         */

        /* interface i_serializable */
        DATATOOLS_SERIALIZATION_DECLARATION();

      };

  } // namespace test

} // namespace brio

#endif // BRIO_TEST_DATA_H
