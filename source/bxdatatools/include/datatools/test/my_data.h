// -*- mode: c++; -*-
/** my_data.h 
 *
 * A serializable test class
 * 
 */

#ifndef __datatools__test__my_data_h
#define __datatools__test__my_data_h 1

#include <iostream>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include <datatools/serialization/i_serializable.h>
#include <datatools/utils/i_tree_dump.h>

namespace datatools {

  namespace test {

    class data_t : 
      public datatools::serialization::i_serializable,     
      public datatools::utils::i_tree_dumpable
    {
    public:

      static const std::string SERIAL_TAG;

      virtual const std::string & get_serial_tag () const;

    private:

      int8_t   __flags;
      int32_t  __ival;
      double   __dval;
      std::vector<double> __dvals;

    public:

      void init ();

      virtual ~data_t ();

      data_t ();

      data_t (char f_, int i_, double d_, size_t n_);

      const int8_t  & flags () const { return __flags; }

      const int32_t & ival ()  const { return __ival; }

      const double  & dval ()  const { return __dval; }

      const std::vector<double> & dvals () const { return __dvals; }
  
    private:

      friend class boost::serialization::access; 

      template<class Archive>
      void serialize (Archive            & ar_, 
		      const unsigned int   version_)
      {
	ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	ar_ & boost::serialization::make_nvp ("flags", __flags);
	ar_ & boost::serialization::make_nvp ("ival",  __ival);
	ar_ & boost::serialization::make_nvp ("dval",  __dval);
	ar_ & boost::serialization::make_nvp ("dvals", __dvals);
	return;
      }

    public:

      virtual void tree_dump (std::ostream & out_         = std::clog, 
			      const std::string & title_  = "", 
			      const std::string & indent_ = "", 
			      bool inherit_ = false) const;

    };

    /****************************************************************************/

    class more_data_t : public data_t
    {
    public:

      static const std::string SERIAL_TAG;

      virtual const std::string & get_serial_tag () const;

    private:

      std::string __name;

    public:

      more_data_t ();

      more_data_t (char c_, int i_, double d_, size_t n_, 
		   const std::string & name_);

      virtual ~more_data_t ();

      const std::string & name () const { return __name; }

    private:
      friend class boost::serialization::access; 
      template<class Archive>
      void serialize (Archive &          ar_, 
		      const unsigned int version_)
      {
	ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP (data_t);
	ar_ & boost::serialization::make_nvp ("name", __name);
	return;
      }

    public:

      virtual void tree_dump (std::ostream & out_         = std::clog, 
			      const std::string & title_  = "", 
			      const std::string & indent_ = "", 
			      bool inherit_ = false) const;

    };
 
    void randomize_data (data_t & data_);
      
    void randomize_more_data (more_data_t & data_);

  } // namespace test 

} // namespace datatools 

BOOST_CLASS_EXPORT_KEY2 (datatools::test::data_t, "datatools::test::data_t")

BOOST_CLASS_EXPORT_KEY2 (datatools::test::more_data_t, "datatools::test::more_data_t")

#endif // __datatools__test__my_data_h

/* end of  my_data.h */
