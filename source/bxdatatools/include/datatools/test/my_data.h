// -*- mode: c++; -*-
/** my_data.h 
 *
 * Two serializable test classes
 * 
 * Use recommandations from:
 *   http://www.boost.org/doc/libs/1_46_1/libs/serialization/doc/index.html
 *
 */

#ifndef __datatools__test__my_data_h
#define __datatools__test__my_data_h 1

#include <iostream>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

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
      BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

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
      BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

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

#endif // __datatools__test__my_data_h

/* end of  my_data.h */
