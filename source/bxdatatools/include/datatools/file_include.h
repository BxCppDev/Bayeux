/// \file datatools/file_include.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2020-01-23
 * Last modified: 2020-01-23
 *
 * License: GPL3
 *
 * Copyright (C) 2020 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   A file inclusion helper.
 *
 */

#ifndef DATATOOLS_FILE_INCLUDE_H
#define DATATOOLS_FILE_INCLUDE_H

// Standard Library:
#include <list>
#include <string>
#include <tuple>

// This project:
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>

namespace datatools {

  /// \brief File inclusion helper
  /// 
  /// This object aims to resolve the path of any file to be resolved,
  /// typically from some kind of preprocessor used for configuration files with
  /// a file inclusion mechanism.
  class file_include
    : public i_tree_dumpable
  {
  public:

    /// \brief Strategy for the list of include paths set by an environment variable
    enum ev_strategy {
      EV_PREPEND = 1, ///< Environment variable has higher priority than the explicit list of include paths
      EV_APPEND = 2,  ///< Environment variable has lower priority than the explicit list of include paths
      EV_CLEAR = 3    ///< Environment variable discards the explicit list of include paths
    };

    /// Constructor
    file_include();

    datatools::logger::priority get_logging() const;

    void set_logging(const datatools::logger::priority);

    bool allows_current_directory() const;

    void set_allows_current_directory(bool);

    void add_explicit_include_path(const std::string & path_);

    void append_explicit_include_path(const std::string & path_);

    void prepend_explicit_include_path(const std::string & path_);
 
    void remove_explicit_include_path(const std::string & path_);

    bool has_explicit_include_path(const std::string & path_) const;

    void set_include_path_env_strategy(const ev_strategy);

    ev_strategy get_include_path_env_strategy() const;

    const std::string & get_include_path_env_name() const;

    void set_include_path_env_name(const std::string & ev_name_);

    void reset_include_path_env_name();

    bool has_include_path_env_name() const;

    bool has_include_path(const std::string & path_) const;

    std::string resolve(const std::string & to_be_included_path_) const noexcept(false);

    bool resolve(const std::string & to_be_included_path_,
                 std::string & resolved_path_) const noexcept;

    std::tuple<bool, std::string> resolve_err(const std::string & to_be_included_path_) const noexcept;

    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;

    bool is_initialized() const;
    
    void initialize();

    void reset();

  private:

    std::string _resolve_(const std::string & to_be_included_path_) const noexcept(false);

    void _compute_internals_();

    void _reset_internals_();
    
  private:

    // Management:
    datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;

    // Configuration:
    bool _allow_current_directory_ = false; ///< Flag to allow file inclusion from the current directory
    ev_strategy _include_path_env_strategy_ = EV_PREPEND;
    std::string _include_path_env_name_; ///< Name of the environment variable with contains an ordered list of directory paths from which file inclusion is allowed
    std::list<std::string> _explicit_include_paths_; ///< Explicit ordered list of directory paths from which file inclusion is allowed

    // Working data:
    bool _has_changed_ = false;
    std::list<std::string> _include_paths_; ///< Effective ordered list of directory paths from which file inclusion is allowed
    
  };
  
} // end of namespace datatools

#endif // DATATOOLS_FILE_INCLUDE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
