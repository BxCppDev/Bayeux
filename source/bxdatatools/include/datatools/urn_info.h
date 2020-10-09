/// \file datatools/urn_info.h
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
 *   URN information node structure.
 *
 */

#ifndef DATATOOLS_URN_INFO_H
#define DATATOOLS_URN_INFO_H

// Standard Library:
#include <string>
#include <vector>
#include <map>

// Datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

namespace datatools {

  /// \brief A class documenting an object addressed by its unique URN and
  ///        linked to other objects (also addressed by URNs)..
  ///
  /// An object A can be connected to other objects (B, C and D), known as components:
  /// \code
  /// B          A          D
  /// o <--------o -------> o
  ///            |
  ///            |
  ///            |
  ///            v
  ///            o
  ///            C
  /// \endcode
  ///
  /// The nature of the relationship between the object and its "components"
  /// depends on the context. It is represented by a specific "topic" which
  /// represents a category of relationship. Example:
  /// \code
  ///            topic
  ///   A     "dependency"       B
  ///    o -------------------> o
  /// depender               dependee
  /// \endcode
  ///
  class urn_info
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
  {
  public:

    /// Dictionary of list of components addressed by topic
    typedef std::map<std::string, std::vector<std::string> > component_topic_dict_type;

    /// Validate an URN representation
    ///
    /// Format:
    /// \code
    /// urn:namespace:path[:to[:some[:object[:...]]]]
    /// \endcode
    ///
    /// Examples:
    /// \code
    /// urn:snemo
    /// urn:snemo:people:johndoe
    /// urn:snemo:demonstrator:simulation:setup:2.1
    /// urn:snemo:demonstrator:simulation:data:0
    /// urn:snemo:demonstrator:simulation:data:1
    /// urn:snemo:geometry:setup:4.0
    /// urn:bipo:data:raw:run_1:metadata
    /// urn:bipo:data:raw:run_1:logfile
    /// urn:bipo:data:raw:run_1:data:0
    /// urn:bipo:data:raw:run_1:data:1
    /// urn:bipo:data:raw:run_1:data:2
    /// \endcode
    static bool validate_urn(const std::string &);

    /// Validate a category name
    static bool validate_category(const std::string &);

    /// Validate a topic name
    static bool validate_topic(const std::string &);

    /// Return the anonymous topic
    static const std::string & anonymous_topic();

    /// Default constructor
    urn_info();

    /// Constructor with explicit path segments
    urn_info(const std::string & urn_,
             const std::string & category_,
             const std::string & description_ = "");

    /// Destructor
    ~urn_info() override;

    /// Check validity
    bool is_valid() const;

    /// Initialize
    /**
     *  Example:
     *  \code
     *  #@description Thr URN identifier
     *  urn : string = "urn:fooexp:analysis:setup:1.0"
     *
     *  #@description Short description text
     *  description : string = "Foo experiment data analysis setup 1.0"
     *
     *  #@description List of topics on which URN info relies/depends on
     *  topics : string[2] = "geometry" "input""
     *
     *  #@description Unique component associated to the 'geometry' topic
     *  topic.geometry.component : string = "urn:fooexp:geometry:setup:1.0"
     *
     *  #@description Several components associated to the 'input' topic
     *  topic.input.components   : string[3] = \\
     *    "urn:fooexp:dataset:1" \\
     *    "urn:fooexp:dataset:2" \\
     *    "urn:fooexp:dataset:3"
     *
     * \endcode
     */
    void initialize(const properties &);

    /// Clear the URN info
    void reset();

    /// Set the URN
    void set_urn(const std::string & urn_);

    /// Return the URN
    const std::string & get_urn() const;

    /// Check if category is set
    bool has_category() const;

    /// Set the category
    void set_category(const std::string & category_);

    /// Return the category
    const std::string & get_category() const;

    /// Set the description
    void set_description(const std::string & description_);

    /// Check if the description is set
    bool has_description() const;

    /// Return the description
    const std::string & get_description() const;

    /// Lock the record
    void lock();

    /// Unlock the record
    void unlock();

    /// Check lock flag
    bool is_locked() const;

    /// Remove all components
    void remove_all_components();

    /// Check if a component exists
    bool has_component(const std::string & comp_urn_) const;

    /// Check if a component exists in a given topic
    bool has_component_in_topic(const std::string & comp_urn_,
                                const std::string & comp_topic_) const;

    /// Add an URN component given its topic
    void add_component(const std::string & comp_urn_,
                       const std::string & comp_topic_ = "");

    /// Remove an URN component
    void remove_component(const std::string & comp_urn_);

    /// Check if URN components are set
    bool has_components() const;

    /// Return the total number of URN components
    std::size_t get_number_of_components() const;

    /// Return the number of URN components with given topic
    std::size_t get_number_of_components_by_topic(const std::string & comp_topic_) const;

    /// Return the URN component with given topic at given rank
    std::string get_component(const std::string & comp_topic_, int rank_ = 0) const;

    /// Check if a given component topic is defined
    bool has_topic(const std::string & comp_topic_) const;

    /// Return the components associated to a given topic
    const std::vector<std::string> &
    get_components_by_topic(const std::string & comp_topic_) const;

    /// Build the current list of component topics
    void build_topics(std::vector<std::string> &) const;

    /// Return the current list of component topics
    std::vector<std::string> get_topics() const;

    /// Main interface method for smart dump
    void tree_dump(std::ostream& out = std::clog,
                           const std::string& title  = "",
                           const std::string& indent = "",
                           bool inherit = false) const override;

    /// Basic comparison with respect to the URN name
    bool operator<(const urn_info & other_) const;

  private:

    // Management:
    bool _locked_ = false; //!< Lock flag

    // Configuration:
    std::string _urn_;         //!< The URN of the object
    std::string _category_;    //!< The category of the object
    std::string _description_; //!< The description of the object
    component_topic_dict_type _components_; //!< Dictionary of URN components, sorted by topics

    //! Support for Boost-based serialization
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(urn_info)

  };

} // end of namespace datatools

#endif // DATATOOLS_URN_INFO_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
