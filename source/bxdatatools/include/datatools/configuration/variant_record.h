/// \file datatools/configuration/variant_record.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-09-22
 * Last modified : 2016-11-03
 *
 * Copyright (C) 2014-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/logger.h>

namespace datatools {

  namespace configuration {

    class variant_registry;

    /// \brief Variant registry record node
    class variant_record
    {
    public:

      // \brief Record for daughter variant record
      class daughter_type {
      public:
        daughter_type(const std::string & name_, variant_record & vrec_, const int rank_ = -1)
          : _name_(name_)
          , _handle_(&vrec_)
          , _rank_((rank_< 0) ? -1 : rank_) {}
        const std::string & get_name() const { return _name_; }
        const variant_record & get_record() const { return *_handle_;}
        variant_record & grab_record() { return *_handle_;}
        bool is_ranked() const { return _rank_ >= 0; }
        int get_rank() const { return _rank_; }
      private:
        std::string      _name_;
        variant_record * _handle_ = nullptr;
        int              _rank_ = -1;
      };

      /// Array of pointers to daughter records
      typedef std::map<std::string, daughter_type> daughter_dict_type;

      /// Default constructor
      variant_record();

      /// Destructor
      virtual ~variant_record();

      /// Return the logging priority threshold
      datatools::logger::priority get_logging() const;

      /// Set the logging priority threshold
      void set_logging(const datatools::logger::priority);

      /// Check if the record is valid
      bool is_valid() const;

      /// Check if the path is defined
      bool has_path() const;

      /// Set the path
      void set_path(const std::string &);

      /// Return the path
      const std::string & get_path() const;

      /// Set with update
      void set_with_update(bool);

      /// Check if the with update flag is set
      bool has_with_update() const;

      /// Return the leaf name
      std::string get_leaf_name() const;

      /// Check if the parent registry handle is set
      bool has_parent_registry() const;

      /// Set the parent registry handle
      void set_parent_registry(const variant_registry & rep_);

      /// Return the parent registry handle
      const variant_registry & get_parent_registry() const;

      /// Check if the parent repository
      bool has_parent_repository() const;

      /// Return the parent repository
      const variant_repository & get_parent_repository() const;

      /// Check if the record is a top variant
      bool is_top_variant() const;

      /// Check if the record has a parent
      bool has_parent() const;

      /// Set the parent record and register itself as a daughter of the parent record
      void set_parent(variant_record & parent_, const std::string & daughter_name_, int rank_ = -1);

      /// Return the parent
      const variant_record & get_parent() const;

      /// Return the parent
      variant_record & grab_parent();

      /// Check if the variant record has daughter records
      bool has_daughters() const;

      /// Return the dictionary of daughter records
      daughter_dict_type & grab_daughters();

      /// Return the dictionary of daughter records
      const daughter_dict_type & get_daughters() const;

      /// Return a daughter record given its name
      const variant_record & get_daughter(const std::string &) const;

      /// Return a daughter record given its name
      variant_record & grab_daughter(const std::string &);

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
      void set_active(bool active_);

      /// Check if record is active (parameter or variant)
      bool is_active() const;

      /// Check if a parameter value is set
      bool value_is_set() const;

      /// Check if a parameter value is set to default
      bool default_value_is_set() const;

      /// Check if a parameter is enabled
      bool check_enabled_parameter() const;

      /// Check if a value is enabled
      bool check_enabled_value(const parameter_value_type & value_) const;

      /// Check is a group is enabled
      bool check_enabled_group(const std::string & group_name_) const;

      /// Set default value
      command::returned_info set_default_value();

      /// Set fixed value
      command::returned_info set_fixed_value();

      /// Validate boolean value
      bool is_boolean_valid(const bool) const;

      /// Set boolean value
      command::returned_info set_boolean_value(bool);

      /// Validate integer value
      bool is_integer_valid(const int) const;

      /// Set integer value
      command::returned_info set_integer_value(int);

      /// Validate real value
      bool is_real_valid(const double) const;

      /// Set real value
      command::returned_info set_real_value(double);

      /// Validate string value
      bool is_string_valid(const std::string &) const;

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

      /// Build and finalize the internals of the record
      void build();

      /// Build the list of effectively ranked parameters
      /// The explicitely ranked params are listed first, then come
      /// unranked params in arbitrary order
      void build_list_of_ranked_parameter_records(std::vector<std::string> & ranked_) const;

      /// Reset the record
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

      /// Update
      void update();

      /// Force update
      void forced_update();

    private:

      // FUTURE: better to have this method
      // void _add_daughter_(variant_record &, name, rank... );

      ///  Recompute ranked/unranked daughters
      void _compute_ranked_unranked_daughters_();

      /// Update the status
      void _update_();

      /// Update the variant
      void _update_parameter_();

      /// Update the variant
      void _update_variant_();

      /// Fix
      void _fix_();

      /// Fix parameter value
      void _fix_parameter_value_();

      /// Fix dependers
      void _fix_dependers_on_this_variant_();

    private:

      // Management:
      datatools::logger::priority _logging_ = datatools::logger::PRIO_FATAL; //! Logging priority threshold
      bool                        _with_update_ = false;       //!< Update flag

      // Configuration:
      std::string              _path_;                      //!< Full path of the record
      const variant_registry * _parent_registry_ = nullptr; //!< Handle to the parent registry
      const parameter_model *  _parameter_model_ = nullptr; //!< Handle to a parameter model
      const variant_model   *  _variant_model_ = nullptr;   //!< Handle to a variant model
      variant_record *         _parent_ = nullptr;          //!< Parent record
      daughter_dict_type       _daughters_;                 //!< Dictionary of pointers to daughter records
      std::vector<std::string> _ranked_daughters_;          //!< Ranked daughters
      std::vector<std::string> _unranked_daughters_;        //!< Unranked daughters

      // Working data:
      bool        _active_ = false;    //!< Active flag of the record
      bool        _value_set_ = false; //!< Value set flag
      bool        _boolean_value_;     //!< Current boolean parameter value
      int32_t     _integer_value_;     //!< Current integer parameter value
      double      _real_value_;        //!< Current real parameter value
      std::string _string_value_;      //!< Current string parameter value

    };

  } // end of namespace configuration

} // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_RECORD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
