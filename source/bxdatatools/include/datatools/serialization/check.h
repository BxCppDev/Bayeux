// -*- mode: c++; -*- 
/* datatools::serialization::check.h */

#ifndef __datatools__serialization__check_h__
#define __datatools__serialization__check_h__ 1

#include <set>
#include <boost/assert.hpp>
#include <cstring>
#include <boost/serialization/singleton.hpp>
#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/extended_type_info_typeid.hpp>

using namespace std;

namespace boost { 
namespace serialization {
namespace detail {

  // From extended_type_info.cpp
  struct key_compare
  {
    bool
    operator()(
	       const extended_type_info * lhs, 
	       const extended_type_info * rhs
	       ) const {
      // performance shortcut
      if(lhs == rhs)
	return false;
      const char * l = lhs->get_key();
      BOOST_ASSERT(NULL != l);
      const char * r = rhs->get_key();
      BOOST_ASSERT(NULL != r);
      // performance shortcut
      // shortcut to exploit string pooling
      if(l == r)
	return false;
      // for exported types, use the string key so that
      // multiple instances in different translation units
      // can be matched up
      return std::strcmp(l, r) < 0;
    }
  };

  typedef std::multiset<const extended_type_info *, key_compare> ktmap;

}}}


namespace boost { 
namespace serialization { 
namespace typeid_system {

struct type_compare
{
    bool
    operator()(
        const extended_type_info_typeid_0 * lhs,
        const extended_type_info_typeid_0 * rhs
    ) const {
        return lhs->is_less_than(*rhs);
    }
};

typedef std::multiset<
    const extended_type_info_typeid_0 *,
    type_compare
> tkmap;
}}}

namespace datatools {

  namespace serialization {

    void print0 (ostream & a_out, 
		 const boost::serialization::extended_type_info & a_eti);

    void check0 ();

    void print (ostream & a_out, 
		const boost::serialization::typeid_system::extended_type_info_typeid_0 & a_eti);

    void check ();

  } // end of namespace serialization

} // end of namespace datatools

#endif // __datatools__serialization__check_h__

/* end of datatools::serialization::check.h */
