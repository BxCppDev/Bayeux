/// \file datatools/urn_query_service.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-03-07
 * Last modified: 2017-03-07
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
 *   URN information meta service.
 *
 */

#ifndef DATATOOLS_URN_QUERY_SERVICE_H
#define DATATOOLS_URN_QUERY_SERVICE_H

// Standard Library:
#include <string>
#include <map>

// This project:
#include <datatools/base_service.h>
#include <datatools/urn_info.h>

namespace datatools {

  class urn_db_service;
  class urn_to_path_resolver_service;

  /// \brief A service which allows queries about URNs thanks to embedded
  ///        specialized dictionaries of URN informations and URN/path resolver tables
  class urn_query_service
    : public ::datatools::base_service
  {
  public:

    /// Default constructor
    urn_query_service();

    /// Destructor
    virtual ~urn_query_service();

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

    /// Check if an URN information is defined
    bool urn_exists(const std::string & urn_,
                    const std::string & category_ = "") const;

    /// Build a list of URNs with requested URN and category patterns
    bool urn_find(std::vector<std::string> & urn_list_,
                  const std::string & urn_db_regex_ = "",
                  const std::string & urn_regex_ = "",
                  const std::string & urn_category_ = "",
                  bool clear_ = true) const;

    /// Return a registered URN info
    const urn_info & get_urn_info(const std::string & urn_) const;

    /// Check if URN is associated to a path
    bool urn_has_path(const std::string & urn_) const;

    /// Resolve an URN as a path, possibly with category and MIME type
    bool resolve_urn_as_path(const std::string & urn_,
                             std::string & category_,
                             std::string & mime_,
                             std::string & path_) const;

    /// Check if an URN database with given name is registered
    bool has_db(const std::string & name_) const;

    /// Check if an URN database is registered
    bool has_db(const urn_db_service &) const;

    /// Register an URN database with given name
    void add_db(const urn_db_service &, const std::string & name_ = "");

    /// Unregister an URN database
    void remove_db(const urn_db_service &);

    /// Unregister an URN database with given name
    void remove_db(const std::string & name_);

    /// Check if an URN path resolver with given name is registered
    bool has_path_resolver(const std::string & name_) const;

    /// Check if an URN path resolver is registered
    bool has_path_resolver(const urn_to_path_resolver_service &) const;

    /// Register an URN path resolver with given name
    void add_path_resolver(const urn_to_path_resolver_service &,
                           const std::string & name_ = "");

    /// Unregister an URN path resolver
    void remove_path_resolver(const urn_to_path_resolver_service &);

    /// Unregister an URN path resolver with given name
    void remove_path_resolver(const std::string & name_);

  //   // Return the mutable singleton URN system instance
  //   static urn_query_service & instance();

  //   // Return the non mutable singleton URN system instance
  //   static const urn_query_service & const_instance();

  // private:

  //   // Return the mutable singleton URN system instance
  //   static urn_query_service & _instance_();

  private:

    // Management:
    bool _initialized_ = false; //!< Initialization flag

    // Private implementation:
    struct pimpl_type;
    std::unique_ptr<pimpl_type> _pimpl_; //!< Private implementation

    // Service registration
    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(urn_query_service)

  };

} // end of namespace datatools

#endif // DATATOOLS_URN_QUERY_SERVICE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
