// -*- mode: c++ ; -*- 
/* smart_ref.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-16
 * Last modified: 2010-03-16
 * 
 * License: 
 * 
 * Description: 
 *   Smart reference to an existing instance
 *   The referenced type must have a default constructor.
 *
 * History: 
 * 
 */

#ifndef __datatools__utils__smart_ref_h
#define __datatools__utils__smart_ref_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <list>

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/utils/properties.h>

namespace datatools {

  namespace utils {

    template <class T>
    class smart_ref
    {
    public:

      typedef T            instance_t;
      typedef T &          reference_t;
      typedef const T &    const_reference_t;
      typedef T *          pointer_t;
      typedef const T *    const_pointer_t;
      typedef std::list<T> col_t;
      typedef smart_ref<T> smart_ref_t;

    private: 

      pointer_t                    ref_;
      datatools::utils::properties properties_;

    public: 

      const datatools::utils::properties & get_properties () const
      {
	return properties_;
      }
    
      datatools::utils::properties & get_properties ()
      {
	return properties_;
      }

      void set_properties (const datatools::utils::properties & a_props)
      {
	properties_ = a_props;
      }
  
    public: 

      // ctor:
      smart_ref ()
      {
	ref_ = 0;
      }

      // ctor:
      smart_ref (const_reference_t a_obj)
      {
	set (a_obj);
 	return;
      }

      // dtor:
      virtual ~smart_ref ()
      {
	return;
       }
  
      void set (const_reference_t a_obj)
      {
	ref_ = const_cast<pointer_t>(&a_obj);
	return;
      }

      const_reference_t get () const
      {
	return *ref_;
      }

      void reset ()
      {
	ref_ = 0;
	return;
       }

      bool is_valid () const
      {
	return ref_ != 0;
      }

    private:
    
      friend class boost::serialization::access; 
      
      template<class Archive>
      void serialize (Archive            & a_ar, 
		      const unsigned int a_version) 
      {
	a_ar & boost::serialization::make_nvp ("properties", properties_);
	a_ar & boost::serialization::make_nvp ("ref",        ref_);
	return;
      }

    public:

      // predicate:
      class has_flag : public std::unary_function<smart_ref_t, bool>
      {
	string flag_;
      public:
	explicit has_flag (const string & a_flag)
	{
	  flag_ = a_flag;
	  return;
	}
    
	bool operator () (const smart_ref_t & a_smart_ref) const
	{
	  if (! a_smart_ref.is_valid ()) return false;
	  return (a_smart_ref.get_properties ().has_flag (flag_));
	}
      };
    
    };

  } // end of namespace utils 

} // end of namespace datatools

#endif // __datatools__utils__smart_ref_h

// end of smart_ref.h
