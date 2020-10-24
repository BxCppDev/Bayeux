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
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_clear.h>
#include <datatools/i_cloneable.h>
#include <datatools/bit_mask.h>
#include <datatools/exception.h>

#include <datatools/make_configuration.h>

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
      /// Returns an error message from an integer error code
      static std::string get_error_message(int error_code_);

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
      /// Check if a string contains a forbidden character
      static bool has_forbidden_char(const std::string & checked_);

      static const size_t SCALAR_DEF  = std::numeric_limits<size_t>::max();
      static const size_t SCALAR_SIZE = 1;

      /// \brief Provides static method for default values for each supported type
      struct defaults {
        static bool boolean_value(); // return false
        static int integer_value();  // return 0
        static double real_value();  // return 0.0
        static std::string string_value(); // return empty string

        template <typename T>
        static T default_value();
      };

      /// \brief Provides static method for type labels for each supported type
      struct format {
        static const std::string & boolean_label();
        static const std::string & integer_label();
        static const std::string & real_label();
        static const std::string & string_label();
      };

    public:
      using vbool = std::vector<bool>;          ///< Container for boolean data
      using vint =  std::vector<int32_t>;       ///< Container for integer data
      using vdouble =  std::vector<double>;     ///< Container for real data
      using vstring = std::vector<std::string>; ///< Container for character string data

    public:
      /// Default constructor
      /// Requires retention as some classes still expect this
      //non-(construction-is-initialization) interface...
      data(char type_ = TYPE_INTEGER_SYMBOL, size_t size_ = SCALAR_DEF);

      /// Constructor for boolean value
      data(bool value_, size_t size_ = SCALAR_DEF);

      /// Constructor for int value
      data(int value_, size_t size_ = SCALAR_DEF);

      /// Constructor for double value
      data(double value_, size_t size_ = SCALAR_DEF);

      /// Constructor for string value
      data(const std::string & value_, size_t size_ = SCALAR_DEF);

      /// Constructor for string value
      data(const char* value_, size_t size_ = SCALAR_DEF);

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

      /// Return type
      int get_type() const;

      /// Get a string label associated to the type of the stored data
      std::string get_type_label() const;

      /// Get a string label associated to the scalar/vector trait of the stored data
      std::string get_vector_label() const;

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

      /// Generic type check
      template <typename T>
      bool is_type() const;

      /// Check is description is set
      bool has_description() const;

      /// Set the description string associated to the stored data
      void set_description(const std::string & description_);

      /// Get the description string associated to the stored data
      const std::string & get_description() const;

      /// Check if the data has an unit symbol
      bool has_unit_symbol() const;

      /// Check if the (real only) data has been initialized with explicit unit
      bool has_explicit_unit() const;

      /// Set the preferred unit symbol associated to the stored real data
      ///
      /// This method implies to set the *explicit unit* flag.
      int set_unit_symbol(const std::string & symbol_);

      /// Get the unit symbol associated to the stored real data
      const std::string & get_unit_symbol() const;

      /// Check if the (string only) data has been initialized with explicit path
      bool is_explicit_path() const;

      /// Assign N boolean values
      int boolean(size_t size_ = SCALAR_DEF);

      /// Assign N integer values
      int integer(size_t size_ = SCALAR_DEF);

      /// Assign N real values
      int real(size_t size_ = SCALAR_DEF);

      /// Assign N string values
      int string(size_t size_ = SCALAR_DEF);

      /// Generic assignment and specializations
      template <typename T>
      int assign(size_t size_ = SCALAR_DEF);

      /// Check if the data is locked (cannot be modified)
      bool is_locked() const;

      /// Lock the value (make it unmutable)
      int lock();

      /// Unlock the value (make it mutable)
      int unlock();

      /// Returns the size of the array of stored values (1 if scalar, >=0 if vector)
      size_t get_size() const;

      /// Returns the size of the array of stored values (1 if scalar, >=0 if vector)
      size_t size() const;

      /// Check if the value array is empty
      bool empty() const;

      /// Check if array index is valid
      bool index_is_valid(size_t index_) const;

      /// Get the boolean value stored at a given rank
      bool get_boolean_value(size_t index_ = 0) const;

      /// Get the integer value stored at a given rank
      int get_integer_value(size_t index_ = 0) const;

      /// Get the real value stored at a given rank
      double get_real_value(size_t index_ = 0) const;

      /// Get the string value stored at a given rank
      std::string get_string_value(size_t index_ = 0) const;

      /// Set the boolean value at a given rank
      int set_value(bool value_, size_t index_ = 0);

      /// Set the integer value at a given rank
      int set_value(int value_, size_t index_ = 0);

      /// Set the real value at a given rank
      int set_value(double value_, size_t index_ = 0, bool explicit_unit_ = false);

      /// Set the real value at a given rank
      int set_value_with_unit(double value_, size_t index_ = 0, const std::string & unit_symbol_ = "");

      /// Set the explicit unit flag
      int set_explicit_unit(bool explicit_unit_flag_);

         /// Set the string value at a given rank
      int set_value(const std::string & value_, size_t index_ = 0, bool explicit_path_flag_ = false);

      /// Set the string value at a given rank
      int set_value(const char * value_, size_t index_ = 0, bool explicit_path_flag_ = false);

      /// Set the explicit path flag
      int set_explicit_path(bool explicit_path_flag_);

      /// Get the boolean value by reference stored at a given rank
      int get_value(bool & value_, size_t index_ = 0) const;

      /// Get the integer value by reference stored at a given rank
      int get_value(int & value_, size_t index_ = 0) const;

      /// Get the real value by reference stored at a given rank
      int get_value(double & value_, size_t index_ = 0) const;

      /// Get the string value by reference stored at a given rank
      int get_value(std::string & value_, size_t index_ = 0) const;

      /// Basic print
      void dump(std::ostream & out_) const;

      /// Convert to string and print in an output stream
      void to_string(std::ostream & out_) const;

      //! Method for smart printing (from the datatools::i_tree_dump interface).
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

    private:

      void clear_values_();

      void clear_unit_symbol_();

      int init_values_(char type_ = TYPE_INTEGER_SYMBOL,
                       size_t size_ = SCALAR_DEF);

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
    size_t size() const;

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
    const std::string & key(size_t) const;

    //! Set the list of keys.
    void keys(std::vector<std::string> &) const;

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

    //! Lock the property stored at the given key.
    void lock(const std::string & prop_key_);

    //! Unlock the property stored at the given key.
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
    size_t size(const std::string & prop_key_) const;

    //! Returns the size of the data stored with a given key/name
    size_t key_size(const std::string & prop_key_) const;

    //! Check if a property with given key/name exists
    bool has_key(const std::string & prop_key_) const;

    //! Get the description string associated to a property with given key/name
    const std::string & get_key_description (const std::string & prop_key_) const;

    //! Set the description string associated to a property with given key/name
    void set_key_description (const std::string & prop_key_, const std::string &desc_);

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

    //! Store a boolean 'true' property with a given key/name, a description string and a lock request
    void store_flag(const std::string & prop_key_, const std::string & desc_ = "",
                    bool lock_ = false);

    //! Store a boolean 'true' property with a given key/name
    /** @param prop_key_ The key of the boolean property
     */
    void set_flag(const std::string & prop_key_);

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


    //! Store a string property with a given key/name and value
    void store(const std::string & prop_key_, const std::string & value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a string property with a given key/name and value (C style)
    void store(const std::string & prop_key_, const char* value_,
               const std::string & desc_ = "", bool lock_ = false);

    //! Store a string property with a given key/name and value
    void store_string(const std::string & prop_key_, const std::string & value_,
                      const std::string & desc_ = "", bool lock_ = false);

    //! Store a path property with a given key/name and value
    void store_path(const std::string & prop_key_, const std::string & path_value_,
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


    //! Change the value of an existing boolean property with a given key/name and index
    void change(const std::string & key_, bool value_, size_t index_ = 0);

    //! Change the value of an existing boolean property with a given key/name and index
    void change_boolean(const std::string & key_, bool value_, size_t index_ = 0);

    //! Change the value of an existing scalar boolean property with a given key/name
    void change_boolean_scalar(const std::string & key_, bool value_);

    //! Change the value of an existing vector boolean property with a given key/name and index
    void change_boolean_vector(const std::string & key_, bool value_, size_t index_);

    //! Change the value of an existing integer property with a given key/name and index
    void change(const std::string & key_, int value_, size_t index_ = 0);

    //! Change the value of an existing integer property with a given key/name and index
    void change_integer(const std::string & key_, int value_, size_t index_ = 0);

    //! Change the value of an existing integer scalar property with a given key/name
    void change_integer_scalar(const std::string & key_, int value_);

    //! Change the value of an existing integer vector property with a given key/name and index
    void change_integer_vector(const std::string & key_, int value_, size_t index_);

    //! Change the value of an existing real property with a given key/name and index
    void change(const std::string & key_, double value_, size_t index_ = 0);

    //! Change the value of an existing real property with a given key/name and index
    void change_real(const std::string & key_, double value_, size_t index_ = 0);

    //! Change the value of an existing real scalar property with a given key/name
    void change_real_scalar(const std::string & key_, double value_);

    //! Change the value of an existing real vector property with a given key/name and index
    void change_real_vector(const std::string & key_, double value_, size_t index_);

    //! Change the value of an existing string property with a given key/name and index
    void change(const std::string & key_, const std::string & value_, size_t index_ = 0);

    //! Change the value of an existing string property (C style) with a given key/name and index
    void change(const std::string & key_, const char * value_, size_t index_ = 0);

    //! Change the value of an existing string property with a given key/name and index
    void change_string(const std::string & key_, const std::string & value_,
                       size_t index_ = 0);

    //! Change the value of an existing string scalar property with a given key/name
    void change_string_scalar(const std::string & key_, const std::string & value_);

    //! Change the value of an existing string vector property with a given key/name and index
    void change_string_vector(const std::string & key_, const std::string & value_,
                              size_t index_);

    //! Change the full contents of an existing boolean vector property with a given key/name
    void change(const std::string & key_, const data::vbool & values_);

    //! Change the full contents of an existing integer vector property with a given key/name
    void change(const std::string & key_, const data::vint & values_);

    //! Change the full contents of an existing real vector property with a given key/name
    void change(const std::string & key_, const data::vdouble & values_);

    //! Change the full contents of an existing string vector property with a given key/name
    void change(const std::string & key_, const data::vstring & values_);

    template <typename T>
    void update(const std::string& a_key, const T& value);

    //! Update a boolean flag with a given key/name and value
    void update_boolean(const std::string & key_, bool value_);

    //! Update a boolean flag to true with a given key/name
    void update_flag(const std::string & key_);

    //! Update an integer flag with a given key/name and value
    void update_integer(const std::string & key_, int value_);

    //! Update a real flag with a given key/name and value
    void update_real(const std::string & key_, double value_);

    //! Update a real flag with a given key/name and value
    void update_real_with_explicit_unit(const std::string & key_, double value_);

    //! Update a real flag with a given key/name and value
    void update_with_explicit_unit(const std::string & key_, double value_);

    //! Update a C-string flag with a given key/name and value
    void update(const std::string & key_, const char* value_);

    //! Update a string flag with a given key/name and value
    void update_string(const std::string & key_, const std::string & value);

    //! Fetch the boolean value stored with a given key/name and index
    void fetch(const std::string & key_, bool & value_, size_t index_ = 0) const;

   //! Fetch the boolean value stored with a given key/name and index
    bool fetch_boolean(const std::string &, size_t index_ = 0) const;

    //! Fetch the boolean scalar value stored with a given key/name
    bool fetch_boolean_scalar(const std::string & name_) const;

    //! Fetch the boolean vector value stored with a given key/name and index
    bool fetch_boolean_vector(const std::string & name_, size_t index_) const;

    //! Check if a boolean value with a given key/name exists with value 'true'
    bool has_flag(const std::string & key_) const;

    //! Fetch the integer value stored with a given key/name and index
    void fetch(const std::string & key_, int & value_, size_t index_ = 0) const;

     //! Fetch the integer value stored with a given key/name and index
    int fetch_integer(const std::string & name_, size_t index_ = 0) const;

    //! Fetch the integer scalar value stored with a given key/name
    int fetch_integer_scalar(const std::string & name_) const;

    //! Fetch the integer vector value stored with a given key/name and index
    int fetch_integer_vector(const std::string & name_ , size_t index_) const;

    //! Fetch the positive integer value stored with a given key/name and index
    unsigned int fetch_positive_integer(const std::string & name_, size_t index_ = 0) const;

    //! Fetch the strict positive integer value stored with a given key/name and index
    unsigned int fetch_strict_positive_integer(const std::string & name_, size_t index_ = 0) const;

    //! Fetch the ranged integer value stored with a given key/name and index
    int fetch_range_integer(const std::string & name_, int min_, int max_, size_t index_ = 0) const;


    //! Fetch the real value stored with a given key/name and index
    void fetch(const std::string & key_, double & value_, size_t index_ = 0) const;

    //! Fetch the real value stored with a given key/name and index
    double fetch_real(const std::string & name_, size_t index_ = 0) const;

    //! Fetch the real scalar value stored with a given key/name
    double fetch_real_scalar(const std::string & name_) const;

    //! Fetch the real vector value stored with a given key/name and index
    double fetch_real_vector(const std::string & name_, size_t index_) const;

    //! Fetch the dimensionless real value stored with a given key/name and index
    double fetch_dimensionless_real(const std::string & name_, size_t index_ = 0) const;

    //! Fetch the physical quantity (with its explicit unit) value stored with a given key/name and index
    double fetch_real_with_explicit_unit(const std::string & name_, size_t index_ = 0) const;

    //! Fetch the physical quantity (with its explicit dimension) value stored with a given key/name and index
    double fetch_real_with_explicit_dimension(const std::string & name_, const std::string & dimension_, size_t index_ = 0) const;


    //! Fetch the string value stored with a given key/name and index
    void fetch(const std::string & key_, std::string & value_,
               size_t index = 0) const;

    //! Fetch the string value stored with a given key/name and index
    std::string fetch_string(const std::string & name_, size_t index_ = 0) const;

    //! Fetch the string scalar value stored with a given key/name
    std::string fetch_string_scalar(const std::string & name_) const;

    //! Fetch the string vector value stored with a given key/name and index
    std::string fetch_string_vector(const std::string & name_, size_t index_) const;

    //! Fetch a file path from a string value stored with a given key/name and index
    std::string fetch_path(const std::string & name_, size_t index_ = 0) const;

    //! Fetch a file path from a string scalar value stored with a given key/name
    std::string fetch_path_scalar(const std::string & name_) const;

    //! Fetch a file path from a string vector value stored with a given key/name and index
    std::string fetch_path_vector(const std::string & name_, size_t index_) const;

    //! Fetch the boolean vector value stored with a given key/name
    void fetch(const std::string & key_, data::vbool & values_) const;

    //! Fetch the integer vector value stored with a given key/name
    void fetch(const std::string & key_, data::vint & values_) const;

    //! Fetch a set of integer values from the vector value stored with a given key/name
    void fetch(const std::string & key_, std::set<int> & values, bool allow_duplication_ = false) const;

    //! Fetch a set of unsigned integer values from the vector value stored with a given key/name
    void fetch_positive(const std::string & key_, std::set<unsigned int> & values_, bool allow_duplication_ = false) const;

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

    //! Fetch a single character value stored with a given key/name and index
    char fetch_one_character(const std::string & name_, size_t index_ = 0) const;

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

    //! Build a new property key from a prefix and a key
    static std::string build_property_key(const std::string & prefix_,
                                          const std::string & subkey_);

  private:
    /// Return reference to element at key, throwing if not present
    data& _get_valid_data_(const std::string& key);

    /// Return rconst eference to element at key, throwing if not present
    const data& _get_valid_data_(const std::string& key) const;

    /// Throw if key is already stored
    void _check_nokey_(const std::string& prop_key_) const;

    /// Throw if key does not meet requirements
    void _validate_key_(const std::string & prop_key_) const;

    /// Store (put) scalar value at key with optional description and lock
    template <typename T>
    void _store_scalar_impl_(const std::string& a_key, const T& value_,
                             const std::string& description_, bool a_lock);

    /// Store (put) vector value at key with optional description and lock
    template <typename T>
    void _store_vector_impl_(const std::string& a_key, const std::vector<T>& values,
                           const std::string& description, bool a_lock);

    /// Change scalar value at key/index
    template <typename T>
    void _change_scalar_impl_(const std::string& key, const T& value, size_t index);

    /// Change vector values at key
    template <typename T>
    void _change_vector_impl_(const std::string& a_key, const std::vector<T> values);

       /// Fetch (get) scalar value from key, index
    template <typename T>
    void _fetch_scalar_impl_(const std::string& a_key, T& value, size_t index) const;

    /// Fetch (get) vector value from key
    template <typename T>
    void _fetch_vector_impl_(const std::string& a_key, std::vector<T>& values) const;

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
  // properties::data class template method definitions
  template <>
  inline bool properties::data::is_type<bool>() const {return is_boolean();}

  template <>
  inline bool properties::data::is_type<int>() const {return is_integer();}

  template <>
  inline bool properties::data::is_type<double>() const {return is_real();}

  template <>
  inline bool properties::data::is_type<std::string>() const {return is_string();}

  template <>
  inline int properties::data::assign<bool>(size_t size_) {return boolean(size_);}

  template <>
  inline int properties::data::assign<int>(size_t size_) {return integer(size_);}

  template <>
  inline int properties::data::assign<double>(size_t size_) {return real(size_);}

  template <>
  inline int properties::data::assign<std::string>(size_t size_) {return string(size_);}

  template <>
  inline bool properties::data::defaults::default_value() {return defaults::boolean_value();}

  template <>
  inline int properties::data::defaults::default_value() {return defaults::integer_value();}

  template <>
  inline double properties::data::defaults::default_value() {return defaults::real_value();}

  template <>
  inline std::string properties::data::defaults::default_value() {return defaults::string_value();}

  //----------------------------------------------------------------------
  // properties class template method definitions
  //
  template <typename T>
  void properties::update(const std::string& a_key, const T& value) {
    //static_assert(can_hold_t_<T>::value, "property_set cannot hold values of type T");
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
    return;
  }

  template <class key_predicate>
  void properties::export_if(properties & props_,
                             const key_predicate & predicate_) const
  {
    DT_THROW_IF (this == &props_,
                 std::logic_error, "Self export is not allowed !");
    for (const auto& pair : _props_) {
      if (predicate_(pair.first)) {
        props_.store(pair.first, pair.second);
      }
    }
    return;
  }

  template <class key_predicate>
  void properties::export_not_if(properties & props_,
                                 const key_predicate & predicate_) const
  {
    export_if(props_, std::not1(predicate_));
    return;
  }

  template <typename T>
  void properties::_store_scalar_impl_(const std::string &a_key, const T &value_,
                                       const std::string &description_, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    data a_data(value_, data::SCALAR_DEF);
    a_data.set_description(description_);
    if (a_lock) a_data.lock();
    _props_[a_key] = a_data;
    return;
  }

  template <typename T>
  void properties::_store_vector_impl_(const std::string &a_key, const std::vector<T> &values,
                                       const std::string &description, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    size_t valsize = values.size();
    data a_data(data::defaults::default_value<T>(), valsize);
    a_data.set_description(description);
    if (a_lock) a_data.lock();

    for (size_t i{0}; i < valsize; ++i) {
      a_data.set_value(values[i], i);
    }

    _props_[a_key] = a_data;
    return;
  }

  template <typename T>
  void properties::_change_scalar_impl_(const std::string& a_key, const T& value, size_t index)
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    int error = data_ptr.set_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot change value for " << data_ptr.get_type_label() << " property '"
                << a_key << "' in properties described by '" << get_description() << "' : "
                << data::get_error_message(error) << "!");
    return;
  }

  template <typename T>
  void properties::_change_vector_impl_(const std::string& a_key, const std::vector<T> values)
  {
    auto& data_ptr = this->_get_valid_data_(a_key);

    // Some problems with logic that follows...
    // 1. resize() will clear data, even for locked data elements, so should surely check for locking *first*?
    // 2. Should not need the DT_THROW_IF in second for loop since everything will have been checked by this point.

    DT_THROW_IF(!data_ptr.is_type<T>() || !data_ptr.is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of " << data_ptr.get_type_label() << " in properties described by '" << get_description() << "' !");

    if (values.size() != data_ptr.get_size()) {
      int error = data_ptr.assign<T>(values.size());
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of " << data_ptr.get_type_label() << " property '"
                  << a_key << "': "
                  << data::get_error_message(error) << " in properties described by '" << get_description() << "' !");
    }
    for (size_t i{0}; i < values.size(); ++i) {
      int error = data_ptr.set_value(values[i], i);
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of booleans property '"
                  << a_key << "' in properties described by '" << get_description() << "': "
                  << data::get_error_message(error) << " !");
    }
    return;
  }
  template <typename T>
  void properties::_fetch_scalar_impl_(const std::string& a_key, T& value, size_t index) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    int error = data_ptr.get_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot fetch " << data_ptr.get_type_label() << "value from property '"
                << a_key << "' in properties described by '" << get_description() << "': "
                << data::get_error_message(error) << " !");
    return;
  }

  template <typename T>
  void properties::_fetch_vector_impl_(const std::string& a_key, std::vector<T>& values) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    DT_THROW_IF(!data_ptr.is_type<T>() || !data_ptr.is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of " << data_ptr.get_type_label() << " !");
    values.resize(data_ptr.size(), data::defaults::default_value<T>());
    for (size_t i{0}; i < values.size(); ++i) {
      T val;
      int error = data_ptr.get_value(val, i);
      // Logically, nothing can actually fail here because we have checked type, vector and size
      // so get_value will never throw, but we need to check the value
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot fetch a vector of " << data_ptr.get_type_label() << " from property '"
                  << a_key << "': " << data::get_error_message(error) << " !");
      values[i] = val;
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
