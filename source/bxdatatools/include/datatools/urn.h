/// \file datatools/urn.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-03-05
 * Last modified: 2017-03-05
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
 *   Utilities for URN manipulation.
 *
 */

#ifndef DATATOOLS_URN_H
#define DATATOOLS_URN_H

// Standard Library:
#include <string>
#include <vector>

// Datatools:
#include <datatools/i_serializable.h>

namespace datatools {

  /// \brief A class representing a simplified URN
  ///        conforming parts of the RFC2141
  ///
  /// \see http://www.ietf.org/rfc/rfc2141.txt
  /**
   *
   * Supported format is :
   * \code
   *   urn:namespace:specific-path
   * \endcode
   *
   */
  class urn
    : public datatools::i_serializable
  {
  public:

    /// Return the URN scheme
    static const std::string & urn_scheme();

    /// Return the URN separator
    static char urn_separator();

    /// Default constructor
    urn();

    /// Constructor with explicit path segments
    urn(const std::vector<std::string> & segments_);

    /// Destructor
    ~urn() override;

    /// Check validity
    bool is_valid() const;

    /// Check is namespace is set
    bool has_namespace() const;

    /// Return the namespace
    const std::string & get_namespace() const;

    /// Set path segments
    void set_segments(const std::vector<std::string> & segments_);

    /// Append a path segment
    void append_segment(const std::string & segment_);

    /// Return the depth of the path segments
    std::size_t get_depth() const;

    /// Clear the path segments
    void reset();

    /// Return the path segments
    const std::vector<std::string> & get_segments() const;

    /// Convert to a string
    void to_string(std::string &) const;

    /// Return a string representation
    std::string to_string() const;

    /// Initialize from a string
    bool from_string(const std::string &);

    friend std::ostream & operator<<(std::ostream &, const urn &);

  private:

    std::vector<std::string> _segments_; //!< Path segments

    //! Support for Boost-based serialization
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(urn)

  };

} // end of namespace datatools

#endif // DATATOOLS_URN_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
