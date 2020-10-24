//! \file datatools/multi_properties.h
/* multi_properties.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009
 * Last modified: 2020-01-28
 *
 * License:
 *
 * Copyright (C) 2009-2020 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A container of properties objects implemented as a dictionnary
 *
 */

#ifndef DATATOOLS_MULTI_PROPERTIES_H
#define DATATOOLS_MULTI_PROPERTIES_H

// Standard Library:
#include <map>
#include <list>
#include <string>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

// This project:
#include <datatools/i_serializable.h>
#include <datatools/i_clear.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_cloneable.h>
#include <datatools/properties.h>
#include <datatools/bit_mask.h>
#include <datatools/logger.h>
#include <datatools/file_include.h>

namespace datatools {

  /*! \brief A container of mapped properties objects
   *
   * \code
   * #include <iostream>
   * #include <datatools/multi_properties.h>
   *
   * int main ()
   * {
   *   {
   *     datatools::multi_properties multi_parameters ("name", "type",
   *                                                   "Setup parameters of a networked random numbers server");
   *
   *     // Add a section for parameters dedicated to the network interface :
   *     datatools::properties & server_section = multi_parameters.add_section ("server", "server_type");
   *     server_section.store_flag ("verbose", "Verbose mode");
   *     server_section.store ("max_number_of_connexions", 5, "Maximum number of simultaneous connections");
   *     server_section.store ("port", 3456, "Port number");
   *     server_section.store ("protocol", "UDP", "Current protocol");
   *
   *     // Add a section for parameters dedicated to an embedded pseuso-random number generator :
   *     datatools::properties & prng_section = multi_parameters.add_section ("prng", "prng_type");
   *     prng_section.store ("seed", 314159, "initial PRNG seed");
   *     prng_section.store ("buffer_size", 1000, "Size of the buffer for pre-computed random numbers");
   *
   *     // Store all parameters in a ASCII test file :
   *     multi_parameters.write ("prng_server.conf");
   *   }
   *   {
   *      datatools::multi_properties multi_parameters;
   *      multi_parameters.read ("prng_server.conf");
   *      std::cout << "PRNG server configuration : " << std::endl;
   *      multi_parameters.print_tree(std::cout);
   *      // Check if a section with given name exists :
   *      if (multi_parameters.has_section ("server")) {
   *         const datatools::properties & server_section = multi_parameters.get_section ("server");
   *         // Read a setup parameter from the section :
   *         std::cout << "Port number : " << server_section.fetch_integer ("port");
   *      }
   *   }
   *   return 0;
   * }
   * \endcode
   */
  class multi_properties
    : public datatools::i_serializable
    , public datatools::i_clear
    , public datatools::i_tree_dumpable
    , public datatools::i_cloneable
  {
  public:
    /// \brief Default key/meta values
    struct defaults {
      /// Default label for primary keys
      static const std::string & key_label();
      /// Default label for meta information text
      static const std::string & meta_label();
    };


    //! \brief Inner class for section entry handle internal data stored within the dictionary of the multi_properties class.
    class entry
      : public datatools::i_tree_dumpable
    {
    public:

      /// Constructor
      entry(const std::string & key_ = "",
            const std::string & meta_ = "");

      virtual ~entry() = default;

      /// Return a const reference to the collection of properties
      const properties& get_properties() const;

      /// Return a mutable reference to the collection of properties
      properties& grab_properties();

      /// Return the primary key
      const std::string & get_key() const;

      /// Set the primary key
      void set_key(const std::string &);

      /// Return the meta information text
      const std::string & get_meta() const;

      /// Set the meta information text
      void set_meta(const std::string &);

      /// Check if meta information text is not empty
      bool has_meta() const;

      /// Smart print
      void print_tree(std::ostream & out_ = std::clog,
                      const boost::property_tree::ptree & options_ = empty_options()) const override;

      /// Smart print
      ///
      /// \deprecated
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

    private:
      std::string _key_;        //!< Primary key of the section
      std::string _meta_;       //!< Meta information text of the section
      properties  _properties_; //!< Container of properties stored in the section

      BOOST_SERIALIZATION_BASIC_DECLARATION()
    }; // multi_properties::entry


  public:
    /// Dictionary of section
    typedef std::map<std::string, entry> entries_col_type;

    /// List of handles on sections
    typedef std::list<entry*> entries_ordered_col_type;

  public:
    /// Default constructor
    multi_properties();

    /// Constructor specifying key label and meta label
    multi_properties(const std::string & key_label_,
                     const std::string & meta_label_);

    /// Constructor specifying key label, meta label and description
    multi_properties(const std::string & key_label_,
                     const std::string & meta_label_,
                     const std::string & description_);

    /// Destructor
    ~multi_properties() override = default;

    /// Copy constructor
    multi_properties(const multi_properties &);

    /// Assignment operator
    multi_properties & operator=(const multi_properties &);

    // Move constructor
    multi_properties(multi_properties &&) = default;

    // Move assignment
    multi_properties & operator=(multi_properties &&) = default;

    /// Check if a description is available
    bool has_description() const;

    /// Set the description
    void set_description(const std::string & description_);

    /// Get the description
    const std::string & get_description() const;

    /// Check if the key label is set
    bool has_key_label() const;

    /// Set the key label
    void set_key_label(const std::string & key_label_);

    /// Clear the key label
    void clear_key_label();

    /// Return the key label
    const std::string & get_key_label() const;

    /// Check if the meta label is set
    bool has_meta_label() const;

    /// Set the meta label
    void set_meta_label(const std::string & meta_label_);

    /// Clear the meta label
    void clear_meta_label();

    /// Return the meta label
    const std::string & get_meta_label() const;

    /// Return the number of entries
    size_t size() const;

    /// Check if the collection of entries is empty
    bool empty() const;

    /// Reset
    void reset();

    /// Clear the dictionary of sections
    void clear() override;

    /// Check if a section with a given key exists
    bool has_key(const std::string & key_) const;

    /// Check if a section with given key and meta exists
    bool has_key_with_meta(const std::string & key_, const std::string & meta_) const;

    //! Returns the ith key
    const std::string & key(size_t) const;

    /// Return an array of keys
    std::vector<std::string> keys() const;

    /// Build an array of keys
    void keys(std::vector<std::string> & keys_) const;

    //! Returns the ith ordered key
    const std::string & ordered_key(size_t) const;

    /// Return an array of orderered keys
    std::vector<std::string> ordered_keys() const;

    /// Build an array of orderered keys
    void ordered_keys(std::vector<std::string> & keys_) const;

    /// Check if a section exists
    bool has_section(const std::string & key_) const;

    /// Return the const reference to the properties store in a section
    const properties & get_section(const std::string & key_) const;

    /// Return the mutable reference to the properties store in a section
    properties & grab_section(const std::string & key_);

    /// Return a const reference to the stored entry
    const entry& get(const std::string & key_) const;

    /// Return a mutable reference to the stored entry
    entry& grab(const std::string & key_);

    /// Return the const reference to the collection of entries
    const entries_col_type& entries() const;

    /// Return the const reference to the ordered collection of entries
    const entries_ordered_col_type& ordered_entries() const;

    /// Add a new section with primary key, meta information text and a collection of properties
    void add(const std::string & key_,
             const std::string & meta_,
             const properties & props_);

    /// Add a new section with primary key and a collection of properties
    void add(const std::string & key_,
             const properties & props_);

    /// Add an empty section with primary key and meta information text
    void add(const std::string & key_,
             const std::string & meta_ = "");

    /// Add a new section with primary key and meta information text and return a mutable reference to the store collection of properties
    properties & add_section(const std::string & key_,
                             const std::string & meta_ = "");

    /// Remove a section
    void remove(const std::string & key_);

    /// Write to a configuration file
    void write(const std::string & filename_,
               uint32_t options_ = 0) const;

    /// Read a multi_properties container from an input stream
    ///
    /// This method is the base of configuration file parsing.
    ///
    /// Example of input file to be parsed:
    /// \code
    ///
    /// # Format:
    /// # - Comment lines start with a '#'.
    /// # This is a  comment line...
    /// # ...and this is another one.
    ///
    /// # - Blank lines are ignored.
    /// # - Lines starting with '#@' are considered as optional metacomments
    /// #   with special embedded parsing options and/or actions.
    ///
    /// #@description The main configuration file for the application
    /// #  This optional metacomment provides the general description of the multi_properties container.
    /// #  This directive must be given before any *section* (see below).
    ///
    /// #@key_label "name"
    /// #  This metacomment provides the key use to identify the name of a *section*.
    /// #  (see below).
    /// #  Syntax is ``[name="foo" ... ]``.
    /// #  This directive must be given before any *section* (see below).
    ///
    /// #@meta_label "type"
    /// #  This metacomment provides the key use to identify the meta information of a *section*
    /// #  (see below).
    /// #  Syntax is ``[name="foo" type="my_model_1" ]``.
    /// #  This directive must be given before any *section* (see below).
    ///
    /// #@variant_devel
    /// #  This metacomment activates development logging from the variant mechanism
    ///
    /// #@variant_section_only "variant-expression"
    /// #  This metacomment corresponds to a directive which accept a given section only if
    ///
    /// #@forbid_include
    /// #  This metacomment inhibits the file inclusion inclusion mechanism
    ///
    /// #@include_debug
    /// #  This metacomment activates debug logging from the file inclusion mechanism
    ///
    /// #@include_dir "path"
    /// #  This metacomment specifies a directory from which files to be included are searched for.
    ///
    /// #@include_path_env "name"
    /// #  This metacomment specifies the name of an environment variable which contains
    /// #  an ordered list of priority directories from which files to be included are searched for.
    ///
    /// #@include_sections "path"
    /// #  This metacomment specifies the path of a file to be included
    ///
    /// #@include_sections_try "path"
    /// #  This metacomment specifies the path of a file to be included if possible
    ///
    // # NOT SUPPORTED"
    // #@include_path_env_strategy "label"
    // #  This metacomment specifies the strategy for resolving the include directories from the
    // #  environment variable set by the "@include_path_env" directive. Supported strategies are:
    // #  - prepend (default) : directories from the environment variable have priority on explicit
    // #    directories set through "@include_dir" directive.
    // #  - append : directories from the environment variable do not have priority on explicit
    // #    directories set through "@include_dir" directive.
    // #  - clear : directories from the environment variable are the only ones used.
    //
    ///
    /// # Sections:
    /// # Each section in a "multi_properties" container implements a single "properties" container
    /// # associated to a unique key/identifier (in the scope of the "multi_properties" container)
    /// # and eventually a meta information, usually documenting the type/category of object
    /// # or component the section refers to.
    ///
    /// [name="core"  type="algo::calibration"]
    /// #  The directive above starts a new section with a given *name* and *type* (meta information)
    ///
    /// ...
    ///
    /// [name="display"  type="algo::calibration::gui"]
    /// #  The directive above starts a new section with a given *name* and *type* (meta information)
    ///
    /// ...
    ///
    /// # Each section contains a set of properties as illustrated in the ``properties`` class.
    ///
    /// [name="debug"  type="algo::calibration::debugger"]
    /// #  This directive above starts the section named 'debug' for a component
    /// #  of which the type is ``algo::calibration::debugger``.
    ///
    /// #@description Verbosity level of the debugger
    /// verbosity : integer = 4
    ///
    /// #@description List of components with active debugging support
    /// active : string[2] = "core" "display"
    ///
    /// [name="log"  type="algo::calibration::logger"]
    /// #  The directive above starts a new section with a given *name* and *type* (meta information)
    ///
    /// # This directive include the definitions of some properties of the current section
    /// #@include "~/.config/logger/main.conf"
    ///
    /// [name="foo"  type="algo::foo"]
    /// #@variant_section_only "variant-expression"
    /// #  The directive above corresponds to a directive which accepts a given section only if
    /// #  a the variant-expression is true. It must be placed at the very beginning of a section.
    ///
    /// # The following directives include some multi-properties files and merge their contents
    /// # in the current one. Existing sections are merged and their properties are overriden
    /// # if they appear both in the current and includes files.
    /// #@include_sections "more_sections.conf"
    /// #@include_sections "overriden_sections.conf"
    ///
    /// # Same as the former directive but failure to resolve the file to be include
    /// # does not fail the parsing.
    /// #@include_sections_try "optional_sections.conf"
    ///
    /// \endcode
    ///
    void read(const std::string & filename_, uint32_t options_ = 0);

    /// Merge with another multi_properties with overriding possibilities
    ///
    /// If the allow_override_sections_ flag is set, any section key existing in both multi_propreties
    /// as records of the same type is overriden by the full section stored in other_.
    /// If the allow_override_section_ flag is set, any overridenkey existing in both propreties as records
    /// of the same type is overriden by the value stored in other_.
    void merge_with(const multi_properties & other_,
                    bool allow_override_sections_ = false,
                    bool allow_override_props_ = false);

    /// Basic print
    void dump(std::ostream & out_ = std::clog) const;

    /// Smart print
    void print_tree(std::ostream & out_ = std::clog,
                    const boost::property_tree::ptree & options_ = empty_options()) const override;

    /// Smart print
    ///
    /// \deprecated
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  private:
    /// Private initialization
    void _init_ (const std::string & key_label_,
                 const std::string & meta_label_,
                 const std::string & description_);

    /// Private copy
    void _copy_impl_(const multi_properties &);

    /// Add section implementation
    properties & add_impl2(const std::string & key_,
                          const std::string & meta_ = "");

  private:
    std::string           _description_; //!< Description of the container
    std::string           _key_label_;   //!< The key label used by the container
    std::string           _meta_label_;  //!< The meta label used by the container
    entries_col_type      _entries_;     //!< List of stored properties objects (unordered)
    entries_ordered_col_type _ordered_entries_; //!< List of ordered properties objects

    //! Cloneable interface
    DATATOOLS_CLONEABLE_DECLARATION(multi_properties)

    //! Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(multi_properties)
    DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()

#ifndef Q_MOC_RUN
    //! Reflection interface
    DR_CLASS_RTTI()
#endif // Q_MOC_RUN

  };

} // end of namespace datatools

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_DECLARATION(::datatools::multi_properties)
// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::datatools::multi_properties)

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(datatools::multi_properties, "datatools::multi_properties")

#ifndef Q_MOC_RUN
// Activate reflection layer for the 'datatools::multi_properties' class:
DR_CLASS_INIT(::datatools::multi_properties)
#endif // Q_MOC_RUN

// Explicit class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::multi_properties, 2)

#endif // DATATOOLS_MULTI_PROPERTIES_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
