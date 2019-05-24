// object_configuration_description.cc

// Ourselves:
#include <datatools/object_configuration_description.h>

// Standard library:
#include <limits>

// Third party:
// - Boost:
#include <boost/version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {

  // static
  const bool configuration_property_description::SCALAR;
  const bool configuration_property_description::ARRAY;
  const bool configuration_property_description::IMPLICIT_UNIT;
  const bool configuration_property_description::EXPLICIT_UNIT;
  const bool configuration_property_description::MUTABLE;
  const bool configuration_property_description::CONST;

  bool configuration_property_description::dependency_entry::dynamic() const
  {
    return type == DEP_DYNAMIC;
  }

  bool configuration_property_description::dependency_entry::by_flag() const
  {
    return type == DEP_BY_FLAG;
  }

  bool configuration_property_description::dependency_entry::by_label() const
  {
    return type == DEP_BY_LABEL;
  }

  bool configuration_property_description::dependency_entry::has_name() const
  {
    return ! name.empty();
  }

  const std::string &
  configuration_property_description::dependency_entry::get_name() const
  {
    return name;
  }

  const configuration_property_description &
  configuration_property_description::dependency_entry::ref() const
  {
    return *address;
  }

  bool configuration_property_description::dependency_entry::has_address() const
  {
    return address != 0;
  }

  bool configuration_property_description::dependency_entry::is_valid() const
  {
    return type != DEP_UNDEFINED && has_name() && has_address();
  }

  void configuration_property_description::dependency_entry::reset()
  {
    type = DEP_UNDEFINED;
    name.clear();
    triggering_status = true;
    triggering_labels.clear();
    address = 0;
    return;
  }

  configuration_property_description::dependency_entry::dependency_entry()
  {
    type = DEP_UNDEFINED;
    triggering_status = true;
    address = 0;
    return;
  }

  configuration_property_description::dependency_entry::~dependency_entry()
  {
    return;
  }

  bool configuration_property_description::is_const() const
  {
    return _const_;
  }

  bool configuration_property_description::has_section() const
  {
    return ! _section_.empty();
  }

  const std::string & configuration_property_description::get_section() const
  {
    return _section_;
  }

  bool configuration_property_description::is_mutable() const
  {
    return ! is_const();
  }

  bool configuration_property_description::is_boolean() const
  {
    return _type_ == TYPE_BOOLEAN;
  }

  bool configuration_property_description::is_integer() const
  {
    return _type_ == TYPE_INTEGER;
  }

  bool configuration_property_description::is_real() const
  {
    return _type_ == TYPE_REAL;
  }

  bool configuration_property_description::is_string() const
  {
    return _type_ == TYPE_STRING;
  }

  bool configuration_property_description::is_path() const
  {
    return is_string() && _path_;
  }

  bool configuration_property_description::is_valid() const
  {
    return ! _name_pattern_.empty() && _type_ != TYPE_NONE;
  }

  bool configuration_property_description::has_type() const
  {
    return _type_ != TYPE_NONE;
  }

  bool configuration_property_description::has_complex_triggering_conditions() const
  {
    return _complex_triggering_conditions_;
  }

  bool configuration_property_description::has_complex_dependencies() const
  {
    return _complex_dependencies_;
  }

  const configuration_property_description::dependency_entry &
  configuration_property_description::get_dynamic_dependee() const
  {
    return _dynamic_dependee_;
  }

  bool configuration_property_description::has_dynamic_dependers() const
  {
    return get_number_of_dynamic_dependers() > 0;
  }

  unsigned int configuration_property_description::get_number_of_dynamic_dependers() const
  {
    return _dynamic_dependers_.size();
  }

  const configuration_property_description::dependency_entry &
  configuration_property_description::get_dynamic_depender(int i_) const
  {
    return _dynamic_dependers_.at(i_);
  }

  unsigned int
  configuration_property_description::get_number_of_triggered_dependers() const
  {
    return _triggering_.size();
  }

  const configuration_property_description::dependency_entry &
  configuration_property_description::get_triggered_depender(int i_) const
  {
    return _triggering_.at(i_);
  }

  const configuration_property_description::dependency_entry &
  configuration_property_description::get_triggered_by_flag() const
  {
    return _triggered_by_flag_;
  }

  const configuration_property_description::dependency_entry &
  configuration_property_description::get_triggered_by_label() const
  {
    return _triggered_by_label_;
  }

  configuration_property_description::configuration_property_description()
  {
    _type_ = TYPE_NONE;
    _const_ = false;
    _path_ = false;
    _explicit_unit_ = false;
    _array_ = false;
    _array_fixed_size_ = -1;
    _default_array_size_ = -1;
    _default_value_boolean_ = boost::logic::tribool(boost::logic::indeterminate);
    _default_value_integer_ = std::numeric_limits<int>::min();
    _default_value_real_ = std::numeric_limits<double>::quiet_NaN();
    _default_value_string_ = "__??__";
    _mandatory_ = false;
    _complex_triggering_conditions_ = false;
    _complex_dependencies_ = false;
    _deprecated_ = false;
    return;
  }

  configuration_property_description &
  configuration_property_description::set_from(const std::string &from_)
  {
    _from_ = from_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_group(const std::string &group_)
  {
    _group_ = group_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_name_pattern(const std::string &np_)
  {
    _name_pattern_ = np_;
    size_t dollar_pos = np_.find('$');
    if (dollar_pos != np_.npos) {
      //std::cerr << "DEVEL: OCD: set_name_pattern : dollar_pos=" << dollar_pos << std::endl;
      DT_THROW_IF(dollar_pos >= np_.size() - 3,
                  std::logic_error,
                  "Invalid format for dynamic property name pattern '" << np_ << "' ! Invalid '${XXX}' dynamic name pattern !");
      DT_THROW_IF(np_[dollar_pos+1] != '{',
                   std::logic_error,
                  "Invalid format for dynamic property name pattern '" << np_ << "' ! Missing opening brace '{' after '$' in dynamic name pattern !");
      std::string subnp = np_.substr(dollar_pos+2);
      //std::cerr << "DEVEL: OCD: set_name_pattern : subnp='" << subnp << "'" << std::endl;
      size_t close_pos = subnp.find('}');
      DT_THROW_IF(close_pos == subnp.npos,
                  std::logic_error,
                  "Invalid format for dynamic property name pattern '" << np_ << "' ! Missing closing brace '}' in dynamic name pattern !");

      std::string source_prop_name = subnp.substr(0, close_pos);
      //std::cerr << "DEVEL: OCD: set_name_pattern : source_prop_name='" << source_prop_name << "'" << std::endl;
      _dynamic_dependee_.type = DEP_DYNAMIC;
      _dynamic_dependee_.name = source_prop_name;
      _dynamic_dependee_.address = 0;
    }
    /*
    std::vector<std::string> strs;
    boost::split(strs,np_,boost::is_any_of("."));
    for (int i = 0; i < strs.size(); i++) {
        const std::string & str = strs[i];
        DT_THROW_IF (str.empty(),
                     std::logic_error,
                     "Invalid property name pattern '" << np_ << "' !");
        if (str[0] == '$') {
          // Minimal token pattern is "${a}"
          DT_THROW_IF (str.length() < 4,
                       std::logic_error,
                       "Invalid format for dynamic property name pattern '" << np_ << "' !");
          DT_THROW_IF (str[1] != '{',
                       std::logic_error,
                       "Invalid format for dynamic property name pattern '" << np_ << "' !");
          DT_THROW_IF (str[str.length()-1] != '}',
                       std::logic_error,
                       "Invalid format for dynamic property name pattern '" << np_ << "' !");
          DT_THROW_IF (_dynamic_dependee_.has_name(),
                       std::logic_error,
                       "Dependee '" << _dynamic_dependee_.name << "' has already been defined ! "
                       << "Only one dependee is allowed for dynamic property name pattern '" << np_ << "' !");
          std::string source_prop_name = str.substr(2,str.length()-3);
          _dynamic_dependee_.type = DEP_DYNAMIC;
          _dynamic_dependee_.name = source_prop_name;
          _dynamic_dependee_.address = 0;
        }
      }
    */

    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_section(const std::string & section_name_)
  {
    _section_ = section_name_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_terse_description(const std::string &desc_)
  {
    _terse_description_ = desc_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_long_description(const std::string &desc_)
  {
    _long_description_ = desc_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_mandatory(bool m_)
  {
    _mandatory_ = m_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_triggered_by_flag(const std::string &prop_name_,
                                                            bool triggering_status_)
  {
    _triggered_by_flag_.type = DEP_BY_FLAG;
    _triggered_by_flag_.name = prop_name_;
    _triggered_by_flag_.triggering_status = triggering_status_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_triggered_by_label(const std::string & prop_name_,
                                                             const std::string & triggering_labels_)
  {
    _triggered_by_label_.type = DEP_BY_LABEL;
    _triggered_by_label_.name = prop_name_;
    boost::split(_triggered_by_label_.triggering_labels,triggering_labels_,boost::is_any_of(" \t,;"));
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_explicit_unit(bool xu_)
  {
    DT_THROW_IF (!is_real() && xu_,
                 std::logic_error,
                 "Explicit unit is only supported by 'real' properties but the property with name pattern '" << _name_pattern_ << "' is not a 'real' !");
    _explicit_unit_ = xu_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_default_array_size(int dasz_)
  {
    DT_THROW_IF(is_fixed_sized_array(), std::logic_error,
                "Property description has a fixed array size !");
    _default_array_size_ = dasz_;
    return *this;
  }

  bool configuration_property_description::has_default_array_size() const
  {
    return _default_array_size_ >= 0;
  }

  int configuration_property_description::get_default_array_size() const
  {
    return _default_array_size_;
  }

  configuration_property_description &
  configuration_property_description::set_default_value_boolean(bool dv_)
  {
    DT_THROW_IF(! is_boolean(), std::logic_error, "Not a boolean property description for the property with name pattern '" << get_name_pattern() << "' !");
    _default_value_boolean_ = boost::logic::tribool(dv_);
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_default_value_integer(int dv_)
  {
    DT_THROW_IF(! is_integer(), std::logic_error, "Not an integer property description for the property with name pattern '" << get_name_pattern() << "' !");
    _default_value_integer_ = dv_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_default_value_real(double dv_, const std::string & unit_symbol_)
  {
    DT_THROW_IF(! is_real(), std::logic_error, "Not a real property description for the property with name pattern '" << get_name_pattern() << "' !");
    _default_value_real_ = dv_;
    _default_value_real_unit_ = unit_symbol_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_default_value_string(const std::string & dv_)
  {
    DT_THROW_IF(! is_string(), std::logic_error, "Not a string property description for the property with name pattern '" << get_name_pattern() << "' !");
    _default_value_string_ = dv_;
    return *this;
  }

  bool configuration_property_description::has_default_value() const
  {
    if (is_scalar()) {
      if (is_boolean()) {
#if BOOST_VERSION < 106800
        return (_default_value_boolean_ == true || _default_value_boolean_ == false);
#else
        // See: https://github.com/dyninst/dyninst/pull/541
        return (static_cast<bool>(_default_value_boolean_) || static_cast<bool>(!_default_value_boolean_));
#endif
      }
      if (is_integer()) {
        return _default_value_integer_ > std::numeric_limits<int>::min();
      }
      if (is_real()) {
        return _default_value_real_ == _default_value_real_;
      }
      if (is_string()) {
        return _default_value_string_ != "__??__";
      }
    }
    return false;
  }

  bool
  configuration_property_description::get_default_value_boolean() const
  {
#if BOOST_VERSION <= 106800
    return _default_value_boolean_;
#else
    if (_default_value_boolean_) return true;
    else if (!_default_value_boolean_) return false;
    DT_THROW(std::logic_error, "No default boolean value is set!");
#endif
  }

  int
  configuration_property_description::get_default_value_integer() const
  {
    return _default_value_integer_;
  }

  double
  configuration_property_description::get_default_value_real() const
  {
    return _default_value_real_;
  }

  const std::string &
  configuration_property_description::get_default_value_real_unit() const
  {
    return _default_value_real_unit_;
  }

  const std::string &
  configuration_property_description::get_default_value_string() const
  {
    return _default_value_string_;
  }

  configuration_property_description &
  configuration_property_description::set_const(bool c_)
  {
    _const_ = c_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_path(bool p_)
  {
    DT_THROW_IF (! is_string(),
                 std::logic_error,
                 "Explicit PATH is only supported by 'string' properties but the property with name pattern '" << _name_pattern_ << "' is not a string' !");
    _path_ = p_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_unit_label(const std::string & ul_)
  {
    DT_THROW_IF (!is_real() && !ul_.empty(),
                 std::logic_error,
                 "Unit label is only supported by 'real' properties but the property with name pattern '" << _name_pattern_ << "' is not a 'real' !");
    _unit_label_ = ul_;
    if(! ul_.empty()) {
      set_explicit_unit(true);
      if (! is_array() && _unit_symbol_.empty()) {
        _unit_symbol_ = datatools::units::get_default_unit_symbol_from_label(_unit_label_);
      }
    }
    return *this;
  }

  bool configuration_property_description::has_unit_label() const
  {
    return !_unit_label_.empty();
  }

  const std::string & configuration_property_description::get_unit_label() const
  {
    return _unit_label_;
  }

  configuration_property_description &
  configuration_property_description::set_unit_symbol(const std::string & us_)
  {
    DT_THROW_IF (!is_real() && !is_array() && !us_.empty(),
                 std::logic_error,
                 "Unit symbol is only supported by 'real' array properties but the property with name pattern '" << _name_pattern_ << "' is not a array of 'real' !");
    _unit_symbol_ = us_;
    if(! us_.empty()) set_explicit_unit(true);
    return *this;
  }

  bool configuration_property_description::has_unit_symbol() const
  {
    return !_unit_symbol_.empty();
  }

  const std::string & configuration_property_description::get_unit_symbol() const
  {
    return _unit_symbol_;
  }

  bool configuration_property_description::is_trigger() const
  {
    return _triggering_.size() > 0;
  }

  bool configuration_property_description::is_triggered_by_flag() const
  {
    return _triggered_by_flag_.by_flag() && ! _triggered_by_flag_.name.empty();
  }

  bool configuration_property_description::is_triggered_by_label() const
  {
    return _triggered_by_label_.by_label()
      && ! _triggered_by_label_.name.empty() ;
  }

  bool configuration_property_description::has_single_type() const
  {
    if (_type_ == TYPE_BOOLEAN) return true;
    if (_type_ == TYPE_INTEGER) return true;
    if (_type_ == TYPE_REAL) return true;
    if (_type_ == TYPE_STRING) return true;
    return false;
  }

  configuration_property_description &
  configuration_property_description::set_traits(int type_,
                                                 bool array_,
                                                 int fixed_size_)
  {
    DT_THROW_IF((size_t)type_ < TYPE_BOOLEAN || (size_t)type_ > TYPE_ANY,
                std::logic_error,
                "Invalid type for property with name pattern '" << _name_pattern_ << "' !");
    _type_ = type_;
    _array_ = array_;
    DT_THROW_IF(fixed_size_ >= 0 && !_array_,
                std::logic_error,
                "Invalid array fixed size directive makes no sense for a scalar property !");
    _array_fixed_size_ = fixed_size_;

    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_complex_triggering_conditions(bool ctc_)
  {
    _complex_triggering_conditions_ = ctc_;
    return *this;
  }

  configuration_property_description &
  configuration_property_description::set_complex_dependencies(bool cd_)
  {
    _complex_dependencies_ = cd_;
    return *this;
  }

  bool configuration_property_description::is_static() const
  {
    return ! _dynamic_dependee_.has_name();
  }

  bool configuration_property_description::is_dynamic() const
  {
    return ! is_static();
  }

  bool configuration_property_description::has_explicit_unit() const
  {
    return _explicit_unit_;
  }

  bool configuration_property_description::is_fixed_sized_array() const
  {
    return _array_fixed_size_ >= 0;
  }

  int configuration_property_description::get_array_fixed_size() const
  {
    return _array_fixed_size_;
  }

  const std::string &
  configuration_property_description::get_name_pattern() const
  {
    return _name_pattern_;
  }

  const std::string &
  configuration_property_description::get_from() const
  {
    return _from_;
  }

  const std::string &
  configuration_property_description::get_group() const
  {
    return _group_;
  }

  const std::string &
  configuration_property_description::get_terse_description() const
  {
    return _terse_description_;
  }

  bool
  configuration_property_description::has_terse_description() const
  {
    return !_terse_description_.empty();
  }


  const std::string &
  configuration_property_description::get_long_description() const
  {
    return _long_description_;
  }

  bool
  configuration_property_description::has_long_description() const
  {
    return !_long_description_.empty();
  }

  unsigned int
  configuration_property_description::get_number_of_examples() const
  {
    return _examples_.size();
  }

  const std::string &
  configuration_property_description::get_example(int index_) const
  {
    return _examples_.at(index_);
  }

  configuration_property_description &
  configuration_property_description::add_example(const std::string &example_)
  {
    _examples_.push_back(example_);
    return *this;
  }

  bool configuration_property_description::is_mandatory() const
  {
    return _mandatory_;
  }

  int  configuration_property_description::get_type() const
  {
    return _type_;
  }

  bool configuration_property_description::is_scalar() const
  {
    return !is_array();
  }

  bool configuration_property_description::is_array() const
  {
    return _array_;
  }

  configuration_property_description &
  configuration_property_description::set_deprecated(bool val_, const std::string &info_)
  {
    _deprecated_ = val_;
    if (val_) {
      _deprecated_info_ = info_;
    } else {
      _deprecated_info_.clear();
    }
    return *this;
  }

  bool configuration_property_description::is_deprecated() const
  {
    return _deprecated_;
  }

  const std::string &
  configuration_property_description::get_deprecated_info() const
  {
    return _deprecated_info_;
  }

  void configuration_property_description::print(std::ostream &out_,
                                                 const std::string & indent_) const
  {
    out_ << indent_ << std::endl;
    /*
    out_ << indent_ << "* Property : ``" << get_name_pattern() << "``"
         << std::endl << std::endl;
    */
    out_ << indent_ << "Property ``" << get_name_pattern() << "``" << std::endl;
    out_ << indent_ << ".........................................................................."  << std::endl;
    out_ << indent_ << std::endl;

    std::string indent = indent_ + "   ";

    if (has_terse_description()) {
      print_multi_lines(out_, _terse_description_, indent);
    }

    out_ << std::endl << indent << "**General informations:** " << std::endl;

    out_ << indent << std::endl;

    if (! _from_.empty()) {
      out_  << indent << "* From : ``" << _from_ << "``" << std::endl;
    }

    if (is_deprecated()) {
      out_ << indent << "* Deprecated";
      if (!_deprecated_info_.empty()) {
        out_ << " : " << _deprecated_info_;
      }
      out_ << std::endl;
    }

    if (! _group_.empty()) {
      out_  << indent << "* Group : ``" << _group_ << "``" << std::endl;
    }

    out_ << indent << "* Type: ";
    if (! has_type()) {
      out_ << "undefined";
    } else {
      out_ << "``";
      out_ << ::datatools::get_label_from_type(_type_);
      if (is_array()) {
        out_ << '[';
        if (is_fixed_sized_array()) {
          out_ << get_array_fixed_size();
        } else {
          out_ << "*";
        }
        out_ << ']';
      }
      if (is_string() && is_path()) {
        out_ << " as path";
      }
      /*
        if (is_real()) {
        if (is_array()) {
        ///out_ << " as <unit label>";
        }
        else {
        ///out_ << " in <unit symbol>";
        }
        }
      */
      //out_ << ") : ";
      out_ << "``";
    }
    out_ << std::endl;

    out_ << indent << "* Default value : ";
    if (has_default_value()) {
      if (is_boolean()) {
        out_ << "``" << get_default_value_boolean() << "``";
      } else if (is_integer()) {
        out_ << "``" << get_default_value_integer() << "``";
      } else if (is_real()) {
        double val = get_default_value_real();
        if (! get_default_value_real_unit().empty()) {
          val /= datatools::units::get_unit(get_default_value_real_unit());
        }
        out_ << "``" << val;
        if (! get_default_value_real_unit().empty()) {
          out_ << " " << get_default_value_real_unit();
        }
        out_ << "``";
      } else if (is_string()) {
        out_ << "``\"" << get_default_value_string() << "\"``";
      }
    } else {
      out_ << "none";
    }
    out_ << std::endl;

    out_ << indent << "* Dynamic : ";
    if (is_dynamic()) {
      out_ << "yes";
    } else {
      out_ << "no (static)";
    }
    out_ << std::endl;

    out_ << indent << "* Mandatory : ";
    if (is_mandatory()) {
      out_ << "yes";
    } else {
      out_ << "no";
    }
    out_ << std::endl;

    out_ << indent << "* Dependency : ";
    bool spe_dpcy = false;
    if (has_complex_triggering_conditions()) {
      out_ << std::endl << indent << "  " << " + "
           << "has complex triggering conditions";
      spe_dpcy = true;
    } else if (has_complex_dependencies()) {
      out_ << std::endl << indent << "  " << " + "
           << "has complex dependencies";
      spe_dpcy = true;
    }
    if (is_dynamic()) {
      out_ << std::endl << indent << "  " << " + "
           << "Depends on property " << "``" << _dynamic_dependee_.name << "``";
      spe_dpcy = true;
    }
    if (is_triggered_by_flag()) {
      out_ << std::endl << indent << "  " << " + "
           << "Triggered by the flag/boolean property " << "``" << _triggered_by_flag_.name << "``";
      spe_dpcy = true;
    }
    if (is_triggered_by_label()) {
      out_ << std::endl << indent << "  " << " + "
           << "Triggered by the string property " << "``" << _triggered_by_label_.name << "`` with any value ";
      out_ << "from the following list: ";
      for (int itl = 0; itl < (int) _triggered_by_label_.triggering_labels.size(); itl++) {
        out_ << "``\"" << _triggered_by_label_.triggering_labels[itl] << "\"`` ";
        if (itl != (int) _triggered_by_label_.triggering_labels.size() - 1) out_ << ", ";
      }
      spe_dpcy = true;

    }
     if (! spe_dpcy)  out_ << "none";
    out_ << std::endl;

    out_ << indent << "* Depender properties : ";
    if (_dynamic_dependers_.size() == 0) {
      out_ << "none";
    } else {
      for (size_t i = 0; i < _dynamic_dependers_.size(); i++) {
        out_ << std::endl << indent << "  " << " + "
             << "``" <<  _dynamic_dependers_[i].get_name() << "``";
       }
    }
    out_ << std::endl;
    out_ << indent << std::endl;;

    if (has_long_description()) {
      out_ << std::endl << indent << "**Detailed description:**" << std::endl;
      out_ << indent << std::endl;
      print_multi_lines(out_, _long_description_, indent);
    }

    if (get_number_of_examples() > 0) {
      for (size_t i = 0; i < get_number_of_examples(); i++) {
        out_ << std::endl << indent << "**Example";
        if (get_number_of_examples() > 1) {
          out_ << " " << (i + 1);
        }
        out_ << ":**" << std::endl;
        out_ << indent << std::endl;
        print_multi_lines(out_, get_example(i), indent);
      }
    }

    return;
  }


  void configuration_property_description::dump(std::ostream & out_,
                                                const std::string & title_,
                                                const std::string & indent_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << '\n';
    }
    out_ << indent_ << "|-- " << "Name pattern : '" << _name_pattern_ << "'\n";
    out_ << indent_ << "|   `-- " << "Static : " << is_static() << "\n";
    out_ << indent_ << "|-- " << "Status : '" << (is_valid()?"Valid":"Invalid") << "'\n";
    out_ << indent_ << "|-- " << "Terse description : '" << _terse_description_ << "'\n";
    out_ << indent_ << "|-- " << "Long description : "
         << (has_long_description() ? "Yes":"No") << "\n";
    out_ << indent_ << "|-- " << "Type : '" << ::datatools::get_label_from_type (_type_) << "'\n";
    out_ << indent_ << "|-- " << "Const : " << _const_ << "\n";
    if (is_real()) {
      out_ << indent_ << "|   |-- " << "Explicit unit : " << _explicit_unit_ << "\n";
      if (is_array()) {
        out_ << indent_ << "|   `-- " << "Unit symbol   : '" << _unit_symbol_ << "'\n";
      } else {
        out_ << indent_ << "|   `-- " << "Unit label   : '" << _unit_label_ << "'\n";
      }
    }
    if (is_string()) {
      out_ << indent_ << "|   `-- " << "Explicit path : " << _path_ << "\n";
    }
    out_ << indent_ << "|-- " << "Array : '" << _array_ << "'\n";
    if (_array_) {
      if (is_real()) out_ << indent_ << "|   |-- " << "Unit symbol : '" << _unit_symbol_ << "'\n";
      out_ << indent_ << "|   `-- " << "Fixed size : '" << _array_fixed_size_ << "'\n";
    }
    if (is_dynamic() && _dynamic_dependee_.has_name()) {
      out_ << indent_ << "|-- ";
      out_ << "Dependee : '" << _dynamic_dependee_.name << "'";
      if (! _dynamic_dependee_.has_address()) {
        out_ << " (no address)";
      }
      out_ << "\n";
    }
    if ( has_complex_dependencies()) {
      out_ << indent_ << "|-- " << "Complex dependencies : " << _complex_dependencies_ << "\n";
    } else {
      if (is_static() && _dynamic_dependers_.size()) {
        out_ << indent_ << "|-- " << "Dependers : " << _dynamic_dependers_.size() << "\n";
        for (size_t i = 0; i < _dynamic_dependers_.size(); i++) {
          out_ << indent_ << "|-- ";
          if (i == _dynamic_dependers_.size() - 1) {
            out_ << "`-- ";
          } else {
            out_ << "|-- ";
          }
          const dependency_entry & der = _dynamic_dependers_[i];
          out_ << "Depender : '" << der.name << "'";
          if (! der.has_address()) {
            out_ << " (no address)";
          }
          out_ << "\n";
        }
      }
    }

    if ( has_complex_triggering_conditions()) {
      out_ << indent_ << "|-- " << "Complex triggering conditions : " << _complex_triggering_conditions_ << "\n";
    } else if (_triggering_.size()) {
      out_ << indent_ << "|-- " << "Triggering : " << _triggering_.size() << "\n";
      for (size_t i = 0; i < _triggering_.size(); i++) {
        out_ << indent_ << "|-- ";
        if (i == _triggering_.size() - 1) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        const dependency_entry & trig = _triggering_[i];
        out_ << "Triggered : '" << trig.name << "'";
        if (! trig.has_address()) {
          out_ << " (no address)";
        }
        out_ << "'\n";
      }
      if (is_triggered_by_flag() && _triggered_by_flag_.has_name()) {
        out_ << indent_ << "|-- " << "Triggered by flag : '"  << _triggered_by_flag_.name << "'";
        if (! _triggered_by_flag_.has_address()) {
          out_ << " (no address)";
        }
        out_ << "\n";
      }
      if (is_triggered_by_label() && _triggered_by_label_.has_name()) {
        out_ << indent_ << "|-- " << "Triggered by label : '"  << _triggered_by_label_.name << "'";
        out_ << " with one of the values in : ";
        for (size_t itl = 0; itl < _triggered_by_label_.triggering_labels.size(); itl++) {
          out_ << " '" << _triggered_by_label_.triggering_labels[itl] << "'";
        }
        if (! _triggered_by_label_.has_address()) {
          out_ << " (no address)";
        }
        out_ << "\n";
      }
    }
    out_ << indent_ << "`-- " << "Mandatory : '" << _mandatory_ << "'\n";
    return;
  }

  bool object_configuration_description::is_available() const
  {
    return ! _class_name_.empty() && ! _configuration_hints_.empty();
  }

  object_configuration_description::object_configuration_description()
  {
    _locked_ = false;
    _validation_support_ = false;
    return;
  }

  object_configuration_description &
  object_configuration_description::set_class_name(const std::string & cn_)
  {
    _class_name_ = cn_;
    return *this;
  }

  object_configuration_description &
  object_configuration_description::set_class_description(const std::string & cd_)
  {
    _class_description_ = cd_;
    return *this;
  }

  object_configuration_description &
  object_configuration_description::set_class_library(const std::string & cd_)
  {
    _class_library_ = cd_;
    return *this;
  }

  object_configuration_description &
  object_configuration_description::set_class_documentation(const std::string & cd_)
  {
    _class_documentation_ = cd_;
    return *this;
  }

  object_configuration_description &
  object_configuration_description::set_configuration_hints(const std::string & ch_)
  {
    _configuration_hints_ = ch_;
    return *this;
  }

  unsigned int object_configuration_description::get_number_of_examples() const
  {
    return _examples_.size();
  }

  const std::string & object_configuration_description::get_example(int i_) const
  {
    return _examples_.at(i_);
  }

  void
  object_configuration_description::add_example(const std::string &example_)
  {
    _examples_.push_back(example_);
    return;
  }

  object_configuration_description &
  object_configuration_description::set_validation_support(bool vs_)
  {
    _validation_support_ = vs_;
    return *this;
  }

  const std::string &
  object_configuration_description::get_class_name() const
  {
    return _class_name_;
  }

  const std::string & object_configuration_description::get_class_description() const
  {
    return _class_description_;
  }

  const std::string & object_configuration_description::get_class_library() const
  {
    return _class_library_;
  }

  const std::string & object_configuration_description::get_class_documentation() const
  {
    return _class_documentation_;
  }

  void object_configuration_description::add_group(const std::string &group_)
  {
    if (_groups_.count(group_) == 0) {
      _groups_.insert(group_);
    }
    return;
  }

  const std::set<std::string> & object_configuration_description::get_groups() const
  {
    return _groups_;
  }

  bool object_configuration_description::has_class_description() const
  {
    return !_class_description_.empty();
  }

  bool object_configuration_description::has_class_library() const
  {
    return !_class_library_.empty();
  }

  bool object_configuration_description::has_class_documentation() const
  {
    return !_class_documentation_.empty();
  }

  const std::string & object_configuration_description::get_configuration_hints() const
  {
    return _configuration_hints_;
  }

  bool object_configuration_description::has_configuration_hints() const
  {
    return !_configuration_hints_.empty();
  }

  bool object_configuration_description::has_validation_support() const
  {
    return _validation_support_;
  }

  void object_configuration_description::print(std::ostream & out_,
                                               const std::string & indent_,
                                               uint32_t po_flags_) const
  {

    if (!(po_flags_ & po_no_title)) {
      std::ostringstream title_oss;
      title_oss << "Configuration of ``" << get_class_name () << "``";
      const size_t len = title_oss.str().size();
      std::ostringstream title_bar;
      for (size_t i = 0; i < len; i++) {
        title_bar << '=';
      }

      out_ << indent_ << std::endl;
      out_ << indent_ << title_bar.str() << std::endl;
      out_ << indent_ << title_oss.str() << std::endl;
      out_ << indent_ << title_bar.str() << std::endl;
      out_ << indent_ << std::endl;
    }

    // out_ << indent_ << ".. contents:: " << std::endl;
    // out_ << indent_ << "   :depth: 2" << std::endl;
    // out_ << indent_ << "   :backlinks: top" << std::endl;
    out_ << indent_ << std::endl;

    out_ << indent_ << std::endl;
    out_ << indent_ << "Generalities" << std::endl;
    out_ << indent_ << "------------" << std::endl;
    out_ << indent_ << std::endl;

    out_ << indent_ << "* Class name : ``" << get_class_name () << "``" << std::endl;

    out_ << indent_ << "* Class description : ";
    if (has_class_description()) {
      out_ << get_class_description ();
    } else {
      out_ << "none";
    }
    out_ << std::endl;

    out_ << indent_ << "* Class library : ";
    if (has_class_library()) {
      out_ << "``" << get_class_library () << "``";
    } else {
      out_ << "undocumented";
    }
    out_ << std::endl;

    out_ << indent_ << "* Class documentation : ";
    if (! has_class_documentation()) {
      out_ << "none" << std::endl;
    } else {
      out_ << std::endl << std::endl;
      print_multi_lines(out_, _class_documentation_, indent_ + "  ");
    }

    if (_groups_.size()) {
      out_ << indent_ << "* Group(s) : " << _groups_.size() << std::endl;
      //for (
      out_ << indent_ << std::endl;
    }

    out_ << indent_ << "* Validation support : " << (_validation_support_ ? "yes" : "no") << std::endl;
    out_ << indent_ << std::endl;

    if (po_flags_ & po_no_config) return;

    if ( _configuration_properties_infos_.size() > 0) {
      out_ << indent_ << std::endl;
      out_ << indent_ << "Description of configuration properties" << std::endl;
      out_ << indent_ << "---------------------------------------" << std::endl;
      for (size_t i = 0; i < _configuration_properties_infos_.size(); i++) {
        out_ << indent_ << std::endl;
        const configuration_property_description & cpd = _configuration_properties_infos_[i];
        cpd.print(out_, indent_);
      }
      out_ << indent_ << std::endl;
    }

    if (has_configuration_hints()) {
      out_ << indent_ << std::endl;
      out_ << indent_ << "Configuration hints" << std::endl;
      out_ << indent_ << "-------------------" << std::endl;
      print_multi_lines(out_, _configuration_hints_, indent_);
    }

    if (_examples_.size()) {
      out_ << indent_ << std::endl;
      out_ << indent_ << "Examples" << std::endl;
      out_ << indent_ << "-------------------" << std::endl;
      for (size_t i = 0; i < _examples_.size(); i++) {
        out_ << std::endl << indent_ << "**Example ";
        if (get_number_of_examples() > 1) {
          out_ << (i + 1);
        }
        out_ <<  " :**" << std::endl;
        out_ << indent_ << std::endl;
        print_multi_lines(out_, _examples_[i], indent_);

      }
    }

    return;
  }

  unsigned int
  object_configuration_description::get_number_of_documented_properties() const
  {
    return _configuration_properties_infos_.size();
  }

  const configuration_property_description &
  object_configuration_description::get_configuration_property_info(int i_) const
  {
    return _configuration_properties_infos_[i_];
  }

  configuration_property_description &
  object_configuration_description::add_configuration_property_info()
  {
    {
      configuration_property_description dummy;
      _configuration_properties_infos_.push_back(dummy);
    }
    return _configuration_properties_infos_.back();
  }


  configuration_property_description &
  object_configuration_description::add_property_info()
  {
    return add_configuration_property_info();
  }

  bool object_configuration_description::is_locked() const
  {
    return _locked_;
  }

  void object_configuration_description::lock()
  {
    if (_locked_) return;
    _at_lock_();
    _locked_ = true;
    return;
  }

  void object_configuration_description::_at_lock_()
  {
    if (! has_class_library()) {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                    "No library for class named '" << get_class_name() << "' !");
    }
    // Establish interdependencies between dependees/dependers dynamic properties :
    for (size_t i = 0; i < _configuration_properties_infos_.size(); i++) {
      configuration_property_description & cpd = _configuration_properties_infos_[i];
      if (cpd.is_static()) {
        continue;
      }
      configuration_property_description::dependency_entry & dde = cpd._dynamic_dependee_;
      for (size_t k = 0; k < _configuration_properties_infos_.size(); k++) {
        if (k == i) continue;
        configuration_property_description & cpd2 = _configuration_properties_infos_[k];
        // std::cerr << "DEVEL: " << "datatools::object_configuration_description::_at_lock_: "
        //           << "Dependee's name : '" << dde.name << "' "
        //           << "versus checked name : '" << cpd2.get_name_pattern() << "' "
        //           << "\n";
        if (dde.name == cpd2.get_name_pattern()) {
          DT_THROW_IF (cpd2.is_dynamic(),
                       std::logic_error,
                       "Dynamic property with name pattern '"
                       << cpd.get_name_pattern() << "' cannot depend on "
                       << "the dynamic property '" << cpd2.get_name_pattern() << "' !");
          DT_THROW_IF (! cpd2.is_string(),
                       std::logic_error,
                       "Dynamic property with name pattern '"
                       << cpd.get_name_pattern() << "' cannot depend on "
                       << "the non-string property '" << cpd2.get_name_pattern() << "' !");
          DT_THROW_IF (cpd2.is_path(),
                       std::logic_error,
                       "Dynamic property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                       << "the path string property '" << cpd2.get_name_pattern() << "' !");
          DT_THROW_IF (! cpd2.is_array(),
                       std::logic_error,
                       "Dynamic property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                       << "the non-array property '" << cpd2.get_name_pattern() << "' !");
          // Set the address of the dependee :
          dde.address = &cpd2;
          // Register the depender in the dependee :
          {
            configuration_property_description::dependency_entry dummy;
            cpd2._dynamic_dependers_.push_back(dummy);
          }
          configuration_property_description::dependency_entry & der = cpd2._dynamic_dependers_.back();
          der.name = cpd.get_name_pattern();
          der.address = &cpd;
          // Abort the loop :
          break;
        }
      }
      DT_THROW_IF (!dde.has_address(),
                   std::logic_error,
                   "Cannot find dependency '"
                   << dde.name
                   << "' for dynamic property with name pattern '" << cpd.get_name_pattern() << "' !");
    }

    // Establish interdependencies between triggering/triggered properties (BY FLAG):
    for (size_t i = 0; i < _configuration_properties_infos_.size(); i++) {
      configuration_property_description & cpd = _configuration_properties_infos_[i];
      if (! cpd.is_triggered_by_flag()) {
        continue;
      }
      configuration_property_description::dependency_entry & trigger = cpd._triggered_by_flag_;
      for (size_t k = 0; k < _configuration_properties_infos_.size(); k++) {
        if (k == i) continue;
        configuration_property_description & cpd2 = _configuration_properties_infos_[k];
        // std::cerr << "DEVEL: " << "datatools::object_configuration_description::_at_lock_: "
        //           << "Trigger's name : '" << trigger.name << "' "
        //           << "versus checked name : '" << cpd2.get_name_pattern() << "' "
        //           << "\n";
        if (trigger.name == cpd2.get_name_pattern()) {
          DT_THROW_IF (cpd2.is_dynamic(),
                       std::logic_error,
                       "Triggered property with name pattern '" << cpd.get_name_pattern() << "' cannot be triggered by "
                       << "the dynamic property '" << cpd2.get_name_pattern() << "' !");

          DT_THROW_IF (! cpd2.is_boolean(),
                       std::logic_error,
                       "Triggered property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                       << "the non-boolean property '" << cpd2.get_name_pattern() << "' !");

          // Set the address of the trigger :
          trigger.address = &cpd2;
          // Register the depender in the dependee :
          {
            configuration_property_description::dependency_entry dummy;
            cpd2._triggering_.push_back(dummy);
          }
          configuration_property_description::dependency_entry & triggered
            = cpd2._triggering_.back();
          triggered.name = cpd.get_name_pattern();
          triggered.address = &cpd;
          // Abort the loop :
          break;
        }
      }
      DT_THROW_IF (!trigger.has_address(),
                   std::logic_error,
                   "Cannot find trigger property '"
                   << trigger.name
                   << "' for property with name pattern '" << cpd.get_name_pattern() << "' !");
    }

    // Establish interdependencies between triggering/triggered properties (BY LABEL):
    for (size_t i = 0; i < _configuration_properties_infos_.size(); i++) {
      configuration_property_description & cpd = _configuration_properties_infos_[i];
      if (! cpd.is_triggered_by_label()) {
        continue;
      }
      configuration_property_description::dependency_entry & trigger = cpd._triggered_by_label_;
      for (size_t k = 0; k < _configuration_properties_infos_.size(); k++) {
        if (k == i) continue;
        configuration_property_description & cpd2 = _configuration_properties_infos_[k];
        // std::cerr << "DEVEL: " << "datatools::object_configuration_description::_at_lock_: "
        //           << "Trigger's name : '" << trigger.name << "' "
        //           << "versus checked name : '" << cpd2.get_name_pattern() << "' "
        //           << "\n";
        if (trigger.name == cpd2.get_name_pattern()) {
          DT_THROW_IF (cpd2.is_dynamic(),
                       std::logic_error,
                       "Triggered property with name pattern '"
                       << cpd.get_name_pattern() << "' cannot be triggered by "
                       << "the dynamic property '" << cpd2.get_name_pattern() << "' !");
          DT_THROW_IF (! cpd2.is_string(),
                       std::logic_error,
                       "Triggered property with name pattern '"
                       << cpd.get_name_pattern() << "' cannot depend on "
                       << "the non-string property '" << cpd2.get_name_pattern() << "' !");
          DT_THROW_IF (cpd2.is_path(),
                       std::logic_error,
                       "Triggered property with name pattern '"
                       << cpd.get_name_pattern() << "' cannot depend on "
                       << "the path string property '" << cpd2.get_name_pattern() << "' !");
          // Set the address of the trigger :
          trigger.address = &cpd2;
          // Register the depender in the dependee :
          {
            configuration_property_description::dependency_entry dummy;
            cpd2._triggering_.push_back(dummy);
          }
          configuration_property_description::dependency_entry & triggered
            = cpd2._triggering_.back();
          triggered.name = cpd.get_name_pattern();
          triggered.address = &cpd;
          // Abort the loop :
          break;
        }
      }
      DT_THROW_IF (!trigger.has_address(),
                   std::logic_error,
                   "Cannot find trigger property '"
                   << trigger.name
                   << "' for property with name pattern '" << cpd.get_name_pattern() << "' !");
    }
    return;
  }

  void object_configuration_description::dump(std::ostream & out_,
                                              const std::string & title_,
                                              const std::string & indent_) const
  {
    if (! title_.empty()) {
        out_ << indent_ << title_ << '\n';
      }
    out_ << indent_ << "|-- " << "Status : " << (is_locked()?"locked":"unlocked") << "\n";
    out_ << indent_ << "|-- " << "Class name          : " << _class_name_ << "\n";
    out_ << indent_ << "|-- " << "Class description   : " << _class_description_ << "\n";
    out_ << indent_ << "|-- " << "Class documentation : " << _class_documentation_ << "\n";
    out_ << indent_ << "|-- " << "Class library       : " << _class_library_ << "\n";
    out_ << indent_ << "|-- " << "Configuration properties : \n";
    for (size_t i = 0; i < _configuration_properties_infos_.size(); i++) {
      out_ << indent_ << "|   ";
      std::string tag = "|   ";
      if (i ==  _configuration_properties_infos_.size() - 1) {
        out_ << "`-- ";
        tag = "    ";
      } else {
        out_ << "|-- ";
      }
      out_ << "Configuration property #" << i << " : \n";
      const configuration_property_description & cpd = _configuration_properties_infos_[i];
      cpd.dump(out_, "", indent_ + "|   " + tag);
    }

    out_ << indent_ << "|-- " << "Configuration hints : " << has_configuration_hints() << "\n";
    out_ << indent_ << "`-- " << "Validation support : " << has_validation_support() << "\n";
    return;
  }

  bool
  object_configuration_description::_validate_static(const configuration_property_description & cpd_,
                                                     const datatools::properties & config_,
                                                     std::string & error_message_) const
  {
    const std::string & prop_name = cpd_.get_name_pattern();
    // Check if the property is missing :
    if (config_.has_key(prop_name)) {
      if (cpd_.is_deprecated()) {
        error_message_ = "Property '" + prop_name + "' is deprecated";
        if (!cpd_.get_deprecated_info().empty()) {
          error_message_ += " (" + cpd_.get_deprecated_info() + ")";
        }
        error_message_ += "!";
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING, error_message_);
      }
      // Check type trait :
      if (cpd_.has_type()) {
        if (! _validate_traits(cpd_,config_, error_message_)) {
          return false;
        }
      }
    } else {
      // Check if the property is missing :
      // If flagged as mandatory :
      if (cpd_.is_mandatory()) {
        if (cpd_.has_complex_triggering_conditions()) {
          // Nothing more can be said here...
        } else {
          if (! cpd_.is_triggered_by_flag()) {
            error_message_ = "Mandatory property '" + prop_name + "' is missing !";
            return false;
          } else {
            // If the trigger property is missing, we are in trouble :
            if (config_.has_flag(cpd_.get_triggered_by_flag().get_name())) {
              error_message_ = "Mandatory property '" + prop_name + "' triggered by flag '"
                + cpd_.get_triggered_by_flag().get_name() + "' is missing !";
              return false;
            } else {
              // Nothing
            }
          }
        } // ! has_complex_triggering_conditions
      }
      // As the property does not exist in the container, there is nothing
      // left to check.
    } // ! has_key
    return true;
  }

  bool
  object_configuration_description::_validate_traits(const configuration_property_description & cpd_,
                                                     const datatools::properties & config_,
                                                     std::string & error_message_) const
  {
    const std::string & prop_name = cpd_.get_name_pattern();
    const properties::data & pd = config_.get(prop_name);
    if (pd.get_type() != cpd_.get_type()) {
      std::ostringstream message_oss;
      message_oss << "Invalid type for property '" << cpd_.get_name_pattern() << "'";
      message_oss << "; expected type is '" << cpd_.get_type() << "' but found '"
                  << pd.get_type() << "' !";
      error_message_ = message_oss.str();
      return false;
    }
    if (cpd_.is_real() && cpd_.has_explicit_unit()) {
      if (! pd.has_explicit_unit()) {
        std::ostringstream message_oss;
        message_oss << "No explicit unit was found for real property '" << cpd_.get_name_pattern() << "'";
        if (cpd_.has_unit_label()) {
          message_oss << "; expected unit label is '" << cpd_.get_unit_label() << "'";
        }
        message_oss << " !";
        error_message_ = message_oss.str();
        return false;
      }
    }
    // Check scalar/array trait :
    if (cpd_.is_array()) {
      if (! pd.is_vector()) {
        error_message_ = "Invalid array traits for property '"
          + cpd_.get_name_pattern() + "' !";
        return false;
      }
      // Check fixed sized array :
      if (cpd_.is_fixed_sized_array()) {
        if (cpd_.get_array_fixed_size() != (int)pd.get_size()) {
          std::ostringstream message_oss;
          message_oss << "Expected array size "
                      << cpd_.get_array_fixed_size()
                      << " for property '"
                      << cpd_.get_name_pattern() + "' "
                      << "but size=" << pd.get_size() << " was found "
                      << "!";
          error_message_ = message_oss.str();
          return false;
        }
      }
    } else {
      if (pd.is_vector()) {
        error_message_ = "Invalid scalar traits for property '"
          + cpd_.get_name_pattern() + "' !";
        return false;
      }
    } // end of scalar/array trait.
    return true;
  }

  bool object_configuration_description::validate(const datatools::properties & config_,
                                                  std::string & error_message_) const
  {
    DT_THROW_IF (! is_locked(),
                 std::logic_error,
                 "OCD object is not locked ! Cannot perform validation !");
    DT_THROW_IF (!has_validation_support(),
                 std::logic_error,
                 "OCD object has no validation support !");
    // Loop on all documented properties :
    for (size_t i = 0; i < _configuration_properties_infos_.size(); i++) {
      const configuration_property_description & cpd = _configuration_properties_infos_[i];

      // Process static properties :
      if (cpd.is_static()) {
        if (! _validate_static(cpd,config_, error_message_)) {
          return false;
        }
      } else {
        const configuration_property_description::dependency_entry & ddee
          = cpd.get_dynamic_dependee();
        const configuration_property_description & dee_ref = ddee.ref();
        std::vector<std::string> words;
        if (! config_.has_key(dee_ref.get_name_pattern())) {
          continue;
        }
        config_.fetch(dee_ref.get_name_pattern(), words);
        for (size_t iword = 0; iword < words.size(); iword++) {
          const std::string & word = words[iword];
          std::string dynprop_name = cpd.get_name_pattern();
          std::string token = "${"+dee_ref.get_name_pattern()+"}";
          boost::replace_first(dynprop_name, token, word);
          // std::cerr << "DEVEL: "
          //           << "Searching for '" << dynprop_name << "'"
          //           << std::endl;
          configuration_property_description temp_cpd = cpd;
          temp_cpd.set_name_pattern (dynprop_name);
          if (temp_cpd.has_type()) {
            if (! _validate_static(temp_cpd,config_, error_message_)) {
              return false;
            }
          }
        }
      } // end of dynamic properties
    } // end of loop on documented properties.

    return true;
  }

   void
   object_configuration_description::_generate_static_property(const configuration_property_description & cpd_,
                                                               properties & prop_) const
   {
     int array_sz= 2;
     if (cpd_.is_array()) {
       if (cpd_.is_fixed_sized_array()) {
         array_sz = cpd_.get_array_fixed_size();
       } else if (cpd_.has_default_array_size()) {
         array_sz = cpd_.get_default_array_size();
       }
     }
     // Boolean :
     if (cpd_.is_boolean()) {
       if (cpd_.is_scalar()) {
         bool default_value = false;
         if (cpd_.has_default_value()) {
           default_value = cpd_.get_default_value_boolean();
         }
         prop_.store_boolean(cpd_.get_name_pattern(),
                             default_value,
                             cpd_.get_terse_description(),
                             cpd_.is_const());
       } else {
         properties::data::vbool v;
         for (int ia = 0; ia < array_sz; ia++) {
           v.push_back(false);
         }
         prop_.store(cpd_.get_name_pattern(),
                     v,
                     cpd_.get_terse_description(),
                     cpd_.is_const());
       }
     }
     // Integer :
     if (cpd_.is_integer()) {
       if (cpd_.is_scalar()) {
         int default_value = 1;
         if (cpd_.has_default_value()) {
           default_value = cpd_.get_default_value_integer();
         }
         prop_.store_integer(cpd_.get_name_pattern(),
                             default_value,
                             cpd_.get_terse_description(),
                             cpd_.is_const());
       } else {
         properties::data::vint v;
         for (int ia = 0; ia < array_sz; ia++) {
           v.push_back(ia+1);
         }
         prop_.store(cpd_.get_name_pattern(),
                     v,
                     cpd_.get_terse_description(),
                     cpd_.is_const());
       }
     }
     // Real :
     if (cpd_.is_real()) {
       if (cpd_.is_scalar()) {
         double default_value = 1.1;
         if (cpd_.has_default_value()) {
           default_value = cpd_.get_default_value_real();
         }
         prop_.store_real(cpd_.get_name_pattern(),
                          default_value,
                          cpd_.get_terse_description(),
                          cpd_.is_const());
       } else {
         properties::data::vdouble v;
         for (int ia = 0; ia < array_sz; ia++) {
           v.push_back(ia*1.1);
         }
         prop_.store(cpd_.get_name_pattern(),
                     v,
                     cpd_.get_terse_description(),
                     cpd_.is_const());
       }
       if (cpd_.has_explicit_unit()) {
         prop_.set_explicit_unit(cpd_.get_name_pattern(),true);
       }
     }
     // String :
     if (cpd_.is_string()) {
       std::string token = "token";
       std::string ext = "";
       if (cpd_.is_path()) {
         token = "file";
         ext = ".ext";
       }
       if (cpd_.is_scalar()) {
         std::string default_value = token + ext;
         if (cpd_.has_default_value()) {
           default_value = cpd_.get_default_value_string();
         }
         prop_.store_string(cpd_.get_name_pattern(),
                            default_value,
                            cpd_.get_terse_description(),
                            cpd_.is_const());
       } else {
         properties::data::vstring v;
         for (int ia = 0; ia < array_sz; ia++) {
           std::ostringstream token_oss;
           token_oss << token << ia << ext;
           v.push_back(token_oss.str());
         }
         prop_.store(cpd_.get_name_pattern(),
                     v,
                     cpd_.get_terse_description(),
                     cpd_.is_const());
       }
       if (cpd_.is_path()) {
         prop_.set_explicit_path(cpd_.get_name_pattern(),true);
       }
     }

     return;
   }

  void
  object_configuration_description::generate_sample_configuration(std::ostream & out_,
                                                                  const std::string& /*topic_*/,
                                                                  uint32_t sgo_flags_) const
  {
    properties PROP;
    PROP.set_description (std::string("Sample configuration for class ") + get_class_name());

    // Not fully implemented yet !!
    properties::config PC(properties::config::SMART_MODULO
                          | properties::config::HEADER_FOOTER);

    // PC.write_header(out_, topic_);
    const std::string prop_comment_prefix = "### ";

    out_ << '\n';
    out_ << "# Lines starting with '" << prop_comment_prefix
         << "' indicate commented out\n"
         << "# optional properties that may be activated under             \n"
         << "# specific conditions.                                        \n";
    out_ << "# Lines starting with '" << "#@" << "' are special directives \n";
    out_ << "# that may be interpreted by parsers as useful metadata.      \n";
    out_ << "# Lines starting with a lone '" << '#' << "' are simple comments. \n";
    out_ << '\n';

    for (size_t i = 0; i < _configuration_properties_infos_.size(); i++) {
      const configuration_property_description & cpd = _configuration_properties_infos_[i];

      // Process static property :
      if (cpd.is_static()) {

        _generate_static_property(cpd, PROP);

        if (! PROP.has_key(cpd.get_name_pattern())) {
          // Undocumented property : we pass
          continue;
        }

        // Output :
        const properties::data& a_prop_data = PROP.get(cpd.get_name_pattern());
        out_ << '\n';
        bool comment = true;
        if (cpd.is_mandatory()) {
          comment = false;
        }
        PC.write_data(out_,
                      cpd.get_name_pattern(),
                      a_prop_data,
                      cpd.get_unit_symbol(),
                      cpd.get_unit_label(),
                      comment ? prop_comment_prefix : "");

        bool more = false;
        // Append long description as a trailing comment block :
        if (cpd.has_long_description()) {
          more = true;
        }

        if (more and ! (sgo_flags_ & sgo_no_add_infos)) {
          out_ << "\n# Additional informations : " << '\n';
          if (cpd.is_deprecated()) {
            out_ << "#   " << "This property is deprecated";
            if (!cpd.get_deprecated_info().empty()) {
              out_ << " (" << cpd.get_deprecated_info() << ")";
            }
            out_ << ".\n";
          }
          if (cpd.has_complex_triggering_conditions()) {
            out_ << "#   " << "This property has complex triggering conditions.\n";
          }
          if (cpd.has_complex_dependencies()) {
            out_ << "#   " << "This property has complex dependencies.\n";
          }
          if (cpd.is_triggered_by_flag()) {
            out_ << "#   " << "This property";
            if (cpd.is_mandatory()) {
              out_ << " is expected";
            } else {
              out_ << " may be set";
            }
            out_ << " if dependee flag property '"
                 << cpd.get_triggered_by_flag().get_name() << "' is set to '"
                 << cpd.get_triggered_by_flag().triggering_status << "'"
                 << "\n";
          }
          if (cpd.is_triggered_by_label()) {
            out_ << "#   " << "This property";
            if (cpd.is_mandatory()) {
              out_ << " is expected";
            } else {
              out_ << " may be set";
            }
            out_ << " if dependee string property '"
                 << cpd.get_triggered_by_label().get_name() << "' is set to one of these values : \n";
            for (size_t itl = 0; itl < cpd.get_triggered_by_label().triggering_labels.size(); itl++) {
              out_ << "#    '" << cpd.get_triggered_by_label().triggering_labels[itl] << "'\n";
            }
          }
          if (cpd.has_long_description()) {
            datatools::print_multi_lines(out_, cpd.get_long_description(), "#   ");
          }
        }

        // Append dynamic depender properties :
        if (cpd.has_dynamic_dependers()) {
          std::vector<std::string> words;
          PROP.fetch(cpd.get_name_pattern(), words);
          for (size_t idder = 0; idder < cpd.get_number_of_dynamic_dependers(); idder++) {
            const configuration_property_description::dependency_entry & dder
              = cpd.get_dynamic_depender(idder);
            //std::cerr << "DEVEL: " << "Depender = '" << dder.get_name() << "' " << '\n';
            for (size_t iword = 0; iword < words.size(); iword++) {
              const std::string & word = words[iword];
              //std::cerr << "DEVEL: " << "  word = '" << word << "' " << '\n';
              std::string dynprop_name = dder.get_name();
              std::string token = "${"+cpd.get_name_pattern()+"}";
              boost::replace_first(dynprop_name, token, word);
              configuration_property_description temp_cpd = dder.ref();
              temp_cpd.set_name_pattern (dynprop_name);
              //std::cerr << "DEVEL: " << "  dynprop_name = '" << dynprop_name << "' " << '\n';
              if (temp_cpd.has_type()) {
                _generate_static_property(temp_cpd, PROP);

                if (! PROP.has_key(temp_cpd.get_name_pattern())) {
                  // Undocumented property : we pass
                  continue;
                }

                // Output :
                const properties::data& a_dyn_prop_data = PROP.get(temp_cpd.get_name_pattern());
                out_ << '\n';
                PC.write_data(out_,
                              temp_cpd.get_name_pattern(),
                              a_dyn_prop_data,
                              temp_cpd.get_unit_symbol(),
                              temp_cpd.get_unit_label(),
                              temp_cpd.is_mandatory() ? "" : prop_comment_prefix);

                // Append long description as a trailing comment block :
                if (! (sgo_flags_ & sgo_no_add_infos)) {
                  if (temp_cpd.has_long_description()) {
                    out_ << "\n# Additional informations : " << '\n';
                    datatools::print_multi_lines(out_,temp_cpd.get_long_description(), "#   ");
                  } // end of trailing comment block.
                }
              } // has type
            } // word loop
          } // depender loop
        } // dynamic depender

        // if (cpd.is_trigger()) {
        //   for (int itder = 0; itder < cpd.get_number_of_triggered_dependers(); itder++) {
        //     const configuration_property_description::dependency_entry & tder
        //       = cpd.get_triggered_depender(itder);
        //   }
        // } // depender loop

      } // end of static properties
    }

    if (! (sgo_flags_ & sgo_no_config_hints)) {
      if (has_configuration_hints()) {
        out_ << '\n';
        out_ << "# " << "Special configuration hints : " << '\n';
        out_ << "# " << '\n';
        datatools::print_multi_lines(out_, get_configuration_hints(), "#   ");
      }
    }

    out_ << '\n';
    // PC.write_footer(out_, topic_);
    return;
  }

} // namespace datatools
