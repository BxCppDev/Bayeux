// datatools/ui/utils.cc - Utilities
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <datatools/ui/utils.h>

// Standard libraries:
#include <iostream>

// Third Party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

// This project:
#include <datatools/exception.h>

namespace datatools {

  namespace ui {

    namespace path {

      const std::string & root_path()
      {
        static const std::string _rpath("/");
        return _rpath;
      }

      const std::string & home_interface_path()
      {
        static const std::string _hpath("~");
        return _hpath;
      }

      const std::string & current_interface_path()
      {
        static const std::string _cpath(".");
        return _cpath;
      }

      const std::string & current_parent_interface_path()
      {
        static const std::string _ppath("..");
        return _ppath;
      }

      char sep()
      {
        return '/';
      }

      char exec()
      {
        return '*';
      }

      std::string remove_trailing_sep(const std::string & path_)
      {
        if (path_.empty()) {
          return path_;
        }
        if (path_ == root_path()) {
          return path_;
        }
        if (path_[path_.length() - 1] == sep()) {
          return path_.substr(0, path_.length() - 1);
        }
        return path_;
      }

      std::string basename(const std::string & path_)
      {
        boost::filesystem::path p(path_);
        if (p.filename().string().empty()) {
          return root_path();
        }
        return p.filename().string();
      }

      std::string parent_path(const std::string & path_)
      {
        boost::filesystem::path p(path_);
        // std::cerr << "DEVEL: parent_path: p  = '" << p.string() << "'" << std::endl;
        // std::cerr << "DEVEL: parent_path: pp = '" << p.parent_path().string() << "'" << std::endl;
        if (p.parent_path().string().empty()) {
          return root_path();
        }
        return p.parent_path().string();
      }

      bool is_hidden(const std::string & name_)
      {
        if (name_.length() && name_[0] == '.') return true;
        return false;
      }

      bool is_valid_name(const std::string & name_)
      {
        if (name_.empty()) return false;
        if (name_.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.") != name_.npos) return false;
        if (name_.find_first_of("0123456789") == 0) return false;
        return true;
      }

      bool is_absolute_path(const std::string & path_)
      {
        if (boost::starts_with(path_, ::datatools::ui::path::root_path())) return true;
        return false;
      }

      bool is_valid_path(const std::string & path_)
      {
        if (path_.empty()) return true;
        if (path_ == ".") return true;
        if (path_ == "..") return true;
        if (path_ == "./") return true;
        if (path_ == "../") return true;
        if (path_ == "/") return true;
        if (path_ == "~") return true;
        if (path_ == "~/") return true;
        if (path_.find("//") != path_.npos) return false;
        return true;
      }

      bool build_leaf_full_path(const std::string & parent_path_,
                                const std::string & leaf_name_,
                                std::string & full_leaf_path_)
      {
        if (!is_valid_path(parent_path_)) return false;
        if (!is_valid_name(leaf_name_)) return false;
        if (parent_path_.empty()) {
          full_leaf_path_ = leaf_name_;
        } else {
          if (parent_path_[parent_path_.size() - 1] == sep()) {
            full_leaf_path_ = parent_path_ + leaf_name_;
          } else {
            full_leaf_path_ = parent_path_ + sep() + leaf_name_;
          }
        }
        return true;
      }

      bool is_child_of(const std::string & child_candidate_,
                       const std::string & parent_path_)
      {
        boost::filesystem::path cc_pathname(child_candidate_);
        std::string ccp_pathname = cc_pathname.parent_path().string();
        return parent_path_ == ccp_pathname;
      }

      std::string build_path(const std::list<std::string> & segments_)
      {
        std::ostringstream pout;
        pout << root_path();
        for (std::list<std::string>::const_iterator i = segments_.begin();
             i != segments_.end();
             i++) {
          const std::string & segment = *i;
          DT_THROW_IF(segment.empty(), std::logic_error, "Invalid empty path segment!");
          DT_THROW_IF(segment.find(sep()) != std::string::npos, std::logic_error, "Path segment contains a path separator!");
          if (i != segments_.begin()) {
            pout << sep();
          }
          // std::cerr << "DEVEL: datatools::ui::path::build_path: segment = '" << segment << "'" << std::endl;
          pout << segment;
        }
        return pout.str();
      }

    } // namespace path

  } // namespace ui

} // namespace datatools
