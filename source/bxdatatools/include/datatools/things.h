/* things.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-03-08
 * Last modified : 2013-02-18
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A serializable container for arbitrary serializable objects
 *
 *   Storable objects must :
 *   - have a default constructor
 *   - inherit from the datatools::i_serializable interface
 *   - have a 'serialize' template method (ala Boost)
 *
 * History:
 *
 */
#ifndef DATATOOLS_THINGS_H_
#define DATATOOLS_THINGS_H_
// Standard Library
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

// Third Party
// - Boost
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/serialization/access.hpp>

// This Project
#include <datatools/i_serializable.h>
#include <datatools/serialization_macros.h>

#include <datatools/i_clear.h>
#include <datatools/i_tree_dump.h>

//! \file datatools/things.h
namespace datatools {

class things;

/* class bad_things_cast */
//! \brief An exception for invalid cast operation within the things class.
class bad_things_cast : public std::exception {
 public:
  explicit bad_things_cast (const std::string& message);
  virtual ~bad_things_cast() throw();

  virtual const char* what() const throw();

 private:
  std::string message_;
};


/* class things */

//! \brief A generic serializable and noncopyable container for arbitrary serializable objects.
class things :
    DATATOOLS_SERIALIZABLE_CLASS,
    public datatools::i_tree_dumpable,
    public datatools::i_clear,
    private boost::noncopyable {
 public:
  //! \brief Internal entry for serializable object stored in the thing class.
  struct entry_type : public datatools::i_tree_dumpable {
    static const char MASK_CONST = 0x1;
   public:
    entry_type();
    virtual ~entry_type();

    bool is_not_const() const;

    bool is_const() const;

    void set_const(bool a_const = true);

    void set_description(const std::string&);

    const std::string& get_description() const;

    bool has_description() const;

    virtual void tree_dump(std::ostream& a_out = std::clog,
                           const std::string& a_title  = "",
                           const std::string& a_indent = "",
                           bool a_inherit = false) const;

   public:
    std::string                 description;
    uint8_t                     flags;
    datatools::i_serializable * handle;

    BOOST_SERIALIZATION_BASIC_DECLARATION ();

 };

  // Begin the things declarations...
 public:
  static const bool constant; //     = true;
  static const bool non_constant; // = !constant;
  static const bool copyable; //     = true;
  static const bool noncopyable; //  = !copyable;

  typedef std::map<std::string, entry_type> dict_type;

 public:
  /// Default constructor
  things();

  /// Constructor with name and description
  things(const std::string& a_name, const std::string& a_description = "");

  // Destructor
  virtual ~things();

  const std::string& get_name() const;

  things& set_name(const std::string&);

  const std::string& get_name_impl() const;

  void set_name_impl(const std::string&);

  bool has_name() const;

  bool has_name_impl() const;

  const std::string& get_description() const;

  things& set_description(const std::string&);

  void set_description_impl(const std::string&);

  const std::string& get_description_impl() const;

  bool has_description_impl() const;

  void reset();

  virtual void clear();

  unsigned int size() const;

  bool empty() const;

  bool has(const std::string& a_name) const;

  bool has_serial_tag(const std::string& a_name,
                      const std::string& a_serial_tag) const;

  bool is_constant(const std::string& a_name) const;

  bool is_mutable(const std::string& a_name) const;

  //! not implemented :
  void set_constant(const std::string& a_name, bool a_const = true);

  const std::string& get_description(const std::string& a_name) const;

  const std::string& get_entry_description(const std::string& a_name) const;

  void set_description(const std::string& name, const std::string& desc);

  void set_entry_description(const std::string& name, const std::string& desc);

  void get_names(std::vector<std::string>& names) const;

  const std::string& get_entry_name(int index_) const;

  void remove(const std::string & a_name);

  void erase(const std::string & a_name);

  template<class T>
  T& add(const std::string& a_name, const std::string& a_desc = "",
         bool a_const = false);

  template<class T>
  T* pop(const std::string& a_name);

  template<class T>
  T& grab(const std::string& a_name);

  template<class T>
  bool is_a(const std::string& a_name) const;

  template<class T>
  const T& get(const std::string& a_name) const;

  const datatools::i_serializable &
  get_entry (const std::string& a_name) const;

  datatools::i_serializable &
  grab_entry (const std::string& a_name);

  datatools::i_serializable &
  add_entry (const std::string& a_name,
	     const std::string& a_serial_tag,
	     const std::string& a_description = "",
	     bool a_const = false);

  datatools::i_serializable &
  add_entry_impl (const std::string& a_name,
		  const std::string& a_serial_tag,
		  const std::string& a_description,
		  bool a_const);

  bool entry_is_a (const std::string& a_name, const std::string&) const;

  bool entry_is_introspectable (const std::string& a_name) const;

  std::string get_entry_serial_tag (const std::string& a_name) const;

  std::string get_entry_introspection_id (const std::string& a_name) const;

  virtual void tree_dump(std::ostream& a_out = std::clog,
                         const std::string& a_title  = "",
                         const std::string& a_indent = "",
                         bool a_inherit = false) const;

  void dump(std::ostream& a_out = std::clog) const;

  datatools::i_serializable &
    grab(const std::string& a_name, const std::string& a_serial_tag= "");

  const datatools::i_serializable &
    get(const std::string& a_name, const std::string& a_serial_tag= "") const;

 private:

  void add_impl(const std::string& a_name,
		datatools::i_serializable* a_obj,
		const std::string& a_desc = "",
		bool a_const = false);

 private:
  // Attributes :
  std::string name_;        //!< The name of the container
  std::string description_; //!< The description of the container
  dict_type   things_;      //!< The internal dictionary of objects

  //! Serialization interface
  DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(things);
  DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT();

  //! Reflection interface
  DR_CLASS_RTTI();

};

}  // end of namespace datatools

// Template methods
#include <datatools/things-inl.h>

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_DECLARATION(::datatools::things);
// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::datatools::things);

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(datatools::things,"datatools::things");

// Activate reflection layer for the 'datatools::things' class:
DR_CLASS_NONCOPYABLE_INIT(::datatools::things);

#endif // DATATOOLS_THINGS_H_

