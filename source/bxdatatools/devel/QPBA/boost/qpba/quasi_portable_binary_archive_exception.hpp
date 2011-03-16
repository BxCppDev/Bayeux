// boost::archive::quasi_portable_binary_archive_exception.hpp

#ifndef BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_ARCHIVE_EXCEPTION_HPP
#define BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_ARCHIVE_EXCEPTION_HPP 1

#pragma once

// (C) Copyright 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>. 
//                    Christian Pfligersdorffer <christian.pfligersdorffer@eos.info>. 
//
// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/version.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/archive/basic_archive.hpp>
#include <boost/archive/archive_exception.hpp>

#if BOOST_VERSION < 104400
#error "Quasi portable binary archives need Boost version >=1.44.0"
#endif

namespace boost {
  namespace archive {

    // version of the linked boost archive library
    const boost::archive::version_type 
    archive_version(boost::archive::BOOST_ARCHIVE_VERSION());

    /**
     * \brief Exception being thrown when serialization cannot proceed.
     *
     * There are several situations in which the portable archives must fail and
     * hence throw an exception: 
     * -# deserialization of an integer value that exceeds the range of the type 
     * -# serialization of special floating point values nan or inf 
     * -# deserialization of a denormalized value without the floating point type
     *    supporting denormalized numbers
     *
     * Note that this exception will also be thrown if you mixed up your stream
     * position and accidentially interpret some value for size data (in this case
     * invalid_size will be totally amiss most of the time).
     *
     */
    class quasi_portable_binary_archive_exception : public boost::archive::archive_exception
    {
      std::string m_msg;

    public:

      //! type size is not large enough for deserialized number
      quasi_portable_binary_archive_exception (signed char invalid_size) 
	: boost::archive::archive_exception (other_exception) 
	, m_msg ("requested integer size exceeds type size: ")
      {
	m_msg += boost::lexical_cast<std::string, int> (invalid_size);
	return;
      }

      //! negative number in unsigned type
      quasi_portable_binary_archive_exception()
	: boost::archive::archive_exception (other_exception)
	, m_msg ("cannot read a negative number into an unsigned type")
      {
	return;
      }

      //! serialization of inf, nan and denormals
      template <typename T> 
      quasi_portable_binary_archive_exception (const T& abnormal) 
	: boost::archive::archive_exception (other_exception) 
	, m_msg ("serialization of non-portable floating point value: ")
      {
	m_msg += boost::lexical_cast<std::string> (abnormal);
	return;
      }

      //! override the base class function with our message
      const char * what () const throw () 
      { 
	return m_msg.c_str(); 
      }

      ~quasi_portable_binary_archive_exception () throw () 
      {
	return;
      }

    };

  } // namespace archive
} // namespace boost

#endif // BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_ARCHIVE_EXCEPTION_HPP 

// end of boost::archive::quasi_portable_binary_archive_exception.hpp
