/// \file dpp/base_module.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-12-13
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Base data processing module.
 *
 * History:
 *
 */

#ifndef DPP_BASE_MODULE_H
#define DPP_BASE_MODULE_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/bit_mask.h>
#include <datatools/i_tree_dump.h>
#include <datatools/factory_macros.h>
#include <datatools/logger.h>
#include <datatools/things.h>

// This project:
#include <dpp/dpp_config.h>
#include <dpp/module_tools.h>

namespace datatools {
  // Forward class declarations :
  class properties;
  class multi_properties;
  class service_manager;
}

/// Top-level namespace of the Bayeux/dpp module library
namespace dpp {

  /// \brief Base processing module (abstract interface)
  class base_module : public datatools::i_tree_dumpable
  {
  public:

    /// Processing status flags used as the returned value of data processing methods through the pipeline
    enum process_status {
      PROCESS_OK       = 0,                          /** Module has processed normally and processing
                                                      *  can go on
                                                      */
      PROCESS_SUCCESS  = PROCESS_OK,                 /** idem PROCESS_OK */
      PROCESS_CONTINUE = PROCESS_OK,                 /** idem PROCESS_OK */

      PROCESS_ERROR    = datatools::bit_mask::bit00, /** Module has met an error (considered as
                                                      *  non critical)
                                                      */
      PROCESS_FAILURE  = PROCESS_ERROR,              /** idem PROCESS_ERROR */

      PROCESS_STOP     = datatools::bit_mask::bit01, /** Module asks for the stop of the processing
                                                      *  of the current data record in the current
                                                      *  pipeline branch
                                                      */

      PROCESS_ERROR_STOP = PROCESS_ERROR | PROCESS_STOP, /** Module asks for the stop of the processing
                                                          *  of the current data record in the current
                                                          *  pipeline branch because of some error
                                                          */

      PROCESS_FATAL    = datatools::bit_mask::bit02, /** Module has met an error and requests a total
                                                      *  abortion of the processing session
                                                      */

      PROCESS_INVALID  = datatools::bit_mask::bit31  /** Invalid process status */
    };

    /// Default constructor :
    base_module(datatools::logger::priority p = datatools::logger::PRIO_FATAL);

    /// Destructor :
    virtual ~base_module();

    /// Check debug logging priority threshold
    bool is_debug() const;

    /// Set debug logging priority threshold
    void set_debug(bool);

    /// Check the module name
    bool has_name() const;

    /// Set the module name
    void set_name(const std::string &);

    /// Return the module name
    const std::string & get_name() const;

    /// Check the module description
    bool has_description() const;

    /// Return the module description
    const std::string & get_description() const;

    /// Set the module description
    void set_description(const std::string & a_description);

    /// Check the module last error message
    bool has_last_error_message() const;

    /// Set the module last error message
    void set_last_error_message(const std::string &);

    /// Append an error message
    void append_last_error_message(const std::string & errmsg_);

    /// Reset the module last error message
    void reset_last_error_message();

    /// Return the module last error message
    const std::string & get_last_error_message() const;

    /// Check initialization flag
    bool is_initialized() const;

    /// Initialize the module without external resource
    virtual void initialize_simple();

    /// Initialize the module using a set of parameters
    virtual void initialize_standalone(const datatools::properties & a_config);

    /// Initialize the module using a set of parameters and a service manager
    virtual void initialize_with_service(const datatools::properties & a_config,
                                         datatools::service_manager & a_service_manager);

    /// Initialize the module using a set of parameters and a map of modules
    virtual void initialize_without_service(const datatools::properties & a_config,
                                            module_handle_dict_type & a_module_dictionnary);

    /** The main initialization method (post-construction):
     * @param a_config the container of configuration parameters
     * @param a_module_dictionnary a dictionnary of existing 'modules' which can be used
     *        to build the current module (used in the framework of a factory)
     * @param a_service_manager a manager for external services
     */
    virtual void initialize(const datatools::properties & a_config,
                            datatools::service_manager & a_service_manager,
                            module_handle_dict_type & a_module_dictionnary) = 0;

    /** The main data model processing method
     * @param a_data_model the data model to be processed
     * @return the status code (0=ok)
     */
    virtual process_status process(datatools::things & a_data_model) = 0;

    /// The main termination method
    virtual void reset() = 0;

    /// Smart print
    virtual void tree_dump(std::ostream &      a_out = std::clog,
                           const std::string & a_title  = "",
                           const std::string & a_indent = "",
                           bool                a_inherit = false) const;

    /// Default print
    void print(std::ostream & a_out = std::clog) const;

    /// Set logging priority
    void set_logging_priority(datatools::logger::priority p);

    /// Returns logging priority
    datatools::logger::priority get_logging_priority() const;

    /// Basic OCD support shared by all inherited modules
    static void common_ocd(datatools::object_configuration_description & ocd_);

  protected:

    /// Set the name of the module
    void _set_name(const std::string & a_name);

    /// Set the initialization flag of the module
    void _set_initialized(bool a_initialized);

    /// Basic initialization shared by all inherited modules
    void _common_initialize(const datatools::properties & a_config);

  protected:

    std::string _name;           //!< The name of the module
    std::string _description;    //!< The description of the module
    datatools::logger::priority _logging; //!< The logging priority threshold

    bool        _initialized;    //!< The initialization flag
    std::string _last_error_message_; //!< Last error message

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_module);

  };

}  // end of namespace dpp

/* Macros for interface/implementation of static creator methods in module classes */

/** Interface macro to automate the registration of the module in the global register for
 *  data processing modules.
 *
 *  Example (in the class header file):
 *  \code
 *  class my_module : public dpp::base_module
 *  {
 *
 *    DPP_MODULE_REGISTRATION_INTERFACE(my_module)
 *  };
 *  \endcode
 */
#define DPP_MODULE_REGISTRATION_INTERFACE(T)                            \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::dpp::base_module,T); \
  /**/

/** Implementation macro to automate the registration of the module in the global register for
 *  data processing modules.
 *
 *  Example (in the class definition file):
 *  \code
 *  DPP_MODULE_REGISTRATION_IMPLEMENT(my_module, "my_module")
 *  \endcode
 */
#define DPP_MODULE_REGISTRATION_IMPLEMENT(T,ModuleID)                   \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::dpp::base_module,T,ModuleID); \
  /**/

#endif // DPP_BASE_MODULE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
