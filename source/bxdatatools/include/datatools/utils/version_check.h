// -*- mode: c++; -*-
// version_check.h
#ifndef DATATOOLS_UTILS_VERSION_CHECK_H_
#define DATATOOLS_UTILS_VERSION_CHECK_H_
// Standard Library
#include <iostream>
#include <string>

// Third Party
// - Boost
#include <boost/cstdlib.hpp>

// This Project

namespace datatools {
namespace utils {

class version_id;

bool validate_version(const std::string& label, const version_id& vid,
                      const std::string& version_rules);

} // namespace utils
} // namespace datatools

#endif // DATATOOLS_UTILS_VERSION_CHECK_H_

