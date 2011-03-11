// -*- mode: c++; -*- 
/* my_data.cc 
 */

#include <datatools/test/my_data.h>

namespace datatools {

  namespace test {

    const std::string data_t::SERIAL_TAG = "datatools::test::data_t";

    const std::string & data_t::get_serial_tag () const
    {
      return data_t::SERIAL_TAG;
    }

    void data_t::init ()
    {
      __flags = 'a';
      __ival = 666;
      __dval = 3.14;
      return;
    }

    data_t::~data_t ()
    {
      __dvals.clear ();
      return;
    }

    data_t::data_t () : datatools::serialization::i_serializable ()
    {
      init ();
      return;
    }

    data_t::data_t (char f_, int i_, double d_, size_t n_) 
    {
      init ();
      __flags = f_;
      __ival = i_;
      __dval = d_;
      double v = 10.;
      for (int i = 0; i < (int) n_; i++) 
	{
	  __dvals.push_back (v);
	  v *= 3.;
	}
      return;
    }
      
    void data_t::tree_dump (std::ostream & out_, 
			    const std::string & title_, 
			    const std::string & indent_, 
			    bool inherit_) const
    {
      using namespace datatools::utils;
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! title_.empty ()) {
	out_ << indent << title_ << std::endl;
      }
      out_ << indent << i_tree_dumpable::tag << "Address : [" << this << ']' << std::endl;
      out_ << indent << i_tree_dumpable::tag << "flags : '" << __flags << "'" << std::endl;
      out_ << indent << i_tree_dumpable::tag << "ival  : "  << __ival << std::endl;
      out_ << indent << i_tree_dumpable::tag << "dval  : "  
	   << __dval << std::endl;
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	   << "dvals[" << __dvals.size () << "] : ";
      for (int i = 0; i < (int) __dvals.size (); i++)
	{ 
	  out_ << __dvals[i] << ' ';
	}
      out_ << std::endl;
      return;
    }

    /****************************************************************************/

    const std::string more_data_t::SERIAL_TAG = "datatools::test::more_data_t";

    more_data_t::more_data_t () : data_t ('z', -1, 3.14159, 3) 
    {
      __name = "<anonymous>";
      return;
    }

    more_data_t::more_data_t (char c_, int i_, double d_, size_t n_, 
			      const std::string & name_) 
      : data_t (c_, i_, d_, n_)
    {
      __name = name_;
      return;
    }

    more_data_t::~more_data_t ()
    {
      return;
    }

    const std::string & more_data_t::get_serial_tag () const
    {
      return more_data_t::SERIAL_TAG;
    }
      
    void more_data_t::tree_dump (std::ostream & out_, 
				 const std::string & title_, 
				 const std::string & indent_, 
				 bool inherit_) const
    {
      using namespace datatools::utils;
      std::string indent;
      if (! indent_.empty ()) indent = indent_;
      data_t::tree_dump (out_, title_, indent_, true);
      out_ << indent << i_tree_dumpable::inherit_tag (inherit_) 
	   << "name = "  << __name << std::endl;
      return;
    } 

    void randomize_data (data_t & data_)
    {
      const data_t d ((char) (32 + drand48 () * 90), 
		      (int) (drand48 () * 1000), 
		      (double) drand48 (), 
		      1 + (size_t) (drand48 () * 5));
      data_ = d;
      return;
    }

    void randomize_more_data (more_data_t & data_)
    {
      std::ostringstream name_oss;
      name_oss << "data_" << (int) (drand48 () * 1000);
      const more_data_t md ((char) (32 + drand48 () * 90), 
			    (int) (drand48 () * 1000), 
			    (double) drand48 (), 
			    1 + (size_t) (drand48 () * 5), 
			    name_oss.str ());
      data_ = md;
      return;
    }

  } // end of namespace test 

} // end of namespace datatools 

BOOST_CLASS_EXPORT_IMPLEMENT (datatools::test::data_t)
BOOST_CLASS_EXPORT_IMPLEMENT (datatools::test::more_data_t)

// end of my_data.cc
