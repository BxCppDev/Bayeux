// version.cc - Implementation of Bayeux/electronics version struct
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
//
// This file is part of Bayeux/electronics.
//
// Bayeux/electronics is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/electronics is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/electronics.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include "electronics/version.h"

// Standard Library
#include <sstream>

// Third Party
// - A

// This Project

namespace electronics {
int version::get_major() {
  return static_cast<int>(ELECTRONICS_VERSION_MAJOR);
}

int version::get_minor() {
  return static_cast<int>(ELECTRONICS_VERSION_MINOR);
}

int version::get_patch() {
  return static_cast<int>(ELECTRONICS_VERSION_PATCH);
}

std::string version::get_version() {
  static std::string version("");

  if (version.empty()) {
    std::ostringstream stream;
    stream << ELECTRONICS_VERSION_MAJOR << "."
           << ELECTRONICS_VERSION_MINOR << "."
           << ELECTRONICS_VERSION_PATCH;
    version = stream.str();
  }

  return version;
}

bool version::is_at_least(int major, int minor, int patch) {
  if (ELECTRONICS_VERSION_MAJOR < major) return false;
  if (ELECTRONICS_VERSION_MAJOR > major) return true;
  if (ELECTRONICS_VERSION_MINOR < minor) return false;
  if (ELECTRONICS_VERSION_MINOR > minor) return true;
  if (ELECTRONICS_VERSION_PATCH < patch) return false;
  return true;
}

bool version::has_feature(const std::string&) {
  /// - If you want to add features, then the following implementation
  ///   provides one example based on string features cached in a set.
  ///
  /// static std::set<std::string> features;
  ///
  /// if (features.empty())
  /// {
  ///   // cache the feature list
  ///   features.insert("FASTAPI");
  ///   features.insert("THREADSAFE");
  /// }
  ///
  /// return features.find(name) != features.end();

  return false;
}
} // namespace electronics
