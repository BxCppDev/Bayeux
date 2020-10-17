/// \file dpp/context_service.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2015-09-22
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *  Context service. A service that provides a general purpose container
 *  as the repository for transcient and sharable data during the
 *  lifetime of a data processing chain.
 *
 */

#ifndef DPP_CONTEXT_SERVICE_H
#define DPP_CONTEXT_SERVICE_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/base_service.h>
#include <datatools/service_tools.h>

// This project:
#include <dpp/dpp_config.h>

namespace datatools {
  class multi_properties;
}

namespace dpp {

  /// \brief A service that handles a context object store implemented as a datatools::multi_properties container
  class context_service
    : public datatools::base_service
  {
  public:

    /// Return a const reference to the internal store
    const datatools::multi_properties & get_store() const;

    /// Return a mutable reference to the internal store
    datatools::multi_properties & grab_store();

    /// Return a const reference to the internal store
    const datatools::multi_properties & operator()() const;

    /// Return a mutable reference to the internal store
    datatools::multi_properties & operator()();

    /// Check initialization
    bool is_initialized() const override;

    /// Initialize
    int initialize(const datatools::properties & config_,
                           datatools::service_dict_type & service_dict_) override;

    /// Reset
    int reset() override;

    /// Default constructor
    context_service();

    /// Destructor
    ~context_service() override;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  private:

    // Management:
    bool _initialized_; //!< Initialization flag

    // Configuration:
    std::string _load_filename_;   //!< Name of the file from which the store is loaded
    std::string _store_filename_;  //!< Name of the file to which the store is saved
    std::string _backup_filename_; //!< name of the backup file

    // Internal data:
    datatools::multi_properties * _store_; //!< Internal store

    // Registration :
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(context_service)

  };

}  // end of namespace dpp

#endif // DPP_CONTEXT_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
