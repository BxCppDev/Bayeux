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

//#include <datatools/serialization/serialization.h>
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

      pointer_t                        __ref;
      datatools::utils::properties __properties;

    public: 

      const datatools::utils::properties & get_properties () const
      {
	return __properties;
      }
    
      datatools::utils::properties & get_properties ()
      {
	return __properties;
      }

      void set_properties (const datatools::utils::properties & props_)
      {
	__properties = props_;
      }
  
    public: 

      // ctor:
      smart_ref ()
      {
	__ref = 0;
      }

      // ctor:
      smart_ref (const_reference_t obj_)
      {
	set (obj_);
      }

      // dtor:
      virtual ~smart_ref ()
      {
      }
  
      void set (const_reference_t obj_)
      {
	__ref = const_cast<pointer_t>(&obj_);
      }

      const_reference_t get () const
      {
	return *__ref;
      }

      void reset ()
      {
	__ref = 0;
      }

      bool is_valid () const
      {
	return __ref != 0;
      }

    private:
    
      friend class boost::serialization::access; 
      
      template<class Archive>
      void serialize (Archive            & ar_, 
		      const unsigned int version_) 
      {
	ar_ & boost::serialization::make_nvp ("properties", __properties);
	ar_ & boost::serialization::make_nvp ("ref",        __ref);
	return;
      }

    public:

      // predicate:
      class has_flag : public std::unary_function<smart_ref_t,  string>
      {
	string __flag;
      public:
	explicit has_flag (const string & flag_)
	{
	  __flag = flag_;
	}
    
	bool operator () (const smart_ref_t & smart_ref_) const
	{
	  if (! smart_ref_.is_valid ()) return false;
	  return (smart_ref_.get_properties ().has_flag (__flag));
	}
      };
    
    };

  } // end of namespace utils 

} // end of namespace datatools

#endif // __datatools__utils__smart_ref_h

// end of smart_ref.h
