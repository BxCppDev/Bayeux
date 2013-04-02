// -*- mode: c++; -*- 
// object_configuration_description.cc

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/scoped_ptr.hpp>

// Ourselves:
#include <datatools/object_configuration_description.h>
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>

namespace datatools {

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
    reset();
    return;
  }

  /***************************************************************/

  bool configuration_property_description::is_const() const
  {
    return _const_;
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
    _mandatory_ = false;
    _complex_triggering_conditions_ = false;
    _complex_dependencies_ = false;
    return;
  }

  configuration_property_description & 
  configuration_property_description::set_name_pattern(const std::string &np_)
  {
    _name_pattern_ = np_;
    std::vector<std::string> strs;
    boost::split(strs,np_,boost::is_any_of("."));
    for (int i = 0; i < strs.size(); i++)
      {
        const std::string & str = strs[i];
        if (str.empty()) {
          std::ostringstream message;
          message << "datatools::configuration_property_description::set_name_pattern: "
                  << "Invalid property name pattern '" << np_ << "' !"
            ;
          throw std::logic_error(message.str());
        }
        if (str[0] == '$') {
          // Minimal token pattern is "${a}"
          if (str.length() < 4) {
            std::ostringstream message;
            message << "datatools::configuration_property_description::set_name_pattern: "
                    << "Invalid format for dynamic property name pattern '" << np_ << "' !"
              ;
            throw std::logic_error(message.str());
          }
          if (str[1] != '{') {
            std::ostringstream message;
            message << "datatools::configuration_property_description::set_name_pattern: "
                    << "Invalid format for dynamic property name pattern '" << np_ << "' !"
              ;
            throw std::logic_error(message.str());
          }
          if (str[str.length()-1] != '}') {
            std::ostringstream message;
            message << "datatools::configuration_property_description::set_name_pattern: "
                    << "Invalid format for dynamic property name pattern '" << np_ << "' !"
              ;
            throw std::logic_error(message.str());
          }
          if (_dynamic_dependee_.has_name())
            {
              std::ostringstream message;
              message << "datatools::configuration_property_description::set_name_pattern: "
                      << "Dependee '" << _dynamic_dependee_.name << "' has already been defined ! "
                      << "Only one dependee is allowed for dynamic property name pattern '" << np_ << "' !"
                ;
              throw std::logic_error(message.str());            
            }
          std::string source_prop_name = str.substr(2,str.length()-3);
          _dynamic_dependee_.type = DEP_DYNAMIC;
          _dynamic_dependee_.name = source_prop_name;
          _dynamic_dependee_.address = 0;
        }
      }

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
    if (!is_real() && xu_)
      {
        std::ostringstream message;
        message << "datatools::configuration_property_description::set_name_pattern: "
                << "Explicit unit is only supported by 'real' properties but the property with name pattern '" << _name_pattern_ << "' is not a 'real' !"
          ;
        throw std::logic_error(message.str());        
      }
    _explicit_unit_ = xu_;
    return *this;
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
    if (! is_string()) {
      std::ostringstream message;
      message << "datatools::configuration_property_description::set_path: "
              << "Explicit PATH is only supported by 'string' properties but the property with name pattern '" << _name_pattern_ << "' is not a string' !"
        ;
      throw std::logic_error(message.str());        
    }
    _path_ = p_;
    return *this;    
  }

  configuration_property_description & 
  configuration_property_description::set_unit_label(const std::string & ul_)
  {
    if (!is_real() && !ul_.empty())
      {
        std::ostringstream message;
        message << "datatools::configuration_property_description::set_unit_label: "
                << "Unit label is only supported by 'real' properties but the property with name pattern '" << _name_pattern_ << "' is not a 'real' !"
          ;
        throw std::logic_error(message.str());        
      }
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
    if (!is_real() && !is_array() && !us_.empty())
      {
        std::ostringstream message;
        message << "datatools::configuration_property_description::set_unit_symbol: "
                << "Unit symbol is only supported by 'real' array properties but the property with name pattern '" << _name_pattern_ << "' is not a array of 'real' !"
          ;
        throw std::logic_error(message.str());        
      }
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
    //&& _triggered_by_label_.triggering_labels.size() != 0;
  }

  configuration_property_description & 
  configuration_property_description::set_traits(int type_, 
                                                 bool array_, 
                                                 int fixed_size_)
  {
    if (type_ < TYPE_BOOLEAN 
        || type_ > TYPE_STRING)
      {
        std::ostringstream message;
        message << "datatools::configuration_property_description::set_name_pattern: "
                << "Invalid type for property with name pattern '" << _name_pattern_ << "' !"
          ;
        throw std::logic_error(message.str());        
      }
    _type_ = type_;
    _array_ = array_;
    if (fixed_size_ >= 0 && !_array_)
      {
        std::ostringstream message;
        message << "datatools::configuration_property_description::set_name_pattern: "
                << "Invalid array fixed size directive makes no sense for a scalar property !"
          ;
        throw std::logic_error(message.str());        
      }
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

  void configuration_property_description::print(std::ostream &out_, 
                                                 const std::string & indent_) const
  {
    out_ << indent_ << get_name_pattern() << " ";
    if (! has_type()) {
      out_ << "(undefined type) : ";
    }
    else {
      out_ << "("
           << ::datatools::get_label_from_type(_type_);
      if (is_array()) {
        out_ << '[';
        if (is_fixed_sized_array())
          out_ << get_array_fixed_size();
        else
          out_ << '*';
        out_ << ']';
      }
      out_ << ") : ";
      if (is_dynamic()) {
        out_ << "[dynamic";
        if (is_mandatory()) out_ << " mandatory";
        out_ << " property depending on values of property ";
        out_ << " '" << _dynamic_dependee_.name << "'";
        out_ << "]";
      }
      else {
        out_ << "[static";
        if (is_mandatory()) out_ << " mandatory";
        out_ << " property]";
      }
    }
    out_ << std::endl;
    if (has_terse_description()) {
      print_multi_lines(out_, _terse_description_, indent_+"  ");
    }
    if (has_long_description()) {
      out_ << indent_ << std::endl;
      print_multi_lines(out_, _long_description_, indent_+"  ");
    }
    return;
  }


  void configuration_property_description::dump(std::ostream & out_, 
                                                const std::string & title_,
                                                const std::string & indent_) const
  {
    if (! title_.empty())
      {
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
      }
      else {
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
    }
    else {
      if (is_static() && _dynamic_dependers_.size()) {
        out_ << indent_ << "|-- " << "Dependers : " << _dynamic_dependers_.size() << "\n";    
        for (int i = 0; i < _dynamic_dependers_.size(); i++) {
          out_ << indent_ << "|-- ";
          if (i == _dynamic_dependers_.size() - 1) out_ << "`-- ";
          else out_ << "|-- ";
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
    }
    else if (_triggering_.size()) {
      out_ << indent_ << "|-- " << "Triggering : " << _triggering_.size() << "\n";    
      for (int i = 0; i < _triggering_.size(); i++) {
        out_ << indent_ << "|-- ";
        if (i == _triggering_.size() - 1) out_ << "`-- ";
        else out_ << "|-- ";
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
        for (int itl = 0; itl < _triggered_by_label_.triggering_labels.size(); itl++) {
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

  /************************************************************************************/

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
  object_configuration_description::set_configuration_hints(const std::string & ch_)
  {
    _configuration_hints_ = ch_;
    return *this;
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
 
  bool object_configuration_description::has_class_description() const
  {
    return !_class_description_.empty();
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
                                               const std::string & indent_) const
  {
    out_ << indent_ << "Class name          : '" << get_class_name () << "'" << std::endl;
    if (has_class_description()) {
      out_ << indent_ << "Class description   : '" << get_class_description () << "'" << std::endl;
    }
    //out_ << indent_ << "Status              : '" << _locked_ << "'" << std::endl;
    out_ << indent_ << "Description of configuration properties : " << std::endl;
    for (int i = 0; i < _properties_infos_.size(); i++) {
      out_ << indent_ << std::endl;
      const configuration_property_description & cpd = _properties_infos_[i];
      cpd.print(out_, indent_ + "  ");
    }
    if (has_configuration_hints()) {
      out_ << indent_ << std::endl;
      out_ << indent_ << "Configuration hints : " << std::endl;
      print_multi_lines(out_, _configuration_hints_, indent_ + "  ");
    }
    return;
  }

  unsigned int 
  object_configuration_description::get_number_of_documented_properties() const
  {
    return _properties_infos_.size();
  }

  const configuration_property_description & 
  object_configuration_description::get_property_info(int i_) const
  {
    return _properties_infos_[i_];
  }
  
  configuration_property_description & 
  object_configuration_description::add_property_info()
  {
    {
      configuration_property_description dummy;
      _properties_infos_.push_back(dummy);
    }
    return _properties_infos_.back();
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
    // Establish interdependencies between dependees/dependers dynamic properties :
    for (int i = 0; i < _properties_infos_.size(); i++) {
      configuration_property_description & cpd = _properties_infos_[i];
      if (cpd.is_static()) {
        continue;
      }
      configuration_property_description::dependency_entry & dde = cpd._dynamic_dependee_;
      bool found_dependency = false;
      for (int k = 0; k < _properties_infos_.size(); k++) {
        if (k == i) continue;
        configuration_property_description & cpd2 = _properties_infos_[k];
        // std::cerr << "DEVEL: " << "datatools::object_configuration_description::_at_lock_: "
        //           << "Dependee's name : '" << dde.name << "' " 
        //           << "versus checked name : '" << cpd2.get_name_pattern() << "' " 
        //           << "\n";
        if (dde.name == cpd2.get_name_pattern()) {
          if (cpd2.is_dynamic()) {
            std::ostringstream message;
            message << "datatools::object_configuration_description::_at_lock_: "
                    << "Dynamic property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                    << "the dynamic property '" << cpd2.get_name_pattern() << "' !";
            throw std::logic_error(message.str());
          }
          if (! cpd2.is_string()) {
            std::ostringstream message;
            message << "datatools::object_configuration_description::_at_lock_: "
                    << "Dynamic property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                    << "the non-string property '" << cpd2.get_name_pattern() << "' !";
            throw std::logic_error(message.str());
          }
          if (cpd2.is_path()) {
            std::ostringstream message;
            message << "datatools::object_configuration_description::_at_lock_: "
                    << "Dynamic property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                    << "the path string property '" << cpd2.get_name_pattern() << "' !";
            throw std::logic_error(message.str());
          }
          if (! cpd2.is_array()) {
            std::ostringstream message;
            message << "datatools::object_configuration_description::_at_lock_: "
                    << "Dynamic property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                    << "the non-array property '" << cpd2.get_name_pattern() << "' !";
            throw std::logic_error(message.str());
          }
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
      if (!dde.has_address()) {
        std::ostringstream message;
        message << "datatools::object_configuration_description::_at_lock_: "
                << "Cannot find dependency '" 
                << dde.name
                << "' for dynamic property with name pattern '" << cpd.get_name_pattern() << "' !";
        throw std::logic_error(message.str());
      }
    }

    // Establish interdependencies between triggering/triggered properties (BY FLAG):
    for (int i = 0; i < _properties_infos_.size(); i++) {
      configuration_property_description & cpd = _properties_infos_[i];
      if (! cpd.is_triggered_by_flag()) {
        continue;
      }
      configuration_property_description::dependency_entry & trigger = cpd._triggered_by_flag_;
      bool found_trigger = false;
      for (int k = 0; k < _properties_infos_.size(); k++) {
        if (k == i) continue;
        configuration_property_description & cpd2 = _properties_infos_[k];
        // std::cerr << "DEVEL: " << "datatools::object_configuration_description::_at_lock_: "
        //           << "Trigger's name : '" << trigger.name << "' " 
        //           << "versus checked name : '" << cpd2.get_name_pattern() << "' " 
        //           << "\n";
        if (trigger.name == cpd2.get_name_pattern()) {
          if (cpd2.is_dynamic()) {
            std::ostringstream message;
            message << "datatools::object_configuration_description::_at_lock_: "
                    << "Triggered property with name pattern '" << cpd.get_name_pattern() << "' cannot be triggered by "
                    << "the dynamic property '" << cpd2.get_name_pattern() << "' !";
            throw std::logic_error(message.str());
          }

          if (! cpd2.is_boolean()) {
            std::ostringstream message;
            message << "datatools::object_configuration_description::_at_lock_: "
                    << "Triggered property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                    << "the non-boolean property '" << cpd2.get_name_pattern() << "' !";
            throw std::logic_error(message.str());
          }

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
      if (!trigger.has_address()) {
        std::ostringstream message;
        message << "datatools::object_configuration_description::_at_lock_: "
                << "Cannot find trigger property '" 
                << trigger.name
                << "' for property with name pattern '" << cpd.get_name_pattern() << "' !";
        throw std::logic_error(message.str());
      }
    }

    // Establish interdependencies between triggering/triggered properties (BY LABEL):
    for (int i = 0; i < _properties_infos_.size(); i++) {
      configuration_property_description & cpd = _properties_infos_[i];
      if (! cpd.is_triggered_by_label()) {
        continue;
      }
      configuration_property_description::dependency_entry & trigger = cpd._triggered_by_label_;
      bool found_trigger = false;
      for (int k = 0; k < _properties_infos_.size(); k++) {
        if (k == i) continue;
        configuration_property_description & cpd2 = _properties_infos_[k];
        // std::cerr << "DEVEL: " << "datatools::object_configuration_description::_at_lock_: "
        //           << "Trigger's name : '" << trigger.name << "' " 
        //           << "versus checked name : '" << cpd2.get_name_pattern() << "' " 
        //           << "\n";
        if (trigger.name == cpd2.get_name_pattern()) {
          if (cpd2.is_dynamic()) {
            std::ostringstream message;
            message << "datatools::object_configuration_description::_at_lock_: "
                    << "Triggered property with name pattern '" << cpd.get_name_pattern() << "' cannot be triggered by "
                    << "the dynamic property '" << cpd2.get_name_pattern() << "' !";
            throw std::logic_error(message.str());
          }

          if (! cpd2.is_string()) {
            std::ostringstream message;
            message << "datatools::object_configuration_description::_at_lock_: "
                    << "Triggered property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                    << "the non-string property '" << cpd2.get_name_pattern() << "' !";
            throw std::logic_error(message.str());
          }

          if (cpd2.is_path()) {
           std::ostringstream message;
            message << "datatools::object_configuration_description::_at_lock_: "
                    << "Triggered property with name pattern '" << cpd.get_name_pattern() << "' cannot depend on "
                    << "the path string property '" << cpd2.get_name_pattern() << "' !";
            throw std::logic_error(message.str());
          }
 
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
      if (!trigger.has_address()) {
        std::ostringstream message;
        message << "datatools::object_configuration_description::_at_lock_: "
                << "Cannot find trigger property '" 
                << trigger.name
                << "' for property with name pattern '" << cpd.get_name_pattern() << "' !";
        throw std::logic_error(message.str());
      }
    }
    return;
  }

  void object_configuration_description::dump(std::ostream & out_, 
                                              const std::string & title_,
                                              const std::string & indent_) const
  {
    if (! title_.empty())
      {
        out_ << indent_ << title_ << '\n';
      }
    out_ << indent_ << "|-- " << "Status : " << (is_locked()?"Locked":"Unlocked") << "'\n";
    out_ << indent_ << "|-- " << "Class name : '" << _class_name_ << "'\n";
    out_ << indent_ << "|-- " << "Class description : '" << _class_description_ << "'\n";
    out_ << indent_ << "|-- " << "Configuration properties : \n";
    for (int i = 0; i < _properties_infos_.size(); i++) {
      out_ << indent_ << "|   ";
      std::string tag = "|   ";
      if (i ==  _properties_infos_.size() - 1) {
        out_ << "`-- ";
        tag = "    ";
      }
      else {
        out_ << "|-- ";
      }
      out_ << "Configuration property #" << i << " : \n";
      const configuration_property_description & cpd = _properties_infos_[i];
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
      // Check type trait :
      if (cpd_.has_type()) {
        if (! _validate_traits(cpd_,config_, error_message_)) {
          return false;
        }
      }
    }
    else {
      // Check if the property is missing : 
      // If flagged as mandatory :
      if (cpd_.is_mandatory()) {
        if (cpd_.has_complex_triggering_conditions()) {
          // Nothing more can be said here...
        }
        else {
          if (! cpd_.is_triggered_by_flag()) {
            error_message_ = "Mandatory property '" + prop_name + "' is missing !";
            return false;
          }
          else {
            // If the trigger property is missing, we are in trouble :
            if (config_.has_flag(cpd_.get_triggered_by_flag().get_name())) {
              error_message_ = "Mandatory property '" + prop_name + "' triggered by flag '"
                + cpd_.get_triggered_by_flag().get_name() + "' is missing !";
              return false;
            }
            else {
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
        if (cpd_.get_array_fixed_size() != pd.get_size()) {
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
    }
    else {
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
    if (! is_locked()) {
      throw std::logic_error("datatools::object_configuration_description::validate: OCD object is not locked ! Cannot perform validation !");
    } 
    if (!has_validation_support()) {
      throw std::logic_error("datatools::object_configuration_description::validate: OCD object has no validation support !");
    } 
    // Loop on all documented properties :
    for (int i = 0; i < _properties_infos_.size(); i++) {
      const configuration_property_description & cpd = _properties_infos_[i];

      // Process static properties :
      if (cpd.is_static()) {
        if (! _validate_static(cpd,config_, error_message_)) {
          return false;
        }
      } // end of static properties
      else {
        const configuration_property_description::dependency_entry & ddee 
          = cpd.get_dynamic_dependee();
        const configuration_property_description & dee_ref = ddee.ref();
        std::vector<std::string> words;
        if (! config_.has_key(dee_ref.get_name_pattern())) {
          continue;
        }
        config_.fetch(dee_ref.get_name_pattern(), words);
        for (int iword = 0; iword < words.size(); iword++) {
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
       } 
     }
     // Boolean :
     if (cpd_.is_boolean()) {
       if (cpd_.is_scalar()) {
         prop_.store_boolean(cpd_.get_name_pattern(), 
                             false, 
                             cpd_.get_terse_description(),
                             cpd_.is_const());
       }
       else {
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
         prop_.store_integer(cpd_.get_name_pattern(), 
                             1, 
                             cpd_.get_terse_description(),
                             cpd_.is_const());
       }
       else {
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
         prop_.store_real(cpd_.get_name_pattern(), 
                          1.1, 
                          cpd_.get_terse_description(),
                          cpd_.is_const());
       }
       else {
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
         ext = ".data";
       }
       if (cpd_.is_scalar()) {
         prop_.store_string(cpd_.get_name_pattern(), 
                            token + ext, 
                            cpd_.get_terse_description(),
                            cpd_.is_const());
       }
       else {
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
                                                                  const std::string & topic_) const
  {
    properties PROP;
    PROP.set_description (std::string("Sample configuration for class ") + get_class_name());

    // Not fully implemented yet !!
    properties::config PC(properties::config::with_smart_modulo,
                          properties::config::mode_header_footer, 
                          properties::config::write_private_also);

    PC.write_header(out_, topic_);
    const std::string prop_comment_prefix = "### ";

    out_ << '\n';
    out_ << "# Lines starting with '" << prop_comment_prefix 
         << "' indicate optional properties\n"
         << "# that may be activated under specific conditions.\n"; 
    out_ << "# Lines starting with '" << "#@" << "' are special directives.\n";
    out_ << "# Lines starting with '" << '#' << "' are simple comments.\n";
    out_ << '\n';
    
    for (int i = 0; i < _properties_infos_.size(); i++) {
      const configuration_property_description & cpd = _properties_infos_[i];

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

        if (more) {
          out_ << "#\n# Additional informations : " << '\n';
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
            }
            else {
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
            }
            else {
              out_ << " may be set";
            }
            out_ << " if dependee string property '" 
                 << cpd.get_triggered_by_label().get_name() << "' is set to one of these values : \n";
            for (int itl = 0; itl < cpd.get_triggered_by_label().triggering_labels.size(); itl++) {
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
          for (int idder = 0; idder < cpd.get_number_of_dynamic_dependers(); idder++) {
            const configuration_property_description::dependency_entry & dder 
              = cpd.get_dynamic_depender(idder);
            //std::cerr << "DEVEL: " << "Depender = '" << dder.get_name() << "' " << '\n';
            for (int iword = 0; iword < words.size(); iword++) {
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
                if (temp_cpd.has_long_description()) {
                  out_ << "#\n# Additional informations : " << '\n';
                  datatools::print_multi_lines(out_,temp_cpd.get_long_description(), "#   ");
                } // end of trailing comment block.
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

    if (has_configuration_hints()) {
      out_ << '\n';
      out_ << "# " << "Special configuration hints : " << '\n';
      out_ << "# " << '\n';
      datatools::print_multi_lines(out_, get_configuration_hints(), "#   ");
    } 
    
    out_ << '\n';
    PC.write_footer(out_, topic_);
    return;
  }

  /*******************************************************************************/

} // namespace datatools

// end of object_configuration_description.cc
