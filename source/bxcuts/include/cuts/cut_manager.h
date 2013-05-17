/* cut_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-22
 * Last modified : 2013-05-17
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
 *   A generic manager for cuts on arbitrary data.
 *
 * History:
 *
 */

#ifndef CUTS_CUT_MANAGER_H_
#define CUTS_CUT_MANAGER_H_ 1

#include <string>

#include <boost/cstdint.hpp>

#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>
#include <datatools/logger.h>

#include <cuts/i_cut.h>
#include <cuts/cut_tools.h>

// Forward declaration :
namespace datatools {
  class multi_properties;
  class service_manager;
}

namespace cuts {

  /// \brief The cut manager class
  class cut_manager : public datatools::i_tree_dumpable

  {
  public:

    enum ctor_flag_type {
      BLANK             = 0,
      FACTORY_VERBOSE   = datatools::bit_mask::bit00,
      FACTORY_NOPRELOAD = datatools::bit_mask::bit01,
      FACTORY_INITIALIZATION_AT_LOAD = datatools::bit_mask::bit02,
    };

    /// Set logging priority
    void set_logging_priority(datatools::logger::priority p);

    /// Returns logging priority
    datatools::logger::priority get_logging_priority() const;

    bool is_debug() const;

    bool is_verbose() const;

    bool is_no_preload() const;

    bool is_initialization_at_load() const;

    bool has(const std::string & a_cut_name) const;

    void remove(const std::string & a_cut_name);

    i_cut & grab(const std::string & a_cut_name);

    const i_cut & get(const std::string & a_cut_name) const;

    cut_handle_dict_type & get_cuts();

    const cut_handle_dict_type & get_cuts() const;

    bool is_initialized() const;

    void initialize(const datatools::properties & a_setup);

    void reset();

    cut_manager(uint32_t the_flags = BLANK);

    virtual ~cut_manager();

    bool has_service_manager() const;

    const datatools::service_manager & get_service_manager() const;

    datatools::service_manager & grab_service_manager();

    void set_service_manager(datatools::service_manager & a_service_manager);

    void install_service_manager(const datatools::properties & a_service_manager_configuration);

    virtual void tree_dump(std::ostream & a_out         = std::clog,
                           const std::string & a_title  = "",
                           const std::string & a_indent = "",
                           bool a_inherit          = false) const;

    void load_cut(const std::string & cut_name_,
                  const std::string & cut_id_,
                  const datatools::properties & cut_config_);

    //void create_cut(cut_entry_type & cut_entry_);

    //void initialize_cut(cut_entry_type & cut_entry_);

    void load_cuts(const datatools::multi_properties & cuts_config_);

    bool has_cut_type(const std::string & cut_id_) const;

    template <class CutClass>
    void register_cut_type(const std::string & cut_id_)
    {
      _factory_register_.registration(cut_id_, boost::factory<CutClass*>());
      return;
    }

    void unregister_cut_type(const std::string & cut_id_);

  protected:

    void _load_cut(const std::string & cut_name_,
                   const std::string & cut_id_,
                   const datatools::properties & cut_config_);

    void _load_cuts(const datatools::multi_properties & cuts_config_);

    void _create_cut(cut_entry_type & cut_entry_);

    void _initialize_cut(cut_entry_type & cut_entry_);

    void _reset_cut(cut_entry_type & cut_entry_);

    void _preload_global_dict();

  private:

    datatools::logger::priority  _logging_;          //!< Logging priority threshold
    bool                         _initialized_;      //!< Initialization status
    uint32_t                     _flags_;            //!< Some flags
    i_cut::factory_register_type _factory_register_; //!< Factory register
    cut_handle_dict_type         _cuts_;             //!< Dictionnary of cuts
    bool                         _service_manager_owner_; //!< Owner flag for the embedded service manager
    datatools::service_manager * _service_manager_;  //!< Handle to the embedded service manager

    friend class cut_entry_type;
  };

}  // end of namespace cuts


/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(cuts::cut_manager)

#endif // CUTS_CUT_MANAGER_H_

// end of cut_manager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
