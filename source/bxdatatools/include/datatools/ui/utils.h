//! \file  datatools/ui/utils.h
//! \brief Utilities
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

#ifndef DATATOOLS_UI_UTILS_H
#define DATATOOLS_UI_UTILS_H

// Standard libraries:
#include <string>
#include <list>

namespace datatools {

  namespace ui {

    namespace path {

      // Return the root path ("/")
      const std::string & root_path();

      // Return the home path ("~")
      const std::string & home_interface_path();

      // Return the current interface path (".")
      const std::string & current_interface_path();

      // Return the current parent interface path ("..")
      const std::string & current_parent_interface_path();

      // Return the path separator ('/')
      char sep();

      // Return the executable suffix ('*')
      char exec();

      //! Remove trailing \a '/' character for an interface path
      //!
      //! Examples:
      //! Path \a ""          returns \a ""
      //! Path \a "/"         returns \a "/"
      //! Path \a "foo"       returns \a "foo"
      //! Path \a "/foo/bar"  returns \a "/foo/bar"
      //! Path \a "/foo/bar/" returns \a "/foo/bar"
      //!
      std::string remove_trailing_sep(const std::string & path_);

      //! Return the basename of a command or interface path
      std::string basename(const std::string & path);

      //! Return the parent interface path of a command or interface path
      std::string parent_path(const std::string & path);

      //! Check if a name is hidden
      bool is_hidden(const std::string & name_);

      //! Check if a name is valid
      bool is_valid_name(const std::string & name_);

      //! Check if a path is valid
      bool is_valid_path(const std::string & path_);

      //! Check if a path is absolute
      bool is_absolute_path(const std::string & path_);

      //! Build the full path of a leaf with respect to its parent path
      bool build_leaf_full_path(const std::string & parent_path_,
                                const std::string & leaf_name_,
                                std::string & full_leaf_path_);

      //! Check if an absolute path if a child of another absolute path
      bool is_child_of(const std::string & child_candidate_, const std::string & parent_path_);

      //! Return the parent interface path of a command or interface path
      std::string build_path(const std::list<std::string> & segments_);

    } // namespace path

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
