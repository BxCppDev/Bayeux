// boost::archive::quasi_portable_binary_oarchive.hpp

#ifndef BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_OARCHIVE_HPP
#define BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_OARCHIVE_HPP 1

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// quasi_portable_binary_oarchive.hpp

// (C) Copyright 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>. 
//                    Christian Pfligersdorffer <christian.pfligersdorffer@eos.info>. 
//
// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <ostream>
#include <boost/config.hpp>
#include <boost/qpba/quasi_portable_binary_oarchive_impl.hpp>
#include <boost/archive/detail/register_archive.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace boost { 
  namespace archive {

    // do not derive from this class.  If you want to extend this functionality
    // via inheritance, derived from quasi_portable_binary_oarchive_impl instead.
    // This will preserve correct static polymorphism.
    class quasi_portable_binary_oarchive : 
      public quasi_portable_binary_oarchive_impl<
      quasi_portable_binary_oarchive, 
      std::ostream::char_type, 
      std::ostream::traits_type
      >
    {
    public:

      quasi_portable_binary_oarchive(std::ostream & os, 
				     unsigned int flags = 0, 
				     unsigned int portability = 0) :
	quasi_portable_binary_oarchive_impl<
      quasi_portable_binary_oarchive, std::ostream::char_type, std::ostream::traits_type
      >(os, flags, portability)
      {
	return;
      }

      quasi_portable_binary_oarchive(std::streambuf & bsb, 
				     unsigned int flags = 0, 
				     unsigned int portability = 0) :
        quasi_portable_binary_oarchive_impl<
	quasi_portable_binary_oarchive, std::ostream::char_type, std::ostream::traits_type
        >(bsb, flags, portability)
      {
	return;
      }

    };

    typedef quasi_portable_binary_oarchive naked_quasi_portable_binary_oarchive;

  } // namespace archive
} // namespace boost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::quasi_portable_binary_oarchive)
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION(boost::archive::quasi_portable_binary_oarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_OARCHIVE_HPP 

// end of boost::archive::quasi_portable_binary_oarchive.hpp
