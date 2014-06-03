// -*- mode: c++ ; -*-
/* brio_test_data.cc
 */

#include <cstdlib>
#include <brio_test_data.h>

namespace brio {

  namespace test {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (data_t,"brio::test::data_t")

    // ctor:
    data_t::data_t ()
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
    data_t::~data_t ()
    {
      return;
    }

    void data_t::randomize ()
    {
      __bval = drand48 () < 0.5 ? false : true;
      __cval = 'A' + (char) (drand48 () * 26);
      __ival = 0 + (char) (drand48 () * 100);
      __fval = (float) (drand48 () * -1.0);
      __dval = drand48 () * 10.;
      __sval = "Hello world !";
      __dval_vec.clear ();
      size_t sz = (size_t) (drand48 () * 10);
      __dval_vec.reserve (sz);
      for (int i = 0; i < (int) sz; i++)
        {
          __dval_vec.push_back (i * 1.0);
        }
      return;
    }

    void data_t::dump (ostream & a_out, const string & a_title) const
    {
      if (a_title.empty ())
        {
          a_out << "brio::test::data_t::dump: " << endl;
        }
      else
        {
          a_out << a_title << endl;
        }
      a_out << "|-- " << "bval  =  " << __bval << endl;
      a_out << "|-- " << "cval  =  '" << __cval << "'" << endl;
      a_out << "|-- " << "ival  =  " << __ival << endl;
      a_out << "|-- " << "fval  =  " << __fval << endl;
      a_out << "|-- " << "dval  =  " << __dval << endl;
      a_out << "|-- " << "sval  =  '" << __sval << "'" << endl;
      a_out << "`-- " << "dval_vec = {";
      for (int i = 0; i < (int) __dval_vec.size (); i++)
        {
          if (i != 0) a_out << "; ";
          a_out << __dval_vec[i];
        }
      a_out << "}" << endl;
      return;
    }

  } // end of namespace test

} // end of namespace brio

// end of brio_test_data.cc
