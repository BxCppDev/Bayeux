/// \file datatools/version_check.h

#ifndef DATATOOLS_VERSION_CHECK_H
#define DATATOOLS_VERSION_CHECK_H

// Standard Library:
#include <string>

namespace datatools {

  class version_id;

  bool validate_version(const std::string & label_,
                        const version_id & vid_,
                        const std::string & version_rules_);

} // namespace datatools

#endif // DATATOOLS_VERSION_CHECK_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
