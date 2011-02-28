// -*- mode: c++ ; -*- 
/* data.cc
 */

#include <cstdlib>
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

    void data::randomize ()
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
      for (int i = 0; i < sz; i++)
	{
	  __dval_vec.push_back (i * 1.0);
	}
      return;
    }
  
    void data::dump (ostream & out_) const
    {
      out_ << "brio::test::data::dump: " << endl;
      out_ << "|-- " << "bval  =  " << __bval << endl;
      out_ << "|-- " << "cval  =  '" << __cval << "'" << endl;
      out_ << "|-- " << "ival  =  " << __ival << endl;
      out_ << "|-- " << "fval  =  " << __fval << endl;
      out_ << "|-- " << "dval  =  " << __dval << endl;
      out_ << "|-- " << "sval  =  '" << __sval << "'" << endl;
      out_ << "`-- " << "dval_vec = {";
      for (int i = 0; i < __dval_vec.size (); i++)
	{
	  if (i != 0) out_ << "; ";
	  out_ << __dval_vec[i];
	}
      out_ << "}" << endl;
      return;
    }
  
  } // end of namespace test
  
} // end of namespace brio

// end of data.cc
