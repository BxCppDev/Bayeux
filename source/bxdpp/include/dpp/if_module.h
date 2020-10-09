/// \file dpp/if_module.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-19
 * Last modified : 2013-05-15
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
 *   A skip data processing module.
 *
 */

#ifndef DPP_IF_MODULE_H
#define DPP_IF_MODULE_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/cuts:
#include <cuts/cut_tools.h>

// This project:
#include <dpp/base_module.h>

namespace dpp {

  /// \brief A data processing module to be apply only if some cut is fulfilled
  class if_module
    : public base_module
  {
  public:

    /// \brief Cut record entry
    struct cut_entry {
      std::string           label;  //!< Cut label
      cuts::cut_handle_type handle; //!< Cut handle
    };

    /// \brief Internal module record entry
    struct module_entry {
      std::string        label;  //!< Module label
      module_handle_type handle; //!< Module handle
    };

    /// Return the default label of an external cut service
    static const std::string & default_cut_service_label();

    /// Set the label of the external cut service
    void set_cut_service_label (const std::string & label_);

    /// Return the label of the external cut service
    const std::string & get_cut_service_label () const;

    void set_condition_cut (const std::string & label_,
                            const cuts::cut_handle_type & condition_cut_);

    bool has_then_module () const;

    void set_then_module (const module_entry & then_module_);

    bool has_else_module () const;

    void set_else_module (const module_entry & else_module_);

    bool has_then_status () const;

    void set_then_status (process_status status_);

    bool has_else_status () const;

    void set_else_status (process_status status_);

    /// Constructor
    if_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    ~if_module() override;

    /// Initialization
    void initialize(const ::datatools::properties & /* config_ */,
                            datatools::service_manager & /* service_mgr_ */,
                            dpp::module_handle_dict_type & /* modules_map_ */) override;

    /// Reset
    void reset() override;

    /// Data record processing
    process_status process(::datatools::things & /* data_ */) override;

    /// Smart print
    void tree_dump (std::ostream      & out_    = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

  protected:

    /// Set default values before explicit settings and initialization
    void _set_defaults ();

  private:

    std::string    _cut_service_label_; /// Label/name of the cut service
    cut_entry      _condition_cut_;     /// Condition cut entry
    process_status _then_status_;       /// Status of the 'then' action module
    module_entry   _then_module_;       /// 'then' action module entry
    process_status _else_status_;       /// Status of the 'else' action module
    module_entry   _else_module_;       /// 'else' action module entry

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(if_module)

  };

}  // end of namespace dpp

#endif // DPP_IF_MODULE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
