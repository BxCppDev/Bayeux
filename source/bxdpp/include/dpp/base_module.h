/* base_module.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-02-15
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

#ifndef DPP_BASE_MODULE_H_
#define DPP_BASE_MODULE_H_ 1

#include <iostream>
#include <string>

#include <dpp/dpp_config.h>
#include <dpp/module_tools.h>
#include <dpp/utils.h>

#include <datatools/bit_mask.h>
#include <datatools/i_tree_dump.h>
#include <datatools/factory_macros.h>
#include <datatools/logger.h>
#include <datatools/things.h>

/// \brief The datatools library's main namespace
namespace datatools {
  // Forward declaration :
  class properties;
  class multi_properties;
  class service_manager;
}

/// \brief The dpp library's main namespace
namespace dpp {

  /// \brief Base processing module (abstract interface)
  class base_module : public datatools::i_tree_dumpable
  {
  public:

    bool is_debug () const;

    void set_debug (bool);

    void set_name (const std::string &);

    const std::string & get_name () const;

    bool has_description () const;

    const std::string & get_description () const;

    void set_description (const std::string & a_description);

    bool has_version () const;

    const std::string & get_version () const;

    void set_version (const std::string & a_version);

    bool has_last_error_message () const;

    void set_last_error_message (const std::string &);

    void append_last_error_message (const std::string & errmsg_);

    void reset_last_error_message ();

    const std::string & get_last_error_message () const;

    bool is_initialized () const;

    virtual void initialize_simple ();

    virtual void initialize_standalone (const datatools::properties & a_config);

    virtual void initialize_with_service (const datatools::properties & a_config,
                                          datatools::service_manager & a_service_manager);

    virtual void initialize_without_service (const datatools::properties & a_config,
                                             module_handle_dict_type & a_module_dictionnary);

    /** The main initialization method (post-construction):
     * @param a_config the container of configuration parameters
     * @param a_module_dictionnary a dictionnary of existing 'modules' which can be used
     *        to build the current module (used in the framework of a factory)
     * @param a_service_manager a manager for external services
     */
    virtual void initialize (const datatools::properties & a_config,
                             datatools::service_manager & a_service_manager,
                             module_handle_dict_type & a_module_dictionnary) = 0;

    /** The main data model processing method
     * @param a_data_model the data model to be processed
     * @return the status code (0=ok)
     */
    virtual int process (datatools::things & a_data_model) = 0;

    /// The main termination method
    virtual void reset () = 0;

    /// Default constructor :
    base_module(datatools::logger::priority p = datatools::logger::PRIO_FATAL);

    /// Destructor :
    virtual ~base_module ();


    /// Smart print
    virtual void tree_dump (std::ostream &      a_out = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool                a_inherit = false) const;

    /// Default print
    void print (std::ostream & a_out = std::clog) const;

    /// Set logging priority
    void set_logging_priority(datatools::logger::priority p);

    /// Returns logging priority
    datatools::logger::priority get_logging_priority() const;

  protected:

    //void _lock_guard (const std::string & where_, const std::string & what_ = "");

    void _set_name (const std::string & a_name);

    void _set_initialized (bool a_initialized);

    void _common_initialize (const datatools::properties & a_config);

  protected:

    std::string _name;           //!< The name of the module
    std::string _description;    //!< The description of the module
    std::string _version;        //!< The version of the module
    datatools::logger::priority _logging; //!< The logging priority threshold

    bool        _initialized;    //!< The initialization flag
    std::string _last_error_message_; //!< Last error message

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_module);

  };

}  // end of namespace dpp

#include <dpp/module_macros.h>

#endif // DPP_BASE_MODULE_H_

// end of base_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
