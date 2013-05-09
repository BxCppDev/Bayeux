/* things.cc
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
 */
// Ourselves
#include <datatools/things.h>
#include <datatools/factory.h>
#include <datatools/factory_macros.h>

#if DATATOOLS_WITH_REFLECTION == 1
#include <camp/detail/typeid.hpp>
#endif // DATATOOLS_WITH_REFLECTION

//----------------------------------------------------------------------
// things class
//

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(::datatools::things,
                                                      "datatools::things");

// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(::datatools::things,
                                                               "datatools::utils::things");

namespace datatools {

DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(things,"datatools::things")

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
// things::entry_type inner class
//
things::entry_type::entry_type() {
  description = "";
  flags       = 0x0;
  handle      = 0;
}


bool things::entry_type::is_not_const() const {
  return (flags & MASK_CONST) == 0;
}


bool things::entry_type::is_const() const {
  return !this->is_not_const();
}


void things::entry_type::set_const(bool a_const) {
  if (a_const) {
    flags |= MASK_CONST;
  } else {
    flags &= ~MASK_CONST;
  }
}


void things::entry_type::set_description(const std::string& a_desc) {
  description = a_desc;
}


const std::string& things::entry_type::get_description() const {
  return description;
}


bool things::entry_type::has_description() const {
  return !description.empty();
}


void things::entry_type::tree_dump(std::ostream& a_out,
                                const std::string& a_title,
                                const std::string& a_indent,
                                bool a_inherit) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) a_out << indent << a_title << std::endl;

  if (!description.empty()) {
    a_out << indent << i_tree_dumpable::tag
          << "Description  : '" << this->get_description()
          << "'" << std::endl;
  }

  a_out << indent << i_tree_dumpable::tag
        << "Const  : " << this->is_const() << std::endl;

  a_out << indent << i_tree_dumpable::inherit_tag(a_inherit)
        << "Handle  : " << std::hex << handle << std::dec;
  if (handle != 0) {
    a_out << " (serial tag: '" << handle->get_serial_tag() << "')";
  }
  a_out << std::endl;
}

 const bool things::constant     = true;
 const bool things::non_constant = !constant;
 const bool things::copyable     = true;
 const bool things::noncopyable  = !copyable;

// ctor:
things::things() : datatools::i_serializable() {}


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


void things::set_name_impl(const std::string& a_name) {
  name_ = a_name;
}

const std::string & things::get_name_impl() const {
  return get_name();
}

bool things::has_name_impl() const {
  return !get_name_impl().empty ();
}

bool things::has_name() const {
  return has_name_impl();
}

things& things::set_name(const std::string& a_name) {
  name_ = a_name;
  return *this;
}

const std::string& things::get_description_impl() const {
  return description_;
}

void things::set_description_impl(const std::string& a_desc) {
  description_ = a_desc;
}

things& things::set_description(const std::string& a_desc) {
  set_description_impl (a_desc);
  return *this;
}

const std::string& things::get_description() const {
  return get_description_impl();
}

bool things::has_description_impl() const {
  return !get_description_impl().empty ();
}

void things::get_names(std::vector<std::string>& the_names) const {
  the_names.clear();
  if (things_.size() > the_names.capacity()) {
    the_names.reserve(things_.size());
  }
  for (dict_type::const_iterator i = things_.begin(); i != things_.end();
       ++i) {
    the_names.push_back(i->first);
  }
}


bool things::has(const std::string& a_name) const {
  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::things::has: Empty name !";
    throw std::logic_error(message.str());
  }
  dict_type::const_iterator found = things_.find(a_name);
  return (found != things_.end());
}


bool things::has_serial_tag(const std::string& a_name,
                            const std::string& a_serial_tag) const {
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::has_serial_tag: No object named '"
            << a_name << "' !";
    throw std::logic_error(message.str());
  }
  return found->second.handle->get_serial_tag() == a_serial_tag;
}


bool things::is_constant(const std::string& a_name) const {
  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::things::is_constant: Empty name !";
    throw std::logic_error(message.str());
  }
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::is_constant: No stored object has name '" << a_name << "' !";
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
    message << "datatools::things::set_constant: Not implemented !";
    throw std::logic_error(message.str());
  }

  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::things::set_constant: Empty name !";
    throw std::logic_error(message.str());
  }

  dict_type::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::set_constant: No stored object has name '" << a_name << "' !";
    throw std::logic_error(message.str());
  }
  found->second.set_const(a_const);
}

const std::string& things::get_entry_description(const std::string& a_name) const {
  return get_description(a_name);
}

const std::string& things::get_description(const std::string& a_name) const {
  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::things::get_description: Empty name !";
    throw std::logic_error(message.str());
  }

  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::get_description: No stored object has name '" << a_name << "' !";
    throw std::logic_error(message.str());
  }
  return found->second.description;
}

void things::set_entry_description(const std::string& a_name,
                                   const std::string& a_desc) {
  set_description(a_name,a_desc);
}

void things::set_description(const std::string& a_name,
                             const std::string& a_desc) {
  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::things::set_description: Empty name !";
    throw std::logic_error(message.str());
  }
  dict_type::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::set_description: No stored object has name '" << a_name << "' !";
    throw std::logic_error(message.str());
  }
  found->second.description = a_desc;
}


unsigned int things::size() const {
  return things_.size();
}


bool things::empty() const {
  return things_.size() == 0;
}


void things::add_impl(const std::string& a_name,
                      datatools::i_serializable* a_obj,
                      const std::string & a_desc,
                      bool a_const) {
  if (a_obj == 0) {
    std::ostringstream message;
    message << "datatools::things::add_impl: Cannot add a NULL pointed object !";
    throw std::logic_error(message.str());
  }

  if (a_name.empty()) {
    std::ostringstream message;
    message << "datatools::things::add_impl: Cannot add an object with an empty name !";
    if (a_obj != 0) {
      delete a_obj;
    }
    throw std::logic_error(message.str());
  }

  dict_type::const_iterator found = things_.find(a_name);
  if (found != things_.end()) {
    std::ostringstream message;
    message << "datatools::things::add_impl: An object with name '"
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
  dict_type::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::remove: No object with name '"
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
  this->tree_dump(a_out, "datatools::utils::things : ");
}


void things::dump_stdout() const {
  this->dump(std::cout);
}


void things::dump_stderr() const {
  this->dump(std::cerr);
}


void things::reset() {
  this->clear();
  description_.clear();
  name_.clear();
}

std::string
things::get_entry_introspection_id (const std::string& a_name) const
{
  std::string iid;
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::get_entry_introspection_id: No stored bank entry has name '"
            << a_name << "' !";
    throw std::domain_error(message.str());
  }
#if DATATOOLS_WITH_REFLECTION == 1
  const datatools::i_serializable & the_entry = *found->second.handle;
  try
    {
      iid = camp::detail::staticTypeId (the_entry);
    }
  catch (std::exception & x)
    {
      iid.clear ();
    }
#endif // DATATOOLS_WITH_REFLECTION == 1
  return iid;
}

std::string
things::get_entry_serial_tag (const std::string& a_name) const
{
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::get_entry_serial_tag: No stored object has name '"
        << a_name << "' !";
    throw std::domain_error(message.str());
  }
  return found->second.handle->get_serial_tag ();
}

bool things::entry_is_introspectable (const std::string& a_name) const
{
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::entry_is_introspectable: No stored object has name '"
            << a_name << "' !";
    throw std::domain_error(message.str());
  }
#if DATATOOLS_WITH_REFLECTION == 1
  const datatools::i_serializable & the_entry = *found->second.handle;
  try {
    camp::detail::staticTypeId (the_entry);
  }
  catch (std::exception & x)
    {
      return false;
    }
  return true;
#else
  return false;
#endif // DATATOOLS_WITH_REFLECTION == 1
}

bool things::entry_is_a (const std::string& a_name,
                         const std::string& a_serial_tag) const
{
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::entry_is_a: No stored object has name '"
        << a_name << "' !";
    throw std::domain_error(message.str());
  }
  return found->second.handle->get_serial_tag () == a_serial_tag;
}

const datatools::i_serializable &
things::get_entry (const std::string& a_name) const
{
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::get_entry: No stored object has name '"
        << a_name << "' !";
    throw std::domain_error(message.str());
  }
  return *found->second.handle;
}

datatools::i_serializable &
things::grab_entry (const std::string& a_name)
{
  dict_type::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::utils::things::grab_entry: No stored object has name '"
        << a_name << "' !";
    throw std::domain_error(message.str());
  }
  return *found->second.handle;
}

datatools::i_serializable &
things::add_entry (const std::string& a_name,
                   const std::string& a_serial_tag,
                   const std::string& a_description,
                   bool a_const)
{
  return add_entry_impl (a_name, a_serial_tag, a_description, a_const);
}

datatools::i_serializable &
things::add_entry_impl (const std::string& a_name,
                        const std::string& a_serial_tag,
                        const std::string& a_description,
                        bool a_const)
{
  datatools::i_serializable * new_serializable = 0;

  typedef datatools::factory_register< ::datatools::i_serializable> fr_type;
  const fr_type & FR = DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(::datatools::i_serializable);
  /*
  std::cerr << "DEVEL : a_name = " << a_name << std::endl;
  std::cerr << "DEVEL : a_serial_tag = " << a_serial_tag << std::endl;
  FR.print (std::cerr, "DEVEL : ");
  */
  if (! FR.has (a_serial_tag))
    {
      std::ostringstream message;
      message << "datatools::utils::things::add_entry_impl: "
              << "No class with serial tag '"
              << a_serial_tag << "' has been registered !";
      throw std::domain_error(message.str());
    }
  const fr_type::factory_type& F = FR.get(a_serial_tag);
  try {
    new_serializable = F();
  }
  catch(std::exception & x)
    {
      std::ostringstream message;
      message << "datatools::utils::things::add_entry_impl: Failed to create a serializable object with serial tag '"
              << a_serial_tag << "' ! Reason : " << x.what ();
      throw std::logic_error(message.str());
    }
  this->add_impl(a_name, new_serializable, a_description, a_const);
  return *new_serializable;
}


const std::string&
things::get_entry_name(int index_) const {
  if (index_ < 0 || index_ >= things_.size ())
    {
      std::ostringstream message;
      message << "datatools::utils::things::get_entry_name: Invalid index '" << index_ << "' !";
      throw std::range_error(message.str());
    }
  int count = 0;
  dict_type::const_iterator iter = things_.begin();
  for (; iter != things_.end(); ++iter, count++) {
    // datatools::i_serializable* s = i->second.handle;
    if (count == index_)
      {
        break;
      }
  }
  return iter->first;
}

void things::clear() {
  for (dict_type::iterator i = things_.begin(); i != things_.end(); ++i) {
    datatools::i_serializable* s = i->second.handle;
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
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!a_title.empty()) a_out << indent << a_title << std::endl;

  if (!name_.empty()) {
    a_out << indent << i_tree_dumpable::tag
          << "Name  : " << this->get_name() << std::endl;
  }

  if (!description_.empty()) {
    a_out << indent << i_tree_dumpable::tag
          << "Description  : " << this->get_description() << std::endl;
  }

  if (things_.size() == 0) {
    a_out << indent << i_tree_dumpable::inherit_tag(a_inherit)
          << "<no property>" << std::endl;
  } else {
    for (dict_type::const_iterator i = things_.begin();
         i != things_.end();
         ++i) {
      const std::string& key = i->first;
      const entry_type& a_entry = i->second;
      a_out << indent;
      std::ostringstream indent_oss;
      indent_oss << indent;
      dict_type::const_iterator j = i;
      j++;
      if (j == things_.end()) {
        a_out << i_tree_dumpable::inherit_tag(a_inherit);
        indent_oss << i_tree_dumpable::inherit_skip_tag(a_inherit);
      } else {
        a_out << i_tree_dumpable::tag;
        indent_oss << i_tree_dumpable::skip_tag;
      }
      a_out << "Name : " << '"' << key << '"' << std::endl;
      a_entry.tree_dump(a_out, "", indent_oss.str());
    }
  }
}


datatools::i_serializable &
things::grab(const std::string& a_name,
             const std::string& a_serial_tag) {
  dict_type::iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::grab: No stored object has name '"
        << a_name << "' !";
    throw std::logic_error(message.str());
  }
  if (found->second.is_const()) {
    std::ostringstream message;
    message << "datatools::things::grab: Object named '"
            << a_name << "' is constant !";
    throw std::logic_error(message.str());
  }
  if (! a_serial_tag.empty  ())
    {
      if (a_serial_tag != found->second.handle->get_serial_tag())
        {
          std::ostringstream message;
          message << "datatools::things::grab: Object named '"
                  << a_name << "' is not a '" << a_serial_tag << "' !";
        }
    }
  return *(found->second.handle);
}

const datatools::i_serializable &
things::get(const std::string& a_name,
            const std::string& a_serial_tag) const {
  dict_type::const_iterator found = things_.find(a_name);
  if (found == things_.end()) {
    std::ostringstream message;
    message << "datatools::things::get: No stored object has name '"
            << a_name << "' !";
    throw std::logic_error(message.str());
  }
  if (! a_serial_tag.empty  ())
    {
      if (a_serial_tag != found->second.handle->get_serial_tag())
        {
          std::ostringstream message;
          message << "datatools::things::get: Object named '"
                  << a_name << "' is not a '" << a_serial_tag << "' !";
        }
    }
  return *(found->second.handle);
}

}  // end of namespace datatools

