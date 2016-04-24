/// \file datatools/configuration/variant_record.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-09-22
 * Last modified : 2014-09-22
 *
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A record for a variant parameter or variant in a registry of
 *   configuration variants and associated parameters.
 *
 * History:
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_RECORD_H
#define DATATOOLS_CONFIGURATION_VARIANT_RECORD_H

// Standard library:
#include <string>
#include <map>
#include <vector>

// This project (Bayeux/datatools):
#include <datatools/configuration/utils.h>
#include <datatools/command_utils.h>
#include <datatools/enriched_base.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>

namespace datatools {

  namespace configuration {

    /// \brief Registry record
    class variant_record : public enriched_base
    {
    public:

      /// Array of pointers to daughter records
      typedef std::map<std::string, variant_record *> daughter_dict_type;

      /// Default constructor
      variant_record();

      /// Destructor
      virtual ~variant_record();

      /// Check if the record is valid
      bool is_valid() const;

      /// Check if the path is defined
      bool has_path() const;

      /// Set the path
      void set_path(const std::string &);

      /// Return the path
      const std::string & get_path() const;

      /// Return the leaf name
      std::string get_leaf_name() const;

      /// Check if the record has a parent
      bool has_parent() const;

      /// Set the parent record and register itself as a daughter of the parent record
      void set_parent(variant_record & parent_, const std::string & daughter_name_);

      /// Return the parent
      const variant_record & get_parent() const;

      /// Return the parent
      variant_record & grab_parent();

      /// Return the dictionary of daughter records
      daughter_dict_type & grab_daughters();

      /// Return the dictionary of daughter records
      const daughter_dict_type & get_daughters() const;

      /// Check if the record is associated to a parameter model
      bool has_parameter_model() const;

      /// Set the parameter model associated to the record
      void set_parameter_model(const parameter_model &);

      /// Return the parameter model associated to the record
      const parameter_model & get_parameter_model() const;

      /// Check if the record is associated to a variant model
      bool has_variant_model() const;

      /// Set the variant model associated to the record
      void set_variant_model(const variant_model &);

      /// Return the variant model associated to the record
      const variant_model & get_variant_model() const;

      /// Check if record is a variant
      bool is_variant() const;

      /// Check if record is a parameter
      bool is_parameter() const;

      /// Check if record is fixed (parameter)
      bool is_fixed_parameter() const;

      /// Set the active flag
      void set_active(bool);

      /// Check if record is active (parameter or variant)
      bool is_active() const;

      /// Check if a parameter value is set
      bool value_is_set() const;

      /// Check if a parameter value is set to default
      bool default_value_is_set() const;

      /// Set default value
      command::returned_info set_default_value();

      /// Set fixed value
      command::returned_info set_fixed_value();

      /// Set boolean value
      command::returned_info set_boolean_value(bool);

      /// Set integer value
      command::returned_info set_integer_value(int);

      /// Set real value
      command::returned_info set_real_value(double);

      /// Set string value
      command::returned_info set_string_value(const std::string &);

      /// Unset a parameter value
      command::returned_info unset_value();

      /// Check set value
      command::returned_info value_is_set(bool &) const;

      /// Get boolean value
      command::returned_info get_boolean_value(bool &) const;

      /// Get integer value
      command::returned_info get_integer_value(int &) const;

      /// Get real value
      command::returned_info get_real_value(double &) const;

      /// Get string value
      command::returned_info get_string_value(std::string &) const;

      /// Build a formatted string from the value
      command::returned_info value_to_string(std::string & format_) const;

      /// Set the value from a formatted string
      command::returned_info string_to_value(const std::string & format_);

      /// Reset the record
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

    protected:

      /// Update the status
      void _update();

    private:

      std::string             _path_;            //!< Full path of the record
      const parameter_model * _parameter_model_; //!< Handle to a parameter model
      const variant_model   * _variant_model_;   //!< Handle to a variant model
      variant_record *        _parent_;          //!< Parent record
      daughter_dict_type      _daughters_;       //!< Dictionary of pointers to daughter records
      bool        _active_;           //!< Active flag of the record
      // For parameters only:
      bool        _value_set_;        //!< Value set flag
      bool        _boolean_value_;    //!< Current boolean parameter value
      int32_t     _integer_value_;    //!< Current integer parameter value
      double      _real_value_;       //!< Current real parameter value
      std::string _string_value_;     //!< Current string parameter value

    };

  } // end of namespace configuration

} // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_RECORD_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
