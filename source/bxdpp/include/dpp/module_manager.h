/// \file dpp/module_manager.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-22
 * Last modified : 2013-11-13
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
 *   A manager/factory class for data (event record?) processing modules.
 *
 */

#ifndef DPP_MODULE_MANAGER_H
#define DPP_MODULE_MANAGER_H 1

// Standard library:
#include <string>
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>

// This project:
#include <dpp/base_module.h>
#include <dpp/module_tools.h>

namespace dpp {

  /// \brief A manager for data processing modules
  class module_manager
    : public datatools::i_tree_dumpable
  {
  public:

    enum ctor_flag_type {
      BLANK             = 0,
      FACTORY_NOPRELOAD = datatools::bit_mask::bit00,
      FACTORY_INITIALIZATION_AT_LOAD = datatools::bit_mask::bit01
    };

    bool is_no_preload() const;

    bool is_initialization_at_load() const;

    bool has(const std::string & module_name_) const;

    void remove(const std::string & module_name_);

    base_module & grab(const std::string & module_name_);

    const base_module & get(const std::string & module_name_) const;

    module_handle_dict_type & grab_modules();

    // DEPRECATED
    module_handle_dict_type & get_modules();

    const module_handle_dict_type & get_modules() const ;

    /// Return an array of pairs describing the name and
    /// class registration ID for each managed modules
    void build_modules_infos(std::vector<std::pair<std::string,std::string> > & infos_) const;

    bool is_initialized() const;

    void initialize(const datatools::properties & setup_);

    void initialize_simple();

    void reset();

    module_manager(uint32_t flags_ = BLANK);

    ~module_manager() override;

    bool has_service_manager() const;

    const datatools::service_manager & get_service_manager() const;

    datatools::service_manager & grab_service_manager();

    void set_service_manager(datatools::service_manager & service_manager_);

    void install_service_manager(const datatools::properties & service_manager_configuration_);

    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    void load_module(const std::string & module_name_,
                     const std::string & module_id_,
                     const datatools::properties & module_config_);

    void create_module(module_entry_type & module_entry_);

    void initialize_module(module_entry_type & module_entry_);

    void load_modules(const datatools::multi_properties & modules_config_);

    void load_modules(const std::string & modules_defs_);

    datatools::logger::priority get_logging_priority() const;

    void set_logging_priority(datatools::logger::priority);

  protected:

    void _load_module(const std::string & module_name_,
                      const std::string & module_id_,
                      const datatools::properties & module_config_);

    void _load_modules(const datatools::multi_properties & modules_config_);

    void _create_module(module_entry_type & module_entry_);

    void _initialize_module(module_entry_type & module_entry_);

    void _reset_module(module_entry_type & module_entry_);

  public:

    /// Check if a module factory given by ID is registered
    bool has_module_type(const std::string & module_id_) const;

    /// Register a factory for a given module class given a (unique) ID
    template <class ModuleClass>
    void register_module_type(const std::string & module_id_)
    {
      _factory_register_.registration(module_id_, boost::factory<ModuleClass*>());
      return;
    }

    /// Unregister a module factory
    void unregister_module_type(const std::string & module_id_);

    /// Copy registered module factories IDs in the supplied vector
    void build_registered_module_types(std::vector<std::string> & ids_) const;

  protected:

    void _preload_global_dict();

  private:

    datatools::logger::priority        _logging_priority_;
    bool                               _initialized_;           //!< Initialization status
    uint32_t                           _flags_;                 //!< Some flags
    base_module::factory_register_type _factory_register_;      //!< Factory register
    module_handle_dict_type            _modules_;               //!< Dictionnary of modules
    bool                               _service_manager_owner_; //!< Owner flag for the embedded service manager
    datatools::service_manager *       _service_manager_;       //!< Handle to the embedded service manager

  };

}  // end of namespace dpp

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(dpp::module_manager)

#endif // DPP_MODULE_MANAGER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
