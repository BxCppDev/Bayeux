/// \file datatools/urn_db_service.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-03-06
 * Last modified: 2017-03-06
 *
 * License:
 *
 * Copyright (C) 2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   URN information database.
 *
 */

#ifndef DATATOOLS_URN_DB_SERVICE_H
#define DATATOOLS_URN_DB_SERVICE_H

// Standard Library:
#include <string>
#include <map>

// This project:
#include <datatools/properties.h>
#include <datatools/base_service.h>
#include <datatools/urn_info.h>

namespace datatools {

  /// \brief A service with a dictionary of URN informations
  class urn_db_service
    : public ::datatools::base_service
  {
  public:

    //! \brief URN info dictionary
    typedef std::map<std::string, urn_info> urn_info_dict_type;

    /// Default constructor
    urn_db_service();

    /// Destructor
    virtual ~urn_db_service();

    //! Check initialization status
    virtual bool is_initialized() const;

    //! Initialize the service from a list of properties
    virtual int initialize(const datatools::properties & config_,
                           datatools::service_dict_type & services_);

    //! Reset
    virtual int reset();

    /// Smart print
    virtual void tree_dump(std::ostream& out = std::clog,
                           const std::string & title = "",
                           const std::string & indent = "",
                           bool inherit = false) const;

    //! Load an URN info definition file
    void load(const std::string & filename_);

    //! Load a list of leaf (no components) URN informations from a CSV file
    void load_csv_leaves(const std::string & filename_);

    //! Add an URN info
    void add(const urn_info & uinfo_);

    //! Check if URN info exists
    bool has(const std::string & urn_) const;

    //! Return the URN info
    const urn_info & get(const std::string &) const;

    //! Return the number of URN infos
    std::size_t get_number_of_urn_infos() const;

    //! Add to system
    void kernel_push(const std::string & = "");

    //! Remove from system
    void kernel_pop();

    //! Return the dictionary of URNS
    const urn_info_dict_type & get_urn_infos() const;

  private:

    void _init_();

    void _reset_();

  private:

    // Management:
    bool _initialized_ = false;

    // Configuration:

    // Private data:
    urn_info_dict_type _urn_infos_;

    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(urn_db_service)

  };

} // end of namespace datatools

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(datatools::urn_db_service)

#endif // DATATOOLS_URN_DB_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
