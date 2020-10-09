/// \file datatools/configuration/variant_registry.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-09-22
 * Last modified : 2016-10-31
 *
 * Copyright (C) 2014-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_H
#define DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_H

// Standard library:
#include <string>
#include <map>
#include <vector>
#include <memory>

// This project (Bayeux/datatools):
#include <datatools/configuration/utils.h>
#include <datatools/enriched_base.h>
#include <datatools/configuration/variant_dependency_model.h>

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
      bool is_name_valid(const std::string & name_) const override;

      /// Default constructor
      variant_registry();

      /// Destructor
      ~variant_registry() override;

      /// Check if the top variant name is set
      bool has_top_variant_name() const;

      /// Return the name of the top variant used to initialize the registry
      const std::string & get_top_variant_name() const;

      /// Return a reference to the mutable dictionary of records
      record_dict_type & grab_records();

      /// Return a reference to the non-mutable dictionary of records
      const record_dict_type & get_records() const;

      /// Check if the registry is mounted in some parent repository
      bool is_mounted() const;

      /// Return the mounting name
      const std::string & get_mounting_name() const;

      /// Check the parent repository
      bool has_parent_repository() const;

      /// Set the parent repository
      void set_parent_repository(const variant_repository &, const std::string & mounting_name_);

      /// Reset the parent repository
      void reset_parent_repository();

      /// Return the handle to the parent repository
      const variant_repository & get_parent_repository() const;

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

      /// Check if a variant parameter exists (active or not)
      bool parameter_exists(const std::string & variant_path_) const;

      /// Check if a variant exists (active or not)
      bool variant_exists(const std::string & variant_path_) const;

      /// Check if a variant is active
      bool is_active_variant(const std::string & variant_path_) const;

      /// Return a reference to a non mutable variant record
      const variant_record & get_variant_record(const std::string & record_path_) const;

      /// Check if a parameter record exists
      bool has_parameter_record(const std::string & record_path_) const;

      /// Check if a parameter record group exists
      bool has_parameter_record_value_group(const std::string & param_group_path_) const;

      /// Return a reference to a mutable parameter record
      variant_record & grab_parameter_record(const std::string & record_path_);

      /// Return a reference to a non mutable parameter record
      const variant_record & get_parameter_record(const std::string & record_path_) const;

      /// Return a reference to the non mutable top variant record
      const variant_record & get_top_variant_record() const;

      /// Check if a local dependency model is set
      bool has_dependency_model() const;

      /// Return the local dependency model
      const variant_dependency_model & get_dependency_model() const;

      /// Load local dependency model
      void load_local_dependency_model(const datatools::properties & ldm_config_);

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & inden_ = "",
                             bool inherit_ = false) const override;

      /// \brief Flags for building the list of parameters
      enum list_flags {
        LIST_ACTIVE_ONLY   = datatools::bit_mask::bit00,
        LIST_NO_SET        = datatools::bit_mask::bit01,
        LIST_NO_UNSET      = datatools::bit_mask::bit02,
        LIST_CLEAR         = datatools::bit_mask::bit03,
        LIST_NO_PARAMETERS = datatools::bit_mask::bit04,
        LIST_NO_VARIANTS   = datatools::bit_mask::bit05,
        LIST_RANKED        = datatools::bit_mask::bit06
      };

      /// Compute the ranked list of parameters
      void list_of_ranked_parameters(std::vector<std::string> & paths_) const;

      /// Compute the ranked list of records
      void list_of_ranked_records(std::vector<std::string> & paths_,
                                  uint32_t flags_ = 0) const;

      /// Compute the list of parameters
      void list_of_parameters(std::vector<std::string> &_paths_,
                              uint32_t flags_ = 0) const;

      /// Compute the list of unset parameters
      void list_of_unset_parameters(std::vector<std::string> & unset_paths_) const;

      /// Compute the recursive list of ranked records
      /// For variant records, the explicitely ranked daughter params are listed first, then come
      /// unranked params in arbitrary (alphabetical) order.
      /// For parameter records, daughter variant records are listed in native order.
      void build_recursive_list_of_ranked_records(const variant_record & record_,
                                                  std::vector<std::string> & ranked_) const;

      /// Check if all parameters are set
      bool is_accomplished() const;

      /// Update record status
      void update();

    private:

      void _build_parameter_records_from_variant_(const variant_model & varmod_,
                                                  variant_record * parent_variant_record_);

      void _build_variant_records_from_parameter_(const parameter_model & varmod_,
                                                  variant_record * parent_param_record_);

    private:

      // Management:
      bool             _initialized_ = false; //!< Initialization flag

      // Configuration:
      std::string      _top_variant_name_;    //!< The name of the top variant used at initialization stage

      // Internal data:
      record_dict_type _records_;             //!< Dictionary of configuration variant records
      const variant_repository * _parent_repository_ = nullptr;     //!< Handle to the parent repository
      std::string                _mounting_name_;                   //!< Mounting name of the registry in its parent repository
      std::unique_ptr<variant_dependency_model> _dependency_model_; //!< Local model of dependencies

    };

  } // end of namespace configuration

} // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
