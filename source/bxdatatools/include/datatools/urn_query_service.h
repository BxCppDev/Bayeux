/// \file datatools/urn_query_service.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-03-07
 * Last modified: 2018-03-23
 *
 * License:
 *
 * Copyright (C) 2017-2018 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   URN query service.
 *
 */

#ifndef DATATOOLS_URN_QUERY_SERVICE_H
#define DATATOOLS_URN_QUERY_SERVICE_H

// Standard Library:
#include <string>
#include <map>
#include <set>

// Third party:
#include <boost/noncopyable.hpp>

// This project:
#include <datatools/base_service.h>
#include <datatools/urn_info.h>

namespace datatools {

  class urn_db_service;
  class urn_to_path_resolver_service;
  class dependency_graph;

  /// \brief A service which allows queries about URNs thanks to embedded
  ///        specialized dictionaries of URN informations and URN/path resolver tables
  class urn_query_service
    : public ::datatools::base_service
    , private boost::noncopyable
  {
  public:

    /// Default constructor
    urn_query_service();

    /// Destructor
    ~urn_query_service() override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialize the service from a list of properties
    int initialize(const datatools::properties & config_,
                           datatools::service_dict_type & services_) override;

    //! Reset
    int reset() override;

    /// Smart print
    void tree_dump(std::ostream & out = std::clog,
                           const std::string & title = "",
                           const std::string & indent = "",
                           bool inherit = false) const override;

    /// Check if an URN information is defined
    bool check_urn_info(const std::string & urn_,
                        const std::string & category_ = "") const;

    /// Build a list of URNs with requested URN and category patterns
    bool find_urn_info(std::vector<std::string> & urn_list_,
                       const std::string & urn_db_regex_ = "",
                       const std::string & urn_regex_ = "",
                       const std::string & urn_category_regex_ = "",
                       bool clear_ = true) const;

    /// Return a registered URN info
    const urn_info & get_urn_info(const std::string & urn_) const;

    /// Build a list of URNs associated to path with requested URN and category patterns
    bool find_urn_to_path(std::vector<std::string> & urn_path_list_,
                          const std::string & urn_resolver_regex_ = "",
                          const std::string & urn_regex_ = "",
                          const std::string & urn_category_regex_ = "",
                          const std::string & urn_mime_regex_ = "",
                          bool clear_ = true) const;

    /// Check if URN is associated to a path
    bool check_urn_to_path(const std::string & urn_) const;

    /// Resolve an URN as a path (filesystem path, URL, database query...),
    /// possibly with category and MIME type
    bool resolve_urn_to_path(const std::string & urn_,
                             std::string & category_,
                             std::string & mime_,
                             std::string & path_) const;

    /// Return the informations associated to a URN to path association
    bool get_urn_to_path(const std::string & urn_,
                         std::string & category_,
                         std::string & mime_,
                         std::string & path_) const;

    /// Check if an URN database with given name is registered
    bool has_db(const std::string & name_) const;

    /// Check if an URN database is registered
    bool has_db(const urn_db_service &) const;

    /// Return an URN db service with a given name
    const urn_db_service & get_db(const std::string & name_) const;

    /// Build the list of URN databases
    void build_list_of_dbs(std::set<std::string> & dbs_) const;

    /// Register an URN database
    void add_db(const urn_db_service &);

    /// Register an URN database with given name
    void add_db(const urn_db_service &, const std::string & name_);

    /// Unregister an URN database
    void remove_db(const urn_db_service &);

    /// Unregister an URN database with given name
    void remove_db(const std::string & name_);

    /// Check if an URN path resolver with given name is registered
    bool has_path_resolver(const std::string & name_) const;

    /// Build the list of URN path resolvers
    void build_list_of_path_resolvers(std::set<std::string> & prs_) const;

    /// Check if an URN path resolver is registered
    bool has_path_resolver(const urn_to_path_resolver_service &) const;

    /// Return an URN path resolver service with a given name
    const urn_to_path_resolver_service & get_path_resolver(const std::string & name_) const;

    /// Register an URN path resolver
    void add_path_resolver(const urn_to_path_resolver_service &);

    /// Register an URN path resolver with given name
    void add_path_resolver(const urn_to_path_resolver_service &,
                           const std::string & name_);

    /// Unregister an URN path resolver
    void remove_path_resolver(const urn_to_path_resolver_service &);

    /// Unregister an URN path resolver with given name
    void remove_path_resolver(const std::string & name_);

    //! Return the auto-build dependency graph associated to the service
    const dependency_graph & get_dependency_graph() const;

    //! Return the auto-build dependency graph associated to the service
    void update_dependency_graph();

  private:

    void _build_dependency_graph_();

    void _dependency_graph_process_db(const urn_db_service * db_,
                                      std::set<const urn_db_service*> & processed_dbs_);

  private:

    // Management:
    bool _initialized_ = false; //!< Initialization flag

    // Private implementation:
    struct pimpl_type;
    std::unique_ptr<pimpl_type>       _pimpl_;  //!< Private implementation
    std::unique_ptr<dependency_graph> _dg_ptr_; //!< Cached dependency graph

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
