/// \file datatools/configuration/ui/variant_registry_cli.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-11-07
 * Last modified : 2016-11-07
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *    A command line interface tool for variant registry.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_CLI_H
#define DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_CLI_H

// Standard library:
#include <string>

// This project (Bayeux/datatools):
#include <datatools/command_utils.h>
#include <datatools/logger.h>

namespace datatools {

  namespace configuration {

    class variant_registry;

    namespace ui {

      class variant_registry_cli
      {
      public:

        /// Constructor
        variant_registry_cli(datatools::configuration::variant_registry &);

        /// Return the logging priority threshold
        datatools::logger::priority get_logging() const;

        /// Set the logging priority threshold
        void set_logging(const datatools::logger::priority);

        /// Command to check if the value of a parameter is enabled
        command::returned_info
        cmd_is_parameter_value_enabled(const std::string & param_path_,
                                       const std::string & value_token_,
                                       bool & enabled_);

        /// Command to set the value of a parameter from a string
        command::returned_info
        cmd_set_parameter_value(const std::string & param_path_,
                                const std::string & value_token_);

        /// Command to get the value string of a parameter
        command::returned_info
        cmd_get_parameter_value(const std::string & param_path_,
                                std::string & value_token_) const;

        /// Check if a variant is active
        command::returned_info
        cmd_is_active_variant(const std::string & variant_path_,
                              bool & active_) const;

        /// Check if a variant exists
        command::returned_info
        cmd_has_variant(const std::string & variant_path_,
                        bool & existing_) const;

        /// Check if a parameter exists
        command::returned_info
        cmd_has_parameter(const std::string & parameter_path_,
                          bool & existing_) const;

      private:

        datatools::logger::priority _logging_; //!< Logging priority threshold
        datatools::configuration::variant_registry & _registry_; //!< Handle to the registry model

      };

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_UI_VARIANT_REGISTRY_CLI_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
