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

// Third Party:
// - Boost:
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>

// This Project:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/logger.h>
#include <datatools/kernel.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/io.h>

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(::datatools::properties,
                                                      "datatools::properties")

// Support for old serialization tag :
  DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(::datatools::properties,
                                                                 "datatools::utils::properties")

#define DT_PROP_CFG_READ_THROW_IF(Condition, ExceptionType,                          \
                                  FileName,                                          \
                                  SectionName, SectionLineNumber,                    \
                                  LineNumber, Message)                               \
  {                                                                                  \
    if (Condition) {                                                                 \
      std::stringstream sDT_THROW_IF_ONLY;                                           \
      sDT_THROW_IF_ONLY << "[" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << ": "; \
      if (!FileName.empty()) {                                                       \
        sDT_THROW_IF_ONLY << "in file '" << FileName << "': ";                       \
      }                                                                              \
      if (SectionName.empty()) {                                                     \
        if (LineNumber > 0) {                                                        \
          sDT_THROW_IF_ONLY << "at line #" << LineNumber << ": ";                    \
        }                                                                            \
      } else {                                                                       \
        sDT_THROW_IF_ONLY << "in section '" << SectionName << "'";                   \
        if (SectionLineNumber > 0) {                                                 \
          sDT_THROW_IF_ONLY << " starting at line #" << SectionLineNumber;           \
        }                                                                            \
        sDT_THROW_IF_ONLY << ": ";                                                   \
      }                                                                              \
      sDT_THROW_IF_ONLY << Message << "]";                                           \
      throw ExceptionType(sDT_THROW_IF_ONLY.str());                                  \
    }                                                                                \
  }                                                                                  \
/**/

namespace {

  struct _format {
    static const char OPEN_VECTOR  = '['; ///< Open section character
    static const char ASSIGN_CHAR  = '='; ///< Assignement key/meta label character
    static const char DESC_CHAR    = ':'; ///< Type descriptor separator character
    static const char CLOSE_VECTOR = ']'; ///< Close section character
    static const char COMMENT_CHAR = '#'; ///< Comment character
    static const char SPACE_CHAR   = ' '; ///< Space character
    static const char CONTINUATION_CHAR = '\\'; ///< Continuation character
  };

}

namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(properties,"datatools::properties")

  properties::default_key_validator & properties::global_default_key_validator()
  {
    static boost::scoped_ptr<properties::default_key_validator> dkv;
    if (!dkv) {
      dkv.reset(new properties::default_key_validator);
    }
    return *dkv;
  }

  //----------------------------------------------------------------------
  // properties::data class implementation
  //
  const int properties::data::ERROR_SUCCESS;
  const int properties::data::ERROR_FAILURE;
  const int properties::data::ERROR_BADTYPE;
  const int properties::data::ERROR_RANGE;
  const int properties::data::ERROR_LOCK;

  const uint8_t properties::data::MASK_TYPE;
  const uint8_t properties::data::MASK_UNIT_SYMBOL;
  const uint8_t properties::data::MASK_EXPLICIT_PATH;
  const uint8_t properties::data::MASK_EXPLICIT_UNIT;
  const uint8_t properties::data::MASK_LOCK;
  const uint8_t properties::data::MASK_VECTOR;

  const uint8_t properties::data::TYPE_NONE;
  const uint8_t properties::data::TYPE_BOOLEAN;
  const uint8_t properties::data::TYPE_INTEGER;
  const uint8_t properties::data::TYPE_REAL;
  const uint8_t properties::data::TYPE_STRING;

  const char properties::data::TYPE_BOOLEAN_SYMBOL;
  const char properties::data::TYPE_INTEGER_SYMBOL;
  const char properties::data::TYPE_REAL_SYMBOL;
  const char properties::data::TYPE_STRING_SYMBOL;

  const char properties::data::STRING_FORBIDDEN_CHAR;

  const int properties::data::SCALAR_DEF;
  const int properties::data::SCALAR_SIZE;

  bool properties::data::defaults::boolean_value() {
    return false;
  }

  int properties::data::defaults::integer_value() {
    return 0;
  }

  double properties::data::defaults::real_value() {
    return 0.0;
  }

  const std::string properties::data::defaults::string_value()
  {
    static std::string s;
    return s;
  }

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
    } else {
      _flags_ |= MASK_VECTOR; // force vector
    }
    _flags_ &= ~MASK_TYPE;
    if (a_type == TYPE_BOOLEAN_SYMBOL) {
      _flags_ |= TYPE_BOOLEAN;
      if (memsize > 0) _boolean_values_.assign(memsize, defaults::boolean_value());
    }
    if (a_type == TYPE_INTEGER_SYMBOL) {
      _flags_ |= TYPE_INTEGER;
      if (memsize > 0) _integer_values_.assign(memsize, defaults::integer_value());
    }
    if (a_type == TYPE_REAL_SYMBOL) {
      _flags_ |= TYPE_REAL;
      if (memsize > 0) _real_values_.assign(memsize, defaults::real_value());
    }
    if (a_type == TYPE_STRING_SYMBOL) {
      _flags_ |= TYPE_STRING;
      if (memsize > 0) _string_values_.assign(memsize, defaults::string_value());
    }
    return ERROR_SUCCESS;
  }


  int properties::data::boolean(int a_size) {
    this->clear_values_();
    return this->init_values_(TYPE_BOOLEAN_SYMBOL, a_size);
  }

  bool properties::data::has_description() const
  {
    return !_description_.empty();
  }

  void properties::data::set_description(const std::string& a_description) {
    _description_ = a_description;
  }

  const std::string& properties::data::get_description() const {
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
    return _flags_ & MASK_UNIT_SYMBOL;
  }

  int properties::data::set_unit_symbol(const std::string& a_symbol) {
    if (!this->is_real()) return ERROR_BADTYPE;
    if (a_symbol.empty()) {
      clear_unit_symbol_();
    } else {
      _flags_ |= MASK_UNIT_SYMBOL;
      _unit_symbol_ = a_symbol;
    }
    return ERROR_SUCCESS;
  }

  const std::string& properties::data::get_unit_symbol() const {
    return _unit_symbol_;
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

  int properties::data::get_type() const {
    return (int) (_flags_ & MASK_TYPE);
  }


  bool properties::data::is_integer() const {
    return (_flags_ & MASK_TYPE) == TYPE_INTEGER;
  }


  bool properties::data::is_real() const {
    return (_flags_ & MASK_TYPE) == TYPE_REAL;
  }


  bool properties::data::is_path() const {
    return is_string() && _flags_ & MASK_EXPLICIT_PATH;
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


  bool properties::data::is_explicit_path() const {
    return (_flags_ & MASK_EXPLICIT_PATH) != 0;
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
    DT_THROW_IF(!this->has_type(),
                std::logic_error,
                "No type!");
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
    _flags_  = 0;
    this->init_values_(TYPE_BOOLEAN_SYMBOL, a_size);
    for (int i = 0; i < (int)this->size(); ++i) {
      this->set_value(a_value, i);
    }
  }


  properties::data::data(int a_value, int a_size) {
    _flags_  = 0;
    this->init_values_(TYPE_INTEGER_SYMBOL, a_size);
    for (int i = 0; i < (int)this->size(); ++i) {
      this->set_value(a_value, i);
    }
  }


  properties::data::data(double a_value, int a_size) {
    _flags_  = 0;
    this->init_values_(TYPE_REAL_SYMBOL, a_size);
    for (int i = 0; i < (int)this->size(); ++i) {
      this->set_value(a_value, i);
    }
  }


  properties::data::data(const std::string& a_value, int a_size) {
    _flags_  = 0;
    int code=0;
    this->init_values_(TYPE_STRING_SYMBOL, a_size);
    DT_THROW_IF(has_forbidden_char(a_value),
                std::logic_error,
                "Forbidden char in string '" << a_value << "'!");
    for (int i = 0; i < (int)this->size(); ++i) {
      code = this->set_value(a_value, i);
      DT_THROW_IF(code != ERROR_SUCCESS,
                  std::logic_error,
                  "Failure!");
    }
  }


  properties::data::data(const char* a_value, int a_size) {
    _flags_ = 0;
    this->init_values_(TYPE_STRING_SYMBOL, a_size);
    std::string tmp;
    if (a_value != 0) {
      tmp = a_value;
      DT_THROW_IF(has_forbidden_char(tmp),
                  std::logic_error,
                  "Forbidden char in string '" << tmp << "'!");
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


  int properties::data::set_value(double a_value, int a_index, bool a_explicit_unit) {
    if (!this->is_real()) return ERROR_BADTYPE;

    if (this->is_locked()) return ERROR_LOCK;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;
    if (!has_explicit_unit()) {
      if (a_explicit_unit) {
        set_explicit_unit(a_explicit_unit);
      }
    }
    _real_values_[a_index] = a_value;
    return ERROR_SUCCESS;
  }


  int properties::data::set_value_with_unit(double a_value, int a_index, const std::string & a_unit_symbol)
  {
    set_unit_symbol(a_unit_symbol);
    return set_value(a_value, a_index, !a_unit_symbol.empty());
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

  int properties::data::set_value(const std::string & a_value,
                                  int a_index,
                                  bool a_explicit_path) {
    if (!this->is_string()) return ERROR_BADTYPE;
    if (this->is_locked()) return ERROR_LOCK;
    if (!this->index_is_valid(a_index)) return ERROR_RANGE;
    /* special trick to forbid character '\"' in string as
     * it is used as separator for parsing:
     */
    if (this->has_forbidden_char(a_value)) return ERROR_FAILURE;
    _string_values_[a_index] = a_value;
    if (!is_explicit_path()) {
      if (a_explicit_path) {
        set_explicit_path(a_explicit_path);
      }
    }
    return ERROR_SUCCESS;
  }


  int properties::data::set_value(const char* a_value,
                                  int a_index,
                                  bool a_explicit_path) {
    return this->set_value(std::string(a_value), a_index, a_explicit_path);
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
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not boolean!");
    return value;
  }


  int properties::data::get_integer_value(int a_index) const {
    int value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not integer!");
    return value;
  }


  double properties::data::get_real_value(int a_index) const {
    double value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not real!");
    return value;
  }


  std::string properties::data::get_string_value(int a_index) const {
    std::string value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not string!");
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
    if (has_unit_symbol()) {
      a_out << ':' << this->get_unit_symbol();
    }
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
          a_out << "<invalid unit symbol '" << unit_symbol << "'>";
          unit_value = 1.0;
        }
      }
      a_out << "]";
    }

    if (this->is_string() && is_explicit_path()) {
      a_out << " [explicit path]";
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
  }


  //----------------------------------------------------------------------
  // properties::key_validator implementation
  //
  // const std::string properties::default_key_validator::ALLOWED_CHARS =
  //              "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";

  const std::string & properties::default_key_validator::allowed_chars()
  {
    static std::string chars;
    if (chars.empty()) {
      //chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_./";
      chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";
    }
    return chars;
  }

  properties::default_key_validator::default_key_validator() {}

  properties::default_key_validator::~default_key_validator() {}

  bool properties::default_key_validator::operator()(const std::string& prop_key) const {
    if (prop_key.empty()) return false;
    if (prop_key.find_first_not_of(allowed_chars()) != prop_key.npos) return false;
    if (prop_key.find_first_of("0123456789.") == 0) return false;
    if (prop_key[prop_key.size()-1] == '.') return false;
    return true;
  }


  //----------------------------------------------------------------------
  // properties class implementation
  //
  const std::string & properties::private_property_prefix()
  {
    static std::string prefix;
    if (prefix.empty()) prefix = "__";
    return prefix;
  }

  void properties::_validate_key_(const std::string& prop_key) const {
    if (_key_validator_ != 0) {
      DT_THROW_IF(!_key_validator_->operator()(prop_key),
                  std::logic_error,
                  "After key validator, the '"
                  << prop_key << "' key is not valid!");
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
    _key_validator_ = &global_default_key_validator();
    _key_validator_deletion_ = false; //true;
  }


  void properties::unset_key_validator() {
    this->_clear_key_validator_();
  }


  properties::properties() : _debug_(false), _key_validator_(0) {
    this->set_description("");
    this->set_default_key_validator();
  }


  properties::properties(const std::string& a_description)
    : _debug_(false), _key_validator_(0) {
    this->set_description(a_description);
    this->set_default_key_validator();
  }


  properties::properties(const std::string& a_description,
                         const basic_key_validator& prop_key_validator)
    : _debug_(false), _key_validator_(0) {
    this->set_description(a_description);
    this->set_key_validator(prop_key_validator);
  }


  properties::properties(const basic_key_validator& prop_key_validator)
    : _debug_(false), _key_validator_(0) {
    this->set_description("");
    this->set_key_validator(prop_key_validator);
  }


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


  properties::~properties() {
    _props_.clear();
    _clear_key_validator_();
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
    DT_THROW_IF(found == _props_.end(),
                std::logic_error,
                "Key '" << prop_key << "' not known!");
    _props_.erase(found);
  }


  void properties::erase_all_starting_with(const std::string& prefix) {
    keys_col_type local_keys;
    this->keys_starting_with(local_keys, prefix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i){
      this->erase(*i);
    }
  }


  void properties::erase_all_not_starting_with(const std::string& prefix) {
    keys_col_type local_keys;
    keys_not_starting_with(local_keys, prefix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i){
      this->erase(*i);
    }
  }


  void properties::export_and_rename_starting_with(properties& a_props,
                                                   const std::string& prop_key_prefix,
                                                   const std::string& a_new_prefix) const {
    DT_THROW_IF(this == &a_props,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys_starting_with(local_keys, prop_key_prefix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i !=  local_keys.end(); ++i) {
      properties& ptmp = const_cast<properties&>(*this);
      std::string new_key = *i;
      boost::replace_first(new_key, prop_key_prefix, a_new_prefix);
      a_props._props_[new_key] = ptmp._props_[*i];
    }
  }

  void properties::export_all(properties & a_props) const {
    DT_THROW_IF(this == &a_props,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys(local_keys);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      properties& ptmp = const_cast<properties&>(*this);
      a_props._props_[*i] = ptmp._props_[*i];
    }
  }

  void properties::export_all_adding_prefix(properties & a_props,
                                            const std::string & prefix) const {
    DT_THROW_IF(this == &a_props,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys(local_keys);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      properties& ptmp = const_cast<properties&>(*this);
      std::ostringstream new_key_oss;
      new_key_oss << prefix << *i;
      a_props._props_[new_key_oss.str()] = ptmp._props_[*i];
    }
  }

  void properties::export_starting_with(properties & a_props,
                                        const std::string& prefix) const {
    DT_THROW_IF(this == &a_props,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys_starting_with(local_keys, prefix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      properties& ptmp = const_cast<properties&>(*this);
      a_props._props_[*i] = ptmp._props_[*i];
    }
  }


  void properties::export_not_starting_with(properties& a_props,
                                            const std::string& prefix) const {
    DT_THROW_IF(this == &a_props,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys_not_starting_with(local_keys, prefix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i !=  local_keys.end(); ++i) {
      properties& ptmp = const_cast<properties&>(*this);
      a_props._props_[*i] = ptmp._props_[*i];
    }
  }


  void properties::erase_all_ending_with(const std::string& suffix) {
    keys_col_type local_keys;
    this->keys_ending_with(local_keys, suffix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      this->erase(*i);
    }
  }


  void properties::erase_all_not_ending_with(const std::string& suffix) {
    keys_col_type local_keys;
    this->keys_not_ending_with(local_keys, suffix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      this->erase(*i);
    }
  }


  void properties::export_ending_with(properties& a_props,
                                      const std::string& suffix) const {
    DT_THROW_IF(this == &a_props,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys_ending_with(local_keys, suffix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      properties& ptmp = const_cast<properties&>(*this);
      a_props._props_[*i] = ptmp._props_[*i];
    }
  }


  void properties::export_not_ending_with(properties& a_props,
                                          const std::string& suffix) const {
    DT_THROW_IF(this == &a_props,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys_not_ending_with(local_keys, suffix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i !=  local_keys.end(); ++i) {
      properties& ptmp = const_cast<properties&>(*this);
      a_props._props_[*i] = ptmp._props_[*i];
    }
  }


  void properties::erase_all() {
    _props_.clear();
  }


  void properties::clean(const std::string& a_key) {
    pmap::iterator found = _props_.find(a_key);
    if (found != _props_.end()) {
      _props_.erase(found);
    }
  }


  void properties::clear() {
    _props_.clear();
    _clear_key_validator_();
  }


  void properties::reset() {
    this->set_description("");
    _props_.clear();
    this->_clear_key_validator_();
    _debug_ = false;
  }


  void properties::keys_not_starting_with(std::vector<std::string>& some_keys,
                                          const std::string& prefix) const {
    DT_THROW_IF(prefix.empty(),
                std::logic_error,
                "Empty key prefix argument !");
    size_t n = prefix.size();
    for (pmap::const_iterator iter = _props_.begin();
         iter != _props_.end();
         ++iter) {
      bool push = true;
      if (iter->first.substr(0, n) == prefix) push = false;
      if (push) some_keys.push_back(iter->first);
    }
  }


  std::vector<std::string>
  properties::keys_not_starting_with(const std::string& prefix) const {
    std::vector<std::string> lkeys;
    this->keys_not_starting_with(lkeys, prefix);
    return lkeys;
  }


  void properties::keys_starting_with(std::vector<std::string>& some_keys,
                                      const std::string& prefix) const {
    DT_THROW_IF(prefix.empty(),
                std::logic_error,
                "Empty key prefix argument !");
    size_t n = prefix.size();
    for (pmap::const_iterator iter = _props_.begin();
         iter != _props_.end();
         ++iter) {
      if (iter->first.size() < n) continue;
      if (iter->first.substr(0, n) == prefix) {
        some_keys.push_back(iter->first);
      }
    }
  }


  std::vector<std::string>
  properties::keys_starting_with(const std::string& prefix) const {
    std::vector<std::string> lkeys;
    this->keys_starting_with(lkeys, prefix);
    return lkeys;
  }


  void properties::keys_not_ending_with(std::vector<std::string>& prop_keys,
                                        const std::string& suffix) const {
    DT_THROW_IF(suffix.empty(),
                std::logic_error,
                "Empty key suffix argument !");
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


  std::vector<std::string> properties::keys_not_ending_with(
                                                            const std::string& suffix) const {
    std::vector<std::string> lkeys;
    this->keys_not_ending_with(lkeys, suffix);
    return lkeys;
  }


  void properties::keys_ending_with(std::vector<std::string>& prop_keys,
                                    const std::string& suffix) const {
    DT_THROW_IF(suffix.empty(),
                std::logic_error,
                "Empty key suffix argument in properties described by '" << get_description() << "' !");
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


  std::vector<std::string> properties::keys_ending_with(const std::string& suffix) const {
    std::vector<std::string> lkeys;
    this->keys_ending_with(lkeys, suffix);
    return lkeys;
  }


  std::vector<std::string> properties::keys() const {
    std::vector<std::string> lkeys;
    this->keys(lkeys);
    return lkeys;
  }


  void properties::keys(std::vector<std::string>& some_keys) const {
    for (pmap::const_iterator iter = _props_.begin();
         iter != _props_.end();
         ++iter) {
      some_keys.push_back(iter->first);
    }
  }

  const properties::data &
  properties::get(const std::string& prop_key) const
  {
    pmap::const_iterator found = _props_.find(prop_key);
    DT_THROW_IF(found == _props_.end(),
                std::logic_error,
                "Property '" << prop_key << "' does not exist in properties described by '" << get_description() << "' !");
    const data & pd = found->second;
    return pd;
  }

  void properties::store(const std::string& prop_key, const data& value) {
    this->_check_nokey_(prop_key);
    this->_validate_key_(prop_key);
    _props_[prop_key] = value;
  }

  const std::string & properties::key(int key_index_) const
  {
    int key_count = 0;
    pmap::const_iterator iter = _props_.begin();
    for (;
         iter != _props_.end();
         ++iter, ++key_count) {
      if (key_count == key_index_) {
        break;
      };
    }
    DT_THROW_IF(iter == _props_.end(),
                std::logic_error,
                "Invalid key index '" << key_index_ << "' in properties described by '" << get_description() << "' !");
    return iter->first;
  }

  void properties::lock(const std::string& a_key) {
    key_lock(a_key);
  }


  void properties::unlock(const std::string& a_key) {
    key_unlock(a_key);
  }


  bool properties::is_locked(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_locked();
  }


  std::string properties::make_private_key(const std::string& a_key) {
    if (properties::key_is_private(a_key)) return a_key;

    std::ostringstream oss;
    oss << private_property_prefix() << a_key;
    return oss.str();
  }


  bool properties::key_is_private(const std::string& a_key) {
    if (a_key.size() < 2) return false;
    return a_key.substr(0, private_property_prefix().size())
      == private_property_prefix();
  }


  bool properties::key_is_public(const std::string& a_key) {
    return !properties::key_is_private(a_key);
  }


  bool properties::is_private(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return this->key_is_private(a_key);
  }


  bool properties::is_public(const std::string& a_key) const {
    return !this->is_private(a_key);
  }


  bool properties::is_boolean(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_boolean();
  }


  bool properties::is_integer(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_integer();
  }


  bool properties::is_real(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_real();
  }


  bool properties::is_string(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_string();
  }


  bool properties::is_scalar(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_scalar();
  }


  bool properties::is_vector(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_vector();
  }


  int32_t properties::size(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->size();
  }

  int32_t properties::key_size(const std::string& a_key) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->size();
  }


  bool properties::has_key(const std::string& a_key) const {
    if (_props_.size() == 0) return false;
    bool hk = _props_.find(a_key) != _props_.end();
    return hk;
  }


  void properties::key_lock(const std::string& a_key)
  {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    data_ptr->lock();
    return;
  }

  void properties::key_unlock(const std::string& a_key)
  {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    data_ptr->unlock();
    return;
  }

  const std::string &
  properties::get_key_description(const std::string& a_key) const
  {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->get_description();
  }

  void properties::set_key_description(const std::string& a_key,
                                       const std::string& desc_)
  {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    data_ptr->set_description(desc_);
    return;
  }

  void properties::_check_nokey_(const std::string& a_key) const {
    DT_THROW_IF(this->has_key(a_key),
                std::logic_error,
                "Key '" << a_key << "' already used in properties described by '" << get_description() << "' !");
  }


  void properties::_check_key_(const std::string& a_key, data **a_data) {
    pmap::iterator iter = _props_.find(a_key);
    DT_THROW_IF(_props_.find(a_key) == _props_.end(),
                std::logic_error,
                "Key '" << a_key << "' does not exist in properties described by '" << get_description() << "' !");
    *a_data = &(iter->second);
  }


  void properties::_check_key_(const std::string& a_key,
                               const data **a_data) const {
    pmap::const_iterator iter = _props_.find(a_key);
    DT_THROW_IF(_props_.find(a_key) == _props_.end(),
                std::logic_error,
                "Key '" << a_key << "' does not exist in properties described by '" << get_description() << "' !");
    *a_data = &(iter->second);
  }


  void properties::store_flag(const std::string& a_key,
                              const std::string& a_description,
                              bool a_lock) {
    this->store(a_key, true, a_description, a_lock);
  }


  void properties::store(const std::string& a_key, bool a_value,
                         const std::string& a_description, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    data a_data(a_value, data::SCALAR_DEF);
    a_data.set_description(a_description);
    _props_[a_key] = a_data;
    if (a_lock) _props_[a_key].lock();
  }


  void properties::store(const std::string& a_key, int value,
                         const std::string& description, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    data a_data(value);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    if (a_lock) _props_[a_key].lock();
  }


  void properties::store(const std::string& a_key, double value,
                         const std::string& description, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    data a_data(value);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    if (a_lock) _props_[a_key].lock();
  }


  bool properties::is_explicit_path(const std::string& a_key) const
  {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_string(),
                std::logic_error,
                "Property '" << a_key << "' is not of string type !");
    return data_ptr->is_explicit_path();
  }


  bool properties::has_unit_symbol(const std::string& a_key) const
  {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    return data_ptr->has_unit_symbol();
  }

  const std::string & properties::get_unit_symbol(const std::string& a_key) const
  {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    return data_ptr->get_unit_symbol();
  }

  void properties::set_unit_symbol(const std::string& a_key, const std::string& unit_symbol)
  {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    DT_THROW_IF(data_ptr->set_unit_symbol(unit_symbol) != data::ERROR_SUCCESS,
                std::logic_error, "Setting unit symbol fails !");
    return;
  }


  bool properties::has_explicit_unit(const std::string& a_key) const
  {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    return data_ptr->has_explicit_unit();
  }


  void properties::set_explicit_path(const std::string& a_key, bool a_explicit_path)
  {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_string(),
                std::logic_error,
                "Property '" << a_key << "' is not of string type in properties described by '" << get_description() << "' !");
    DT_THROW_IF(data_ptr->set_explicit_path(a_explicit_path) != data::ERROR_SUCCESS,
                std::logic_error, "Setting explicit path fails !");
    return;
  }


  void properties::set_explicit_unit(const std::string& a_key, bool a_explicit_unit)
  {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    DT_THROW_IF(data_ptr->set_explicit_unit(a_explicit_unit) != data::ERROR_SUCCESS,
                std::logic_error,
                "Setting explicit unit fails for property '" << a_key
                << "' in properties described by '" << get_description() << "' !");
    return;
  }

  void properties::store(const std::string& a_key, const std::string& value,
                         const std::string& description, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    data a_data(value);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    if (a_lock) _props_[a_key].lock();
    return;
  }


  void properties::store(const std::string& a_key, const char* value,
                         const std::string& description, bool a_lock) {
    this->store(a_key, std::string(value), description, a_lock);
    return;
  }


  void properties::store(const std::string& a_key, const data::vbool& values,
                         const std::string& description, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    int valsize = values.size();
    data a_data(data::TYPE_BOOLEAN_SYMBOL, valsize);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    for (int i = 0; i < valsize; i++) {
      _props_[a_key].set_value(values[i], i);
    }
    if (a_lock) _props_[a_key].lock();
    return;
  }


  void properties::store(const std::string& a_key, const data::vint& values,
                         const std::string& description, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    int valsize = values.size();
    data a_data(data::TYPE_INTEGER_SYMBOL, valsize);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    for (int i = 0; i < valsize; i++) {
      _props_[a_key].set_value(values[i], i);
    }
    if (a_lock) _props_[a_key].lock();
    return;
  }


  void properties::store(const std::string& a_key, const data::vdouble& values,
                         const std::string& description, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    int valsize = values.size();
    data a_data(data::TYPE_REAL_SYMBOL, valsize);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    for (int i = 0; i < valsize; i++) {
      _props_[a_key].set_value(values[i], i);
    }
    if (a_lock) _props_[a_key].lock();
  }


  void properties::store(const std::string& a_key, const data::vstring& values,
                         const std::string& description, bool a_lock) {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    int valsize = values.size();
    data a_data(data::TYPE_STRING_SYMBOL, valsize);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    for (int i = 0; i < valsize; i++) {
      _props_[a_key].set_value(values[i], i);
    }
    if (a_lock) _props_[a_key].lock();
  }

  void properties::store_paths(const std::string& a_key, const data::vstring& a_path_values,
                               const std::string& a_desc, bool a_lock)
  {
    store(a_key, a_path_values, a_desc, a_lock);
    set_explicit_path(a_key, true);
  }

  void properties::set_flag(const std::string& a_key) {
    this->store_flag(a_key, "", false);
  }


  void properties::unset_flag(const std::string& a_key) {
    this->clean(a_key);
  }


  void properties::store_boolean(const std::string& a_key, bool value,
                                 const std::string& desc, bool a_lock) {
    this->store(a_key, value, desc, a_lock);
  }


  void properties::store_integer(const std::string& a_key, int value,
                                 const std::string& desc, bool a_lock) {
    this->store(a_key, value, desc, a_lock);
  }


  void properties::store_real(const std::string& a_key, double value,
                              const std::string& desc, bool a_lock) {
    this->store(a_key, value, desc, a_lock);
  }


  void properties::store_real_with_explicit_unit(const std::string& a_key, double value,
                                                 const std::string& desc, bool a_lock) {
    this->store(a_key, value, desc, a_lock);
    this->set_explicit_unit(a_key, true);
  }

  void properties::store_with_explicit_unit(const std::string& a_key, double value,
                                            const std::string& desc, bool a_lock) {
    this->store_real_with_explicit_unit(a_key, value, desc, a_lock);
  }

  void properties::store_string(const std::string& a_key,
                                const std::string& value,
                                const std::string& desc,
                                bool a_lock) {
    this->store(a_key, value, desc, a_lock);
  }

  void properties::store_path(const std::string& a_key,
                              const std::string& path_value,
                              const std::string& desc,
                              bool a_lock) {
    this->store(a_key, path_value, desc, a_lock);
    this->set_explicit_path(a_key, true);
  }

  void properties::change_boolean(const std::string& a_key, bool value,
                                  int index) {
    this->change(a_key, value, index);
  }

  void properties::change_boolean_scalar(const std::string& a_key, bool value) {
    this->change(a_key, value, 0);
  }

  void properties::change_boolean_vector(const std::string& a_key, bool value,
                                         int index) {
    this->change(a_key, value, index);
  }


  void properties::change_integer(const std::string& a_key, int value,
                                  int index) {
    this->change(a_key, value, index);
  }

  void properties::change_integer_scalar(const std::string& a_key, int value) {
    this->change(a_key, value, 0);
  }

  void properties::change_integer_vector(const std::string& a_key, int value,
                                         int index) {
    this->change(a_key, value, index);
  }


  void properties::change_real(const std::string& a_key, double value,
                               int index) {
    this->change(a_key, value, index);
  }


  void properties::change_real_scalar(const std::string& a_key, double value)
  {
    this->change(a_key, value, 0);
  }

  void properties::change_real_vector(const std::string& a_key, double value,
                                      int index) {
    this->change(a_key, value, index);
  }

  void properties::change_string(const std::string& a_key,
                                 const std::string& value,
                                 int index) {
    this->change(a_key, value, index);
  }

  void properties::change_string_scalar(const std::string& a_key,
                                        const std::string& value) {
    this->change(a_key, value, 0);
  }


  void properties::change_string_vector(const std::string& a_key,
                                        const std::string& value,
                                        int index) {
    this->change_string(a_key, value, index);
  }

  void properties::change(const std::string& a_key, bool value, int index) {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);

    int error = data_ptr->set_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot change value for boolean property '"
                << a_key << "' in properties described by '" << get_description() << "' : "
                << data::get_error_message(error) << "!");
  }


  void properties::change(const std::string& a_key, int value, int index) {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->set_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot change value for integer property '"
                << a_key << "' in properties described by '" << get_description() << "' : "
                << data::get_error_message(error) << "!");
  }


  void properties::change(const std::string& a_key, double value, int index) {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->set_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot change value for property '"
                << a_key << "' in properties described by '" << get_description() << "' : "
                << data::get_error_message(error) << "!");
  }


  void properties::change(const std::string& a_key, const std::string& value,
                          int index) {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->set_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot change value for string property '"
                << a_key << "' in properties described by '" << get_description() << "' : "
                << data::get_error_message(error) << "!");
  }


  void properties::change(const std::string& a_key, const char* value,
                          int index) {
    std::string tmp = "";
    if (value != 0) tmp = value;
    this->change(a_key, tmp, index);
  }


  void properties::change(const std::string& a_key, const data::vbool& values) {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(!data_ptr->is_boolean() || !data_ptr->is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of booleans in properties described by '" << get_description() << "' !");
    if ((int)values.size() != data_ptr->get_size()) {
      int error = data_ptr->boolean(values.size());
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of booleans property '"
                  << a_key << "': "
                  << data::get_error_message(error) << " in properties described by '" << get_description() << "' !");
    }
    for (int i = 0; i < (int)values.size(); ++i) {
      int error = data_ptr->set_value(values[i], i);
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of booleans property '"
                  << a_key << "' in properties described by '" << get_description() << "': "
                  << data::get_error_message(error) << " !");
    }
  }


  void properties::change(const std::string& a_key, const data::vint& values) {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(!data_ptr->is_integer() || !data_ptr->is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of integers in properties described by '" << get_description() << "' !");
    if ((int)values.size() != data_ptr->get_size()) {
      int error = data_ptr->integer(values.size());
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of integers property '"
                  << a_key << "' in properties described by '" << get_description() << "': "
                  << data::get_error_message(error) << " !");
    }
    for (int i = 0; i < (int)values.size(); ++i) {
      int error = data_ptr->set_value(values[i], i);
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of integers property '"
                  << a_key << "' in properties described by '" << get_description() << "': "
                  << data::get_error_message(error) << " !");
    }
  }

  // XXX
  void properties::change(const std::string& a_key,
                          const data::vdouble& values) {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(!data_ptr->is_real() || ! data_ptr->is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of reals in properties described by '" << get_description() << "' !");
    if ((int)values.size() != data_ptr->get_size()) {
      int error = data_ptr->real(values.size());
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of reals property '"
                  << a_key << "' in properties described by '" << get_description() << "': "
                  << data::get_error_message(error) << " !");
    }
    for (int i = 0; i < (int)values.size(); ++i) {
      int error = data_ptr->set_value(values[i], i);
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of reals property '"
                  << a_key << "' in properties described by '" << get_description() << "': "
                  << data::get_error_message(error) << " !");
    }
  }


  void properties::change(const std::string& a_key,
                          const data::vstring& values) {
    data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(!data_ptr->is_string() || ! data_ptr->is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of strings in properties described by '" << get_description() << "' !");
    if ((int)values.size() != data_ptr->get_size()) {
      int error = data_ptr->string(values.size());
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of strings property '"
                  << a_key << "' in properties described by '" << get_description() << "': "
                  << data::get_error_message(error) << " !");
    }
    for (int i = 0; i < (int)values.size(); ++i) {
      int error = data_ptr->set_value(values[i], i);
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot change value for vector of strings property '"
                  << a_key << "' in properties described by '" << get_description() << "': "
                  << data::get_error_message(error) << " !");
    }
  }


  void properties::update_string(const std::string& a_key,
                                 const std::string& value) {
    this->update(a_key, value);
  }


  void properties::update_boolean(const std::string& a_key, bool value) {
    this->update(a_key, value);
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


  void properties::update_flag(const std::string& a_key) {
    this->update(a_key, true);
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


  void properties::update(const std::string& a_key, bool value) {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
  }


  void properties::update(const std::string& a_key, int value) {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
  }


  void properties::update(const std::string& a_key, double value) {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
    return;
  }

  void properties::update_with_explicit_unit(const std::string& a_key, double value) {
    update_real_with_explicit_unit(a_key, value);
  }

  void properties::update_real_with_explicit_unit(const std::string& a_key, double value) {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
    this->set_explicit_unit(a_key, true);
    return;
  }

  void properties::update(const std::string& a_key, const std::string& value) {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
  }

  void properties::update(const std::string& a_key, const char* value) {
    std::string tmp = "";
    if (value != 0) tmp = value;
    if (this->has_key(a_key)) {
      this->change(a_key, tmp);
    } else {
      this->store(a_key, tmp);
    }
  }


  void properties::update(const std::string& a_key,
                          const data::vbool& values) {
    if (this->has_key(a_key)) {
      this->change(a_key, values);
    } else {
      this->store(a_key, values);
    }
  }


  void properties::update(const std::string& a_key, const data::vint& values) {
    if (this->has_key(a_key)) {
      this->change(a_key, values);
    } else {
      this->store(a_key, values);
    }
  }


  void properties::update(const std::string& a_key,
                          const data::vdouble& values) {
    if (this->has_key(a_key)) {
      this->change(a_key, values);
    } else {
      this->store(a_key, values);
    }
  }


  void properties::update(const std::string& a_key,
                          const data::vstring& values) {
    if (this->has_key(a_key)) {
      this->change(a_key, values);
    } else {
      this->store(a_key, values);
    }
  }


  void properties::update_integer(const std::string& a_key, int value) {
    this->update(a_key, value);
  }


  void properties::update_real(const std::string& a_key, double value) {
    this->update(a_key, value);
  }



  void properties::fetch(const std::string& a_key, bool& value,
                         int index) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->get_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot fetch boolean value from property '"
                << a_key << "' in properties described by '" << get_description() << "': "
                << data::get_error_message(error) << " !");
  }


  void properties::fetch(const std::string& a_key, int& value,
                         int index) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->get_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot fetch integer value from property '"
                << a_key << "' in properties described by '" << get_description() << "': "
                << data::get_error_message(error) << " !");
  }


  void properties::fetch(const std::string& a_key, double& value,
                         int index) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->get_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot fetch real value from property '"
                << a_key << "' in properties described by '" << get_description() << "': "
                << data::get_error_message(error) << " !");
  }


  void properties::fetch(const std::string& a_key, std::string& value,
                         int index) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->get_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot fetch string value from property '"
                << a_key << "' in properties described by '" << get_description() << "': "
                << data::get_error_message(error) << " !");
  }


  bool properties::has_flag(const std::string& a_key) const {
    bool flag_is_on = false;
    try {
      flag_is_on = this->fetch_boolean(a_key, 0);
    }
    catch (std::logic_error& e) {
    }
    return flag_is_on;
  }


  bool properties::fetch_boolean(const std::string& a_key, int index) const {
    bool value;
    this->fetch(a_key, value, index);
    return value;
  }


  int properties::fetch_integer(const std::string& a_key, int index) const {
    int value;
    this->fetch(a_key, value, index);
    return value;
  }


  double properties::fetch_dimensionless_real(const std::string& a_key,
                                              int index) const {
    double value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(has_explicit_unit(a_key),
                std::logic_error,
                "Property '" << a_key << "' is not dimensionless!");
    return value;
  }

  double properties::fetch_real(const std::string& a_key, int index) const {
    double value;
    this->fetch(a_key, value, index);
    return value;
  }


  std::string properties::fetch_string(const std::string& a_key,
                                       int index) const {
    std::string value;
    this->fetch(a_key, value, index);
    return value;
  }


  std::string properties::fetch_path(const std::string& a_key,
                                     int index) const {
    std::string value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(!fetch_path_with_env(value),
                std::logic_error,
                "Property '" << a_key << "' cannot be interpreted as a valid path string !");
    return value;
  }


  void properties::fetch(const std::string& a_key, data::vbool& values) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(!data_ptr->is_boolean() || !data_ptr->is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of booleans !");
    values.resize(data_ptr->size());
    values.assign(data_ptr->size(), data::defaults::boolean_value());
    for (int i = 0; i < (int)values.size(); ++i) {
      bool val;
      int error = data_ptr->get_value(val, i);
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot fetch a vector of booleans from property '"
                  << a_key << "': " << data::get_error_message(error) << " !");
      values[i] = val;
    }
  }


  void properties::fetch(const std::string& a_key, data::vint& values) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(!data_ptr->is_integer() || ! data_ptr->is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of integers !");
    values.resize(data_ptr->size());
    values.assign(data_ptr->size(),data::defaults::integer_value());
    for (int i = 0; i < (int)values.size(); ++i) {
      int error = data_ptr->get_value(values[i], i);
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot fetch a vector of integers from property '"
                  << a_key << "' in properties described by '" << get_description() << "': " << data::get_error_message(error) << " !");
    }
  }

  void properties::fetch_dimensionless(const std::string& a_key,
                                       data::vdouble& values) const {
    this->fetch(a_key, values);
    DT_THROW_IF(has_explicit_unit(a_key),
                std::logic_error,
                "Property '" << a_key << "' is not dimensionless!");
  }

  void properties::fetch(const std::string& a_key,
                         data::vdouble& values) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(!data_ptr->is_real() || ! data_ptr->is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of reals !");
    values.resize(data_ptr->size());
    values.assign(data_ptr->size(), data::defaults::real_value());
    for (int i = 0; i < (int)values.size(); ++i) {
      int error = data_ptr->get_value(values[i], i);
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot fetch a vector of reals from property '"
                  << a_key << "' in properties described by '" << get_description() << "': " << data::get_error_message(error) << " !");
    }
  }


  void properties::fetch(const std::string& a_key,
                         data::vstring& values) const {
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(!data_ptr->is_string() || ! data_ptr->is_vector(),
                std::logic_error,
                "Property '" << a_key << "' is not a vector of string in properties described by '" << get_description() << "' !");
    values.resize(data_ptr->size());
    values.assign(data_ptr->size(), data::defaults::string_value());
    for (int i = 0; i < (int)values.size(); ++i) {
      int error = data_ptr->get_value(values[i], i);
      DT_THROW_IF(error != data::ERROR_SUCCESS,
                  std::logic_error,
                  "Cannot fetch  a vector of strings from property '"
                  << a_key << "' in properties described by '" << get_description() << "': " << data::get_error_message(error) << " !");
    }
  }

  void properties::fetch(const std::string& a_key,
                         std::set<std::string>& values,
                         bool allow_duplication_) const
  {
    std::vector<std::string> vvalues;
    this->fetch(a_key, vvalues);
    for (int i = 0; i < (int) vvalues.size(); i++) {
      const std::string & value = vvalues[i];
      DT_THROW_IF(values.count(value) != 0 && ! allow_duplication_,
                  std::logic_error,
                  "Duplicated string value '" << value << "' at key '" << a_key << "'!");
      values.insert(value);
    }
  }

  std::string properties::key_to_string(const std::string& a_key) const {
    if (!this->has_key(a_key)) return "";
    std::ostringstream oss;
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    data_ptr->to_string(oss);
    return oss.str();
  }


  std::string
  properties::key_to_property_string(const std::string& a_key) const {
    if (!this->has_key(a_key)) return "";
    std::ostringstream oss;
    const data *data_ptr = 0;
    this->_check_key_(a_key, &data_ptr);
    oss << a_key << '=';
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
        const std::string& a_key = i->first;
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
        out << "Name : " << "'" << a_key << "'" << std::endl;
        a_data.tree_dump(out, "", indent_oss.str());
      }
    }
  }


  void properties::dump(std::ostream& out) const {
    this->tree_dump(out, "datatools::properties");
  }

  void properties::write_configuration(const std::string& filename,
                                       uint32_t options) const
  {
    config writer(options);
    writer.write(filename, *this);
    return;
  }

  /*
  void properties::write_configuration(const std::string& filename,
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

  void properties::read_configuration(const std::string& filename, uint32_t options)
  {
    read_config(filename, *this, options);
    return;
  }

  // static
  void properties::write_config(const std::string& a_filename,
                                const properties& a_props,
                                uint32_t options) {
    config writer(options);
    writer.write(a_filename, a_props);
    return;
  }

  /*
  void properties::write_config(const std::string& a_filename,
                                const properties& a_props,
                                bool a_use_smart_modulo,
                                bool a_write_public_only) {
    DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS, "Deprecated method!");
    uint32_t writer_opts = 0;
    if (a_use_smart_modulo) {
      writer_opts |= config::SMART_MODULO;
    }
    if (a_write_public_only)  {
      writer_opts |= config::SKIP_PRIVATE;
    }
    config writer(writer_opts);
    writer.write(a_filename, a_props);
    return;
  }
  */

  // static
  void properties::read_config(const std::string& filename,
                               properties& props,
                               uint32_t options) {
    config r(options);
    r.read(filename, props);
    return;
  }

  //----------------------------------------------------------------------
  // properties::config class implementation
  //

  bool properties::config::has_section_info() const
  {
    return !_section_name_.empty();
  }

  void properties::config::set_section_info(const std::string & section_name,
                                            int section_start_line_number)
  {
     _section_name_ = section_name;
     _section_start_line_number_ = section_start_line_number;
    return;
  }

  void properties::config::reset_section_info()
  {
    _section_name_.clear();
    _section_start_line_number_ = -1;
    return;
  }

  const std::string & properties::config::get_section_name() const
  {
    return _section_name_;
  }

  int properties::config::get_section_start_line_number() const
  {
    return _section_start_line_number_;
  }

  bool properties::config::has_topic() const
  {
    return !_topic_.empty();
  }

  void properties::config::set_topic(const std::string & topic)
  {
    if (!topic.empty()) {
      uint32_t validation_flags = 0;
      // validation_flags != NV_NO_HYPHEN
      DT_THROW_IF(!name_validation(topic, validation_flags),
                  std::logic_error,
                  "Keyword/topic '" << topic << "' contains forbidden characters!");
      _requested_topic_ = true;
    } else {
      _requested_topic_ = false;
    }
    _topic_ = topic;
    return;
  }

  const std::string & properties::config::get_topic() const
  {
    return _topic_;
  }

  // static
  const std::string & properties::config::lock_decorator()
  {
    static const std::string _label("const");
    return _label;
  }

  // static
  const std::string & properties::config::as_directive()
  {
    static const std::string _label("as");
    return _label;
  }

  // static
  const std::string & properties::config::in_directive()
  {
    static const std::string _label("in");
    return _label;
  }

  // static
  const std::string & properties::config::path_decorator()
  {
    static const std::string _label("path");
    return _label;
  }

  // static
  const std::string & properties::config::metacomment_prefix()
  {
    static const std::string _label("#@");
    return _label;
  }

  void properties::config::write_data(std::ostream& out,
                                      const std::string & a_prop_key,
                                      const properties::data & a_data,
                                      const std::string & /*a_unit_symbol*/,
                                      const std::string & /*a_unit_label*/,
                                      const std::string & a_comment)
  {
    std::string eol = "\n";
    if (!a_comment.empty()) {
      DT_THROW_IF(a_comment[0] != _format::COMMENT_CHAR, std::logic_error,
                  "Comment line prefix does not start with '" << _format::COMMENT_CHAR << "'!");
      eol += a_comment;
    }
    out << eol;

    bool real_with_unit = false;
    std::string unit_symbol;
    std::string unit_label;
    double      unit_value = 1.0;
    if (a_data.is_real()) {
      if (a_data.has_unit_symbol()) {
        real_with_unit = true;
        unit_symbol = a_data.get_unit_symbol();
      }
      /*
        else if (! a_unit_symbol.empty() || ! a_unit_label.empty()) {
        real_with_unit = true;
        if (! a_unit_symbol.empty()) {
        unit_symbol = a_unit_symbol;
        } else if (! a_unit_label.empty()) {
        unit_symbol = units::get_default_unit_symbol_from_label(a_unit_label);
        unit_label = a_unit_label;
        }
        }
      */
      if (!unit_symbol.empty()) {
        std::string unit_label2;
        DT_THROW_IF(!units::find_unit(unit_symbol, unit_value, unit_label2),
                    std::logic_error,
                    "Invalid unit symbol '" << unit_symbol << "'!");
        // DT_THROW_IF(unit_label2 != unit_label,
        //             std::logic_error,
        //             "No match between unit symbol '" << unit_symbol << "' and unit label '"
        //             << a_unit_label << "'!");
      }
    }
    if (real_with_unit) {
      // out << metacomment_prefix() << "enable_real_with_unit" << eol;
      write_metacomment(out, "enable_real_with_unit", "", a_comment);
    }

    if (! a_data.get_description().empty()) {
      write_metacomment(out, "description", a_data.get_description(), a_comment);
      // out << metacomment_prefix() << "description " << a_data.get_description() << eol;
    }

    // name:
    out << a_prop_key << _format::SPACE_CHAR << _format::DESC_CHAR << _format::SPACE_CHAR;

    if (a_data.is_locked()) {
      out << lock_decorator() << _format::SPACE_CHAR;
    }

    // type:
    int size = properties::data::SCALAR_SIZE;
    out << a_data.get_type_label();

    if (a_data.is_vector()) {
      size = a_data.get_size();
      out << _format::OPEN_VECTOR << a_data.get_size() << _format::CLOSE_VECTOR;
    }

    // Real :
    if (a_data.is_real()) {
      // Vectors of real with the same explicit unit sybol applied to all items :
      if (a_data.is_vector()) {
        if (! unit_symbol.empty()) {
          out << _format::SPACE_CHAR << in_directive() << _format::SPACE_CHAR << unit_symbol << _format::SPACE_CHAR;
        }
      }
      // Scalar real with the explicit unit label applied to all items :
      else {
        if (! unit_label.empty()) {
          out << _format::SPACE_CHAR << as_directive() << _format::SPACE_CHAR << unit_label << _format::SPACE_CHAR;
        }
      }
    }

    // String :
    if (a_data.is_string() && a_data.is_explicit_path()) {
      out << _format::SPACE_CHAR << as_directive() << _format::SPACE_CHAR << path_decorator() << _format::SPACE_CHAR;
    }
    out << _format::SPACE_CHAR;
    out << _format::ASSIGN_CHAR;

    int modulo = 1; // Default modulo (for real and string)

    if (a_data.is_boolean()) modulo = 10;

    if (a_data.is_integer()) modulo = 5;

    if (_use_smart_modulo_) {
      if ((size > 1) && (size > modulo)) {
        out << _format::SPACE_CHAR << _format::CONTINUATION_CHAR << eol;
      }
    }

    // Values:
    // For scalar or vector/array :
    for (int i = 0; i < size; i++) {
      out << _format::SPACE_CHAR;
      if (a_data.is_boolean()) datatools::io::write_boolean(out, a_data.get_boolean_value(i));

      if (a_data.is_integer()) datatools::io::write_integer(out, a_data.get_integer_value(i));

      if (a_data.is_real()) {
        double val = a_data.get_real_value(i);
        val /= unit_value;
        datatools::io::write_real_number(out, val, datatools::io::REAL_PRECISION);
        if (a_data.is_scalar() && ! unit_symbol.empty()) {
          out << _format::SPACE_CHAR << unit_symbol;
        }
      }

      if (a_data.is_string()) {
        datatools::io::write_quoted_string(out, a_data.get_string_value(i));
      }

      if (_use_smart_modulo_) {
        if ((i < (size - 1)) && ((i + 1) % modulo) == 0) {
          out << _format::SPACE_CHAR << _format::CONTINUATION_CHAR << eol;
        }
      }
    }
    out << eol;

    if (real_with_unit) {
      write_metacomment(out, "disable_real_with_unit", "", a_comment);
    }
    return;
  }

  void properties::config::write_metacomment(std::ostream& out,
                                             const std::string & tag,
                                             const std::string & value,
                                             const std::string & comment)
  {
    std::string eol = "\n";
    if (!comment.empty()) eol += comment;
    out << eol;

    out << metacomment_prefix() << tag;
    if (!value.empty()) {
      out << _format::SPACE_CHAR << value;
    }
    out << eol;
    return;
  }

  void properties::config::write(const std::string& filename,
                                 const properties& props)
  {
    std::string the_filename = filename;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(the_filename),
                  std::logic_error,
                  "Cannot resolve filename '" + filename + "'!");
    }
    std::ofstream fout(the_filename.c_str());
    DT_THROW_IF(!fout,
                std::logic_error,
                "Cannot open filename '" + filename + "' (resolved as '" + the_filename + "'!");
    write(fout, props);
    fout.close();
    return;
  }

  void properties::config::write(std::ostream& out,
                                 const properties& props)
  {
    this->write_(out, props);
    return;
  }

  void properties::config::write_(std::ostream& out,
                                  const properties& props)
  {
    if (_mode_ == MODE_HEADER_FOOTER) {
      out << "# List of configuration properties (datatools::properties)"
          << std::endl << std::endl;
    }

    if (has_topic() && _requested_topic_) {
      out << "#@topic" << _format::SPACE_CHAR << get_topic() << std::endl;
    }

    if (!props.get_description().empty()) {
      out << "#@config" << _format::SPACE_CHAR << props.get_description() << std::endl;
      out << std::endl;
    }

    for (pmap::const_iterator i = props._props_.begin();
         i != props._props_.end();
         ++i) {
      const std::string &     a_key  = i->first;
      const properties::data& a_data = i->second;

      if (_write_public_only_) {
        if (key_is_private(a_key)) continue;
      }

      write_data(out, a_key, a_data, "", "", "");
      out << std::endl;

    }

    if (_mode_ == MODE_HEADER_FOOTER) {
      out << "# End of list of configuration properties (datatools::properties)"
          << std::endl;
    }
    return;
  }


  // bool properties::config::is_debug() const {
  //   return logger::is_debug(_logging_);
  // }


  // void properties::config::set_debug(bool debug) {
  //   if (debug) {
  //     _logging_ = logger::PRIO_DEBUG;
  //   } else {
  //     _logging_ = logger::PRIO_FATAL;
  //   }
  // }

  void properties::config::init_defaults_()
  {
    _logging_ = datatools::logger::PRIO_WARNING;
    _mode_ = MODE_BARE;
    _dont_clear_ = false;
    _use_smart_modulo_ = false;
    _write_public_only_ = false;
    _current_line_number_ = 0;
    _forbid_variants_ = false;
    _forbid_includes_ = false;
    _requested_topic_ = false;
    _resolve_path_    = false;
    _section_start_line_number_ = -1;
    return;
  }

  properties::config::config(uint32_t options_,
                             const std::string & topic_,
                             const std::string & section_name_,
                             int section_start_line_number_) {
    init_defaults_();
    if (options_ & SKIP_PRIVATE) {
      _write_public_only_ = true;
    }
    if (options_ & FORBID_VARIANTS) {
      _forbid_variants_ = true;
    }
    if (options_ & FORBID_INCLUDES) {
      _forbid_includes_ = true;
    }
    if (options_ & LOG_MUTE) {
      set_logging(datatools::logger::PRIO_FATAL);
    }
    if (options_ & LOG_DEBUG) {
      set_logging(datatools::logger::PRIO_DEBUG);
    }
    if (options_ & LOG_TRACE) {
      set_logging(datatools::logger::PRIO_TRACE);
    }
    if (options_ & SMART_MODULO) {
      _use_smart_modulo_ = true;
    }
    if (options_ & HEADER_FOOTER) {
      _mode_ = MODE_HEADER_FOOTER;
    }
    if (options_ & DONT_CLEAR) {
      _dont_clear_ = true;
    }
    if (options_ & RESOLVE_PATH) {
      _resolve_path_ = true;
    }
    if (!topic_.empty()) {
      set_topic(topic_);
    }
    if (!section_name_.empty()) {
      set_section_info(section_name_, section_start_line_number_);
    }
    return;
  }

  datatools::logger::priority properties::config::get_logging() const
  {
    return _logging_;
  }

  void properties::config::set_logging(datatools::logger::priority p_)
  {
    _logging_ = p_;
    return;
  }

  properties::config::~config() {}

  void properties::config::read(const std::string & in, properties& props) {
    std::string filename = in;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(filename),
                  std::logic_error,
                  "Cannot resolve filename '" + in + "'!");
    }
    std::ifstream fin(filename.c_str());
    DT_THROW_IF(!fin,
                std::logic_error,
                "Cannot open filename '" + in + "' (resolved as '" + filename + "'!");
    set_reader_input(filename, -1);
    read_(fin, props);
    fin.close();
    _current_filename_.clear();
    _current_line_number_ = 0;
    return;
  }

  void properties::config::reset()
  {
    _current_filename_.clear();
    _topic_.clear();
    _section_name_.clear();
    init_defaults_();
    return;
  }

  void properties::config::set_reader_input(const std::string & filename, int line_count)
  {
    _current_filename_ = filename;
    if (line_count >= 0) {
      _current_line_number_ = line_count;
    }
    return;
  }

  int properties::config::get_current_line_number() const
  {
    return _current_line_number_;
  }

  void properties::config::read(std::istream& in, properties& props) {
    this->read_(in, props);
  }

  void properties::config::read_(std::istream& a_in, properties& a_props) {
    if (!_dont_clear_) {
      a_props.clear();
    }
    datatools::logger::priority logging = _logging_;
    std::string line_in;
    std::string prop_topic;
    std::string prop_config;
    std::string prop_description;
    bool property_parsing_started = false;

    bool line_goon = false;
    // 2013-04-05 FM : default is to allow unit directives for real numbers
    bool enable_real_with_unit = true;
    bool enable_variants = true;
    bool variant_trace = false;
    if (_forbid_variants_) {
      enable_variants = false;
    } else {
      if (datatools::logger::is_trace(logging)) {
        variant_trace = true;
      }
    }

    /* The variant_if directive must be placed before the property line:
     *
     *  Examples:
     *
     *
     *  #@variant_if trigger:trigger_mode/coincidence
     *
     *  #@description The submodules to be triggered in coincidence
     *  trigger.coincidence.submodules    : string[2] = "calorimeter" "tracker"
     *
     *  #@description The coincidence time gate between submodules
     *  trigger.coincidence.time_gate     : real as time = 1260 us
     *
     *  #@description The number of neighbouring sectors to be considered
     *  trigger.coincidence.sector_spread : integer = 3
     *
     *  #@variant_endif
     *
     */
    std::list<std::string> variant_if_blocks;

    /* The variant_only directive must be placed before the property line:
     *
     *  Examples:
     *
     *  #@variant_only trigger:trigger_mode/if_multiplicity
     *  #@description The multiplicity threshold of the trigger system (when the trigger runs multiplicity mode)
     *  trigger.multiplicity.threshold : integer = 3
     *
     *  #@variant_only !trigger:trigger_mode/if_multiplicity
     *  #@description The ADC threshold of the trigger system (when the trigger runs NO multiplicity mode)
     *  trigger.adc.threshold : real = 15 mV
     *
     */
    std::string variant_only;
    // The variant preprocessor:
    // for now this preprocessor uses the datatools' kernel variant repository,
    // if it has been instanciated and properly feed by user (at application startup for example):

    unsigned int vpp_flags = 0;
    if (variant_trace) {
      // Special trace print:
      vpp_flags |= configuration::variant_preprocessor::FLAG_TRACE;
    }
    configuration::variant_preprocessor vpp(vpp_flags);

    while (a_in) {
      DT_LOG_DEBUG(logging, "Loop on input stream...");
      std::string line_get;
      std::getline(a_in, line_get);
      _current_line_number_++;

      // check if line has a continuation mark at the end:
      bool line_continue = false;
      int sz = line_get.size();

      if (sz > 0 && line_get[sz-1] == _format::CONTINUATION_CHAR) {
        line_continue = true;
        line_get = line_get.substr(0, sz - 1);
        DT_LOG_TRACE(logging,
                     "line_get='" << line_get << "'");
      }

      if (line_goon) {
        // append to previous line:
        line_in += line_get;
        DT_LOG_TRACE(logging, "Append to line='"<< line_in<< "'" );
      } else {
        // a new line
        line_in = line_get;
        DT_LOG_TRACE(logging, "New line='"<< line_in<< "'");
      }
      line_goon = false;

      // check if line should go on:
      if (line_continue) line_goon = true;

      bool allow_include = ! _forbid_includes_;
      if (!line_goon) {
        bool skip_line = false;
        std::string line = line_in;
        DT_LOG_TRACE(logging, "Line " << _current_line_number_ << " size is " << line.size());
        // check if line is blank:
        std::istringstream check_iss(line_in);
        std::string check_word;
        check_iss >> std::ws >> check_word;
        if (check_word.empty()) {
          DT_LOG_TRACE(logging, "Line " << _current_line_number_ << " is blank");
          skip_line = true;
        }

        bool parsing = true;

        // check if line is a comment:
        if (!skip_line) {
          std::istringstream iss(line);
          char c = 0;
          iss >> c;
          if (c == _format::COMMENT_CHAR) {
            // Handle meta comments:
            DT_LOG_TRACE(logging, "Line " << _current_line_number_ << " is a comment.");
            iss >> std::ws;
            std::string token;
            iss >> token;
            if (token == "@end") {
              break;
            }

            if (token == "@verbose_parsing") {
              logging = datatools::logger::PRIO_TRACE;
            } else if (token == "@mute_parsing") {
              logging = datatools::logger::PRIO_FATAL;
            } else if (token == "@enable_variants") {
              DT_PROP_CFG_READ_THROW_IF(_forbid_variants_, std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Variant directives are forbidden!");
              enable_variants = true;
            } else if (token == "@disable_variants") {
              enable_variants = false;
            } else if (token.substr(0, 9) == "@variant_") {
              // Variant support :
              DT_PROP_CFG_READ_THROW_IF(_forbid_variants_, std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Variant directives are forbidden!");
              if (token == "@variant_devel") {
                vpp.set_trace(true);
              }
              if (token == "@variant_no_devel") {
                vpp.set_trace(false);
              }
              if (token == "@variant_remove_quotes") {
                vpp.set_remove_quotes(true);
              }
              if (token == "@variant_preserve_quotes") {
                vpp.set_remove_quotes(false);
              }
              if (token == "@variant_only") {
                // DT_THROW_IF(!enable_variants, std::logic_error, "Variants are not supported!");
                std::string variant_path_rule;
                iss >> std::ws >> variant_path_rule;
                variant_only = variant_path_rule;
                DT_LOG_TRACE(logging, "Next parameter is active only with variant '" << variant_only << "'.");
              }

              if (token == "@variant_if") {
                // DT_THROW_IF(!enable_variants, std::logic_error, "Variants are not supported!");
                std::string variant_path_rule;
                iss >> std::ws >> variant_path_rule;
                variant_if_blocks.push_back(variant_path_rule);
                DT_LOG_TRACE(logging, "Open a variant if block with variant '" << variant_if_blocks.back() << "'.");
              } else if (token == "@variant_endif") {
                DT_PROP_CFG_READ_THROW_IF(!enable_variants,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Variants are not supported!");
                DT_PROP_CFG_READ_THROW_IF(variant_if_blocks.size() == 0,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "No variant conditional block is currently set!");
                std::string variant_path;
                iss >> std::ws >> variant_path;
                if (!variant_path.empty()) {
                  std::string vibr_current = variant_if_blocks.back();
                  const size_t npipe = vibr_current.find('|');
                  std::string vib_path = vibr_current;
                  if (npipe != vibr_current.npos) {
                    vib_path = vibr_current.substr(0, npipe);
                  }
                  DT_PROP_CFG_READ_THROW_IF(variant_path != vib_path,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Unmatching closing variant conditional block '"
                                            << variant_path << "'! "
                                            << "Expected variant path is '" << vib_path << "' !");
                }
                variant_if_blocks.pop_back();
                if (variant_if_blocks.size()) {
                  DT_LOG_TRACE(logging, "Current variant conditional block is '"
                               << variant_if_blocks.back() << "' (was '" << variant_path << "')");;
                }
              }

            } else if (parsing) {

              // Warn if more than one '@topic' directive is set...
              if (token == "@topic") {
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '@topic' is not allowed after first property record!");
                iss >> std::ws;
                std::string topic_desc;
                if (!prop_topic.empty()) {
                  DT_LOG_WARNING(logging, "Duplicated '@topic' directive; "
                                 << "Configuration topic '" << get_topic() << "' already loaded!");
                }
                std::getline(iss, topic_desc);
                boost::trim(topic_desc);
                remove_quotes(topic_desc);
                if (topic_desc.empty()) {
                  DT_LOG_WARNING(_logging_, "Found an empty '@topic' directive! Ignore!");
                } else {
                  if (has_topic()) {
                    DT_PROP_CFG_READ_THROW_IF(topic_desc != get_topic(),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Parsed topic '" << topic_desc << "' does not match expected topic '" << get_topic() << "'!");
                  }
                }
              }

              // Warn if more than one '@config' directive is set...
              if (token == "@config") {
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '@config' is not allowed after first property record!");
                iss >> std::ws;
                std::string config_desc;
                if (!prop_config.empty()) {
                  DT_LOG_WARNING(logging, "Duplicated '@config' directive; "
                                 << "Configuration description '" << a_props.get_description() << "' already loaded!");
                }
                std::getline(iss, config_desc);
                boost::trim(config_desc);
                if (!config_desc.empty()) {
                  prop_config = config_desc;
                  remove_quotes(config_desc);
                  a_props.set_description(config_desc);
                }
              }

              if (allow_include) {
                if (token == "@include") {
                  DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Directive '@include' is not allowed after first property record!");
                  DT_LOG_TRACE(logging, "Using '@include' directive from line '" << line << "'");
                  iss >> std::ws;
                  std::string include_desc;
                  std::getline(iss, include_desc);
                  boost::trim(include_desc);
                  if (!include_desc.empty()) {
                    std::string include_config_file;
                    if (datatools::is_quoted(include_desc, '\'')) {
                      datatools::remove_quotes(include_desc, include_config_file, '\'');
                    } else if (datatools::is_quoted(include_desc, '"')) {
                      datatools::remove_quotes(include_desc, include_config_file, '"');
                    } else {
                      include_config_file = include_desc;
                    }
                    datatools::fetch_path_with_env(include_config_file);
                    DT_LOG_TRACE(logging, "Included file is : '" << include_config_file << "'");
                    datatools::properties::read_config(include_config_file, a_props);
                    return;
                  }
                }
              }
              allow_include = false;

              if (token == "@enable_real_with_unit") {
                enable_real_with_unit = true;
              }

              if (token == "@disable_real_with_unit") {
                enable_real_with_unit = false;
              }

              if (token == "@description") {
                iss >> std::ws;
                std::string desc;
                std::getline(iss, desc);
                if (!desc.empty()) {
                  prop_description = desc;
                } else {
                  DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Empty property description !");
                }
                property_parsing_started = true;
              }
            }

            skip_line = true;
          } // comment char
        } // if (! skip_line)

        // parse line:
        if (!skip_line && parsing ) {
          DT_LOG_TRACE(logging, "Line " << _current_line_number_ << " is '" << line << "'");
          std::string line_parsing = line;
          std::size_t flag_pos = line_parsing.find_first_of(_format::ASSIGN_CHAR);
          DT_PROP_CFG_READ_THROW_IF(flag_pos == line_parsing.npos,
                                    std::logic_error,
                                    _current_filename_,
                                    _section_name_,
                                    _section_start_line_number_,
                                    _current_line_number_,
                                    "Cannot find assign symbol '" << _format::ASSIGN_CHAR << "'!");
          property_parsing_started = true;
          // parse property desc:
          std::string property_desc_str = line_parsing.substr(0, flag_pos);
          DT_LOG_TRACE(logging, "property_desc_str='" << property_desc_str << "'");
          // parse property value:
          std::string property_value_str = line_parsing.substr(flag_pos+1);
          DT_LOG_TRACE(logging, "property_value_str='" << property_value_str << "'");
          bool scalar = true;
          bool locked = false;
          int  vsize  = -1;
          char type   =  0;
          std::string prop_key;
          std::size_t desc_pos = property_desc_str.find_first_of(_format::DESC_CHAR);
          if (desc_pos == property_desc_str.npos) {
            std::istringstream key_ss(property_desc_str);
            key_ss >> std::ws >> prop_key;
            type = properties::data::TYPE_STRING_SYMBOL;
          } else {
            std::string key_str = property_desc_str.substr(0, desc_pos);
            std::istringstream key_ss(key_str);
            key_ss >> std::ws >> prop_key;
            std::string type_str = property_desc_str.substr(desc_pos + 1);
            std::string type_str2;
            std::string type_str3;
            DT_LOG_TRACE(logging, "type_str='" << type_str << "'");
            std::size_t vec_pos = type_str.find_first_of(_format::OPEN_VECTOR);
            if (vec_pos != type_str.npos) {
              scalar = false;
              type_str2 = type_str.substr(0, vec_pos);
              DT_LOG_TRACE(logging, "type_str2='" << type_str2 << "'");
              std::string vec_str = type_str.substr(vec_pos + 1);
              std::istringstream vec_ss(vec_str);
              vec_ss >> vsize;
              DT_PROP_CFG_READ_THROW_IF(!vec_ss,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Cannot find vector size for key '" << prop_key << "' at line '" << line << "' !");
              DT_PROP_CFG_READ_THROW_IF(vsize < 0,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Invalid vector size !");
              char c = 0;
              // DT_PROP_CFG_READ_THROW_IF(!vec_ss,
              //                                std::logic_error,
              //                                _current_filename_,
              //                                _section_name_,
              //                                _section_start_line_number_,
              //                                _current_line_number_,
              //                                "Cannot find expected vector size closing symbol for key '"
              //                                << prop_key
              //                                << "' at line '"
              //                                << line << "'  !");
              vec_ss >> c;
              DT_PROP_CFG_READ_THROW_IF(c !=_format:: CLOSE_VECTOR,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Invalid vector size closing symbol for key '"
                                        << prop_key
                                        << "' at line '"
                                        << line << "' !");
              std::getline(vec_ss, type_str3);
            } else {
              scalar = true;
              type_str2 = type_str;
            }
            std::istringstream type_ss(type_str2);
            std::string type_token;
            type_ss >> std::ws >> type_token >> std::ws;
            if (type_token == lock_decorator()) {
              locked = true;
              type_token.clear();
              type_ss >> std::ws >> type_token >> std::ws;
            }
            if (type_token == "boolean") {
              type = properties::data::TYPE_BOOLEAN_SYMBOL;
            } else if (type_token == "integer") {
              type = properties::data::TYPE_INTEGER_SYMBOL;
            } else if (type_token == "real") {
              type = properties::data::TYPE_REAL_SYMBOL;
            } else if (type_token == "string") {
              type = properties::data::TYPE_STRING_SYMBOL;
            } else {
              DT_PROP_CFG_READ_THROW_IF(true,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Invalid type specifier '"
                                        << type_token << "' "
                                        << "at key '"
                                        << prop_key << "' !");
            }
            if (! scalar && ! type_str3.empty()) {
              std::istringstream iss3(type_str3);
              std::string dummy;
              iss3 >> std::ws >> dummy;
              if (! dummy.empty()) {
                type_ss.clear();
                type_ss.str(type_str3);
              }
            }
            bool with_explicit_path = false;
            if (type == properties::data::TYPE_STRING_SYMBOL) {
              std::string token;
              type_ss >> std::ws >> token >> std::ws;
              if (! token.empty()) {
                if (token == as_directive()) {
                  std::string token2;
                  type_ss >> token2;
                  DT_PROP_CFG_READ_THROW_IF(token2.empty(),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Missing directive (as) for string value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                  DT_PROP_CFG_READ_THROW_IF(token2 != "path",
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Invalid token '" << token2 << "' for string value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                  with_explicit_path = true;
                }
              }
            }

            /// 2013-03-07 FM : add support for embedded unit directives in real property :
            std::string requested_unit_label;
            std::string requested_unit_symbol;
            double      requested_unit_value;
            datatools::invalidate(requested_unit_value);
            if (type == properties::data::TYPE_REAL_SYMBOL) {
              std::string token;
              type_ss >> std::ws >> token >> std::ws;
              if (! token.empty()) {
                if (token == as_directive()) {
                  type_ss >> std::ws >> requested_unit_label;
                  DT_PROP_CFG_READ_THROW_IF(requested_unit_label.empty(),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Missing unit label (ex: 'as length') for real value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                  DT_PROP_CFG_READ_THROW_IF(! units::is_unit_label_valid(requested_unit_label),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Invalid unit label '" << requested_unit_label
                                            << "' for real value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                  DT_LOG_TRACE(logging, "Unit label '"<< requested_unit_label << "' is valid !");
                } else if (token == in_directive()) {
                  type_ss >> std::ws >> requested_unit_symbol;
                  DT_PROP_CFG_READ_THROW_IF(requested_unit_symbol.empty(),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Missing unit symbol (ex: 'in millimeter') for real value with key '"
                                            << prop_key << "' at line '" << line << "' !");
                  if (! scalar) {
                    // For vectors of real:
                    DT_PROP_CFG_READ_THROW_IF(!units::find_unit(requested_unit_symbol,
                                                                requested_unit_value,
                                                                requested_unit_label),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Invalid unit symbol '" << requested_unit_symbol << "' for real value with key '"
                                              << prop_key << "' at line '" << line << "' !");
                    DT_LOG_TRACE(logging, "Unit symbol '" << requested_unit_symbol << "' is valid !");
                  } else {
                    DT_PROP_CFG_READ_THROW_IF(true,
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Directive '" << in_directive() << "' " << requested_unit_symbol
                                              << "' is not supported for scalar real value with key '"
                                              << prop_key
                                              << "' at line '"
                                              << line << "' !");
                  }
                } else {
                  DT_PROP_CFG_READ_THROW_IF(true,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Unknown directive '" << token
                                            << "' for real value with key '"
                                            << prop_key
                                            << "' at line '"
                                            << line << "' !");
                }
              } // token not emtpy
              if (! enable_real_with_unit) {
                DT_PROP_CFG_READ_THROW_IF(! requested_unit_label.empty()
                                          || ! requested_unit_symbol.empty(),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "The use of unit directives '" << token
                                          << "' is not allowed for real value with key '"
                                          << prop_key << "' at line '" << line << "' !");
              }
            } // end of REAL
            DT_LOG_TRACE(logging, "type='"<< type << "'");
            DT_LOG_TRACE(logging, "locked='" << locked << "'");
            DT_LOG_TRACE(logging, "vsize='" << vsize << "'");
            DT_LOG_TRACE(logging, "prop_description='" << prop_description << "'");
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
                                  properties::data::defaults::boolean_value());
              }
            }
            if (type == properties::data::TYPE_INTEGER_SYMBOL && !scalar) {
              if (vsize > 0) {
                v_integers.assign(vsize,
                                  properties::data::defaults::integer_value());
              }
            }
            if (type == properties::data::TYPE_REAL_SYMBOL && !scalar) {
              if (vsize > 0) {
                v_reals.assign(vsize,
                               properties::data::defaults::real_value());
              }
            }
            if (type == properties::data::TYPE_STRING_SYMBOL && !scalar) {
              if (vsize > 0) {
                v_strings.assign(vsize,
                                 properties::data::defaults::string_value());
              }
            }

            // Property is enabled by default:
            bool exhibit_property = true;
            if (variant_if_blocks.size()) {
              for (std::list<std::string>::const_iterator ivib = variant_if_blocks.begin();
                   ivib != variant_if_blocks.end();
                   ivib++) {
                bool variant_if_found = false;
                bool variant_if_reverse = false;
                const std::string & variant_if_rule = *ivib;
                command::returned_info cri = vpp.resolve_variant(variant_if_rule,
                                                                 variant_if_found,
                                                                 variant_if_reverse);
                DT_PROP_CFG_READ_THROW_IF(cri.is_failure(),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Cannot preprocess variant if block directive from '"
                                          << variant_if_rule << "' : "
                                          << cri.get_error_message());
                exhibit_property = variant_if_found;
                if (variant_if_reverse) {
                  exhibit_property = !variant_if_found;
                }
                if (!exhibit_property) {
                  // We break at first inhibited conditional block:
                  break;
                }
              }
            }

            bool variant_only_found = false;
            /* The variant_only_reverse flag reverses the condition:
             *
             *  Example:
             *
             *  #@variant_only !trigger:trigger_mode/multiplicity_mode
             *  #@description The ADC threshold of the trigger system (when the trigger runs NO multiplicity mode)
             *  trigger.adc.threshold : real = 15 mV
             *
             */
            bool variant_only_reverse = false;
            // Process the variant_only directive if it exists:
            if (exhibit_property && !variant_only.empty()) {
              command::returned_info cri = vpp.resolve_variant(variant_only,
                                                               variant_only_found,
                                                               variant_only_reverse);
              DT_PROP_CFG_READ_THROW_IF(cri.is_failure(),
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Cannot preprocess variant only directive from '"
                                        << variant_only << "' : "
                                        << cri.get_error_message());
              DT_LOG_TRACE(logging, "VPP ==> variant_only_found='" << variant_only_found << "'");
              DT_LOG_TRACE(logging, "VPP ==> variant_only_reverse='" << variant_only_reverse << "'");
            }

            std::string pv_str = property_value_str;
            boost::trim(pv_str);
            DT_LOG_TRACE(logging, "pv_str='" << pv_str << "'");
            std::string effective_property_values_line = property_value_str;
            {
              std::string effective_line;
              command::returned_info cri = vpp.preprocess(pv_str, effective_line);
              DT_PROP_CFG_READ_THROW_IF(cri.is_failure(),
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Cannot preprocess variant in '" << pv_str << "' : "
                                        << cri.get_error_message());
              DT_LOG_TRACE(logging, "VPP ==> effective_line='" << effective_line << "'");
              effective_property_values_line = effective_line;
            }
            /*
              if (scalar) {
            */

            DT_LOG_TRACE(logging, "--> effective_property_values_line='" << effective_property_values_line << "'");
            std::istringstream iss(effective_property_values_line);
            /***************
             *   BOOLEAN   *
             ***************/
            if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
              DT_LOG_TRACE(logging, "boolean: effective_property_values_line='" << effective_property_values_line << "'");
              if (scalar) {
                DT_PROP_CFG_READ_THROW_IF(!io::read_boolean(iss, a_boolean),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Cannot read boolean value from '" << effective_property_values_line << "' for key '"
                                          << prop_key << "' at line '" << line << "' !");
                // iss >> a_boolean;
                // DT_THROW_IF(!iss,
                //              std::logic_error,
                //              "Cannot read boolean value for key '"
                // << prop_key << "' at line '" << line << "' !");
              } else {
                for (int i = 0; i < vsize; ++i) {
                  bool b;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_boolean(iss, b),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read vector boolean value for key '"
                                            << prop_key << "' at line '" << line << "' !");
                  // iss >> b;
                  // DT_THROW_IF(!iss,
                  //              std::logic_error,
                  //              "Cannot read vector boolean value for key '"
                  //              << prop_key << "' at line '" << line << "' !");
                  v_booleans[i] = b;
                }
              }
            }

            /***************
             *   INTEGER   *
             ***************/
            if (type == properties::data::TYPE_INTEGER_SYMBOL) {
              if (scalar) {
                iss >> a_integer;
                  DT_PROP_CFG_READ_THROW_IF(!iss,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read integer value for key '"
                                            << prop_key << "' at line '" << line << "' !");
              } else {
                for (int i = 0; i < vsize; ++i) {
                  int k;
                  iss >> k;
                  DT_PROP_CFG_READ_THROW_IF(!iss,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read vector integer value for key '"
                                            << prop_key << "' at line '" << line << "' !");
                  v_integers[i] = k;
                }
              }
            }

            /**************
             *    REAL    *
             **************/
            bool with_explicit_unit = false;
            std::string unit_symbol;
            double      unit_value = 1.0;
            std::string unit_label;
            if (type == properties::data::TYPE_REAL_SYMBOL) {
              if (scalar) {
                // Scalar real :
                // Special mode to accept unit symbol after the real value :
                bool normal_value = true;
                DT_PROP_CFG_READ_THROW_IF(! datatools::io::read_real_number(iss, a_real, normal_value),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Cannot read vector real value for key '"
                                          << prop_key << "' at line '" << line << "' (" << a_props.get_description() << ") !");
                iss >> std::ws;
                if (! iss.eof())  {
                  std::string unit_word;
                  iss >> unit_word >> std::ws;
                  if (! unit_word.empty()) {
                    if (unit_word[0] == _format::COMMENT_CHAR) {
                      // Eat the end of the line:
                      std::string trailing_bits;
                      std::getline(iss, trailing_bits);
                    } else {
                      // Try to parse a unit symbol:
                      DT_PROP_CFG_READ_THROW_IF(! enable_real_with_unit,
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Trailing token '" << unit_word << "' is not allowed for real value with key '"
                                                << prop_key << "' at line '" << line
                                                << "'; you should activate the 'real with unit' feature to support this syntax !");
                      bool found_unit = datatools::units::find_unit(unit_word,
                                                                    unit_value,
                                                                    unit_label);
                      DT_PROP_CFG_READ_THROW_IF(! found_unit,
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Invalid unit symbol for key '"
                                                << prop_key
                                                << "' in line '"
                                                << line
                                                << "' !");
                      DT_PROP_CFG_READ_THROW_IF(! requested_unit_label.empty()
                                                && unit_label != requested_unit_label,
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Unit symbol '" << unit_word << "' in not compatible with requested unit label '"
                                                << requested_unit_label << "' for real scalar property with key '"
                                                << prop_key << "' at line '" << line << "' !");
                      a_real *= unit_value;
                      with_explicit_unit = true;
                      unit_symbol = unit_word;
                    }
                  } // End of unit symbol parsing.
                }
              } else {
                // Vector real:
                for (int i = 0; i < vsize; ++i) {
                  double x;
                  bool normal_value;
                  DT_PROP_CFG_READ_THROW_IF(! datatools::io::read_real_number(iss, x, normal_value),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read vector real value for key '"
                                            << prop_key << "' at line '" << line << "' !");
                  if (enable_real_with_unit && datatools::is_valid(requested_unit_value)) {
                    x *= requested_unit_value;
                    with_explicit_unit = true;
                    unit_symbol = requested_unit_symbol;
                  }
                  v_reals[i] = x;
                }
                // if (requested_unit_symbol.empty()) {
                //   with_explicit_unit = true;
                // }
              }
            } // Real

            /**************
             *   STRING   *
             **************/
            if (type == properties::data::TYPE_STRING_SYMBOL) {
              if (scalar) {
                DT_PROP_CFG_READ_THROW_IF(!io::read_quoted_string(iss, a_string),
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Cannot read string value for key '"
                                          << prop_key << "' at line '" << line << "' !");
              } else {
                for (int i = 0; i < vsize; ++i) {
                  std::string str;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_quoted_string(iss, str),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read string value for key '"
                                            << prop_key << "' at line '" << line << "' !");
                  v_strings[i] = str;
                }
              }
            }

            {
              // Analyse the end of the line and search for spurious trailing bits:
              std::string trailing_bits;
              std::getline(iss, trailing_bits);
              boost::trim(trailing_bits);
              if (trailing_bits.length() > 0) {
                if (trailing_bits[0] != _format::COMMENT_CHAR) {
                  std::ostringstream message_oss;
                  if (!_current_filename_.empty()) {
                    message_oss << "File '" << _current_filename_ << "': ";
                  }
                  if (_section_name_.empty()) {
                    if (_current_line_number_ > 0) {
                      message_oss << "Line #" << _current_line_number_ << ": ";
                    }
                  } else {
                    message_oss << "Section '" << _section_name_ << "'";
                    if (_section_start_line_number_ > 0) {
                      message_oss << " starting at line #" << _section_start_line_number_;
                    }
                    message_oss << ": ";
                  }
                  message_oss << "There are unprocessed trailing characters '" << trailing_bits << "' after value record of property '"
                              << prop_key << "'!";
                  DT_LOG_WARNING(_logging_,message_oss.str());
                }
              }
            }

            bool store_it = exhibit_property;
            if (store_it) {
              if (! variant_only.empty()) {
                store_it = variant_only_found;
                if (variant_only_reverse) {
                  store_it = !variant_only_found;
                }
              }
            }

            if (store_it) {
              // scalar :
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

              // vector :
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

              // Special flags:
              if (type == properties::data::TYPE_STRING_SYMBOL && with_explicit_path) {
                a_props.set_explicit_path(prop_key, true);
              }
              if (type == properties::data::TYPE_REAL_SYMBOL && with_explicit_unit) {
                a_props.set_explicit_unit(prop_key, true);
              }
              if (type == properties::data::TYPE_REAL_SYMBOL && !unit_symbol.empty()) {
                a_props.set_unit_symbol(prop_key, unit_symbol);
              }
              prop_description.clear();
            } // if (store_it)

            // Clear current requested variant only :
            if (! variant_only.empty()) {
              variant_only.clear();
            }
            variant_only_reverse = false;
          }
        } // !skip_line && parsing
      } // if (! line_goon)
    } // while (*_in)
    DT_PROP_CFG_READ_THROW_IF(variant_if_blocks.size() > 0,
                              std::logic_error,
                              _current_filename_,
                              _section_name_,
                              _section_start_line_number_,
                              _current_line_number_,
                              "Unclosed variant conditional block '" << variant_if_blocks.back() << "'!");
    return;
  }


  std::string properties::build_property_key(const std::string& prefix,
                                             const std::string& subkey) {
    std::string s(prefix);
    s += '.';
    s += subkey;
    return s;
  }


  void properties::export_to_string_based_dictionary(std::map<std::string, std::string>& dict,
                                                     bool quoted_strings) const {
    for (pmap::const_iterator iter = _props_.begin();
         iter != _props_.end();
         ++iter) {
      const std::string& prop_key = iter->first;
      std::ostringstream valoss;
      const data& a_data = iter->second;

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
          if (quoted_strings) valoss << '"';
          valoss << a_data.get_string_value(i);
          if (quoted_strings) valoss << '"';
        }
      }
      if (a_data.is_vector()) valoss << ')';

      dict[prop_key] = valoss.str();
    }
  }

  // Specialization :
  template <>
  bool check_serial_tag<properties>(const std::string stag_,
                                    const std::string /*alt_tag_*/,
                                    boost::enable_if< has_bsts<properties> >::type* /*dummy*/) {
    if (stag_ == properties::SERIAL_TAG) return true;
    if (stag_ == ::datatools::backward_serial_tag<properties> (0)) return true;
    if (stag_ == "datatools:utils::properties") return true;
    return false;
  }


} // end of namespace datatools
