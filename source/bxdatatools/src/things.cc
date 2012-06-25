/* things.cc
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
 */
// Ourselves
#include <datatools/utils/things.h>

namespace datatools {
namespace utils {

//----------------------------------------------------------------------
// bad_things_cast class
//
bad_things_cast::bad_things_cast(const std::string& a_msg)
    : std::exception(), message_(a_msg) {}


bad_things_cast::~bad_things_cast() throw() {}


const char* bad_things_cast::what() const throw() {
  return message_.c_str();
}

//----------------------------------------------------------------------
// things::entry_t inner class
//
things::entry_t::entry_t() {
  description = "";
  flags       = 0x0;
  handle      = 0;
}


bool things::entry_t::is_not_const() const {
  return (flags & MASK_CONST) == 0;
}


bool things::entry_t::is_const() const {
  return !this->is_not_const();
}


void things::entry_t::set_const(bool a_const) {
  if (a_const) {
    flags |= MASK_CONST;
  } else {
    flags &= ~MASK_CONST;
  }
}


void things::entry_t::set_description(const std::string& a_desc) {
  description = a_desc;
}


const std::string& things::entry_t::get_description() const {
  return description;
}


bool things::entry_t::has_description() const {
  return !description.empty();
}


void things::entry_t::tree_dump(std::ostream& a_out,
                                const std::string& a_title,
                                const std::string& a_indent,
                                bool a_inherit) const {
  namespace du = datatools::utils;
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) a_out << indent << a_title << std::endl;

  if (!description.empty()) {
    a_out << indent << du::i_tree_dumpable::tag
          << "Description  : '" << this->get_description()
          << "'" << std::endl;
  }

  a_out << indent << du::i_tree_dumpable::tag
        << "Const  : " << this->is_const() << std::endl;

  a_out << indent << du::i_tree_dumpable::inherit_tag(a_inherit)
        << "Handle  : " << std::hex << handle << std::dec;
  if (handle != 0) {
    a_out << " (serial tag: '" << handle->get_serial_tag() << "')";
  }
  a_out << std::endl;
}


//----------------------------------------------------------------------
// things class
//
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(things,"datatools::utils::things")

// ctor:
things::things() : datatools::serialization::i_serializable() {}


// ctor:
things::things(const std::string& a_name, const std::string& a_description) {
  this->set_name(a_name);
  this->set_description(a_description);
}


// dtor:
things::~things() {
  this->reset();
}


const std::string & things::get_name() const {
  return name_;
}


things& things::set_name(const std::string& a_name) {
  name_ = a_name;
  return *this;
}


const std::string& things::get_description() const {
  return description_;
}


things& things::set_description(const std::string& a_desc) {
  description_ = a_desc;
  return *this;
}


void things::get_names(std::vector<std::string>& the_names) const {
  the_names.clear();
  if (things_.size() > the_names.capacity()) {
    the_names.reserve(things_.size());
  }
  for (dict_t::const_iterator i = things_.begin(); i != things_.end(); 
       ++i) {
    the_names.push_back(i->first);
  }
}


bool things::has(const std::string& a_name) const {
  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::utils::things::has: Empty name !";
    throw std::logic_error(message.str());
  }
  dict_t::const_iterator found = things_.find(a_name);
  return (found != things_.end());
}


bool things::has_serial_tag(const std::string& a_name, 
                            const std::string& a_serial_tag) const {
  dict_t::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::has_serial_tag: No object named '"
            << a_name << "' !";
    throw std::logic_error(message.str());
  }
  return found->second.handle->get_serial_tag() == a_serial_tag;
}


bool things::is_constant(const std::string& a_name) const {
  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::utils::things::is_constant: Empty name !";
    throw std::logic_error(message.str());
  }
  dict_t::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::is_constant: No stored object has name '" << a_name << "' !";
    throw std::logic_error(message.str());
  }
  return found->second.is_const();
}


bool things::is_mutable(const std::string& a_name) const {
  return !this->is_constant(a_name);
}


void things::set_constant(const std::string& a_name, bool a_const) {
  bool not_implemented_yet = true;
  if (not_implemented_yet) {
    std::ostringstream message;
    message << "datatools::utils::things::set_constant: Not implemented !";
    throw std::logic_error(message.str());
  }

  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::utils::things::set_constant: Empty name !";
    throw std::logic_error(message.str());
  }

  dict_t::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::set_constant: No stored object has name '" << a_name << "' !";
    throw std::logic_error(message.str());
  }
  found->second.set_const(a_const);
}


const std::string& things::get_description(const std::string& a_name) const {
  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::utils::things::get_description: Empty name !";
    throw std::logic_error(message.str());
  }

  dict_t::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::get_description: No stored object has name '" << a_name << "' !";
    throw std::logic_error(message.str());
  }
  return found->second.description;
}


void things::set_description(const std::string& a_name, 
                             const std::string& a_desc) {
  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::utils::things::set_description: Empty name !";
    throw std::logic_error(message.str());
  }
  dict_t::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::set_description: No stored object has name '" << a_name << "' !";
    throw std::logic_error(message.str());
  }
  found->second.description = a_desc;
}


size_t things::size() const {
  return things_.size();
}


bool things::empty() const {
  return things_.size() == 0;
}


void things::add_impl(const std::string& a_name,
                      datatools::serialization::i_serializable* a_obj,
                      const std::string & a_desc,
                      bool a_const) {
  if (a_obj == 0) {
    std::ostringstream message;
    message << "datatools::utils::things::add_impl: Cannot add a NULL pointed object !";
    throw std::logic_error(message.str());
  }

  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::utils::things::add_impl: Cannot add an object with an empty name !";
    if (a_obj != 0) {
      delete a_obj;
    }
    throw std::logic_error(message.str());
  }

  dict_t::const_iterator found = things_.find(a_name);
  if (found != things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::add_impl: An object with name '" 
            << a_name
            << "' is already stored in the dictionnary !";
    throw std::logic_error(message.str());
  }
  things_[a_name].set_description(a_desc);
  things_[a_name].set_const(a_const);
  things_[a_name].handle = a_obj;
}


void things::remove(const std::string& a_name) {
  if (a_name.empty()) {
    return;
  }
  dict_t::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::remove: No object with name '" 
            << a_name
            << "' is stored in the dictionnary !";
    throw std::logic_error(message.str());
  }

  if (found->second.handle != 0) {
    delete found->second.handle;
    found->second.handle = 0;
  }
  things_.erase(found);
}


void things::erase(const std::string& a_name) {
  this->remove(a_name);
}


void things::dump(std::ostream& a_out) const {
  this->tree_dump(a_out, "datatools::utils::things:");
}


void things::reset() {
  this->clear();
  description_.clear();
  name_.clear();
}


void things::clear() {
  for (dict_t::iterator i = things_.begin(); i != things_.end(); ++i) {
    datatools::serialization::i_serializable* s = i->second.handle;
    if (s != 0) {
      delete s;
      i->second.handle = 0;
    }
  }
  things_.clear();
}


void things::tree_dump(std::ostream& a_out, const std::string& a_title,
                        const std::string& a_indent,
                        bool a_inherit) const {
  namespace du = datatools::utils;
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) a_out << indent << a_title << std::endl;

  if (!name_.empty()) {
    a_out << indent << du::i_tree_dumpable::tag
          << "Name  : " << this->get_name() << std::endl;
  }

  if (!description_.empty()) {
    a_out << indent << du::i_tree_dumpable::tag
          << "Description  : " << this->get_description() << std::endl;
  }

  if (things_.size() == 0) {
    a_out << indent << du::i_tree_dumpable::inherit_tag(a_inherit)
          << "<no property>" << std::endl;
  } else {
    for (dict_t::const_iterator i = things_.begin(); 
         i != things_.end();
         ++i) {
      const std::string& key = i->first;
      const entry_t& a_entry = i->second;
      a_out << indent;
      std::ostringstream indent_oss;
      indent_oss << indent;
      dict_t::const_iterator j = i;
      j++;
      if (j == things_.end()) {
        a_out << du::i_tree_dumpable::inherit_tag(a_inherit);
        indent_oss << du::i_tree_dumpable::inherit_skip_tag(a_inherit);
      } else {
        a_out << du::i_tree_dumpable::tag;
        indent_oss << du::i_tree_dumpable::skip_tag;
      }
      a_out << "Name : " << '"' << key << '"' << std::endl;
      a_entry.tree_dump(a_out, "", indent_oss.str());
    }
  }
}

}  // end of namespace utils
}  // end of namespace datatools

