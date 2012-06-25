/* things.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-03-08
 * Last modified : 2011-06-29
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
 *   A serializable container for arbitrary serializable objects
 * 
 *   Storable objects must :
 *   - have a default constructor
 *   - inherit from the datatools::serialization::i_serializable interface
 *   - have a 'serialize' template method (ala Boost)
 *  
 * History: 
 * 
 */
#ifndef DATATOOLS_UTILS_THINGS_H_
#define DATATOOLS_UTILS_THINGS_H_
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
#include <datatools/serialization/i_serializable.h>
#include <datatools/serialization/utils.h>

#include <datatools/utils/i_clear.h>
#include <datatools/utils/i_tree_dump.h>

//! \file datatools/utils/things.h
namespace datatools {
namespace utils {

/* class bad_things_cast */
//! \brief An exception for invalid cast operation within the \class things class.
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
class things : public datatools::serialization::i_serializable,
    public datatools::utils::i_tree_dumpable,
    public datatools::utils::i_clear,
    private boost::noncopyable {
 public:
  //! \brief Internal entry for serializable object storing in the \class thing class.
  struct entry_t : public datatools::utils::i_tree_dumpable {
    static const char MASK_CONST = 0x1;
   public:
    entry_t();

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
    std::string                              description;
    uint8_t                                  flags;
    datatools::serialization::i_serializable *handle;
 
   private:
    friend class boost::serialization::access; 
    BOOST_SERIALIZATION_SERIALIZE_DECLARATION()
 };

  // Begin the things declarations...
 public:
  static const bool constant     = true;
  static const bool non_constant = !constant;
  static const bool copyable     = true;
  static const bool noncopyable  = !copyable;

  typedef std::map<std::string, entry_t> dict_t;
  DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()

 public: 
  // ctor:
  things();

  // ctor:
  things(const std::string& a_name, const std::string& a_description = "");

  // dtor:
  virtual ~things();

  const std::string& get_name() const;

  things& set_name(const std::string&);

  const std::string& get_description() const;

  things& set_description(const std::string&);

  void reset();

  virtual void clear();

  size_t size() const;

  bool empty() const;

  bool has(const std::string& a_name) const;

  bool has_serial_tag(const std::string& a_name, 
                      const std::string& a_serial_tag) const;

  bool is_constant(const std::string& a_name) const;

  bool is_mutable(const std::string& a_name) const;

  //! not implemented :
  void set_constant(const std::string& a_name, bool a_const = true);

  const std::string& get_description(const std::string& a_name) const;

  void set_description(const std::string& name, const std::string& desc);

  void get_names(std::vector<std::string>& names) const;

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

  virtual void tree_dump(std::ostream& a_out = std::clog, 
                         const std::string& a_title  = "",
                         const std::string& a_indent = "",
                         bool a_inherit = false) const;

  void dump(std::ostream& a_out = std::clog) const;


 private:
  // non-copyable
  things(const things&);
  things& operator=(const things&);
  
  void add_impl(const std::string& a_name, 
             datatools::serialization::i_serializable* a_obj,
             const std::string& a_desc = "",
             bool a_const = false);

  // serialization :
  friend class boost::serialization::access;
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

 private:
  // Attributes : 
  std::string name_;        //!< The name of the container
  std::string description_; //!< The description of the container
  dict_t      things_;      //!< The internal dictionary of objects
};


//----------------------------------------------------------------------
// things template methods
//
template<class T>
T& things::add(const std::string& a_name, const std::string& a_desc, 
               bool a_const) {
  T* new_obj = 0;
  new_obj = new T;

  if (new_obj == 0) {
    std::ostringstream message;
    message << "datatools::utils::things::add<T>: " 
        << "Cannot allocate object '" << a_name << "' !";
    throw std::logic_error(message.str());
  }

  datatools::serialization::i_serializable* new_cast 
      = dynamic_cast<datatools::serialization::i_serializable*>(new_obj); 
  if (new_cast == 0) {
    if (new_obj != 0) {
      delete new_obj;
    }
    const std::type_info& ti = typeid(T);
    std::ostringstream message;
    message << "datatools::utils::things::add<T>: Request type '" 
        << ti.name() 
        << "' does not inherit from the '" 
        << "datatools::serialization::i_serializable"  
        << "' base class !";
    throw bad_things_cast(message.str());
  }
  this->add_impl(a_name, new_cast, a_desc, a_const);
  return *new_obj;
}


template<class T>
T* things::pop(const std::string& a_name) {
  dict_t::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::pop: No stored object has name '" 
        << a_name << "' !";
    throw std::logic_error(message.str());
    return 0;
  }
  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(*(found->second.handle));
  if (ti != tf) {
    T tmp;
    std::ostringstream message;
    message << "datatools::utils::things::pop: Request type '" 
        << ti.name() 
        << "' ('" << tmp.get_serial_tag() 
        << "') does not match the type '" 
        << tf.name() 
        << "' of the stored object named '" 
        << a_name 
        << "' ('" 
        << found->second.handle->get_serial_tag() 
        << "') !";
    throw bad_things_cast(message.str());
  }
  T* ptr = dynamic_cast<T*>(found->second.handle);
  found->second.handle = 0;
  things_.erase(found);
  return ptr;
}


template<class T>
T& things::grab(const std::string& a_name) {
  dict_t::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::grab: No stored object has name '" 
        << a_name << "' !";
    throw std::logic_error(message.str());
  }
  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(*(found->second.handle));
  if (ti != tf) {
    T tmp;
    std::ostringstream message;
    message << "datatools::utils::things::grab: Request type '" 
        << ti.name() 
        << "' ('" 
        << tmp.get_serial_tag() 
        << "') does not match the type '" 
        << tf.name() 
        << "' of the stored object named '" 
        << a_name 
        << "' ('" 
        << found->second.handle->get_serial_tag() 
        << "') !";
    throw bad_things_cast(message.str());
  }
  if (found->second.is_const()) {
    std::ostringstream message;
    message << "datatools::utils::things::grab: Object named '" 
        << a_name << "' is constant !";
    throw std::logic_error(message.str());
  }
  return *(dynamic_cast<T*>(found->second.handle));
}


template<class T>
bool things::is_a(const std::string& a_name) const {
  dict_t::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::is_a: No object named '" 
        << a_name << "' !";
    throw std::logic_error(message.str());
  }
  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(*found->second.handle);
  return (ti == tf); 
}


template<class T>
const T& things::get(const std::string& a_name) const {
  dict_t::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::get: No object named '" 
        << a_name << "' !";
    throw std::logic_error(message.str());
  }
  const std::type_info& ti = typeid(T);
  const std::type_info& tf = typeid(*found->second.handle);
  if (ti != tf) {
    T tmp;
    std::ostringstream message;
    message << "datatools::utils::things::get: Request type '" 
        << ti.name() 
        << "' ('" 
        << tmp.get_serial_tag() 
        << "') does not match the type '" 
        << tf.name() 
        << "' of the stored object named '" << a_name 
        << "' ('" 
        << found->second.handle->get_serial_tag() 
        << "') !";
    throw bad_things_cast(message.str());
  }
  return *(dynamic_cast<const T*>(found->second.handle));
}


}  // end of namespace utils
}  // end of namespace datatools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::things,"datatools::utils::things")

#endif // DATATOOLS_UTILS_THINGS_H_

