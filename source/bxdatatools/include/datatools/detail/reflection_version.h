#ifndef DATATOOLS_DETAIL_REFLECTION_VERSION_H
#define DATATOOLS_DETAIL_REFLECTION_VERSION_H

// MS compatible compilers support #pragma once
/*
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif
*/

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// reflection_version_h
// This code is derived from Boost/Serialization's version.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/integral_c_tag.hpp>
#include <boost/type_traits/is_base_and_derived.hpp>
#include <boost/mpl/less.hpp>
#include <boost/mpl/comparison.hpp>

namespace datatools {
  namespace detail {
    namespace reflection {

    struct basic_traits;

    //! \brief Struct used internally by the reflection mechanism for versionning
    // Default version number is 0. Override with higher version
    // when class definition changes.
    template<class T>
      struct version
      {
        //! \brief Struct used internally by the reflection mechanism for versionning
	template<class U>
	struct traits_class_version {
	  typedef BOOST_DEDUCED_TYPENAME U::version type;
	};

	typedef boost::mpl::integral_c_tag tag;
	// note: at least one compiler complained w/o the full qualification
	// on basic traits below
	typedef
        BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<
	boost::is_base_and_derived<datatools::detail::reflection::basic_traits,T>,
	  traits_class_version< T >,
	  boost::mpl::int_<0>
	  >::type type;
	BOOST_STATIC_CONSTANT(int, value = version::type::value);
      };

#ifndef BOOST_NO_INCLASS_MEMBER_INITIALIZATION
    /// Special initialization for specific compiler not supporting in class member initialization
    template<class T>
      const int version<T>::value;
#endif

    } // namespace reflection
  } // namespace detail
} // namespace datatools

#endif // DATATOOLS_DETAIL_REFLECTION_VERSION_H
