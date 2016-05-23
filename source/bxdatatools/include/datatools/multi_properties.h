//! \file datatools/multi_properties.h
/* multi_properties.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009
 * Last modified: 2016-05-20
 *
 * License:
 *
 * Copyright (C) 2011-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * History:
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
   *      multi_parameters.tree_dump (std::cout, "PRNG server configuration : ");
   *      // Check if a section with given name exists :
   *      if (multi_parameters.has_section ("server"))
   *        {
   *           const datatools::properties & server_section = multi_parameters.get_section ("server");
   *           // Read a setup parameter from the section :
   *           std::cout << "Port number : " <<  server_section.fetch_integer ("port");
   *        }
   *   }
   *   return 0;
   * }
   * \endcode
   */
  class multi_properties :
    public datatools::i_serializable,
    public datatools::i_clear,
    public datatools::i_tree_dumpable,
    public datatools::i_cloneable {
  public:

    /// \brief Default values
    struct defaults {
      /// Default label for primary keys
      static const std::string & key_label();
      /// Default label for meta information text
      static const std::string & meta_label();
    };

  public:
    //! \brief Section entry handle internal data stored within the dictionary of the multi_properties class.
    class entry : public datatools::i_tree_dumpable {
    public:

      /// Constructor
      entry(const std::string& a_key = "",
            const std::string& a_meta = "");

      /// Destructor
      virtual ~entry();

      /// Return a const reference to the collection of properties
      const properties& get_properties() const;

      /// Return a mutable reference to the collection of properties
      properties& grab_properties();

      /// Return the primary key
      const std::string& get_key() const;

      /// Set the primary key
      void set_key(const std::string&);

      /// Return the meta information text
      const std::string& get_meta() const;

      /// Set the meta information text
      void set_meta(const std::string&);

      /// Check if meta information text is not empty
      bool has_meta() const;

      /// Smart print
      virtual void tree_dump(std::ostream& a_out = std::clog,
                             const std::string& a_title = "",
                             const std::string& a_oindent = "",
                             bool a_inherit = false) const;

    private:
      std::string key_;        //!< Primary key of the section
      std::string meta_;       //!< Meta information text of the section
      properties  properties_; //!< Container of properties stored in the section

      BOOST_SERIALIZATION_BASIC_DECLARATION()

    }; // multi_properties::entry


  public:

    /// Dictionary of section
    typedef std::map<std::string, entry> entries_col_type;

    /// List of handles on sections
    typedef std::list<entry*> entries_ordered_col_type;

  private:

    /// Private initialization
    void _init_ (const std::string& a_key_label,
                 const std::string& a_meta_label,
                 const std::string& a_description,
                 bool a_debug);

    /// Private copy
    void _copy_impl_(const multi_properties &);

  public:

    /// Default constructor
    multi_properties();

    /// Constructor specifying key label and meta label
    multi_properties(const std::string& a_key_label,
                     const std::string& a_meta_label);

    /// Constructor specifying key label, meta label, description and debug flag
    multi_properties(const std::string& a_key_label,
                     const std::string& a_meta_label,
                     const std::string& a_description,
                     bool a_debug = false);

    /// Destructor
    virtual ~multi_properties();

    /// Copy constructor
    multi_properties(const multi_properties &);

    /// Assignment operator
    multi_properties & operator=(const multi_properties &);

    /// Check the debug flag
    bool is_debug() const;

    /// Set the debug flag
    void set_debug(bool = true);

    /// Set the description
    void set_description(const std::string& a_description);

    /// Check if a description is available
    bool has_description() const;

    /// Get the description
    const std::string& get_description() const;

    /// Set the key label
    void set_key_label(const std::string& a_key_label);

    /// Return the key label
    const std::string& get_key_label() const;

    /// Set the meta label
    void set_meta_label(const std::string& a_meta_label);

    /// Return the meta label
    const std::string& get_meta_label() const;

    /// Return the number of entries
    uint32_t size() const;

    /// Check if the collection of entries is empty
    bool empty() const;

    /// Reset
    void reset();

    /// Clear the dictionary of sections
    virtual void clear();

    /// Return the const reference to the collection of entries
    const entries_col_type& entries() const;

    /// Return the const reference to the ordered collection of entries
    const entries_ordered_col_type& ordered_entries() const;

    /// Return a const reference to the stored entry
    const entry& get(const std::string& a_key) const;

    /// Return a mutable reference to the stored entry
    entry& grab(const std::string& a_key);

    /// Check if a section with a given key exists
    bool has_key(const std::string& a_key) const;

    /// Check if a section with given key and meta exists
    bool has_key_with_meta(const std::string& a_key, const std::string& a_meta) const;

    //! Returns the ith key
    const std::string & key(int) const;

    //! Returns the ith ordered key
    const std::string & ordered_key(int) const;

    /// Return an array of keys
    std::vector<std::string> keys() const;

    /// Build an array of keys
    void keys(std::vector<std::string>&k) const;

    /// Return an array of orderered keys
    std::vector<std::string> ordered_keys() const;

    /// Build an array of orderered keys
    void ordered_keys(std::vector<std::string>&k) const;

    /// Check if a section exists
    bool has_section(const std::string& a_key) const;

    /// Return the const reference to the properties store in a section
    const properties& get_section(const std::string& a_key) const;

    /// Return the const reference to the properties store in a section
    const properties& get_section_const(const std::string& a_key) const;

    /// Return the mutable reference to the properties store in a section
    properties& grab_section(const std::string& a_key);

    /// Add a new section with primary key, meta information text and a collection of properties
    void add(const std::string& a_key,
             const std::string& a_meta,
             const properties& a_props);

    /// Add a new section with primary key and a collection of properties
    void add(const std::string& a_key,
             const properties& a_props);

    /// Add an empty section with primary key and meta information text
    void add(const std::string& a_key,
             const std::string& a_meta = "");

    /// Add a new section with primary key and meta information text and return a mutable reference to the store collection of properties
    properties& add_section(const std::string& a_key,
                            const std::string& a_meta = "");

    /// Remove a section
    void remove(const std::string& a_key);

    /// Write to a configuration file
    void write(const std::string& filename,
               uint32_t options = config::HEADER_FOOTER | config::SKIP_PRIVATE_SECTIONS) const;

    /// Read from a configuration file
    void read(const std::string& filename, uint32_t options = 0);

    /// Basic print
    void dump(std::ostream& a_out = std::clog) const;

    /// Smart print
    virtual void tree_dump(std::ostream& a_out         = std::clog,
                           const std::string& a_title  = "",
                           const std::string& a_indent = "",
                           bool a_inherit          = false) const;

    /// \brief Reader/writer class for multi_properties objects
    class config {
    public:
      enum options_flag {
        SKIP_PRIVATE_SECTIONS = bit_mask::bit00, ///< Skip private sections bit
        FORBID_VARIANTS       = bit_mask::bit01, ///< Forbid variant block directives bit
        LOG_MUTE              = bit_mask::bit02, ///< Mute mode activation bit
        LOG_DEBUG             = bit_mask::bit03, ///< Debug mode activation bit
        LOG_TRACE             = bit_mask::bit04, ///< Trace mode activation bit
        SKIP_PRIVATE_PROPS    = bit_mask::bit05, ///< Skip private properties in sections bit
        HEADER_FOOTER         = bit_mask::bit06, ///< Use header/footer (write)
        DONT_CLEAR            = bit_mask::bit07, ///< Don't clear before parsing bit (read)
        REQUESTED_TOPIC       = bit_mask::bit08, ///< Requested topic (read/write)
        RESOLVE_PATH          = bit_mask::bit09  ///< Resolve path for input filename (read/write)
      };

      /// Default constructor
      config(uint32_t options_ = 0, const std::string & topic_ = "");

      /// Return the logging priority threshold
      datatools::logger::priority get_logging() const;

      /// Set the logging priority threshold
      void set_logging(datatools::logger::priority);

      /// Reset the reader/writer
      void reset();

      /// Read from an input file
      void read(const std::string & a_filename, multi_properties& target_);

      /// Read from an input stream
      void read(std::istream& a_in, multi_properties& target_);

      /// Write to an output stream
      void write(std::ostream& a_out, const multi_properties& source_);

      /// Write to an output file
      void write(const std::string & a_filename, const multi_properties& source_);

      /// Check if topic is set
      bool has_topic() const;

      /// Set the topic that should be matched
      void set_topic(const std::string & topic);

      /// Return the topic
      const std::string & get_topic() const;

    protected:

      /// Default initialization
      void _init_defaults();

      /// Read from an input stream
      void _read(std::istream& a_in, multi_properties& target_);

      /// Write to an output stream
      void _write(std::ostream& a_out, const multi_properties& target_);

      /// Store the current filename
      void _set_current_filename(const std::string & filename_);

    private:

      // Configuration:
      datatools::logger::priority _logging_; ///< Logging priority threshold (read/write)
      bool _skip_private_sections_;   ///< Flag to skip private sections (read/write)
      bool _skip_private_properties_; ///< Flag to skip private properties in sections (read/write)
      bool _forbid_variants_;         ///< Flag to forbid variant directives (read)
      bool _header_footer_;           ///< Flag to print header/footer (write)
      bool _requested_topic_;         ///< Flag to activate topic matching (read)
      std::string _topic_;            ///< Topic to be validated
      bool _resolve_path_;            ///< Explicitely resolve path for input/output filenames (read/write)

      // Working parsing data:
      std::string _current_filename_;    ///< Current filename
      int         _current_line_number_; ///< Current line number

    }; //----- end of class config

  private:

    /// Remove section implementation
    void remove_impl(const std::string& a_key);

    /// Add section implementation
    void add_impl(const std::string& a_key,
                  const std::string& a_meta = "");

    /// Add section implementation
    properties& add_impl2(const std::string& a_key,
                          const std::string& a_meta = "");

    // /// @deprecated Read section implementation
    // void read_impl(std::istream& a_in, bool a_skip_private);

    /// Set default values at construction
    void init_defaults();

  private:
    bool                  debug_;       //!< Debug flag
    std::string           description_; //!< Description of the container
    std::string           key_label_;   //!< The key label used by the container
    std::string           meta_label_;  //!< The meta label used by the container
    entries_col_type      entries_;     //!< List of stored properties objects (unordered)
    entries_ordered_col_type ordered_entries_; //!< List of ordered properties objects

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

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
