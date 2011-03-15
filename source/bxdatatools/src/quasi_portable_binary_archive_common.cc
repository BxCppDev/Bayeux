// boost::archive::quasi_portable_binary_archive_common.cpp

#include <boost/qpba/quasi_portable_binary_archive_common.hpp>

namespace boost {
  namespace archive {

    bool quasi_portable_binary_archive_common::g_devel = false;

    ///////////////////////////////////////////////////////////////////////
    // constants used in archive signature
    //This should never ever change. note that is not an std::string
    // string.

    BOOST_ARCHIVE_DECL(const char *) 
    BOOST_ARCHIVE_QPBA_SIGNATURE ()
    {
      return "qpba";
    }

    // this should change if the capabilities are added to the library
    // such that archives can be created which can't be read by previous
    // versions of this library
    // 1 - initial version
    //

    BOOST_ARCHIVE_DECL(library_version_type)
    BOOST_ARCHIVE_QPBA_VERSION ()
    {
      return library_version_type (1);
    }

  } // namespace archive
} // namespace boost

// end of boost::archive::quasi_portable_binary_archive_common.cpp
