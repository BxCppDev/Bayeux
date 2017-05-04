/// \file dpp/skip_module.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description:
 *
 *   A skip data record processing module.
 *
 * History:
 *
 */

#ifndef DPP_SKIP_MODULE_H
#define DPP_SKIP_MODULE_H 1

// This project:
#include <dpp/base_module.h>

namespace dpp {

  /// \brief A data processing module to skip some data records
  class skip_module : public base_module
  {
  public:

    /// Skip mode
    enum mode_type
    {
      SKIP_MODE_INVALID  = 0,
      SKIP_MODE_INTERVAL = 1,
      MODE_DEFAULT = SKIP_MODE_INTERVAL
    };

    /// Constructor
    skip_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~skip_module();

    /// Initialization
    virtual void initialize(const ::datatools::properties & /* config_ */,
                            datatools::service_manager & /* service_mgr_ */,
                            dpp::module_handle_dict_type & /* modules_map_ */);

    /// Reset
    virtual void reset();

    /// Data record processing
    virtual process_status process(::datatools::things & /* data_ */);

    /// \brief Internal class of the skip_module class
    struct module_entry
    {
      std::string        label;
      module_handle_type handle;
    };

    int get_first () const;

    int get_last () const;

    bool is_inverted () const;

    void set_first_last (int a_first,
                         int a_last,
                         bool a_inverted = false);

    void set_first_number (int a_first,
                           int a_number,
                           bool a_inverted = false);

  protected:

    /// Set default values before explicit settings and initialization
    void _set_defaults ();

  private:

    module_entry       _module_;   //!< The module
    int                _counter_;  //!< Data record counter
    int                _first_; //!< First data record to be skipped
    int                _last_; //!< Last data record to be skipped
    bool               _inverted_; //!< Invert data record selection flag

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(skip_module)

  };

}  // end of namespace dpp

#endif // DPP_SKIP_MODULE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
