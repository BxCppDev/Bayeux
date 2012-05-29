// -*- mode: c++; -*-
// version_check.h

#ifndef __datatools__utils__version_check_h
#define __datatools__utils__version_check_h 1

#include <iostream>
#include <string>
#include <boost/cstdlib.hpp>

namespace datatools {
  namespace utils {

    class version_id;

    bool validate_version(const std::string& label_, 
			  const version_id& vid_,
			  const std::string& version_rules_);

  } // namespace utils
} // namespace datatools

#endif // __datatools__utils__version_check_h

// end of version_check.h
