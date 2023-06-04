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
#include <datatools/kernel.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/io.h>
#include <datatools/file_include.h>

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(::datatools::properties,
                                                      "datatools::properties")

// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(::datatools::properties,
                                                               "datatools::utils::properties")

#define DT_PROP_CFG_READ_THROW_IF(Condition, ExceptionType,             \
                                  FileName,                             \
                                  SectionName, SectionLineNumber,       \
                                  LineNumber, Message)                  \
  {                                                                     \
    if (Condition) {                                                    \
      std::stringstream sDT_THROW_IF_ONLY;                              \
      sDT_THROW_IF_ONLY << "[" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << ": "; \
      if (!FileName.empty()) {                                          \
        sDT_THROW_IF_ONLY << "in file '" << FileName << "': ";          \
      }                                                                 \
      if (SectionName.empty()) {                                        \
        if (LineNumber > 0) {                                           \
          sDT_THROW_IF_ONLY << "at line #" << LineNumber << ": ";       \
        }                                                               \
      } else {                                                          \
        sDT_THROW_IF_ONLY << "in section '" << SectionName << "'";      \
        if (SectionLineNumber > 0) {                                    \
          sDT_THROW_IF_ONLY << " starting at line #" << SectionLineNumber; \
        }                                                               \
        sDT_THROW_IF_ONLY << ": ";                                      \
      }                                                                 \
      sDT_THROW_IF_ONLY << Message << "]";                              \
      throw ExceptionType(sDT_THROW_IF_ONLY.str());                     \
    }                                                                   \
  }                                                                     \
  /**/

#define DT_PROP_CFG_READ_LOG_DEBUG(LogLevel,FileName,                   \
                                   SectionName, SectionLineNumber,      \
                                   LineNumber, Message)                 \
  {                                                                     \
    std::stringstream sDT_LOG;                                          \
    sDT_LOG << "[" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << ": "; \
    if (!FileName.empty()) {                                            \
      sDT_LOG << "in file '" << FileName << "': ";                      \
    }                                                                   \
    if (SectionName.empty()) {                                          \
      if (LineNumber > 0) {                                             \
        sDT_LOG << "at line #" << LineNumber << ": ";                   \
      }                                                                 \
    } else {                                                            \
      sDT_LOG << "in section '" << SectionName << "'";                  \
      if (SectionLineNumber > 0) {                                      \
        sDT_LOG << " starting at line #" << SectionLineNumber;          \
      }                                                                 \
      sDT_LOG << ": ";                                                  \
    }                                                                   \
    sDT_LOG << Message << "]";                                          \
    DT_LOG_DEBUG(LogLevel, sDT_LOG.str());                              \
  }                                                                     \
  /**/

namespace {

  struct _format {
    static const char OPEN_VECTOR  = '['; ///< Open section character
    static const char ASSIGN_CHAR  = '='; ///< Assignement key/meta label character
    static const char DESC_CHAR    = ':'; ///< Type descriptor separator character
    static const char CLOSE_VECTOR = ']'; ///< Close section character
    static const char COMMENT_CHAR = '#'; ///< Comment character
    static const char METACOMMENT_CHAR = '@'; ///< Comment character
    static const char SPACE_CHAR   = ' '; ///< Space character
    static const char CONTINUATION_CHAR = '\\'; ///< Continuation character

    static const std::string & boolean_label()
    {
      static const std::string _lbl("boolean");
      return _lbl;
    }

    static const std::string & integer_label()
    {
      static const std::string _lbl("integer");
      return _lbl;
    }

    static const std::string & real_label()
    {
      static const std::string _lbl("real");
      return _lbl;
    }

    static const std::string & string_label()
    {
      static const std::string _lbl("string");
      return _lbl;
    }
  };

  const char kAllowedChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.";
}

namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(properties,"datatools::properties")

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

  bool properties::data::has_forbidden_char(const std::string & str_)
  {
    return str_.find_first_of(STRING_FORBIDDEN_CHAR) != std::string::npos;
  }

  std::string properties::data::get_error_message(int error_code_)
  {
    if (error_code_ == ERROR_SUCCESS) return "success";
    if (error_code_ == ERROR_FAILURE) return "generic error";
    if (error_code_ == ERROR_BADTYPE) return "invalid type";
    if (error_code_ == ERROR_RANGE ) return "invalid index";
    if (error_code_ == ERROR_LOCK  ) return "lock error";
    return "undocumented error";
  }

  void properties::data::clear_values_()
  {
    _boolean_values_.clear();
    _integer_values_.clear();
    _real_values_.clear();
    _string_values_.clear();
    return;
  }

  int properties::data::init_values_(char type_, int size_)
  {
    int memsize = size_;
    if (size_ < 0) {
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

  int properties::data::boolean(int size_)
  {
    this->clear_values_();
    return this->init_values_(TYPE_BOOLEAN_SYMBOL, size_);
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

  int properties::data::integer(int size_)
  {
    this->clear_values_();
    return this->init_values_(TYPE_INTEGER_SYMBOL, size_);
  }

  int properties::data::real(int size_)
  {
    this->clear_values_();
    return this->init_values_(TYPE_REAL_SYMBOL, size_);
  }

  int properties::data::string(int size_)
  {
    this->clear_values_();
    return this->init_values_(TYPE_STRING_SYMBOL, size_);
  }

  bool properties::data::has_type() const
  {
    return (_flags_ & MASK_TYPE) != TYPE_NONE;
  }

  bool properties::data::is_boolean() const
  {
    return (_flags_ & MASK_TYPE) == TYPE_BOOLEAN;
  }

  int properties::data::get_type() const
  {
    return (int) (_flags_ & MASK_TYPE);
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

  bool properties::data::is_unlocked() const
  {
    return (_flags_ & MASK_LOCK) == 0;
  }

  bool properties::data::is_locked() const
  {
    return !this->is_unlocked();
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

  int32_t properties::data::get_size() const
  {
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

  int32_t properties::data::size() const
  {
    return this->get_size();
  }

  bool properties::data::empty() const
  {
    return (this->size() == 0);
  }

  properties::data::data(char type_, int size_)
  {
    _flags_  = 0;
    this->init_values_(type_, size_);
    return;
  }

  properties::data::data(bool value_, int size_)
  {
    _flags_  = 0;
    this->init_values_(TYPE_BOOLEAN_SYMBOL, size_);
    for (int i = 0; i < (int)this->size(); ++i) {
      this->set_value(value_, i);
    }
    return;
  }

  properties::data::data(int value_, int size_)
  {
    _flags_  = 0;
    this->init_values_(TYPE_INTEGER_SYMBOL, size_);
    for (int i = 0; i < (int)this->size(); ++i) {
      this->set_value(value_, i);
    }
    return;
  }

  properties::data::data(double value_, int size_)
  {
    _flags_  = 0;
    this->init_values_(TYPE_REAL_SYMBOL, size_);
    for (int i = 0; i < (int)this->size(); ++i)
      {
        this->set_value(value_, i);
      }
    return;
  }

  properties::data::data(const std::string & value_, int size_)
  {
    _flags_  = 0;
    int code=0;
    this->init_values_(TYPE_STRING_SYMBOL, size_);
    DT_THROW_IF(has_forbidden_char(value_),
                std::logic_error,
                "Forbidden char in string '" << value_ << "'!");
    for (int i = 0; i < (int)this->size(); ++i) {
      code = this->set_value(value_, i);
      DT_THROW_IF(code != ERROR_SUCCESS,
                  std::logic_error,
                  "Failure!");
    }
    return;
  }

  properties::data::data(const char* value_, int size_)
  {
    _flags_ = 0;
    this->init_values_(TYPE_STRING_SYMBOL, size_);
    std::string tmp;
    if (value_ != nullptr) {
      tmp = value_;
      DT_THROW_IF(has_forbidden_char(tmp),
                  std::logic_error,
                  "Forbidden char in string '" << tmp << "'!");
    }
    for (int i = 0; i < (int)this->size(); ++i) {
      this->set_value(tmp, i);
    }
    return;
  }

  bool properties::data::index_is_valid(int a_index) const
  {
    return (a_index >= 0) && (a_index < (int)this->get_size());
  }

  int properties::data::set_value(bool value_, int a_index)
  {
    if (!this->is_boolean()) return ERROR_BADTYPE;

    if (this->is_locked()) return ERROR_LOCK;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    _boolean_values_[a_index] = value_;
    return ERROR_SUCCESS;
  }

  int properties::data::set_value(int value_, int a_index)
  {
    if (!this->is_integer()) return ERROR_BADTYPE;

    if (this->is_locked()) return ERROR_LOCK;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    _integer_values_[a_index] = value_;
    return ERROR_SUCCESS;
  }

  int properties::data::set_value(double value_, int a_index, bool a_explicit_unit)
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

  int properties::data::set_value_with_unit(double value_, int a_index, const std::string & a_unit_symbol)
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
                                  int a_index,
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
                                  int a_index,
                                  bool a_explicit_path)
  {
    return this->set_value(std::string(value_), a_index, a_explicit_path);
  }

  int properties::data::get_value(bool& value_, int a_index) const
  {
    if (!this->is_boolean()) return ERROR_BADTYPE;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    value_ = _boolean_values_[a_index];
    return ERROR_SUCCESS;
  }

  int properties::data::get_value(int& value_, int a_index) const
  {
    if (!this->is_integer()) return ERROR_BADTYPE;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    value_ = _integer_values_[a_index];
    return ERROR_SUCCESS;
  }

  int properties::data::get_value(double& value_, int a_index) const
  {
    if (!this->is_real()) return ERROR_BADTYPE;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    value_ = _real_values_[a_index];
    return ERROR_SUCCESS;
  }

  int properties::data::get_value(std::string & value_, int a_index) const
  {
    if (!this->is_string()) return ERROR_BADTYPE;

    if (!this->index_is_valid(a_index)) return ERROR_RANGE;

    value_ = _string_values_[a_index];
    return ERROR_SUCCESS;
  }

  bool properties::data::get_boolean_value(int a_index) const
  {
    bool value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not boolean!");
    return value;
  }

  int properties::data::get_integer_value(int a_index) const
  {
    int value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not integer!");
    return value;
  }

  double properties::data::get_real_value(int a_index) const
  {
    double value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not real!");
    return value;
  }

  std::string properties::data::get_string_value(int a_index) const
  {
    std::string value;
    DT_THROW_IF(this->get_value(value, a_index) != ERROR_SUCCESS,
                std::logic_error,
                "Type is not string!");
    return value;
  }

  std::string properties::data::get_type_label() const
  {
    if (this->is_boolean()) return _format::boolean_label();
    if (this->is_integer()) return _format::integer_label();
    if (this->is_real())    return _format::real_label();
    if (this->is_string())  return _format::string_label();
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
    return;
  }

  void properties::data::tree_dump(std::ostream& a_out,
                                   const std::string & a_title,
                                   const std::string & a_indent,
                                   bool a_inherit) const
  {
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
          a_out << " <invalid unit symbol>";
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
  const std::string & properties::private_property_prefix()
  {
    static std::string prefix;
    if (prefix.empty()) prefix = "__";
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
    aux_.clear();
    std::vector<std::string> desc_parts;
    boost::split(desc_parts, _description_, boost::is_any_of(";"));
    if (desc_parts.size() > 1) {
      for (std::size_t iaux = 1; iaux < desc_parts.size(); iaux++) {
        std::string aux_desc = desc_parts[iaux];
        boost::trim(aux_desc);
        if (!aux_desc.empty()) {
          aux_.push_back(aux_desc);
        }
      }
    }
    return !aux_.empty();
  }

  int32_t properties::size() const
  {
    return _props_.size();
  }

  bool properties::empty() const
  {
    return _props_.empty();
  }


  properties::properties(const std::string & description_)
  {
    this->set_description(description_);
    return;
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

  void properties::erase_all_starting_with(const std::string & prefix_)
  {
    keys_col_type local_keys;
    this->keys_starting_with(local_keys, prefix_);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i){
      this->erase(*i);
    }
    return;
  }

  void properties::erase_all_not_starting_with(const std::string & prefix_)
  {
    keys_col_type local_keys;
    keys_not_starting_with(local_keys, prefix_);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i){
      this->erase(*i);
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
    keys_col_type local_keys;
    this->keys_starting_with(local_keys, prop_key_prefix_);
    for (keys_col_type::const_iterator i = local_keys.begin(); i !=  local_keys.end(); ++i) {
      auto & ptmp = const_cast<properties &>(*this);
      std::string new_key = *i;
      boost::replace_first(new_key, prop_key_prefix_, a_new_prefix);
      props_._props_[new_key] = ptmp._props_[*i];
    }
    return;
  }

  void properties::export_all(properties & props_) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys(local_keys);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      auto& ptmp = const_cast<properties &>(*this);
      props_._props_[*i] = ptmp._props_[*i];
    }
    return;
  }

  void properties::export_all_adding_prefix(properties & props_,
                                            const std::string & prefix_) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys(local_keys);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      auto& ptmp = const_cast<properties &>(*this);
      std::ostringstream new_key_oss;
      new_key_oss << prefix_ << *i;
      props_._props_[new_key_oss.str()] = ptmp._props_[*i];
    }
    return;
  }

  void properties::export_starting_with(properties & props_,
                                        const std::string & prefix_) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys_starting_with(local_keys, prefix_);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      auto& ptmp = const_cast<properties &>(*this);
      props_._props_[*i] = ptmp._props_[*i];
    }
    return;
  }

  void properties::export_not_starting_with(properties & props_,
                                            const std::string & prefix_) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys_not_starting_with(local_keys, prefix_);
    for (keys_col_type::const_iterator i = local_keys.begin(); i !=  local_keys.end(); ++i) {
      auto& ptmp = const_cast<properties &>(*this);
      props_._props_[*i] = ptmp._props_[*i];
    }
    return;
  }

  void properties::erase_all_ending_with(const std::string & suffix)
  {
    keys_col_type local_keys;
    this->keys_ending_with(local_keys, suffix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      this->erase(*i);
    }
    return;
  }

  void properties::erase_all_not_ending_with(const std::string & suffix)
  {
    keys_col_type local_keys;
    this->keys_not_ending_with(local_keys, suffix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      this->erase(*i);
    }
    return;
  }

  void properties::export_ending_with(properties & props_,
                                      const std::string & suffix) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys_ending_with(local_keys, suffix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i != local_keys.end(); ++i) {
      auto& ptmp = const_cast<properties &>(*this);
      props_._props_[*i] = ptmp._props_[*i];
    }
    return;
  }

  void properties::export_not_ending_with(properties & props_,
                                          const std::string & suffix) const
  {
    DT_THROW_IF(this == &props_,
                std::logic_error,
                "Self export is not allowed !");
    keys_col_type local_keys;
    this->keys_not_ending_with(local_keys, suffix);
    for (keys_col_type::const_iterator i = local_keys.begin(); i !=  local_keys.end(); ++i) {
      auto& ptmp = const_cast<properties &>(*this);
      props_._props_[*i] = ptmp._props_[*i];
    }
    return;
  }

  void properties::erase_all()
  {
    _props_.clear();
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

  void properties::clear()
  {
    _props_.clear();
    return;
  }

  void properties::reset()
  {
    this->set_description("");
    _props_.clear();
    return;
  }

  void properties::keys_not_starting_with(std::vector<std::string>& some_keys,
                                          const std::string & prefix_) const
  {
    DT_THROW_IF(prefix_.empty(),
                std::logic_error,
                "Empty key prefix argument !");
    size_t n = prefix_.size();
    for (const auto& p : _props_) {
      bool push = true;
      if (p.first.substr(0, n) == prefix_) push = false;
      if (push) some_keys.push_back(p.first);
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
    size_t n = prefix_.size();
    for (const auto& p : _props_) {
      if (p.first.size() < n) continue;
      if (p.first.substr(0, n) == prefix_) {
        some_keys.push_back(p.first);
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
    size_t n = suffix.size();
    for (const auto& p : _props_) {
      bool push = true;
      if (p.first.substr(p.first.size() - n, p.first.size()) == suffix) {
        push = false;
      }
      if (push) prop_keys.push_back(p.first);
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
                "Empty key suffix argument in properties described by '" << get_description() << "' !");
    size_t n = suffix.size();
    for (const auto& p : _props_ ) {
      if (p.first.size() < n) continue;
      if (p.first.substr(p.first.size()-n, p.first.size()) == suffix) {
        prop_keys.push_back(p.first);
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
    this->compute_keys(skeys);
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
    for (const auto& p : _props_) {
      some_keys_.push_back(p.first);
    }
    return;
  }

  void properties::compute_keys(std::set<std::string> & some_keys_) const
  {
    for (const auto& p : _props_) {
      some_keys_.insert(p.first);
    }
    return;
  }

  const properties::data &
  properties::get(const std::string & prop_key) const
  {
    auto found = _props_.find(prop_key);
    DT_THROW_IF(found == _props_.end(),
                std::logic_error,
                "Property '" << prop_key << "' does not exist in properties described by '" << get_description() << "' !");
    const data & pd = found->second;
    return pd;
  }

  void properties::store(const std::string & prop_key, const data & value)
  {
    this->_check_nokey_(prop_key);
    this->_validate_key_(prop_key);
    _props_[prop_key] = value;
    return;
  }

  const std::string & properties::key(int key_index_) const
  {
    int key_count = 0;
    auto iter = _props_.begin();
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

  void properties::lock(const std::string & a_key)
  {
    key_lock(a_key);
    return;
  }

  void properties::unlock(const std::string & a_key)
  {
    key_unlock(a_key);
    return;
  }

  bool properties::is_locked(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_locked();
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
    if (a_key.size() < 2) return false;
    return a_key.substr(0, private_property_prefix().size())
      == private_property_prefix();
  }

  bool properties::key_is_public(const std::string & a_key)
  {
    return !properties::key_is_private(a_key);
  }

  bool properties::is_private(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return this->key_is_private(a_key);
  }

  bool properties::is_public(const std::string & a_key) const
  {
    return !this->is_private(a_key);
  }

  int properties::get_type(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->get_type();
  }

  std::string properties::get_type_label(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->get_type_label();
  }

  bool properties::is_boolean(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_boolean();
  }

  bool properties::is_integer(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_integer();
  }

  bool properties::is_real(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_real();
  }

  bool properties::is_string(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_string();
  }

  bool properties::is_scalar(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_scalar();
  }

  bool properties::is_vector(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->is_vector();
  }

  int32_t properties::size(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->size();
  }

  int32_t properties::key_size(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->size();
  }

  bool properties::has_key(const std::string & a_key) const
  {
    if (_props_.size() == 0) return false;
    bool hk = _props_.find(a_key) != _props_.end();
    return hk;
  }

  void properties::key_lock(const std::string & a_key)
  {
    data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    data_ptr->lock();
    return;
  }

  void properties::key_unlock(const std::string & a_key)
  {
    data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    data_ptr->unlock();
    return;
  }

  const std::string &
  properties::get_key_description(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    return data_ptr->get_description();
  }

  void properties::set_key_description(const std::string & a_key,
                                       const std::string & desc_)
  {
    data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    data_ptr->set_description(desc_);
    return;
  }

  void properties::_check_nokey_(const std::string & a_key) const
  {
    DT_THROW_IF(this->has_key(a_key),
                std::logic_error,
                "Key '" << a_key << "' already used in properties described by '" << get_description() << "' !");
    return;
  }

  void properties::_check_key_(const std::string & a_key, data **a_data)
  {
    auto iter = _props_.find(a_key);
    DT_THROW_IF(_props_.find(a_key) == _props_.end(),
                std::logic_error,
                "Key '" << a_key << "' does not exist in properties described by '" << get_description() << "' !");
    *a_data = &(iter->second);
    return;
  }

  void properties::_check_key_(const std::string & a_key,
                               const data **a_data) const
  {
    auto iter = _props_.find(a_key);
    DT_THROW_IF(_props_.find(a_key) == _props_.end(),
                std::logic_error,
                "Key '" << a_key << "' does not exist in properties described by '" << get_description() << "' !");
    *a_data = &(iter->second);
    return;
  }

  void properties::store_flag(const std::string & a_key,
                              const std::string & description_,
                              bool a_lock)
  {
    this->store(a_key, true, description_, a_lock);
    return;
  }

  void properties::store(const std::string & a_key, bool value_,
                         const std::string & description_, bool a_lock)
  {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    data a_data(value_, data::SCALAR_DEF);
    a_data.set_description(description_);
    _props_[a_key] = a_data;
    if (a_lock) _props_[a_key].lock();
    return;
  }

  void properties::store(const std::string & a_key, int value,
                         const std::string & description, bool a_lock)
  {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    data a_data(value);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    if (a_lock) _props_[a_key].lock();
    return;
  }

  void properties::store(const std::string & a_key, double value,
                         const std::string & description, bool a_lock)
  {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    data a_data(value);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    if (a_lock) _props_[a_key].lock();
    return;
  }

  bool properties::is_explicit_path(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_string(),
                std::logic_error,
                "Property '" << a_key << "' is not of string type !");
    return data_ptr->is_explicit_path();
  }

  bool properties::has_unit_symbol(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    return data_ptr->has_unit_symbol();
  }

  const std::string & properties::get_unit_symbol(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    return data_ptr->get_unit_symbol();
  }

  void properties::set_unit_symbol(const std::string & a_key, const std::string & unit_symbol)
  {
    data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    DT_THROW_IF(data_ptr->set_unit_symbol(unit_symbol) != data::ERROR_SUCCESS,
                std::logic_error, "Setting unit symbol fails !");
    return;
  }

  bool properties::has_explicit_unit(const std::string & a_key) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_real(),
                std::logic_error,
                "Property '" << a_key << "' is not of real type in properties described by '" << get_description() << "' !");
    return data_ptr->has_explicit_unit();
  }

  void properties::set_explicit_path(const std::string & a_key, bool a_explicit_path)
  {
    data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    DT_THROW_IF(! data_ptr->is_string(),
                std::logic_error,
                "Property '" << a_key << "' is not of string type in properties described by '" << get_description() << "' !");
    DT_THROW_IF(data_ptr->set_explicit_path(a_explicit_path) != data::ERROR_SUCCESS,
                std::logic_error, "Setting explicit path fails !");
    return;
  }

  void properties::set_explicit_unit(const std::string & a_key, bool a_explicit_unit)
  {
    data *data_ptr = nullptr;
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

  void properties::store(const std::string & a_key, const std::string & value,
                         const std::string & description, bool a_lock)
  {
    this->_check_nokey_(a_key);
    this->_validate_key_(a_key);
    data a_data(value);
    a_data.set_description(description);
    _props_[a_key] = a_data;
    if (a_lock) _props_[a_key].lock();
    return;
  }

  void properties::store(const std::string & a_key, const char* value,
                         const std::string & description, bool a_lock)
  {
    this->store(a_key, std::string(value), description, a_lock);
    return;
  }

  void properties::store(const std::string & a_key, const data::vbool& values,
                         const std::string & description, bool a_lock)
  {
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

  void properties::store(const std::string & a_key, const data::vint& values,
                         const std::string & description, bool a_lock)
  {
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

  void properties::store(const std::string & a_key, const data::vdouble& values,
                         const std::string & description, bool a_lock)
  {
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
    return;
  }

  void properties::store(const std::string & a_key, const data::vstring& values,
                         const std::string & description, bool a_lock)
  {
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
    return;
  }

  void properties::store_paths(const std::string & a_key, const data::vstring& a_path_values,
                               const std::string & a_desc, bool a_lock)
  {
    store(a_key, a_path_values, a_desc, a_lock);
    set_explicit_path(a_key, true);
    return;
  }

  void properties::set_flag(const std::string & a_key)
  {
    this->store_flag(a_key, "", false);
    return;
  }

  void properties::unset_flag(const std::string & a_key)
  {
    this->clean(a_key);
    return;
  }

  void properties::store_boolean(const std::string & a_key, bool value,
                                 const std::string & desc, bool a_lock)
  {
    this->store(a_key, value, desc, a_lock);
    return;
  }

  void properties::store_integer(const std::string & a_key, int value,
                                 const std::string & desc, bool a_lock)
  {
    this->store(a_key, value, desc, a_lock);
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

  void properties::change_boolean(const std::string & a_key, bool value,
                                  int index)
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
                                         int index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change_integer(const std::string & a_key, int value,
                                  int index)
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
                                         int index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change_real(const std::string & a_key, double value,
                               int index)
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
                                      int index)
  {
    this->change(a_key, value, index);
    return;
  }

  void properties::change_string(const std::string & a_key,
                                 const std::string & value,
                                 int index)
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
                                        int index)
  {
    this->change_string(a_key, value, index);
    return;
  }

  void properties::change(const std::string & a_key, bool value, int index)
  {
    data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);

    int error = data_ptr->set_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot change value for boolean property '"
                << a_key << "' in properties described by '" << get_description() << "' : "
                << data::get_error_message(error) << "!");
    return;
  }

  void properties::change(const std::string & a_key, int value, int index)
  {
    data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->set_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot change value for integer property '"
                << a_key << "' in properties described by '" << get_description() << "' : "
                << data::get_error_message(error) << "!");
    return;
  }

  void properties::change(const std::string & a_key, double value, int index)
  {
    data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->set_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot change value for property '"
                << a_key << "' in properties described by '" << get_description() << "' : "
                << data::get_error_message(error) << "!");
    return;
  }

  void properties::change(const std::string & a_key, const std::string & value,
                          int index)
  {
    data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->set_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot change value for string property '"
                << a_key << "' in properties described by '" << get_description() << "' : "
                << data::get_error_message(error) << "!");
    return;
  }

  void properties::change(const std::string & a_key, const char* value,
                          int index)
  {
    std::string tmp = "";
    if (value != nullptr) tmp = value;
    this->change(a_key, tmp, index);
    return;
  }

  void properties::change(const std::string & a_key, const data::vbool& values)
  {
    data *data_ptr = nullptr;
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
    return;
  }

  void properties::change(const std::string & a_key, const data::vint& values)
  {
    data *data_ptr = nullptr;
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
    return;
  }

  void properties::change(const std::string & a_key,
                          const data::vdouble& values)
  {
    data *data_ptr = nullptr;
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
    return;
  }

  void properties::change(const std::string & a_key,
                          const data::vstring& values)
  {
    data *data_ptr = nullptr;
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
    return;
  }

  void properties::update_string(const std::string & a_key,
                                 const std::string & value)
  {
    this->update(a_key, value);
    return;
  }

  void properties::update_boolean(const std::string & a_key, bool value)
  {
    this->update(a_key, value);
    return;
  }

  int properties::fetch_integer_scalar(const std::string & name) const
  {
    return this->fetch_integer(name, 0);
  }

  int properties::fetch_integer_vector(const std::string & name,
                                       int index) const
  {
    return this->fetch_integer(name, index);
  }

  bool properties::fetch_boolean_scalar(const std::string & name) const
  {
    return this->fetch_boolean(name, 0);
  }

  bool properties::fetch_boolean_vector(const std::string & name,
                                        int index) const
  {
    return this->fetch_boolean(name, index);
  }

  double properties::fetch_real_scalar(const std::string & name) const
  {
    return this->fetch_real(name, 0);
  }

  double properties::fetch_real_vector(const std::string & name,
                                       int index) const
  {
    return this->fetch_real(name, index);
  }

  void properties::update_flag(const std::string & a_key)
  {
    this->update(a_key, true);
    return;
  }

  std::string properties::fetch_string_scalar(const std::string & name) const
  {
    return this->fetch_string(name, 0);
  }

  std::string properties::fetch_string_vector(const std::string & name,
                                              int index) const
  {
    return this->fetch_string(name, index);
  }

  std::string properties::fetch_path_scalar(const std::string & name) const
  {
    return this->fetch_path(name, 0);
  }

  std::string properties::fetch_path_vector(const std::string & name,
                                            int index) const
  {
    return this->fetch_path(name, index);
  }

  void properties::update(const std::string & a_key, bool value)
  {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
    return;
  }

  void properties::update(const std::string & a_key, int value)
  {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
  }

  void properties::update(const std::string & a_key, double value)
  {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
    return;
  }

  void properties::update_with_explicit_unit(const std::string & a_key, double value)
  {
    update_real_with_explicit_unit(a_key, value);
    return;
  }

  void properties::update_real_with_explicit_unit(const std::string & a_key, double value)
  {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
    this->set_explicit_unit(a_key, true);
    return;
  }

  void properties::update(const std::string & a_key, const std::string & value)
  {
    if (this->has_key(a_key)) {
      this->change(a_key, value);
    } else {
      this->store(a_key, value);
    }
    return;
  }

  void properties::update(const std::string & a_key, const char* value)
  {
    std::string tmp = "";
    if (value != nullptr) tmp = value;
    if (this->has_key(a_key)) {
      this->change(a_key, tmp);
    } else {
      this->store(a_key, tmp);
    }
    return;
  }

  void properties::update(const std::string & a_key,
                          const data::vbool& values)
  {
    if (this->has_key(a_key)) {
      this->change(a_key, values);
    } else {
      this->store(a_key, values);
    }
    return;
  }

  void properties::update(const std::string & a_key, const data::vint& values)
  {
    if (this->has_key(a_key)) {
      this->change(a_key, values);
    } else {
      this->store(a_key, values);
    }
    return;
  }

  void properties::update(const std::string & a_key,
                          const data::vdouble& values)
  {
    if (this->has_key(a_key)) {
      this->change(a_key, values);
    } else {
      this->store(a_key, values);
    }
    return;
  }

  void properties::update(const std::string & a_key,
                          const data::vstring& values)
  {
    if (this->has_key(a_key)) {
      this->change(a_key, values);
    } else {
      this->store(a_key, values);
    }
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

  void properties::fetch(const std::string & a_key, bool& value,
                         int index) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->get_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot fetch boolean value from property '"
                << a_key << "' in properties described by '" << get_description() << "': "
                << data::get_error_message(error) << " !");
    return;
  }

  void properties::fetch(const std::string & a_key, int& value,
                         int index) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->get_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot fetch integer value from property '"
                << a_key << "' in properties described by '" << get_description() << "': "
                << data::get_error_message(error) << " !");
    return;
  }

  void properties::fetch(const std::string & a_key, double& value,
                         int index) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->get_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot fetch real value from property '"
                << a_key << "' in properties described by '" << get_description() << "': "
                << data::get_error_message(error) << " !");
    return;
  }

  void properties::fetch(const std::string & a_key, std::string & value,
                         int index) const
  {
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    int error = data_ptr->get_value(value, index);
    DT_THROW_IF(error != data::ERROR_SUCCESS,
                std::logic_error,
                "Cannot fetch string value from property '"
                << a_key << "' in properties described by '" << get_description() << "': "
                << data::get_error_message(error) << " !");
    return;
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

  bool properties::fetch_boolean(const std::string & a_key, int index) const
  {
    bool value;
    this->fetch(a_key, value, index);
    return value;
  }

  int properties::fetch_integer(const std::string & a_key, int index) const
  {
    int value;
    this->fetch(a_key, value, index);
    return value;
  }

  int properties::fetch_range_integer(const std::string & a_key, int min_, int max_, int index) const
  {
    int value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(value < min_ || value > max_, std::range_error,
                "Out of [" << min_ << ";" << max_ << "] range integer value [" << value << "]!");
    return value;
  }

  unsigned int properties::fetch_positive_integer(const std::string & a_key, int index) const
  {
    int value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(value < 0, std::domain_error, "Unexpected integer negative value [" << value << "]!");
    return (unsigned int) value;
  }

  unsigned int properties::fetch_strict_positive_integer(const std::string & a_key, int index) const
  {
    int value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(value <= 0, std::domain_error, "Unexpected integer negative/zero value [" << value << "]!");
    return (unsigned int) value;
  }

  double properties::fetch_real_with_explicit_dimension(const std::string & a_key,
                                                        const std::string & a_dimension,
                                                        int index) const
  {
    double q = fetch_real_with_explicit_unit(a_key, index);
    double unit_value = 1.0;
    std::string unit_symbol;
    std::string unit_label;
    DT_THROW_IF(!units::find_unit(get_unit_symbol(a_key), unit_value, unit_label), std::logic_error, "Missing unit !");
    DT_THROW_IF(unit_label != a_dimension,
                std::logic_error,
                "Property '" << a_key << "' has no '" << a_dimension << "' unit; found '" << unit_label << "'!");
    return q;
  }

  double properties::fetch_real_with_explicit_unit(const std::string & a_key,
                                                   int index) const
  {
    double value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(!has_explicit_unit(a_key),
                std::logic_error,
                "Property '" << a_key << "' has no explicit unit!");
    return value;
  }

  double properties::fetch_dimensionless_real(const std::string & a_key,
                                              int index) const
  {
    double value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(has_explicit_unit(a_key),
                std::logic_error,
                "Property '" << a_key << "' is not dimensionless!");
    return value;
  }

  double properties::fetch_real(const std::string & a_key, int index) const
  {
    double value;
    this->fetch(a_key, value, index);
    return value;
  }

  char properties::fetch_one_character(const std::string & a_key, int index) const
  {
    std::string value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(value.length() != 1, std::logic_error,
                "String property named '" << a_key << "' with value '" << value << "' does not contain one single character!");
    return value[0];
  }

  std::string properties::fetch_string(const std::string & a_key,
                                       int index) const
  {
    std::string value;
    this->fetch(a_key, value, index);
    return value;
  }

  std::string properties::fetch_path(const std::string & a_key,
                                     int index) const
  {
    std::string value;
    this->fetch(a_key, value, index);
    DT_THROW_IF(!fetch_path_with_env(value),
                std::logic_error,
                "Property '" << a_key << "' cannot be interpreted as a valid path string !");
    return value;
  }

  void properties::fetch(const std::string & a_key, data::vbool& values) const
  {
    const data *data_ptr = nullptr;
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
    return;
  }

  void properties::fetch(const std::string & a_key, data::vint& values) const
  {
    const data *data_ptr = nullptr;
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
    return;
  }

  void properties::fetch_dimensionless(const std::string & a_key,
                                       data::vdouble& values) const
  {
    this->fetch(a_key, values);
    DT_THROW_IF(has_explicit_unit(a_key),
                std::logic_error,
                "Property '" << a_key << "' is not dimensionless!");
    return;
  }

  void properties::fetch(const std::string & a_key,
                         data::vdouble& values) const
  {
    const data *data_ptr = nullptr;
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
    return;
  }

  void properties::fetch(const std::string & a_key,
                         data::vstring& values) const
  {
    const data *data_ptr = nullptr;
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
    return;
  }

  void properties::fetch(const std::string & a_key,
                         std::set<std::string>& values,
                         bool allow_duplication_) const
  {
    std::vector<std::string> vvalues;
    this->fetch(a_key, vvalues);

    for (const auto& value : vvalues) {
      DT_THROW_IF(values.count(value) != 0 && ! allow_duplication_,
                  std::logic_error,
                  "Duplicated string value '" << value << "' at key '" << a_key << "'!");
      values.insert(value);
    }
    return;
  }

  void properties::fetch(const std::string & a_key,
                         std::set<int>& values,
                         bool allow_duplication_) const
  {
    std::vector<int> vvalues;
    this->fetch(a_key, vvalues);
    for (const auto& value : vvalues) {
      DT_THROW_IF(values.count(value) != 0 && ! allow_duplication_,
                  std::logic_error,
                  "Duplicated integer value [" << value << "] at key '" << a_key << "'!");
      values.insert(value);
    }
    return;
  }

  void properties::fetch_unique_ordered(const std::string & key_,
                                        std::vector<std::string> & values_) const
  {
    values_.clear();
    std::vector<std::string> vvalues;
    this->fetch(key_, vvalues);
    values_.reserve(vvalues.size());
    std::set<std::string> records;
    for (const auto& value : vvalues) {
      DT_THROW_IF(records.count(value), std::logic_error,
                  "Duplicated string value [" << value << "] at key '" << key_ << "'!");
      records.insert(value);
      values_.push_back(value);
    }
    return;
  }

  void properties::fetch_positive(const std::string & a_key,
                                  std::set<unsigned int>& values,
                                  bool allow_duplication_) const
  {
    std::vector<int> vvalues;
    this->fetch(a_key, vvalues);

    for (int value : vvalues) {
      DT_THROW_IF(value < 0,
                  std::logic_error,
                  "Unauthorized negative integer value [" << value << "] at key '" << a_key << "'!");
      DT_THROW_IF(values.count(value) != 0 && ! allow_duplication_,
                  std::logic_error,
                  "Duplicated integer value [" << value << "] at key '" << a_key << "'!");
      values.insert(static_cast<unsigned int>(value));
    }
    return;
  }

  std::string properties::key_to_string(const std::string & a_key) const
  {
    if (!this->has_key(a_key)) return "";
    std::ostringstream oss;
    const data *data_ptr = nullptr;
    this->_check_key_(a_key, &data_ptr);
    data_ptr->to_string(oss);
    return oss.str();
  }

  std::string
  properties::key_to_property_string(const std::string & key_) const
  {
    if (!this->has_key(key_)) return "";
    std::ostringstream oss;
    const data *data_ptr = nullptr;
    this->_check_key_(key_, &data_ptr);
    oss << key_ << '=';
    data_ptr->to_string(oss);
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
    config writer(options_);
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
    config writer(options_);
    writer.write(filename_, props_);
    return;
  }

  // static
  void properties::read_config(const std::string & filename_,
                               properties & props_,
                               uint32_t options_)
  {
    config r(options_);
    r.read(filename_, props_);
    return;
  }

  //----------------------------------------------------------------------
  // properties::config class implementation
  //

  bool properties::config::has_section_info() const
  {
    return !_section_name_.empty();
  }

  void properties::config::set_section_info(const std::string & section_name_,
                                            int section_start_line_number_)
  {
    _section_name_ = section_name_;
    _section_start_line_number_ = section_start_line_number_;
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

  void properties::config::set_topic(const std::string & topic_)
  {
    if (!topic_.empty()) {
      uint32_t validation_flags = 0;
      // validation_flags != NV_NO_HYPHEN
      DT_THROW_IF(!name_validation(topic_, validation_flags),
                  std::logic_error,
                  "Keyword/topic '" << topic_ << "' contains forbidden characters!");
      _requested_topic_ = true;
    } else {
      _requested_topic_ = false;
    }
    _topic_ = topic_;
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

  void properties::config::write_data(std::ostream & out_,
                                      const std::string & prop_key_,
                                      const properties::data & data_,
                                      const std::string & /* unit_symbol_ */,
                                      const std::string & /* unit_label_ */,
                                      const std::string & comment_)
  {
    std::string eol = "\n";
    if (!comment_.empty()) {
      DT_THROW_IF(comment_[0] != _format::COMMENT_CHAR, std::logic_error,
                  "Comment line prefix does not start with '" << _format::COMMENT_CHAR << "'!");
      eol += comment_;
    }

    bool real_with_unit = false;
    std::string unit_symbol;
    std::string unit_label;
    double      unit_value = 1.0;
    if (data_.is_real()) {
      if (data_.has_unit_symbol()) {
        real_with_unit = true;
        unit_symbol = data_.get_unit_symbol();
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
      write_metacomment(out_, "enable_real_with_unit", "", comment_);
    }

    if (! data_.get_description().empty()) {
      write_metacomment(out_, "description", data_.get_description(), comment_);
    }

    // name:
    out_ << prop_key_ << _format::SPACE_CHAR << _format::DESC_CHAR << _format::SPACE_CHAR;

    if (data_.is_locked()) {
      out_ << lock_decorator() << _format::SPACE_CHAR;
    }

    // type:
    int size = properties::data::SCALAR_SIZE;
    out_ << data_.get_type_label();

    if (data_.is_vector()) {
      size = data_.get_size();
      out_ << _format::OPEN_VECTOR << data_.get_size() << _format::CLOSE_VECTOR;
    }

    // Real :
    if (data_.is_real()) {
      // Vectors of real with the same explicit unit sybol applied to all items :
      if (data_.is_vector()) {
        if (! unit_symbol.empty()) {
          out_ << _format::SPACE_CHAR << in_directive() << _format::SPACE_CHAR << unit_symbol << _format::SPACE_CHAR;
        }
      }
      // Scalar real with the explicit unit label applied to all items :
      else {
        if (! unit_label.empty()) {
          out_ << _format::SPACE_CHAR << as_directive() << _format::SPACE_CHAR << unit_label << _format::SPACE_CHAR;
        }
      }
    }

    // String :
    if (data_.is_string() && data_.is_explicit_path()) {
      out_ << _format::SPACE_CHAR << as_directive() << _format::SPACE_CHAR << path_decorator() << _format::SPACE_CHAR;
    }
    out_ << _format::SPACE_CHAR;
    out_ << _format::ASSIGN_CHAR;

    int modulo = 1; // Default modulo (for real and string)

    if (data_.is_boolean()) modulo = 10;

    if (data_.is_integer()) modulo = 5;

    if (_use_smart_modulo_) {
      if ((size > 1) && (size > modulo)) {
        out_ << _format::SPACE_CHAR << _format::CONTINUATION_CHAR << std::endl;
      }
    }

    // Values:
    // For scalar or vector/array :
    for (int i = 0; i < size; i++) {
      out_ << _format::SPACE_CHAR;
      if (data_.is_boolean()) datatools::io::write_boolean(out_, data_.get_boolean_value(i));

      if (data_.is_integer()) datatools::io::write_integer(out_, data_.get_integer_value(i));

      if (data_.is_real()) {
        double val = data_.get_real_value(i);
        val /= unit_value;
        datatools::io::write_real_number(out_, val, datatools::io::REAL_PRECISION);
        if (data_.is_scalar() && ! unit_symbol.empty()) {
          out_ << _format::SPACE_CHAR << unit_symbol;
        }
      }

      if (data_.is_string()) {
        datatools::io::write_quoted_string(out_, data_.get_string_value(i));
      }

      if (_use_smart_modulo_) {
        if ((i < (size - 1)) && ((i + 1) % modulo) == 0) {
          out_ << _format::SPACE_CHAR << _format::CONTINUATION_CHAR << std::endl;
        }
      }
    }
    out_ << std::endl;

    if (real_with_unit) {
      write_metacomment(out_, "disable_real_with_unit", "", comment_);
    }
    return;
  }

  void properties::config::write_metacomment(std::ostream & out_,
                                             const std::string & tag_,
                                             const std::string & value_,
                                             const std::string & comment_)
  {
    std::string eol = "\n";
    if (!comment_.empty()) eol += comment_;
    out_ << eol;

    out_ << metacomment_prefix() << tag_;
    if (!value_.empty()) {
      out_ << _format::SPACE_CHAR << value_;
    }
    out_ << eol;
    return;
  }

  void properties::config::write(const std::string & filename_,
                                 const properties & props_)
  {
    std::string the_filename = filename_;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(the_filename),
                  std::logic_error,
                  "Cannot resolve filename '" + filename_ + "'!");
    }
    std::ofstream fout(the_filename.c_str());
    DT_THROW_IF(!fout,
                std::logic_error,
                "Cannot open filename '" + filename_ + "' (resolved as '" + the_filename + "'!");
    write(fout, props_);
    fout.close();
    return;
  }

  void properties::config::write(std::ostream & out_,
                                 const properties & props_)
  {
    this->_write_(out_, props_);
    return;
  }

  void properties::config::_write_(std::ostream & out_,
                                   const properties & props_)
  {
    if (_mode_ == MODE_HEADER_FOOTER) {
      out_ << "# List of configuration properties (datatools::properties)"
           << std::endl << std::endl;
    }

    if (!props_.get_description().empty()) {
      out_ << "#@configuration" << _format::SPACE_CHAR << props_.get_description() << std::endl;
      out_ << std::endl;
    }

    if (has_topic() and _requested_topic_) {
      out_ << "#@topic" << _format::SPACE_CHAR << get_topic() << std::endl;
    }

    for (const auto & p : props_._props_) {
      if (_write_public_only_) {
        if (key_is_private(p.first)) continue;
      }

      write_data(out_, p.first, p.second, "", "", "");
      out_ << std::endl;

    }

    if (_mode_ == MODE_HEADER_FOOTER) {
      out_ << "# End of list of configuration properties (datatools::properties)"
           << std::endl;
    }
    return;
  }


  void properties::config::_init_defaults_()
  {
    _logging_ = datatools::logger::PRIO_FATAL;
    _mode_ = MODE_BARE;
    _dont_clear_ = false;
    _use_smart_modulo_ = false;
    _write_public_only_ = false;
    _current_line_number_ = 0;
    _forbid_variants_ = false;
    _forbid_include_  = false;
    _requested_topic_ = false;
    _resolve_path_    = false;
    _allow_key_override_ = false;
    _section_start_line_number_ = -1;
    return;
  }

  properties::config::config(uint32_t options_,
                             const std::string & topic_,
                             const std::string & section_name_,
                             int section_start_line_number_)
  {
    _init_defaults_();
    if (options_ & SKIP_PRIVATE) {
      _write_public_only_ = true;
    }
    if (options_ & FORBID_VARIANTS) {
      _forbid_variants_ = true;
    }
    if (options_ & FORBID_INCLUDE) {
      _forbid_include_ = true;
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
    if (options_ & LOG_WARNING) {
      set_logging(datatools::logger::PRIO_WARNING);
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

  properties::config::~config()
  {
    return;
  }

  const file_include & properties::config::get_fi() const
  {
    return _fi_;
  }

  file_include & properties::config::grab_fi()
  {
    return _fi_;
  }

  void properties::config::set_fi(const file_include & fi_)
  {
    _fi_ = fi_;
    if (!_fi_.is_initialized()) {
      _fi_.initialize();
    }
    return;
  }

  void properties::config::read(const std::string & filename_, properties & props_)
  {
    std::string filename = filename_;
    if (_resolve_path_) {
      DT_THROW_IF(!fetch_path_with_env(filename),
                  std::logic_error,
                  "Cannot resolve filename '" + filename_ + "'!");
    }
    std::ifstream fin(filename.c_str());
    DT_THROW_IF(!fin,
                std::logic_error,
                "Cannot open filename '" + filename_ + "' (resolved as '" + filename + "'!");
    set_reader_input(filename, -1);
    _read_(fin, props_);
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
    _init_defaults_();
    return;
  }

  void properties::config::set_reader_input(const std::string & filename_, int line_count_)
  {
    _current_filename_ = filename_;
    if (line_count_ >= 0) {
      _current_line_number_ = line_count_;
    }
    return;
  }

  int properties::config::get_current_line_number() const
  {
    return _current_line_number_;
  }

  void properties::config::read(std::istream & in_, properties & props_)
  {
    this->_read_(in_, props_);
    return;
  }

  void properties::config::_read_(std::istream & in_, properties & props_)
  {
    if (!_dont_clear_) {
      props_.clear();
    }
    datatools::logger::priority logging = _logging_;
    DT_LOG_TRACE(logging, "Parsing a properties file...");
    std::string line_in;
    std::string prop_topic;
    std::string prop_config;
    std::string prop_description;
    bool property_parsing_started = false;
    bool allow_key_override = _allow_key_override_;
    // bool allow_array_override_index = true;
    bool line_goon = false;
    // 2013-04-05 FM : default is to allow unit directives for real numbers
    bool enable_real_with_unit = true;
    // File inclusion support:
    bool enable_file_inclusion = true;
    // By default, file inclusion rules are propagated to included files:
    bool include_file_propagate = true;
    bool include_file_allow_override = true;
    logger::priority fi_debug = logger::PRIO_FATAL;
    if (_forbid_include_) {
      enable_file_inclusion = false;
    }
    bool allow_fi_setup = true;
    if (_fi_.is_initialized()) {
      allow_fi_setup = false;
    }
    if (allow_fi_setup) {
      _fi_.set_include_path_env_strategy(file_include::EV_PREPEND);
    }
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
    std::vector<std::string> paths_to_be_included;
    bool do_break = false;
    while (in_) {
      DT_LOG_TRACE(logging, "Loop on input stream...");
      std::string line_get;
      std::getline(in_, line_get);
      _current_line_number_++;
      DT_LOG_TRACE(logging, "Raw line = '" << line_get << "'");
      // Check if line has a continuation mark at the end:
      bool line_continue = false;
      int sz = line_get.size();
      if (sz > 0 && line_get[sz-1] == _format::CONTINUATION_CHAR) {
        line_continue = true;
        line_get = line_get.substr(0, sz - 1);
        DT_LOG_TRACE(logging, "line_get='" << line_get << "'");
      }
      if (line_goon) {
        // Append to previous line:
        line_in += line_get;
        DT_LOG_TRACE(logging, "Append to line='"<< line_in<< "'" );
      } else {
        // A new line
        line_in = line_get;
        DT_LOG_TRACE(logging, "New full line = '" << line_in << "'");
      }
      line_goon = false;
      // Check if line should go on:
      if (line_continue) line_goon = true;
      // bool allow_include = ! _forbid_include_;
      if (!line_goon) {
        bool skip_line = false;
        std::string & line = line_in;
        DT_LOG_TRACE(logging, "Line #" << _current_line_number_ << " size is " << line.size());
        // Check if line is blank:
        std::istringstream check_iss(line_in);
        std::string check_word;
        check_iss >> std::ws >> check_word;
        if (check_word.empty()) {
          DT_LOG_TRACE(logging, "Line #" << _current_line_number_ << " is blank");
          skip_line = true;
        }
        DT_LOG_TRACE(logging, "skip_line #" << _current_line_number_ << " = " << std::boolalpha << skip_line);
        // Check if line is a comment:
        if (!skip_line) {
          DT_LOG_TRACE(logging, "Processing line #" << _current_line_number_ << " ...");
          std::istringstream iss(line);
          char c = 0;
          iss >> c;
          if (c == _format::COMMENT_CHAR) {
            // Handle comments:
            DT_LOG_TRACE(logging, "Line #" << _current_line_number_ << " is a comment.");
            iss >> std::ws;
            std::string token;
            iss >> token;
            if (token.size() > 0 && token[0] == _format::METACOMMENT_CHAR) {
              DT_LOG_TRACE(logging, "Line #" << _current_line_number_ << " is a meta-comment.");
              // Parse meta-comments:
              if (token == "@end") {
                DT_LOG_TRACE(logging, "Break the parsing loop");
                do_break = true;
                break;
              } else if (token == "@verbose_parsing") {
                DT_LOG_TRACE(logging, "Activate parsing verbosity");
                logging = datatools::logger::PRIO_TRACE;
              } else if (token == "@mute_parsing") {
                DT_LOG_TRACE(logging, "Mute parsing verbosity");
                logging = datatools::logger::PRIO_FATAL;
              } else if (token == "@enable_variants") {
                DT_LOG_TRACE(logging, "Enabling variants");
                DT_PROP_CFG_READ_THROW_IF(_forbid_variants_, std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Variant directives are forbidden!");
                enable_variants = true;
              } else if (token == "@disable_variants") {
                DT_LOG_TRACE(logging, "Disabling variants");
                enable_variants = false;
              } else if (token.substr(0, 9) == "@variant_") {
                DT_LOG_TRACE(logging, "Entering variant support");
                // Variant support :
                DT_PROP_CFG_READ_THROW_IF(_forbid_variants_, std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Variant directives are forbidden!");
                if (token == "@variant_devel") {
                  DT_LOG_TRACE(logging, "Activate variant support verbosity");
                  vpp.set_logging(logger::PRIO_TRACE);
                } else if (token == "@variant_no_devel") {
                  DT_LOG_TRACE(logging, "Mute variant support verbosity");
                  vpp.set_logging(logger::PRIO_FATAL);
                } else if (token == "@variant_remove_quotes") {
                  DT_LOG_TRACE(logging, "Set variant 'remove quotes' mode");
                  vpp.set_remove_quotes(true);
                } else if (token == "@variant_preserve_quotes") {
                  DT_LOG_TRACE(logging, "Set variant 'preserve quotes' mode");
                  vpp.set_remove_quotes(false);
                } else if (token == "@variant_only") {
                  DT_LOG_TRACE(logging, "Set a 'variant-only' conditional directive");
                  // DT_THROW_IF(!enable_variants, std::logic_error, "Variants are not supported!");
                  std::string variant_path_rule;
                  iss >> std::ws >> variant_path_rule;
                  variant_only = variant_path_rule;
                  property_parsing_started = true;
                  DT_LOG_TRACE(logging, "Next parameter is active only with variant '" << variant_only << "'.");
                } else if (token == "@variant_if") {
                  DT_LOG_TRACE(logging, "Set a 'variant-if' conditional block");
                  // DT_THROW_IF(!enable_variants, std::logic_error, "Variants are not supported!");
                  std::string variant_path_rule;
                  iss >> std::ws >> variant_path_rule;
                  variant_if_blocks.push_back(variant_path_rule);
                  property_parsing_started = true;
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
                  DT_LOG_TRACE(logging, "Close a 'variant if block' with variant '" << variant_if_blocks.back() << "'.");
                  variant_if_blocks.pop_back();
                  if (variant_if_blocks.size()) {
                    DT_LOG_TRACE(logging, "Current variant conditional block is '"
                                 << variant_if_blocks.back() << "' (was '" << variant_path << "')");;
                  }
                }
                DT_LOG_TRACE(logging, "End of a variant support");
              } else if (token == "@topic") {
                DT_LOG_TRACE(logging, "Topic directive");
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
                  // Warn if more than one '@topic' directive is set...
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
              } else if (token == "@allow_key_override") {
                bool flag_value = true;
                iss >> std::ws;
                std::string flag_repr;
                std::getline(iss, flag_repr);
                if (flag_repr == "0"
                    or flag_repr == "false"
                    or flag_repr == "no") {
                  flag_value = false;
                }
                DT_LOG_TRACE(logging, "Force allow key override to : " << std::boolalpha << flag_value);
                allow_key_override = flag_value;
              } else if (token == "@forbid_key_override") {
                bool flag_value = true;
                iss >> std::ws;
                std::string flag_repr;
                std::getline(iss, flag_repr);
                if (flag_repr == "0"
                    or flag_repr == "false"
                    or flag_repr == "no") {
                  flag_value = false;
                }
                DT_LOG_TRACE(logging, "Force forbid key override to : " << std::boolalpha << flag_value);
                allow_key_override = !flag_value;
                if (! allow_key_override) {
                  // Specific case when 'key overriding' is explicitely forbiden,
                  // it is also for file inclusion mechanism:
                  include_file_allow_override = false;
                }
              } else if (token == "@config" or token == "@configuration") {
                // Warn if more than one '@config' directive is set...
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '@config[uration]' is not allowed after first property record!");
                iss >> std::ws;
                std::string config_desc;
                if (!prop_config.empty()) {
                  DT_LOG_WARNING(logging, "Duplicated '@config[uration]' directive; "
                                 << "Configuration description '" << props_.get_description() << "' already loaded!");
                }
                std::getline(iss, config_desc);
                boost::trim(config_desc);
                if (!config_desc.empty()) {
                  prop_config = config_desc;
                  remove_quotes(config_desc);
                  props_.set_description(config_desc);
                }
              } else if (token == "@forbid_include") {
                DT_LOG_TRACE(logging, "Inhibit file inclusion");
                enable_file_inclusion = false;
              } else if (token == "@include_debug") {
                DT_LOG_TRACE(logging, "Activate file inclusion verbosity");
                fi_debug = datatools::logger::PRIO_DEBUG;
                _fi_.set_logging(fi_debug);
              } else if (token == "@include_no_propagate") {
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '" << token << "' is not allowed after first property record!");
                include_file_propagate = false;
              } else if (token == "@include_dir") {
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '" << token << "' is not allowed after first property record!");
                if (!allow_fi_setup) {
                  DT_LOG_TRACE(logging, "Ignoring directive '" << token << "'");
                } else {
                  DT_LOG_TRACE(logging, "Specify a file inclusion search directory");
                  DT_PROP_CFG_READ_THROW_IF(!enable_file_inclusion,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "File inclusion is not allowed! Directive '" << token << "' is not supported!");
                  iss >> std::ws;
                  std::string fi_dir_path;
                  uint32_t reader_flags = 0;
                  DT_THROW_IF(!io::read_quoted_string(iss, fi_dir_path, reader_flags),
                              std::logic_error,
                              (_current_filename_.empty() ? "" : "File '" + _current_filename_ + "': ") <<
                              "Line " << _current_line_number_ << ": "
                              << "Unquoted value for '@include_dir'");
                  DT_LOG_DEBUG(fi_debug, "Append include path '" << fi_dir_path << "'");
                  _fi_.append_explicit_include_path(fi_dir_path);
                }
              } else if (token == "@include_path_env") {
                DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Directive '" << token << "' is not allowed after first property record!");
                if (!allow_fi_setup) {
                  DT_LOG_TRACE(logging, "Ignoring directive '" << token << "'");
                } else {
                  DT_LOG_TRACE(logging, "Specify a file inclusion search directory environment variable");
                  DT_PROP_CFG_READ_THROW_IF(!enable_file_inclusion,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "File inclusion is not allowed! Directive '" << token << "' is not supported!");
                  iss >> std::ws;
                  std::string fi_env_name;
                  uint32_t reader_flags = 0;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_quoted_string(iss, fi_env_name, reader_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Unquoted value for '@include_path_env'");
                  DT_LOG_DEBUG(fi_debug, "Set include path env '" << fi_env_name << "'");
                  _fi_.set_include_path_env_name(fi_env_name);
                }
              } else if (token == "@include") {
                DT_LOG_TRACE(logging, "Specify a file to be included");
                DT_PROP_CFG_READ_THROW_IF(!enable_file_inclusion,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "File inclusion is not allowed! Directive '" << token << "' is not supported!");
                DT_LOG_DEBUG(logging, "Using '@include' directive from line <<<" << line << ">>>");
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
                  std::tuple<bool, std::string> fi_result = _fi_.resolve_err(include_config_file);
                  DT_PROP_CFG_READ_THROW_IF(! std::get<0>(fi_result),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot resolve include file path for '" << include_config_file << "'!");
                  std::string resolved_path = std::get<1>(fi_result);
                  DT_LOG_DEBUG(logging, "Set resolved include path '" << resolved_path << "'");
                  paths_to_be_included.push_back(resolved_path);
                  // Any include directive forbid to change the settings of the file inclusion mechanism:
                  property_parsing_started = true;
                }
              } else if (token == "@include_try") {
                DT_LOG_TRACE(logging, "Specify a file to be included if it exists");
                DT_PROP_CFG_READ_THROW_IF(!enable_file_inclusion,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "File inclusion is not allowed! Directive '" << token << "' is not supported!");
                DT_LOG_DEBUG(logging, "Using '@include' directive from line <<<" << line << ">>>");
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
                  std::tuple<bool, std::string> fi_result = _fi_.resolve_err(include_config_file);
                  if(std::get<0>(fi_result)) {
                    std::string resolved_path = std::get<1>(fi_result);
                    DT_LOG_DEBUG(logging, "Set resolved include path '" << resolved_path << "'");
                    paths_to_be_included.push_back(resolved_path);
                  } else {
                    DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                                   "Could not resolve file inclusion '" << include_config_file << "'!");
                  }
                  // Any include directive forbid to change the settings of the file inclusion mechanism:
                  property_parsing_started = true;
                }
              } else if (token == "@enable_real_with_unit") {
                DT_LOG_TRACE(logging, "Enable unit support for real properties");
                enable_real_with_unit = true;
              } else if (token == "@disable_real_with_unit") {
                DT_LOG_TRACE(logging, "Disable unit support for real properties");
                enable_real_with_unit = false;
              } else if (token == "@description" or token == "@parameter") {
                DT_LOG_TRACE(logging, "Set the description of the next property to be parsed");
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
              } else {
                DT_PROP_CFG_READ_THROW_IF(true,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Unsupported meta-comment token '" << token << "'!");
              }
            } // end of metacomments '@'
            skip_line = true;
          } // end of comments
        } // if (! skip_line)

        DT_LOG_TRACE(logging, "skip_line = " << std::boolalpha << skip_line);

        // Manage breaks:
        if (do_break) {
          DT_PROP_CFG_READ_THROW_IF(property_parsing_started,
                                    std::logic_error,
                                    _current_filename_,
                                    _section_name_,
                                    _section_start_line_number_,
                                    _current_line_number_,
                                    "Enforced break while parsing of a property has started!");
        }

        // Manage included files:
        if (paths_to_be_included.size()) {
          DT_LOG_DEBUG(_logging_, "Before inclusion : ");
          props_.print_tree(std::cerr);
          // Trigger the inclusion of external file:
          for (const std::string & inc_path : paths_to_be_included) {
            DT_LOG_DEBUG(_logging_, "About to include file with path '" << inc_path << "'");
            uint32_t reader_options = properties::config::RESOLVE_PATH;
            if (datatools::logger::is_debug(_logging_)) {
              reader_options |= properties::config::LOG_DEBUG;
            }
            properties::config reader(reader_options);
            if (include_file_propagate) {
              // Propagate the resolving rules of the local file inclusion mechanism:
              reader.set_fi(_fi_);
            }
            properties inc_conf;
            reader.read(inc_path, inc_conf);
            DT_LOG_DEBUG(_logging_, " -> Included config : ");
            if (datatools::logger::is_debug(_logging_)) {
              inc_conf.print_tree(std::cerr);
            }
            props_.merge_with(inc_conf, include_file_allow_override);
          }
          paths_to_be_included.clear();
        }

        // Parse line:
        // if (!skip_line && parsing ) {
        if (!skip_line) {
          DT_LOG_TRACE(logging, "Parsing payload line #" << _current_line_number_ << " : '" << line << "'");
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
          bool array_append = false;
          bool array_override_item = false;
          int array_override_index = -1;
          if (flag_pos > 0) {
            if (line_parsing.substr(flag_pos - 1, 2) == "+=") {
              array_append = true;
            }
          }
          // Parse property desc:
          std::string property_desc_str = line_parsing.substr(0, flag_pos - (array_append ? 1 : 0));
          DT_LOG_TRACE(logging, "property_desc_str='" << property_desc_str << "'");
          // Parse property value:
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

            {
              // Search for syntax like : 'array_foo[42]'
              int open_index_pos = -1;
              //std::size_t close_index_pos = -1;
              open_index_pos = prop_key.find('[');
              if (open_index_pos >= 0) {
                DT_PROP_CFG_READ_THROW_IF(prop_key.back() != ']',
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Invalid array item format for key '"
                                          << prop_key
                                          << "' at line '"
                                          << line << "' !");
                std::string array_prop_key = prop_key.substr(0, open_index_pos);
                std::string array_index_token = prop_key.substr(open_index_pos + 1,
                                                                prop_key.length() - array_prop_key.length() - 2);
                // std::cerr << "******** DEVEL ***" << "array_prop_key = '" << array_prop_key << "'" << std::endl;
                // std::cerr << "******** DEVEL ***" << "array_index_token = '" << array_index_token << "'" << std::endl;
                std::istringstream array_index_ss(array_index_token);
                int index = -1;
                array_index_ss >> index;
                DT_PROP_CFG_READ_THROW_IF(!array_index_ss or index < 0,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Invalid array item format from '" << array_index_token << "' for key '"
                                          << prop_key
                                          << "' at line '"
                                          << line << "' !");
                // std::cerr << "******** DEVEL ***" << "index = [" << index<< "]" << std::endl;
                array_override_item = true;
                array_override_index = index;
                prop_key = array_prop_key;
                // std::cerr << "******** DEVEL ***" << "array_override_item  = " << std::boolalpha << array_override_item << std::endl;
                // std::cerr << "******** DEVEL ***" << "array_override_index = [" << array_override_index << "]" << std::endl;
                // std::cerr << "******** DEVEL ***" << "prop_key_token       = '" << prop_key << "'" << std::endl;
              }
            }
            if (array_override_item) {
              DT_PROP_CFG_READ_THROW_IF(! allow_key_override,
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Key override mode is not allowed for property with key '"
                                        << prop_key
                                        << "' at line '"
                                        << line << "' !");
              DT_PROP_CFG_READ_THROW_IF(! props_.is_vector(prop_key),
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Key override mode is only supported for array property with key '"
                                        << prop_key
                                        << "' at line '"
                                        << line << "' !");
              DT_PROP_CFG_READ_THROW_IF(array_override_index < 0 or array_override_index >= props_.key_size(prop_key),
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Invalid array index [" << std::to_string(array_override_index) << "] for key override mode for array property with key '"
                                        << prop_key
                                        << "' at line '"
                                        << line << "' !");
            }
            DT_PROP_CFG_READ_THROW_IF(array_append and scalar,
                                      std::logic_error,
                                      _current_filename_,
                                      _section_name_,
                                      _section_start_line_number_,
                                      _current_line_number_,
                                      "Append mode is only supported for array property with key '"
                                      << prop_key
                                      << "' at line '"
                                      << line << "' !");
            std::istringstream type_ss(type_str2);
            std::string type_token;
            type_ss >> std::ws >> type_token >> std::ws;
            if (type_token == lock_decorator()) {
              locked = true;
              type_token.clear();
              type_ss >> std::ws >> type_token >> std::ws;
            }
            if (type_token == _format::boolean_label()) {
              type = properties::data::TYPE_BOOLEAN_SYMBOL;
            } else if (type_token == _format::integer_label()) {
              type = properties::data::TYPE_INTEGER_SYMBOL;
            } else if (type_token == _format::real_label()) {
              type = properties::data::TYPE_REAL_SYMBOL;
            } else if (type_token == _format::string_label()) {
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
            DT_PROP_CFG_READ_THROW_IF(array_override_item and props_.get_type_label(prop_key) != type_token,
                                      std::logic_error,
                                      _current_filename_,
                                      _section_name_,
                                      _section_start_line_number_,
                                      _current_line_number_,
                                      "Type mismatch '" << type_token << "' vs '" << props_.get_type_label(prop_key) << "' for array item override mode for key '"
                                      << prop_key
                                      << "' at line '"
                                      << line << "' !");
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
              } // Token not emtpy

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
            } // End of REAL

            {
              std::string trailing_token;
              type_ss >> std::ws >> trailing_token >> std::ws;
              DT_PROP_CFG_READ_THROW_IF(! trailing_token.empty(),
                                        std::logic_error,
                                        _current_filename_,
                                        _section_name_,
                                        _section_start_line_number_,
                                        _current_line_number_,
                                        "Unsupported trailing token '" << trailing_token
                                        << "' for key '"
                                        << prop_key << "' at line '" << line << "' !");
            }

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
            if (!scalar) {
              if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                if (vsize > 0) {
                  v_booleans.assign(vsize,
                                    properties::data::defaults::boolean_value());
                }
              }
              if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                if (vsize > 0) {
                  v_integers.assign(vsize,
                                    properties::data::defaults::integer_value());
                }
              }
              if (type == properties::data::TYPE_REAL_SYMBOL) {
                if (vsize > 0) {
                  v_reals.assign(vsize,
                                 properties::data::defaults::real_value());
                }
              }
              if (type == properties::data::TYPE_STRING_SYMBOL) {
                if (vsize > 0) {
                  v_strings.assign(vsize,
                                   properties::data::defaults::string_value());
                }
              }
            }
            // Property is enabled by default:
            bool exhibit_property = true;
            if (variant_if_blocks.size()) {
              for (std::list<std::string>::const_iterator ivib = variant_if_blocks.begin();
                   ivib != variant_if_blocks.end();
                   ivib++) {
                bool variant_if_active = false;
                bool variant_if_reverse = false;
                const std::string & variant_if_rule = *ivib;
                command::returned_info cri = vpp.resolve_variant(variant_if_rule,
                                                                 variant_if_active,
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
                exhibit_property = variant_if_active;
                if (variant_if_reverse) {
                  exhibit_property = !variant_if_active;
                }
                if (!exhibit_property) {
                  // We break at first inhibited conditional block:
                  break;
                }
              }
            }
            bool variant_only_active = false;
            /* The variant_only_reverse flag reverses the condition:
             *
             *  Example:
             *
             *  #@variant_only !trigger:trigger_mode/if_multiplicity_mode
             *  #@description The ADC threshold of the trigger system (when the trigger runs NO multiplicity mode)
             *  trigger.adc.threshold : real = 15 mV
             *
             */
            bool variant_only_reverse = false;
            // Process the variant_only directive if it exists:
            if (exhibit_property && !variant_only.empty()) {
              command::returned_info cri = vpp.resolve_variant(variant_only,
                                                               variant_only_active,
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
              DT_LOG_TRACE(logging, "VPP ==> variant_only_active='" << variant_only_active << "'");
              DT_LOG_TRACE(logging, "VPP ==> variant_only_reverse='" << variant_only_reverse << "'");
            }
            // Process the line or not:
            bool process_line = exhibit_property;
            if (process_line) {
              // variant_only may inhibit the property:
              if (! variant_only.empty()) {
                process_line = variant_only_active;
                if (variant_only_reverse) {
                  process_line = !variant_only_active;
                }
              }
            }
            // Process the current line:
            if (!process_line) {
              DT_PROP_CFG_READ_LOG_DEBUG(logging,
                                         _current_filename_,
                                         _section_name_,
                                         _section_start_line_number_,
                                         _current_line_number_,
                                         "Do not process property value repr '"
                                         << property_value_str << "' for property '"
                                         << prop_key << "'...");
            } else {
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

              DT_LOG_TRACE(logging, "--> effective_property_values_line='" << effective_property_values_line << "'");
              std::istringstream iss(effective_property_values_line);
              /***************
               *   BOOLEAN   *
               ***************/
              if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                DT_LOG_TRACE(logging, "boolean: effective_property_values_line='" << effective_property_values_line << "'");
                if (scalar) {
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_boolean(iss, a_boolean, read_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read boolean value from '"
                                            << effective_property_values_line << "' for key '"
                                            << prop_key << "' at line '" << line << "' !");
                } else {
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  for (int i = 0; i < vsize; ++i) {
                    bool boolean_value;
                    DT_PROP_CFG_READ_THROW_IF(!io::read_boolean(iss, boolean_value, read_flags),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Cannot read vector boolean value for key '"
                                              << prop_key << "' at line '" << line << "' !");
                    v_booleans[i] = boolean_value;
                  }
                }
              }
              /***************
               *   INTEGER   *
               ***************/
              if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                if (scalar) {
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_integer(iss, a_integer, read_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read integer value for key '"
                                            << prop_key << "' at line '" << line << "' !");
                } else {
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  for (int i = 0; i < vsize; ++i) {
                    int int_value = 0;
                    DT_PROP_CFG_READ_THROW_IF(!io::read_integer(iss, int_value, read_flags),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Cannot read vector integer value for key '"
                                              << prop_key << "' at line '" << line << "' !");
                    v_integers[i] = int_value;
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
                  bool normal_value = false;
                  uint32_t read_flags = 0;
                  read_flags |= ::datatools::io::reader_allow_trailing_characters;
                  DT_PROP_CFG_READ_THROW_IF(! datatools::io::read_real_number(iss, a_real, normal_value, read_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read vector real value for key '"
                                            << prop_key << "' at line '" << line << "' (" << props_.get_description() << ") !");
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
                    double real_value;
                    bool normal_value = false;
                    uint32_t reader_flags = datatools::io::reader_allow_trailing_characters;
                    // No unit is allowed after the real value in vector mode:
                    DT_PROP_CFG_READ_THROW_IF(! datatools::io::read_real_number(iss, real_value, normal_value, reader_flags),
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Cannot read vector real value for key '"
                                              << prop_key << "' at line '" << line << "' !");
                    if (enable_real_with_unit && datatools::is_valid(requested_unit_value)) {
                      real_value *= requested_unit_value;
                      with_explicit_unit = true;
                      unit_symbol = requested_unit_symbol;
                    }
                    v_reals[i] = real_value;
                  }
                }
              } // Real
              /**************
               *   STRING   *
               **************/
              if (type == properties::data::TYPE_STRING_SYMBOL) {
                if (scalar) {
                  uint32_t reader_flags = datatools::io::reader_allow_trailing_characters;
                  DT_PROP_CFG_READ_THROW_IF(!io::read_quoted_string(iss, a_string, reader_flags),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Cannot read string value for key '"
                                            << prop_key << "' at line '" << line << "' !");
                } else {
                  for (int i = 0; i < vsize; ++i) {
                    uint32_t reader_flags = datatools::io::reader_allow_trailing_characters;
                    std::string str;
                    DT_PROP_CFG_READ_THROW_IF(!io::read_quoted_string(iss, str, reader_flags),
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
                    message_oss << "There are unprocessed trailing characters '"
                                << trailing_bits << "' after value record of property '"
                                << prop_key << "'!";
                    DT_LOG_WARNING(_logging_,message_oss.str());
                  }
                }
              }
              bool store_it = true;
              // bool store_it = exhibit_property;
              // if (store_it) {
              //        if (! variant_only.empty()) {
              //          store_it = variant_only_active;
              //          if (variant_only_reverse) {
              //            store_it = !variant_only_active;
              //          }
              //        }
              // }
              if (not scalar and array_append) {
                DT_PROP_CFG_READ_THROW_IF(! allow_key_override,
                                          std::logic_error,
                                          _current_filename_,
                                          _section_name_,
                                          _section_start_line_number_,
                                          _current_line_number_,
                                          "Property override/append is not allowed for array with key '" << prop_key << "'!");
		if (! props_.has_key(prop_key)) {
		  // Revert to non "append mode" for array
		  array_append = false;
		  /*
		  DT_PROP_CFG_READ_THROW_IF(true,
					    std::logic_error,
					    _current_filename_,
					    _section_name_,
					    _section_start_line_number_,
					    _current_line_number_,
					    "Key '" << prop_key << "' is not used and no append action can be done on it!");
		  */
		} else {
		  DT_PROP_CFG_READ_THROW_IF(! props_.is_vector(prop_key),
					    std::logic_error,
					    _current_filename_,
					    _section_name_,
					    _section_start_line_number_,
					    _current_line_number_,
					    "Key '" << prop_key << "' is not a vector property and no append action can be done on it!");
		}
              } else {
                if (props_.has_key(prop_key)) {
                  if (!allow_key_override) {
                    DT_PROP_CFG_READ_THROW_IF(! array_override_item,
                                              std::logic_error,
                                              _current_filename_,
                                              _section_name_,
                                              _section_start_line_number_,
                                              _current_line_number_,
                                              "Key '" << prop_key << "' is already used and property override is not allowed!");
                  }
                  DT_LOG_WARNING(get_logging(), "Overriden property with key '" << prop_key << "' at line #" <<
                                 std::to_string(_current_line_number_) << " : '" << line << "'");
                  bool clean_key = true;
                  if (scalar and array_override_item) {
                    // std::cerr << "***** DEVEL ***" << "Clean existing array property '" << prop_key << "' ************" << std::endl;
                    clean_key = false;
                  }
                  if (clean_key) {
                    props_.clean(prop_key);
                  }
		}
              }
              // std::cerr << "***** DEVEL ***" << "About to store property '" << prop_key << "' ************" << std::endl;

              if (store_it) {
                // std::cerr << "***** DEVEL ***" << "Store property '" << prop_key << "' ************" << std::endl;
                if (type != properties::data::TYPE_REAL_SYMBOL) {
                  DT_PROP_CFG_READ_THROW_IF(with_explicit_unit or !unit_symbol.empty(),
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Non-real property with key '" << prop_key << "' does not support explicit unit!");
                }
                if (type != properties::data::TYPE_STRING_SYMBOL) {
                  DT_PROP_CFG_READ_THROW_IF(with_explicit_path,
                                            std::logic_error,
                                            _current_filename_,
                                            _section_name_,
                                            _section_start_line_number_,
                                            _current_line_number_,
                                            "Non-string property with key '" << prop_key << "' does not support explicit path!");
                }
                if (scalar) {
                  // std::cerr << "***** DEVEL ***" << "Scalar property '" << prop_key << "' ************" << std::endl;
                  if (! array_override_item) {
                    // std::cerr << "***** DEVEL ***" << "Store normal scalar property '" << prop_key << "' ************" << std::endl;
                    if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                      props_.store(prop_key, a_boolean, prop_description, locked);
                    }
                    if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                      props_.store(prop_key, a_integer, prop_description, locked);
                    }
                    if (type == properties::data::TYPE_REAL_SYMBOL) {
                      props_.store(prop_key, a_real, prop_description, locked);
                      if (with_explicit_unit) {
                        props_.set_explicit_unit(prop_key, true);
                      }
                      if (!unit_symbol.empty()) {
                        props_.set_unit_symbol(prop_key, unit_symbol);
                      }
                    }
                    if (type == properties::data::TYPE_STRING_SYMBOL) {
                      props_.store(prop_key, a_string, prop_description, locked);
                      if (with_explicit_path) {
                        props_.set_explicit_path(prop_key, true);
                      }
                    }
                  } else {
                    // Array override item:
                    // std::cerr << "***** DEVEL ***" << "Array override item for key '" << prop_key << "'************" << std::endl;
                    if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                      props_.change_boolean_vector(prop_key, a_boolean, array_override_index);
                    }
                    if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                      props_.change_integer_vector(prop_key, a_integer, array_override_index);
                    }
                    if (type == properties::data::TYPE_REAL_SYMBOL) {
                      DT_PROP_CFG_READ_THROW_IF(with_explicit_unit != props_.has_explicit_unit(prop_key),
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Overriden item for real vector key '" << prop_key << "' has unmatching explicit unit ith original vector!");
                      DT_PROP_CFG_READ_THROW_IF(props_.get_unit_symbol(prop_key) != unit_symbol,
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Overriden item for real vector key '" << prop_key
                                                << "' has unmatching explicit unit '" << props_.get_unit_symbol(prop_key)
                                                << "' with respect to original value with unit '" << unit_symbol << "'!");
                      props_.change_real_vector(prop_key, a_real, array_override_index);
                    }
                    if (type == properties::data::TYPE_STRING_SYMBOL) {
                      DT_PROP_CFG_READ_THROW_IF(with_explicit_path != props_.is_explicit_path(prop_key),
                                                std::logic_error,
                                                _current_filename_,
                                                _section_name_,
                                                _section_start_line_number_,
                                                _current_line_number_,
                                                "Original string vector key '" << prop_key << "' has unmatching explicit path with respect to overriden vector item!");
                      props_.change_string_vector(prop_key, a_string, array_override_index);
                    }
                  }

                } else {
                  // std::cerr << "***** DEVEL ***" << "Vector property '" << prop_key << "' ************" << std::endl;
                  if (not array_append) {
                    // std::cerr << "***** DEVEL ***" << "Normal store Vector property '" << prop_key << "' ************" << std::endl;
                    if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
                      props_.store(prop_key, v_booleans, prop_description, locked);
                    }
                    if (type == properties::data::TYPE_INTEGER_SYMBOL) {
                      props_.store(prop_key, v_integers, prop_description, locked);
                    }
                    if (type == properties::data::TYPE_REAL_SYMBOL) {
                      props_.store(prop_key, v_reals, prop_description, locked);
                      if (with_explicit_unit) {
                        props_.set_explicit_unit(prop_key, true);
                      }
                      if (!unit_symbol.empty()) {
                        props_.set_unit_symbol(prop_key, unit_symbol);
                      }
                    }
                    if (type == properties::data::TYPE_STRING_SYMBOL) {
                      props_.store(prop_key, v_strings, prop_description, locked);
                      if (with_explicit_path) {
                        props_.set_explicit_path(prop_key, true);
                      }
                    }
                  } else {
                    // std::cerr << "***** DEVEL ***" << "Append mode vector property '" << prop_key << "' ************" << std::endl;
                    if (type == properties::data::TYPE_BOOLEAN_SYMBOL) {
		      DT_PROP_CFG_READ_THROW_IF(not props_.is_boolean(prop_key) and not props_.is_vector(prop_key),
						std::logic_error,
						_current_filename_,
						_section_name_,
						_section_start_line_number_,
						_current_line_number_,
						"Original property with key '" << prop_key << "' is not a boolean vector property!");
			
		      data::vbool v_booleans_appended;
		      props_.fetch(prop_key, v_booleans_appended);
		      for (auto val : v_booleans) {
			v_booleans_appended.push_back(val);
		      }
		      props_.update(prop_key, v_booleans_appended);
		    }
                    if (type == properties::data::TYPE_INTEGER_SYMBOL) {
		      DT_PROP_CFG_READ_THROW_IF(not props_.is_integer(prop_key) and not props_.is_vector(prop_key),
						std::logic_error,
						_current_filename_,
						_section_name_,
						_section_start_line_number_,
						_current_line_number_,
						"Original property with key '" << prop_key << "' is not an integer vector property!");
			
		      data::vint v_integers_appended;
		      props_.fetch(prop_key, v_integers_appended);
		      for (auto val : v_integers) {
			v_integers_appended.push_back(val);
		      }
		      props_.update(prop_key, v_integers_appended);
                    }
                    if (type == properties::data::TYPE_REAL_SYMBOL) {
		      DT_PROP_CFG_READ_THROW_IF(not props_.is_real(prop_key) and not props_.is_vector(prop_key),
						std::logic_error,
						_current_filename_,
						_section_name_,
						_section_start_line_number_,
						_current_line_number_,
						"Original property with key '" << prop_key << "' is not a real vector property!");
		      data::vdouble v_reals_appended;
		      props_.fetch(prop_key, v_reals_appended);
		      for (auto val : v_reals) {
			v_reals_appended.push_back(val);
		      }
		      if (with_explicit_unit) {
			DT_PROP_CFG_READ_THROW_IF(not props_.has_explicit_unit(prop_key),
						  std::logic_error,
						  _current_filename_,
						  _section_name_,
						  _section_start_line_number_,
						  _current_line_number_,
						  "Original real vector key '" << prop_key << "' has no explicit unit!");
			DT_PROP_CFG_READ_THROW_IF(props_.has_unit_symbol(prop_key)
						  and props_.get_unit_symbol(prop_key) != unit_symbol,
						  std::logic_error,
						  _current_filename_,
						  _section_name_,
						  _section_start_line_number_,
						  _current_line_number_,
						  "Original real vector key '" << prop_key << "' has unmatching explicit unit '" << props_.get_unit_symbol(prop_key) << "' with respect to appended values with unit '" << unit_symbol << "'!");
		      }
		      props_.update(prop_key, v_reals_appended);
                    }
                    if (type == properties::data::TYPE_STRING_SYMBOL) {
		      DT_PROP_CFG_READ_THROW_IF(not props_.is_string(prop_key) and not props_.is_vector(prop_key),
						std::logic_error,
						_current_filename_,
						_section_name_,
						_section_start_line_number_,
						_current_line_number_,
						"Original property with key '" << prop_key << "' is not a string vector property!");
		      DT_PROP_CFG_READ_THROW_IF(with_explicit_path != props_.is_explicit_path(prop_key),
						std::logic_error,
						_current_filename_,
						_section_name_,
						_section_start_line_number_,
						_current_line_number_,
						"Original string vector key '" << prop_key << "' has unmatching explicit path with respect to appended vector!");
		      data::vstring v_strings_appended;
		      props_.fetch(prop_key, v_strings_appended);
		      for (auto val : v_strings) {
			v_strings_appended.push_back(val);
		      }
		      props_.update(prop_key, v_strings_appended);
		    }
                  }
                }
                prop_description.clear();
              } // if (store_it)
            } // if (process_line)
            // Clear current requested variant only :
            if (! variant_only.empty()) {
              variant_only.clear();
            }
            variant_only_reverse = false;
          }
        } // !skip_line
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
