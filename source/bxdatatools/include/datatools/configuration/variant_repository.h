/// \file datatools/configuration/variant_repository.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-09-22
 * Last modified : 2016-11-04
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
 *   A repository for variant registries.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_REPOSITORY_H
#define DATATOOLS_CONFIGURATION_VARIANT_REPOSITORY_H

// Standard library:
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <memory>

// This project (Bayeux/datatools):
#include <datatools/configuration/utils.h>
#include <datatools/enriched_base.h>
#include <datatools/bit_mask.h>
#include <datatools/configuration/variant_dependency.h>
#include <datatools/configuration/variant_dependency_model.h>

namespace datatools {

  namespace configuration {

    class variant_registry;
    class variant_registry_manager;
    class variant_reporting;
    class variant_record;

    /// \brief Variant repository
    class variant_repository : public datatools::enriched_base
    {
    public:

      struct ui_config_type
      {
        bool writable_at_start = false; //!< Start UI in writable mode
        bool inhibit_secondary_choices = false; //!< Start UI inhibiting secondary choices
      };
      
      /// Extract the registry name and variant path from an
      /// activated variant directive.
      ///
      /// \param variant_full_path_ The input variant full path
      /// \param registry_name_     The output resolved registry name
      /// \param variant_path_      The output resolved variant path
      /// \param reversed_          Reversed activation flag
      ///
      /// Supported syntax:
      ///  - "myregistry:the/path/to/a/variant"
      ///  - "!myregistry:the/path/to/a/variant"
      /// A leading '!' means that the variant should not be active
      static bool parse_variant_path(const std::string & variant_full_path_,
                                     std::string & registry_name_,
                                     std::string & variant_path_,
                                     bool & reversed_);

      /// \brief Entry that hosts a variant registry
      struct registry_entry
      {
      public:

        /// Constructor
        registry_entry(variant_repository &);

        /// Destructor
        ~registry_entry();

        /// Set the registry name
        void set_name(const std::string &);

        /// Return the registry name
        const std::string & get_name() const;

        /// Return the handle to the parent repository
        const variant_repository & get_parent_repository() const;

        /// Return the handle to the registry
        const variant_registry & get_registry() const;

        /// Return the mutable handle to the registry
        variant_registry & grab_registry();

        /// Reset
        void reset();

        /// Check if the registry is external
        bool is_external() const;

        /// Check if the registry is embedded
        bool is_embedded() const;

        /// Check if the entry is valid
        bool is_valid() const;

        /// Set the external registry
        void set_external_registry(variant_registry &);

        /// Reset the external registry
        void reset_external_registry();

        /// Set the rank of the registry
        void set_rank(int);

        /// Get the rank of the registry
        int  get_rank() const;

        /// Check if rank is set
        bool has_rank() const;

        /// Check if registry is sealed (non mutable)
        bool is_sealed() const;

        /// Place seal on the registry
        void seal();

      private:

        std::string                         _name_; //!< Registry (mounting) name
        variant_repository *                _parent_repository_ = nullptr; //!< Parent repository
        int                                 _rank_ = -1;         //!< Rank of the registry
        std::shared_ptr<variant_registry_manager> _embedded_manager_; //!< Embedded registry factory/manager
        std::shared_ptr<variant_registry>   _embedded_registry_; //!< Embedded registry
        variant_registry                  * _external_registry_ = nullptr; //!< Handle to an external registry
        bool _last_active_ = false; //!< Last active flag
        bool _sealed_ = false; //!< Registry is sealed

        friend class variant_repository;

      };

      /// Type of dictionary of configuration variant registries
      typedef std::map<std::string, registry_entry> registry_dict_type;
      typedef std::map<int, std::string> ranked_dict_type;

      /// Check if a name is valid
      bool is_name_valid(const std::string & name_) const override;

      /// Default constructor
      variant_repository();

      /// Destructor
      ~variant_repository() override;

      /// Return the UI configuration
      const ui_config_type & get_ui_config() const;

      /// Return the UI configuration
      ui_config_type & grab_ui_config();
      
      /// Check the name of the organization
      bool has_organization() const;

      /// Return the name of the organization
      const std::string & get_organization() const;

      /// Set the name of the organization
      void set_organization(const std::string &);

      /// Check the name of the application
      bool has_application() const;

      /// Return the name of the application
      const std::string & get_application() const;

      /// Set the name of the application
      void set_application(const std::string &);

      /// Initialize from a properties container
      void initialize(const datatools::properties & config_);

      /// Load embedded registries from a properties container
      void load_registries(const datatools::properties & config_);

      /// Load global dependency model
      void load_global_dependency_model(const datatools::properties & gdm_config_);

      /// Basic initialize
      void initialize_simple();

      /// Reset
      void reset();

      /// Check the initialization flag
      bool is_initialized() const;

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                     const std::string & title_ = "",
                     const std::string & indent_ = "",
                     bool inherit_ = false) const override;

      /// Return the number of registries
      unsigned int get_number_of_registries() const;

      /// Build the list of registry keys
      void list_of_registry_keys(std::set<std::string> & keys_) const;

      /// Check if a registry exists
      bool has_registry(const std::string & registry_name_) const;

      /// Check if an existing registry is sealed
      bool is_sealed_registry(const std::string & registry_name_) const;

      /// Return a reference to a non mutable registry
      const variant_registry & get_registry(const std::string & registry_name_) const;

      /// Return a reference to a mutable registry
      variant_registry & grab_registry(const std::string & registry_name_);

      /// Return a reference to the non-mutable dictionary of registries
      const registry_dict_type & get_registries() const;

      /// Return a reference to the mutable dictionary of registries
      registry_dict_type & grab_registries();

      /// Check if a given rank is used
      bool rank_is_used(int rank_) const;

      /// Registration of an embedded registry
      void registration_embedded(const std::string & registry_manager_config_,
                                 const std::string & top_variant_name_ = "",
                                 const std::string & registry_name_ = "",
                                 const std::string & registry_display_name_ = "",
                                 const std::string & registry_terse_description_ = "",
                                 int registry_rank_ = -1,
                                 const datatools::logger::priority registry_logging_ = datatools::logger::PRIO_UNDEFINED);

      /// Registration of an external registry
      void registration_external(variant_registry & external_registry_,
                                 const std::string & registry_name_ = "",
                                 int rank_ = -1);

      /// Unregistration of some specific registry
      void unregistration(const std::string & registry_name_);

      /// Built the ordered list of registry keys
      void build_ordered_registry_keys(std::vector<std::string> & keys_) const;

      /// Check if a registry is ranked
      bool is_ranked(const std::string & registry_name_) const;

      /// Return the rank of a registry
      int get_rank(const std::string & registry_name_) const;

      /// Check if a variant registry is active
      bool is_active_registry(const std::string & registry_key_) const;

      /// Check if a variant is active
      bool is_active_variant(const std::string & registry_key_,
                             const std::string & variant_path_) const;

      /// Clear all registries
      void clear_registries();

      /// Unregistration of external registries only
      void external_registries_unregistration();

      /// Check if all embedded registries are accomplished
      bool is_accomplished() const;

      /// Set flag for requested lock after initialization
      void set_requested_lock(bool rl_);

      /// Check if lock is requested after initialization
      bool is_requested_lock() const;

      /// Check if the repository is locked
      bool is_locked() const;

      /// Lock the repository
      void lock();

      /// Unlock the repository
      void unlock();

      /// Check if a variant is known
      bool has_variant(const std::string & registry_key_,
                       const std::string & variant_path_) const;

      /// Check if a variant parameter is known
      bool has_variant_parameter(const std::string & registry_key_,
                                 const std::string & variant_parameter_path_) const;

      /// Check if a global dependency model is set
      bool has_dependency_model() const;

      /// Return the global dependency model
      const variant_dependency_model & get_dependency_model() const;

      /// \brief Restructured text formatting
      enum rst_flags {
        PRINT_RST_NO_TITLE = datatools::bit_mask::bit00
      };

      /// Print documentation
      void print_rst(std::ostream & out_, uint32_t flags_ = 0) const;

      /// Export to the system variant repository
      void system_export(uint32_t flags_ = 0);

      /// Discard from the system variant repository
      void system_discard() const;

      /// Check if a variant usage reporting is attached
      bool has_reporting() const;

      /// Attach a variant usage reporting
      void set_reporting(variant_reporting &);

      /// Detach a variant usage reporting (if any)
      void reset_reporting();

      /// Access to the variant usage reporting (if any)
      variant_reporting & grab_reporting();

      /// Update all registries and variant record status
      void update();

      /// \brief Import a variant repository from a properties container
      class importer
      {
      public:
        /// \brief Import flags
        enum init_flags {
          IMPORT_DEBUG   = datatools::bit_mask::bit00,
          IMPORT_NOCLEAR = datatools::bit_mask::bit01,
          IMPORT_DONT_IGNORE_UNKNOWN_REGISTRY = datatools::bit_mask::bit02,
          IMPORT_DONT_IGNORE_UNKNOWN_ORGAPP = datatools::bit_mask::bit03
        };
        /// Constructor
        importer(const datatools::properties & config_, uint32_t flags_ = 0);
        /// Import method
        void operator()(variant_repository & vrep_);
      protected:
        void _process_registry(variant_registry & vreg_);
        void _process_record(variant_record & vrec_);
      private:
        datatools::logger::priority   _logging_ = datatools::logger::PRIO_FATAL;
        bool                          _noclear_ = false;
        bool                          _dont_ignore_unknown_registries_ = false;
        bool                          _dont_ignore_unknown_orgapp_ = false;
        const datatools::properties & _config_;
        std::vector<std::string>      _settings_;
        std::string                   _current_registry_name_;
        std::string                   _current_setting_;
      };

      /// \brief Export a variant repository to a properties container
      class exporter
      {
      public:
        /// \brief Export flags
        enum init_flags {
          EXPORT_DEBUG    = datatools::bit_mask::bit00,
          EXPORT_NOCLEAR  = datatools::bit_mask::bit01,
          EXPORT_NOORGAPP = datatools::bit_mask::bit02
        };
        /// Constructor
        exporter(datatools::properties & config_, uint32_t flags_ = 0);
        void set_settings_key(const std::string &);
        /// Export method
        void process(const variant_repository & vrep_);
        const std::vector<std::string> & get_settings() const;
        void operator()(const variant_repository & vrep_);
      protected:
        void _process_registry(const variant_registry & vreg_);
        void _process_record(const variant_record & vrec_);
      private:
        datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL;
        bool                        _noclear_ = false;
        bool                        _noorgapp_ = false;
        datatools::properties &     _config_;   //!< Target properties
        std::string                 _settings_key_;
        // Working temporary data:
        std::vector<std::string>    _settings_;
        std::string                 _current_registry_name_;
      };

    protected:

      registry_entry & _add_entry(const std::string & registry_name_, int rank_ = -1);

      /// \deprecated Deprecated configuration mode for embedded variant registries
      void _legacy_load_registries(const datatools::properties & config_);

      void _at_init_();

      void _at_reset_();

      void _compute_ranked_unranked_();

    private:

      // Management:
      bool               _initialized_ = false;  //!< Initialization flag

      // Configuration:
      std::string        _organization_;         //!< The name of the organization
      std::string        _application_;          //!< The name of the application
      bool               _requested_lock_ = false; //!< Lock request
      ui_config_type     _ui_config_; //!< UI config
      
      // Working data:
      bool               _locked_ = false;       //!< Lock flag
      registry_dict_type _registries_;           //!< Dictionary of configuration variant registries
      ranked_dict_type   _ranked_;               //!< Dictionary of ranked configuration variant registries
      std::vector<std::string> _unranked_;       //!< List of unranked configuration variant registries
      std::unique_ptr<variant_dependency_model> _dependency_model_; //!< Global model of dependencies
      variant_reporting * _reporting_ = nullptr; //!< Handle to a reporting object

    };

  } // end of namespace configuration

} // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_REPOSITORY_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
