// boost::archive::quasi_portable_binary_iarchive.hpp

#ifndef BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_IARCHIVE_HPP
#define BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_IARCHIVE_HPP 1

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// quasi_portable_binary_iarchive.hpp

// (C) Copyright 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>. 
//                    Christian Pfligersdorffer <christian.pfligersdorffer@eos.info>. 
//
// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <istream>
#include <boost/qpba/quasi_portable_binary_iarchive_impl.hpp>
#include <boost/archive/detail/register_archive.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace boost { 
  namespace archive {

    // do not derive from the classes below.  If you want to extend this functionality
    // via inhertance, derived from quasi_portable_binary_iarchive_impl instead.  This will
    // preserve correct static polymorphism.

    // same as quasi_portable_binary_iarchive below - without the shared_ptr_helper
    class naked_quasi_portable_binary_iarchive : 
      public quasi_portable_binary_iarchive_impl<
      boost::archive::naked_quasi_portable_binary_iarchive, 
      std::istream::char_type, 
      std::istream::traits_type
      >
    {
    public:

      naked_quasi_portable_binary_iarchive(std::istream & is, 
					   unsigned int flags = 0, 
					   unsigned int portability = 0) :
        quasi_portable_binary_iarchive_impl<
      naked_quasi_portable_binary_iarchive, std::istream::char_type, std::istream::traits_type
      >(is, flags, portability)
      {
	return;
      }

      naked_quasi_portable_binary_iarchive(std::streambuf & bsb, 
					   unsigned int flags = 0, 
					   unsigned int portability = 0) :
        quasi_portable_binary_iarchive_impl<
	naked_quasi_portable_binary_iarchive, std::istream::char_type, std::istream::traits_type
        >(bsb, flags, portability)
      {
	return;
      }

    };

  } // namespace archive
} // namespace boost

// note special treatment of shared_ptr. This type needs a special
// structure associated with every archive.  We created a "mix-in"
// class to provide this functionality.  Since shared_ptr holds a
// special esteem in the boost library - we included it here by default.
#include <boost/archive/shared_ptr_helper.hpp>

namespace boost { 
  namespace archive {

    // do not derive from this class.  If you want to extend this functionality
    // via inheritance, derived from quasi_portable_binary_iarchive_impl instead.  
    // This will preserve correct static polymorphism.
    class quasi_portable_binary_iarchive : 
      public quasi_portable_binary_iarchive_impl<
      boost::archive::quasi_portable_binary_iarchive, 
      std::istream::char_type, 
      std::istream::traits_type
      >,
      public detail::shared_ptr_helper
    {
    public:
      quasi_portable_binary_iarchive(std::istream & is, 
				     unsigned int flags = 0, 
				     unsigned int portability = 0) :
        quasi_portable_binary_iarchive_impl<
      quasi_portable_binary_iarchive, std::istream::char_type, std::istream::traits_type
      >(is, flags, portability)
      {
	return;
      }

      quasi_portable_binary_iarchive(std::streambuf & bsb, 
				     unsigned int flags = 0, 
				     unsigned int portability = 0) :
        quasi_portable_binary_iarchive_impl<
	quasi_portable_binary_iarchive, std::istream::char_type, std::istream::traits_type
        >(bsb, flags, portability)
      {
	return;
      }

    };

  } // namespace archive
} // namespace boost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::quasi_portable_binary_iarchive)
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION(boost::archive::quasi_portable_binary_iarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_IARCHIVE_HPP

// end of boost::archive::quasi_portable_binary_iarchive.hpp
