// -*- mode: c++; -*-
//! \file datatools/utils/multi_properties.h
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
#ifndef DATATOOLS_UTILS_MULTI_PROPERTIES_H_
#define DATATOOLS_UTILS_MULTI_PROPERTIES_H_
// Standard Library
#include <map>
#include <list>
#include <string>

// Third Party
// - Boost
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

// This Project
#include <datatools/serialization/i_serializable.h>
#include <datatools/utils/i_clear.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/i_cloneable.h>
#include <datatools/utils/properties.h>

namespace datatools {
namespace utils {

class multi_properties :    
    public datatools::serialization::i_serializable,
    public datatools::utils::i_clear,
    public datatools::utils::i_tree_dumpable,
    public datatools::utils::i_cloneable {
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
  //static const std::string SERIAL_TAG;

  static bool g_debug;

 public:
  // entry class (inner class)
  class entry :    
      public datatools::serialization::i_serializable,
      public datatools::utils::i_tree_dumpable {
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
    DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()

    friend class boost::serialization::access; 
    BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

   private:
    std::string key_;
    std::string meta_;
    properties properties_;
  }; // multi_properties::entry


 public:
  typedef std::map<std::string, entry> entries_col_t;
  typedef std::list<entry*> entries_ordered_col_t;

 public:
  multi_properties(const std::string& a_key_label = "", 
                   const std::string& a_meta_label = "", 
                   const std::string& a_description = "", 
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

  size_t size() const;

  void reset();

  virtual void clear();

  const entries_col_t& entries() const;

  const entries_ordered_col_t& ordered_entries() const;

  const entry& get(const std::string& a_key) const;

  entry& get(const std::string& a_key);

  bool has_key(const std::string& a_key) const;

  bool has_section(const std::string& a_key) const;

  const properties& get_section(const std::string& a_key) const;

  properties& get_section(const std::string& a_key);

  void add(const std::string& a_key, 
           const std::string& a_meta,
           const properties& a_props);

  void add(const std::string& a_key, 
           const properties& a_props);

  void add(const std::string& a_key, 
           const std::string& a_meta = "");

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

 public:
  //! From the datatools::utils::i_cloneable interface :
  DATATOOLS_CLONEABLE_DECLARATION(multi_properties)

  DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()


 private:
  friend class boost::serialization::access; 
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()
  //BOOST_SERIALIZATION_SPLIT_MEMBER_SERIALIZE_DECLARATIONS()

 private:
  void remove_impl(const std::string& a_key);

  void add_impl(const std::string& a_key, 
                const std::string& a_meta = "");

  void read_impl(std::istream& a_in, bool a_skip_private);

 private:
  bool                  debug_;
  std::string           description_;
  std::string           key_label_;
  std::string           meta_label_;
  entries_col_t         entries_;
  entries_ordered_col_t ordered_entries_;
};

} // end of namespace utils 
} // end of namespace datatools 


#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::multi_properties, "datatools::utils::multi_properties")

#endif // DATATOOLS_UTILS_MULTI_PROPERTIES_H_

