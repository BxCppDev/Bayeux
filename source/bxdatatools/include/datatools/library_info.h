/* library_info.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-09-16
 * Last modified: 2013-09-16
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

#ifndef DATATOOLS_LIBRARY_INFO_H_
#define DATATOOLS_LIBRARY_INFO_H_

// Standard library
#include <iostream>
#include <string>
#include <vector>

// Datatools
#include <datatools/logger.h>

namespace datatools {

  class multi_properties;
  class properties;

  /// \brief Host the register of library informations
  struct library_info {

    static const std::string LIB_LABEL;
    static const std::string EXEC_LABEL;
    static const std::string RESOURCE_LABEL;

    static const std::string NAME;
    static const std::string LIBNAME;
    static const std::string DESCRIPTION;
    static const std::string URL;
    static const std::string AUTHORS;
    static const std::string COPYRIGHT;
    static const std::string VERSION;
    static const std::string VERSION_MAJOR;
    static const std::string VERSION_MINOR;
    static const std::string VERSION_PATCH;
    static const std::string BUILD_TYPE;
    static const std::string MODULES;
    static const std::string INSTALL_PREFIX;
    static const std::string INSTALL_INCLUDE_DIR;
    static const std::string INSTALL_LIB_DIR;
    static const std::string INSTALL_BIN_DIR;
    static const std::string INSTALL_RESOURCE_DIR;
    static const std::string ENV_LIB_DIR;
    static const std::string ENV_RESOURCE_DIR;

    // Should one expose these 2 funcs ?
    /// Return a mutable reference to the library info register
    static multi_properties & grab_db();

    /// Return a non mutable reference to the library info register
    static const multi_properties & get_db();

    /// Return the logging priority of the library info register
    static logger::priority logging();

    /// Set the logging priority of the library info register
    static void logging(logger::priority);

    /// Build a list of currently registered library names
    static void names(std::vector<std::string> &);

    /// Print status of the library info register
    static void status(std::ostream & = std::clog);

    /// Register basic infos for a given library
    static properties & registration(const std::string & library_name_,
				     const std::string & library_desc_ = "",
				     const std::string & library_version_ = "",
				     const std::string & install_prefix_ = "",
				     const std::string & install_lib_path_ = "",
				     const std::string & install_resource_path_ = "");

    /// Unregister a given library
    static void unregistration(const std::string & library_name_);

    /// Check if some infos about a given library exist
    static bool has(const std::string & library_name_);

    static void update(const std::string & library_name_,
		       const std::string & info_key_,
		       const std::string & info_value_,
		       const std::string & info_type_ = "",
		       bool locked_ = true);

    /// Erase a library info from the register
    static void erase(const std::string & library_name_,
		      const std::string & info_key_);

    /// Print the library infos from the register
    static void print(const std::string & library_name_,
		      std::ostream & out_ = std::clog);

    /// Return a mutable reference to the library infos container
    static properties & grab(const std::string & library_name_);

    /// Return a non-mutable reference to the library infos container
    static const properties & get(const std::string & library_name_);

  };

} // end of namespace datatools

#endif // DATATOOLS_LIBRARY_INFO_H_
