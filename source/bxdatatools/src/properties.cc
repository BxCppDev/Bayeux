// -*- mode: c++; -*-
/* properties.cc
 */
// Ourselves
#include <datatools/properties.h>

// Standard Library
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>

// Third Party
// - A
#include <boost/algorithm/string/replace.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>
#include <boost/archive/polymorphic_iarchive.hpp>

// This Project
#include <datatools/utils.h>
#include <datatools/units.h>

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(::datatools::properties,
                                                      "datatools::properties");

// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(::datatools::properties, 
                                                               "datatools::utils::properties");

namespace datatools {

DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(properties,"datatools::properties")

bool properties::g_debug = false;

properties::default_key_validator properties::g_default_key_validator;

//----------------------------------------------------------------------
// properties::data class implementation
//
const bool        properties::data::DEFAULT_VALUE_BOOLEAN = false;
const int         properties::data::DEFAULT_VALUE_INTEGER = 0;
const double      properties::data::DEFAULT_VALUE_REAL    = 0.0;
const std::string properties::data::DEFAULT_VALUE_STRING  = "";

bool properties::data::has_forbidden_char(const std::string& a_str) {
  return a_str.find_first_of(STRING_FORBIDDEN_CHAR) != a_str.npos;
}


std::string properties::data::get_error_message(int a_error_code) {
  if (a_error_code == ERROR_SUCCESS) return "success";
  if (a_error_code == ERROR_FAILURE) return "generic error";
  if (a_error_code == ERROR_BADTYPE) return "invalid type";
  if (a_error_code == ERROR_RANGE ) return "invalid index";
  if (a_error_code == ERROR_LOCK  ) return "lock error";
  return "undocumented error";
}


void properties::data::clear_values_() {
  _boolean_values_.clear();
  _integer_values_.clear();
  _real_values_.clear();
  _string_values_.clear();
}


int properties::data::init_values_(char a_type, int a_size) {
  int memsize = a_size;
  if (a_size < 0) {
    memsize = 1;
    _flags_ &= ~MASK_VECTOR; // force scalar
    // if (properties::g_debug) {
    //   std::cerr << "DEBUG: properties::data::init_values_: SCALAR... size="
    //             << memsize << std::endl;
    // }
  } else {
    _flags_ |= MASK_VECTOR; // force vector
    // if (properties::g_debug) {
    //   std::cerr << "DEBUG: properties::data::init_values_: VECTOR... size="
    //             << memsize << std::endl;
    // }
  }

  _flags_ &= ~MASK_TYPE;
  if (properties::g_debug) {
    std::cerr << "DEBUG: properties::data::init_values_: desc="
              << std::hex << (int) _flags_
              << std::dec << std::endl;
  }

  if (a_type == TYPE_BOOLEAN_SYMBOL) {
    // if (properties::g_debug) {
    //   std::cerr << "DEBUG: properties::data::init_values_: BOOLEAN..."
    //             << std::endl;
    // }
    _flags_ |= TYPE_BOOLEAN;
    if (memsize > 0) _boolean_values_.assign(memsize, DEFAULT_VALUE_BOOLEAN);
  }

  if (a_type == TYPE_INTEGER_SYMBOL) {
    // if (properties::g_debug) {
    //   std::cerr << "DEBUG: properties::data::init_values_: INTEGER..."
    //             << " TYPE_INTEGER=" << (int) TYPE_INTEGER << std::endl;
    // }
    _flags_ |= TYPE_INTEGER;
    if (memsize > 0) _integer_values_.assign(memsize, DEFAULT_VALUE_INTEGER);
  }

  if (a_type == TYPE_REAL_SYMBOL) {
    // if (properties::g_debug) {
    //   std::cerr << "DEBUG: properties::data::init_values_: REAL..."
    //             << std::endl;
    // }
    _flags_ |= TYPE_REAL;
    if (memsize > 0) _real_values_.assign(memsize, DEFAULT_VALUE_REAL);
  }

  if (a_type == TYPE_STRING_SYMBOL) {
    // if (properties::g_debug) {
    //   std::cerr << "DEBUG: properties::data::init_values_: STRING..."
    //             << std::endl;
    // }
    _flags_ |= TYPE_STRING;
    if (memsize > 0) _string_values_.assign(memsize, DEFAULT_VALUE_STRING);
  }

  // if (properties::g_debug) {
  //   std::cerr << "DEBUG: properties::data::init_values_: exiting."
  //             << std::endl;
  // }
  return ERROR_SUCCESS;
}


int properties::data::boolean(int a_size) {
  this->clear_values_();
  return this->init_values_(TYPE_BOOLEAN_SYMBOL, a_size);
}


void properties::data::set_description(const std::string& a_description) {
  _description_ = a_description;
}


const std::string& properties::data::get_description() const {
  return _description_;
}


int properties::data::integer(int a_size) {
  this->clear_values_();
  return this->init_values_(TYPE_INTEGER_SYMBOL, a_size);
}


int properties::data::real(int a_size) {
  this->clear_values_();
  return this->init_values_(TYPE_REAL_SYMBOL, a_size);
}


int properties::data::string(int a_size) {
  this->clear_values_();
  return this->init_values_(TYPE_STRING_SYMBOL, a_size);
}


bool properties::data::has_type() const {
  return (_flags_ & MASK_TYPE) != TYPE_NONE;
}


bool properties::data::is_boolean() const {
  return (_flags_ & MASK_TYPE) == TYPE_BOOLEAN;
}


bool properties::data::is_integer() const {
  return (_flags_ & MASK_TYPE) == TYPE_INTEGER;
}


bool properties::data::is_real() const {
  return (_flags_ & MASK_TYPE) == TYPE_REAL;
}


bool properties::data::is_string() const {
  return (_flags_ & MASK_TYPE) == TYPE_STRING;
}


bool properties::data::is_scalar() const {
  return (_flags_ & MASK_VECTOR) == 0;
}

bool properties::data::is_vector() const {
  return !this->is_scalar();
}


bool properties::data::has_explicit_unit() const {
  return (_flags_ & MASK_EXPLICIT_UNIT) != 0;
}


bool properties::data::is_unlocked() const {
  return (_flags_ & MASK_LOCK) == 0;
}


bool properties::data::is_locked() const {
  return !this->is_unlocked();
}


int properties::data::lock() {
  _flags_ |= MASK_LOCK;
  return ERROR_SUCCESS;
}


int properties::data::unlock() {
  _flags_ &= ~MASK_LOCK;
  return ERROR_SUCCESS;
}


int32_t properties::data::get_size() const {
  if (!this->has_type()) {
    throw std::logic_error("datatools::properties::data::get_size: No type!");
  }

  if (this->is_vector()) {
    if (this->is_boolean()) return _boolean_values_.size();
    if (this->is_integer()) return _integer_values_.size();
    if (this->is_real())    return _real_values_.size();
    if (this->is_string())  return _string_values_.size();
  } else {
    return SCALAR_SIZE;
  }
  return 0;
}


int32_t properties::data::size() const {
  return this->get_size();
}


bool properties::data::empty() const {
  return (this->size() == 0);
}


properties::data::data(char a_type, int a_size) {
  _flags_  = 0;
  this->init_values_(a_type, a_size);
}


properties::data::data(bool a_value, int a_size) {
  if (properties::g_debug) {
    std::cerr << "DEBUG: properties::data::ctor (...bool...): entering..."
              << std::endl;
  }
  _flags_  = 0;
  this->init_values_(TYPE_BOOLEAN_SYMBOL, a_size);
  for (int i = 0; i < (int)this->size(); ++i) {
    this->set_value(a_value, i);
  }
}


properties::data::data(int a_value, int a_size) {
  // if (properties::g_debug) {
  //   std::cerr << "DEBUG: properties::data::ctor (...int...): entering..."
  //             << std::endl;
  // }
  _flags_  = 0;
  this->init_values_(TYPE_INTEGER_SYMBOL, a_size);
  for (int i = 0; i < (int)this->size(); ++i) {
    this->set_value(a_value, i);
  }
}


properties::data::data(double a_value, int a_size) {
  // if (properties::g_debug) {
  //   std::cerr << "DEBUG: properties::data::ctor (...double...): entering..."
  //             << std::endl;
  // }
  _flags_  = 0;
  this->init_values_(TYPE_REAL_SYMBOL, a_size);
  for (int i = 0; i < (int)this->size(); ++i) {
    this->set_value(a_value, i);
  }
}


properties::data::data(const std::string& a_value, int a_size) {
  // if (properties::g_debug) {
  //   std::cerr << "DEBUG: properties::data::ctor (...std::string...): entering... "
  //             << a_value  << std::endl;
  // }
  _flags_  = 0;
  int code=0;
  this->init_values_(TYPE_STRING_SYMBOL, a_size);
  if (has_forbidden_char (a_value)) {
    std::ostringstream message;
    message << "datatools::properties::data::data (std::string): Forbidden char in string '" << a_value << "'!";
    throw std::logic_error(message.str());
  }
  for (int i = 0; i < (int)this->size(); ++i) {
    code = this->set_value(a_value, i);
    if (code != ERROR_SUCCESS) {
      throw std::logic_error("datatools::properties::data::data (std::string): failure!");
    }
  }
  // if (properties::g_debug) {
  //   this->tree_dump(std::cerr, "datatools::properties::data::data", "DEBUG: ");
  //   std::cerr << "DEBUG: properties::data::ctor (...std::string...): entering... "
  //             << "code=" << code  << std::endl;

  // }
}


properties::data::data (const char* a_value, int a_size) {
  // if (properties::g_debug) {
  //   std::cerr << "DEBUG: properties::data::ctor (...char *...): entering..." << std::endl;
  // }
  _flags_ = 0;
  this->init_values_(TYPE_STRING_SYMBOL, a_size);
  std::string tmp;
  if (a_value != 0) {
    tmp = a_value;
    if (has_forbidden_char(tmp)) {
      std::ostringstream message;
      message << "datatools::properties::data::data (std::string): Forbidden char in string '" << tmp << "'!";
      throw std::logic_error(message.str());
    }
  }
  for (int i = 0; i < (int)this->size(); ++i) {
    this->set_value(tmp, i);
  }
}


properties::data::~data() {}


bool properties::data::index_is_valid(int a_index) const {
  return (a_index >= 0) && (a_index < (int)this->get_size());
}


int properties::data::set_value(bool a_value, int a_index) {
  if (!this->is_boolean()) return ERROR_BADTYPE;
  
  if (this->is_locked()) return ERROR_LOCK;

  if (!this->index_is_valid(a_index)) return ERROR_RANGE;
  
  _boolean_values_[a_index] = a_value;
  return ERROR_SUCCESS;
}


int properties::data::set_value(int a_value, int a_index) {
  if (!this->is_integer()) return ERROR_BADTYPE;
  
  if (this->is_locked()) return ERROR_LOCK;
  
  if (!this->index_is_valid(a_index)) return ERROR_RANGE;
  
  _integer_values_[a_index] = a_value;
  return ERROR_SUCCESS;
}


int properties::data::set_value(double a_value, int a_index) {
  if (!this->is_real()) return ERROR_BADTYPE;
  
  if (this->is_locked()) return ERROR_LOCK;
  
  if (!this->index_is_valid(a_index)) return ERROR_RANGE;
  set_explicit_unit(false);  
  _real_values_[a_index] = a_value;
  return ERROR_SUCCESS;
}


int properties::data::set_explicit_unit(bool xu_)
{
  if (! xu_) {
    _flags_ &= ~MASK_EXPLICIT_UNIT; // force scalar
  } else {
    _flags_ |= MASK_EXPLICIT_UNIT; // force vector
  }
  return ERROR_SUCCESS;
}

int properties::data::set_value(const std::string & a_value, int a_index) {
  if (!this->is_string()) return ERROR_BADTYPE;
  //std::cerr << "DEVEL: test1 " << '\n';
  if (this->is_locked()) return ERROR_LOCK;
  //std::cerr << "DEVEL: test2 " << '\n';
  
  if (!this->index_is_valid(a_index)) return ERROR_RANGE;
  //std::cerr << "DEVEL: test3 " << '\n';
  
  /* special trick to forbid character '\"' in string as
   * it is used as separator for parsing:
   */
  if (this->has_forbidden_char(a_value)) return ERROR_FAILURE;
  //std::cerr << "DEVEL: test4 " << '\n';
  
  _string_values_[a_index] = a_value;
  //std::cerr << "DEVEL: test5 " << '\n';
  return ERROR_SUCCESS;
}


int properties::data::set_value(const char* a_value, int a_index) {
  return this->set_value(std::string(a_value), a_index);
}


int properties::data::get_value(bool& a_value, int a_index) const {
  if (!this->is_boolean()) return ERROR_BADTYPE;
  
  if (!this->index_is_valid(a_index)) return ERROR_RANGE;
  
  a_value = _boolean_values_[a_index];
  return ERROR_SUCCESS;
}


int properties::data::get_value(int& a_value, int a_index) const {
  if (!this->is_integer()) return ERROR_BADTYPE;
  
  if (!this->index_is_valid(a_index)) return ERROR_RANGE;
  
  a_value = _integer_values_[a_index];
  return ERROR_SUCCESS;
}


int properties::data::get_value(double& a_value, int a_index) const {
  if (!this->is_real()) return ERROR_BADTYPE;
  
  if (!this->index_is_valid(a_index)) return ERROR_RANGE;
  
  a_value = _real_values_[a_index];
  return ERROR_SUCCESS;
}


int properties::data::get_value(std::string& a_value, int a_index) const {
  if (!this->is_string()) return ERROR_BADTYPE;
  
  if (!this->index_is_valid(a_index)) return ERROR_RANGE;
  
  a_value = _string_values_[a_index];
  return ERROR_SUCCESS;
}


bool properties::data::get_boolean_value(int a_index) const {
  bool value;
  if (this->get_value(value, a_index) != ERROR_SUCCESS) {
    throw std::logic_error("datatools::properties::data::get_boolean_value: type is not boolean!");
  }
  return value;
}


int properties::data::get_integer_value(int a_index) const {
  int value;
  if (this->get_value(value, a_index) != ERROR_SUCCESS) {
    throw std::logic_error("datatools::properties::data::get_integer_value: type is not integer!");
  }
  return value;
}


double properties::data::get_real_value(int a_index) const {
  double value;
  if (this->get_value(value, a_index) != ERROR_SUCCESS) {
    throw std::logic_error("datatools::properties::data::get_real_value: type is not double!");
  }
  return value;
}


std::string properties::data::get_string_value(int a_index) const {
  std::string value;
  if (this->get_value(value, a_index) != ERROR_SUCCESS) {
    throw std::logic_error("datatools::properties::data::get_string_value: type is not string!");
  }
  return value;
}


std::string properties::data::get_type_label() const {
  if (this->is_boolean()) return "boolean";
  if (this->is_integer()) return "integer";
  if (this->is_real()) return "real";
  if (this->is_string()) return "string";
  return "none";
}


std::string properties::data::get_vector_label() const {
  if (this->is_scalar()) return "scalar";
  return "vector";
}


void properties::data::dump(std::ostream& a_out) const {
  this->tree_dump(a_out, "datatools::properties::data");
}


void properties::data::to_string(std::ostream& a_out) const {
  a_out << '[';
  a_out << this->get_type_label() << ':'
        << this->get_vector_label() << ":"
        << this->get_size() << ':';
  for (int i = 0; i < (int)this->get_size(); ++i) {
    if (i != 0) a_out << ' ';
    if (this->is_boolean()) a_out << this->get_boolean_value(i);
    if (this->is_integer()) a_out << this->get_integer_value(i);
    if (this->is_real())    a_out << this->get_real_value(i);
    if (this->is_string())  a_out << this->get_string_value(i);
  }
  a_out << ':' << this->get_description();
  a_out << ']';
}


void properties::data::tree_dump(std::ostream& a_out,
                                 const std::string& a_title,
                                 const std::string& a_indent,
                                 bool a_inherit) const {
  std::string indent;
  if (! a_indent.empty()) indent = a_indent;
  if (! a_title.empty()) a_out << indent << a_title << std::endl;
  
  if (!_description_.empty()) {
    a_out << indent << i_tree_dumpable::tag
          << "Description  : " << this->get_description() << std::endl;
  }

  a_out << indent << i_tree_dumpable::tag
        << "Type  : " ;
  if (this->is_locked()) a_out << "const ";
  a_out << this->get_type_label();
  if (this->is_vector()) {
    a_out << '[' << this->size() << ']' << " (vector)";
  } else {
    a_out << " (scalar)";
  }
  if (this->is_real() && has_explicit_unit())
    {
      a_out << " [explicit unit]";
    }
  //a_out << ' ' << std::hex << (int) _flags_ << std::dec;
  a_out << std::endl;

  if (this->get_size() > 0) {
    for (int i = 0; i < (int)this->get_size(); ++i) {
      a_out << indent;
      if (i == (int)(this->get_size() - 1)) {
        a_out << i_tree_dumpable::inherit_tag (a_inherit);
      } else {
        a_out << i_tree_dumpable::tag;
      }
      a_out << "Value";
      if (this->is_vector()) a_out << "[" << i << "]";
      
      a_out << " : ";
      if (this->is_boolean()) {
        a_out << std::dec << this->get_boolean_value(i) << std::endl;
      }
      if (this->is_integer()) {
        a_out << std::dec << this->get_integer_value(i) << std::endl;
      }
      if (this->is_real()) 
        {
          a_out.precision(16);
          a_out << this->get_real_value(i);
          a_out << std::endl;
        }
      
      if (this->is_string()) {
        a_out << '"' << this->get_string_value(i) << '"' << std::endl;
      }
    }
  } else {
    a_out << indent;
    a_out << i_tree_dumpable::inherit_tag(a_inherit);
    a_out << "<no value>" << std::endl;
  }
}


//----------------------------------------------------------------------
// properties::key_validator implementation
//
const std::string properties::default_key_validator::ALLOWED_CHARS =
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";

properties::default_key_validator::default_key_validator() {}

properties::default_key_validator::~default_key_validator() {}

bool properties::default_key_validator::operator()(
    const std::string& prop_key) const {
  if (prop_key.empty()) return false;
  if (prop_key.find_first_not_of(ALLOWED_CHARS) != prop_key.npos) return false;
  if (prop_key.find_first_of("0123456789.") == 0) return false;
  if (prop_key[prop_key.size()-1] == '.') return false;
  return true;
}


//----------------------------------------------------------------------
// properties class implementation
//
const std::string properties::PRIVATE_PROPERTY_PREFIX = "__";

void properties::_validate_key_(const std::string& prop_key) const {
  if (_key_validator_ != 0) {
    if (!_key_validator_->operator()(prop_key)) {
      std::ostringstream message;
      message << "datatools::properties::_validate_key_: After key validator, the '"
              << prop_key << "' key is not valid!";
      throw std::logic_error(message.str());
    }
  }
}


void properties::set_description(const std::string& a_description) {
  _description_ = a_description;
}


const std::string & properties::get_description() const {
  return _description_;
}


bool properties::is_debug() const {
  return _debug_;
}


void properties::set_debug(bool a_new_value) {
  _debug_ = a_new_value;
}


int32_t properties::size() const {
  return _props_.size();
}


bool properties::empty() const {
  return _props_.empty();
}


void properties::set_key_validator(
    const basic_key_validator& prop_key_validator) {
  _clear_key_validator_();
  _key_validator_ = &prop_key_validator;
  _key_validator_deletion_ = false;
}


void properties::set_key_validator(
    const basic_key_validator* prop_key_validator,
    bool a_deletion_on_destroy) {
  if (_key_validator_ != 0 && _key_validator_== prop_key_validator) {
    return;
  }
  _clear_key_validator_();
  if (prop_key_validator != 0) {
    _key_validator_ = prop_key_validator;
    _key_validator_deletion_ = a_deletion_on_destroy;
  }
}


void properties::set_default_key_validator() {
  this->_clear_key_validator_();
  _key_validator_ = &g_default_key_validator;// new default_key_validator;
  _key_validator_deletion_ = false; //true;
}


void properties::unset_key_validator() {
  this->_clear_key_validator_();
}


// ctor:
properties::properties() : _debug_(false), _key_validator_(0) {
  this->set_description("");
  this->set_default_key_validator();
}


// ctor:
properties::properties(const std::string& a_description) 
    : _debug_(false), _key_validator_(0) {
  this->set_description(a_description);
  this->set_default_key_validator();
}


// ctor:
properties::properties(const std::string& a_description,
                       const basic_key_validator& prop_key_validator)
    : _debug_(false), _key_validator_(0) {
  this->set_description(a_description);
  this->set_key_validator(prop_key_validator);
}


// ctor:
properties::properties(const basic_key_validator& prop_key_validator)
    : _debug_(false), _key_validator_(0) {
  this->set_description("");
  this->set_key_validator(prop_key_validator);
}


// ctor:
properties::properties(const std::string& a_description,
                       const basic_key_validator* prop_key_validator,
                       bool a_deletion_on_destroy)
    : _debug_(false), _key_validator_(0) {
  this->set_description(a_description);
  this->set_key_validator(prop_key_validator, a_deletion_on_destroy);
}


properties::properties(const basic_key_validator* prop_key_validator,
                       bool a_deletion_on_destroy)
    : _debug_(false), _key_validator_(0) {
  this->set_description("");
  this->set_key_validator(prop_key_validator, a_deletion_on_destroy);
}


// dtor:
properties::~properties() {
  properties::clear();
}


DATATOOLS_CLONEABLE_IMPLEMENTATION(properties)


void properties::_clear_key_validator_() {
  if (_key_validator_ != 0) {
    if (_key_validator_deletion_) {
      delete _key_validator_;
    }
    _key_validator_ = 0;
  }
}


void properties::erase(const std::string& prop_key) {
  pmap::iterator found = _props_.find(prop_key);
  if (found == _props_.end()) {
    std::ostringstream message;
    message << "datatools::properties::erase: key '" 
            << prop_key << "' not known!";
    throw std::logic_error(message.str());
  }
  _props_.erase(found);
}


void properties::erase_all_starting_with(const std::string& prefix) {
  keys_col_type keys;
  this->keys_starting_with(keys, prefix);
  for (keys_col_type::const_iterator i = keys.begin(); i != keys.end(); ++i){
    this->erase(*i);
  }
}


void properties::erase_all_not_starting_with(const std::string& prefix) {
  keys_col_type keys;
  keys_not_starting_with (keys, prefix);
  for (keys_col_type::const_iterator i = keys.begin(); i != keys.end(); ++i){
    this->erase(*i);
  }
}


void properties::export_and_rename_starting_with(properties& a_props,
    const std::string& prop_key_prefix,
    const std::string& a_new_prefix) const {
  if (this == &a_props) {
    throw std::logic_error("datatools::properties::export_and_rename_starting_with: Self export is not allowed !");
  }
  keys_col_type ks;
  this->keys_starting_with(ks, prop_key_prefix);
  for (keys_col_type::const_iterator i = ks.begin(); i !=  ks.end(); ++i) {
    properties& ptmp = const_cast<properties&>(*this);
    std::string new_key = *i;
    boost::replace_first(new_key, prop_key_prefix, a_new_prefix);
    a_props._props_[new_key] = ptmp._props_[*i];
  }
}

void properties::export_all(properties & a_props) const {
  if (this == &a_props) {
    throw std::logic_error("datatools::properties::export_all: Self export is not allowed !");
  }

  keys_col_type ks;
  this->keys(ks);
  for (keys_col_type::const_iterator i = ks.begin(); i != ks.end(); ++i) {
    properties& ptmp = const_cast<properties&>(*this);
    a_props._props_[*i] = ptmp._props_[*i];
  }
}

void properties::export_starting_with(properties & a_props,
                                      const std::string& prefix) const {
  if (this == &a_props) {
    throw std::logic_error("datatools::properties::export_starting_with: Self export is not allowed !");
  }

  keys_col_type ks;
  this->keys_starting_with(ks, prefix);
  for (keys_col_type::const_iterator i = ks.begin(); i != ks.end(); ++i) {
    properties& ptmp = const_cast<properties&>(*this);
    a_props._props_[*i] = ptmp._props_[*i];
  }
}


void properties::export_not_starting_with(properties& a_props,
    const std::string& prefix) const {
  if (this == &a_props) {
    throw std::logic_error("datatools::properties::export_starting_with: Self export is not allowed !");
  }
  keys_col_type ks;
  this->keys_not_starting_with(ks, prefix);
  for (keys_col_type::const_iterator i = ks.begin(); i !=  ks.end(); ++i) {
    properties& ptmp = const_cast<properties&>(*this);
    a_props._props_[*i] = ptmp._props_[*i];
  }
}


void properties::erase_all_ending_with(const std::string& suffix) {
  keys_col_type keys;
  this->keys_ending_with(keys, suffix);
  for (keys_col_type::const_iterator i = keys.begin(); i != keys.end(); ++i) {
    this->erase(*i);
  }
}


void properties::erase_all_not_ending_with(const std::string& suffix) {
  keys_col_type keys;
  this->keys_not_ending_with(keys, suffix);
  for (keys_col_type::const_iterator i = keys.begin(); i != keys.end(); ++i) {
    this->erase (*i);
  }
}


void properties::export_ending_with(properties& a_props,
                                    const std::string& suffix) const {
  if (this == &a_props) {
    throw std::logic_error("datatools::properties::export_ending_with: Self export is not allowed !");
  }
  keys_col_type ks;
  this->keys_ending_with(ks, suffix);
  for (keys_col_type::const_iterator i = ks.begin(); i != ks.end(); ++i) {
    properties& ptmp = const_cast<properties&>(*this);
    a_props._props_[*i] = ptmp._props_[*i];
  }
}


void properties::export_not_ending_with(properties& a_props,
                                        const std::string& suffix) const {
  if (this == &a_props) {
    throw std::logic_error("datatools::properties::export_ending_with: Self export is not allowed !");
  }
  keys_col_type ks;
  this->keys_not_ending_with(ks, suffix);
  for (keys_col_type::const_iterator i = ks.begin(); i !=  ks.end(); ++i) {
    properties& ptmp = const_cast<properties&>(*this);
    a_props._props_[*i] = ptmp._props_[*i];
  }
}


void properties::erase_all() {
  _props_.clear();
}


void properties::clean(const std::string& key) {
  pmap::iterator found = _props_.find(key);
  if (found != _props_.end()) {
    _props_.erase(found);
  }
}


void properties::clear() {
  this->reset();
}


void properties::reset() {
  this->set_description("");
  _props_.clear();
  this->_clear_key_validator_();
  _debug_ = false;
}


void properties::keys_not_starting_with(properties::vkeys& keys,
                                        const std::string& prefix) const {
  if (prefix.empty()) {
    throw std::logic_error("datatools::properties::keys_not_starting_with: Empty key prefix argument !");
  }

  size_t n = prefix.size();
  for (pmap::const_iterator iter = _props_.begin();
       iter != _props_.end();
       ++iter) {
    bool push = true;
    if (iter->first.substr(0, n) == prefix) push = false;
    if (push) keys.push_back(iter->first);
  }
}


properties::vkeys properties::keys_not_starting_with(
    const std::string& prefix) const {
  properties::vkeys lkeys;
  this->keys_not_starting_with(lkeys, prefix);
  return lkeys;
}


void properties::keys_starting_with(properties::vkeys& keys,
                                    const std::string& prefix) const {
  if (prefix.empty()) {
    throw std::logic_error("datatools::properties::keys_starting_with: Empty key prefix argument !");
  }

  size_t n = prefix.size();
  for (pmap::const_iterator iter = _props_.begin();
       iter != _props_.end();
       ++iter) {
    if (iter->first.size() < n) continue;
    if (iter->first.substr(0, n) == prefix) {
      keys.push_back(iter->first);
    }
  }
}


properties::vkeys properties::keys_starting_with(
    const std::string& prefix) const {
  properties::vkeys lkeys;
  this->keys_starting_with(lkeys, prefix);
  return lkeys;
}


void properties::keys_not_ending_with(properties::vkeys& prop_keys,
                                      const std::string& suffix) const {
  if (suffix.empty()) {
    throw std::logic_error("datatools::properties::keys_not_ending_with: Empty key suffix argument !");
  }

  size_t n = suffix.size();
  for (pmap::const_iterator iter = _props_.begin();
       iter != _props_.end();
       ++iter) {
    bool push = true;
    if (iter->first.substr(iter->first.size() - n, iter->first.size())
        == suffix) {
      push = false;
    }
    if (push) prop_keys.push_back(iter->first);
  }
}


properties::vkeys properties::keys_not_ending_with(
    const std::string& suffix) const {
  properties::vkeys lkeys;
  this->keys_not_ending_with(lkeys, suffix);
  return lkeys;
}


void properties::keys_ending_with(properties::vkeys& prop_keys,
                                  const std::string& suffix) const {
  if (suffix.empty()) {
    throw std::logic_error("datatools::properties::keys_ending_with: Empty key suffix argument !");
  }

  size_t n = suffix.size();
  for (pmap::const_iterator iter = _props_.begin();
       iter != _props_.end();
       ++iter) {
    if (iter->first.size() < n) continue;
    if (iter->first.substr(iter->first.size()-n, iter->first.size())
        == suffix) {
      prop_keys.push_back(iter->first);
    }
  }
}


properties::vkeys properties::keys_ending_with(
    const std::string& suffix) const {
  properties::vkeys lkeys;
  this->keys_ending_with(lkeys, suffix);
  return lkeys;
}


properties::vkeys properties::keys() const {
  properties::vkeys lkeys;
  this->keys(lkeys);
  return lkeys;
}


void properties::keys(properties::vkeys& keys) const {
  for (pmap::const_iterator iter = _props_.begin();
       iter != _props_.end();
       ++iter) {
    keys.push_back(iter->first);
  }
}

const std::string & properties::key (int key_index_) const
{
  int key_count = 0;
  pmap::const_iterator iter = _props_.begin();
  for (;
       iter != _props_.end();
       ++iter, ++key_count) {
    if (key_count == key_index_)
      {
        break;
      };
  }
  if (iter == _props_.end())
    {
    std::ostringstream message;
    message << "datatools::utils::properties::key: Invalid key index '" 
            << key_index_ << "' !";
    throw std::logic_error(message.str());
    }
  return iter->first;
}

void properties::lock(const std::string& key) {
  key_lock (key);
  /*
  data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  data_ptr->lock();
  */
}


void properties::unlock(const std::string& key) {
  key_unlock (key);
  /*
  data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  data_ptr->unlock();
  */
}


bool properties::is_locked(const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return data_ptr->is_locked();
}


std::string properties::make_private_key(const std::string& key) {
  if (properties::key_is_private(key)) return key;
  
  std::ostringstream oss;
  oss << PRIVATE_PROPERTY_PREFIX << key;
  return oss.str();
}


bool properties::key_is_private(const std::string& key) {
  if (key.size() < 2) return false;
  return key.substr(0, PRIVATE_PROPERTY_PREFIX.size()) 
      == PRIVATE_PROPERTY_PREFIX;
}


bool properties::key_is_public(const std::string& key) {
  return !properties::key_is_private(key);
}


bool properties::is_private(const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return this->key_is_private(key);
}


bool properties::is_public(const std::string& key) const {
  return !this->is_private(key);
}


bool properties::is_boolean(const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return data_ptr->is_boolean();
}


bool properties::is_integer(const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return data_ptr->is_integer();
}


bool properties::is_real(const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return data_ptr->is_real();
}


bool properties::is_string (const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return data_ptr->is_string();
}


bool properties::is_scalar (const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return data_ptr->is_scalar();
}


bool properties::is_vector(const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return data_ptr->is_vector();
}


int32_t properties::size(const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return data_ptr->size();
}

int32_t properties::key_size(const std::string& key) const {
  const data *data_ptr = 0;
  this->_check_key_(key, &data_ptr);
  return data_ptr->size();
}


bool properties::has_key(const std::string& key) const {
  if (_props_.size() == 0) return false;
  bool hk = _props_.find(key) != _props_.end();
  return hk;
}


void properties::key_lock (const std::string& prop_key)
{
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  data_ptr->lock();
  return;
}
  
void properties::key_unlock (const std::string& prop_key)
{
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  data_ptr->unlock();
  return;
}

const std::string & 
properties::get_key_description (const std::string& prop_key) const
{
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  return data_ptr->get_description();
}

void properties::set_key_description (const std::string& prop_key, 
                                      const std::string& desc_)
{
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  data_ptr->set_description(desc_);
  return;
}

void properties::_check_nokey_(const std::string& prop_key) const {
  if (this->has_key(prop_key)) {
    std::ostringstream message;
    message << "datatools::properties::_check_nokey_: key '" 
            << prop_key << "' already used";
    if (!this->get_description().empty()) {
      message << " (" << this->get_description() << ")";
    }
    message << " !";
    throw std::logic_error(message.str());
  }
}


void properties::_check_key_(const std::string& prop_key, data **a_data) {
  pmap::iterator iter = _props_.find(prop_key);
  if (_props_.find(prop_key) == _props_.end()) {
    std::ostringstream message;
    message << "datatools::properties::_check_key_: key '" 
            << prop_key << "' does not exist!";
    throw std::logic_error(message.str());
  }
  *a_data = &(iter->second);
  //iter->second.tree_dump(std::cerr, "properties::_check_key_: ", "DEVEL: ");
}


void properties::_check_key_(const std::string& prop_key, 
                             const data **a_data) const {
  pmap::const_iterator iter = _props_.find(prop_key);
  if (_props_.find(prop_key) == _props_.end()) {
    std::ostringstream message;
    message << "datatools::properties::_check_key_: key '" 
            << prop_key << "' does not exist!";
    throw std::logic_error(message.str());
  }
  *a_data = &(iter->second);
}


void properties::store_flag(const std::string& prop_key,
                            const std::string& a_description,
                            bool a_lock) {
  this->store(prop_key, true, a_description, a_lock);
}


void properties::store(const std::string& prop_key, bool a_value,
                       const std::string& a_description, bool a_lock) {
  this->_check_nokey_(prop_key);
  this->_validate_key_(prop_key);
  data a_data(a_value, data::SCALAR_DEF);
  a_data.set_description(a_description);
  _props_[prop_key] = a_data;
  if (a_lock) _props_[prop_key].lock();
}


void properties::store (const std::string& prop_key, int value,
                        const std::string& description, bool lock) {
  this->_check_nokey_(prop_key);
  this->_validate_key_(prop_key);
  data a_data(value);
  a_data.set_description(description);
  _props_[prop_key] = a_data;
  if (lock) _props_[prop_key].lock();
}


void properties::store(const std::string& prop_key, double value,
                       const std::string& description, bool lock) {
  this->_check_nokey_(prop_key);
  this->_validate_key_(prop_key);
  data a_data(value);
  a_data.set_description(description);
  _props_[prop_key] = a_data;
  if (lock) _props_[prop_key].lock();
}


bool properties::has_explicit_unit(const std::string& prop_key) const
{
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (! data_ptr->is_real()) {
    std::ostringstream message;
    message << "datatools::properties::set_explicit_unit: Property '" 
            << prop_key << "' is not of real type !";
    throw std::logic_error(message.str());
  }
  return data_ptr->has_explicit_unit();
}


void properties::set_explicit_unit(const std::string& prop_key, bool a_explicit_unit)
{
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (! data_ptr->is_real()) {
    std::ostringstream message;
    message << "datatools::properties::set_explicit_unit: Property '" 
            << prop_key << "' is not of real type !";
    throw std::logic_error(message.str());
  }
  int error = data_ptr->set_explicit_unit(a_explicit_unit);
}

void properties::store(const std::string& key, const std::string& value,
                        const std::string& description, bool lock) {
  this->_check_nokey_(key);
  this->_validate_key_(key);
  data a_data(value);
  a_data.set_description(description);
  _props_[key] = a_data;
  if (lock) _props_[key].lock();
}


void properties::store(const std::string& prop_key, const char* value,
                       const std::string& description, bool lock) {
  this->store(prop_key, std::string(value), description, lock);
}


void properties::store(const std::string& prop_key, const data::vbool& values,
                       const std::string& description, bool lock) {
  this->_check_nokey_(prop_key);
  this->_validate_key_(prop_key);
  int size = values.size();
  data a_data(data::TYPE_BOOLEAN_SYMBOL, size);
  a_data.set_description(description);
  _props_[prop_key] = a_data;
  for (int i = 0; i < size; i++) {
    _props_[prop_key].set_value(values[i], i);
  }
  if (lock) _props_[prop_key].lock();
}


void properties::store(const std::string& prop_key, const data::vint& values,
                       const std::string& description, bool lock) {
  this->_check_nokey_(prop_key);
  this->_validate_key_(prop_key);
  int size = values.size();
  data a_data(data::TYPE_INTEGER_SYMBOL, size);
  a_data.set_description(description);
  _props_[prop_key] = a_data;
  for (int i = 0; i < size; i++) {
    _props_[prop_key].set_value(values[i], i);
  }
  if (lock) _props_[prop_key].lock();
}


void properties::store(const std::string& prop_key, const data::vdouble& values,
                       const std::string& description, bool lock) {
  this->_check_nokey_(prop_key);
  this->_validate_key_(prop_key);
  int size = values.size();
  data a_data(data::TYPE_REAL_SYMBOL, size);
  a_data.set_description(description);
  _props_[prop_key] = a_data;
  for (int i = 0; i < size; i++) {
    _props_[prop_key].set_value(values[i], i);
  }
  if (lock) _props_[prop_key].lock();
}


void properties::store(const std::string& prop_key, const data::vstring& values,
                       const std::string& description, bool lock) {
  this->_check_nokey_(prop_key);
  this->_validate_key_(prop_key);
  int size = values.size();
  data a_data(data::TYPE_STRING_SYMBOL, size);
  a_data.set_description(description);
  _props_[prop_key] = a_data;
  for (int i = 0; i < size; i++) {
    _props_[prop_key].set_value(values[i], i);
  }
  if (lock) _props_[prop_key].lock();
}


void properties::set_flag(const std::string& prop_key) {
  this->store_flag(prop_key, "", false);
}


void properties::unset_flag(const std::string& prop_key) {
  this->clean(prop_key);
}


void properties::store_boolean(const std::string& prop_key, bool value,
                               const std::string& desc, bool lock) {
  this->store(prop_key, value, desc, lock);
}


void properties::store_integer(const std::string& prop_key, int value,
                               const std::string& desc, bool lock) {
  this->store(prop_key, value, desc, lock);
}


void properties::store_real(const std::string& prop_key, double value,
                            const std::string& desc, bool lock) {
  this->store(prop_key, value, desc, lock);
}


void properties::store_string(const std::string& prop_key,
                              const std::string& value,
                              const std::string& desc,
                              bool lock) {
  this->store(prop_key, value, desc, lock);
}


void properties::change_boolean(const std::string& prop_key, bool value,
                                int index) {
  this->change(prop_key, value, index);
}

void properties::change_boolean_scalar (const std::string& prop_key, bool value) {
  this->change(prop_key, value, 0);
}

void properties::change_boolean_vector(const std::string& prop_key, bool value,
                                       int index) {
  this->change(prop_key, value, index);
}


void properties::change_integer(const std::string& prop_key, int value,
                                int index) {
  this->change(prop_key, value, index);
}

void properties::change_integer_scalar(const std::string& prop_key, int value) {
  this->change(prop_key, value, 0);
}

void properties::change_integer_vector(const std::string& prop_key, int value,
                                       int index) {
  this->change(prop_key, value, index);
}


void properties::change_real(const std::string& prop_key, double value,
                             int index) {
  this->change(prop_key, value, index);
}


void properties::change_real_scalar(const std::string& key, double value)
{
  this->change(key, value, 0);
}

void properties::change_real_vector(const std::string& key, double value,
                                    int index) {
  this->change(key, value, index);
}

void properties::change_string(const std::string& prop_key,
                               const std::string& value,
                               int index) {
  this->change(prop_key, value, index);
}

void properties::change_string_scalar(const std::string& prop_key,
                                      const std::string& value) {
  this->change(prop_key, value, 0);
}


void properties::change_string_vector(const std::string& prop_key,
                                      const std::string& value,
                                      int index) {
  this->change_string(prop_key, value, index);
}

void properties::change(const std::string& prop_key, bool value, int index) {
  //std::cerr << "DEVEL: " 
  //          << "datatools::properties::change(bool): Entering..." << '\n';
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);

  int error = data_ptr->set_value(value, index);
  if (error != data::ERROR_SUCCESS) {
    std::ostringstream message;
    message << "datatools::properties::change(bool): Cannot change value for property '" 
            << prop_key << "': "
            << data::get_error_message(error) << "!";
    throw std::logic_error(message.str());
  }
}


void properties::change(const std::string& prop_key, int value, int index) {
  // std::cerr << "DEVEL: " 
  //           << "datatools::properties::change(int): Entering..." << '\n';
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  int error = data_ptr->set_value(value, index);
  if (error != data::ERROR_SUCCESS) {
    std::ostringstream message;
    message << "datatools::properties::change(int): Cannot change value for property '" 
            << prop_key << "': "
            << data::get_error_message(error) << "!";
    throw std::logic_error(message.str());
  }
}


void properties::change(const std::string& prop_key, double value, int index) {
  // std::cerr << "DEVEL: " 
  //           << "datatools::properties::change(double): Entering..." << '\n';
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  int error = data_ptr->set_value(value, index);
  if (error != data::ERROR_SUCCESS) {
    std::ostringstream message;
    message << "datatools::properties::change(double): Cannot change value for property '" 
            << prop_key << "': "
            << data::get_error_message (error) << "!";
    throw std::logic_error(message.str());
  }
}


void properties::change(const std::string& prop_key, const std::string& value,
                        int index) {
  // std::cerr << "DEVEL: " 
  //           << "datatools::properties::change(string): Entering..." << '\n';
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  int error = data_ptr->set_value(value, index);
  if (error != data::ERROR_SUCCESS) {
    std::ostringstream message;
    message << "datatools::properties::change(string): Cannot change value for property '"
            << prop_key << "': "
            << data::get_error_message(error) << "!";
    throw std::logic_error(message.str());
  }
}


void properties::change(const std::string& prop_key, const char* value,
                        int index) {
  // std::cerr << "DEVEL: " 
  //           << "datatools::properties::change(const char*): Entering..." << '\n';
  std::string tmp = "";
  if (value != 0) tmp = value;
  this->change(prop_key, tmp, index);
}


void properties::change(const std::string& prop_key, const data::vbool& values) {
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (!data_ptr->is_boolean() || !data_ptr->is_vector()) {
    std::ostringstream message;
    message << "datatools::properties::change: property '"
            << prop_key 
            << "' is not a vector of booleans!";
    throw std::logic_error(message.str());
  }
  if ((int)values.size() != data_ptr->get_size()) {
    int error = data_ptr->boolean(values.size());
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::change: Cannot change values for vector property '" 
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
  for (int i = 0; i < (int)values.size(); ++i) {
    int error = data_ptr->set_value(values[i], i);
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::change: Cannot change values for vector property '" 
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
}


void properties::change(const std::string& prop_key, const data::vint& values) {
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (! data_ptr->is_integer() || !data_ptr->is_vector()) {
    std::ostringstream message;
    message << "datatools::properties::change: property '"
            << prop_key 
            << "' is not a vector of integers!";
    throw std::logic_error(message.str());
  }
  if ((int)values.size() != data_ptr->get_size()) {
    int error = data_ptr->integer(values.size());
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::change: Cannot change values for vector property '" 
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
  for (int i = 0; i < (int)values.size(); ++i) {
    int error = data_ptr->set_value(values[i], i);
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::change: Cannot change values for vector property '" 
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
}


void properties::change(const std::string& prop_key,
                        const data::vdouble& values) {
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (!data_ptr->is_real() || ! data_ptr->is_vector()) {
    std::ostringstream message;
    message << "datatools::properties::change: property '"
            << prop_key << "' is not a vector of reals!";
    throw std::logic_error(message.str());
  }
  if ((int)values.size() != data_ptr->get_size()) {
    int error = data_ptr->real(values.size());
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::change: Cannot change values for vector property '" 
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
  for (int i = 0; i < (int)values.size(); ++i) {
    int error = data_ptr->set_value(values[i], i);
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::change: Cannot change values for vector property '" 
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
}


void properties::change(const std::string& prop_key,
                        const data::vstring& values) {
  data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (!data_ptr->is_string() || ! data_ptr->is_vector()) {
    std::ostringstream message;
    message << "datatools::properties::change: property '"
            << prop_key << "' is not a vector of strings!";
    throw std::logic_error(message.str());
  }
  if ((int)values.size() != data_ptr->get_size()) {
    int error = data_ptr->string(values.size());
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::change: Cannot change values for vector property '" 
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
  for (int i = 0; i < (int)values.size(); ++i) {
    int error = data_ptr->set_value(values[i], i);
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::change: Cannot change values for vector property '" 
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
}


void properties::dump_stderr() const {
  this->dump(std::cerr);
}


void properties::dump_stdout() const {
  this->dump(std::cout);
}


void properties::update_string(const std::string& prop_key,
                               const std::string& value) {
  this->update(prop_key, value);
}


void properties::update_boolean(const std::string& prop_key, bool value) {
  this->update(prop_key, value);
}


int properties::fetch_integer_scalar(const std::string& name) const {
  return this->fetch_integer(name, 0);
}


int properties::fetch_integer_vector(const std::string& name, 
                                     int index) const {
  return this->fetch_integer(name, index);
}


bool properties::fetch_boolean_scalar(const std::string& name) const {
  return this->fetch_boolean(name, 0);
}


bool properties::fetch_boolean_vector(const std::string& name, 
                                      int index) const {
  return this->fetch_boolean(name, index);
}


double properties::fetch_real_scalar(const std::string& name) const {
  return this->fetch_real(name, 0);
}


double properties::fetch_real_vector(const std::string& name, 
                                     int index) const {
  return this->fetch_real(name, index);
}


void properties::update_flag(const std::string& prop_key) {
  this->update(prop_key, true);
}


std::string properties::fetch_string_scalar(
    const std::string& name) const {
  return this->fetch_string(name, 0);
}


std::string properties::fetch_string_vector(const std::string& name, 
                                            int index) const {
  return this->fetch_string(name, index);
}


std::string properties::fetch_path_scalar(const std::string& name) const {
  return this->fetch_path(name, 0);
}


std::string properties::fetch_path_vector(const std::string& name, 
                                          int index) const {
  return this->fetch_path(name, index);
}


void properties::update(const std::string& prop_key, bool value) {
  if (this->has_key(prop_key)) {
    this->change(prop_key, value);
  } else {
    this->store(prop_key, value);
  }
}


void properties::update(const std::string& prop_key, int value) {
  if (this->has_key(prop_key)) {
    this->change(prop_key, value);
  } else {
    this->store(prop_key, value);
  }
}


void properties::update(const std::string& prop_key, double value) {
  if (this->has_key(prop_key)) {
    this->change(prop_key, value);
  } else {
    this->store(prop_key, value);
  }
  return;
}


void properties::update(const std::string& prop_key, const std::string& value) {
  if (this->has_key(prop_key)) {
    this->change(prop_key, value);
  } else {
    this->store(prop_key, value);
  }
}


void properties::update(const std::string& prop_key, const char* value) {
  std::string tmp = "";
  if (value != 0) tmp = value;
  if (this->has_key(prop_key)) {
    this->change(prop_key, tmp);
  } else {
    this->store(prop_key, tmp);
  }
}


void properties::update(const std::string& prop_key,
                        const data::vbool& values) {
  if (this->has_key(prop_key)) {
    this->change(prop_key, values);
  } else {
    this->store(prop_key, values);
  }
}


void properties::update(const std::string& prop_key, const data::vint& values) {
  if (this->has_key(prop_key)) {
    this->change(prop_key, values);
  } else {
    this->store(prop_key, values);
  }
}


void properties::update(const std::string& prop_key,
                        const data::vdouble& values) {
  if (this->has_key(prop_key)) {
    this->change(prop_key, values);
  } else {
    this->store(prop_key, values);
  }
}


void properties::update(const std::string& prop_key,
                        const data::vstring& values) {
  if (this->has_key(prop_key)) {
    this->change(prop_key, values);
  } else {
    this->store(prop_key, values);
  }
}


void properties::update_integer(const std::string& prop_key, int value) {
  this->update(prop_key, value);
}


void properties::update_real(const std::string& prop_key, double value) {
  this->update(prop_key, value);
}



void properties::fetch(const std::string& prop_key, bool& value, 
                       int index) const {
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  int error = data_ptr->get_value(value, index);
  if (error != data::ERROR_SUCCESS) {
    std::ostringstream message;
    message << "datatools::properties::fetch: Cannot fetch boolean value from property '" 
            << prop_key << "': "
            << data::get_error_message(error) << "!";
    throw std::logic_error(message.str());
  }
}


void properties::fetch(const std::string& prop_key, int& value, 
                       int index) const {
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  int error = data_ptr->get_value(value, index);
  if (error != data::ERROR_SUCCESS) {
    std::ostringstream message;
    message << "datatools::properties::fetch: Cannot fetch integer value from property '" 
            << prop_key << "': "
            << data::get_error_message(error) << "!";
    throw std::logic_error(message.str());
  }
}


void properties::fetch(const std::string& prop_key, double& value, 
                       int index) const {
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  int error = data_ptr->get_value(value, index);
  if (error != data::ERROR_SUCCESS) {
    std::ostringstream message;
    message << "datatools::properties::fetch: Cannot fetch real value from property '" 
           << prop_key << "': "
           << data::get_error_message(error) << "!";
    throw std::logic_error(message.str());
  }
}


void properties::fetch(const std::string& prop_key, std::string& value, 
                       int index) const {
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  int error = data_ptr->get_value(value, index);
  if (error != data::ERROR_SUCCESS) {
    std::ostringstream message;
    message << "datatools::properties::fetch: Cannot fetch string value from property '" 
            << prop_key << "': "
            << data::get_error_message(error) << "!";
    throw std::logic_error(message.str());
  }
}


bool properties::has_flag(const std::string& prop_key) const {
  bool flag_is_on = false;
  try {
    flag_is_on = this->fetch_boolean(prop_key, 0);
  }
  catch (std::logic_error& e) {
  }
  return flag_is_on;
}


bool properties::fetch_boolean(const std::string& prop_key, int index) const {
  bool value;
  this->fetch(prop_key, value, index);
  return value;
}


int properties::fetch_integer(const std::string& prop_key, int index) const {
  int value;
  this->fetch(prop_key, value, index);
  return value;
}


double properties::fetch_real(const std::string& prop_key, int index) const {
  double value;
  this->fetch(prop_key, value, index);
  return value;
}


std::string properties::fetch_string(const std::string& prop_key,
                                     int index) const {
  std::string value;
  this->fetch(prop_key, value, index);
  return value;
}


std::string properties::fetch_path(const std::string& prop_key,
                                   int index) const {
  std::string value;
  this->fetch(prop_key, value, index);
  if (!fetch_path_with_env(value)) {
    std::ostringstream message;
    message << "datatools::properties::fetch_path: property '"
            << prop_key 
            << "' cannot be interpreted as a valid path string !";
    throw std::logic_error(message.str());
  }
  return value;
}


void properties::fetch(const std::string& prop_key, data::vbool& values) const {
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (!data_ptr->is_boolean() || !data_ptr->is_vector()) {
    std::ostringstream message;
    message << "datatools::properties::fetch: property '"
            << prop_key 
            << "' is not a vector of booleans!";
    throw std::logic_error(message.str());
  }
  values.resize(data_ptr->size());
  values.assign(data_ptr->size(), data::DEFAULT_VALUE_BOOLEAN);
  for (int i = 0; i < (int)values.size(); ++i) {
    bool val;
    int error = data_ptr->get_value(val, i);
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::fetch: Cannot fetch boolean values for vector property '"
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
    values[i] = val;
  }
}


void properties::fetch(const std::string& prop_key, data::vint& values) const {
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (!data_ptr->is_integer() || ! data_ptr->is_vector()) {
    std::ostringstream message;
    message << "datatools::properties::fetch: property '"
            << prop_key 
            << "' is not a vector of integers!";
    throw std::logic_error(message.str());
  }
  values.resize(data_ptr->size());
  values.assign(data_ptr->size(), data::DEFAULT_VALUE_INTEGER);
  for (int i = 0; i < (int)values.size(); ++i) {
    int error = data_ptr->get_value(values[i], i);
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::fetch: Cannot fetch integer values for vector property '"
             << prop_key << "': "
             << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
}


void properties::fetch(const std::string& prop_key,
                       data::vdouble& values) const {
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (!data_ptr->is_real() || !data_ptr->is_vector()) {
    std::ostringstream message;
    message << "datatools::properties::fetch: property '"
            << prop_key 
            << "' is not a vector of reals!";
    throw std::logic_error(message.str());
  }
  values.resize(data_ptr->size());
  values.assign(data_ptr->size(), data::DEFAULT_VALUE_REAL);
  for (int i = 0; i < (int)values.size(); ++i) {
    int error = data_ptr->get_value(values[i], i);
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::fetch: Cannot fetch real values for vector property '"
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
}


void properties::fetch(const std::string& prop_key, 
                       data::vstring& values) const {
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  if (!data_ptr->is_string() || ! data_ptr->is_vector()) {
    std::ostringstream message;
    message << "datatools::properties::fetch: property '"
            << prop_key 
            << "' is not a vector of strings!";
    throw std::logic_error(message.str());
  }
  values.resize(data_ptr->size());
  values.assign (data_ptr->size (), data::DEFAULT_VALUE_STRING);
  for (int i = 0; i < (int)values.size(); ++i) {
    int error = data_ptr->get_value(values[i], i);
    if (error != data::ERROR_SUCCESS) {
      std::ostringstream message;
      message << "datatools::properties::fetch: Cannot fetch string values for vector property '"
              << prop_key << "': "
              << data::get_error_message(error) << "!";
      throw std::logic_error(message.str());
    }
  }
}


std::string properties::key_to_string(const std::string& prop_key) const {
  if (!this->has_key(prop_key)) return "";
  std::ostringstream oss;
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  data_ptr->to_string(oss);
  return oss.str();
}


std::string properties::key_to_property_string(
    const std::string& prop_key) const {
  if (!this->has_key(prop_key)) return "";
  std::ostringstream oss;
  const data *data_ptr = 0;
  this->_check_key_(prop_key, &data_ptr);
  oss << prop_key << '=';
  data_ptr->to_string(oss);
  return oss.str();
}


void properties::tree_dump(std::ostream& out, const std::string& title,
                           const std::string& a_indent, 
                           bool inherit) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;
  if (!title.empty()) out << indent << title << std::endl;
   
  if (!_description_.empty()) {
    out << indent << i_tree_dumpable::tag
        << "Description  : '" << this->get_description() << "'" << std::endl;
  }

  if (_props_.size() == 0) {
    out << indent << i_tree_dumpable::inherit_tag(inherit)
        << "<no property>" << std::endl;
  } else {
    for (pmap::const_iterator i = _props_.begin(); 
         i != _props_.end(); 
         ++i) {
      const std::string& prop_key = i->first;
      const properties::data& a_data = i->second;
      out << indent;
      std::ostringstream indent_oss;
      indent_oss << indent;
      pmap::const_iterator j = i;
      j++;
      if (j == _props_.end()) {
        out << i_tree_dumpable::inherit_tag(inherit);
        indent_oss << i_tree_dumpable::inherit_skip_tag(inherit);
      } else {
        out << i_tree_dumpable::tag;
        indent_oss << i_tree_dumpable::skip_tag;
      }
      out << "Name : " << '"' << prop_key << '"' << std::endl;
      a_data.tree_dump(out, "", indent_oss.str());
    }
  }
}


void properties::dump(std::ostream& out) const {
  this->tree_dump(out, "datatools::properties");
}

void properties::write_configuration (const std::string& filename,
                                      bool a_use_smart_modulo,
                                      bool a_write_public_only) const
{
  write_config(filename, 
               *this, 
               a_use_smart_modulo, 
               a_write_public_only);
  return;
} 
  
void properties::read_configuration (const std::string& filename)
{
  read_config(filename, *this);
  return;
}

void properties::write_config(const std::string& a_filename,
                              const properties& a_props,
                              bool a_use_smart_modulo,
                              bool a_write_public_only) {
  std::ostream *out = &std::cout;
  std::ofstream fout;

  if (!a_filename.empty()) {
    fout.open(a_filename.c_str());
    if (!fout) {
      std::ostringstream message;
      message << "datatools::properties::write_config: "
              << "Cannot open file '"
              << a_filename << "!"
              << std::endl;
      throw std::logic_error(message.str());
    }
    out = &fout;
  }
  config a_cfg(a_use_smart_modulo,
               config::mode_header_footer,
               (a_write_public_only ? 
                config::write_public_only:
                config::write_private_also));

  a_cfg.write(*out, a_props);
}


void properties::read_config(const std::string& filename,
                             properties& props) {
  std::ifstream fin(filename.c_str());
  if (!fin) {
    std::ostringstream message;
    message << "datatools::properties::read_config: "
            << "Cannot open file '"
        << filename << "!"
        << std::endl;
    throw std::logic_error(message.str());
  }
  config cfg;
  props.clear();
  cfg.read(fin, props);
  return;
}

/**********************************************************************/
//----------------------------------------------------------------------
// properties::config class implementation
//
const bool properties::config::write_public_only = true;
const bool properties::config::write_private_also = false;

void properties::config::write(std::ostream& out,
                               const properties& a_props) {
  int mode = _mode_;
  if (mode == MODE_HEADER_FOOTER) {
    out << "# List of properties (datatools::properties):" 
        << std::endl; 
    out << std::endl;
  }

  if (!a_props.get_description().empty()) {
    out << "#@config " << a_props.get_description() << std::endl;
    out << std::endl;
  }

  for (pmap::const_iterator i = a_props._props_.begin();
       i != a_props._props_.end();
       ++i) {
    const std::string       prop_key  = i->first;
    const properties::data& a_data = i->second;

    if (_write_public_only_) {
      if (key_is_private(prop_key)) continue;
    }

    if (!a_data.get_description().empty()) {
      out << "#@description " << a_data.get_description() << std::endl;
    }

    // name:
    out << prop_key << " : ";

    if (a_data.is_locked()) out << "const ";

    // type:
    int size = properties::data::SCALAR_SIZE;
    out << a_data.get_type_label();
    
    if (a_data.is_vector()) {
      size = a_data.get_size();
      out << '[' << a_data.get_size() << ']';
    }
    out << ' ';

    out << "=";

    // values (s):
    int modulo = 1;

    if (a_data.is_boolean()) modulo = 10;

    if (a_data.is_integer()) modulo = 5;

    if (_use_smart_modulo_) {
      if ((size > 1) && (size > modulo)) {
        out << ' ' << _continuation_char_ << '\n';
      }
    }

    for (int i = 0; i < size; i++) {
      out << ' ';
      if (a_data.is_boolean()) out << a_data.get_boolean_value(i);

      if (a_data.is_integer()) out << a_data.get_integer_value(i);

      if (a_data.is_real()) {
        int oldprec = out.precision();
        out.precision(16);
        out << a_data.get_real_value(i);
        out.precision(oldprec);
      }

      if (a_data.is_string()) {
        out << '"' << a_data.get_string_value(i) << '"';
      }

      if (_use_smart_modulo_) {
        if ((i < (size - 1)) && ((i + 1) % modulo) == 0) {
          out << ' ' << _continuation_char_ << '\n';
        }
      }
    }
    out << std::endl << std::endl;
  }

  if (mode == MODE_HEADER_FOOTER) {
    out << "# End of list of properties." << std::endl;
  }
}


bool properties::config::is_debug() const {
  return _debug_;
}


void properties::config::set_debug(bool debug) {
  _debug_ = debug;
}


properties::config::config(bool a_use_smart_modulo,
                           int a_mode,
                           bool a_write_public_only) {
  _debug_ = false;
  _use_smart_modulo_ = a_use_smart_modulo ;
  _mode_ = MODE_DEFAULT;
  _mode_ = a_mode;
  _write_public_only_ = a_write_public_only;
  _read_line_count_ = 0;
  _continuation_char_ = DEFAULT_CONTINUATION_CHAR;
  _comment_char_ = DEFAULT_COMMENT_CHAR;
  _assign_char_ = DEFAULT_ASSIGN_CHAR;
  _desc_char_ = DEFAULT_DESC_CHAR;
}

properties::config::~config() {}


void properties::config::read(std::istream& in, properties& props) {
  this->read_(in, props);
}


void properties::config::read_(std::istream& a_in, properties& a_props) {
  bool verbose_parsing  = _debug_ || properties::g_debug;

  std::string line_in;
  std::string prop_config;
  std::string prop_description;
  bool line_goon = false;
  bool enable_real_with_unit = false;

  while (a_in) {
    if (verbose_parsing) {
      std::cerr << "DEBUG: datatools::properties::config::read_: "
                << "loop on input stream..."
                << std::endl;
    }
    std::string line_get;
    std::getline(a_in, line_get);
    _read_line_count_++;

    // check if line has a continuation mark at the end:
    bool line_continue = false;
    int sz = line_get.size();

    if (sz > 0 && line_get[sz-1] == _continuation_char_) {
      line_continue = true;
      line_get = line_get.substr(0, sz - 1);
      
      if (verbose_parsing) {
        std::cerr << "DEBUG: datatools::properties::config::read_: "
                  << "line_get='"
                  << line_get
                  << "'" << std::endl;
      }
    }

    if (line_goon) {
      // append to previous line:
      line_in += line_get;
      if (verbose_parsing) {
        std::cerr << "DEBUG: datatools::properties::config::read_: "
                  << "append --> line_in='"
                  << line_in 
                  << "'" << std::endl;
      }
    } else {
      // a new line
      line_in = line_get;
      if (verbose_parsing) {
        std::cerr << "DEBUG: datatools::properties::config::read_: "
                  << "new --> line_in='"
                  << line_in 
                  << "'" << std::endl;
      }
    }
    line_goon = false;

    // check if line should go on:
    if (line_continue) line_goon = true;

    if (!line_goon) {
      bool skip_line = false;

      std::string line = line_in;
      if (verbose_parsing) {
        std::cerr << "DEBUG: datatools::properties::config::read_: line "
                  << _read_line_count_ << " size is "
                  << line.size() 
                  << "" << std::endl;
      }

      // check if line is blank:
      std::istringstream check_iss(line_in);
      std::string check_word;
      check_iss >> std::ws >> check_word;
      if (check_word.empty()) {
        if (verbose_parsing) {
          std::cerr << "DEBUG: datatools::properties::config::read_: line "
                    << _read_line_count_ 
                    << " is blank" << std::endl;
        }
        skip_line=true;
      }

      // check if line is a comment:
      if (!skip_line) {
        std::istringstream iss(line);
        char c = 0;
        iss >> c;
        if (c == _comment_char_) {
          if (verbose_parsing) {
            std::cerr << "DEBUG: datatools::properties::config::read_: line "
                      << _read_line_count_ 
                      << " is a comment" << std::endl;
          }

          iss >> std::ws;

          std::string token;

          iss >> token;
          if (token == "@end") {
            break;
          }

          // Maybe we should ensure only one '@config' directive
          // here only warn...
          if (token == "@config") {
            iss >> std::ws;
            std::string config_desc;
            if (!prop_config.empty()) {
              std::cerr << "WARNING: " << "datatools::properties::config::read_: "
                        << "Duplicated '@config' directive !" << std::endl;
            }
            std::getline(iss, config_desc);
            if (!config_desc.empty()) {
              prop_config = config_desc;
              a_props.set_description(config_desc);
            }
          }

          //>>>
          if (token == "@enable_real_with_unit") {
            /*
            std::cerr << "DEVEL: " << "datatools::properties::config::read_: "
                      << "@enable_real_with_unit "
                      << std::endl;
            */
            enable_real_with_unit = true;
          }
          if (token == "@disable_real_with_unit") {
            /*
            std::cerr << "DEVEL: " << "datatools::properties::config::read_: "
                      << "@disable_real_with_unit "
                      << std::endl;
            */
            enable_real_with_unit = false;
          }

          if (token == "@verbose_parsing") {
            verbose_parsing = true;
          }
          //<<<

          if (token == "@description") {
            iss >> std::ws;
            std::string desc;
            std::getline(iss, desc);
            if (!desc.empty()) {
              prop_description = desc;
            }
          }

          skip_line = true;
        }
      } // if (! skip_line)

      // parse line:
      if (!skip_line) {
        if (verbose_parsing) {
          std::cerr << "DEBUG: datatools::properties::config::read_: "
                    << "line "
                    << _read_line_count_ 
                    << " is '" 
                    << line 
                    << "'" << std::endl;
        }
        std::string line_parsing = line;

        int flag_pos = line_parsing.find_first_of(_assign_char_);
        if (flag_pos >= (int)line_parsing.size()) {
          std::ostringstream message;
          message << "datatools::properties::config::read_: "
                  << "Cannot find assign symbol !" ;
          throw std::logic_error(message.str());
        }

        // parse property desc:
        std::string property_desc_str = line_parsing.substr(0, flag_pos);
        if (verbose_parsing) {
          std::cerr << "DEBUG: datatools::properties::config::read_: "
                    << "property_desc_str='"
                    << property_desc_str 
                    << "'" << std::endl;
        }

        // parse property value:
        std::string property_value_str = line_parsing.substr(flag_pos+1);
        if (verbose_parsing) {
          std::cerr << "DEBUG: datatools::properties::config::read_: "
                    << "property_value_str='"
                    << property_value_str << "'" << std::endl;
        }

        bool scalar = true;
        bool locked = false;
        int  vsize  = -1;
        char type   = 0;
        std::string prop_key;

        int desc_pos = property_desc_str.find_first_of(_desc_char_);
        if (desc_pos == (int)property_desc_str.npos) {
          std::istringstream key_ss(property_desc_str);
          key_ss >> std::ws >> prop_key;
          type = properties::data::TYPE_STRING_SYMBOL;
        } else {
          std::string key_str = property_desc_str.substr(0, desc_pos);
          std::istringstream key_ss(key_str);
          key_ss >> std::ws >> prop_key;

          std::string type_str = property_desc_str.substr(desc_pos+1);
          std::string type_str2;
          std::string type_str3;
          if (verbose_parsing) {
            std::cerr << "DEBUG: datatools::properties::config::read_: "
                      << "type_str='"
                      << type_str << "'" << std::endl;
          }

          int vec_pos = type_str.find_first_of (OPEN_VECTOR);
          if (vec_pos != (int)type_str.npos) {
            scalar = false;
            type_str2 = type_str.substr(0, vec_pos);
            if (verbose_parsing) {
              std::cerr << "DEBUG: datatools::properties::config::read_: "
                        << "type_str2='"
                        << type_str2 << "'" << std::endl;
            }
            std::string vec_str = type_str.substr(vec_pos + 1);
            std::istringstream vec_ss(vec_str);
            vec_ss >> vsize;
            
            if (!vec_ss) {
              std::ostringstream message;
              message << "datatools::properties::config::read_: "
                      << "Cannot find vector size for key '" 
                      << prop_key 
                      << "' at line '" 
                      << line << "' !" ;
              throw std::logic_error(message.str());
            }

            if (vsize < 0) {
              std::ostringstream message;
              message << "datatools::properties::config::read_: "
                      << "Invalid vector size !" ;
              throw std::logic_error(message.str());
            }

            char c = 0;
            if (!vec_ss) {
              std::ostringstream message;
              message << "datatools::properties::config::read_: "
                      << "Cannot find expected vector size closing symbol for key '" 
                      << prop_key 
                      << "' at line '" 
                      << line << "'  !" ;
              throw std::logic_error(message.str());
            }

            vec_ss >> c;
            if (c != CLOSE_VECTOR) {
              std::ostringstream message;
              message << "datatools::properties::config::read_: "
                      << "Invalid vector size closing symbol for key '" 
                      << prop_key 
                      << "' at line '" 
                      << line << "'  !" ;
              throw std::logic_error(message.str());
            }
            std::getline(vec_ss, type_str3);
          } else {
            scalar = true;
            type_str2 = type_str;
          }
 
          /*
          std::cerr << "DEVEL: type_str2=" << type_str2 << std::endl;
          std::cerr << "DEVEL: type_str3=" << type_str3 << std::endl;
          */
          std::istringstream type_ss(type_str2);
          std::string type_token;
          
          type_ss >> std::ws >> type_token >> std::ws;
          if (type_token == "const") {
            //std::cerr << "DEVEL: is const" << std::endl;
            locked = true;
            type_token.clear();
            type_ss >> std::ws >> type_token >> std::ws;
          }
 
          //std::cerr << "DEVEL: type_token=" << type_token << std::endl;
          if  (type_token == "boolean") {
            type = properties::data::TYPE_BOOLEAN_SYMBOL;
          } else if (type_token == "integer") {
            type = properties::data::TYPE_INTEGER_SYMBOL;
          } else if (type_token == "real") {
            type = properties::data::TYPE_REAL_SYMBOL;
          } else if (type_token == "string") {
            type = properties::data::TYPE_STRING_SYMBOL;
          } else {
            std::ostringstream message;
            message << "datatools::properties::config::read_: "
                    << "Invalid type specifier '" 
                    << type_token << "' "
                    << "at key '" 
                    << prop_key << "' ";
            if (! a_props.get_description().empty()) {
              message << "for list described by '" 
                      << a_props.get_description() << "' ";
            }
            message << "!" ;
            throw std::logic_error(message.str());
          }

          if (! scalar && ! type_str3.empty())
            {
              std::istringstream iss3(type_str3);
              std::string dummy;
              iss3 >> std::ws >> dummy;
              if (! dummy.empty())
                {
                  type_ss.clear();
                  type_ss.str(type_str3);
                }
            }

          /// 2013-03-07 FM : add support for embeded unit directives in real property :
          std::string requested_unit_label;
          std::string requested_unit_symbol;
          double      requested_unit;
          datatools::invalidate(requested_unit);
          //std::cerr << "DEVEL: enable_real_with_unit=" << enable_real_with_unit << std::endl;
          std::string token;
          if (type == properties::data::TYPE_REAL_SYMBOL)
            {
              //std::cerr << "DEVEL: TYPE_REAL_SYMBOL/enable_real_with_unit" << std::endl;
              type_ss >> std::ws >> token >> std::ws;
              //std::cerr << "DEVEL: TYPE_REAL_SYMBOL/token=" << token << std::endl;
             
              if (! token.empty())
                 {
                   if (token == "as")
                     {
                       type_ss >> std::ws >> requested_unit_label;
                       //std::cerr << "DEVEL: requested_unit_label=" << requested_unit_label << std::endl;
                       if (requested_unit_label.empty())
                         {
                          std::ostringstream message;
                           message << "datatools::properties::config::read_: "
                                   << "Missing unit label (as) for real value with key '" 
                                   << prop_key 
                                   << "' at line '" 
                                   << line << "' !" ;
                           throw std::logic_error(message.str());
                         }
                        if (!units::is_unit_label_valid(requested_unit_label))
                         {
                           std::ostringstream message;
                           message << "datatools::properties::config::read_: "
                                   << "Invalid unit label '" << requested_unit_label << "' for real value with key '" 
                                   << prop_key 
                                   << "' at line '" 
                                   << line << "' !" ;
                           throw std::logic_error(message.str());
                         }
                        if (verbose_parsing) {
                          std::cerr << "DEBUG: datatools::properties::config::read_: "
                                    << "Unit label '"
                                    << requested_unit_label 
                                    << "' is valid!" << std::endl;
                        }
                     }
                   else if (token == "in")
                     {
                       type_ss >> std::ws >> requested_unit_symbol;
                       //std::cerr << "DEVEL: requested_unit_symbol=" << requested_unit_symbol << std::endl;
                       if (requested_unit_symbol.empty())
                         {
                          std::ostringstream message;
                           message << "datatools::properties::config::read_: "
                                   << "Missing unit symbol (in) for real value with key '" 
                                   << prop_key 
                                   << "' at line '" 
                                   << line << "' !" ;
                           throw std::logic_error(message.str());
                         }
                       // For vectors of real:
                       if (! scalar)
                         {
                           if (!units::find_unit(requested_unit_symbol, requested_unit, requested_unit_label))
                             {
                               std::ostringstream message;
                               message << "datatools::properties::config::read_: "
                                       << "Invalid unit symbol '" << requested_unit_symbol << "' for real value with key '" 
                                       << prop_key 
                                       << "' at line '" 
                                       << line << "' !" ;
                               throw std::logic_error(message.str());
                             }
                           if (verbose_parsing) {
                             std::cerr << "DEBUG: datatools::properties::config::read_: "
                                       << "Unit symbol '"
                                       << requested_unit_symbol 
                                       << "' is valid!" << std::endl;
                           }
                         }
                       else
                         {
                           std::ostringstream message;
                           message << "datatools::properties::config::read_: "
                                   << "Directive 'as " << requested_unit_symbol 
                                   << "' is not supported for scalar real value with key '" 
                                   << prop_key 
                                   << "' at line '" 
                                   << line << "' !" ;
                           throw std::logic_error(message.str());                          
                         }
                     }
                   else 
                     {
                       std::ostringstream message;
                       message << "datatools::properties::config::read_: "
                               << "Unknow directive '" << token 
                               << "' for real value with key '" 
                               << prop_key 
                               << "' at line '" 
                               << line << "' !" ;
                       throw std::logic_error(message.str());                          
                     }
                 }
            }
          if (! enable_real_with_unit)
            {
              if(! requested_unit_label.empty()
                 || ! requested_unit_symbol.empty())
                {
                  std::ostringstream message;
                  message << "datatools::properties::config::read_: "
                          << "The use of unit directives '" << token 
                          << "' is not allowed for real value with key '" 
                          << prop_key 
                          << "' at line '" 
                          << line << "' !" ;
                  throw std::logic_error(message.str());                                          
                }
            }
          if (verbose_parsing) {
            std::cerr << "DEBUG: datatools::properties::config::read_: "
                      << "type='"
                      << type << "'" << std::endl;
            std::cerr << "DEBUG: datatools::properties::config::read_: "
                      << "locked='"
                      << locked << "'" << std::endl;
            std::cerr << "DEBUG: datatools::properties::config::read_: "
                      << "vsize='"
                      << vsize << "'" << std::endl;
            std::cerr << "DEBUG: datatools::properties::config::read_: "
                      << "prop_description='"
                      << prop_description << "'" << std::endl;
          }

          bool        a_boolean = false;
          int         a_integer = 0;
          double      a_real    = 0.0;
          std::string a_string  = "";
          std::vector<bool>        v_booleans;
          std::vector<int>         v_integers;
          std::vector<double>      v_reals;
          std::vector<std::string> v_strings;

          if (type == properties::data::TYPE_BOOLEAN_SYMBOL && !scalar) {
            if (vsize > 0) {
              v_booleans.assign(vsize,
                                properties::data::DEFAULT_VALUE_BOOLEAN);
            }
          }

          if (type == properties::data::TYPE_INTEGER_SYMBOL && !scalar)
          {
            if (vsize > 0) {
              v_integers.assign(vsize,
                                properties::data::DEFAULT_VALUE_INTEGER);
            }
          }

          if (type == properties::data::TYPE_REAL_SYMBOL && !scalar) {
            if (vsize > 0) {
              v_reals.assign(vsize,properties::data::DEFAULT_VALUE_REAL);
            }
          }

          if (type == properties::data::TYPE_STRING_SYMBOL && !scalar) {
            if (vsize > 0) {
              v_strings.assign(vsize,
                               properties::data::DEFAULT_VALUE_STRING);
            }
          }

          std::istringstream iss(property_value_str);
          
          if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
            if (scalar) {
              iss >> a_boolean;
              if (!iss) {
                std::ostringstream message;
                message << "datatools::properties::config::read_: "
                        << "Cannot read boolean value for key '" 
                        << prop_key 
                        << "' at line '" 
                        << line << "' !" ;
                throw std::logic_error(message.str());
              }
            } else {
              for (int i = 0; i < vsize; ++i) {
                bool b;
                iss >> b;
                if (!iss) {
                  std::ostringstream message;
                  message << "datatools::properties::config::read_: "
                          << "Cannot read vector boolean value for key '" 
                          << prop_key 
                          << "' at line '" 
                          << line << "' !" ;
                  throw std::logic_error(message.str());
                }
                v_booleans[i] = b;
              }
            }
          }

          if (type == properties::data::TYPE_INTEGER_SYMBOL) {
            if (scalar) {
              iss >> a_integer;
              if (!iss) {
                std::ostringstream message;
                message << "datatools::properties::config::read_: "
                        << "Cannot read integer value for key '" 
                        << prop_key 
                        << "' at line '" 
                        << line << "' !" ;
                throw std::logic_error(message.str());
              }
            } else {
              for (int i = 0; i < vsize; ++i) {
                int k;
                iss >> k;
                if (!iss) {
                  std::ostringstream message;
                  message << "datatools::properties::config::read_: "
                          << "Cannot read vector integer value for key '" 
                          << prop_key 
                          << "' at line '" 
                          << line << "' !" ;
                  throw std::logic_error(message.str());
                }
                v_integers[i] = k;
              }
            }
          }

          bool with_explicit_unit = false;
          if (type == properties::data::TYPE_REAL_SYMBOL) {
            if (scalar) {
              if(enable_real_with_unit)
                {
                  //std::cerr << "DEVEL: enable_real_with_unit/scalar real" << std::endl;
                  // Special mode to accept unit symbol after the real value :
                  std::string real_word;
                  iss >> real_word >> std::ws;
                  if (!iss) {
                    std::ostringstream message;
                    message << "datatools::properties::config::read_: "
                            << "Cannot read value token for key '" 
                            << prop_key 
                            << "' at line '" 
                            << line 
                            << "' !" ;
                    throw std::logic_error(message.str());
                  }
                  //std::cerr << "DEVEL: enable_real_with_unit/scalar: real_word=" << real_word << std::endl;
                  std::istringstream iss2(real_word);
                  iss2 >> a_real >> std::ws;
                  if (!iss2) {
                    std::ostringstream message;
                    message << "datatools::properties::config::read_: "
                            << "Cannot read real value for key '" 
                            << prop_key 
                            << "' at line '" 
                            << line 
                            << "' !" ;
                    throw std::logic_error(message.str());
                  }
                  if (! iss.eof())
                    {
                      //std::cerr << "DEVEL: enable_real_with_unit/scalar: !EOF " << std::endl;
                      std::string unit_word;
                      iss >> unit_word >> std::ws;
                      //std::cerr << "DEVEL: enable_real_with_unit/scalar: unit_word=" << unit_word << std::endl; 
                      if (!unit_word.empty() && unit_word[0] != _comment_char_)
                        {
                          if (!iss) {
                            std::ostringstream message;
                            message << "datatools::properties::config::read_: "
                                    << "Cannot read unit token for key '" 
                                    << prop_key 
                                    << "' at line '" 
                                    << line 
                                    << "' !" ;
                            throw std::logic_error(message.str());
                          }    
                          double      unit_value;
                          std::string unit_label;
                          bool found_unit = datatools::units::find_unit(unit_word, unit_value, unit_label); 
                          if (! found_unit)
                            {
                              std::ostringstream message;
                              message << "datatools::properties::config::read_: "
                                      << "Invalid unit symbol for key '" 
                                      << prop_key 
                                      << "' at line '" 
                                  << line 
                                      << "' !" ;
                              throw std::logic_error(message.str());
                            }
                          if(!requested_unit_label.empty() && unit_label != requested_unit_label)
                            {
                              std::ostringstream message;
                              message << "datatools::properties::config::read_: "
                                      << "Unit symbol '" << unit_word << "' in not compatible with requested unit label '"
                                      << requested_unit_label << "' for real property with key '" 
                                      << prop_key 
                                      << "' at line '" 
                                      << line 
                                      << "' !" ;
                              throw std::logic_error(message.str());                          
                            }
                          //std::cerr << "DEVEL: enable_real_with_unit/scalar: unit_value=" << unit_value << std::endl;
                          a_real *= unit_value;
                          with_explicit_unit = true;
                        }
                    }
                }
              else {
                // Standard mode with only the plain real value (no trailing unit)
                iss >> a_real >> std::ws;
                if (!iss) {
                  std::ostringstream message;
                  message << "datatools::properties::config::read_: "
                          << "Cannot read real value for key '" 
                          << prop_key 
                          << "' at line '" 
                          << line 
                          << "' !" ;
                  throw std::logic_error(message.str());
                }
                std::string dummy;
                iss >> dummy;
                if (!dummy.empty() && dummy[0] != _comment_char_ )
                  {
                    std::ostringstream message;
                    message << "datatools::properties::config::read_: "
                            << "Trailing token '" << dummy << "' is not allowed for real value with key '" 
                            << prop_key 
                            << "' at line '" 
                            << line 
                            << "' !" ;
                    throw std::logic_error(message.str());
                  }
              }
              //std::cerr << "DEVEL: real/scalar: a_real=" << a_real << std::endl;
            } else {
              for (int i = 0; i < vsize; ++i) {
                double x;
                iss >> x;
                if (!iss) {
                  std::ostringstream message;
                  message << "datatools::properties::config::read_: "
                          << "Cannot read vector real value for key '" 
                          << prop_key 
                          << "' at line '" 
                          << line << "' !" ;
                  throw std::logic_error(message.str());
                }
                //std::string requested_unit_label;
                //std::string requested_unit_symbol;
                //double      requested_unit;
                if (enable_real_with_unit && datatools::is_valid(requested_unit))
                  {
                    x *= requested_unit;
                    with_explicit_unit = true;
                  }
                v_reals[i] = x; 
              }
            }
          }

          if (type == properties::data::TYPE_STRING_SYMBOL) {
            if (scalar) {
              if (!read_quoted_string(iss, a_string)) {
                std::ostringstream message;
                message << "datatools::properties::config::read_: "
                        << "Cannot read string value for key '" 
                        << prop_key 
                        << "' at line '" 
                        << line << "' !" ;
                throw std::logic_error(message.str());
              }
            } else {
              for (int i = 0; i < vsize; ++i) {
                std::string str;
                if (!read_quoted_string(iss, str)) {
                  std::ostringstream message;
                  message << "datatools::properties::config::read_: "
                          << "Cannot read string value for key '" 
                          << prop_key 
                          << "' at line '" 
                          << line << "' !" ;
                  throw std::logic_error(message.str());
                }
                v_strings[i] = str;
              }
            }
          }


          if (type == properties::data::TYPE_BOOLEAN_SYMBOL && scalar) {
            a_props.store(prop_key, a_boolean, prop_description, locked);
          }
          if (type == properties::data::TYPE_INTEGER_SYMBOL && scalar) {
            a_props.store(prop_key, a_integer, prop_description, locked);
          }
          if (type == properties::data::TYPE_REAL_SYMBOL && scalar) {
            a_props.store(prop_key, a_real, prop_description, locked);
          }
          if (type == properties::data::TYPE_STRING_SYMBOL && scalar) {
            a_props.store(prop_key, a_string, prop_description, locked);
          }

          if (type == properties::data::TYPE_BOOLEAN_SYMBOL && !scalar) {
            a_props.store(prop_key, v_booleans, prop_description, locked);
          }
          if (type == properties::data::TYPE_INTEGER_SYMBOL && !scalar) {
            a_props.store(prop_key, v_integers, prop_description, locked);
          }
          if (type == properties::data::TYPE_REAL_SYMBOL && !scalar) {
            a_props.store(prop_key, v_reals, prop_description, locked);
          }
          if (type == properties::data::TYPE_STRING_SYMBOL && !scalar) {
            a_props.store(prop_key, v_strings, prop_description, locked);
          }
          if (type == properties::data::TYPE_REAL_SYMBOL && with_explicit_unit) {
            a_props.set_explicit_unit(prop_key, true);
          }
          prop_description = "";
        }
      } // !skip_line
    } // if (! line_goon)
  } // while (*_in)
  return;
}


bool properties::config::read_quoted_string(std::istream& a_in,
                                            std::string& a_str) {
  std::string chain = "";
  a_in >> std::ws;
  if (!a_in) {
    a_str = chain;
    return true;
  }

  bool quoted = false;
  char lastc = 0;
  do {
    char c = 0;
    a_in.get(c);
    if (! a_in) break;
    if (c == '"') {
      if (quoted) {
        quoted=false;
        break;
      } else {
        if (lastc == 0) {
          quoted = true;
          continue;
        } else {
          chain += c;
          lastc  = c;
        }
      }
    } else {
      if (isspace(c)) {
        if (!quoted) {
          a_in.putback(c);
          break;
        }
      }
      chain += c;
      lastc = c;
    }

  } while(a_in);

  if (quoted) {
    return false;
  }

  a_str = chain;
  return true;
}


std::string properties::build_property_key(const std::string& prefix,
                                           const std::string& subkey) {
  std::string s(prefix);
  s += '.';
  s += subkey;
  return s;
}


void properties::export_to_string_based_dictionary(
    std::map<std::string, std::string>& dict, bool quoted_strings) const {
  for (pmap::const_iterator i = _props_.begin();
       i != _props_.end();
       ++i) {
    const std::string& prop_key = i->first;
    std::ostringstream valoss;
    const data& data = i->second;
    
    if (data.is_vector()) valoss << '(';
    
    for (int i = 0; i < (int)data.get_size(); ++i) {
      if (i != 0) valoss << ',';
      if (data.is_boolean()) valoss << data.get_boolean_value(i);
      if (data.is_integer()) valoss << data.get_integer_value(i);
      if (data.is_real()) {
        valoss.precision(15);
        valoss << data.get_real_value(i);
      }
      if (data.is_string()) {
        if (quoted_strings) valoss << '"';
        valoss << data.get_string_value(i);
        if (quoted_strings) valoss << '"';
      }
    }
    if (data.is_vector()) valoss << ')';
    
    dict[prop_key] = valoss.str();
  }
}

// Specialization :
template <>
bool check_serial_tag<properties>(const std::string stag_, 
                                  const std::string alt_tag_,  
                                  boost::enable_if< has_bsts<properties> >::type* dummy) {
  if (stag_ == properties::SERIAL_TAG) return true;
  if (stag_ == ::datatools::backward_serial_tag<properties> (0)) return true;
  if (stag_ == "datatools:utils::properties") return true;
  return false;
}


} // end of namespace datatools

