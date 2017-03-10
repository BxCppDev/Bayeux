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

// Datatools:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/properties.h>

namespace datatools {

  /// \brief A class representing the association of an URN to a path.
  ///        The path maybe a filesystem path, an URL or any other
  ///        location address that can be represented as a character string.
  class urn_to_path
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
  {
  public:

    /// Default constructor
    urn_to_path();

    /// Constructor with explicit path segments
    urn_to_path(const std::string & urn_,
		const std::string & path_,
		const std::string & category_,
		const std::string & mime_);

    /// Destructor
    virtual ~urn_to_path();

    /// Check validity
    bool is_valid() const;

    /// Reset
    void reset();

    /// Main interface method for smart dump
    virtual void tree_dump (std::ostream & out_ = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const;

    bool has_urn() const;
    void set_urn(const std::string &);
    const std::string & get_urn() const;

    bool has_path() const;
    void set_path(const std::string &);
    const std::string & get_path() const;

    bool has_category() const;
    void set_category(const std::string &);
    const std::string & get_category() const;

    bool has_mime() const;
    void set_mime(const std::string &);
    const std::string & get_mime() const;

  private:

    std::string _urn_;      //!< The URN representation of an object
    std::string _path_;     //!< The path associated to the URN which locate the object
    std::string _category_; //!< The category of the object (optional)
    std::string _mime_;     //!< The MIME type (optional)

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
