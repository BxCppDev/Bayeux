// version.cc - Implementation of bayeux version struct
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2020 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2020 by Université de Caen
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include "bayeux/version.h"
#include "bayeux/bayeux_config.h"

// Standard Library:
#include <sstream>
#include <set>
#include <memory>

namespace bayeux {

  int version::get_major() {
    return static_cast<int>(BAYEUX_VERSION_MAJOR);
  }

  int version::get_minor() {
    return static_cast<int>(BAYEUX_VERSION_MINOR);
  }

  int version::get_patch() {
    return static_cast<int>(BAYEUX_VERSION_PATCH);
  }

  int version::get_revision() {
    return static_cast<int>(BAYEUX_VERSION_REVISION);
  }

  std::string version::get_version() {
    static std::string version("");

    if (version.empty()) {
      std::ostringstream stream;
      stream << BAYEUX_VERSION_MAJOR << "."
             << BAYEUX_VERSION_MINOR << "."
             << BAYEUX_VERSION_PATCH;
      version = stream.str();
    }

    return version;
  }

  bool version::is_at_least(int major_, int minor_, int patch_) {
    if (BAYEUX_VERSION_MAJOR < major_) return false;
    if (BAYEUX_VERSION_MAJOR > major_) return true;
    if (BAYEUX_VERSION_MINOR < minor_) return false;
    if (BAYEUX_VERSION_MINOR > minor_) return true;
    if (BAYEUX_VERSION_PATCH < patch_) return false;
    return true;
  }

  bool version::has_feature(const std::string & name_) {
    /// - If you want to add features, then the following implementation
    ///   provides one example based on string features cached in a set.
    ///
    static std::unique_ptr<std::set<std::string>> _features;
    
    if (! _features.get()) {
      _features.reset(new std::set<std::string>);
      // Cache the feature list
      if (BAYEUX_WITH_QT) _features->insert("with-qt");
      if (BAYEUX_WITH_BXDECAY0) _features->insert("with-bxdecay0");
      if (BAYEUX_WITH_GEANT4_MODULE) _features->insert("with-geant4-module");
    }
    
    return _features->find(name_) != _features->end();
  }

} // namespace bayeux
