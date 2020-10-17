/// \file datatools/library_info.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-09-16
 * Last modified: 2013-09-26
 *
 * License:
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *  Some tools to register static and/or dynamic informations about libraries.
 *
 */

#ifndef DATATOOLS_LIBRARY_INFO_H
#define DATATOOLS_LIBRARY_INFO_H

// Standard library:
#include <iostream>
#include <string>
#include <vector>
#include <set>

// This project:
#include <datatools/logger.h>
#include <datatools/multi_properties.h>
#include <datatools/i_tree_dump.h>

namespace datatools {

  class multi_properties;
  class properties;

  /// \brief Host a register of static and/or dynamic informations about libraries or software components
  class library_info
    : public i_tree_dumpable
  {

  public:
    /// \brief Library/software component description property keys
    struct keys {
      static const std::string & lib_label();
      static const std::string & exec_label();
      static const std::string & resource_label();
      static const std::string & name();
      static const std::string & libname();
      static const std::string & description();
      static const std::string & urn();
      static const std::string & url();
      static const std::string & authors();
      static const std::string & copyright();
      static const std::string & version();
      static const std::string & version_major();
      static const std::string & version_minor();
      static const std::string & version_patch();
      static const std::string & build_type();
      static const std::string & modules();
      static const std::string & install_prefix();
      static const std::string & install_include_dir();
      static const std::string & install_lib_dir();
      static const std::string & install_bin_dir();
      static const std::string & install_data_dir();
      static const std::string & install_resource_dir();
      static const std::string & install_plugin_lib_dir();
      static const std::string & install_doc_dir();
      static const std::string & env_prefix();
      static const std::string & env_include_dir();
      static const std::string & env_lib_dir();
      static const std::string & env_bin_dir();
      static const std::string & env_data_dir();
      static const std::string & env_resource_dir();
      static const std::string & env_plugin_lib_dir();
      static const std::string & env_doc_dir();
    };

    // Set of supported topic labels
    static const std::set<std::string> & topic_labels();

    // Default topic label ("resources");
    static std::string default_topic_label();

    // Convert
    static std::string topic_label_to_install_key(const std::string & label_);

    // Convert
    static std::string topic_label_to_env_key(const std::string & label_);

    /// Constructor
    library_info();

    /// Destructor
    ~library_info() override;

    /// Return the logging priority of the library info register
    logger::priority get_logging() const;

    /// Set the logging priority of the library info register
    void set_logging(logger::priority);

    /// Build a list of currently registered library names
    void names(std::vector<std::string> &) const;

    /// Build a list of currently defined library aliases
    void aliases(std::vector<std::string> &) const;

    /// Add alias
    void add_alias(const std::string & library_alias_,
                   const std::string & library_name_);

    /// Remove alias
    void remove_alias(const std::string & library_alias_);

    /// Parse of a path registration directive
    ///
    /// Format is :
    /// \code
    ///   "NAME[.TOPIC]@PATH/TO/REGISTERED/DIR"
    /// \endcode
    /// where:
    ///  - NAME : is the registration name (should not contains dots),
    ///  - TOPIC : is chosen among "resources" (default), "data", "libraries", "plugins"...,
    ///  - PATH/TO/REGISTERED/DIR : is the (absolute) path which should contain the topic files.
    /// Examples:
    /// \code
    ///   "foo@dir/subdir/subsubdir/..."
    ///   "foo.resource@dir/subdir/subsubdir/..."
    ///   "foo.libraries@dir/subdir/subsubdir/..."
    ///   "foo.binaries@dir/subdir/subsubdir/..."
    ///   "foo.plugins@dir/subdir/subsubdir/..."
    ///   "foo.docs@dir/subdir/subsubdir/..."
    ///   "foo.prefix@dir/subdir/subsubdir/..."
    ///   "foo.data@dir/subdir/subsubdir/..."
    /// \endcode
    static bool parse_path_registration_directive(const std::string & rule_,
                                                  std::string & library_name_,
                                                  std::string & topic_,
                                                  std::string & path_,
                                                  std::string & error_msg_);

    /// Resolve a path with the syntax:
    /// \code
    ///   "@foo[.TOPIC]:dir/subdir/subsubdir/..."
    /// \endcode
    /// with TOPIC chosen among "resources" (default), "data", "libraries", "plugins"...
    bool resolve_path(const std::string & path_,
                      std::string & resolved_path_,
                      std::string & error_msg_) const;

    /// Register a path associated to a library/app name
    void path_registration(const std::string & library_name_,
                           const std::string & topic_,
                           const std::string & path_,
                           bool overwrite_ = false);

    /// Unregister a path associated to a library/app name
    void path_unregistration(const std::string & library_name_,
                             const std::string & topic_);

    /// Register basic infos for a given library
    properties & registration(const std::string & library_name_,
                              const std::string & library_desc_ = "",
                              const std::string & library_version_ = "",
                              const std::string & install_prefix_ = "",
                              const std::string & install_lib_path_ = "",
                              const std::string & install_resource_path_ = "");

    /// Unregister a given library
    void unregistration(const std::string & library_name_);

    /// Check if some infos about a given library exist (by name or alias)
    bool has(const std::string & library_key_) const;

    /// Check if a given library exist (by name)
    bool is_library(const std::string & library_name_) const;

    /// Check if an alias is defined
    bool is_alias(const std::string & library_key_) const;

    /// Resolve the true name of the library
    std::string resolve(const std::string & library_key_,
                        bool check_ = false) const;

    /// Update
    void update(const std::string & library_name_,
                const std::string & info_key_,
                const std::string & info_value_,
                const std::string & info_type_ = "",
                bool locked_ = true);

    /// Erase a library info from the register
    void erase(const std::string & library_name_,
               const std::string & info_key_);

    /// Clear all library infos from the register
    void clear();

    /// Print the library infos from the register
    void print(const std::string & library_name_,
               std::ostream & out_ = std::clog) const;

    /// Initialize the library info register
    void initialize();

    /// Terminate the library info register
    void reset();

    /// Check the library info register initialization status
    bool is_initialized();

    /// Return a non-mutable reference to the library infos container
    const properties & get(const std::string & library_key_) const;

    /// Return a mutable reference to the library infos container
    properties & grab(const std::string & library_key_);

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                   const std::string & title_ = "",
                   const std::string & indent_ = "",
                   bool inherit_ = false) const override;

  private:

    // Management:
    bool                              _initialized_; //!< Initialization flag
    logger::priority                  _logging_;     //!< Logging priority

    // Working data:
    datatools::multi_properties       _db_;          //!< Main register
    std::map<std::string,std::string> _db_aliases_;  //!< Dictionary of aliases

  };

} // end of namespace datatools

#endif // DATATOOLS_LIBRARY_INFO_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
