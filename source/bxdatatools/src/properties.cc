// datatools/properties.cc

// Ourselves:
#include <datatools/properties.h>

// Standard Library:
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <list>
#include <memory>

// Third Party:
// - Boost:
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

// This Project:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/logger.h>
#include <datatools/properties_config.h>

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(::datatools::properties,
                                                      "datatools::properties")

// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(::datatools::properties,
                                                               "datatools::utils::properties")

namespace {
  const char kAllowedChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";
}

namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(properties,"datatools::properties")

  //----------------------------------------------------------------------
  // properties::data class implementation
  //
  std::string properties::data::get_error_message(int error_code_)
  {
    if (error_code_ == ERROR_SUCCESS) return "success";
    if (error_code_ == ERROR_FAILURE) return "generic error";
    if (error_code_ == ERROR_BADTYPE) return "invalid type";
    if (error_code_ == ERROR_RANGE ) return "invalid index";
    if (error_code_ == ERROR_LOCK  ) return "lock error";
    return "undocumented error";
  }

  bool properties::data::has_forbidden_char(const std::string & str_)
  {
    return str_.find_first_of(STRING_FORBIDDEN_CHAR) != str_.npos;
  }

  bool properties::data::defaults::boolean_value()
  {
    return false;
  }

  int properties::data::defaults::integer_value()
  {
    return 0;
  }

  double properties::data::defaults::real_value()
  {
    return 0.0;
  }

  std::string properties::data::defaults::string_value()
  {
    return std::string{};
  }

  const std::string & properties::data::format::boolean_label()
  {
    static const std::string _lbl("boolean");
    return _lbl;
  }

  const std::string & properties::data::format::integer_label()
  {
    static const std::string _lbl("integer");
    return _lbl;
  }

  const std::string & properties::data::format::real_label()
  {
    static const std::string _lbl("real");
    return _lbl;
  }

  const std::string & properties::data::format::string_label()
  {
    static const std::string _lbl("string");
    return _lbl;
  }


  properties::data::data(char type_, size_t size_)
  {
    _flags_  = 0;
    this->init_values_(type_, size_);
    return;
  }

  properties::data::data(bool value_, size_t size_)
      : properties::data::data(TYPE_BOOLEAN_SYMBOL, size_)
  {
    for (size_t i{0}; i < this->size(); ++i) {
      this->set_value(value_, i);
    }
    return;
  }

  properties::data::data(int value_, size_t size_)
      : properties::data::data(TYPE_INTEGER_SYMBOL, size_)
  {
    for (size_t i{0}; i < this->size(); ++i) {
      this->set_value(value_, i);
    }
    return;
  }

  properties::data::data(double value_, size_t size_)
      : properties::data::data(TYPE_REAL_SYMBOL, size_)
  {
    for (size_t i{0}; i < this->size(); ++i) {
      this->set_value(value_, i);
    }
    return;
  }

  properties::data::data(const std::string & value_, size_t size_)
      : properties::data::data(TYPE_STRING_SYMBOL, size_)
  {
    DT_THROW_IF(has_forbidden_char(value_),
                std::logic_error,
                "Forbidden char in string '" << value_ << "'!");
    int code=0;
    for (size_t i{0}; i < this->size(); ++i) {
      code = this->set_value(value_, i);
      DT_THROW_IF(code != ERROR_SUCCESS,
                  std::logic_error,
                  "Failure!");
    }
    return;
  }

  properties::data::data(const char* value_, size_t size_)
      : properties::data::data(TYPE_STRING_SYMBOL, size_)
  {
    std::string tmp;
    if (value_ != nullptr) {
      tmp = value_;
    }
    DT_THROW_IF(has_forbidden_char(tmp),
                std::logic_error,
                "Forbidden char in string '" << tmp << "'!");
    for (int i = 0; i < (int)this->size(); ++i) {
      this->set_value(tmp, i);
    }
    return;
  }

  int properties::data::init_values_(char type_, size_t size_)
  {
    size_t memsize = size_;
    if (size_ == SCALAR_DEF) {
      memsize = 1;
      _flags_ &= ~MASK_VECTOR; // force scalar
    } else {
      _flags_ |= MASK_VECTOR; // force vector
    }

    _flags_ &= ~MASK_TYPE;
    if (type_ == TYPE_BOOLEAN_SYMBOL) {
      _flags_ |= TYPE_BOOLEAN;
      if (memsize > 0) _boolean_values_.assign(memsize, defaults::boolean_value());
    }
    if (type_ == TYPE_INTEGER_SYMBOL) {
      _flags_ |= TYPE_INTEGER;
      if (memsize > 0) _integer_values_.assign(memsize, defaults::integer_value());
    }
    if (type_ == TYPE_REAL_SYMBOL) {
      _flags_ |= TYPE_REAL;
      if (memsize > 0) _real_values_.assign(memsize, defaults::real_value());
    }
    if (type_ == TYPE_STRING_SYMBOL) {
      _flags_ |= TYPE_STRING;
      if (memsize > 0) _string_values_.assign(memsize, defaults::string_value());
    }

    return ERROR_SUCCESS;
  }


  void properties::data::clear_values_()
  {
    _boolean_values_.clear();
    _integer_values_.clear();
    _real_values_.clear();
    _string_values_.clear();
    return;
  }

  bool properties::data::has_description() const
  {
    return !_description_.empty();
  }

  void properties::data::set_description(const std::string & description_)
  {
    _description_ = description_;
    return;
  }

  const std::string & properties::data::get_description() const
  {
    return _description_;
  }

  void properties::data::clear_unit_symbol_()
  {
    _flags_ &= ~MASK_UNIT_SYMBOL;
    _unit_symbol_.clear();
    return;
  }

  bool properties::data::has_unit_symbol() const
  {
    return (_flags_ & MASK_UNIT_SYMBOL) != 0;
  }

  int properties::data::set_unit_symbol(const std::string & a_symbol)
  {
    if (!this->is_real()) return ERROR_BADTYPE;
    if (a_symbol.empty()) {
      clear_unit_symbol_();
    } else {
      _flags_ |= MASK_UNIT_SYMBOL;
      _unit_symbol_ = a_symbol;
    }
    return ERROR_SUCCESS;
  }

  const std::string & properties::data::get_unit_symbol() const
  {
    return _unit_symbol_;
  }

  int properties::data::boolean(size_t size_)
  {
    this->clear_values_();
    return this->init_values_(TYPE_BOOLEAN_SYMBOL, size_);
  }

  int properties::data::integer(size_t size_)
  {
    this->clear_values_();
    return this->init_values_(TYPE_INTEGER_SYMBOL, size_);
  }

  int properties::data::real(size_t size_)
  {
    this->clear_values_();
    return this->init_values_(TYPE_REAL_SYMBOL, size_);
  }

  int properties::data::string(size_t size_)
  {
    this->clear_values_();
    return this->init_values_(TYPE_STRING_SYMBOL, size_);
  }

  int properties::data::get_type() const
  {
    return (int) (_flags_ & MASK_TYPE);
  }

  bool properties::data::is_boolean() const
  {
    return (_flags_ & MASK_TYPE) == TYPE_BOOLEAN;
  }

  bool properties::data::is_integer() const
  {
    return (_flags_ & MASK_TYPE) == TYPE_INTEGER;
  }

  bool properties::data::is_real() const
  {
    return (_flags_ & MASK_TYPE) == TYPE_REAL;
  }

  bool properties::data::is_path() const
  {
    return is_string() && ((_flags_ & MASK_EXPLICIT_PATH) != 0);
  }

  bool properties::data::is_string() const
  {
    return (_flags_ & MASK_TYPE) == TYPE_STRING;
  }

  bool properties::data::is_scalar() const
  {
    return (_flags_ & MASK_VECTOR) == 0;
  }

  bool properties::data::is_vector() const
  {
    return !this->is_scalar();
  }

  bool properties::data::has_explicit_unit() const
  {
    return (_flags_ & MASK_EXPLICIT_UNIT) != 0;
  }

  bool properties::data::is_explicit_path() const
  {
    return (_flags_ & MASK_EXPLICIT_PATH) != 0;
  }

  bool properties::data::is_locked() const
  {
    return (_flags_ & MASK_LOCK) != 0;
  }

  int properties::data::lock()
  {
    _flags_ |= MASK_LOCK;
    return ERROR_SUCCESS;
  }

  int properties::data::unlock()
  {
    _flags_ &= ~MASK_LOCK;
    return ERROR_SUCCESS;
  }

  size_t properties::data::get_size() const
  {
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

  size_t properties::data::size() const
  {
    return this->get_size();
  }

  bool properties::data::empty() const
  {
    return (this->size() == 0);
  }

  bool properties::data::index_is_valid(size_t a_index) const
  {
    return a_index < this->get_size();
  }

  int properties::data::set_value(bool value_, size_t a_index)
  {
    if (!this->is_boolean()) return ERROR_BADTYPE;

    if (this->is_locked()) return ERROR_LOCK;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    _boolean_values_[a_index] = value_;
    return ERROR_SUCCESS;
  }

  int properties::data::set_value(int value_, size_t a_index)
  {
    if (!this->is_integer()) return ERROR_BADTYPE;

    if (this->is_locked()) return ERROR_LOCK;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    _integer_values_[a_index] = value_;
    return ERROR_SUCCESS;
  }

  int properties::data::set_value(double value_, size_t a_index, bool a_explicit_unit)
  {
    if (!this->is_real()) return ERROR_BADTYPE;

    if (this->is_locked()) return ERROR_LOCK;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;
    if (!has_explicit_unit()) {
      if (a_explicit_unit) {
        set_explicit_unit(a_explicit_unit);
      }
    }
    _real_values_[a_index] = value_;
    return ERROR_SUCCESS;
  }

  int properties::data::set_value_with_unit(double value_, size_t a_index, const std::string & a_unit_symbol)
  {
    set_unit_symbol(a_unit_symbol);
    return set_value(value_, a_index, !a_unit_symbol.empty());
  }

  int properties::data::set_explicit_path(bool xp_)
  {
    if (! xp_) {
      _flags_ &= ~MASK_EXPLICIT_PATH;
    } else {
      _flags_ |= MASK_EXPLICIT_PATH; // Force explicit path
    }
    return ERROR_SUCCESS;
  }

  int properties::data::set_explicit_unit(bool xu_)
  {
    DT_THROW_IF(!this->is_real(),
                std::logic_error,
                "Not a real type!");
    if (! xu_) {
      _flags_ &= ~MASK_EXPLICIT_UNIT;
    } else {
      _flags_ |= MASK_EXPLICIT_UNIT;  // Force explicit unit
    }
    return ERROR_SUCCESS;
  }

  int properties::data::set_value(const std::string & value_,
                                  size_t a_index,
                                  bool a_explicit_path)
  {
    if (!this->is_string()) return ERROR_BADTYPE;
    if (this->is_locked()) return ERROR_LOCK;
    if (!this->index_is_valid(a_index)) return ERROR_RANGE;
    /* special trick to forbid character '\"' in string as
     * it is used as separator for parsing:
     */
    if (this->has_forbidden_char(value_)) return ERROR_FAILURE;
    _string_values_[a_index] = value_;
    if (!is_explicit_path()) {
      if (a_explicit_path) {
        set_explicit_path(a_explicit_path);
      }
    }
    return ERROR_SUCCESS;
  }

  int properties::data::set_value(const char* value_,
                                  size_t a_index,
                                  bool a_explicit_path)
  {
    return this->set_value(std::string(value_), a_index, a_explicit_path);
  }

  int properties::data::get_value(bool& value_, size_t a_index) const
  {
    if (!this->is_boolean()) return ERROR_BADTYPE;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    value_ = _boolean_values_[a_index];
    return ERROR_SUCCESS;
  }

  int properties::data::get_value(int& value_, size_t a_index) const
  {
    if (!this->is_integer()) return ERROR_BADTYPE;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    value_ = _integer_values_[a_index];
    return ERROR_SUCCESS;
  }

  int properties::data::get_value(double& value_, size_t a_index) const
  {
    if (!this->is_real()) return ERROR_BADTYPE;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    value_ = _real_values_[a_index];
    return ERROR_SUCCESS;
  }

  int properties::data::get_value(std::string & value_, size_t a_index) const
  {
    if (!this->is_string()) return ERROR_BADTYPE;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    value_ = _string_values_[a_index];
    return ERROR_SUCCESS;
  }

  bool properties::data::get_boolean_value(size_t a_index) const
  {
    bool value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not boolean!");
    return value;
  }

  int properties::data::get_integer_value(size_t a_index) const
  {
    int value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not integer!");
    return value;
  }

  double properties::data::get_real_value(size_t a_index) const
  {
    double value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not real!");
    return value;
  }

  std::string properties::data::get_string_value(size_t a_index) const
  {
    std::string value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not string!");
    return value;
  }

  std::string properties::data::get_type_label() const
  {
    if (this->is_boolean()) return format::boolean_label();
    if (this->is_integer()) return format::integer_label();
    if (this->is_real())    return format::real_label();
    if (this->is_string())  return format::string_label();
    return "none";
  }

  std::string properties::data::get_vector_label() const
  {
    if (this->is_scalar()) return "scalar";
    return "vector";
  }

  void properties::data::dump(std::ostream& a_out) const
  {
    this->tree_dump(a_out, "datatools::properties::data");
    return;
  }

  void properties::data::to_string(std::ostream& a_out) const
  {
    a_out << '[';
    a_out << this->get_type_label() << ':'
          << this->get_vector_label() << ":"
          << this->get_size() << ':';
    for (size_t i{0} ; i < this->get_size(); ++i) {
      if (i != 0) a_out << ' ';
      if (this->is_boolean()) a_out << this->get_boolean_value(i);
      if (this->is_integer()) a_out << this->get_integer_value(i);
      if (this->is_real())    a_out << this->get_real_value(i);
      if (this->is_string())  a_out << this->get_string_value(i);
    }
    a_out << ':' << this->get_description();
    if (has_unit_symbol()) {
      a_out << ':' << this->get_unit_symbol();
    }
    a_out << ']';
    return;
  }

  void properties::data::tree_dump(std::ostream& a_out,
                                   const std::string & a_title,
                                   const std::string & a_indent,
                                   bool a_inherit) const
  {
    std::string indent{a_indent};

    if (! a_title.empty()) a_out << indent << a_title << std::endl;

    if (!_description_.empty()) {
      a_out << indent << i_tree_dumpable::tag
            << "Description  : '" << this->get_description() << "'" << std::endl;
    }

    double unit_value = 1.0;
    std::string unit_symbol;
    std::string unit_label;

    a_out << indent << i_tree_dumpable::tag
          << "Type  : " ;
    if (this->is_locked()) a_out << "const ";
    a_out << this->get_type_label();

    if (this->is_vector()) {
      a_out << '[' << this->size() << ']' << " (vector)";
    } else {
      a_out << " (scalar)";
    }

    if (this->is_real() && has_explicit_unit()) {
      a_out << " [explicit unit";
      if (has_unit_symbol()) {
        unit_symbol = get_unit_symbol();
        a_out << "='" << unit_symbol << "'";
        if (!units::find_unit(unit_symbol, unit_value, unit_label)) {
          a_out << " <invalid unit symbol>";
          unit_value = 1.0;
        }
      }
      a_out << "]";
    }

    if (this->is_string() && is_explicit_path()) {
      a_out << " [explicit path]";
    }
    a_out << std::endl;

    if (this->get_size() > 0) {
      for (size_t i{0}; i < this->get_size(); ++i) {
        a_out << indent;
        if (i == (this->get_size() - 1)) {
          a_out << i_tree_dumpable::inherit_tag (a_inherit);
        } else {
          a_out << i_tree_dumpable::tag;
        }
        a_out << "Value";
        if (this->is_vector()) a_out << "[" << i << "]";

        a_out << " : ";
        if (this->is_boolean()) {
          a_out << std::boolalpha << this->get_boolean_value(i) << std::endl;
        }
        if (this->is_integer()) {
          a_out << std::dec << this->get_integer_value(i) << std::endl;
        }
        if (this->is_real()) {
          double val = this->get_real_value(i);
          val /= unit_value;
          datatools::io::write_real_number(a_out, val, datatools::io::REAL_PRECISION);
          if (! unit_symbol.empty()) {
            a_out << " " << unit_symbol;
          }
          a_out << std::endl;
        }
        if (this->is_string()) {
          a_out << "'" << this->get_string_value(i) << "'" << std::endl;
        }
      }
    } else {
      a_out << indent;
      a_out << i_tree_dumpable::inherit_tag(a_inherit);
      a_out << "<no value>" << std::endl;
    }
    return;
  }

  //----------------------------------------------------------------------
  // properties class implementation
  //
  properties::properties(const std::string & description_)
  {
    this->set_description(description_);
    return;
  }

  const std::string & properties::private_property_prefix()
  {
    static std::string prefix{"__"};
    return prefix;
  }

  void properties::_validate_key_(const std::string & prop_key_) const
  {
    if (prop_key_.empty()) {
      DT_THROW(std::logic_error, "properties key must not be empty");
    }
    if (prop_key_.find_first_not_of(kAllowedChars) != std::string::npos) {
      DT_THROW(std::logic_error, "properties key must only contain alphanumeric chars, underscores, and periods");
    }
    if ((std::isdigit(static_cast<unsigned char>(prop_key_.front())) != 0) || prop_key_.front() == '.') {
      DT_THROW(std::logic_error, "properties key must not start with a digit or period");
    }
    if (prop_key_.back() == '.') {
      DT_THROW(std::logic_error, "properties key must not end with '.'");
    }
    return;
  }

  void properties::set_description(const std::string & description_)
  {
    _description_ = description_;
    return;
  }

  const std::string & properties::get_description() const
  {
    return _description_;
  }

  bool properties::fetch_short_description(std::string & short_desc_) const
  {
    short_desc_.clear();
    std::vector<std::string> desc_parts;
    boost::split(desc_parts, _description_, boost::is_any_of(";"));
    if (!desc_parts.empty()) {
      short_desc_ = desc_parts.front();
      boost::trim(short_desc_);
    }
    return !short_desc_.empty();
  }

  bool properties::has_short_description() const
  {
    std::string tmp;
    return fetch_short_description(tmp);
  }

  std::string properties::get_short_description() const
  {
    std::string tmp;
    fetch_short_description(tmp);
    return tmp;
  }

  bool properties::has_auxiliary_descriptions() const
  {
    std::vector<std::string> aux;
    return fetch_auxiliary_descriptions(aux);
  }

  bool properties::fetch_auxiliary_descriptions(std::vector<std::string> & aux_) const
  {
    std::list<std::string> desc_parts;
    boost::split(desc_parts, _description_, boost::is_any_of(";"));
    desc_parts.pop_front();

    aux_.clear();
    for (std::string& s : desc_parts) {
      boost::trim(s);
      if (!s.empty()) {
        aux_.push_back(s);
      }
    }
    return !aux_.empty();
  }

  size_t properties::size() const
  {
    return _props_.size();
  }

  bool properties::empty() const
  {
    return _props_.empty();
  }

  DATATOOLS_CLONEABLE_IMPLEMENTATION(properties)

  void properties::erase(const std::string & prop_key_)
  {
    auto found = _props_.find(prop_key_);
    DT_THROW_IF(found == _props_.end(),
                std::logic_error,
                "Key '" << prop_key_ << "' not known!");
    _props_.erase(found);
    return;
  }

  void properties::clean(const std::string & a_key)
  {
    auto found = _props_.find(a_key);
    if (found != _props_.end()) {
      _props_.erase(found);
    }
    return;
  }

  void properties::erase_all()
  {
    this->clear();
    return;
  }

  void properties::clear()
  {
    _props_.clear();
    return;
  }

  void properties::reset()
  {
    this->set_description("");
    this->clear();
    return;
  }


  void properties::erase_all_starting_with(const std::string & prefix_)
  {
    for (const auto& k : this->keys_starting_with(prefix_)) {
      this->erase(k);
    }
    return;
  }

  void properties::erase_all_not_starting_with(const std::string & prefix_)
  {
    for (const auto& k : this->keys_not_starting_with(prefix_)) {
      this->erase(k);
    }
    return;
  }

  void properties::erase_all_ending_with(const std::string & suffix)
  {
    for (const auto& k : this->keys_ending_with(suffix)) {
      this->erase(k);
    }
    return;
  }

  void properties::erase_all_not_ending_with(const std::string & suffix)
  {
    for (const auto& k : this->keys_not_ending_with(suffix)) {
      this->erase(k);
    }
    return;
  }


  void properties::export_and_rename_starting_with(properties & props_,
                                                   const std::string & prop_key_prefix_,
                                                   const std::string & a_new_prefix) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");

    for (const auto& k : this->keys_starting_with(prop_key_prefix_)) {
      auto new_key = boost::replace_first_copy(k, prop_key_prefix_, a_new_prefix);
      props_._props_[new_key] = _props_.at(k);
    }
    return;
  }

  void properties::export_all(properties & props_) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    props_._props_.insert(_props_.begin(), _props_.end());
    return;
  }

  void properties::export_all_adding_prefix(properties & props_,
                                            const std::string & prefix_) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");

    for (const auto& pair : _props_) {
      props_._props_[prefix_ + pair.first] = pair.second;
    }
    return;
  }

  void properties::export_starting_with(properties & props_,
                                        const std::string & prefix_) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    for (const auto& k : this->keys_starting_with(prefix_)) {
      props_._props_[k] = _props_.at(k);
    }
    return;
  }

  void properties::export_not_starting_with(properties & props_,
                                            const std::string & prefix_) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    for (const auto& k : this->keys_not_starting_with(prefix_)) {
      props_._props_[k] = _props_.at(k);
    }
    return;
  }

  void properties::export_ending_with(properties & props_,
                                      const std::string & suffix) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    for (const auto& k : this->keys_ending_with(suffix)) {
      props_._props_[k] = _props_.at(k);
    }
    return;
  }

  void properties::export_not_ending_with(properties & props_,
                                          const std::string & suffix) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    for (const auto& k : this->keys_not_ending_with(suffix)) {
      props_._props_[k] = _props_.at(k);
    }
    return;
  }


  void properties::keys_not_starting_with(std::vector<std::string>& some_keys,
                                          const std::string & prefix_) const
  {
    DT_THROW_IF(prefix_.empty(),
                std::logic_error,
                "Empty key prefix argument !");
    for (const auto& pair : _props_) {
      if (! boost::algorithm::starts_with(pair.first, prefix_)) {
        some_keys.push_back(pair.first);
      }
    }

    return;
  }


  std::vector<std::string>
  properties::keys_not_starting_with(const std::string & prefix_) const
  {
    std::vector<std::string> lkeys;
    this->keys_not_starting_with(lkeys, prefix_);
    return lkeys;
  }

  void properties::keys_starting_with(std::vector<std::string>& some_keys,
                                      const std::string & prefix_) const
  {
    DT_THROW_IF(prefix_.empty(),
                std::logic_error,
                "Empty key prefix argument !");

    for (const auto& pair : _props_) {
      if (boost::algorithm::starts_with(pair.first, prefix_)) {
        some_keys.push_back(pair.first);
      }
    }
    return;
  }

  std::vector<std::string>
  properties::keys_starting_with(const std::string & prefix_) const
  {
    std::vector<std::string> lkeys;
    this->keys_starting_with(lkeys, prefix_);
    return lkeys;
  }

  void properties::keys_not_ending_with(std::vector<std::string>& prop_keys,
                                        const std::string & suffix) const
  {
    DT_THROW_IF(suffix.empty(),
                std::logic_error,
                "Empty key suffix argument !");
    for (const auto& pair : _props_) {
      if (! boost::algorithm::ends_with(pair.first, suffix)) {
        prop_keys.push_back(pair.first);
      }
    }

    return;
  }

  std::vector<std::string> properties::keys_not_ending_with(const std::string & suffix) const
  {
    std::vector<std::string> lkeys;
    this->keys_not_ending_with(lkeys, suffix);
    return lkeys;
  }

  void properties::keys_ending_with(std::vector<std::string>& prop_keys,
                                    const std::string & suffix) const
  {
    DT_THROW_IF(suffix.empty(),
                std::logic_error,
                "Empty key suffix argument !");
    for (const auto& pair : _props_) {
      if (boost::algorithm::ends_with(pair.first, suffix)) {
        prop_keys.push_back(pair.first);
      }
    }
    return;
  }

  std::vector<std::string> properties::keys_ending_with(const std::string & suffix_) const
  {
    std::vector<std::string> lkeys;
    this->keys_ending_with(lkeys, suffix_);
    return lkeys;
  }

  std::set<std::string> properties::get_keys() const
  {
    std::set<std::string> skeys;
    for(const std::string& k : this->keys()) {
      skeys.insert(k);
    }
    return skeys;
  }

  std::vector<std::string> properties::keys() const
  {
    std::vector<std::string> lkeys;
    this->keys(lkeys);
    return lkeys;
  }

  void properties::keys(std::vector<std::string> & some_keys_) const
  {
    for(const auto& pair : _props_) {
      some_keys_.push_back(pair.first);
    }
    return;
  }

  const std::string & properties::key(size_t key_index_) const
  {
    DT_THROW_IF(key_index_ >= this->size(),
                std::out_of_range,
                "Out of range key index '" << key_index_ << "' in properties described by '" << get_description() << "' !");
    auto start = _props_.begin();
    std::advance(start, key_index_);
    return start->first;
  }

  void properties::lock(const std::string & a_key)
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    data_ptr.lock();
    return;
  }

  void properties::unlock(const std::string & a_key)
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    data_ptr.unlock();
    return;
  }

  bool properties::is_locked(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.is_locked();
  }

  std::string properties::make_private_key(const std::string & a_key)
  {
    if (properties::key_is_private(a_key)) return a_key;

    std::ostringstream oss;
    oss << private_property_prefix() << a_key;
    return oss.str();
  }

  bool properties::key_is_private(const std::string & a_key)
  {
    return boost::algorithm::starts_with(a_key, private_property_prefix());
  }

  bool properties::key_is_public(const std::string & a_key)
  {
    return !properties::key_is_private(a_key);
  }

  bool properties::is_private(const std::string & a_key) const
  {
    // Assert that the instance *must* contain this key
    this->_get_valid_data_(a_key);
    return this->key_is_private(a_key);
  }

  bool properties::is_public(const std::string & a_key) const
  {
    return !this->is_private(a_key);
  }

  int properties::get_type(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.get_type();
  }

  std::string properties::get_type_label(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.get_type_label();
  }

  bool properties::is_boolean(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.is_boolean();
  }

  bool properties::is_integer(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.is_integer();
  }

  bool properties::is_real(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.is_real();
  }

  bool properties::is_string(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.is_string();
  }

  bool properties::is_scalar(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.is_scalar();
  }

  bool properties::is_vector(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.is_vector();
  }

  size_t properties::size(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.size();
  }

  size_t properties::key_size(const std::string & a_key) const
  {
    return this->size(a_key);
  }

  bool properties::has_key(const std::string & a_key) const
  {
    if (_props_.size() == 0) return false;
    return _props_.find(a_key) != _props_.end();
  }

  const std::string &
  properties::get_key_description(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    return data_ptr.get_description();
  }

  void properties::set_key_description(const std::string & a_key,
                                       const std::string & desc_)
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    data_ptr.set_description(desc_);
    return;
  }

  void properties::_check_nokey_(const std::string & a_key) const
  {
    DT_THROW_IF(this->has_key(a_key),
                std::logic_error,
                "Key '" << a_key << "' already used in properties described by '" << get_description() << "' !");
    return;
  }

  properties::data& properties::_get_valid_data_(const std::string& a_key) {
    auto iter = _props_.find(a_key);
    DT_THROW_IF(iter == _props_.end(),
                std::logic_error,
                "Key '" << a_key << "' does not exist in properties described by '" << get_description() << "' !");
    return iter->second;
  }

  const properties::data& properties::_get_valid_data_(const std::string& a_key) const {
    auto iter = _props_.find(a_key);
    DT_THROW_IF(iter == _props_.end(),
                std::logic_error,
                "Key '" << a_key << "' does not exist in properties described by '" << get_description() << "' !");
    return iter->second;
  }


  void properties::store(const std::string & prop_key, const data & value)
  {
    this->_check_nokey_(prop_key);
    this->_validate_key_(prop_key);
    _props_[prop_key] = value;
    return;
  }

  void properties::store(const std::string & a_key, bool value_,
                         const std::string & description_, bool a_lock)
  {
    this->_store_scalar_impl_(a_key, value_, description_, a_lock);
    return;
  }

  void properties::store_boolean(const std::string & a_key, bool value,
                                 const std::string & desc, bool a_lock)
  {
    this->store(a_key, value, desc, a_lock);
    return;
  }

  void properties::store_flag(const std::string & a_key,
                              const std::string & description_,
                              bool a_lock)
  {
    this->store(a_key, true, description_, a_lock);
    return;
  }

  void properties::set_flag(const std::string & a_key)
  {
    this->store(a_key, true, "", false);
    return;
  }

  void properties::store(const std::string & a_key, int value,
                         const std::string & description, bool a_lock)
  {
    this->_store_scalar_impl_(a_key, value, description, a_lock);
    return;
  }

  void properties::store_integer(const std::string & a_key, int value,
                                 const std::string & desc, bool a_lock)
  {
    this->store(a_key, value, desc, a_lock);
    return;
  }


  void properties::store(const std::string & a_key, double value,
                         const std::string & description, bool a_lock)
  {
    this->_store_scalar_impl_(a_key, value, description, a_lock);
    return;
  }

  void properties::store_real(const std::string & a_key, double value,
                              const std::string & desc, bool a_lock)
  {
    this->store(a_key, value, desc, a_lock);
    return;
  }

  void properties::store_real_with_explicit_unit(const std::string & a_key, double value,
                                                 const std::string & desc, bool a_lock)
  {
    this->store(a_key, value, desc, a_lock);
    this->set_explicit_unit(a_key, true);
    return;
  }

  void properties::store_with_explicit_unit(const std::string & a_key, double value,
                                            const std::string & desc, bool a_lock)
  {
    this->store_real_with_explicit_unit(a_key, value, desc, a_lock);
    return;
  }


  void properties::store(const std::string & a_key, const std::string & value,
                         const std::string & description, bool a_lock)
  {
    this->_store_scalar_impl_(a_key, value, description, a_lock);
    return;
  }

  void properties::store(const std::string & a_key, const char* value,
                         const std::string & description, bool a_lock)
  {
    this->store(a_key, std::string{value}, description, a_lock);
    return;
  }

  void properties::store_string(const std::string & a_key,
                                const std::string & value,
                                const std::string & desc,
                                bool a_lock)
  {
    this->store(a_key, value, desc, a_lock);
    return;
  }

  void properties::store_path(const std::string & a_key,
                              const std::string & path_value,
                              const std::string & desc,
                              bool a_lock)
  {
    this->store(a_key, path_value, desc, a_lock);
    this->set_explicit_path(a_key, true);
    return;
  }


  void properties::store(const std::string & a_key, const data::vbool& values,
                         const std::string & description, bool a_lock)
  {
    this->_store_vector_impl_(a_key, values, description, a_lock);
    return;
  }

  void properties::store(const std::string & a_key, const data::vint& values,
                         const std::string & description, bool a_lock)
  {
    this->_store_vector_impl_(a_key, values, description, a_lock);
    return;
  }

  void properties::store(const std::string & a_key, const data::vdouble& values,
                         const std::string & description, bool a_lock)
  {
    this->_store_vector_impl_(a_key, values, description, a_lock);
    return;
  }

  void properties::store(const std::string & a_key, const data::vstring& values,
                         const std::string & description, bool a_lock)
  {
    this->_store_vector_impl_(a_key, values, description, a_lock);
    return;
  }

  void properties::store_paths(const std::string & a_key, const data::vstring& a_path_values,
                               const std::string & a_desc, bool a_lock)
  {
    store(a_key, a_path_values, a_desc, a_lock);
    set_explicit_path(a_key, true);
    return;
  }


  bool properties::has_unit_symbol(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    DT_THROW_IF(! data_ptr.is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    return data_ptr.has_unit_symbol();
  }

  const std::string & properties::get_unit_symbol(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    DT_THROW_IF(! data_ptr.is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    return data_ptr.get_unit_symbol();
  }

  void properties::set_unit_symbol(const std::string & a_key, const std::string & unit_symbol)
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    DT_THROW_IF(! data_ptr.is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    DT_THROW_IF(data_ptr.set_unit_symbol(unit_symbol) != data::ERROR_SUCCESS,
                std::logic_error, "Setting unit symbol fails !");
    return;
  }

  bool properties::has_explicit_unit(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    DT_THROW_IF(! data_ptr.is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    return data_ptr.has_explicit_unit();
  }

  void properties::set_explicit_unit(const std::string & a_key, bool a_explicit_unit)
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    DT_THROW_IF(! data_ptr.is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    DT_THROW_IF(data_ptr.set_explicit_unit(a_explicit_unit) != data::ERROR_SUCCESS,
                std::logic_error,
                "Setting explicit unit fails for property '" << a_key
                << "' in properties described by '" << get_description() << "' !");
    return;
  }

  void properties::set_explicit_path(const std::string & a_key, bool a_explicit_path)
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    DT_THROW_IF(! data_ptr.is_string(),
                std::logic_error,
                "Property '" << a_key << "' is not of string type in properties described by '" << get_description() << "' !");
    DT_THROW_IF(data_ptr.set_explicit_path(a_explicit_path) != data::ERROR_SUCCESS,
                std::logic_error, "Setting explicit path fails !");
    return;
  }

  bool properties::is_explicit_path(const std::string & a_key) const
  {
    auto& data_ptr = this->_get_valid_data_(a_key);
    DT_THROW_IF(! data_ptr.is_string(),
                std::logic_error,
                "Property '" << a_key << "' is not of string type !");
    return data_ptr.is_explicit_path();
  }

  void properties::unset_flag(const std::string & a_key)
  {
    this->clean(a_key);
    return;
  }

  void properties::change(const std::string & a_key, bool value, size_t index)
  {
    this->_change_scalar_impl_(a_key, value, index);
    return;
  }

  void properties::change_boolean(const std::string & a_key, bool value,
                                  size_t index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change_boolean_scalar(const std::string & a_key, bool value)
  {
    this->change(a_key, value, 0);
    return;
  }

  void properties::change_boolean_vector(const std::string & a_key, bool value,
                                         size_t index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change(const std::string & a_key, int value, size_t index)
  {
    this->_change_scalar_impl_(a_key, value, index);
    return;
  }

  void properties::change_integer(const std::string & a_key, int value,
                                  size_t index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change_integer_scalar(const std::string & a_key, int value)
  {
    this->change(a_key, value, 0);
    return;
  }

  void properties::change_integer_vector(const std::string & a_key, int value,
                                         size_t index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change(const std::string & a_key, double value, size_t index)
  {
    this->_change_scalar_impl_(a_key, value, index);
    return;
  }

  void properties::change_real(const std::string & a_key, double value,
                               size_t index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change_real_scalar(const std::string & a_key, double value)
  {
    this->change(a_key, value, 0);
    return;
  }

  void properties::change_real_vector(const std::string & a_key, double value,
                                      size_t index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change(const std::string & a_key, const std::string & value,
                          size_t index)
  {
    this->_change_scalar_impl_(a_key, value, index);
    return;
  }

  void properties::change(const std::string & a_key, const char* value,
                          size_t index)
  {
    std::string tmp = (value != 0 ? value : "");
    this->change(a_key, tmp, index);
    return;
  }

  void properties::change_string(const std::string & a_key,
                                 const std::string & value,
                                 size_t index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change_string_scalar(const std::string & a_key,
                                        const std::string & value)
  {
    this->change(a_key, value, 0);
    return;
  }

  void properties::change_string_vector(const std::string & a_key,
                                        const std::string & value,
                                        size_t index)
  {
    this->change(a_key, value, index);
    return;
  }



  void properties::change(const std::string & a_key, const data::vbool& values)
  {
    this->_change_vector_impl_(a_key, values);
    return;
  }

  void properties::change(const std::string & a_key, const data::vint& values)
  {
    this->_change_vector_impl_(a_key, values);
    return;
  }

  void properties::change(const std::string & a_key,
                          const data::vdouble& values)
  {
    this->_change_vector_impl_(a_key, values);
    return;
  }

  void properties::change(const std::string & a_key,
                          const data::vstring& values)
  {
    this->_change_vector_impl_(a_key, values);
    return;
  }

  //------
  // - "update" member functions

  void properties::update_boolean(const std::string & a_key, bool value)
  {
    this->update(a_key, value);
    return;
  }

  void properties::update_flag(const std::string & a_key)
  {
    this->update(a_key, true);
    return;
  }

  void properties::update_integer(const std::string & a_key, int value)
  {
    this->update(a_key, value);
    return;
  }

  void properties::update_real(const std::string & a_key, double value)
  {
    this->update(a_key, value);
    return;
  }

  void properties::update_real_with_explicit_unit(const std::string & a_key, double value)
  {
    this->update(a_key, value);
    this->set_explicit_unit(a_key, true);
    return;
  }

  void properties::update_with_explicit_unit(const std::string & a_key, double value)
  {
    this->update(a_key, value);
    this->set_explicit_unit(a_key, true);
    return;
  }

  void properties::update(const std::string & a_key, const char* value)
  {
    std::string tmp = (value != 0 ? value : "");
    this->update(a_key, tmp);
    return;
  }

  void properties::update_string(const std::string & a_key,
                                 const std::string & value)
  {
    this->update(a_key, value);
    return;
  }

  //-----
  // - "fetch" member functions
  const properties::data &
  properties::get(const std::string & prop_key) const
  {
    return this->_get_valid_data_(prop_key);
  }

 void properties::fetch(const std::string & a_key, bool& value,
                         size_t index) const
  {
    this->_fetch_scalar_impl_(a_key, value, index);
    return;
  }

  bool properties::fetch_boolean(const std::string & a_key, size_t index) const
  {
    bool value;
    this->fetch(a_key, value, index);
    return value;
  }

  bool properties::fetch_boolean_scalar(const std::string & name) const
  {
    return this->fetch_boolean(name, 0);
  }

  bool properties::fetch_boolean_vector(const std::string & name,
                                        size_t index) const
  {
    return this->fetch_boolean(name, index);
  }

  bool properties::has_flag(const std::string & a_key) const
  {
    bool flag_is_on = false;
    try {
      flag_is_on = this->fetch_boolean(a_key, 0);
    }
    catch (std::logic_error& e) {
    }
    return flag_is_on;
  }


  void properties::fetch(const std::string & a_key, int& value,
                         size_t index) const
  {
    this->_fetch_scalar_impl_(a_key, value, index);
    return;
  }

  int properties::fetch_integer(const std::string & a_key, size_t index) const
  {
    int value;
    this->fetch(a_key, value, index);
    return value;
  }

  int properties::fetch_integer_scalar(const std::string & name) const
  {
    return this->fetch_integer(name, 0);
  }

  int properties::fetch_integer_vector(const std::string & name,
                                       size_t index) const
  {
    return this->fetch_integer(name, index);
  }

  int properties::fetch_range_integer(const std::string & a_key, int min_, int max_, size_t index) const
  {
    int value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(value < min_ || value > max_, std::range_error,
                "Out of [" << min_ << ";" << max_ << "] range integer value [" << value << "]!");
    return value;
  }

  unsigned int properties::fetch_positive_integer(const std::string & a_key, size_t index) const
  {
    int value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(value < 0, std::domain_error, "Unexpected integer negative value [" << value << "]!");
    return (unsigned int) value;
  }

  unsigned int properties::fetch_strict_positive_integer(const std::string & a_key, size_t index) const
  {
    int value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(value <= 0, std::domain_error, "Unexpected integer negative/zero value [" << value << "]!");
    return (unsigned int) value;
  }

  void properties::fetch(const std::string & a_key, double& value,
                         size_t index) const
  {
    this->_fetch_scalar_impl_(a_key, value, index);
    return;
  }

  double properties::fetch_real(const std::string & a_key, size_t index) const
  {
    double value;
    this->fetch(a_key, value, index);
    return value;
  }

  double properties::fetch_real_scalar(const std::string & name) const
  {
    return this->fetch_real(name, 0);
  }

  double properties::fetch_real_vector(const std::string & name,
                                       size_t index) const
  {
    return this->fetch_real(name, index);
  }

  double properties::fetch_dimensionless_real(const std::string & a_key,
                                              size_t index) const
  {
    DT_THROW_IF(has_explicit_unit(a_key),
                std::logic_error,
                "Property '" << a_key << "' is not dimensionless!");
    return this->fetch_real(a_key, index);
  }

  double properties::fetch_real_with_explicit_unit(const std::string & a_key,
                                                   size_t index) const
  {
    DT_THROW_IF(!has_explicit_unit(a_key),
                std::logic_error,
                "Property '" << a_key << "' has no explicit unit!");
    return this->fetch_real(a_key, index);
  }

  double properties::fetch_real_with_explicit_dimension(const std::string & a_key,
                                                        const std::string & a_dimension,
                                                        size_t index) const
  {
    double unit_value = 1.0;
    std::string unit_symbol;
    std::string unit_label;
    DT_THROW_IF(!units::find_unit(get_unit_symbol(a_key), unit_value, unit_label), std::logic_error, "Missing unit !");
    DT_THROW_IF(unit_label != a_dimension,
                std::logic_error,
                "Property '" << a_key << "' has no '" << a_dimension << "' unit; found '" << unit_label << "'!");
    return this->fetch_real_with_explicit_unit(a_key, index);
  }


  void properties::fetch(const std::string & a_key, std::string & value,
                         size_t index) const
  {
    this->_fetch_scalar_impl_(a_key, value, index);
    return;
  }

  std::string properties::fetch_string(const std::string & a_key,
                                       size_t index) const
  {
    std::string value;
    this->fetch(a_key, value, index);
    return value;
  }

  std::string properties::fetch_string_scalar(const std::string & name) const
  {
    return this->fetch_string(name, 0);
  }

  std::string properties::fetch_string_vector(const std::string & name,
                                              size_t index) const
  {
    return this->fetch_string(name, index);
  }

  std::string properties::fetch_path(const std::string & a_key,
                                     size_t index) const
  {
    std::string value = this->fetch_string(a_key, index);
    DT_THROW_IF(!fetch_path_with_env(value),
                std::logic_error,
                "Property '" << a_key << "' cannot be interpreted as a valid path string !");
    return value;
  }

  std::string properties::fetch_path_scalar(const std::string & name) const
  {
    return this->fetch_path(name, 0);
  }

  std::string properties::fetch_path_vector(const std::string & name,
                                            size_t index) const
  {
    return this->fetch_path(name, index);
  }


  char properties::fetch_one_character(const std::string & a_key, size_t index) const
  {
    std::string value = this->fetch_string(a_key, index);
    DT_THROW_IF(value.length() != 1, std::logic_error,
                "String property named '" << a_key << "' with value '" << value << "' does not contain one single character!");
    return value[0];
  }


  void properties::fetch(const std::string & a_key, data::vbool& values) const
  {
    this->_fetch_vector_impl_(a_key, values);
    return;
  }

  void properties::fetch(const std::string & a_key, data::vint& values) const
  {
    this->_fetch_vector_impl_(a_key, values);
    return;
  }

  void properties::fetch(const std::string & a_key,
                         std::set<int>& values,
                         bool allow_duplication_) const
  {
    data::vint vvalues;
    this->fetch(a_key, vvalues);
    for (const int& value : vvalues) {
      DT_THROW_IF(! allow_duplication_ && values.count(value) != 0,
                  std::logic_error,
                  "Duplicated integer value [" << value << "] at key '" << a_key << "'!");
      values.insert(value);
    }
    return;
  }

  void properties::fetch_positive(const std::string & a_key,
                                  std::set<unsigned int>& values,
                                  bool allow_duplication_) const
  {
    std::set<int> vvalues;
    this->fetch(a_key, vvalues, allow_duplication_);
    auto firstNeg = std::find_if(vvalues.begin(), vvalues.end(), [](int i){return i < 0;});
    DT_THROW_IF(firstNeg != vvalues.end(),
                std::logic_error,
                "Unauthorized negative integer value [" << *firstNeg << "] at key '" << a_key << "'!");
    values.insert(vvalues.begin(), vvalues.end());
    return;
  }


  void properties::fetch(const std::string & a_key,
                         data::vdouble& values) const
  {
    this->_fetch_vector_impl_(a_key, values);
    return;
  }

  void properties::fetch_dimensionless(const std::string & a_key,
                                       data::vdouble& values) const
  {
    DT_THROW_IF(has_explicit_unit(a_key),
                std::logic_error,
                "Property '" << a_key << "' is not dimensionless!");
    this->fetch(a_key, values);
    return;
  }


  void properties::fetch(const std::string & a_key,
                         data::vstring& values) const
  {
    this->_fetch_vector_impl_(a_key, values);
    return;
  }

  void properties::fetch(const std::string & a_key,
                         std::set<std::string>& values,
                         bool allow_duplication_) const
  {
    std::vector<std::string> vvalues;
    this->fetch(a_key, vvalues);
    for (const std::string& value : vvalues) {
      DT_THROW_IF(! allow_duplication_ && values.count(value) != 0,
                  std::logic_error,
                  "Duplicated string value '" << value << "' at key '" << a_key << "'!");
      values.insert(value);
    }
    return;
  }

  void properties::fetch_unique_ordered(const std::string & key_,
                                        std::vector<std::string> & values_) const
  {
    // Modifies whatever happens later...
    values_.clear();

    // Fetching via the set specialization will throw on duplication
    std::set<std::string> tmp;
    this->fetch(key_, tmp, false);

    this->fetch(key_, values_);
    return;
  }

   std::string properties::key_to_string(const std::string & a_key) const
  {
    if (!this->has_key(a_key)) return "";
    std::ostringstream oss;
    auto& data_ptr = this->_get_valid_data_(a_key);
    data_ptr.to_string(oss);
    return oss.str();
  }

  std::string
  properties::key_to_property_string(const std::string & key_) const
  {
    if (!this->has_key(key_)) return "";
    std::ostringstream oss;
    auto& data_ptr = this->_get_valid_data_(key_);
    oss << key_ << '=';
    data_ptr.to_string(oss);
    return oss.str();
  }

  void properties::print_tree(std::ostream & out_,
                              const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);
    std::ostringstream outs;
    if (! popts.title.empty ()) {
      outs << popts.indent << popts.title << std::endl;
    }
    bool list_props = true;
    // if (options_.get<bool>("short")) {
    //   list_props = false;
    // }

    if (!_description_.empty()) {
      outs << popts.indent << tag
           << "Description  : '" << this->get_description() << "'" << std::endl;
      std::string short_desc;
      outs << popts.indent << skip_tag << tag
           << "Short description : ";
      if (fetch_short_description(short_desc)) {
        outs << "'" << short_desc << "'";
      } else {
        outs << "<none>";
      }
      outs << std::endl;
      outs << popts.indent << skip_tag << last_tag
           << "Auxiliary descriptions : ";
      std::vector<std::string> aux;
      fetch_auxiliary_descriptions(aux);
      if (aux.size() == 0) outs << "<none>";
      outs << std::endl;
      for (std::size_t iaux = 0; iaux < aux.size(); iaux++) {
        outs << popts.indent << skip_tag << last_skip_tag;
        if (iaux + 1 == aux.size()) {
          outs << last_tag;
        } else {
          outs << tag;
        }
        outs << "Aux: '" << aux[iaux] << "'";
        outs << std::endl;
      }
    }

    outs << popts.indent << inherit_tag(popts.inherit)
         << "Properties : ";
    if (_props_.size() == 0) {
      outs << "<empty>";
    } else {
      outs << '[' << _props_.size() << ']';
    }
    outs << std::endl;
    if (list_props && _props_.size()) {
      for (auto i = _props_.begin();
           i != _props_.end();
           ++i) {
        const std::string & a_key = i->first;
        const properties::data & a_data = i->second;
        outs << popts.indent << inherit_skip_tag(popts.inherit);
        std::ostringstream indent_oss;
        indent_oss << popts.indent;
        indent_oss << inherit_skip_tag(popts.inherit);
        auto j = i;
        j++;
        if (j == _props_.end()) {
          outs << i_tree_dumpable::inherit_tag(popts.inherit);
          indent_oss << i_tree_dumpable::inherit_skip_tag(popts.inherit);
        } else {
          outs << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        outs << "Name : " << "'" << a_key << "'" << std::endl;
        a_data.tree_dump(outs, "", indent_oss.str());
      }
    }

    out_ << outs.str();
    return;
  }

  void properties::tree_dump(std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    std::string indent;
    if (!indent_.empty()) indent = indent_;
    std::ostringstream outs;
    if (!title_.empty()) outs << indent << title_ << std::endl;

    if (!_description_.empty()) {
      outs << indent << i_tree_dumpable::tag
           << "Description  : '" << this->get_description() << "'" << std::endl;
      std::string short_desc;
      outs << indent << i_tree_dumpable::skip_tag << i_tree_dumpable::tag
           << "Short description : ";
      if (fetch_short_description(short_desc)) {
        outs << "'" << short_desc << "'";
      } else {
        outs << "<none>";
      }
      outs << std::endl;
      outs << indent << i_tree_dumpable::skip_tag << i_tree_dumpable::last_tag
           << "Auxiliary descriptions : ";
      std::vector<std::string> aux;
      fetch_auxiliary_descriptions(aux);
      if (aux.size() == 0) outs << "<none>";
      outs << std::endl;
      for (std::size_t iaux = 0; iaux < aux.size(); iaux++) {
        outs << indent << i_tree_dumpable::skip_tag << i_tree_dumpable::last_skip_tag;
        if (iaux + 1 == aux.size()) {
          outs << i_tree_dumpable::last_tag;
        } else {
          outs << i_tree_dumpable::tag;
        }
        outs << "Aux: '" << aux[iaux] << "'";
        outs << std::endl;
      }
    }

    if (_props_.size() == 0) {
      outs << indent << i_tree_dumpable::inherit_tag(inherit_)
           << "<no property>" << std::endl;
    } else {
      for (auto i = _props_.begin();
           i != _props_.end();
           ++i) {
        const std::string & a_key = i->first;
        const properties::data& a_data = i->second;
        outs << indent;
        std::ostringstream indent_oss;
        indent_oss << indent;
        auto j = i;
        j++;
        if (j == _props_.end()) {
          outs << i_tree_dumpable::inherit_tag(inherit_);
          indent_oss << i_tree_dumpable::inherit_skip_tag(inherit_);
        } else {
          outs << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        outs << "Name : " << "'" << a_key << "'" << std::endl;
        a_data.tree_dump(outs, "", indent_oss.str());
      }
    }
    out_ << outs.str();
    return;
  }

  void properties::dump(std::ostream & out_) const
  {
    this->tree_dump(out_, "datatools::properties");
    return;
  }

  void properties::write_configuration(const std::string & filename_,
                                       uint32_t options_) const
  {
    properties_config writer(options_);
    writer.write(filename_, *this);
    return;
  }

  /*
    void properties::write_configuration(const std::string & filename,
    bool a_use_smart_modulo,
    bool a_write_public_only) const
    {
    DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Deprecated method!");
    uint32_t writer_opts = 0;
    if (a_use_smart_modulo) {
    writer_opts |= config::SMART_MODULO;
    }
    if (a_write_public_only)  {
    writer_opts |= config::SKIP_PRIVATE;
    }
    config writer(writer_opts);
    writer.write(filename, *this);
    return;
    }
  */

  void properties::read_configuration(const std::string & filename_, uint32_t options_)
  {
    read_config(filename_, *this, options_);
    return;
  }

  // static
  void properties::write_config(const std::string & filename_,
                                const properties & props_,
                                uint32_t options_)
  {
    properties_config writer(options_);
    writer.write(filename_, props_);
    return;
  }

  // static
  void properties::read_config(const std::string & filename_,
                               properties & props_,
                               uint32_t options_)
  {
    properties_config r(options_);
    r.read(filename_, props_);
    return;
  }


  // static
  std::string properties::build_property_key(const std::string & prefix_,
                                             const std::string & subkey_)
  {
    std::string s(prefix_);
    s += '.';
    s += subkey_;
    return s;
  }

  void properties::merge_with(const properties & other_, bool allow_override_)
  {
    std::vector<std::string> other_keys;
    other_.keys(other_keys);
    for (const std::string & okey : other_keys) {
      const data & odata = other_.get(okey);
      if (!this->has_key(okey)) {
        this->_props_[okey] = odata;
      } else {
        DT_THROW_IF(!allow_override_, std::logic_error,
                    "Overriding property '" << okey << "' is not allowed!");
        const data & tdata = this->get(okey);
        // Checl consistency of the override:
        DT_THROW_IF(odata.get_type() != tdata.get_type(),
                    std::logic_error, "Unmatching type for overriden property '" << okey << "'!");
        if (odata.is_vector() && !tdata.is_vector()) {
          DT_THROW(std::logic_error, "Unmatching cardinality for overriden property '" << okey << "'!");
        }
        if (odata.is_locked() && !tdata.is_locked()) {
          DT_THROW(std::logic_error, "Unmatching const flag for overriden property '" << okey << "'!");
        }
        if (odata.is_string()) {
          DT_THROW_IF(odata.is_explicit_path() && ! tdata.is_explicit_path(),
                      std::logic_error,
                      "Unmatching explicit path flag for overriden property '" << okey << "'!");
        } else if (odata.is_real()) {
          DT_THROW_IF(odata.has_explicit_unit() && ! tdata.has_explicit_unit(),
                      std::logic_error,
                      "Unmatching explicit unit flag for overriden property '" << okey << "'!");
          if (odata.has_explicit_unit() && ! odata.get_unit_symbol().empty()) {
            double ounit_value = 1.0;
            std::string ounit_symbol = odata.get_unit_symbol();
            std::string ounit_label;
            units::find_unit(ounit_symbol, ounit_value, ounit_label);
            if (tdata.has_explicit_unit() && ! tdata.get_unit_symbol().empty()) {
              double tunit_value = 1.0;
              std::string tunit_symbol = tdata.get_unit_symbol();
              std::string tunit_label;
              units::find_unit(tunit_symbol, tunit_value, tunit_label);
              DT_THROW_IF(ounit_label != tunit_label, std::logic_error,
                          "Unmatching dimension for unit '" << ounit_symbol << "' vs '" << tunit_symbol << "' for overriden property '" << okey << "'!");
            }
          }
        }
        this->clean(okey);
        this->_props_[okey] = odata;
      }
    }
    return;
  }

  // static
  const properties & empty_config()
  {
    static const properties _empty;
    return _empty;
  }

  void properties::export_to_string_based_dictionary(std::map<std::string, std::string> & dict_,
                                                     bool quoted_strings_) const
  {
    for (const auto& p : _props_) {
      const std::string & prop_key = p.first;
      const data & a_data = p.second;

      std::ostringstream valoss;

      if (a_data.is_vector()) valoss << '(';

      for (int i = 0; i < (int)a_data.get_size(); ++i) {
        if (i != 0) valoss << ',';
        if (a_data.is_boolean()) valoss << a_data.get_boolean_value(i);
        if (a_data.is_integer()) valoss << a_data.get_integer_value(i);
        if (a_data.is_real()) {
          ::datatools::io::write_real_number(valoss,
                                             a_data.get_real_value(i),
                                             datatools::io::REAL_PRECISION);
        }
        if (a_data.is_string()) {
          if (quoted_strings_) valoss << '"';
          valoss << a_data.get_string_value(i);
          if (quoted_strings_) valoss << '"';
        }
      }
      if (a_data.is_vector()) valoss << ')';

      dict_[prop_key] = valoss.str();
    }
    return;
  }

  // Specialization :
  template <>
  bool check_serial_tag<properties>(const std::string stag_,
                                    const std::string /*alt_tag_*/,
                                    boost::enable_if< has_bsts<properties> >::type* /*dummy*/)
  {
    if (stag_ == properties::SERIAL_TAG) return true;
    if (stag_ == ::datatools::backward_serial_tag<properties> (0)) return true;
    if (stag_ == "datatools:utils::properties") return true;
    return false;
  }

} // end of namespace datatools
