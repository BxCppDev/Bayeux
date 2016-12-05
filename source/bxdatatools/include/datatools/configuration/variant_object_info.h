/// \file datatools/configuration/variant_object_info.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-10-27
 * Last modified : 2016-10-27
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A structure describing a variant object.
 *
 * History:
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_OBJECT_INFO_H
#define DATATOOLS_CONFIGURATION_VARIANT_OBJECT_INFO_H

// Standard library:
#include <string>

// This project:
#include <datatools/bit_mask.h>
#include <datatools/logger.h>

namespace datatools {

  namespace configuration {

    /// \brief Structure describing a variant object of interest which may have (dynamic) dependencies.
    ///
    /// Supported variant object types are:
    ///
    /// * a variant registry hosted by a variant repository
    /// * a variant hosted by a variant registry
    /// * a variant parameter hosted by a variant registry
    /// * a group of values defined associated to a variant parameter in a variant registry
    /// * a specific enumerated string value defined within a variant parameter in a variant registry
    ///
    /// Some of these objects is likely to be dependent of some dependee variant(s).
    /// \see variant_dependency
    class variant_object_info
    {

    public:

      /// \brief Object type:
      enum object_type {
        OBJECT_INVALID     = 0,      //!< Invalid object
        OBJECT_REGISTRY    = 1,      //!< Variant registry object
        OBJECT_VARIANT     = 2,      //!< Variant object
        OBJECT_PARAMETER   = 3,      //!< Variant object
        OBJECT_PARAMETER_VALUE_GROUP = 4,      //!< Variant value group object
        OBJECT_PARAMETER_ENUM_STRING_VALUE = 5 //!< Variant enumerated string value object
      };

      void set_registry_name(const std::string & registry_name_);

      void make_registry(const std::string & registry_name_);

      void make_global(const std::string & registry_name_);

      void make_variant(const std::string & registry_name_,
                        const std::string & variant_local_path_);

      void make_local_variant(const std::string & variant_local_path_);

      void make_parameter(const std::string & registry_name_,
                          const std::string & parameter_local_path_);

      void make_local_parameter(const std::string & parameter_local_path_);

      void make_parameter_value_group(const std::string & registry_name_,
                                      const std::string & param_local_path_,
                                      const std::string & group_name_);

      void make_local_parameter_value_group(const std::string & param_local_path_,
                                            const std::string & group_name_);

      void make_parameter_enum_string_value(const std::string & registry_name_,
                                            const std::string & param_local_path_,
                                            const std::string & value_);

      void make_local_parameter_enum_string_value(const std::string & param_local_path_,
                                                  const std::string & value_);

      bool is_valid() const;

      bool is_local() const;

      bool is_global() const;

      object_type get_type() const;

      bool is_registry() const;

      bool is_variant() const;

      bool is_parameter() const;

      bool is_parameter_value_group() const;

      bool is_parameter_enum_string_value() const;

      const std::string & get_registry_name() const;

      const std::string & get_local_path() const;

      const std::string & get_variant_local_path() const;

      const std::string & get_parameter_local_path() const;

      const std::string & get_parameter_value_group_name() const;

      const std::string & get_parameter_enum_string_value() const;

      /// Reset
      void reset();

      /// \brief Parsing flags
      enum parse_flags {
        PARSE_DEBUG     = datatools::bit_mask::bit00,
        PARSE_NO_GLOBAL = datatools::bit_mask::bit01,
        PARSE_NO_LOCAL  = datatools::bit_mask::bit02
      };

      /// Parse from a string
      bool parse_from_string(const std::string & repr_, uint32_t flags_ = 0);

      /// Convert to a string
      void to_string(std::string & repr_) const;

      /// Stream
      friend std::ostream & operator<<(std::ostream &, const variant_object_info &);

      /// Return the full path
      std::string get_full_path() const;

      /// Check compatibility with a given path
      bool match_path(const std::string & path_) const;

    protected:

      bool _parse_from_string_global(const std::string & repr_, const datatools::logger::priority logging_);

      bool _parse_from_string_local(const std::string & repr_, const datatools::logger::priority logging_);

    private:

      object_type _type_ = OBJECT_INVALID;       //!< Type of variant object
      std::string _registry_name_;               //!< The name of the variant registry
      std::string _local_path_;                  //!< The local path of the object in its variant registry
      std::string _parameter_value_group_name_;  //!< The name of the group from its local path in its variant registry
      std::string _parameter_enum_string_value_; //!< The value of the enumerated string from its local path in its variant registry

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_OBJECT_INFO_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
