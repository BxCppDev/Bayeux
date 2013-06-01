// -*- mode: c++; -*-
//! \file datatools/multi_properties.h
/* multi_properties.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009
 * Last modified: 2011-03-09
 *
 * License:
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_MULTI_PROPERTIES_H_
#define DATATOOLS_MULTI_PROPERTIES_H_
// Standard Library
#include <map>
#include <list>
#include <string>

// Third Party
// - Boost
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

// This Project
#include <datatools/i_serializable.h>
#include <datatools/i_clear.h>
#include <datatools/i_tree_dump.h>
#include <datatools/i_cloneable.h>
#include <datatools/properties.h>

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
    DATATOOLS_SERIALIZABLE_CLASS,
    public datatools::i_clear,
    public datatools::i_tree_dumpable,
    public datatools::i_cloneable {
 public:
  static const char OPEN;
  static const char CLOSE;
  static const char COMMENT;
  static const std::string DEFAULT_KEY_LABEL;
  static const std::string DEFAULT_META_LABEL;
  static const bool with_header_footer;
  static const bool without_header_footer;
  static const bool write_public_only;
  static const bool write_private_also;
  static const bool read_public_only;
  static const bool read_private_also;

 public:
  //! \brief Internal data stored within the dictionary of the multi_properties class.
  class entry : public datatools::i_tree_dumpable {
   public:
    entry(const std::string& a_key = "",
          const std::string& a_meta = "");

    virtual ~entry();

    const properties& get_properties() const;

    properties& get_properties ();

    const std::string& get_key() const;

    void set_key(const std::string&);

    const std::string& get_meta() const;

    void set_meta(const std::string&);

    bool has_meta() const;

    virtual void tree_dump(std::ostream& a_out = std::clog,
                           const std::string& a_title = "",
                           const std::string& a_oindent = "",
                           bool a_inherit = false) const;

   private:
    std::string key_;
    std::string meta_;
    properties properties_;

    BOOST_SERIALIZATION_BASIC_DECLARATION();

  }; // multi_properties::entry


 public:
  typedef std::map<std::string, entry> entries_col_type;
  typedef std::list<entry*> entries_ordered_col_type;

private:

  void _init_ (const std::string& a_key_label,
               const std::string& a_meta_label,
               const std::string& a_description,
               bool a_debug);

 public:

  multi_properties();

  multi_properties(const std::string& a_key_label,
                   const std::string& a_meta_label);

  multi_properties(const std::string& a_key_label,
                   const std::string& a_meta_label,
                   const std::string& a_description,
                   bool a_debug = false);

  virtual ~multi_properties();

  bool is_debug() const;

  void set_debug(bool = true);

  void set_description(const std::string& a_description);

  const std::string& get_description() const;

  void set_key_label(const std::string& a_key_label);

  const std::string& get_key_label() const;

  void set_meta_label(const std::string& a_meta_label);

  const std::string& get_meta_label() const;

  uint32_t size() const;

  bool empty () const;

  void reset();

  virtual void clear();

  const entries_col_type& entries() const;

  const entries_ordered_col_type& ordered_entries() const;

  const entry& get(const std::string& a_key) const;

  entry& get(const std::string& a_key);

  bool has_key(const std::string& a_key) const;

  //! Returns the ith key
  const std::string & key (int) const;

  //! Returns the ith ordered key
  const std::string & ordered_key (int) const;

  std::vector<std::string> keys () const;

  void keys(std::vector<std::string>&k) const;

  std::vector<std::string> ordered_keys () const;

  void ordered_keys(std::vector<std::string>&k) const;

  bool has_section(const std::string& a_key) const;

  const properties& get_section(const std::string& a_key) const;

  const properties& get_section_const(const std::string& a_key) const;

  properties& get_section(const std::string& a_key);

  properties& grab_section(const std::string& a_key);

  void add(const std::string& a_key,
           const std::string& a_meta,
           const properties& a_props);

  void add(const std::string& a_key,
           const properties& a_props);

  void add(const std::string& a_key,
           const std::string& a_meta = "");

  properties& add_section(const std::string& a_key,
                          const std::string& a_meta);

  void remove(const std::string& a_key);

  void write(const std::string& a_filename,
             bool a_header_footer = true,
             bool a_write_private = false) const;

  void read(const std::string& a_filename,
            bool a_skip_private = false);

  void dump(std::ostream& a_out = std::clog) const;

  virtual void tree_dump(std::ostream& a_out         = std::clog,
                         const std::string& a_title  = "",
                         const std::string& a_indent = "",
                         bool a_inherit          = false) const;

 private:
  void remove_impl(const std::string& a_key);

  void add_impl(const std::string& a_key,
                const std::string& a_meta = "");

  properties& add_impl2(const std::string& a_key,
                        const std::string& a_meta = "");

  void read_impl(std::istream& a_in, bool a_skip_private);

 private:
  bool                  debug_; /// Debug flag
  std::string           description_; /// Description of the container
  std::string           key_label_; /// The key label used by the container
  std::string           meta_label_; /// The meta label used by the container
  entries_col_type         entries_; /// List of stored properties objects (unordered)
  entries_ordered_col_type ordered_entries_; /// List of ordered properties objects

  //! Cloneable interface
  DATATOOLS_CLONEABLE_DECLARATION(multi_properties);

  //! Serialization interface
  DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(multi_properties);
  DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT();

#if DATATOOLS_WITH_REFLECTION == 1
  //! Reflection interface
  DR_CLASS_RTTI();
#endif

};

} // end of namespace datatools

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_DECLARATION(::datatools::multi_properties);
// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::datatools::multi_properties);

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(datatools::multi_properties, "datatools::multi_properties")

#if DATATOOLS_WITH_REFLECTION == 1
// Activate reflection layer for the 'datatools::multi_properties' class:
DR_CLASS_INIT(::datatools::multi_properties);
#endif // DATATOOLS_WITH_REFLECTION

#endif // DATATOOLS_MULTI_PROPERTIES_H_

