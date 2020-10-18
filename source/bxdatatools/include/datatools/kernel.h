/// \file datatools/kernel.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-09-26
 * Last modified: 2016-06-25
 *
 * License:
 *
 * Copyright (C) 2013-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *  The datatools kernel singleton with embedded resources and components.
 *
 */

#ifndef DATATOOLS_KERNEL_H
#define DATATOOLS_KERNEL_H

// Standard Library:
#include <iostream>
#include <memory>

// Third Party:
#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/program_options.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// This project:
#include <datatools/logger.h>
#include <datatools/i_tree_dump.h>
#include <datatools/datatools_config.h>
#include <datatools/bit_mask.h>
#include <datatools/datatools_init.h>

namespace datatools {

  // Forward declaration:
  class service_manager;
  class urn_query_service;
  class library_query_service;
  class library_info;

  namespace configuration {

    // Forward declaration:
    class variant_repository;
    class variant_service;

  }

  /// \brief The datatools kernel singleton with embedded resources and components
  class kernel
    : private boost::noncopyable
    , public ::datatools::i_tree_dumpable
  {

  public:

    /// \brief The set of configuration parameters for the datatools kernel
    struct param_type {

    public:

      /// Constructor
      param_type();

      /// Reset
      void reset();

      /// Print
      void print(std::ostream & out_,
                 const std::string & title_ = "",
                 const std::string & indent_ = "");

    public:

      bool                     help = false;            //!< Flag to print the help about datatools' command line options
      std::string              logging_label = "fatal";   //!< Logging priority label
      std::string              locale_category; //!< The locale category
      std::string              library_query_logging_label = "fatal";  //!< Logging priority label for the embedded library information registry
      bool                     inhibit_library_query = false; //!< Flag to inhibit the use of the embedded library information registry
      bool                     inhibit_urn_query = false;    //!< Flag to inhibit the use of the URN query service
      bool                     inhibit_variant_repository = false;    //!< Flag to inhibit the use of the variant repository
      std::vector<std::string> unrecognized_args; //!< List of unrecognized arguments
      std::vector<std::string> resource_paths;    //!< List of resource paths directives
      std::string              variant_config;    //!< The configuration file for the system variant repository
      std::vector<std::string> variant_registry_configs; //!< List of configuration files for the variant registries to be embedded in the system variant repository
      std::string              variant_load;   //!< Path to the file from which one loads the values of the variant parameters
      std::string              variant_store;  //!< Path to the file where one stores the values of the variant parameters
      std::vector<std::string> variant_sets;   //!< List of directives to set the values of the variant parameters

      bool                     splash = false;         //!< Flag to display the datatools splash screen
#if DATATOOLS_WITH_QT_GUI == 1
      bool                     inhibit_qt_gui = false; //!< Flag to inhibit GUI
      bool                     variant_qt_gui = false; //!< Flag to run the variant GUI
      std::string              variant_qt_gui_style = "plastique"; //!< Qt style
#endif // DATATOOLS_WITH_QT_GUI == 1

    };

    /// \brief Bit mask for the kernel initialization flags
    static const uint32_t init_mask = datatools::bit_mask::nbits15;

    /// \brief Kernel initialization flags (15 bits)
    enum init_flags
      {
       // Import value from datatools_init_flags:
       init_no_help             = ::datatools::init_no_help,
       init_no_splash           = ::datatools::init_no_splash,
       init_no_locale_category  = ::datatools::init_no_locale_category,
       init_no_logging          = ::datatools::init_no_logging,
       init_no_inhibit_libquery = ::datatools::init_no_inhibit_libquery,
       init_no_libquery_logging = ::datatools::init_no_libquery_logging,
       init_no_inhibit_libinfo  = ::datatools::init_no_inhibit_libinfo, //!< Deprecated
       init_no_libinfo_logging  = ::datatools::init_no_libinfo_logging, //!< Deprecated
       init_no_resource_path    = ::datatools::init_no_resource_path,
       init_no_inhibit_variant  = ::datatools::init_no_inhibit_variant,
       init_no_variant          = ::datatools::init_no_variant,
       init_no_inhibit_qt_gui   = ::datatools::init_no_inhibit_qt_gui,
       init_no_inhibit_urnquery = ::datatools::init_no_inhibit_urnquery,
       init_reserved_11         = ::datatools::init_reserved_11,
       init_reserved_12         = ::datatools::init_reserved_12,
       init_reserved_13         = ::datatools::init_reserved_13,
       init_reserved_14         = ::datatools::init_reserved_14,
      };
    
    /// Default constructor
    kernel();

    /// Constructor with on-the-fly initialization
    kernel(int argc_, char * argv_[], uint32_t flags = 0);

    /// Destruction with on-the-fly shutdown if needed
    ~kernel() override;

    /// Initialization from command line arguments
    void initialize(int argc_, char * argv_[], uint32_t flags_ = 0);

    /// Shutdown
    void shutdown();

    /// Check initialization status
    bool is_initialized() const;

    /// Return the logging priority threshold
    datatools::logger::priority get_logging() const;

    /// Set the logging priority threshold
    void set_logging(datatools::logger::priority);

    /// Check if the system services are available
    bool has_services() const;

    /// Check if the system URN query service is available
    bool has_urn_query() const;

    /// Return a mutable reference to the system URN query service
    urn_query_service & grab_urn_query();

    /// Return a non mutable reference to the system URN query service
    const urn_query_service & get_urn_query() const;

    /// Check if the system library query service is available
    bool has_library_query() const;

    /// Return a mutable reference to the system library query service
    library_query_service & grab_library_query();

    /// Return a non mutable reference to the system library query service
    const library_query_service & get_library_query() const;

    /// Check if the library/component info register is available
    bool has_library_info_register() const;

    /// Return a mutable reference to the library/component info register
    library_info & grab_library_info_register();

    /// Return a non mutable reference to the library/component info register
    const library_info & get_library_info_register() const;

    /// Check if the configuration variant repository is available
    bool has_variant_repository() const;

    /// Return a non mutable reference to the configuration variant repository
    const configuration::variant_repository & get_variant_repository() const;

    /// Return a mutable reference to the configuration variant repository
    configuration::variant_repository & grab_variant_repository();

    /// Check if a configuration variant service is available
    bool has_variant_service() const;

    /// Return a non mutable reference to the configuration variant service
    const configuration::variant_service & get_variant_service() const;

    /// Return a mutable reference to the configuration variant service
    configuration::variant_service & grab_variant_service();

    /// Set a variant service
    void set_variant_service(configuration::variant_service & var_serv_);

    /// Reset variant service
    void reset_variant_service();

    /// Check if the effective configuration variant repository is available
    bool has_effective_variant_repository() const;

    /// Return a non mutable reference to the effective configuration variant repository
    const configuration::variant_repository & get_effective_variant_repository() const;

    /// Return a mutable reference to the effective configuration variant repository
    configuration::variant_repository & grab_effective_variant_repository();

    /// Check if a specific locale_category is defined
    bool has_locale_category() const;

    /// Set the locale category
    void set_locale_category(const std::string & lc_);

    /// Return the locale category
    const std::string & get_locale_category() const;

    /// Return the name of the current application
    const std::string & get_application_name() const;

    /// Check if the name of the current application is defined
    bool has_application_name() const;

    /// Check if the kernel singleton is available
    static bool is_instantiated();

    /// Return a mutable reference to the kernel singleton
    static kernel & instance();

    /// Instantiate and return a mutable reference to the kernel singleton
    static kernel & instantiate();

    /// Return a non mutable reference to the kernel singleton
    static const kernel & const_instance();

    /// Build description of options
    static void build_opt_desc(boost::program_options::options_description &, param_type &, uint32_t flags_ = 0);

    /// Print splash screen
    static void print_splash(std::ostream & = std::clog);

    /// Print description of options
    static void print_opt_desc(const boost::program_options::options_description &,
                               std::ostream & = std::clog,
                               const std::string & indent_ = "");

#if DATATOOLS_WITH_QT_GUI == 1
    /// Check if the Qt GUI is activated
    bool is_qt_gui_activated() const;
#endif // DATATOOLS_WITH_QT_GUI == 1

    /// Return the command line argument count
    unsigned int get_nargs() const;

    /// Return the list of command line arguments
    const std::vector<std::string> & get_args() const;

    /// Return the command line argument count
    int get_argc() const;

    /// Return the command line arguments
    char ** get_argv() const;

    /// Return the command line argument by index
    char * get_argv(int i_) const;

    /// Check if the library info register is activated
    /// \deprecated
    bool is_library_info_register_activated() const;

    /// Check if the library query service is activated
    bool is_library_query_service_activated() const;

    /// Check if the URN query service is activated
    bool is_urn_query_service_activated() const;

    /// Check if the variant repository is activated
    bool is_variant_repository_activated() const;

    /// Check if the variant service is allowed
    bool is_variant_service_allowed() const;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                   const std::string & title_ = "",
                   const std::string & indent_ = "",
                   bool inherit_ = false) const override;

    /// Registration of resource paths
    void register_resource_paths();

    /// Registration of configuration variant registries
    void register_configuration_variant_registries();

    /// Import some (or all) configuration variant registry from a repository
    void import_configuration_registry(configuration::variant_repository & rep_,
                                       const std::string & registry_name_ = "");

    /// Check if the kernel has configuration variant registries
    bool has_configuration_registries() const;

    /// Check for an external registry given its name
    bool has_external_configuration_registry(const std::string & registry_name_) const;

    /// Remove a imported registry given its name
    void clear_configuration_registry(const std::string & registry_name_);

    /// Import a configuration variant repository
    void import_configuration_repository(configuration::variant_repository & rep_);

    /// Clear the configuration variant repository
    void clear_configuration_repository();

    /// Clear all imported registries from the configuration variant repository
    void clear_imported_configuration_repository();

  private:

    /// Initialization
    void _initialize_();

    /// Shutdown
    void _shutdown_();

    /// Set default attributes
    void _set_defaults_();

    /// Basic construction
    void _construct_();

    /// Initialization of system services
    void _initialize_services_();

    /// Shutdown services
    void _shutdown_services_();

    /// Initialization of library query service
    void _initialize_library_query_service_();

    /// Shutdown library query service
    void _shutdown_library_query_service_();

    /// Initialization of URN query service
    void _initialize_urn_query_service_();

    /// Shutdown URN query service
    void _shutdown_urn_query_service_();

    /// Initialization of configuration variants
    void _initialize_configuration_variant_repository_();

  private:

    // Management:
    bool             _initialized_ = false; //!< Initialization flag
    param_type       _params_;           //!< Setup parameters
    logger::priority _logging_;          //!< Logging priority

    // Configuration parameters:
    std::string      _locale_category_;  //!< Locale category
    std::string      _application_name_; //!< The name of the current application
    unsigned int     _nargs_ = 0;
    std::vector<std::string> _args_; //!< List of arguments
    int              _argc_ = 0;             //!< Command line argument count
    char **          _argv_ = nullptr;       //!< Command line arguments
    std::vector<std::string> _unrecognized_args_; //!< List of unrecognized arguments
    bool             _activate_library_query_service_ = true; //!< Activation flag for Library/component query service
    bool             _activate_urn_query_service_ = true;     //!< Activation flag for URN query service
    bool             _activate_variant_repository_ = true;    //!< Activation flag for configuration variant repository
    bool             _allowed_variant_service_ = true;        //!< Allow flag for configuration variant service
#if DATATOOLS_WITH_QT_GUI == 1
    bool            _activate_qt_gui_ = true; //!< Flag to activate the Qt GUI
#endif // DATATOOLS_WITH_QT_GUI == 1

    // Working internals:
    std::unique_ptr<datatools::service_manager> _services_; //!< System services manager
    std::unique_ptr<configuration::variant_repository> _variant_repository_; //!< Variant repository (obsolete?)
    configuration::variant_service * _variant_service_ = nullptr; //!< handle to an external variant service
#if DATATOOLS_WITH_QT_GUI == 1
    //    QApplication  * _qapp_; //!< Handle to the Qt application
#endif // DATATOOLS_WITH_QT_GUI == 1

    // Singleton:
    static kernel * _instance_; //!< Kernel singleton handle

  };

} // end of namespace datatools

#endif // DATATOOLS_KERNEL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
