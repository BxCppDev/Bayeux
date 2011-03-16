// boost::archive::quasi_portable_binary_archive_common.hpp

#ifndef BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_ARCHIVE_COMMON_HPP
#define BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_ARCHIVE_COMMON_HPP 1

#pragma once

// (C) Copyright 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>. 
//                    Christian Pfligersdorffer <christian.pfligersdorffer@eos.info>. 
//
// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <boost/archive/detail/decl.hpp>
#include <boost/archive/basic_archive.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/item_version_type.hpp>

namespace boost {
  namespace archive {

    class quasi_portable_binary_archive_common
    {
    public:
      
      static bool g_devel;

      static const int quasi_portability      = 0;	  
      static const int restricted_portability = 1;
      static const int strict_portability     = 2;
    };

    BOOST_ARCHIVE_DECL(const char *)
    BOOST_ARCHIVE_QPBA_SIGNATURE();

    BOOST_ARCHIVE_DECL(library_version_type)
    BOOST_ARCHIVE_QPBA_VERSION();

  } // namespace archive
} // namespace boost

#endif // BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_ARCHIVE_COMMON_HPP 

// end of boost::archive::quasi_portable_binary_archive_common.hpp


