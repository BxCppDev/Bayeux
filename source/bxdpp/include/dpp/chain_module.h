/// \file dpp/chain_module.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 *
 * Description:
 *
 *   Chain processing module.
 *
 */

#ifndef DPP_CHAIN_MODULE_H
#define DPP_CHAIN_MODULE_H 1

// Standard library:
#include <string>
#include <list>

// This project:
#include <dpp/base_module.h>

namespace dpp {

  /// \brief A data processing module to chain children data processing modules
  class chain_module
    : public base_module
  {
  public:

    /// Record for a module using a specific label
    struct module_entry
    {
      std::string        label;  //!< Label of the module
      module_handle_type handle; //!< Handle of the module
    };

    typedef std::list<module_entry> module_list_type;

    /// Add a module in the processing chain
    void add_module(const std::string & label_,
                    const module_handle_type & handle_module_);

    /// Check if a module with a given name exists
    bool has_module(const std::string & label_) const;

    /// Constructor
    chain_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    ~chain_module() override;

    /// Initialization
    void initialize(const ::datatools::properties & /* config_ */,
                            datatools::service_manager & /* service_mgr_ */,
                            dpp::module_handle_dict_type & /* modules_map_ */) override;

    /// Reset
    void reset() override;

    /// Data record processing
    process_status process(::datatools::things & /* data_ */) override;

    /// Smart print
    void tree_dump(std::ostream      & out_    = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  protected:

    module_list_type _modules_;  //!< The list of data processing modules

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(chain_module)

  };

}  // end of namespace dpp

#endif // DPP_CHAIN_MODULE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
