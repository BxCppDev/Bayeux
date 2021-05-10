/// \file datatools/properties.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-19
 * Last modified: 2019-01-17
 *
 * License: GPL3
 *
 * Copyright (C) 2008-2019 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A simple properties dictionary with I/O functionalities.
 *
 */

#ifndef DATATOOLS_PROPERTIES_H
#define DATATOOLS_PROPERTIES_H

// Standard Library:
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

// This Project:
#include <datatools/datatools_config.h>
#include <datatools/utils.h>
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/i_cloneable.h>
#include <datatools/bit_mask.h>
#include <datatools/exception.h>
#include <datatools/file_include.h>

namespace datatools {

  //! \brief A dictionary of arbitrary properties.
  /**
   *  The properties class provides a serializable container that holds
   *  many different data of different types (bool, int, double, string)
   *  as well as sized arrays (std::vector) of these build-in types.
   *
   * Additional features are available for some types of properties:
   *
   * - explicit support for units with real parameters
   * - string parameters possibly marked as filesystem path
   * - support for variant configuration parameter parsing
   * 
   * The properties class is provided with different I/O mechnisms:
   *
   * - Boost/Serialization I/O (text, binary, XML archives)
   * - I/O to/from plain ASCII files (with 'variant' support)
   *
   * Example:
   *
   * \code
   * // Instantiate a container of arbitrary parameters:
   * datatools::properties my_parameters;
   *
   * // Store properties:
   * my_parameters.store("test", true, "Test flag");
   * my_parameters.store("nitems", 42, "Number of items");
   * my_parameters.store("favorite_color", "Blue", "my favorite color");
   * my_parameters.store("ratio", 3.14159, "The Pi ratio");
   * my_parameters.store_with_explicit_unit("width", 12.34 * CLHEP::meter, "The width of the table");
   * my_parameters.set_unit_symbol("width", "mm");
   * std::vector<double> pos({ 1.2 * CLHEP::km, 3.4 * CLHEP::km});
   * my_parameters.store("position", pos, "The position on the 2D-map");
   * my_parameters.set_unit_symbol("position", "km");
   *
   * // Smart print:
   * std::cout << "My parameters: " << std::endl;
   * my_parameters.print_tree(std::cout);
   *
   * // Save as a file:
   * my_parameters.write_configuration("my_params.conf");
   *
   * // Fetch properties:
   * bool test = my_parameters.fetch_boolean("test");
   * uint32_t nitems = my_parameters.fetch_positive_integer("nitems");
   * std::string favorite_color = my_parameters.fetch_string("favorite_color");
   * double ratio = my_parameters.fetch_dimensionless_real("ratio");
   * double width = my_parameters.fetch_real_with_explicit_dimension("width", "length");
   *
   * // Instantiate another container:
   * datatools::properties my_parameters2;
   *
   * // Load it from the configuration file:
   * my_parameters2.read_configuration("my_params.conf");
   *
   * // Smart print:
   * std::cout << "My parameters: " << std::endl;
   * my_parameters.print_tree(std::cout);
   *
   * // Fetch properties:
   * bool test2 = my_parameters2.fetch_boolean("test");
   * 
   * \endcode
   * 
   */
  class properties
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
    , public datatools::i_clear
    , public datatools::i_cloneable
  {
  public:
    
    //----------------------------------------------------------------------
    //
    //! \brief Internal data stored within the dictionary of the properties class.
    class data {
      
    public:
      static const int ERROR_SUCCESS = 0;
      static const int ERROR_FAILURE = 1;
      static const int ERROR_BADTYPE = 2;
      static const int ERROR_RANGE   = 3;
      static const int ERROR_LOCK    = 4;

      static const uint8_t MASK_TYPE          = 0x7;  // = 00000111
      static const uint8_t MASK_UNIT_SYMBOL   = 0x8;  // = 00001000 for real parameters
      static const uint8_t MASK_EXPLICIT_PATH = 0x10; // = 00010000 for string parameters
      static const uint8_t MASK_EXPLICIT_UNIT = 0x20; // = 00100000 for real parameters
      static const uint8_t MASK_LOCK          = 0x40; // = 01000000
      static const uint8_t MASK_VECTOR        = 0x80; // = 10000000

      static const uint8_t TYPE_NONE    = 0x0; // = 000
      static const uint8_t TYPE_BOOLEAN = 0x1; // = 001
      static const uint8_t TYPE_INTEGER = 0x2; // = 010
      static const uint8_t TYPE_REAL    = 0x3; // = 011
      static const uint8_t TYPE_STRING  = 0x4; // = 100

      static const char TYPE_BOOLEAN_SYMBOL = 'B';
      static const char TYPE_INTEGER_SYMBOL = 'I';
      static const char TYPE_REAL_SYMBOL    = 'R';
      static const char TYPE_STRING_SYMBOL  = 'S';

      static const char STRING_FORBIDDEN_CHAR = '"';

      static const int SCALAR_DEF  = -1;
      static const int SCALAR_SIZE =  1;

      /// \brief Provides static method for default values for each supported type
      struct defaults {
        static bool boolean_value(); // return false
        static int integer_value();  // return 0
        static double real_value();  // return 0.0
        static std::string string_value(); // return empty string
      };

    public:
      
      typedef std::vector<bool>        vbool;   ///< Container for boolean data
      typedef std::vector<int32_t>     vint;    ///< Container for integer data
      typedef std::vector<double>      vdouble; ///< Container for real data
      typedef std::vector<std::string> vstring; ///< Container for character string data

    public:

      /// Constructor
      data(char type_ = TYPE_INTEGER_SYMBOL,
           int size_ = SCALAR_DEF);

      /// Constructor
      data(bool value_, int size_ = SCALAR_DEF);

      /// Constructor
      data(int value_, int size_ = SCALAR_DEF);

      /// Constructor
      data(double value_, int size_ = SCALAR_DEF);

      /// Constructor
      data(const std::string & value_, int size_ = SCALAR_DEF);

      /// Constructor
      data(const char* value_, int size_ = SCALAR_DEF);

      /// Destructor
      virtual ~data() = default;

      // Copy constructor
      data(const data &) = default;

      // Copy assignment
      data & operator=(const data &) = default;

      // Move construction
      data(data &&) = default;

      // Move assignment
      data & operator=(data &&) = default;

      /// Check is description is set
      bool has_description() const;

      /// Set the description string associated to the stored data
      void set_description(const std::string & description_);

      /// Get the description string associated to the stored data
      const std::string & get_description() const;

      /// Set the preferred unit symbol associated to the stored real data
      ///
      /// This method implies to set the *explicit unit* flag.
      int set_unit_symbol(const std::string & symbol_);

      /// Get the unit symbol associated to the stored real data
      const std::string & get_unit_symbol() const;

      /// Check if the data type is valid
      bool has_type() const;

      /// Return type
      int get_type() const;

      /// Check if the data is a boolean value
      bool is_boolean() const;

      /// Check if the data is a integer value
      bool is_integer() const;

      /// Check if the data is a real value
      bool is_real() const;

      /// Check if the data is a string value
      bool is_string() const;

      /// Check if the data is a path string value
      bool is_path() const;

      /// Check if the data is scalar (exactly one value)
      bool is_scalar() const;

      /// Check if the data is vector (>=0 stored values in an array)
      bool is_vector() const;

      /// Check if the data is locked (cannot be modified)
      bool is_locked() const;

      /// Check if the data has an unit symbol
      bool has_unit_symbol() const;

      /// Check if the (real only) data has been initialized with explicit unit
      bool has_explicit_unit() const;

      /// Check if the (string only) data has been initialized with explicit path
      bool is_explicit_path() const;

      /// Check if the data is not locked (can be modified)
      bool is_unlocked() const;

      /// Assign N boolean values
      int boolean(int size_ = SCALAR_DEF);

      /// Assign N integer values
      int integer(int size_ = SCALAR_DEF);

      /// Assign N real values
      int real(int size_ = SCALAR_DEF);

      /// Assign N string values
      int string(int size_ = SCALAR_DEF);

      /// Lock the value (make it unmutable)
      int lock();

      /// Unlock the value (make it mutable)
      int unlock();

      /// Returns the size of the array of stored values (1 if scalar, >=0 if vector)
      int32_t get_size() const;

      /// Returns the size of the array of stored values (1 if scalar, >=0 if vector)
      int32_t size() const;

      /// Check if the value array is empty
      bool empty() const;

      /// Check if array index is valid
      bool index_is_valid(int index_) const;

      /// Get the boolean value stored at a given rank
      bool get_boolean_value(int index_ = 0) const;

      /// Get the integer value stored at a given rank
      int get_integer_value(int index_ = 0) const;

      /// Get the real value stored at a given rank
      double get_real_value(int index_ = 0) const;

      /// Get the string value stored at a given rank
      std::string get_string_value(int index_ = 0) const;

      /// Set the boolean value at a given rank
      int set_value(bool value_, int index_ = 0);

      /// Set the integer value at a given rank
      int set_value(int value_, int index_ = 0);

      /// Set the real value at a given rank
      int set_value(double value_, int index_ = 0, bool explicit_unit_ = false);

      /// Set the real value at a given rank
      int set_value_with_unit(double value_, int index_ = 0, const std::string & unit_symbol_ = "");

      /// Set the explicit unit flag
      int set_explicit_unit(bool explicit_unit_flag_);

      /// Set the explicit path flag
      int set_explicit_path(bool explicit_path_flag_);

      /// Set the string value at a given rank
      int set_value(const std::string & value_, int index_ = 0, bool explicit_path_flag_ = false);

      /// Set the string value at a given rank
      int set_value(const char * value_, int index_ = 0, bool explicit_path_flag_ = false);

      /// Get the boolean value by reference stored at a given rank
      int get_value(bool & value_, int index_ = 0) const;

      /// Get the integer value by reference stored at a given rank
      int get_value(int & value_, int index_ = 0) const;

      /// Get the real value by reference stored at a given rank
      int get_value(double & value_, int index_ = 0) const;

      /// Get the string value by reference stored at a given rank
      int get_value(std::string & value_, int index_ = 0) const;

      /// Get a string label associated to the type of the stored data
      std::string get_type_label() const;

      /// Get a string label associated to the scalar/vector trait of the stored data
      std::string get_vector_label() const;

      /// Check if a string contains a forbidden character
      static bool has_forbidden_char(const std::string & checked_);

      /// Basic print
      void dump(std::ostream & out_) const;

      /// Convert to string and print in an output stream
      void to_string(std::ostream & out_) const;

      /// Returns an error message from an integer error code
      static std::string get_error_message(int error_code_);

      //! Method for smart printing (from the datatools::i_tree_dump interface).
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

    private:
      
      void clear_values_();

      void clear_unit_symbol_();

      int init_values_(char type_ = TYPE_INTEGER_SYMBOL,
                       int size_ = SCALAR_DEF);

      BOOST_SERIALIZATION_BASIC_DECLARATION()

    private:
      
      std::string _description_; //!< Description of the property
      /** 8-bits description flags :
       * Format is : VLUPSTTT
       *  V   == vector bit
       *  L   == lock bit
       *  U   == explicit unit bit (real)
       *  P   == path bit (string)
       *  S   == unit symbol (real)
       *  TTT == type bits
       */
      uint8_t     _flags_;          //!< Traits
      vbool       _boolean_values_; //!< Stored boolean values
      vint        _integer_values_; //!< Stored integer values
      vdouble     _real_values_;    //!< Stored real values
      vstring     _string_values_;  //!< Stored string values
      std::string _unit_symbol_;    //!< Preferred unit symbol for real properties

    }; //----- end of data inner class


    //----------------------------------------------------------------------
    // Config inner class
    //
    friend class config;
    //! \brief Class for ASCII file I/O operations with properties objects.
    class config {
    public:

      static const std::string & lock_decorator();
      static const std::string & as_directive();
      static const std::string & in_directive();
      static const std::string & path_decorator();
      static const std::string & metacomment_prefix();

      /// \brief Decoration mode
      /// @deprecated is replaced by
      /// the dedicated options_flag (HEADER_FOOTER) in constructor.
      enum decoration_mode_type {
        MODE_BARE          = 0,         ///< No decoration
        MODE_HEADER_FOOTER = 1,         ///< Header/footer decoration
        MODE_DEFAULT       = MODE_BARE, ///< Default decoration mode
        mode_bare          = MODE_BARE,
        mode_header_footer = MODE_HEADER_FOOTER
      };

      /// \brief Option flags used at construction
      enum options_flag {
        SKIP_PRIVATE       = bit_mask::bit00, ///< Skip private properties bit
        FORBID_VARIANTS    = bit_mask::bit01, ///< Forbid variant directives bit
        LOG_MUTE           = bit_mask::bit02, ///< Mute mode activation bit
        LOG_DEBUG          = bit_mask::bit03, ///< Debug mode activation bit
        LOG_TRACE          = bit_mask::bit04, ///< Trace mode activation bit
        SMART_MODULO       = bit_mask::bit05, ///< Use smart modulo (write)
        HEADER_FOOTER      = bit_mask::bit06, ///< Use header/footer (write)
        REQUESTED_TOPIC    = bit_mask::bit07, ///< Requested topic (read/write)
        FORBID_INCLUDE     = bit_mask::bit08, ///< Forbid include directives bit (read)
        DONT_CLEAR         = bit_mask::bit09, ///< Don't clear before parsing bit (read)
        RESOLVE_PATH       = bit_mask::bit10, ///< Resolve path for input filename (read/write)
        ALLOW_KEY_OVERRIDE = bit_mask::bit11, ///< Allow key override (read)
        LOG_WARNING        = bit_mask::bit12  ///< Warning mode activation bit
      };

      /// Constructor
      config(uint32_t options_ = 0,
             const std::string & topic_ = "",
             const std::string & section_name_ = "",
             int section_start_line_number_ = -1);

      /// Destructor
      virtual ~config();

      /// Return the logging priority threshold
      datatools::logger::priority get_logging() const;

      /// Set the logging priority threshold
      void set_logging(datatools::logger::priority);

      /// \brief Read a properties container from an input stream
      ///
      /// This method is the main parsing tool for datatools::properties-based configuration files.
      ///
      /// Example of usage which loads a configuration file to store the configuration parameters
      /// of an algorithm in a datatools::properties object:
      /// \code
      /// // A properties container:
      /// datatools::properties algo_parameters; 
      ///
      /// // A parser for configuration parameters:
      /// uint32_t parser_options = 0;                                   // No print option
      /// parser_options |= datatools::properties::config::LOG_DEBUG;    // Add debug option
      /// parser_options |= datatools::properties::config::RESOLVE_PATH; // Add path resolving option
      /// datatools::properties::config parser(parser_options);
      ///
      /// // Parse a file and fill the properties object:
      /// parser.read("${MYSETUP}/config/algo.conf", algo_parameters);
      /// {
      ///   // Smart print:
      ///   boost::property_tree::ptree smart_print_options;
      ///   smart_print_options.put("indent", "[debug] ");
      ///   smart_print_options.put("title", "Configuration parameters for my algorithm: ");
      ///   algo_parameters.print_tree(std::clog, smart_print_options);
      /// }
      /// \endcode
      ///
      /// Example of input file to be parsed:
      /// \code
      ///
      /// # Supported format:
      /// # - Comment lines start with a '#' and any segment of a line
      /// #   starting with '#' is also ignored;
      /// # - Blank lines are ignored;
      /// # - Lines starting with '#@' are considered as optional meta-comments
      /// #   with special embedded parsing options and/or actions.
      /// 
      /// ### Header ###
      /// #
      /// # The directives in this section can only be placed at the beginning of the 
      /// # file, always before the definitions of properties.
      ///
      /// #@topic "rootfinding"
      /// #  This optional meta-comment provides a conventional keyword that
      /// #  identifies the functionnality of the properties container
      /// #  accessed by an external tool / user.
      /// #  This directive must be given before any property directive.
      ///
      /// #@config[uration] Configuration parameters for a root finding algorithm
      /// #  This meta-comment provides the general description of the set of properties.
      /// #  This directive must be given before any property directive.
      /// 
      /// #@include_dir "${DATATOOLS_TESTING_DIR}/config/user"
      /// #@include_dir "${DATATOOLS_TESTING_DIR}/config/defaults"
      /// #@include_dir "/var/${USER}/config"
      /// #@include_dir "/etc/config"
      /// #  These meta-comments set the ordered list of absolute paths from which
      /// #  other properties files to be included are resolved. The first directive
      /// #  has the highest priority.
      /// #  This directive must be given before any property directive.
      /// 
      /// #@include_path_env "DATATOOLS_INCLUDE_PATH"
      /// #  This meta-comment specifies the name of an environment variable which
      /// #  sets a list of absolute paths from which other properties files to
      /// #  be included are resolved. This list is preprended to the specified list
      /// #  above, if any.
      /// #  This directive must be given before any property directive.
      /// 
      /// #@include_no_propagate
      /// #  This meta-comment specifies that the file inclusion rules are not
      /// #  propagated to included files which use their own rules.
      /// #  This directive must be given before any property directive.
      /// 
      /// #@include "path"
      /// #  This meta-comment specifies the path of a file to be included.
      /// #  This directive must be given before any property directive.
      /// 
      /// #@include_try "path"
      /// #  This meta-comment specifies the path of a file to try to include.
      /// #  This directive must be given before any property directive.
      /// 
      /// ### Definitions of properties ###
      ///
      /// # The '#@description text' optional meta-comments is used to document the next property to
      /// # be parsed (alternative: '#@parameter text')
      /// #@description The name of the method
      /// #  This meta-comment provides the specific description of next 'root_finder_type' property record.
      /// root_finder_type : string = "Newton-Raphson"
      /// 
      /// #@description The path of the log file
      /// #  The 'as path' directive below indicates that the string value must
      /// #  be considered as a filesystem path.
      /// log_file : string as path = "/tmp/root_finding.log"
      ///
      /// #@description Relative tolerance (dimensionless)
      /// #  The property record below shows a pure dimensionless floating-point based quantity.
      /// relative_epsilon : real = 5e-6
      ///
      /// #@description Absolute tolerance (with explicit dimension and associated unit)
      /// #  The 'as length' directive below indicates that the real value has an explicit
      /// #  dimension. The explicit 'um' unit symbol after the numerical value must
      /// #  match the required dimension. 
      /// absolute_epsilon : real as length = 1.5 um  # Symbol for micrometer
      ///
      /// #@description A constant value
      /// #  The 'const' directive will prevent any further modification of the property
      /// #  value after parsing.
      /// time_duration : const real as time = 0.05 ms
      ///
      /// #@verbose_parsing
      /// #  Special directive which activates, from this point, verbose parsing for debugging purpose.
      ///
      /// #@description Maximum number of iterations
      /// max_iterations : integer = 100
      /// 
      /// #@description Array of guess values to start with (unit is explicitely given)
      /// #  The 'in mm' directive indicates that all real values in the array
      /// #  are explicitly expressed in 'mm' unit (of 'length' dimension).
      /// #  Also a 'long' property directive can be splitted on several lines
      /// #  using a final '\' (after which no character, is allowed).
      /// guess_values : real[4] in mm = \  # no space is allowed after '\'
      ///   1.0 \   # no space is allowed after '\'
      ///   1.2 \   # no space is allowed after '\'
      ///   2.4 \   # no space is allowed after '\'
      ///   4.8     # last value
      ///
      /// #@description Here two more elements are appended to the original four items
      /// in the 'guess_values' array so that it contains finally six elements.
      /// In case of real values, the unit ('in UNITNAME') should be the same that
      /// the one used for the previous definition in order to avoid confusion or inconsistency.
      /// guess_values : real[2] in mm += 3.2 1.32
      ///    
      /// #@description Algorithm verbosity
      /// logging : string = "mute"  # no print at all
      ///
      /// #@allow_key_override 
      /// #  From this point, overriding already defined/duplicated properties is allowed.
      /// #  By default, overriding properties is forbidden.
      ///
      /// #@description Overriden algorithm verbosity (discard the first value above)
      /// logging : string = "debug" # print debug messages
      ///
      /// #@forbid_key_override # Special directive
      /// #  From this point, overriding already defined properties is forbidden again.
      ///
      /// #@enable_variants
      /// #  Enable the parsing of variant preprocessor directives.
      ///
      /// #@variant_only math:numerical_library/with_gsl|true
      /// #  This variant directive only applies to the next property directive
      /// #  if the "math:numerical_library/with_gsl" variant condition is set (boolean)
      /// #  where 'math' is the name of the target variant registry and
      /// # 'numerical_library/with_gsl' is the path of a variant condition within
      /// # this registry. The '|true' tag indicates that the condition should be
      /// # considered as true if no variant system is activated.
      /// #@description GSL error handling flag is set only if the GSL library is used
      /// gsl_error_support : boolean = true
      ///
      /// #@description Default tolerance on double precision floats
      /// #@variant_only math:tolerance/is_native|true
      /// #  This variant directive only applies to the next property directive
      /// #  if the "math:epsilon/is_native" variant condition is set (boolean).
      /// #  If the variant system is not activated, the 'true' value after
      /// #  the '|' character in the "@variant_only" directive is used to
      /// #  consider the condition set by default.
      /// default_epsilon : real = 1e-16
      ///
      /// #@variant_only math:tolerance/is_user|false
      /// #  This variant directive only applies to the next property directive
      /// #  if the "math:tolerance/is_user" variant condition is set.
      /// #  The parsed value is extracted by the variant preprocessor
      /// #  from a special variant variable (math:epsilon/is_user/epsilon) available
      /// #  only when the "math:tolerance/is_user" condition is set.
      /// #  If the variant system is not activated, the 'false' value after
      /// #  the '|' character in the "@variant_only" directive is used to consider
      /// #  the condition unset by default.
      /// #  If the variant system is not activated, the value '1.e-7' after
      /// #  the '|' character is used as a fallback value for this property.
      /// # 
      /// default_epsilon : real = @variant(math:tolerance/is_user/epsilon|1.e-7)
      ///
      /// #@variant_if math:numerical_library/with_std|true
      /// #  This variant directive starts a conditional block of property directives.
      /// #  The block is activated if and only if the "math:numerical_library/with_std"
      /// #  variant condition is set. This directive implies the use of a matching
      /// # '#@variant_endif' closing directive (see below).
      /// 
      /// #@description The identifier of the base pseudo-random number generator (PRNG)
      /// random_prng : string = "mersenne_twister"
      ///
      /// # The next property directive is commented out.
      /// # #@description The PRNG seed 
      /// # random_seed : integer = 314159
      /// 
      /// #@description The path of the input file where to load initial PRNG state 
      /// random_input_state_path : string as path = "${MYSETUP}/run_42/config/prng_state_in.state"
      /// 
      /// #@description The path of the output file where to save the final initial PRNG state 
      /// random_output_state_path : string as path = "${MYSETUP}/run_42/product/prng_state_out.data"
      ///
      /// #@variant_endif # End of the variant conditional block of property directives
      /// # Optionaly the '#@variant_endif' can specify the variant condition is refers to :
      /// # #@variant_endif math:numerical_library/with_std
      /// # Variant conditional blocks can be nested.
      ///
      /// #@description The Pi constant in explicit angular unit
      /// #  The variant system provides three different options associated to the
      /// #  'math:accuracy' variant parameter, each of them triggers
      /// #  the use of a specific value for the Pi constant in this file.
      /// #@variant_only math:accuracy/if_accurate|true
      /// Pi : const real as angle = 3.14159265358979 radian
      ///
      /// #@variant_only math:accuracy/if_approximated|false
      /// Pi : const real as angle = 3.14 radian
      ///
      /// #@variant_only math:accuracy/if_inaccurate|false
      /// Pi : const real as angle = 3 radian
      ///
      /// #@disable_variants
      /// #  Disable variant preprocessor directives support from this point.
      ///
      /// #@description GUI activation
      /// gui : boolean = false
      ///
      /// #@description GUI mode
      /// gui.mode : string = "light"
      ///
      /// #@description GUI colors (background, foreground then alarm foreground)
      /// gui.colors : string[3] = "grey" "black" "red"
      ///
      /// # Load properties from another ``properties`` container which is merged in
      /// # in the current container. Key overriding is allowed by default and inhibited
      /// # only if "#@forbid_key_override" is explicitely set
      /// #@include : string as path = "~/myconfig/defaults.conf"
      ///
      /// #@end
      /// #  Special directives which forces the end of parsing so that
      /// #  next lines will be ignored.
      /// not_parsed : string = "unused"  # This property directive will not be parsed.
      ///
      ///
      /// \endcode
      /// 
      void read(std::istream & in_, properties & prop_);

      /// Read a properties container from an input file
      void read(const std::string & in_, properties & prop_);

      /// Write a properties container to an output stream
      void write(std::ostream & out_, const properties & prop_);

      /// Write a properties container to an output file
      void write(const std::string & filename_, const properties & prop_);

      /// Return the current value of the line counter
      int get_current_line_number() const;

      /// Set the filename and the line counter before parsing
      void set_reader_input(const std::string & filename_, int line_count_ = -1);

      /// Check if topic is set
      bool has_topic() const;

      /// Set the topic that should be matched
      void set_topic(const std::string & topic_);

      /// Return the topic
      const std::string & get_topic() const;

      /// Check if section info is set
      bool has_section_info() const;

      /// Set the section info
      void set_section_info(const std::string & section_name_,
                            int section_start_line_number_);

      /// Reset the section info
      void reset_section_info();

      /// Return the section
      const std::string & get_section_name() const;

      /// Return the section start line
      int get_section_start_line_number() const;

      /// Return the embedded file inclusion solver 
      const file_include & get_fi() const;

      /// Return the mutable embedded file inclusion solver 
      file_include & grab_fi();

      /// Override the embedded file inclusion solver 
      void set_fi(const file_include &);

      /// Reset
      void reset();

      /// Write meta-comment
      void write_metacomment(std::ostream & out_,
                             const std::string & tag_,
                             const std::string & value_ = "",
                             const std::string & comment_ = "");

      /// Write a property data
      void write_data(std::ostream & out_,
                      const std::string & data_key_,
                      const properties::data& prop_data_,
                      const std::string & unit_symbol_ = "",
                      const std::string & unit_label_ = "",
                      const std::string & comment_ = "");

    private:

      /// Set default values at construction
      void _init_defaults_();

      /// Parsing implementation
      void _read_(std::istream & in_, properties & prop_);

      /// Saving implementation
      void _write_(std::ostream & out_, const properties & prop_);

    private:

      // Configuration:
      logger::priority _logging_; ///< Logging priority threshold (read/write)
      int    _mode_;              ///< Decoration mode (header/footer)
      bool   _dont_clear_;        ///< Do not clear the container at the beginning of parsing (read)
      bool   _use_smart_modulo_;  ///< Use multiline formatting for vector data (write)
      bool   _write_public_only_; ///< Only output public properties
      bool   _forbid_variants_;   ///< Flag to forbid variant directives (read)
      bool   _forbid_include_ = false; ///< Flag to forbid include directives (read)
      bool   _requested_topic_;   ///< Flag to activate topic matching (read)
      bool   _resolve_path_;      ///< Explicitely resolve path for input/output filenames (read/write)
      bool   _allow_key_override_ = false; ///< Allow key override when duplicated keys are parsed (read)
      std::string _topic_;             ///< Topic to be validated
      std::string _section_name_;      ///< Current section name (from hosting multi_properties, read)
      int _section_start_line_number_; ///< Current section start line number (from hosting multi_properties, read)

      // Working parsing data:
      size_t      _current_line_number_; ///< Current input line number
      std::string _current_filename_;    ///< Current filename
      file_include _fi_;                 ///< File inclusion solver

    }; //----- end of class config

    //----------------------------------------------------------------------
    // properties class declarations
    //
  public:

    //! Prefix string used for the naming of private properties
    //!
    //! A private property has a key starting with this prefix,
    //! namely '__' (double underscore).
    static const std::string & private_property_prefix();

    // Typedefs declarations:
    
  protected:

    typedef std::map<std::string, data> pmap;

  public:

    typedef std::vector<std::string> keys_col_type;
    
    /// Default constructor
    properties() = default;

    /// Constructor with explicit description
    explicit properties(const std::string & desc_);

    /// Destructor
    ~properties() override = default;

    /// Copy constructor
    properties(const properties &) = default;

    /// Copy assignment
    properties & operator=(const properties &) = default;

    /// Move constructor
    properties(properties &&) = default;

    /// Move assignment
    properties & operator=(properties &&) = default;

    /// Returns the number of stored properties
    int32_t size() const;

    /// Check if the properties container is empty
    bool empty() const;

    /// Set the description string of the properties container
    ///
    /// The description is a single line of text possibly splitted
    /// in subparts separated by the semicolon (';') character.
    ///
    /// Examples:
    /// \code
    /// Head
    /// Head;Text;...;Text;...
    /// ;Text;...;Text;...
    /// \endcode
    /// The leading (optional) subpart corresponds to a short text
    /// which describes the container of properties.
    /// The other subparts are optional and possibly add some auxiliary informations
    /// about the container of properties.
    ///
    /// Examples:
    /// \code
    /// My beautiful set of configuration parameters; group=geometry ; price=100 credits ; company=ACME
    /// \endcode
    /// The leading (optional) subpart corresponds to a short text
    ///
    /// Supported format in configuration file:
    /// \code
    /// #@config short-description-text;metadata0;metadata1;metadata2
    /// \endcode
    void set_description(const std::string &);

    /// Returns the description string of the properties container
    const std::string & get_description() const;

    /// Fetch the short description string associated to the container
    bool fetch_short_description(std::string &) const;

    /// Check if a short description string is associated to the container
    bool has_short_description() const;

    /// Get the short description string associated to the container
    std::string get_short_description() const;

    /// Check if some auxiliary descriptions are set
    bool has_auxiliary_descriptions() const;

    /// Fetch the auxiliary description strings associated to the container
    bool fetch_auxiliary_descriptions(std::vector<std::string> &) const;

    //! Returns the list of keys stored in the map (read-only).
    std::vector<std::string> keys() const;

    //! Returns the set of keys stored in the map (read-only).
    std::set<std::string> get_keys() const;

    //! Returns the ith key
    const std::string & key(int) const;

    //! Set the list of keys.
    void keys(std::vector<std::string> &) const;

    //! Compute the set of keys.
    void compute_keys(std::set<std::string> &) const;

    //! Access to a non-mutable reference to a property data object
    const data & get(const std::string & prop_key_) const;

    //! Store data item with supplied key
    void store(const std::string & key_, const data & value_);

    //! builds the list of keys (by reference) stored in the map that start with prefix.
    void keys_not_starting_with(std::vector<std::string> &, const std::string & prefix_) const;

    //! returns the list of keys stored in the map that start with prefix.
    std::vector<std::string> keys_not_starting_with(const std::string & prefix_) const;

    //! builds the list of keys (by reference) stored in the map that start with prefix.
    void keys_starting_with(std::vector<std::string> &, const std::string & prefix_) const;

    //! returns the list of keys stored in the map that start with prefix.
    std::vector<std::string> keys_starting_with(const std::string & prefix_) const;

    //! builds the list of keys (by reference) stored in the map that end with suffix.
    void keys_not_ending_with(std::vector<std::string> &, const std::string & suffix_) const;

    //! returns the list of keys stored in the map that end with suffix.
    std::vector<std::string> keys_not_ending_with(const std::string & suffix_) const;

    //! builds the list of keys (by reference) stored in the map that end with suffix.
    void keys_ending_with(std::vector<std::string> &, const std::string & suffix_) const;

    //! returns the list of keys stored in the map that end with suffix.
    std::vector<std::string> keys_ending_with(const std::string & suffix_) const;

    //! Lock the properties dictionary.
    void lock(const std::string & prop_key_);

    //! Unlock the properties dictionary.
    void unlock(const std::string & prop_key_);

    //! Check if the instance is locked.
    bool is_locked(const std::string & prop_key_) const;

    //! Return a static key.
    static std::string make_private_key(const std::string & prop_key_);

    //! Check if a string matches a private 'prop_key_'.
    static bool key_is_private(const std::string & prop_key_);

    //! Check if a string matches a public 'prop_key_'.
    static bool key_is_public(const std::string & prop_key_);

    //! Check if data with name 'prop_key_' is private
    bool is_private(const std::string & prop_key_) const;

    //! Check if data with name 'prop_key_' is public
    bool is_public(const std::string & prop_key_) const;

    //! Return the type of the data
    int get_type(const std::string & prop_key_) const;
    
    /// Get a string label associated to the type of the data
    std::string get_type_label(const std::string & prop_key_) const;
    
    //! Check if data with name 'prop_key_' is boolean
    bool is_boolean(const std::string & prop_key_) const;

    //! Check if data with name 'prop_key_' is integer
    bool is_integer(const std::string & prop_key_) const;

    //! Check if data with name 'prop_key_' is real
    bool is_real(const std::string & prop_key_) const;

    //! Check if data with name 'prop_key_' is string
    bool is_string(const std::string & prop_key_) const;

    //! Check if data with name 'prop_key_' is scalar
    bool is_scalar(const std::string & prop_key_) const;

    //! Check if data with name 'prop_key_' is vector
    bool is_vector(const std::string & prop_key_) const;

    // 2012-11-14 FM : Should be deprecated
    int32_t size(const std::string & prop_key_) const;

    //! Returns the size of the data stored with a given key/name
    int32_t key_size(const std::string & prop_key_) const;

    //! Check if a property with given key/name exists
    bool has_key(const std::string & prop_key_) const;

    //! Lock a property with given key/name
    void key_lock (const std::string & prop_key_);

    //! Unlock a property with given key/name
    void key_unlock (const std::string & prop_key_);

    //! Get the description string associated to a property with given key/name
    const std::string & get_key_description (const std::string & prop_key_) const;

    //! Set the description string associated to a property with given key/name
    void set_key_description (const std::string & prop_key_, const std::string &desc_);

    // 2011-11-27 FM: could be useful
    //! Rename a property with a new name.
    //void rename (const std::string & prop_key_, const std::string & a_new_key);

    //! Erase property with a given key/name
    void erase(const std::string & key_);

    //! Erase all properties.
    void erase_all();

    //! Erase all properties with key/name starting with prefix
    void erase_all_starting_with(const std::string & prefix_);

    //! Erase all properties with key/name not starting with prefix
    void erase_all_not_starting_with(const std::string & prefix_);

    //! Export all properties into another properties container
    void export_all(properties & props_) const;

    //! Export all properties into another properties container adding a prefix
    void export_all_adding_prefix(properties & props_, const std::string & prefix_) const;

    //! Export all properties with key/name starting with a given prefix to another properties container
    void export_starting_with(properties & props_,
                              const std::string & prop_key_prefix_) const;

    //! Export all properties with key/name starting with a given prefix to another properties container but renaming the prefix on the fly
    void export_and_rename_starting_with(properties & props_,
                                         const std::string & prop_key_prefix_,
                                         const std::string & new_prefix_) const;

    //! Export all properties with key/name not starting with a given suffix to another properties container
    void export_not_starting_with(properties & props_,
                                  const std::string & prop_key_prefix_) const;

    //! Export all properties with key/name fulfilling a given predicate
    template <class key_predicate>
    void export_if(properties & props_, const key_predicate & predicate_) const;

    //! Export all properties with key/name not fulfilling a given predicate
    template <class key_predicate>
    void export_not_if(properties & props_,
                       const key_predicate & predicate_) const;

    //! Erase all properties with key/name ending with a given suffix
    void erase_all_ending_with(const std::string & suffix_);

    //! Erase all properties with key/name not ending with a given suffix
    void erase_all_not_ending_with(const std::string & suffix_);

    //! Export all properties with key/name ending with a given suffix to another properties container
    void export_ending_with(properties & props_,
                            const std::string & suffix_) const;

    //! Export all properties with key/name not ending with a given suffix to another properties container
    void export_not_ending_with(properties & props,
                                const std::string & suffix_) const;

    //! Remove a property with a given key/name
    void clean(const std::string & prop_key_);

    //! Reset method (from the datatools::i_clear interface).
    void clear() override;

    //! Reset method
    void reset();

    //! Set a boolean 'true' flag with a given key/name, a description string and a lock request
    void store_flag(const std::string & prop_key_, const std::string & desc_ = "",
                    bool lock_ = false);

    //! Set a boolean 'true' flag with a given key/name
    /** @param prop_key_ The key of the boolean property
     */
    void set_flag(const std::string & prop_key_);

    //! Remove a boolean flag with a given key/name
    /** @param prop_key_ The key of the boolean property
     */
    void unset_flag(const std::string & prop_key_);

    //! Store a boolean property with a given key/name and value
    void store(const std::string & prop_key_, bool value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a boolean property with a given key/name and value
    void store_boolean(const std::string & prop_key_, bool value_,
                       const std::string & desc_ = "", bool lock_ = false);

    //! Store an integer property with a given key/name and value
    void store(const std::string & prop_key_, int value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store an integer property with a given key/name and value
    void store_integer(const std::string & prop_key_, int value_,
                       const std::string & desc_ = "",
                       bool lock_ = false);

    //! Store a real property with a given key/name and value
    void store(const std::string & prop_key_, double value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a real property with a given key/name and value
    void store_real(const std::string & prop_key_, double value_,
                    const std::string & desc_ = "", bool lock_ = false);

    //! Store a real property with a given key/name and value with the explicit unit flag
    /**
     *   \code
     *   datatools::properties config;
     *   config.store_real("box.zoom_factor", 2.5);
     *   config.store_real("box.efficiency", 0.17);
     *   config.store_real_with_explicit_unit("box.width", 3.4 * CLHEP::cm);
     *   config.store_real_with_explicit_unit("box.height", 5.2 * CLHEP::mm);
     *   \endcode
     */
    void store_real_with_explicit_unit(const std::string & prop_key_, double value_,
                                       const std::string & desc = "", bool lock_ = false);

    //! Store a real property with a given key/name and value with the explicit unit flag
    /// @see store_real_with_explicit_unit
    void store_with_explicit_unit(const std::string & prop_key_, double value_,
                                  const std::string & desc = "", bool lock_ = false);

    //! Set flag for explicit unit for a real property with a given key/name
    void set_explicit_unit(const std::string & prop_key_, bool a_explicit_unit = true);

    //! Check flag for explicit unit for a real property with a given key/name
    bool has_explicit_unit(const std::string & prop_key_) const;

    //! Set the unit symbol for a real property with a given key/name
    void set_unit_symbol(const std::string & prop_key_, const std::string & unit_symbol = "");

    //! Check flag for unit symbol for a real property with a given key/name
    bool has_unit_symbol(const std::string & prop_key_) const;

    //! Return the unit symbol for a real property with a given key/name
    const std::string & get_unit_symbol(const std::string & prop_key_) const;

    //! Set flag for explicit path for a string property with a given key/name
    void set_explicit_path(const std::string & prop_key_, bool a_explicit_path = true);

    //! Check flag for explicit path for a string property with a given key/name
    bool is_explicit_path(const std::string & prop_key_) const;

    //! Store a string property with a given key/name and value
    void store(const std::string & prop_key_, const std::string & value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a string property with a given key/name and value
    void store_string(const std::string & prop_key_, const std::string & value_,
                      const std::string & desc_ = "", bool lock_ = false);

    //! Store a path property with a given key/name and value
    void store_path(const std::string & prop_key_, const std::string & path_value_,
                    const std::string & desc_ = "", bool lock_ = false);

    //! Store a string property with a given key/name and value (C style)
    void store(const std::string & prop_key_, const char* value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a boolean vector property with a given key/name and value
    void store(const std::string & prop_key_, const data::vbool & value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a integer vector property with a given key/name and value
    void store(const std::string & prop_key_, const data::vint & value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a real vector property with a given key/name and value
    void store(const std::string & prop_key_, const data::vdouble & value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a string vector property with a given key/name and value
    void store(const std::string & prop_key_, const data::vstring & value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a path string vector property with a given key/name and value
    void store_paths(const std::string & prop_key_, const data::vstring & path_value_,
                     const std::string & desc_ = "", bool lock_ = false);

    //! Change the value of an existing boolean property with a given key/name and index
    void change(const std::string & key_, bool value_, int index_ = 0);

    //! Change the value of an existing boolean property with a given key/name and index
    void change_boolean(const std::string & key_, bool value_, int index_ = 0);

    //! Change the value of an existing scalar boolean property with a given key/name
    void change_boolean_scalar(const std::string & key_, bool value_);

    //! Change the value of an existing vector boolean property with a given key/name and index
    void change_boolean_vector(const std::string & key_, bool value_, int index_);

    //! Change the value of an existing integer property with a given key/name and index
    void change(const std::string & key_, int value_, int index_ = 0);

    //! Change the value of an existing integer property with a given key/name and index
    void change_integer(const std::string & key_, int value_, int index_ = 0);

    //! Change the value of an existing integer scalar property with a given key/name
    void change_integer_scalar(const std::string & key_, int value_);

    //! Change the value of an existing integer vector property with a given key/name and index
    void change_integer_vector(const std::string & key_, int value_, int index_);

    //! Change the value of an existing real property with a given key/name and index
    void change(const std::string & key_, double value_, int index_ = 0);

    //! Change the value of an existing real property with a given key/name and index
    void change_real(const std::string & key_, double value_, int index_ = 0);

    //! Change the value of an existing real scalar property with a given key/name
    void change_real_scalar(const std::string & key_, double value_);

    //! Change the value of an existing real vector property with a given key/name and index
    void change_real_vector(const std::string & key_, double value_, int index_);

    //! Change the value of an existing string property with a given key/name and index
    void change(const std::string & key_, const std::string & value_,
                int index_ = 0);

    //! Change the value of an existing string property with a given key/name and index
    void change_string(const std::string & key_, const std::string & value_,
                       int index_ = 0);

    //! Change the value of an existing string scalar property with a given key/name
    void change_string_scalar(const std::string & key_, const std::string & value_);

    //! Change the value of an existing string vector property with a given key/name and index
    void change_string_vector(const std::string & key_, const std::string & value_,
                              int index_);

    //! Change the value of an existing string property (C style) with a given key/name and index
    void change(const std::string & key_, const char * value_, int index_ = 0);

    //! Change the full contents of an existing boolean vector property with a given key/name
    void change(const std::string & key_, const data::vbool & values_);

    //! Change the full contents of an existing integer vector property with a given key/name
    void change(const std::string & key_, const data::vint & values_);

    //! Change the full contents of an existing real vector property with a given key/name
    void change(const std::string & key_, const data::vdouble & values_);

    //! Change the full contents of an existing string vector property with a given key/name
    void change(const std::string & key_, const data::vstring & values_);

    //! Update a boolean flag to true with a given key/name
    void update_flag(const std::string & key_);

    //! Update a boolean flag with a given key/name and value
    void update(const std::string & key_, bool value_);

    //! Update a boolean flag with a given key/name and value
    void update_boolean(const std::string & key_, bool value_);

    //! Update an integer flag with a given key/name and value
    void update(const std::string & key_, int value_);

    //! Update an integer flag with a given key/name and value
    void update_integer(const std::string & key_, int value_);

    //! Update a real flag with a given key/name and value
    void update(const std::string & key_, double value_);

    //! Update a real flag with a given key/name and value
    void update_real(const std::string & key_, double value_);

    //! Update a real flag with a given key/name and value
    void update_real_with_explicit_unit(const std::string & key_, double value_);

    //! Update a real flag with a given key/name and value
    void update_with_explicit_unit(const std::string & key_, double value_);

    //! Update a string flag with a given key/name and value
    void update(const std::string & key_, const std::string & value_);

    //! Update a C-string flag with a given key/name and value
    void update(const std::string & key_, const char* value_);

    //! Update a string flag with a given key/name and value
    void update_string(const std::string & key_, const std::string & value);

    //! Update the full contents of a boolean vector with a given key/name
    void update(const std::string & key_, const data::vbool & values_);

    //! Update the full contents of an integer vector with a given key/name
    void update(const std::string & key_, const data::vint & values_);

    //! Update the full contents of a real vector with a given key/name
    void update(const std::string & key_, const data::vdouble & values_);

    //! Update the full contents of a string vector with a given key/name
    void update(const std::string & key_, const data::vstring & values_);

    //! Check if a boolean value with a given key/name exists with value 'true'
    bool has_flag(const std::string & key_) const;

    //! Fetch the boolean value stored with a given key/name and index
    void fetch(const std::string & key_, bool & value_, int index_ = 0) const;

    //! Fetch the integer value stored with a given key/name and index
    void fetch(const std::string & key_, int & value_, int index_ = 0) const;

    //! Fetch the real value stored with a given key/name and index
    void fetch(const std::string & key_, double & value_, int index_ = 0) const;

    //! Fetch the string value stored with a given key/name and index
    void fetch(const std::string & key_, std::string & value_,
               int index = 0) const;

    //! Fetch the boolean vector value stored with a given key/name
    void fetch(const std::string & key_, data::vbool & values_) const;

    //! Fetch the integer vector value stored with a given key/name
    void fetch(const std::string & key_, data::vint & values_) const;

    //! Fetch the real vector value stored with a given key/name
    void fetch(const std::string & key_, data::vdouble & values_) const;

    //! Fetch the dimensionless real vector value stored with a given key/name
    void fetch_dimensionless(const std::string & key_, data::vdouble & values_) const;

    //! Fetch the string vector value stored with a given key/name
    void fetch(const std::string & key_, data::vstring & values_) const;

    //! Fetch a set of string values from the vector value stored with a given key/name
    void fetch(const std::string & key_, std::set<std::string> & values_, bool allow_duplication_ = false) const;

    //! Fetch a list of unique string values from the vector value stored with a given key/name
    void fetch_unique_ordered(const std::string & key_, std::vector<std::string> & values_) const;

    //! Fetch a set of integer values from the vector value stored with a given key/name
    void fetch(const std::string & key_, std::set<int> & values, bool allow_duplication_ = false) const;

    //! Fetch a set of unsigned integer values from the vector value stored with a given key/name
    void fetch_positive(const std::string & key_, std::set<unsigned int> & values_, bool allow_duplication_ = false) const;

    //! Fetch the boolean value stored with a given key/name and index
    bool fetch_boolean(const std::string &, int index_ = 0) const;

    //! Fetch the boolean scalar value stored with a given key/name
    bool fetch_boolean_scalar(const std::string & name_) const;

    //! Fetch the boolean vector value stored with a given key/name and index
    bool fetch_boolean_vector(const std::string & name_, int index_) const;

    //! Fetch the integer value stored with a given key/name and index
    int fetch_integer(const std::string & name_, int index_ = 0) const;

    //! Fetch the positive integer value stored with a given key/name and index
    unsigned int fetch_positive_integer(const std::string & name_, int index_ = 0) const;

    //! Fetch the strict positive integer value stored with a given key/name and index
    unsigned int fetch_strict_positive_integer(const std::string & name_, int index_ = 0) const;

    //! Fetch the ranged integer value stored with a given key/name and index
    int fetch_range_integer(const std::string & name_, int min_, int max_, int index_ = 0) const;

    //! Fetch the integer scalar value stored with a given key/name
    int fetch_integer_scalar(const std::string & name_) const;

    //! Fetch the integer vector value stored with a given key/name and index
    int fetch_integer_vector(const std::string & name_ , int index_) const;

    //! Fetch the real value stored with a given key/name and index
    double fetch_real(const std::string & name_, int index_ = 0) const;

    //! Fetch the dimensionless real value stored with a given key/name and index
    double fetch_dimensionless_real(const std::string & name_, int index_ = 0) const;

    //! Fetch the physical quantity (with its explicit unit) value stored with a given key/name and index
    double fetch_real_with_explicit_unit(const std::string & name_, int index_ = 0) const;

    //! Fetch the physical quantity (with its explicit dimension) value stored with a given key/name and index
    double fetch_real_with_explicit_dimension(const std::string & name_, const std::string & dimension_, int index_ = 0) const;

    //! Fetch the real scalar value stored with a given key/name
    double fetch_real_scalar(const std::string & name_) const;

    //! Fetch the real vector value stored with a given key/name and index
    double fetch_real_vector(const std::string & name_, int index_) const;

    //! Fetch the string value stored with a given key/name and index
    std::string fetch_string(const std::string & name_, int index_ = 0) const;

    //! Fetch a single character value stored with a given key/name and index
    char fetch_one_character(const std::string & name_, int index_ = 0) const;

    //! Fetch the string scalar value stored with a given key/name
    std::string fetch_string_scalar(const std::string & name_) const;

    //! Fetch the string vector value stored with a given key/name and index
    std::string fetch_string_vector(const std::string & name_, int index_) const;

    //! Fetch a file path from a string value stored with a given key/name and index
    std::string fetch_path(const std::string & name_, int index_ = 0) const;

    //! Fetch a file path from a string scalar value stored with a given key/name
    std::string fetch_path_scalar(const std::string & name_) const;

    //! Fetch a file path from a string vector value stored with a given key/name and index
    std::string fetch_path_vector(const std::string & name_, int index_) const;

    //! Basic print
    void dump(std::ostream & out_ = std::clog) const;

    //! \deprecated Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;

    /// Merge with another properties with overriding possibilities
    ///
    /// If the allow_override_ flag is set, any key also existing in other_,
    /// with the same type, is overriden by the value stored in other_. 
    void merge_with(const properties & other_, bool allow_overrride_ = false);
    
    std::string key_to_string(const std::string & key_) const;

    std::string key_to_property_string(const std::string & key_) const;

    void export_to_string_based_dictionary(std::map<std::string, std::string> & dict_,
                                           bool quoted_strings_ = true) const;

    //! Store the properties' container object in an ASCII text file
    void write_configuration(const std::string & filename_,
                             uint32_t options_ = config::SMART_MODULO | config::SKIP_PRIVATE) const;

    //! Load the properties' container object from an ASCII text file
    //!
    //! \see The datatools::properties::config::read method for file format
    void read_configuration(const std::string & filename_,
                            uint32_t options_ = config::SMART_MODULO);

    //! Store the properties' container object in an ASCII text file
    static void write_config(const std::string & filename_,
                             const properties & props_,
                             uint32_t options_ = 0);

    //! Load the properties' container object from an ASCII text file
    //!
    //! \see The datatools::properties::config::read method for file format
    static void read_config(const std::string & filename_,
                            properties & props_,
                            uint32_t options_ = 0);

    //! Build a new property key from a prefix and a key
    static std::string build_property_key(const std::string & prefix_,
                                          const std::string & subkey_);

     
  private:

    void _check_nokey_(const std::string & prop_key_) const;

    void _check_key_(const std::string & prop_key_, data ** data_);

    void _check_key_(const std::string & prop_key_, const data ** data_) const;

    void _validate_key_(const std::string & prop_key_) const;

  private:

    // Internal data:
    std::string _description_ = ""; //!< Description string
    pmap        _props_{};          //!< Internal list of properties

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(properties)

    //! Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(properties)
    DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()

#ifndef Q_MOC_RUN
    //! Reflection interface
    DR_CLASS_RTTI()
#endif // Q_MOC_RUN

  };

  //! Return a non mutable reference to a singleton empty configuration container (can be used as default empty set of configuration parameters)
  const properties & empty_config();

  //----------------------------------------------------------------------
  // properties class template method definitions
  //
  template <class key_predicate>
  void properties::export_if(properties & props_,
                             const key_predicate & predicate_) const
  {
    DT_THROW_IF (this == &props_,
                 std::logic_error, "Self export is not allowed !");
    for(const auto& p : _props_) {
      if (predicate_(p.first)) {
        props_._props_[p.first] = p.second;
      }
    }
    return;
  }

  template <class key_predicate>
  void properties::export_not_if(properties & props_,
                                 const key_predicate & predicate_) const
  {
    DT_THROW_IF (this == &props_,
                 std::logic_error, "Self export is not allowed !");
    for(const auto& p : _props_) {
      if (!predicate_(p.first)) {
        props_._props_[p.first] = p.second;
      }
    }
    return;
  }

} // end of namespace datatools

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_DECLARATION(::datatools::properties)
// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::datatools::properties)

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/serialization/export.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

BOOST_CLASS_EXPORT_KEY2(datatools::properties, "datatools::properties")

#ifndef Q_MOC_RUN
// Activate reflection layer for the 'datatools::properties' class:
DR_CLASS_INIT(::datatools::properties)
#endif // Q_MOC_RUN

// Explicit class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::properties::data, 2)
BOOST_CLASS_VERSION(datatools::properties, 2)

#endif // DATATOOLS_PROPERTIES_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
