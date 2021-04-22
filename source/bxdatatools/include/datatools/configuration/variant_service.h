//! \file    datatools/configuration/variant_service.h
//! \brief   Variant support
//! \details
//
// Copyright (c) 2016 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Université de Caen
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

#ifndef DATATOOLS_CONFIGURATION_VARIANT_SERVICE_H
#define DATATOOLS_CONFIGURATION_VARIANT_SERVICE_H

// Standard Library:
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

// Third party:
// - Boost
#include <boost/cstdint.hpp>
#include <boost/program_options.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/io.h>

namespace datatools {

  namespace configuration {

    //! \brief Variant user interface service
    class variant_service
    {
    public:

      //! Exception for variant related error
      class variant_exception : public std::runtime_error {
      public:
        variant_exception(const std::string& msg) : std::runtime_error(msg) {}
      };

      /// \brief Special flags to publish variant related program options
      enum config_flags {
        NO_LABEL                  = datatools::bit_mask::bit00,
        NO_LOGGING                = datatools::bit_mask::bit01,
        NO_CONFIG_FILENAME        = datatools::bit_mask::bit02,
        NO_REGISTRY_RULES         = datatools::bit_mask::bit03,
        NO_REGISTRY_DEPENDENCIES  = datatools::bit_mask::bit04,
        NO_PROFILE_LOAD           = datatools::bit_mask::bit05,
        PROFILE_LOAD_DONT_IGNORE_UNKNOWN = datatools::bit_mask::bit06,
        NO_SETTINGS               = datatools::bit_mask::bit07,
        NO_PROFILE_STORE          = datatools::bit_mask::bit08,
        NO_GUI                    = datatools::bit_mask::bit09,
        NO_TUI                    = datatools::bit_mask::bit10,
        NO_REPORTING              = datatools::bit_mask::bit11,
        NO_UI_WRITABLE_AT_START   = datatools::bit_mask::bit12,
        NO_UI_SECONDARY_CHOICES   = datatools::bit_mask::bit13
      };

      /// \brief Variant service configuration parameters
      ///
      /// Used by Boost/program_options support.
      struct config
      {
        /// Default constructor
        config();

        /// Check if the variant is activated
        bool is_active() const;

        /// Smart print
        void print(std::ostream & out_,
                   const std::string & title_ = "",
                   const std::string & indent_ = "") const;

        // Configuration parameters:
        std::string                     label;           //!< Label
        std::string                     logging;         //!< Variant logging
        std::string                     config_filename; //!< Variant configuration file
        std::vector<std::string>        registry_rules;  //!< Variant registry definitions
        std::vector<std::string>        registry_dependencies; //!< Variant registry dependencies
        std::string                     profile_load;    //!< Variant profile load
        bool                            profile_load_dont_ignore_unknown = false; //!< Flag to ignore unknown registry at profile load
        std::vector<std::string>        settings;        //!< List of variant settings
        std::string                     profile_store;   //!< Variant profile store
#if DATATOOLS_WITH_QT_GUI == 1
        bool                            gui = false;        //!< Launch the variant GUI editor
#endif // DATATOOLS_WITH_QT_GUI == 1
        bool                            tui = false;        //!< Launch the variant TUI editor
        bool                            ui_writable_at_start = false; //!< Launch the variant repository and registries in writable mode
        bool                            ui_inhibit_secondary_choices = false; //!< Launch the variant registries without proposing secondary choices
        std::string                     reporting_filename; //!< Variant usage reporting file
      };

      /// Initialize options description associated to a config structure
      static void init_options(boost::program_options::options_description & opts_,
                               config & cfg_,
                               uint32_t flags_ = 0);

      /// \brief Record for an additional registry
      struct registry_record
      {
        /// Default constructor
        registry_record();

        /// Apply a registry rule
        int init_registry(int & max_rank_, const std::string & registry_rule_);

        /// Apply a registry dependency rule
        int add_dependency(const std::string & registry_dependency_rule_);

        /// Check the validity of the record
        bool is_valid() const;

        /// Return the name of the registry
        const std::string & get_name() const;

        /// Return the display name of the registry
        const std::string & get_display_name() const;

        /// Return the description of the registry
        const std::string & get_description() const;

        /// Return the rank of the registry
        int get_rank() const;

        /// Return the config file of the registry
        const std::string & get_config_file() const;

        /// Return the list of dependencies of the registry
        const std::vector<std::string> & get_dependencies() const;

        // Attributes:
        std::string _name_;         ///< Name of the variant registry
        std::string _display_name_; ///< Display name of the variant registry
        std::string _description_;  ///< Description of the variant registry
        int         _rank_;         ///< Rank of the variant registry
        std::string _config_file_;  ///< Configuration file
        std::vector<std::string> _dependencies_; ///< Dependencies

      };

      /// Type alias for a dictionary of registry records
      typedef std::map<std::string, registry_record> registry_record_dict_type;

      // Default Constructor
      variant_service(uint32_t flags_ = 0);

      // Destructor
      ~variant_service();

      //! Set the label
      void set_label(const std::string &);

      //! Return the label
      const std::string & get_label() const;

      //! Set the logging priority thresold
      void set_logging(datatools::logger::priority);

      //! Set the main configuration file for the variant repository
      void set_repository_config(const std::string & config_);

      //! Add the configuration of a variant registry in the variant repository
      void add_registry_rule(const std::string & registry_rule_);

      //! Add a dependency to a variant registry in the variant repository
      void add_registry_dependency(const std::string & registry_dependency_rule_);

      //! Check if an additional variant registry is defined
      bool has_registry_record(const std::string & registry_name_) const;

      //! Set the optional input variant profile filename
      void set_input_profile(const std::string & ip_);

      //! Set the ignore unknown at load flag
      void set_dont_ignore_unknown_at_load(bool);

      //! Set the setting rules for variant parameters
      void set_settings(const std::vector<std::string> & setting_);

      //! Set the optional output variant profile filename
      void set_output_profile(const std::string & op_);

      //! Check is reporting is used
      bool is_reporting() const;

      //! Check for a usage reporting filename
      bool has_reporting_file() const;

      //! Set the usage reporting filename
      void set_reporting_file(const std::string & urf_);

      //! Configure the service with a set of parameters
      void configure(const config & cfg_, bool lock_ = true);

      //! Return the max rank for registries
      int32_t get_max_rank() const;

      //! Return the next available rank for registries
      int32_t get_next_rank() const;

      bool is_ui_writable_at_start() const;

      void set_ui_writable_at_start(bool);
      
      bool is_ui_inhibit_secondary_choices() const;

      void set_ui_inhibit_secondary_choices(bool);
      
      // //! Return the first free rank for registries
      // int32_t get_free_rank() const;

#if DATATOOLS_WITH_QT_GUI == 1
      //! Activate the variant GUI editor
      void set_gui(bool);
#endif // DATATOOLS_WITH_QT_GUI == 1

      //! Activate the variant TUI editor
      void set_tui(bool);

      //! Check started flag
      bool is_started() const;

      //! Run startup operations at application startup
      void start();

      //! Run termination at application stop
      void stop();

      //! Apply a setting
      //! @arg setting_ avariant parameter setting directive
      //! Supported format is : "foo:param0/var0/key0=value"
      void apply_setting(const std::string & setting_);

      //!< Return a handle to the variant repository
      variant_repository & grab_repository();

      //!< Return a handle to the variant repository
      const variant_repository & get_repository() const;

    private:

      // Private methods wrapped in start/stop actions:

      //----------------------------------------------------------------------
      //! Build the variant repository:
      void _do_build_();

      //----------------------------------------------------------------------
      //! Initialize the variant repository from a configuration file:
      void _do_variant_config_();

      //----------------------------------------------------------------------
      //! Add the configuration of a variant registry in the variant repository
      registry_record & _add_registry_record(const registry_record & registry_record_);

      //----------------------------------------------------------------------
      //! Initialize additional variant registries:
      void _do_variant_additional_registries_setup_();

      //----------------------------------------------------------------------
      //! Load variant parameters from a variant profile input file:
      void _do_variant_load_profile_();

      //----------------------------------------------------------------------
      //! Store variant parameters in a variant profile output file:
      void _do_variant_store_profile_();

      //----------------------------------------------------------------------
      //! Apply variant settings:
      void _do_variant_apply_settings_();

#if DATATOOLS_WITH_QT_GUI == 1
      //----------------------------------------------------------------------
      //! Launch GUI variant editor:
      void _do_variant_launch_gui_();
#endif // DATATOOLS_WITH_QT_GUI == 1

      //----------------------------------------------------------------------
      //! Launch TUI variant editor:
      void _do_variant_launch_tui_();

      //----------------------------------------------------------------------
      //! Export the variant repository in the Bayeux/datatools kernel.
      //! This makes all variant registries embedded in the application variant
      //! repository accessible from the system variant repository (singleton)
      //! of the Bayeux/datatools' kernel.
      //! The variant service is thus accessible from any component instantiated
      //! in the application framework.
      void _do_variant_system_export_();

      //----------------------------------------------------------------------
      //! Discard the variant repository from the system variant repository
      //! in the Bayeux/datatools' kernel.
      void _do_variant_system_discard_();

      //----------------------------------------------------------------------
      //! Initialize the variant usage reporting mechanism
      void _do_variant_reporting_init_();

      //----------------------------------------------------------------------
      //! Terminate the variant usage reporting mechanism
      void _do_variant_reporting_fini_();

    private:

      // Configuration:
      datatools::logger::priority _logging_;           //!< Logging priority
      std::string                 _label_;             //!< Label
      std::string                 _repository_config_; //!< Main variant configuration file (mandatory)
      std::vector<std::string>    _registry_rules_;    //!< Rules to load additional registries
      std::vector<std::string>    _registry_deps_;     //!< Dependency rules associated to additional registries
      std::string                 _input_profile_;     //!< Input variant profile (optional)
      std::vector<std::string>    _settings_;          //!< Collection of variant parameters setting rules (optional)
      std::string                 _output_profile_;    //!< Output variant profile (optional)
      bool                        _dont_ignore_unknown_at_load_ = false; //! Don't ignore unknown registries at load
 #if DATATOOLS_WITH_QT_GUI == 1
      bool                        _gui_ = false;       //!< Qt GUI activation (optional)
#endif // DATATOOLS_WITH_QT_GUI == 1
      bool                        _tui_ = false;       //!< TUI activation (optional, not implemented yet)
      std::string                 _reporting_file_;    //!< Reporting filename (optional)
      bool                        _ui_writable_at_start_ = false; //!< Directive for starting UI in write mode
      bool                        _ui_inhibit_secondary_choices_ = false; //<! Directive for starting UI inhibiting the display of secondary options (if any)

      // Embedded working data:
      bool                      _started_;    //!< Start flag
      registry_record_dict_type _registries_; //!< Registry records
      variant_repository        _repository_; //!< Application variant repository
      variant_reporting         _reporting_;  //!< Processing reporting

    };

  } // namespace configuration

} // namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
