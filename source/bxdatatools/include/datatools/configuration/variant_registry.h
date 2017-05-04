/// \file datatools/configuration/variant_registry.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-09-22
 * Last modified : 2014-09-22
 *
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A registry of configuration variants and associated parameters.
 *
 * History:
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_H
#define DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_H

// Standard library:
#include <string>
#include <map>
#include <vector>

// This project (Bayeux/datatools):
#include <datatools/configuration/utils.h>
#include <datatools/command_utils.h>
#include <datatools/enriched_base.h>

namespace datatools {

  namespace configuration {

    // Forward declaration:
    class variant_registry_manager;
    class variant_record;
    class variant_repository;

    /// \brief Registry of configuration variant parameters and associated variants
    class variant_registry : public datatools::enriched_base
    {
    public:

      /// Type of dictionary of configuration variant records
      typedef std::map<std::string, variant_record> record_dict_type;

      /// Return the default root variant name
      static const std::string & default_top_variant_name();

      /// Check if a name is valid
      virtual bool is_name_valid(const std::string & name_) const;

      /// Default constructor
      variant_registry();

      /// Destructor
      virtual ~variant_registry();

      /// Return the name of the top variant used to initialize the registry
      const std::string & get_top_variant_name() const;

      /// Return a reference to the mutable dictionary of records
      record_dict_type & grab_records();

      /// Return a reference to the non-mutable dictionary of records
      const record_dict_type & get_records() const;

      /// Check initialization
      bool is_initialized() const;

      /// Initialize
      void initialize_from(const variant_registry_manager & mgr_,
                           const std::string & top_variant_ = "",
                           const std::string & name_ = "",
                           const std::string & display_name_ = "",
                           const std::string & terse_description_ = "");

      /// Reset
      void reset();

      /// Add a new record
      variant_record & add_record(const std::string & record_path_);

      /// Check if a variant record exists
      bool has_variant_record(const std::string & record_path_) const;

      /// Return a reference to a mutable variant record
      variant_record & grab_variant_record(const std::string & record_path_);

      /// Return a reference to a non mutable variant record
      const variant_record & get_variant_record(const std::string & record_path_) const;

      /// Check if a parameter record exists
      bool has_parameter_record(const std::string & record_path_) const;

      /// Return a reference to a mutable parameter record
      variant_record & grab_parameter_record(const std::string & record_path_);

      /// Return a reference to a non mutable parameter record
      const variant_record & get_parameter_record(const std::string & record_path_) const;

      /// Return a reference to the non mutable top variant record
      const variant_record & get_top_variant_record() const;

      /// Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & inden_ = "",
                             bool inherit_ = false) const;

      /// Compute the list of unset parameters
      void list_of_unset_parameters(std::vector<std::string> & unset_paths_) const;

      /// Check if all parameters are set
      bool is_accomplished() const;

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

    protected:

      void _build_parameter_records_from_variant(const variant_model & varmod_,
                                                 variant_record * parent_variant_record_);

      void _build_variant_records_from_parameter(const parameter_model & varmod_,
                                                 variant_record * parent_param_record_);

    private:

      bool             _initialized_;      //!< Initialization flag
      std::string      _top_variant_name_; //!< The name of the top variant used at initialization stage
      record_dict_type _records_;          //!< Dictionary of configuration variant records

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
