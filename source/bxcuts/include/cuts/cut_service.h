/// \file cuts/cut_service.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-20
 * Last modified : 2013-04-22
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
 *   A cut distribution service embedding a cut manager.
 *
 * History:
 *
 */

#ifndef CUTS_CUT_SERVICE_H
#define CUTS_CUT_SERVICE_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/base_service.h>
#include <datatools/service_tools.h>

// Forward declaration :
namespace datatools {
  class properties;
}

namespace cuts {

  // Forward declaration :
  class cut_manager;

  /// \brief The cut service
  class cut_service : public datatools::base_service
  {
  public:

    /// Check if the cut manager is owned by the service
    bool owns_cut_manager () const;

    /// Return a const reference to the cut manager
    const cut_manager & get_cut_manager () const;

    /// Return a mutable reference to the cut manager
    cut_manager & grab_cut_manager ();

    /// Set the reference to an external cut manager
    void set_cut_manager (const cut_manager & a_cut_manager);

    /// Check the initialization status
    bool is_initialized () const override;

    /// Initialize the service through a collection of setup properties
    int initialize (const datatools::properties & a_config,
                            datatools::service_dict_type & a_service_dict) override;

    /// Reset
    int reset () override;

    /// Constructor
    cut_service ();

    /// Destructor
    ~cut_service () override;

    /// Smart print
    void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit               = false) const override;

  private:

    bool          _owns_manager_; //!< Flag for embedded cut manager ownership
    cut_manager * _cut_manager_;  //!< Handle to the embedded cut manager

    // Registration :
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE (cut_service)

  };

}  // end of namespace cuts

#endif // CUTS_CUT_SERVICE_H

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(cuts::cut_service)

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
