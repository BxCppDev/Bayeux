/// \file datatools/version_check.h

#ifndef DATATOOLS_VERSION_CHECK_H
#define DATATOOLS_VERSION_CHECK_H

// Standard Library:
#include <iostream>
#include <string>

// Third Party:
// - Boost:
#include <boost/cstdlib.hpp>

namespace datatools {

  class version_id;

  bool validate_version(const std::string& label, const version_id& vid,
                        const std::string& version_rules);

} // namespace datatools

#endif // DATATOOLS_VERSION_CHECK_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
