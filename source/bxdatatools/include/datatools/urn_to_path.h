/// \file datatools/urn_to_path.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-03-10
 * Last modified: 2017-03-10
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
 *   URN to path association record.
 *
 */

#ifndef DATATOOLS_URN_TO_PATH_H
#define DATATOOLS_URN_TO_PATH_H

// Standard Library:
#include <string>
#include <vector>

// Datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

namespace datatools {

  /// \brief A class representing the association of an URN to some paths.
  ///        A path maybe a filesystem path, an URL, a hostname/port identifier
  ///        or any other location address that can be represented as a character
  ///        string.
  ///        The list if registered paths is ordered to reflect some possible
  ///        priority policy, depending on the way paths have been inserted
  ///        in the register.
  ///
  ///        It is possible to assign a category to the object, examples:
  ///          "configuration", "data", "debian::package", "images"...
  ///
  ///        It is possible to assign a MIME type to the object, examples:
  ///          "text/configuration", "image/png"...
  ///
  ///        A URN-to-path record is considered value if its URN is set and
  ///        at least one path is registered.
  class urn_to_path
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
  {
  public:

    /// Default constructor
    urn_to_path();

    /// Constructor with explicit attributes and one single registered path
    urn_to_path(const std::string & urn_,
                const std::string & path_,
                const std::string & category_ = "",
                const std::string & mime_ = "");

    /// Constructor with  explicit attributes
    urn_to_path(const std::string & urn_,
                const std::string & category_,
                const std::string & mime_,
                const std::vector<std::string> & paths_);

    /// Destructor
    ~urn_to_path() override;

    /// Check validity
    bool is_valid() const;

    /// Reset
    void reset();

    /// Check if URN identifier is set
    bool has_urn() const;

    /// Set the URN identifier
    void set_urn(const std::string &);

    /// Return the URN identifier
    const std::string & get_urn() const;

    /// Check if one single path is set
    bool has_single_path() const;

    /// Check is at least one path is set
    bool has_path() const;

    /// Check if a specific path is registered
    bool has_path_by_value(const std::string &) const;

    /// Set a single path at first position
    void set_path(const std::string & path_);

    /// Return the path at first position
    const std::string & get_path() const;

    /// Return the number of regisgtered paths
    std::size_t get_number_of_paths() const;

    /// Return the path registered at given position
    const std::string & get_path_by_index(const std::size_t index_ = 0) const;

    /// Add a path (postpend)
    void add_path(const std::string &, bool preprend_ = false);

    /// Remove all registered paths
    void clear_paths();

    /// Check if the category is set
    bool has_category() const;

    /// Set the category
    void set_category(const std::string &);

    /// Return the category
    const std::string & get_category() const;

    /// Reset the category
    void reset_category();

    /// Check if the MIME type is set
    bool has_mime() const;

    /// Set the MIME type
    void set_mime(const std::string &);

    /// Return the MIME type
    const std::string & get_mime() const;

    /// Reset the MIME type
    void reset_mime();

    /// Main interface method for smart dump
    void tree_dump (std::ostream & out_ = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const override;

  private:

    std::string _urn_;      //!< The URN representation of an object
    std::string _category_; //!< The category of the object (optional)
    std::string _mime_;     //!< The MIME type (optional)
    std::vector<std::string> _paths_; //!< The paths associated to the URN which locate the object

    //! Support for Boost-based serialization
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(urn_to_path)

  };

} // end of namespace datatools

#endif // DATATOOLS_URN_TO_PATH_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
