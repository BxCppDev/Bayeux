/// \file datatools/configuration/parameter_model.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-05-11
 * Last modified : 2016-10-31
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
 * Foundation, Inc. 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   The model of a configuration variant parameter.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_PARAMETER_MODEL_H
#define DATATOOLS_CONFIGURATION_PARAMETER_MODEL_H

// Standard library:
#include <string>
#include <set>
#include <vector>
#include <map>

// This project (Bayeux/datatools)
#include <datatools/i_tree_dump.h>
#include <datatools/enriched_base.h>
#include <datatools/types.h>
#include <datatools/configuration/utils.h>
#include <datatools/object_configuration_description.h>
#include <datatools/integer_range.h>
#include <datatools/real_range.h>

namespace datatools {

  // Forward declaration:
  class multi_properties;

  namespace configuration {

    // Forward declaration:
    class variant_model;
    class variant_physical;
    class variant_repository;

    /// \brief Description/model of a configuration parameter
    class parameter_model : public datatools::enriched_base
    {
    public:

      /// \brief Special flag for RestructuredText formatting
      enum rst_flags {
        PRINT_RST_NO_TITLE = datatools::bit_mask::bit00,
        PRINT_RST_NO_DESC  = datatools::bit_mask::bit01
      };

      /// Check if a name is valid
      bool is_name_valid(const std::string & name_) const override;

      /// Dictionary of variant physicals
      typedef std::map<std::string, variant_physical> variant_dict_type;

      /// Dictionary of groups
      typedef std::map<std::string, std::string> group_dict_type;

      /// \brief Parameter mode
      enum mutability_type {
        MUTABILITY_UNDEFINED = 0, //!< Undefined mutability trait
        MUTABILITY_FIXED     = 1, //!< Parameter is fixed (no mutability)
        MUTABILITY_VARIABLE  = 2, //!< Parameter is variable (mutability)
        MUTABILITY_DEFAULT   = MUTABILITY_VARIABLE  //!< Default mutability trait
      };

      /// Return the label associated to the mutability
      static std::string label_from_mutability(const mutability_type);

      /// Return the mutability associated to the mutability label
      static mutability_type mutability_from_label(const std::string &);

      /// \brief Parameter variable mode
      enum variable_mode_type {
        VARIABLE_MODE_UNDEFINED =   0, //!< Undefined variable mode
        VARIABLE_MODE_FREE      =   1, //!< Parameter can have a free value
        VARIABLE_MODE_ENUM      =   2, //!< Parameter can have values from an enumeration
        VARIABLE_MODE_INTERVAL  =   4, //!< Parameter can have values in an interval (if applicable)
        VARIABLE_MODE_DEFAULT   = 255  //!< Default variable mode
      };

      /// Return the label associated to a variable mode
      static std::string label_from_variable_mode(const variable_mode_type);

      /// Return the variable mode associated to a label
      static variable_mode_type variable_mode_from_label(const std::string &);

      /// Return the prefix for auxialiary properties
      static const std::string & aux_prefix();

      /// \brief Path I/O flags
      enum path_io_type {
        PATH_UNDEFINED    = 0x0, //!< Undefined I/O flag
        PATH_INPUT        = 0x1, //!< Flag for input file
        PATH_OUTPUT       = 0x2, //!< Flag for output file
        PATH_INPUT_OUTPUT = 0x3  //!< Flag for input & output file
      };

      /// Return the label associated to a path I/O mode
      static std::string label_from_path_io(const path_io_type);

      /// Return the path I/O mode associated to a label
      static path_io_type path_io_from_label(const std::string &);

      /// \brief Base enumerated item metadata
      ///
      /// This contains some metadata associated to an enumeration item.
      /// The item itself can be a single value or a set of values (i.e. interval).
      struct base_enum_metadata
        : public datatools::i_tree_dumpable
      {
      public:

        /// \brief Constructor flags
        enum ctor_flags {
          CTOR_NO_GROUP_SUPPORT = datatools::bit_mask::bit00 //!< No group support bit
        };

        /// Constructor
        base_enum_metadata(uint32_t flags_ = 0);

        /// Check if documentation is set
        bool has_documentation() const;

        /// Set documentation
        void set_documentation(const std::string &);

        /// Return documentation
        const std::string & get_documentation() const;

        /// Check is some variants are set
        bool has_variants() const;

        /// Check is a variant is set, given its name
        bool has_variant(const std::string & variant_name_) const;

        /// Add a variant, given its name
        void add_variant(const std::string & variant_name_);

        /// Remove a variant, given its name
        void remove_variant(const std::string & variant_name_);

        /// Remove all variants
        void remove_variants();

        /// Return the set of variants
        const std::set<std::string> & get_variants() const;

        /// Check if group is set
        bool has_group() const;

        /// Set group
        void set_group(const std::string & group_name_);

        /// Return the group
        const std::string & get_group() const;

        /// Check if a group name matches the group set
        bool match_group(const std::string & group_name_) const;

        /// Initialization
        void initialize(const datatools::properties & config_,
                        parameter_model & parmod_);

        /// Reset
        void reset();

        /// Smart print
        void tree_dump(std::ostream & out_ = std::clog,
                               const std::string & title_ = "",
                               const std::string & indent_ = "",
                               bool inherit_ = false) const override;

        /// \brief Restructured text formatting
        enum rst_flags {
          PRINT_RST_NO_DOC = datatools::bit_mask::bit00
        };

        /// Print in RestructuredText format
        void print_rst(std::ostream & out_,
                       const std::string & indent_ = "",
                       const uint32_t flags_ = 0) const;

        /// Check if group support is set
        bool has_group_support() const;

      protected:

        std::string _documentation;      //!< Terse documentation string associated to the enumerated item
        std::set<std::string> _variants; //!< Set of variants associated to /triggered by the value
        std::string _group;              //!< Group the value belongs to

      private:
        bool _no_group_support_ = false;
      };

      /// \brief Boolean enumeration support
      struct boolean_enum_value_metadata : public base_enum_metadata  {
        boolean_enum_value_metadata();
      };

      /// \brief Integer enumeration support
      struct integer_enum_value_metadata : public base_enum_metadata  {
      };
      typedef std::map<int, integer_enum_value_metadata> integer_enum_dict_type;

      /// \brief Integer range support
      struct integer_range_enum_metadata : public base_enum_metadata {
      };
      typedef std::map<integer_range, integer_range_enum_metadata> integer_range_enum_dict_type;

      /// \brief Real enumeration support
      struct real_enum_value_metadata : public base_enum_metadata  {
      };
      typedef std::map<double, real_enum_value_metadata> real_enum_dict_type;

      /// \brief String enumeration support
      struct string_enum_value_metadata : public base_enum_metadata {
      };
      typedef std::map<std::string, string_enum_value_metadata> string_enum_dict_type;

      /// \brief Real range support
      struct real_range_enum_metadata : public base_enum_metadata {
      };
      typedef std::map<real_range, real_range_enum_metadata> real_range_enum_dict_type;

      /// Constructor
      parameter_model();

      /// Destructor
      ~parameter_model() override;

      /// Check if the documentation is empty
      bool has_documentation() const;

      /// Set the documentation
      void set_documentation(const std::string & documentation_);

      /// Return the documentation
      const std::string & get_documentation() const;

      /// Check if type is defined
      bool has_type() const;

      /// Set the type of the parameter
      void set_type(const basic_type type_);

      /// Return the type of the parameter
      basic_type get_type() const;

      /// Check boolean type
      bool is_boolean() const;

      /// Check integer type
      bool is_integer() const;

      /// Check real type
      bool is_real() const;

      /// Check string type
      bool is_string() const;

      /// Check path string
      bool is_path() const;

      /// Set the path flag
      void set_path(const bool is_path_);

      /// Reset the path flag
      void reset_path();

      /// Check if path I/O mode is defined
      bool has_path_io() const;

      /// Set the path I/O
      void set_path_io(const path_io_type path_io_);

      /// Check path input flag
      bool is_path_input() const;

      /// Check path output flag
      bool is_path_output() const;

      /// Check path output flag
      bool is_path_input_output() const;

      /// Set the type and variable mode of the parameter
      void set(const basic_type type_, const variable_mode_type variable_mode_);

      /// Check if variable mode is defined
      bool has_variable_mode() const;

      /// Set the mode of the parameter
      void set_variable_mode(const variable_mode_type variable_mode_);

      /// Return the mode of the parameter
      variable_mode_type get_variable_mode() const;

      /// Check mutability
      bool has_mutability() const;

      /// Set the mutability trait
      void set_mutability(const mutability_type mutability_);

      /// Get mutability
      mutability_type get_mutability() const;

      /// Check fixed trait
      bool is_fixed() const;

      /// Check mutability trait
      bool is_variable() const;

      /// Check fixed value
      bool has_fixed_value() const;

      /// Return the fixed boolean
      bool get_fixed_boolean() const;

      /// Return the fixed integer
      int get_fixed_integer() const;

      /// Return the fixed real
      double get_fixed_real() const;

      /// Return the fixed string
      const std::string & get_fixed_string() const;

      /// Set fixed boolean value
      void set_fixed_boolean(const bool value_);

      /// Set fixed integer value
      void set_fixed_integer(const int value_);

      /// Set fixed real value
      void set_fixed_real(const double value_);

      /// Set fixed string value
      void set_fixed_string(const std::string & value_);

      /// Check free mode
      bool is_free() const;

      /// Check interval mode
      bool is_interval() const;

      /// Check enumeration mode
      bool is_enumeration() const;

      /// Check the integer domain
      bool has_integer_domain() const;

      /// Set integer interval
      void set_integer_domain(const integer_range & ir_);

      /// Return the integer domain
      const integer_range & get_integer_domain() const;

      /// Check the real domain
      bool has_real_domain() const;

      // Set real interval
      void set_real_domain(const real_range & ir_);

      /// Return the real domain
      const real_range & get_real_domain() const;

      /// Check default value
      bool has_default_value() const;

      /// Reset the default value
      void reset_default_value();

      /// Set default boolean value
      void set_default_boolean(const bool value_);

      /// Return the default boolean
      bool get_default_boolean() const;

      /// Set default integer value
      void set_default_integer(const int value_);

      /// Return the default integer
      int get_default_integer() const;

      /// Check the real precision
      bool has_real_precision() const;

      /// Set the real precision
      void set_real_precision(const double prec_);

      /// Reset the real precision
      void reset_real_precision();

      /// Return the real precision
      double get_real_precision() const;

      /// Return the real effective precision
      double get_real_effective_precision() const;

      /// Set default real value
      void set_default_real(const double value_);

      /// Return the default real
      double get_default_real() const;

      /// Set default string value
      void set_default_string(const std::string & value_);

      /// Return the default string
      const std::string & get_default_string() const;

      /// Check the real unit label
      bool has_real_unit_label() const;

      /// Set the unit label
      void set_real_unit_label(const std::string & unit_label_);

      /// Return the unit label
      const std::string & get_real_unit_label() const;

      /// Check the real unit symbol
      bool has_real_preferred_unit() const;

      /// Set the unit symbol
      void set_real_preferred_unit(const std::string & value_);

      /// Return the unit symbol
      const std::string & get_real_preferred_unit() const;

      /// Fetch the display unit symbol
      bool fetch_real_display_unit(std::string & display_unit_symbol_, double & unit_value_) const;

      //
      // Boolean
      //

      /// Set the metadata associated to a boolean value
      void set_enumerated_boolean_value_metadata(const bool value_, const boolean_enum_value_metadata & metadata_);

      /// Return the metadata associated to a boolean value
      const boolean_enum_value_metadata & get_enumerated_boolean_value_metadata(const bool value_) const;

      /// Return the metadata associated to a boolean value
      boolean_enum_value_metadata & grab_enumerated_boolean_value_metadata(const bool value_);

      /// Check if an enumerated boolean value has a group
      bool enumerated_boolean_value_has_group(const bool value_) const;

      /// Return the group associated to an enumerated boolean value
      const std::string & get_enumerated_boolean_value_group(const bool value_) const;

      /// Build the list of groups associated to enumerated boolean value
      void build_list_of_enumerated_boolean_groups(std::set<std::string> & groups_) const;

      /// Build the list of enumerated boolean values associated to a given group
      void build_list_of_enumerated_boolean_values_in_group(const std::string & group_,
                                                            std::set<bool> & values_) const;

      /// Build the list of enumerated boolean values
      void build_list_of_enumerated_boolean_values(std::set<bool> & values_) const;

      //
      // Integer enumerated
      //

      /// Add an enumerated integer value
      void add_enumerated_integer_value(const int value_,
                                        const integer_enum_value_metadata & metadata,
                                        const bool default_ = false);

      /// Add an enumerated integer value with its metadata
      void add_enumerated_integer_value(const int value_,
                                        const bool default_ = false);

      /// Check enumerated integer value
      bool has_enumerated_integer_value(const int value_) const;

      /// Fetch the list of enumerated integer values
      void fetch_integer_enumeration(std::vector<int> & values_) const;

      /// Check if an enumerated integer value has a group
      bool enumerated_integer_value_has_group(const int value_) const;

      /// Return the group associated to an enumerated integer value
      const std::string & get_enumerated_integer_value_group(const int value_) const;

      /// Return the metadata associated to an enumerated integer value
      const integer_enum_value_metadata & get_enumerated_integer_value_metadata(const int value_) const;

      /// Return the metadata associated to an enumerated integer value
      integer_enum_value_metadata & grab_enumerated_integer_value_metadata(const int value_);

      /// Build the list of groups associated to enumerated integer value
      void build_list_of_enumerated_integer_groups(std::set<std::string> & groups_) const;

      /// Build the list of enumerated integer values associated to a given group
      void build_list_of_enumerated_integer_values_in_group(const std::string & group_,
                                                            std::set<int> & values_) const;

      /// Build the list of enumerated integer values
      void build_list_of_enumerated_integer_values(std::set<int> & values_) const;

      //
      // Real enumerated
      //

      /// Check enumerated real value
      bool has_enumerated_real_value(const double value_) const;

      /// Find the matching enumerated real value taking into account the precision if any
      bool find_enumerated_real_value(const double value_, double & enum_value_) const;

      /// Add an enumerated real value with its metadata
      void add_enumerated_real_value(const double value_,
                                     const real_enum_value_metadata & metadata_,
                                     const bool default_ = false);

      /// Add an enumerated real value
      void add_enumerated_real_value(const double value_,
                                     const bool default_ = false);

      /// Check if an enumerated real value has a group
      bool enumerated_real_value_has_group(const double value_) const;

      /// Return the group associated to an enumerated real value
      const std::string & get_enumerated_real_value_group(const double value_) const;

      /// Return the metadata associated to an enumerated real value
      const real_enum_value_metadata & get_enumerated_real_value_metadata(const double value_) const;

      /// Return the metadata associated to an enumerated real value
      real_enum_value_metadata & grab_enumerated_real_value_metadata(const double value_);

      /// Fetch the list of enumerated real values
      void fetch_real_enumeration(std::vector<double> & values_) const;

      /// Build the list of groups associated to enumerated reals
      void build_list_of_enumerated_real_groups(std::set<std::string> & groups_) const;

      /// Build the list of enumerated real values associated to a given group
      void build_list_of_enumerated_real_values_in_group(const std::string & group_,
                                                         std::set<double> & values_) const;

      /// Build the list of enumerated integer values
      void build_list_of_enumerated_real_values(std::set<double> & values_) const;

      //
      // String enumerated
      //

      /// Check enumerated string value
      bool has_enumerated_string_value(const std::string & value_) const;

      /// Add an enumerated string value with its metadata
      void add_enumerated_string_value(const std::string & value_,
                                       const string_enum_value_metadata & metadata_,
                                       const bool default_ = false);

      /// Check if an enumerated string value has a group
      bool enumerated_string_value_has_group(const std::string & value_) const;

      /// Return the group associated to an enumerated string value
      const std::string & get_enumerated_string_value_group(const std::string & value_) const;

      /// Add an enumerated string value
      void add_enumerated_string_value(const std::string & value_,
                                       const bool default_ = false);

      /// Return the metadata associated to an enumerated string value
      const string_enum_value_metadata & get_enumerated_string_value_metadata(const std::string & value_) const;

      /// Return the metadata associated to an enumerated string value
      string_enum_value_metadata & grab_enumerated_string_value_metadata(const std::string & value_);

      /// Build the list of groups associated to enumerated strings
      void build_list_of_enumerated_string_groups(std::set<std::string> & groups_) const;

      /// Build the list of enumerated string values associated to a given group
      void build_list_of_enumerated_string_values_in_group(const std::string & group_,
                                                           std::set<std::string> & values_) const;

      /// Build the list of enumerated string values
      void build_list_of_enumerated_string_values(std::set<std::string> & values_) const;

      //
      // Group

      /// Check if groups are defines
      bool has_groups() const;

      /// Check if a group exists
      bool has_group(const std::string & group_name_) const;

      /// Add a group
      void add_group(const std::string & group_name_,
                     const std::string & doc_ = "");

      /// Build the list of groups
      void build_set_of_groups(std::set<std::string> & group_) const;

      //
      // Variant
      //

      /// Check if a variant exists
      bool has_variant(const std::string & variant_name_) const;

      /// Add a variant with its model and description string
      void add_variant(const std::string &    variant_name_,
                       const vm_handle_type & variant_model_handle_,
                       const std::string &    description_ = "");

      /// Remove a variant
      void remove_variant(const std::string & variant_name_);

      /// Return a description string associated to a given variant
      const std::string & get_variant_description(const std::string & variant_name_) const;

      /// Return a const reference to a variant model associated to a given variant
      const variant_model & get_variant_model(const std::string & variant_name_) const;

      /// Return a variant model handle associated to a given variant
      vm_handle_type get_variant_model_handle(const std::string & variant_name_) const;

      // Variant associations:

      /// Associate a variant to an enumerated boolean value
      void associate_variant_to_boolean(const bool value_, const std::string & variant_name_);

      /// Associate a variant to an enumerated integer value
      void associate_variant_to_integer(const int value_, const std::string & variant_name_);

      /// Associate a variant to an enumerated real value
      void associate_variant_to_real(const double value_, const std::string & variant_name_);

      /// Associate a variant to an enumerated string value
      void associate_variant_to_string(const std::string & value_,
                                       const std::string & variant_name_);

      //
      // Management
      //

      /// Check initialization status
      bool is_initialized() const;

      /// Initialization from properties and a dictionary of configuration items
      void initialize(const properties & setup_, model_item_dict_type & items_);

      /// Initialization from properties only
      void initialize_standalone(const properties &);

      /// Bare initialization
      void initialize_simple();

      /// Reset
      void reset();

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

      /// Check if a boolean value is valid
      bool is_boolean_valid(const bool) const;

      /// Check if an integer value is valid
      bool is_integer_valid(const int) const;

      /// Check if a real value is valid
      bool is_real_valid(const double) const;

      /// Check if a string value is valid
      bool is_string_valid(const std::string &) const;

      /// Return a reference to the list of variants
      const variant_dict_type & get_variants() const;

      /// Find the set of variants associated to a given boolean value
      bool find_variants_associated_to_boolean(const bool value_, std::set<std::string> & variants_) const;

      /// Find the set of variants associated to a given integer value
      bool find_variants_associated_to_integer(const int value_, std::set<std::string> & variants_) const;

      /// Find the name of the variant associated to a given real value
      bool find_variants_associated_to_real(const double value_, std::set<std::string> & variants_) const;

      /// Find the name of the variant associated to a given string value
      bool find_variants_associated_to_string(const std::string & value_, std::set<std::string> & variants_) const;

      //
      // Ranges of interest (available for integer and real parameters)
      //

      /// Associate a variant to an integer range
      void associate_variant_to_integer_range(const integer_range & range_,
                                              const std::string & variant_name_);

      /// Associate a variant to an real range
      void associate_variant_to_real_range(const real_range & range_,
                                           const std::string & variant_name_);

      /// Add a integer range
      void add_integer_range(const integer_range &, const integer_range_enum_metadata &);

      /// Add a real range
      void add_real_range(const real_range &, const real_range_enum_metadata &);

      /// Return the dictionary of integer ranges
      const integer_range_enum_dict_type & get_integer_ranges() const;

      /// Return the dictionary ofreal ranges
      const real_range_enum_dict_type & get_real_ranges() const;

      //
      // Miscellaneous
      //

      /// OCD support
      static void init_ocd(datatools::object_configuration_description &);

      /// Print in ReST format
      void print_rst(std::ostream & out_,
                     const std::string & indent_ = "",
                     const uint32_t flags_ = 0) const;

    protected:

      /// Set the mode
      void _set_variable_mode(const variable_mode_type);

      /// Set default values
      void _set_default();

      /// Load a list of strings (in enumeration variable mode) from a file
      /// using a CSV format:
      /// - one "value" per line with optional "group" separated with a colon ':'
      /// - commented lines start with a sharp '#'
      /// - blank lines are ignored
      /// - a value with a leading star '*' is used as the default value
      /// \code
      /// *Foo  : Description : group1 : variant0
      /// Bar   : Description : group2
      /// Baz   : Description :        : variant1
      /// John  : Description : group1 : variant2
      /// Cindy : Description : group3 : variant1;variant2
      /// \endcode
      void _parse_string_enumeration_from_csv_file(const std::string & csv_filename_,
                                                   const char csv_separator_ = ':',
                                                   const char csv_subseparator_ = ';',
                                                   const std::string & value_regex_ = "",
                                                   const std::string & group_regex_ = "");

    private:

      /// Update real domain
      void _update_real_domain_();

    private:

      bool        _initialized_;   //!< Initialization flag
      std::string _documentation_; //!< Documentation

      // Basic traits:
      basic_type          _type_;              //!< Type of the parameter (boolean, integer, real, string)
      mutability_type     _mutability_;        //!< Parameter's mutability trait
      bool                _has_default_value_; //!< Flag for an existing default value
      bool                _has_fixed_value_;   //!< Flag for an existing fixed value (mandatory for 'fixed' mutability)
      variable_mode_type  _variable_mode_;     //!< Mode of the variable parameter (free, interval, enumeration)
      variant_dict_type   _variants_;          //!< Dictionary of variants that may be activated depending of the value of the parameter
      group_dict_type     _groups_;            //!< Dictionary of groups to which to values or ranges of values can be associated

      // Boolean property:
      bool _boolean_default_;      //!< Default boolean value
      bool _boolean_fixed_;        //!< Fixed boolean value
      boolean_enum_value_metadata _false_metadata_; //!< Metadata associated to the false value
      boolean_enum_value_metadata _true_metadata_;  //!< Metadata associated to the true value

      // Integer property:
      integer_range _integer_domain_; //!< Domain of accepted values
      int _integer_default_;          //!< Default integer value
      int _integer_fixed_;            //!< Fixed integer value
      integer_enum_dict_type _integer_enumeration_; //!< Enumeration of supported integer values associated to specific configuration variants
      integer_range_enum_dict_type _integer_ranges_; //!< List of integer ranges associated to specific configuration variants

      // Real property:
      std::string _real_unit_label_;     //!< Label of the associated unit ("length", "time"...)
      std::string _real_preferred_unit_; //!< Symbol of the preferred unit
      double _real_precision_;  //!< Precision on real values
      double _real_default_;    //!< Default real value
      double _real_fixed_;      //!< Fixed real value
      real_range _real_domain_; //!< Domain of accepted values
      real_enum_dict_type _real_enumeration_; //!< Enumeration of supported real values associated to specific configuration variants
      real_range_enum_dict_type _real_ranges_; //!< List of real ranges associated to specific configuration variants

      // String property:
      std::string  _string_default_; //!< Default string value
      std::string  _string_fixed_;   //!< Fixed string value
      bool         _path_;           //!< Path flag
      path_io_type _path_io_;        //!< Path I/O mode
      string_enum_dict_type _string_enumeration_; //!< Enumeration of supported string values associated to metadata

    };

  }  // end of namespace configuration

}  // end of namespace datatools

// OCD support:
#include <datatools/ocd_macros.h>

/// OCD registration
/// @arg datatools::configuration::parameter the name of the class
DOCD_CLASS_DECLARATION(datatools::configuration::parameter_model)

#endif // DATATOOLS_CONFIGURATION_PARAMETER_MODEL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
