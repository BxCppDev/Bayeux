/// \file datatools/things.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-03-08
 * Last modified : 2014-06-18
 *
 * Copyright (C) 2011-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_THINGS_H
#define DATATOOLS_THINGS_H

// Standard Library:
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/serialization/access.hpp>

// This Project:
#include <datatools/i_serializable.h>
#include <datatools/serialization_macros.h>
#include <datatools/i_clear.h>
#include <datatools/i_tree_dump.h>
#include <datatools/bit_mask.h>

namespace datatools {

  // Forward declaration:
  class things;

  //! \brief An exception for invalid cast operation within the things class.
  class bad_things_cast
    : public std::exception
  {
  public:

    explicit bad_things_cast (const std::string & msg_);

    ~bad_things_cast() throw() override;

    const char* what() const throw() override;

  private:

    std::string _message_; ///< Message data

  };

  //! \brief A generic serializable and noncopyable container for arbitrary serializable objects.
  class things
    : public datatools::i_serializable
    , public datatools::i_tree_dumpable
    , public datatools::i_clear
    , private boost::noncopyable
  {
  public:

    //! \brief Internal entry for serializable object stored in the thing class.
    struct entry_type
      : public datatools::i_tree_dumpable
    {
      static const char MASK_CONST = bit_mask::bit00;

    public:

      entry_type();

      ~entry_type() override;

      bool is_not_const() const;

      bool is_const() const;

      void set_const(bool const_ = true);

      void set_description(const std::string &);

      const std::string & get_description() const;

      bool has_description() const;

      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

    public:

      std::string                 description;
      uint8_t                     flags;
      datatools::i_serializable * handle;

      BOOST_SERIALIZATION_BASIC_DECLARATION ()

    };

    // Begin the things declarations...
  public:

    static const bool constant;     // = true;
    static const bool non_constant; // = !constant;
    static const bool copyable;     // = true;
    static const bool noncopyable;  // = !copyable;

    /// Embedded dictionary of arbitrary serializable objects
    typedef std::map<std::string, entry_type> dict_type;

    /// Default constructor
    things();

    /// Constructor with name and description
    things(const std::string & name_, const std::string & description_ = "");

    // Destructor
    ~things() override;

    /// Return the name of the container
    const std::string & get_name() const;

    /// Set the name of the container
    things & set_name(const std::string &);

    /// Return the name of the container (low-level implementation)
    const std::string & get_name_impl() const;

    /// Set the name of the container (low-level implementation)
    void set_name_impl(const std::string &);

    /// Check if the container has a name
    bool has_name() const;

    /// Check if the container has a name (low-level implementation)
    bool has_name_impl() const;

    /// Return the description of the container
    const std::string & get_description() const;

    /// Set the description of the container
    things & set_description(const std::string &);

    /// Set the description of the container (low-level implementation)
    void set_description_impl(const std::string &);

    /// Return the description of the container (low-level implementation)
    const std::string & get_description_impl() const;

    /// Check if the container has a description (low-level implementation)
    bool has_description_impl() const;

    /// Reset the container
    void reset();

    /// Clear the container
    void clear() override;

    /// Return the number of objects stored in the container
    unsigned int size() const;

    /// Check if the container is empty
    bool empty() const;

    /// Check if the container stores an object with a given name
    bool has(const std::string & name_) const;

    /// Check if the container stores an object with a given name and a given serial tag
    bool has_serial_tag(const std::string & name_,
                        const std::string & serial_tag_) const;

    /// Check if the object stored is the container with a given name is constant
    bool is_constant(const std::string & name_) const;

    /// Check if the object stored is the container with a given name is mutable
    bool is_mutable(const std::string & name_) const;

    //! not implemented :
    void set_constant(const std::string & name_, bool const_ = true);

    /// Return the description of the object stored with a given name
    const std::string & get_description(const std::string & name_) const;

    /// Return the description of the object stored with a given name
    const std::string & get_entry_description(const std::string & name_) const;

    /// Set the description of the object stored with a given name
    void set_description(const std::string & name_, const std::string & desc_);

    /// Set the description of the object stored with a given name
    void set_entry_description(const std::string & name_, const std::string & desc_);

    /// Build the list of names of all objects stored in the container
    void get_names(std::vector<std::string> & names_) const;

    /// Return the name of the object stored at a given index
    const std::string & get_entry_name(int index_) const;

    /// Remove the object stored in the container with a given name
    void remove(const std::string & name_);

    /// Remove the object stored in the container with a given name
    void erase(const std::string & name_);

    /// Instantiate and add an object of given type and name
    template<class T>
    T& add(const std::string & name_, const std::string & desc_ = "",
           bool const_ = false);

    /// Extract the object of given type and name and remove the corresponding entry from the container
    ///
    template<class T>
    T* pop(const std::string & name_);

    /// Return a reference to a mutable object of given type and name
    template<class T>
    T& grab(const std::string & name_);

    /// Check if a stored object with given name is of given type
    template<class T>
    bool is_a(const std::string & name_) const;

    /// Return a reference to a non mutable object of given type and name
    template<class T>
    const T& get(const std::string & name_) const;

    /// Return a reference to a non mutable stored object of given name
    const datatools::i_serializable &
    get_entry(const std::string & name_) const;

    /// Return a reference to a mutable stored object of given name
    datatools::i_serializable &
    grab_entry(const std::string & name_);

    /// Instantiate and add an object of given type id with given name, description and constness flag
    datatools::i_serializable &
    add_entry(const std::string & name_,
              const std::string & serial_tag_,
              const std::string & description_ = "",
              bool const_ = false);

    /// Instantiate and add an object of given type id with given name, description and constness flag (implementation)
    datatools::i_serializable &
    add_entry_impl(const std::string & name_,
                   const std::string & serial_tag_,
                   const std::string & description_,
                   bool const_);

    /// Check if a stored object with given name has a type with given type id
    bool entry_is_a(const std::string & name_, const std::string &) const;

    /// Check if a stored object with given name is introspectable through CAMP interface
    bool entry_is_introspectable(const std::string & name_) const;

    /// Return the serial tag of a stored object with given name
    std::string get_entry_serial_tag(const std::string & name_) const;

    /// Return the introspection id of a stored object with given name
    std::string get_entry_introspection_id(const std::string & name_) const;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Basic print
    void dump(std::ostream & out_ = std::clog) const;

    /// Return a mutable reference to an object of given name and optional type id
    datatools::i_serializable &
    grab(const std::string & name_, const std::string & serial_tag_ = "");

    /// Return a non mutable reference to an object of given name and optional type id
    const datatools::i_serializable &
    get(const std::string & name_, const std::string & serial_tag_ = "") const;

  private:

    /// Implementation for adding a object in the container with a given name
    void add_impl(const std::string & name_,
                  datatools::i_serializable * obj_,
                  const std::string & desc_ = "",
                  bool const_ = false);

  private:

    // Attributes :
    std::string _name_;        //!< The name of the container
    std::string _description_; //!< The description of the container
    dict_type   _things_;      //!< The internal dictionary of objects

    //! Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(things)
    DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()

    //! Reflection interface
    DR_CLASS_RTTI()

  };

}  // end of namespace datatools

// Template methods
#include <datatools/things-inl.h>

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_DECLARATION(::datatools::things)
// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(::datatools::things)

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/serialization/export.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

BOOST_CLASS_EXPORT_KEY2(datatools::things,"datatools::things")

// Activate reflection layer for the 'datatools::things' class:
DR_CLASS_NONCOPYABLE_INIT(::datatools::things)

// Explicit class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::things, 1)

#endif // DATATOOLS_THINGS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
