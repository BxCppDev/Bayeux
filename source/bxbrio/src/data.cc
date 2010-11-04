// -*- mode: c++ ; -*- 
/* data.cc
 */

#include <brio/test/data.h>

namespace brio {

  namespace test {

    const std::string data::SERIAL_TAG = "brio::test::data";
  
    const std::string & data::get_serial_tag () const
    {
      return data::SERIAL_TAG;
    }
  
    // ctor:
    data::data ()
    {
      // Initialize members with arbitrary values:
      __bval = false;
      __cval = '\0';
      __ival = 0;
      __fval = 0.0F;
      __dval = 0.0;
      __sval = "Hello world !";
      __dval_vec.reserve (10);
      __dval_vec.push_back (__dval);
      return;
    }
  
    // dtor:
    data::~data ()
    {
    }
  
  } // end of namespace test
  
} // end of namespace brio

// end of data.cc
