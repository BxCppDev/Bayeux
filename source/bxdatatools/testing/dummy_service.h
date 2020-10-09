/* dummy_service.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Dummy service.
 *
 * History:
 *
 */

#ifndef DATATOOLS_DUMMY_SERVICE_H
#define DATATOOLS_DUMMY_SERVICE_H 1

// Standard Library:
#include <string>

// This project:
#include <datatools/base_service.h>
#include <datatools/service_tools.h>

namespace datatools {

  class properties;

  /// \brief A dummy service
  class dummy_service : public base_service
  {
  public:

    const std::string & get_label() const;

    void set_label(const std::string & a_label);

    /// Check initialization
    bool is_initialized() const override;

    /// Initialize
    int initialize(const datatools::properties & a_config,
                   service_dict_type & a_service_dict) override;

    /// Reset
    int reset() override;

    /// Default constructor
    dummy_service();

    /// Destructor
    ~dummy_service() override;

    /// Smart print:
    virtual void tree_dump(std::ostream & a_out         = std::clog,
                           const std::string & a_title  = "",
                           const std::string & a_indent = "",
                           bool a_inherit          = false) const override;

  private:

    std::string _label_; //!< Label

    // Registration :
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(dummy_service)

  };

}  // end of namespace datatools

#endif // DATATOOLS_DUMMY_SERVICE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
