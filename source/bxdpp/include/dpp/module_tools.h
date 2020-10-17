/// \file dpp/module_tools.h
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
 * Description:
 *
 *   Typedefs for the event record processing module factory.
 *
 */

#ifndef DPP_MODULE_TOOLS_H
#define DPP_MODULE_TOOLS_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>
#include <exception>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/bit_mask.h>
#include <datatools/properties.h>

// This project:
#include <dpp/dpp_config.h>

namespace dpp {

  class base_module;

  typedef std::map<std::string, base_module *> module_dict_type;

  typedef datatools::handle<base_module> module_handle_type;

  class module_manager;

  /// \brief Module record entry class
  class module_entry_type : public datatools::i_tree_dumpable
  {
  public:

    enum status_type
      {
        STATUS_BLANK       = 0,
        STATUS_CREATED     = datatools::bit_mask::bit00,
        STATUS_INITIALIZED = datatools::bit_mask::bit01
      };

  public:

    const datatools::properties & get_module_config() const;

    datatools::properties & grab_module_config();

    void set_module_config(const datatools::properties &);

    const std::string & get_module_name() const;

    void set_module_name(const std::string &);

    const std::string & get_module_id() const;

    void set_ptr(base_module *);

    void set_module_id(const std::string &);

    bool has_manager() const;

    void set_manager(module_manager &);

    void set_blank();

    void set_created();

    void set_initialized();

    void set_uninitialized();

    bool is_created() const;

    bool is_initialized() const;

    /// Constructor
    module_entry_type();

    /// Destructor
    ~module_entry_type() override;

    /// Check is the module is referenced in the embedded handle
    bool has_module() const;

    const base_module & get_module() const;

    base_module & grab_module();

    const module_handle_type & get_module_handle() const;

    module_handle_type & grab_module_handle();

    module_handle_type & grab_initialized_module_handle();

    void tree_dump(std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

  private:

    std::string           _module_name_;      ///< Module name
    std::string           _module_id_;        ///< Module class registration ID
    datatools::properties _module_config_;    ///< The configuration of the module
    uint32_t              _module_status_;    ///< The status of the module
    module_handle_type    _module_handle_;    ///< The handle for the allocated service
    module_manager *      _manager_;          ///< Handle to the module manager

    friend class module_manager;
  };

  typedef std::map<std::string, module_entry_type> module_handle_dict_type;

}  // end of namespace dpp

#endif // DPP_MODULE_TOOLS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
