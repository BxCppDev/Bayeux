// version.cc - Implementation of lahague version struct
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.
//

// Ourselves:
#include <lahague/version.h>

// Standard Library:
#include <sstream>

namespace lahague {

  int version::get_major()
  {
    return static_cast<int>(LAHAGUE_VERSION_MAJOR);
  }

  int version::get_minor()
  {
    return static_cast<int>(LAHAGUE_VERSION_MINOR);
  }

  int version::get_patch()
  {
    return static_cast<int>(LAHAGUE_VERSION_PATCH);
  }

  std::string version::get_version()
  {
    static std::string version("");

    if (version.empty()) {
      std::ostringstream stream;
      stream << LAHAGUE_VERSION_MAJOR << "."
             << LAHAGUE_VERSION_MINOR << "."
             << LAHAGUE_VERSION_PATCH;
      version = stream.str();
    }

    return version;
  }

  bool version::is_at_least(int major_, int minor_, int patch_)
  {
    if (LAHAGUE_VERSION_MAJOR < major_) return false;
    if (LAHAGUE_VERSION_MAJOR > major_) return true;
    if (LAHAGUE_VERSION_MINOR < minor_) return false;
    if (LAHAGUE_VERSION_MINOR > minor_) return true;
    if (LAHAGUE_VERSION_PATCH < patch_) return false;
    return true;
  }

  bool version::has_feature(const std::string &)
  {
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
} // namespace lahague
