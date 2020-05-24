// resource.cc - Implementation of Bayeux resource
//
// Copyright (c) 2017 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2017 by The University of Warwick
// Copyright (c) 2017 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen
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

// Ourselves
#include <bayeux/resource.h>

// Standard Library
#include <iostream>
#include <fstream>
#include <cstdlib>

// Third Party
// Boost
#include <boost/filesystem.hpp>
// - Bayeux
#include <datatools/exception.h>
#include <datatools/logger.h>

// This Project
#include <bayeux/bayeux_config.h>
// - Bayeux
#include <bayeux/reloc.h>

namespace {
  //! Return the path to the root resource directory
  std::string get_resource_root(bool overriden_env_)
  {
    if (overriden_env_) {
      const char * env_key = BAYEUX_ENV_RESOURCE_DIR;
      if (std::getenv(env_key)) {
        return std::string(std::getenv(env_key));
      }
    }
    static boost::filesystem::path install_resource_root;
    if (install_resource_root.empty()) {
      // std::cerr << "[devel] " << "[bayeux] get_resource_root: Bayeux resource dir = '" << bayeux::get_resource_dir() << "'\n";
      install_resource_root = bayeux::get_resource_dir();
      install_resource_root /= "bayeux";
    }
    return install_resource_root.string();
  }
} // namespace

namespace bayeux {

  std::string get_bayeux_resource_dir(bool overriden_env_)
  {
    return get_resource_root(overriden_env_);
  }

  std::string get_bayeux_resource(const std::string & rname_, bool overriden_env_)
  {
    DT_THROW_IF(rname_[0] == '/', std::logic_error, "invalid resource name");

    std::string fullpath(get_resource_root(overriden_env_) + "/" + rname_);
    std::ifstream check(fullpath.c_str());
    DT_THROW_IF(!check.good(),
                std::runtime_error,
                "Unreadable resource '" << fullpath << "'");
    return fullpath;
  }

} // namespace bayeux
