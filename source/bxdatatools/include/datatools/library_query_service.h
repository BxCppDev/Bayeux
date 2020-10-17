//! \file    datatools/library_query_service.h
//! \brief   Library information query service
//! \details
//
// Copyright (c) 2017 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2017 by Université de Caen
//
// This file is part of Bayeux/datatools.
//
// Bayeux/datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_LIBRARY_QUERY_SERVICE_H
#define DATATOOLS_LIBRARY_QUERY_SERVICE_H

// This project
#include <datatools/properties.h>
#include <datatools/base_service.h>
#include <datatools/library_info.h>

namespace datatools {

  //! \brief Library information query service
  class library_query_service
    : public ::datatools::base_service
  {
  public:

    //! Constructor
    library_query_service();

    //! Destructor
    ~library_query_service() override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialize the service from a list of properties
    int initialize(const datatools::properties & config_,
                           datatools::service_dict_type & services_) override;

    //! Reset
    int reset() override;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Return a mutable reference to the embedded library info dictionary
    library_info & grab_libinfo();

    /// Return a non mutable reference to the embedded library info dictionary
    const library_info & get_libinfo() const;

  private:

    void _init_();

    void _reset_();

  private:

    // Management:
    bool _initialized_ = false; //!< Initialization flag

    // Private data:
    library_info _libinfo_; //!< Library info dictionary

    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(library_query_service)

  };

} // end of namespace datatools

#endif // DATATOOLS_LIBRARY_QUERY_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
