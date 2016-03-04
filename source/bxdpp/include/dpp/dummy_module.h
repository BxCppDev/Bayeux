/// \file dpp/dummy_module.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-19
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
 *
 * Description:
 *
 *   A dummy data processing module.
 *
 * History:
 *
 */

#ifndef DPP_DUMMY_MODULE_H
#define DPP_DUMMY_MODULE_H 1

// Standard library:
#include <string>

// This project:
#include <dpp/base_module.h>

namespace dpp {

  /// \brief A dummy data processing module for tests
  /** The 'dummy_module' class inherits the interface of the
   * 'dpp::base_module' abstract class.
   */
  class dummy_module : public base_module
  {
  public:

    static const std::string & default_gp_label();
    static const std::string & default_flag_name();

    /// Set the flag name
    void set_flag_name(const std::string & a_flag_name);

    /// Get the flag name
    const std::string & get_flag_name() const;

    /// Set the "GP" bank label
    void set_GP_label(const std::string & a_gp_label);

    /// Get the "GP" bank label
    const std::string & get_GP_label() const;

    /// Constructor
    dummy_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~dummy_module();

    /** Initialization method :
     *
     * \param a_properties is a const reference to some container of
     * various property configuration parameters
     * \param a_srv_mgr a reference to a service manager(provided by some
     * external agent)
     * \param a_mod_dict a reference to a dictionnary of pre-existing modules
     * (provided by some external agent)
     */
    virtual void initialize(const datatools::properties & a_properties,
                            datatools::service_manager & a_srv_mgr,
                            module_handle_dict_type & a_mod_dict);

    /// Termination method
    virtual void reset();

    /** Event processing method :
     *  \param a_data_record is a mutable reference to the data model instance to be processed.
     *  \return the error status of the data record processing
     */
    virtual process_status process(datatools::things & a_data_record);

  private:

    std::string _GP_label_;  //!< The name of the "General properties" data bank in the data record.
    std::string _flag_name_; //!< The name of the flag to be stored inside the "General properties" data bank.

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(dummy_module)

  };

}  // end of namespace dpp

#endif // DPP_DUMMY_MODULE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
