/// \file datatools/urn_db_service.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-03-06
 * Last modified: 2017-03-31
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
#include <set>

// Third party:
// - Boost:
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

// This project:
#include <datatools/properties.h>
#include <datatools/base_service.h>
#include <datatools/bit_mask.h>
#include <datatools/urn_info.h>

namespace datatools {

  class dependency_graph;

  /// \brief A service with a dictionary of URN informations.
  ///
  /// This service describes not only a collection of objects addresses by their
  /// unique URNs but also the relationships between these objects.
  ///
  /// Objects addresses through their URN are connected via "links".
  /// A link represents an unidirectional relationship between two nodes
  /// conventionaly labelled [from] and [to].
  /// The [from] node can be considered as the client which explicitely depends on
  /// the [to] node, considered as the server.
  /// The exact nature of the relationship depends on the context. For example, it
  /// may express a dependency:
  /// \code
  ///   [from]  "dependency"      [to]
  ///      o --------------------> o
  ///  "depender"              "dependee"
  /// \endcode
  /// or a composition:
  /// \code
  ///   [from]   "composition"    [to]
  ///      o --------------------> o
  /// "composite"              "element"
  /// \endcode
  /// or a service:
  /// \code
  ///   [from]     "service"      [to]
  ///      o --------------------> o
  ///  "client"                 "server"
  /// \endcode
  /// or a subscription:
  /// \code
  ///   [from]  "subscription"    [to]
  ///      o --------------------> o
  /// "subscriber"             "service"
  /// \endcode
  /// or a usage:
  /// \code
  ///   [from]      "usage"       [to]
  ///      o --------------------> o
  /// "application"             "data"
  /// \endcode
  /// A link is always defined on the [from] node's side, i.e. from the object
  /// which explicitly depends on the object pointed by the link (forward link),
  /// because such
  /// an object cannot exist or work without the explicit knowledge of the
  /// objects to which it is connected.
  /// The service keeps also records about the backward links, i.e. links pointing
  /// from [to] to [from] nodes. This way, it is possible to know whether or not
  /// a node can be safely removed because no other node points to it.
  /// \code
  /*
  ///                     urn1
  ///             [to]...o
  ///                   /|\  \
  ///             ^    / | \  \ backward
  ///            /    /  |  \  \  links
  ///   forward /    /   |   \  \
  ///    links /    /    |    \  \
  ///         /    /     |     \  \
  ///        /    /      |      \  v
  ///   [from]...o.......o.......o
  ///          urn2     urn3    urn4
  /// \endcode
  ///
  /// It is forbidden to create cycles of links between nodes:
  /// \code
  ///   urn1 o <-----------------.
  ///         \                   \
  ///          \                   \
  ///           `-----------------> o urn2
  ///
  ///                  urn1
  ///                   .o
  ///                   / ^
  ///                  /   \
  ///                 /     \
  ///                /       \
  ///               /         \
  ///              /           \
  ///             v             \
  ///            o ------------> o
  ///          urn2             urn3
  /// \endcode
  ///
  /// In terms of the Boost Graph Library, URN nodes are "vertices",
  /// forward links are "out-edges". Backward links are "in-edges".
  ///
  */
  class urn_db_service
    : public ::datatools::base_service
  {
  public:

    /// \brief External dependee URN database service
    struct dependee_db_entry
    {
      dependee_db_entry();
      explicit dependee_db_entry(const urn_db_service & db_);
      void make(const urn_db_service & db_);
      const urn_db_service & get_db() const;
    private:
      const urn_db_service * _db_ = nullptr;
    };

    /// \brief Dictionary of external dependee URN database services
    typedef std::map<std::string, dependee_db_entry> dependee_db_dict_type;

    /// \brief URN record
    struct urn_record
    {
      bool is_local() const;
      bool is_mounted() const;
      bool is_valid() const;
      void make_local(const std::string & urn_,
                      const std::string & category_,
                      const std::string & description_);
      void make_mounted(const urn_db_service & mounted_db_,
                        const std::string & mounted_urn_);
      const urn_info & get() const;
    private:
      urn_info & _grab_();
    public:
      urn_info               _uinfo_;
      const urn_db_service * _mounted_db_ref_ = nullptr;
      const urn_info *       _mounted_uinfo_ = nullptr;
      friend urn_db_service;
    };

    //! \brief URN record dictionary
    typedef std::map<std::string, urn_record> urn_record_dict_type;

    /// \brief List of [from] URN node backward pointing links associated to a [to] URN node
    struct backward_links
    {
      std::size_t get_number_of_links() const;
      bool is_linked_to_urn(const std::string & urn_) const;
      void add_link_to_urn(const std::string & urn_);
      void remove_link_to_urn(const std::string & urn_);
      void clear();
      const std::set<std::string> & get_urns() const;
    private:
      std::set<std::string> _urns_; //!< List of pointed backward URNs
    };

    /// \brief Dictionary of backward links
    typedef std::map<std::string, backward_links> backward_links_dict_type;

    /// Default constructor
    urn_db_service();

    /// Destructor
    ~urn_db_service() override;

    //! Check if mounted URN are allowed
    bool is_allow_mounted() const;

    //! Set allowed mounted flag
    void set_allow_mounted(bool m_);

    //! Check if explicit allowed categories are defined
    bool has_explicit_allowed_categories() const;

    //! Check if a category is a allowed
    bool has_allowed_category(const std::string & category_) const;

    //! Add an allowed category
    void add_allowed_category(const std::string & category_);

    //! Remove an allowed category
    void remove_allowed_category(const std::string & category_);

    //! Return allowed categories
    const std::set<std::string> & get_allowed_categories() const;

    //! Check if a category is allowed
    bool check_category(const std::string & category_) const;

    //! Check if a link topic is a allowed
    bool has_allowed_link_topic(const std::string & link_topic_) const;

    //! Check if explicit allowed link topics are defined
    bool has_explicit_allowed_link_topics() const;

    //! Add an allowed link topic
    void add_allowed_link_topic(const std::string & link_topic_);

    //! Remove an allowed link topic
    void remove_allowed_link_topic(const std::string & link_topic_);

    //! Return allowed link topics
    const std::set<std::string> & get_allowed_link_topics() const;

    //! Check if a link topic is allowed
    bool check_link_topic(const std::string & link_topic_) const;

    //! Connect to an external URN database dependee service
    void connect_db(const urn_db_service & db_);

    //! Check if the db has dependees
    bool has_dependees() const;

    //! Return the list of dependees
    std::set<std::string> get_dependees() const;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialize the service from a list of properties
    int initialize(const datatools::properties & config_,
                           datatools::service_dict_type & services_) override;

    //! Reset
    int reset() override;

    /// Smart print
    void tree_dump(std::ostream& out = std::clog,
                           const std::string & title = "",
                           const std::string & indent = "",
                           bool inherit = false) const override;

    //! Load an URN info definition file
    void load(const std::string & filename_);

    //! Load a list of URN records without components from a CSV file
    void load_csv(const std::string & filename_);

    //! Check if an URN is mounted
    bool is_mounted(const std::string & urn_) const;

    //! Check if an URN is local
    bool is_local(const std::string & urn_) const;

    //! Parse mount rule
    static bool parse_mount_rule(const std::string & rule_,
                                 std::string & external_db_,
                                 std::string & mounted_urn_);

    // //! Parse mount rule
    // static bool parse_mount_rule(const std::string & rule_,
    //                              std::string & external_db_,
    //                              std::set<std::string> & mounted_urns_);

    //! Add a mounted URN
    void add_mounted(const std::string & external_db_,
                     const std::string & mounted_urn_);

    //! Add a new URN info
    const urn_info & add(const std::string & urn_,
                         const std::string & category_,
                         const std::string & description_ = "");

    //! Remove an URN info
    void remove(const std::string & urn_);

    //! Add a forward link with given topic from one [from] URN record to another [to]
    //! \code
    //! URN-from   "forward topic >"   URN-to
    //!       o ----------------------> o
    //! \endcode
    bool add_link(const std::string & urn_from_,
                  const std::string & urn_to_,
                  const std::string & forward_topic_,
                  std::string & errmsg_);

    //! Add a forward link with given topic from one [from] URN record to another [to]
    void add_link(const std::string & urn_from_,
                  const std::string & urn_to_,
                  const std::string & forward_topic_);

    //! Remove a link between two URN info records
    bool remove_link(const std::string & urn_from_,
                     const std::string & urn_to_,
                     std::string & errmsg_);

    //! Remove a link between two URN info records
    void remove_link(const std::string & urn_from_,
                     const std::string & urn_to_);

    //! Remove all links from a URN record
    void remove_all_links(const std::string & urn_from_);

    //! Check if an URN record is linked to another one through a certain topic
    bool is_linked_to(const std::string & urn_,
                      const std::string & other_urn_,
                      const std::string & topic_ = "") const;

    //! Check if an URN record is linked to some other ones (forward links only)
    bool has_links(const std::string & urn_) const;

    //! Check if URN info exists
    bool has(const std::string & urn_) const;

    //! Return the URN info
    const urn_info & get(const std::string &) const;

    //! Return the number of URN records
    std::size_t get_number_of_urn_records() const;

    //! \brief URN record selection flags
    enum urn_select_flag {
      SELECT_NOCLEAR   = bit_mask::bit00,
      SELECT_LOCAL     = bit_mask::bit01,
      SELECT_MOUNTED   = bit_mask::bit02,
      SELECT_REMOVABLE = bit_mask::bit03,
      SELECT_LINKED    = bit_mask::bit04
    };

    //! Build the list of registered URNs
    void build_list_of_urns(std::vector<std::string> & urns_,
                            const uint32_t flags_) const;

    //! Check if a URN record has backward links
    bool has_backward_links(const std::string & urn_) const;

    //! Check if a URN record can be removed
    bool is_removable(const std::string & urn_) const;

    //! Check if the service is locked
    bool is_locked() const;

    //! Lock the service
    void lock();

    //! Unlock the service
    void unlock();

    //! Add to system
    void kernel_push(const std::string & name_ = "");

    //! Remove from system
    void kernel_pop();

    //! Check if the service is pushed (registered) in the kernel
    bool is_kernel_pushed() const;

    //! Populated a dependency graph
    struct dependency_graph_builder
    {
      dependency_graph_builder(const urn_db_service &, const logger::priority & = logger::PRIO_FATAL);
      void add_topic(const std::string & topic_);
      void make_deps(dependency_graph & deps_) const;
    private:
      void _process_node_components_(const urn_record & urec_,
                                     dependency_graph & deps_,
                                     std::set<std::string> & processed_) const;

    private:

      const urn_db_service & _db_;      ///< Source DB service
      logger::priority       _logging_; ///< Logging priority threshold
      std::set<std::string>  _topics_;  ///< Link topics to be taken into account for creating dependency edges
    };

  private:

    //! Return the mutable URN info
    urn_info & _grab_(const std::string &);

    void _init_();

    void _reset_();

    void _at_lock_();

    void _at_unlock_();

    void _add_backward_link_(const std::string & urn_to_,
                             const std::string & urn_from_);

    void _remove_backward_link_(const std::string & urn_to_,
                                const std::string & urn_from_);

    void _process_links_(const std::string & urn_from_);

  private:

    // Management:
    bool _initialized_ = false;
    bool _locked_      = false;

    // Configuration:
    std::set<std::string> _allowed_categories_;    //!< List of allowed categories
    std::set<std::string> _allowed_link_topics_;   //!< List of allowed link topic
    bool                  _allow_mounted_ = false; //!< Flag to allow links to external URN
    dependee_db_dict_type _dependee_dbs_;          //!< Dictionary of external dependee database services

    // Private data:
    urn_record_dict_type     _urn_records_;    //!< Dictionary of URN records
    backward_links_dict_type _backward_links_; //!< Dictionary of backward links

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
