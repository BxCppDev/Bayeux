// boost::archive::quasi_portable_binary_archive_common.hpp

#ifndef BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_ARCHIVE_COMMON_HPP
#define BOOST_ARCHIVE_QUASI_PORTABLE_BINARY_ARCHIVE_COMMON_HPP 1

#pragma once

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


