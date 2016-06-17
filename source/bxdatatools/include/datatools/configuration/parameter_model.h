/// \file datatools/configuration/parameter_model.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-05-11
 * Last modified : 2014-07-18
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
 *
 * Description:
 *
 *   The model of a configuration parameter.
 *
 * History:
 *
 */

#ifndef DATATOOLS_CONFIGURATION_PARAMETER_MODEL_H
#define DATATOOLS_CONFIGURATION_PARAMETER_MODEL_H

// Standard library:
#include <string>
#include <set>
#include <vector>
#include <map>

// Third party:

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

    /// \brief Description/model of a configuration parameter
    class parameter_model : public datatools::enriched_base
    {
    public:

      /// Check if a name is valid
      virtual bool is_name_valid(const std::string & name_) const;

      /// Dictionary of variant physicals
      typedef std::map<std::string, variant_physical> variant_dict_type;

      /// \brief Parameter mode
      enum mutability_type {
        MUTABILITY_UNDEFINED = 0, //!< Undefined mutability trait
        MUTABILITY_FIXED     = 1, //!< Parameter is fixed (no mutability)
        MUTABILITY_VARIABLE  = 2, //!< Parameter is variable (mutability)
        MUTABILITY_DEFAULT   = MUTABILITY_VARIABLE  //!< Default mutability trait
      };

      /// Return the label associated to the mutability
      static std::string label_from_mutability(mutability_type);

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
      static std::string label_from_variable_mode(variable_mode_type);

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
      static std::string label_from_path_io(path_io_type);

      /// Return the path I/O mode associated to a label
      static path_io_type path_io_from_label(const std::string &);

      /// Constructor
      parameter_model();

      /// Destructor
      virtual ~parameter_model();

      /// Check if the documentation is empty
      bool has_documentation() const;

      /// Set the documentation
      void set_documentation(const std::string & documentation_);

      /// Return the documentation
      const std::string & get_documentation() const;

      /// Check if type is defined
      bool has_type() const;

      /// Set the type of the parameter
      void set_type(basic_type);

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
      void set_path(bool);

      /// Reset the path flag
      void reset_path();

      /// Check if path I/O mode is defined
      bool has_path_io() const;

      /// Set the path I/O
      void set_path_io(path_io_type);

      /// Check path input flag
      bool is_path_input() const;

      /// Check path output flag
      bool is_path_output() const;

      /// Check path output flag
      bool is_path_input_output() const;

      /// Set the type and variable mode of the parameter
      void set(basic_type, variable_mode_type);

      /// Check if variable mode is defined
      bool has_variable_mode() const;

      /// Set the mode of the parameter
      void set_variable_mode(variable_mode_type);

      /// Return the mode of the parameter
      variable_mode_type get_variable_mode() const;

      /// Check mutability
      bool has_mutability() const;

      /// Set the mutability trait
      void set_mutability(mutability_type);

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
      void set_fixed_boolean(bool);

      /// Set fixed integer value
      void set_fixed_integer(int);

      /// Set fixed real value
      void set_fixed_real(double);

      /// Set fixed string value
      void set_fixed_string(const std::string &);

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
      void set_default_boolean(bool);

      /// Return the default boolean
      bool get_default_boolean() const;

      /// Set default integer value
      void set_default_integer(int);

      /// Return the default integer
      int get_default_integer() const;

      /// Check the real precision
      bool has_real_precision() const;

      /// Set the real precision
      void set_real_precision(double prec_);

      /// Reset the real precision
      void reset_real_precision();

      /// Return the real precision
      double get_real_precision() const;

      /// Set default real value
      void set_default_real(double);

      /// Return the default real
      double get_default_real() const;

      /// Set default string value
      void set_default_string(const std::string &);

      /// Return the default string
      const std::string & get_default_string() const;

      /// Check the real unit label
      bool has_real_unit_label() const;

      /// Set the unit label
      void set_real_unit_label(const std::string &);

      /// Return the unit label
      const std::string & get_real_unit_label() const;

      /// Check the real unit symbol
      bool has_real_preferred_unit() const;

      /// Set the unit symbol
      void set_real_preferred_unit(const std::string &);

      /// Return the unit symbol
      const std::string & get_real_preferred_unit() const;

      /// Fetch the display unit symbol
      bool fetch_real_display_unit(std::string & display_unit_symbol_, double & unit_value_) const;

      /// Fetch the list of enumerated string values
      void fetch_string_enumeration(std::vector<std::string> &) const;

      /// Fetch the list of enumerated real values
      void fetch_real_enumeration(std::vector<double> &) const;

      /// Fetch the list of enumerated integer values
      void fetch_integer_enumeration(std::vector<int> &) const;

      /// Check enumerated integer value
      bool has_enumerated_value_integer(int) const;

      /// Add an enumerated integer value
      void add_enumerated_value_integer(int, bool default_ = false);

      /// Check enumerated real value
      bool has_enumerated_value_real(double) const;

      /// Find the matching enumerated real value taking into account the precision if any
      bool find_enumerated_value_real(double value_, double & enum_value_) const;

      /// Add an enumerated real value
      void add_enumerated_value_real(double, bool default_ = false);

      /// Check enumerated string value
      bool has_enumerated_value_string(const std::string &) const;

      /// Add an enumerated string value
      void add_enumerated_value_string(const std::string &, bool default_ = false);

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
      void associate_variant_to_boolean(bool value_, const std::string & variant_name_);

      /// Unlink the variant associated to a boolean value
      void unlink_variant_from_boolean(bool value_);

      /// Associate a variant to an enumerated integer value
      void associate_variant_to_integer(int value_, const std::string & variant_name_);

      /// Unlink the variant associated to an integer value
      void unlink_variant_from_integer(int value_);

      /// Associate a variant to an integer range
      void associate_variant_to_integer_range(const integer_range & range_, const std::string & variant_name_);

      /// Unlink the variant associated to an integer range
      void unlink_variant_from_integer_range(const integer_range & range_);

      /// Associate a variant to an enumerated real value
      void associate_variant_to_real(double value_, const std::string & variant_name_);

      /// Unlink the variant associated to a real value
      void unlink_variant_from_real(double value_);

      /// Associate a variant to an real range
      void associate_variant_to_real_range(const real_range & range_, const std::string & variant_name_);

      /// Unlink the variant associated to an real range
      void unlink_variant_from_real_range(const real_range & range_);

      /// Associate a variant to an enumerated string value
      void associate_variant_to_string(const std::string & value_, const std::string & variant_name_);

      /// Unlink the variant associated to a string value
      void unlink_variant_from_string(const std::string & value_);

      /// Check initialization status
      bool is_initialized() const;

      /// Initialization from properties and a dictionary of configuration items
      void initialize(const properties & setup_, item_dict_type & items_);

      /// Initialization from properties only
      void initialize_standalone(const properties &);

      /// Bare initialization
      void initialize_simple();

      /// Reset
      void reset();

      /// Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

      /// Check if a boolean value is valid
      bool is_boolean_valid(bool) const;

      /// Check if an integer value is valid
      bool is_integer_valid(int) const;

      /// Check if a real value is valid
      bool is_real_valid(double) const;

      /// Check if a string value is valid
      bool is_string_valid(const std::string &) const;

      /// Return a reference to the list of variants
      const variant_dict_type & get_variants() const;

      /// Find the name of the variant associated to a given boolean value
      bool find_variant_associated_to_boolean(bool, std::string &) const;

      /// Find the name of the variant associated to a given integer value
      bool find_variant_associated_to_integer(int, std::string &) const;

      /// Find the name of the variant associated to a given real value
      bool find_variant_associated_to_real(double, std::string &) const;

      /// Find the name of the variant associated to a given string value
      bool find_variant_associated_to_string(const std::string &, std::string &) const;

      /// OCD support
      static void init_ocd(datatools::object_configuration_description &);

      enum rst_flags {
        PRINT_RST_NO_TITLE = datatools::bit_mask::bit00,
        PRINT_RST_NO_DESC  = datatools::bit_mask::bit01
     };

      /// Print in ReST format
      void print_rst(std::ostream & out_,
                     const std::string & indent_ = "",
                     uint32_t flags_ = 0) const;

    protected:

      /// Set the mode
      void _set_variable_mode(variable_mode_type);

      /// Set default values
      void _set_default();

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

      // Boolean property:
      bool _boolean_default_;      //!< Default boolean value
      bool _boolean_fixed_;        //!< Fixed boolean value
      std::string _false_variant_; //!< Label of the variant associated to the false value
      std::string _true_variant_;  //!< Label of the variant associated to the true value

      // Integer property:
      integer_range _integer_domain_; //!< Domain of accepted values
      int _integer_default_;          //!< Default integer value
      int _integer_fixed_;            //!< Fixed integer value
      std::map<int, std::string> _integer_enumeration_;   //!< Enumeration of supported integer values associated to specific configuration variants
      std::map<integer_range, std::string> _integer_r2v_; //!< List of integer ranges associated to specific configuration variants

      // Real property:
      std::string _real_unit_label_;     //!< Label of the associated unit ("length", "time"...)
      std::string _real_preferred_unit_; //!< Symbol of the preferred unit
      double _real_precision_;  //!< Precision on real values
      double _real_default_;    //!< Default real value
      double _real_fixed_;      //!< Fixed real value
      real_range _real_domain_; //!< Domain of accepted values
      std::map<double, std::string> _real_enumeration_; //!< Enumeration of supported real values associated to specific configuration variants
      std::map<real_range, std::string> _real_r2v_;     //!< List of real ranges associated to specific configuration variants

      // String property:
      std::string _string_default_; //!< Default string value
      std::string _string_fixed_;   //!< Fixed string value
      bool _path_; //!< Path flag
      path_io_type _path_io_; //!< Path I/O mode
      std::map<std::string, std::string> _string_enumeration_; //!< Enumeration of supported string values associated to specific configuration variants

    };

  }  // end of namespace configuration

}  // end of namespace datatools

// OCD support:
#include <datatools/ocd_macros.h>

/// OCD registration
/// @arg datatools::configuration::parameter the name of the class
DOCD_CLASS_DECLARATION(datatools::configuration::parameter_model)

#endif // DATATOOLS_CONFIGURATION_PARAMETER_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
