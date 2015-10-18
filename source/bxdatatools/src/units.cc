/* units.cc
 *
 * All units from "include/CLHEP/Units/SystemOfUnits.h"
 *
 */

// Ourselves:
#include <datatools/units.h>

// Standard Library:
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <limits>
#include <list>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/replace_if.hpp>

// This Project:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/exception.h>

namespace datatools {

  namespace units {

    size_t registered_unit_dimension_labels(std::vector<std::string> & dimension_labels_)
    {
      dimension_labels_.clear();
      const registry & sys_reg = registry::const_system_registry();
      const registry::dimension_dict_type & dims = sys_reg.get_dimensions();
      dimension_labels_.reserve(dims.size());
      for (registry::dimension_dict_type::const_iterator i = dims.begin();
           i != dims.end();
           i++) {
        dimension_labels_.push_back(i->first);
      }
      return dimension_labels_.size();
    }

    size_t registered_unit_symbols(std::vector<std::string> & unit_symbols_)
    {
      unit_symbols_.clear();
      const registry & sys_reg = registry::const_system_registry();
      const registry::symbol_dict_type & s = sys_reg.get_symbols();
      unit_symbols_.reserve(s.size());
      for (registry::symbol_dict_type::const_iterator i = s.begin();
           i != s.end();
           i++) {
        unit_symbols_.push_back(i->first);
      }
      return unit_symbols_.size();
    }

    size_t registered_unit_names(std::vector<std::string> & unit_names_)
    {
      unit_names_.clear();
      const registry & sys_reg = registry::const_system_registry();
      const registry::unit_dict_type & u = sys_reg.get_units();
      unit_names_.reserve(u.size());
      for (registry::unit_dict_type::const_iterator i = u.begin();
           i != u.end();
           i++) {
        unit_names_.push_back(i->first);
      }
      return unit_names_.size();
    }

    double get_unit_in_dimension_from(const std::string & unit_id_,
                                      const std::string & dimension_label_) {
      const unit & the_unit = registry::const_system_registry().get_unit_from_any(unit_id_);
      DT_THROW_IF(!the_unit.is_dimension(dimension_label_), std::logic_error,
                  "Unit '" << unit_id_ << "' is not of " << dimension_label_ << " dimension !");
      return the_unit.get_value();
    }

    bool is_unit_in_dimension_from(const std::string& unit_id_,
                                  const std::string& unit_dimension_label_)
    {
      const unit & the_unit = registry::const_system_registry().get_unit_from_any(unit_id_);
      return the_unit.is_dimension(unit_dimension_label_);
    }

    double get_length_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "length");
    }


    double get_surface_unit_from(const std::string& unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "surface");
    }


    double get_volume_unit_from(const std::string& unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "volume");
    }


    double get_time_unit_from(const std::string& unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "time");
    }


    double get_angle_unit_from(const std::string& unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "angle");
    }


    double get_solid_angle_unit_from(const std::string& unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "solid_angle");
    }


    double get_energy_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "energy");
    }


    double get_mass_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "mass");
    }


    double get_pressure_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "pressure");
    }

    double get_magnetic_flux_density_unit_from(const std::string & unit_id_) {
     return get_unit_in_dimension_from(unit_id_, "magnetic_flux_density");
    }

    double get_magnetic_field_unit_from(const std::string & unit_id_) {
     return get_unit_in_dimension_from(unit_id_, "magnetic_flux_density");
     /*
       if ((word == "T") || (word == "tesla")) return CLHEP::tesla;
      if ((word == "G") || (word == "gauss")) return CLHEP::gauss;
      if ((word == "kG") || (word == "kilogauss")) return CLHEP::kilogauss;
      DT_THROW_BAD_UNIT("magnetic field", word);
      // return std::numeric_limits<double>::quiet_NaN();
      */
    }


    double get_electric_field_unit_from(const std::string & unit_id_) {
     return get_unit_in_dimension_from(unit_id_, "electric_field");
    }

    double get_electric_potential_unit_from(const std::string & unit_id_) {
     return get_unit_in_dimension_from(unit_id_, "electric_potential");
    }

    double get_electric_tension_unit_from(const std::string & unit_id_) {
     return get_unit_in_dimension_from(unit_id_, "electric_potential");
    }


    double get_temperature_unit_from(const std::string & unit_id_) {
     return get_unit_in_dimension_from(unit_id_, "temperature");
    }


    double get_density_unit_from(const std::string & unit_id_) {
     return get_unit_in_dimension_from(unit_id_, "density");
    }

    double get_frequency_unit_from(const std::string & unit_id_) {
     return get_unit_in_dimension_from(unit_id_, "frequency");
    }


    double get_activity_unit_from(const std::string & unit_id_) {
     return get_unit_in_dimension_from(unit_id_, "activity");
   }


    double get_volume_activity_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "volume_activity");
    }


    double get_surface_activity_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "surface_activity");
    }


    double get_electric_charge_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "electric_charge");
    }


    double get_electric_current_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "electric_current");
    }


    double get_mass_activity_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "mass_activity");
    }


    double get_velocity_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "velocity");
    }

    double get_speed_unit_from(const std::string & unit_id_) {
      return get_unit_in_dimension_from(unit_id_, "velocity");
    }


    double get_unit_from(const std::string& unit_type,
                         const std::string& unit_str) {
      return get_unit_in_dimension_from(unit_str, unit_type);
    }


    bool is_unit_label_valid(const std::string & unit_label)
    {
      return registry::const_system_registry().has_dimension(unit_label);
    }

    std::string get_default_unit_symbol_from_label(const std::string & unit_type)
    {
      const unit_dimension & the_unit_dim = registry::const_system_registry().get_dimension(unit_type);
      const std::string & unit_name = the_unit_dim. get_default_unit_name();
      const unit & the_unit = registry::const_system_registry().get_unit(unit_name);
      return the_unit.get_main_symbol();
    }

    bool find_unit(const std::string & unit_str,
                   double & unit_value,
                   std::string & unit_label)
    {
      unit_label = "";
      unit_value = std::numeric_limits<double>::quiet_NaN();
      const registry & sys_unit_reg = registry::const_system_registry();
      const unit * found_unit = 0;
      if (sys_unit_reg.has_symbol(unit_str)) {
        // Search symbols:
        found_unit = &sys_unit_reg.get_unit_from_symbol(unit_str);
      } else if (sys_unit_reg.has_unit(unit_str)) {
        // Search names:
        found_unit = &sys_unit_reg.get_unit(unit_str);
      }
      if (found_unit) {
        unit_value = found_unit->get_value();
        unit_label = found_unit->get_dimension_label();
        return true;
      }
      /*
      const std::vector<std::string>& ulabels = get_unit_labels_registry();
      double val = std::numeric_limits<double>::quiet_NaN();
      int count = -1;
      for (std::vector<std::string>::const_iterator i = ulabels.begin();
           i != ulabels.end();
           ++i) {
        try {
          count++;
          unit_label = *i;
          val = get_unit_from(*i, unit_str);
          break;
        }
        catch (std::exception& x) {
          val = std::numeric_limits<double>::quiet_NaN();
        }
      }

      if (!std::isnormal(val)) {
        unit_label = "";
        unit_value = val;
        return false;
      }
      unit_label = ulabels[count];
      unit_value = val;
      return true;
      */
      return false;
    }

    double get_unit(const std::string& unit_str_, bool throw_)
    {
      double unit_val;
      std::string unit_label;
      bool res = find_unit(unit_str_, unit_val, unit_label);
      if (!res) {
        DT_THROW_IF(throw_, std::logic_error, "Unknow unit '" << unit_str_ << "'!");
        return std::numeric_limits<double>::quiet_NaN();
      }
      return unit_val;
    }

    bool parse_value_with_unit(const std::string & word,
                               double & value,
                               std::string & unit_symbol,
                               std::string & unit_label)
    {
      value = std::numeric_limits<double>::quiet_NaN();
      unit_label = "";
      double val;
      std::istringstream iss(word);
      iss >> val;
      if (!iss) {
        // std::ostringstream message;
        // message << "datatools::find_value_with_unit: Format error while reading a double value !";
        return false;
      }
      iss >> std::ws;
      if (!iss.eof()) {
        std::string ustr;
        iss >> ustr;
        if (ustr.empty()) return false;
        double any_unit_value;
        std::string any_unit_label;
        bool res = find_unit(ustr, any_unit_value, any_unit_label);
        if (!res) return false;
        val *= any_unit_value;
        unit_symbol = ustr;
        unit_label = any_unit_label;
      }
      value = val;
      return true;
    }

    bool find_value_with_unit(const std::string & word,
                              double & value,
                              std::string & unit_label,
                              double default_unit_)
    {
      value = std::numeric_limits<double>::quiet_NaN();
      unit_label = "";
      double val;
      std::istringstream iss(word);
      iss >> val;
      if (!iss) {
        // std::ostringstream message;
        // message << "datatools::find_value_with_unit: Format error while reading a double value !";
        return false;
      }
      bool use_default_unit = true;
      iss >> std::ws;
      if (!iss.eof()) {
        std::string ustr;
        iss >> ustr;
        if (ustr.empty()) return false;
        double any_unit_value;
        std::string any_unit_label;
        bool res = find_unit(ustr, any_unit_value, any_unit_label);
        if (!res) return false;
        val *= any_unit_value;
        unit_label = any_unit_label;
        use_default_unit = false;
      }
      if (datatools::is_valid(default_unit_) && use_default_unit) {
        val *= default_unit_;
      }
      value = val;
      return true;
    }


    double get_value_with_unit(const std::string& word) {
      double unit_value = std::numeric_limits<double>::quiet_NaN();
      std::string unit_label;
      DT_THROW_IF (!find_value_with_unit(word, unit_value, unit_label),
                   std::logic_error,
                   "Cannot parse a value from '" << word << "' with its units from '" << word << "' !");
      return unit_value;
    }

    /* -------------------*/

    void unit::_set_default()
    {
      datatools::invalidate(_value_);
      _SI_main_ = false;
      return;
    }

    unit::unit()
    {
      _set_default();
      return;
    }

    unit::unit(const std::string & name_,
               const std::string & dimension_infos_,
               double value_,
               bool SI_main_)
    {
      _set_default();
      set_name(name_);
      add_symbol(name_, true);
      set_dimension_infos(dimension_infos_);
      set_value(value_);
      set_SI_main(SI_main_);
      return;
    }

    unit::unit(const std::string & name_,
               const std::string & symbol_,
               const std::string & dimension_infos_,
               double value_,
               bool SI_main_)
    {
      _set_default();
      set_name(name_);
      add_symbol(symbol_, true);
      set_dimension_infos(dimension_infos_);
      set_value(value_);
      set_SI_main(SI_main_);
      return;
    }

    unit::unit(const std::string & name_,
               const std::string & main_symbol_,
               const std::string & alt_symbol_,
               const std::string & dimension_infos_,
               double value_,
               bool SI_main_)
    {
      _set_default();
      set_name(name_);
      add_symbol(main_symbol_, true);
      add_symbol(alt_symbol_, false);
      set_dimension_infos(dimension_infos_);
      set_value(value_);
      set_SI_main(SI_main_);
      return;
    }

    unit::~unit()
    {
      return;
    }

    bool unit::is_valid() const
    {
      return has_name() && has_main_symbol()
        && has_dimension_label() && (_value_ == _value_) && (_value_ > 0.0);
    }

    void unit::reset()
    {
      _name_.clear();
      _symbols_.clear();
      _main_symbol_.clear();
      _dimension_label_.clear();
      _dimension_meta_.clear();
      _SI_main_ = false;
      datatools::invalidate(_value_);
      return;
    }

    bool unit::has_name() const
    {
      return !_name_.empty();
    }

    const std::string & unit::get_name() const
    {
      return _name_;
    }

    unit & unit::set_name(const std::string & s_)
    {
      _name_ = s_;
      return *this;
    }

    bool unit::has_symbol(const std::string & symbol_) const
    {
      return _symbols_.count(symbol_);
    }

    bool unit::match(const std::string & id_) const
    {
      return _name_ == id_ || has_symbol(id_);
    }

    unit & unit::add_symbol(const std::string & symbol_, bool main_)
    {
      std::vector<std::string> tokens;
      boost::split(tokens, symbol_, boost::is_any_of(";:"));
      DT_THROW_IF(tokens.size() == 0, std::logic_error,
                  "Missing symbol(s) for unit '" << _name_ << "'!");
      for (int i = 0; i < (int) tokens.size(); i++) {
        std::string symbol = tokens[i];
        boost::trim(symbol);
        DT_THROW_IF(symbol.empty(),
                    std::logic_error,
                    "Empty symbol in unit '" << _name_ << "'!");
        DT_THROW_IF(_symbols_.count(symbol),
                    std::logic_error,
                    "Symbol '" << symbol << "' already registered in unit '" << _name_ << "'!");
        _symbols_.insert(symbol);
        if (i == 0) {
          if (!has_main_symbol() || main_) {
            set_main_symbol(symbol);
          }
        }
      }
      return *this;
    }

    const std::set<std::string> & unit::get_symbols() const
    {
      return _symbols_;
    }

    bool unit::has_main_symbol() const
    {
      return !_main_symbol_.empty();
    }

    const std::string & unit::get_main_symbol() const
    {
      DT_THROW_IF(!has_main_symbol(), std::logic_error,
                  "Unit '" << _name_ << "' has no main symbol!");
      return _main_symbol_;
    }

    unit & unit::set_main_symbol(const std::string & s_)
    {
      if (!has_symbol(s_)) {
        add_symbol(s_);
      }
      _main_symbol_ = s_;
      return *this;
    }

    bool unit::has_dimension_label() const
    {
      return !_dimension_label_.empty();
    }

    const std::string & unit::get_dimension_label() const
    {
      return _dimension_label_;
    }

    bool unit::is_dimension(const std::string & ucl_) const
    {
      return _dimension_label_ == ucl_;
    }

    unit & unit::set_dimension_label(const std::string & ucl_)
    {
      DT_THROW_IF(!unit_dimension::validate_dimension_label(ucl_), std::logic_error,
                  "Dimension label '" << ucl_ << "' is not valid!");
      _dimension_label_ = ucl_;
      return *this;
    }

    bool unit::has_dimension_meta() const
    {
      return !_dimension_meta_.empty();
    }

    const std::string & unit::get_dimension_meta() const
    {
      return _dimension_meta_;
    }

    unit & unit::set_dimension_meta(const std::string & meta_)
    {
      _dimension_meta_ = meta_;
      return *this;
    }

    unit & unit::set_dimension_infos(const std::string & dimension_infos_)
    {
      std::vector<std::string> tokens;
      boost::split(tokens, dimension_infos_, boost::is_any_of(";:"));
      if (tokens.size() > 0) {
        std::string dim_label = tokens[0];
        boost::trim(dim_label);
        DT_THROW_IF(dim_label.empty(),
                    std::logic_error,
                    "Empty dimension label in unit '" << _name_ << "'!");
        set_dimension_label(dim_label);
      }
      if (tokens.size() > 1) {
        std::string dim_meta = tokens[1];
        boost::trim(dim_meta);
        DT_THROW_IF(dim_meta.empty(),
                    std::logic_error,
                    "Empty dimension meta in unit '" << _name_ << "'!");
        set_dimension_meta(dim_meta);
      }
      return *this;
    }

    bool unit::is_SI_main() const
    {
      return _SI_main_;
    }

    unit &  unit::set_SI_main(bool flag_)
    {
      _SI_main_ = flag_;
      return *this;
    }

    double unit::get_value() const
    {
      return _value_;
    }

    unit & unit::set_value(double uv_)
    {
      DT_THROW_IF(uv_ != uv_ || uv_ <= 0.0, std::logic_error,
                  "Invalid value for unit '" << _name_ << "'!");
      _value_ = uv_;
      return *this;
    }

    unit::operator double() const
    {
      DT_THROW_IF(!datatools::is_valid(_value_), std::logic_error,
                  "Unit value is not valid!");
      return _value_;
    }

    const std::string & unit::str() const
    {
      return this->get_main_symbol();
    }

    // std::ostream & unit::operator()(std::ostream & out_, double x_) const
    // {
    //   out_ << x_  / _value_ << ' ' << get_main_symbol();
    //   return out_;
    // }

    void unit::tree_dump(std::ostream & out_,
                         const std::string & title_,
                         const std::string & indent_,
                         bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Name : '" << _name_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Valid : " << is_valid()
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "List of symbols : ";
      for (std::set<std::string>::const_iterator i = _symbols_.begin();
           i != _symbols_.end();
           i++) {
        out_ << " '" << *i << "'";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Main symbol : '" << _main_symbol_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Dimension label : '" << _dimension_label_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Dimension meta : '" << _dimension_meta_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "SI main unit : '" << _SI_main_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Value : " << _value_
           << std::endl;

      return;
    }

    /* ------------------------ */

    // static
    std::string unit_dimension::base_dimension_to_symbol(base_dimension_type index_)
    {
      switch (index_) {
      case BD_MASS               : return "M";
      case BD_LENGTH             : return "L";
      case BD_TIME               : return "T";
      case BD_ELECTRIC_CURRENT   : return "I";
      case BD_TEMPERATURE        : return "theta";
      case BD_AMOUNT             : return "N";
      case BD_LUMINOUS_INTENSITY : return "J";
      default                    : break;
      }
      return "";
    }

    // static
    std::string unit_dimension::base_dimension_to_label(base_dimension_type index_)
    {
      switch (index_) {
      case BD_MASS               : return "mass";
      case BD_LENGTH             : return "length";
      case BD_TIME               : return "time";
      case BD_ELECTRIC_CURRENT   : return "electric_current";
      case BD_TEMPERATURE        : return "temperature";
      case BD_AMOUNT             : return "amount";
      case BD_LUMINOUS_INTENSITY : return "luminous_intensity";
      default                    : break;
      }
      return "";
    }

    // static
    unit_dimension::base_dimension_type
    unit_dimension::symbol_to_base_dimension(const std::string & symbol_)
    {
      if (symbol_ == base_dimension_to_symbol(BD_MASS)) {
        return BD_MASS;
      }
      if (symbol_ == base_dimension_to_symbol(BD_LENGTH)) {
        return BD_LENGTH;
      }
      if (symbol_ == base_dimension_to_symbol(BD_TIME)) {
        return BD_TIME;
      }
      if (symbol_ == base_dimension_to_symbol(BD_ELECTRIC_CURRENT)) {
        return BD_ELECTRIC_CURRENT;
      }
      if (symbol_ == base_dimension_to_symbol(BD_TEMPERATURE)) {
        return BD_TEMPERATURE;
      }
      if (symbol_ == base_dimension_to_symbol(BD_AMOUNT)) {
        return BD_AMOUNT;
      }
      if (symbol_ == base_dimension_to_symbol(BD_LUMINOUS_INTENSITY)) {
        return BD_LUMINOUS_INTENSITY;
      }
      return BD_INVALID;
    }

    // static
    unit_dimension::base_dimension_type
    unit_dimension::label_to_base_dimension(const std::string & label_)
    {
      if (label_ == base_dimension_to_label(BD_MASS)) {
        return BD_MASS;
      }
      if (label_ == base_dimension_to_label(BD_LENGTH)) {
        return BD_LENGTH;
      }
      if (label_ == base_dimension_to_label(BD_TIME)) {
        return BD_TIME;
      }
      if (label_ == base_dimension_to_label(BD_ELECTRIC_CURRENT)) {
        return BD_ELECTRIC_CURRENT;
      }
      if (label_ == base_dimension_to_label(BD_TEMPERATURE)) {
        return BD_TEMPERATURE;
      }
      if (label_ == base_dimension_to_label(BD_AMOUNT)) {
        return BD_AMOUNT;
      }
      if (label_ == base_dimension_to_label(BD_LUMINOUS_INTENSITY)) {
        return BD_LUMINOUS_INTENSITY;
      }
      return BD_INVALID;
    }

    // static
    bool unit_dimension::validate_dimension_label(const std::string & name_)
    {
      // std::cerr << "DEVEL: unit_dimension::validate_dimension_label: name_='" << name_ << "'\n";
      static const std::string allowed = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
      size_t cnt = std::count_if(name_.begin(), name_.end(), boost::is_any_of(allowed));
      if (cnt == 0 || cnt != name_.size()) {
        return false;
      }
      return true;
    }

    void unit_dimension::_set_default()
    {
      reset_dimensional_powers();
      return;
    }

    unit_dimension::unit_dimension(const std::string & label_meta_,
                                   const std::string & default_unit_name_)
    {
      unit_dimension::_set_default();
      std::vector<std::string> tokens;
      boost::split(tokens, label_meta_, boost::is_any_of(";:"));
      if (tokens.size() > 0) {
        std::string dim_label = tokens[0];
        boost::trim(dim_label);
        DT_THROW_IF(dim_label.empty(),
                    std::logic_error,
                    "Empty dimension label!");
        set_label(dim_label);
      }
      if (tokens.size() > 1) {
        std::string dim_meta = tokens[1];
        boost::trim(dim_meta);
        DT_THROW_IF(dim_meta.empty(),
                    std::logic_error,
                    "Empty meta in unit dimension '" << _label_ << "'!");
        if (!decode_dimensional_powers(dim_meta)) {
          reset_dimensional_powers();
        }
      }
      set_default_unit_name(default_unit_name_);
      return;
    }

    unit_dimension::unit_dimension()
    {
      unit_dimension::_set_default();
      return;
    }

    unit_dimension::~unit_dimension()
    {
      return;
    }

    bool unit_dimension::is_valid() const
    {
      return has_label() && has_default_unit();
    }

    void unit_dimension::reset()
    {
      _default_unit_name_.clear();
      _unit_names_.clear();
      _label_.clear();
      unit_dimension::_set_default();
      return;
    }

    bool unit_dimension::has_label() const
    {
      return !_label_.empty();
    }

    unit_dimension & unit_dimension::set_label(const std::string & label_)
    {
      DT_THROW_IF(!unit_dimension::validate_dimension_label(label_), std::logic_error,
                  "Dimension label '" << label_ << "' is not valid!");
      _label_ = label_;
      return *this;
    }

    const std::string & unit_dimension::get_label() const
    {
      return _label_;
    }

    bool unit_dimension::has_unit(const std::string & unit_name_) const
    {
      return _unit_names_.count(unit_name_) != 0;
    }

    unit_dimension & unit_dimension::add_unit(const std::string & unit_name_, bool default_unit_)
    {
      _unit_names_.insert(unit_name_);
      if (!has_default_unit() || default_unit_) {
        set_default_unit_name(unit_name_);
      }
      return *this;
    }

    void unit_dimension::remove_unit(const std::string & unit_name_)
    {
      DT_THROW_IF (!has_unit(unit_name_), std::logic_error,
                   "Unit dimension '" << _label_ << "' contains no unit named '" << unit_name_ << "'!");
      _unit_names_.erase(unit_name_);
      if (has_default_unit()) {
        if (unit_name_ == _default_unit_name_) {
          _default_unit_name_.clear();
          if (_unit_names_.size()) {
            // Use the first unit in the set has the default one
            _default_unit_name_ = *_unit_names_.begin();
          }
        }
      }
      return;
    }

    const std::set<std::string> & unit_dimension::get_unit_names() const
    {
      return _unit_names_;
    }

    bool unit_dimension::has_default_unit() const
    {
      return !_default_unit_name_.empty();
    }

    unit_dimension & unit_dimension::set_default_unit_name(const std::string & unit_name_)
    {
      if (!has_unit(unit_name_)) {
        add_unit(unit_name_, true);
      } else {
        _default_unit_name_ = unit_name_;
      }
      return *this;
    }

    const std::string & unit_dimension::get_default_unit_name() const
    {
      return _default_unit_name_;
    }

    bool unit_dimension::has_dimensional_powers() const
    {
      return _dimensional_powers_[BD_INVALID] == 0;
    }

    void unit_dimension::reset_dimensional_powers()
    {
      _dimensional_powers_[BD_INVALID] = 1;
      for (int i = BD_MASS; i <= BD_LUMINOUS_INTENSITY; i++) {
        _dimensional_powers_[i] = 0;
      }
      return;
    }

    bool unit_dimension::is_dimensionless() const
    {
      DT_THROW_IF(!has_dimensional_powers(), std::logic_error,
                  "Unit dimension '" << _label_ << "' has no known dimension power!");
      for (int i = BD_MASS; i < BD_LAST; i++) {
        if (_dimensional_powers_[i] != 0) {
          return false;
        }
      }
      return true;
    }

    void unit_dimension::encode_dimensional_powers(std::string & word_) const
    {
      std::ostringstream out;
      if (! has_dimensional_powers()) {
        word_.clear();
        return;
      }
      if (is_dimensionless()) {
        out << "[1]";
      } else {
        for (int i = BD_MASS; i < BD_LAST; i++) {
          int pow = _dimensional_powers_[i];
          if (pow != 0) {
            std::string dim_sym = base_dimension_to_symbol((base_dimension_type)i);
            out << '[' << dim_sym ;
            if (pow != 1) {
              out << pow;
            }
            out << ']';
          }
        }
      }
      word_ = out.str();
      return;
    }

    bool unit_dimension::decode_dimensional_powers(const std::string & word_)
    {
      bool devel = false;
      if (devel) std::cerr << "DEVEL: compute_dimensional_powers: " << "Entering..." << std::endl;
      if (devel) std::cerr << "DEVEL: compute_dimensional_powers: " << "word_='" << word_ << "'" << std::endl;
      reset_dimensional_powers();
      // [1] == dimensionless
      // [M][L3][T-1][A3]
      std::string word = word_;
      boost::replace_all(word, "\t", " ");
      boost::replace_all(word, " ", "");
      boost::replace_all(word, "][", ":");
      boost::replace_all(word, "[", "");
      boost::replace_all(word, "]", "");
      if (devel) std::cerr << "DEVEL: compute_dimensional_powers: " << "word='" << word << "'" << std::endl;
      bool decode = false;
      if (word == "1") {
        decode = true;
      } else {
        std::vector<std::string> tokens;
        boost::split(tokens, word, boost::is_any_of(":"));
        for (int i = 0; i < (int) tokens.size(); i++) {
          // M:L3:T-1:A3
          if (tokens[i].empty()) {
            break;
          }
          const std::string & tok = tokens[i];
          if (devel) std::cerr << "DEVEL: compute_dimensional_powers: "
                    << "tok='" << tok << "'" << std::endl;
          decode = true;
          for (int j = BD_MASS; j < BD_LAST; j++) {
            std::string dim_sym = base_dimension_to_symbol((base_dimension_type)j);
            if (devel) std::cerr << "DEVEL: compute_dimensional_powers: " << "dim_sym='"
                      << dim_sym << "'" << std::endl;
            std::string ttt = tok.substr(0, dim_sym.size());
            if (devel) std::cerr << "DEVEL: compute_dimensional_powers: ttt=" << ttt << std::endl;
            if (ttt == dim_sym) {
              std::string dim_pow_str = tok.substr(dim_sym.size());
              if (devel) std::cerr << "DEVEL: compute_dimensional_powers: dim_pow_str='"
                        << dim_pow_str << "'" << std::endl;
              int dim_pow = 0;
              if (dim_pow_str.size()) {
                std::istringstream dim_pow_in(dim_pow_str);
                dim_pow_in >> dim_pow;
                if (!dim_pow_in) {
                  decode = false;
                  break;
                } // !dim_pow_in
              } // dim_pow_str.size()
              else {
                dim_pow = 1;
              }
              _dimensional_powers_[(int)j] += dim_pow;
              if (devel) std::cerr << "DEVEL: compute_dimensional_powers: "
                        << "dimension [" << dim_sym << "] = "
                        << _dimensional_powers_[(int)j] << std::endl;
              break;
            } // if
          } // for
        } // for
      }
      if (decode) {
        _dimensional_powers_[BD_INVALID] = 0;
      } else {
        reset_dimensional_powers();
        std::cerr << "DEVEL: compute_dimensional_powers: "
                  << "Failure!" << std::endl;
      }
      return _dimensional_powers_[BD_INVALID] == 0;
    }

    bool unit_dimension::match_dimensional_powers(const unit_dimension & ud_) const
    {
      if (_dimensional_powers_[BD_INVALID] || ud_._dimensional_powers_[BD_INVALID]) {
        return false;
      }
      for (int i = BD_MASS; i < BD_LAST; i++) {
        if (_dimensional_powers_[i] != ud_._dimensional_powers_[i]) {
          return false;
        }
      }
      return true;
    }


    void unit_dimension::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Label : '" << _label_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Valid : " << is_valid()
           << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Units : " << std::endl;
      for (std::set<std::string>::const_iterator i = _unit_names_.begin();
           i != _unit_names_.end();
           i++) {
        out_ << indent_ << i_tree_dumpable::skip_tag;
        std::set<std::string>::const_iterator j = i;
        if (++j == _unit_names_.end()) {
          out_ << indent_ << i_tree_dumpable::last_tag;
        } else {
          out_ << indent_ << i_tree_dumpable::tag;
        }
        out_ << "'" << *i << "'";
        if (*i == _default_unit_name_) {
          out_ << " (default)";
        }
        out_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Default unit name: '" << _default_unit_name_ << "'"
           << std::endl;

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Dimensional info: ";
      if (has_dimensional_powers()) {
        std::string encoded;
        encode_dimensional_powers(encoded);
        out_ << encoded;
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      return;
    }

    /* ------------------------ */

    registry::registry(uint32_t flags_)
    {
      bool do_register_standard_unit = true;
      if (flags_ & NO_STANDARD_UNITS) {
        do_register_standard_unit = false;
      }

      if (do_register_standard_unit) {
        register_standard_units();
      }
      return;
    }

    registry::~registry()
    {
      return;
    }

    void registry::registration(const unit & unit_, bool default_in_dimension_ )
    {
      DT_THROW_IF(!unit_.is_valid(), std::logic_error,
                  "Unit with name '" << unit_.get_name() << "' is not valid!");
      DT_THROW_IF(has_unit(unit_.get_name()), std::logic_error,
                  "Unit with name '" << unit_.get_name() << "' is already registered!");
      const std::string & unit_name = unit_.get_name();
      {
        _units_[unit_name] = unit_;
      }
      if (!has_dimension(unit_.get_dimension_label())) {
        unit_dimension dummy;
        dummy.set_label(unit_.get_dimension_label());
        _dimensions_[unit_.get_dimension_label()] = dummy;
      }
      unit & the_unit = _grab_unit(unit_name);
      unit_dimension & the_dimension = _grab_dimension(the_unit.get_dimension_label());
      bool default_in_dimension = default_in_dimension_;
      the_dimension.add_unit(unit_name, default_in_dimension);
      if (the_unit.has_dimension_meta()) {
        DT_THROW_IF(!the_dimension.decode_dimensional_powers(the_unit.get_dimension_meta()),
                    std::logic_error,
                    "Invalid unit dimension meta data '" << the_unit.get_dimension_meta() << "'!");
      }

      for (std::set<std::string>::const_iterator i = the_unit.get_symbols().begin();
           i != the_unit.get_symbols().end();
           i++) {
        const std::string & the_symbol = *i;
        DT_THROW_IF(_symbols_.find(the_symbol) != _symbols_.end(), std::logic_error,
                    "Symbol '" << the_symbol << "' is already used in the unit registry!");
        _symbols_[the_symbol] = &the_unit;
      }
      return;
    }

    void registry::unregistration(const std::string & unit_name_)
    {
      DT_THROW_IF(!has_unit(unit_name_), std::logic_error,
                  "Unit registry has no registered unit with name '" << unit_name_ << "'!");
      unit & the_unit = _grab_unit(unit_name_);
      for (std::set<std::string>::const_iterator i = the_unit.get_symbols().begin();
           i != the_unit.get_symbols().end();
           i++) {
        const std::string & the_symbol = *i;
        _symbols_.erase(the_symbol);
      }
      std::string the_dimension_label = the_unit.get_dimension_label();
      unit_dimension & the_dimension = _grab_dimension(the_dimension_label);
      if (the_dimension.has_unit(unit_name_)) {
        the_dimension.remove_unit(unit_name_);
        if (the_dimension.get_unit_names().size() == 0) {
          _dimensions_.erase(the_dimension_label);
        }
      }
      _units_.erase(unit_name_);
      return;
    }

    bool registry::has_unit_from_any(const std::string & unit_label_) const
    {
      return has_unit(unit_label_) || has_symbol(unit_label_);
    }

    bool registry::has_unit(const std::string & unit_name_) const
    {
      return _units_.find(unit_name_) != _units_.end();
    }

    bool registry::has_symbol(const std::string & unit_symbol_) const
    {
      return _symbols_.find(unit_symbol_) != _symbols_.end();
    }

    bool registry::has_dimension(const std::string & dimension_label_) const
    {
      return _dimensions_.find(dimension_label_) != _dimensions_.end();
    }

    const unit & registry::get_unit(const std::string & unit_name_) const
    {
      DT_THROW_IF(!has_unit(unit_name_), std::logic_error, "No unit with name '"
                  << unit_name_ << "' is registered!");
      return _units_.find(unit_name_)->second;
    }

    unit & registry::_grab_unit(const std::string & unit_name_)
    {
      DT_THROW_IF(!has_unit(unit_name_), std::logic_error, "No unit with name '"
                  << unit_name_ << "' is registered!");
      return _units_.find(unit_name_)->second;
    }

    const unit & registry::get_unit_from_symbol(const std::string & unit_symbol_) const
    {
      DT_THROW_IF(!has_symbol(unit_symbol_), std::logic_error, "No unit with symbol '"
                  << unit_symbol_ << "' is registered!");
      return *_symbols_.find(unit_symbol_)->second;
    }

    const unit & registry::get_unit_from_any(const std::string & unit_id_) const
    {
      if (has_symbol(unit_id_)) {
        return get_unit_from_symbol(unit_id_);
      }
      return get_unit(unit_id_);
    }

    unit_dimension & registry::_grab_dimension(const std::string & dimension_label_)
    {
      DT_THROW_IF(!has_dimension(dimension_label_), std::logic_error, "No unit dimension with label '"
                  << dimension_label_ << "' is registered!");
      return _dimensions_.find(dimension_label_)->second;
    }

    const unit_dimension & registry::get_dimension(const std::string & dimension_label_) const
    {
      DT_THROW_IF(!has_dimension(dimension_label_), std::logic_error, "No unit dimension with label '"
                  << dimension_label_ << "' is registered!");
      return _dimensions_.find(dimension_label_)->second;
    }

    void registry::clear()
    {
      _units_.clear();
      _dimensions_.clear();
      return;
    }

    const registry::unit_dict_type & registry::get_units() const
    {
      return _units_;
    }

    const registry::symbol_dict_type & registry::get_symbols() const
    {
      return _symbols_;
    }

    const registry::dimension_dict_type & registry::get_dimensions() const
    {
      return _dimensions_;
    }

    // static
    registry & registry::system_registry()
    {
      return _access_system_registry_();
    }

    // static
    const registry & registry::const_system_registry()
    {
      return const_cast<registry &>(system_registry());
    }

    // static
    registry & registry::_access_system_registry_()
    {
      static boost::scoped_ptr<registry> _reg_singleton;
      if (! _reg_singleton) {
        _reg_singleton.reset(new registry);

      }
      return *_reg_singleton;
    }

    void registry::tree_dump(std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
    {
      if (!title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Units: [" << _units_.size() << ']' << std::endl;
      for (unit_dict_type::const_iterator i = _units_.begin();
           i != _units_.end();
           i++) {
        unit_dict_type::const_iterator j = i;
        out_ << indent_ << i_tree_dumpable::skip_tag;
        if (++j == _units_.end()) {
          out_ << indent_ << i_tree_dumpable::last_tag;
        } else {
          out_ << indent_ << i_tree_dumpable::tag;
        }
        out_ << "Unit '" << i->first << "' in dimension '" << i->second.get_dimension_label() << "'";
        out_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Unit dimensions: [" << _dimensions_.size() << ']' << std::endl;
      for (dimension_dict_type::const_iterator i = _dimensions_.begin();
           i != _dimensions_.end();
           i++) {
        dimension_dict_type::const_iterator j = i;
        out_ << indent_ << i_tree_dumpable::skip_tag;
        if (++j == _dimensions_.end()) {
          out_ << indent_ << i_tree_dumpable::last_tag;
        } else {
          out_ << indent_ << i_tree_dumpable::tag;
        }
        std::string encoded;
        i->second.encode_dimensional_powers(encoded);
        out_ << "Dimension '" << i->first << "' " << encoded
             << " with default unit '" << i->second.get_default_unit_name() << "'";
        out_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Unit symbols: [" << _symbols_.size() << ']' << std::endl;
      for (symbol_dict_type::const_iterator i = _symbols_.begin();
           i != _symbols_.end();
           i++) {
        symbol_dict_type::const_iterator j = i;
        out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
        if (++j == _symbols_.end()) {
          out_ << indent_ << i_tree_dumpable::last_tag;
        } else {
          out_ << indent_ << i_tree_dumpable::tag;
        }
        out_ << "Symbol '" << i->first << "' for unit '" << i->second->get_name() << "'";
        out_ << std::endl;
      }

      return;
    }

    void registry::register_standard_units()
    {
      // Length [L]:
      registration(unit("meter",       "m",  "length;[L]",         CLHEP::meter, true));
      registration(unit("angstrom",   "AA",  "length",             CLHEP::angstrom));
      registration(unit("femtometer", "fm",
                        /**/       "fermi",  "length",             CLHEP::fermi));
      registration(unit("picometer",  "pm",  "length",      1e-3 * CLHEP::nanometer));
      registration(unit("nanometer",  "nm",  "length",             CLHEP::nanometer));
      registration(unit("micrometer", "um",  "length",             CLHEP::micrometer));
      registration(unit("millimeter", "mm",  "length",             CLHEP::millimeter));
      registration(unit("centimeter", "cm",  "length",             CLHEP::centimeter));
      registration(unit("decimeter",  "dm",  "length",       0.1 * CLHEP::meter));
      registration(unit("decameter", "dam",  "length",       10. * CLHEP::meter));
      registration(unit("hectometer", "hm",  "length",      100. * CLHEP::meter));
      registration(unit("kilometer",  "km",  "length",             CLHEP::kilometer));
      registration(unit("astronomical_unit", "au"
                        /**/          "AU",  "length", 1.4960e11 * CLHEP::meter));
      const double light_year = 9.4607e15 * CLHEP::meter;
      registration(unit("light_year",      "ly",  "length", light_year));
      registration(unit("kilolight_year", "kly",  "length", 1.e3 * light_year));
      registration(unit("megalight_year", "Mly",  "length", 1.e6 * light_year));
      registration(unit("gigalight_year", "Gly",  "length", 1.e9 * light_year));
      registration(unit("parsec",      "pc",  "length",            CLHEP::parsec));
      registration(unit("kiloparsec", "kpc",  "length",     1.e3 * CLHEP::parsec));
      registration(unit("megaparsec", "Mpc",  "length",     1.e6 * CLHEP::parsec));
      registration(unit("gigaparsec", "Gpc",  "length",     1.e9 * CLHEP::parsec));
      registration(unit("inch",       "in",  "length",      2.54 * CLHEP::centimeter));
      registration(unit("foot",       "ft",
                        /**/        "feet",  "length",    0.3048 * CLHEP::meter));
      registration(unit("yard",       "yd",
                        /**/          "vg", "length",    0.9144 * CLHEP::meter));
      const double mile = 1609.344 * CLHEP::meter;
      registration(unit("mile",       "mi", "length",  mile));
      const double nautical_mile = 1852.00 * CLHEP::meter;
      registration(unit("nautical_mile", "M",
                        /**/            "NM", "length",  nautical_mile));

      // Mass [M]:
      registration(unit("kilogram",  "kg",     "mass;[M]",    CLHEP::kilogram, true));
      registration(unit("decagram", "dag",     "mass", 0.01 * CLHEP::kilogram));
      registration(unit("hectogram", "hg",     "mass",  0.1 * CLHEP::kilogram));
      registration(unit("ton",        "t",     "mass", 1e+3 * CLHEP::kilogram));
      registration(unit("gram",       "g",     "mass",        CLHEP::gram));
      registration(unit("decigram",  "dg",     "mass",  0.1 * CLHEP::gram));
      registration(unit("centigram", "cg",     "mass", 0.01 * CLHEP::gram));
      registration(unit("milligram", "mg",     "mass",        CLHEP::milligram));
      registration(unit("microgram", "ug",     "mass", 1e-3 * CLHEP::milligram));
      registration(unit("eV/c2",               "mass",        CLHEP::eV / CLHEP::c_squared));
      registration(unit("MeV/c2",              "mass",        CLHEP::MeV / CLHEP::c_squared));
      registration(unit("GeV/c2",              "mass",        CLHEP::GeV / CLHEP::c_squared));
      registration(unit("dalton", "u", "Da",   "mass",        1.660538921e-27 * CLHEP::kilogram));

      // Time [T]:
      registration(unit("second",       "s",  "time;[T]",     CLHEP::second, true));
      registration(unit("millisecond", "ms",  "time",         CLHEP::millisecond));
      registration(unit("microsecond", "us",  "time",         CLHEP::microsecond));
      registration(unit("nanosecond",  "ns",  "time",         CLHEP::nanosecond));
      registration(unit("picosecond",  "ps",  "time",         CLHEP::picosecond));
      registration(unit("femtosecond", "fs",  "time", 1.e-3 * CLHEP::picosecond));
      const double minute =   60 * CLHEP::second;
      registration(unit("minute",      "min", "time",  minute));
      const double hour   = 3600 * CLHEP::second;
      registration(unit("hour",        "h",   "time",  hour));
      const double day   = 24 * hour;
      registration(unit("day",         "d",   "time",  day));
      const double week  = 7 * day;
      registration(unit("week",               "time",  week));

      // https://en.wikipedia.org/wiki/SI_electromagnetism_units

      // Electric current [I] (A = W/V = C/s) :
      registration(unit("ampere",       "A",    "electric_current;[I]",      CLHEP::ampere, true));
      registration(unit("milliampere", "mA",    "electric_current", 1.e-3  * CLHEP::ampere));
      registration(unit("microampere", "uA",    "electric_current", 1.e-6  * CLHEP::ampere));
      registration(unit("nanoampere",  "nA",    "electric_current", 1.e-9  * CLHEP::ampere));
      registration(unit("picoampere",  "pA",    "electric_current", 1.e-12 * CLHEP::ampere));
      registration(unit("femtoampere", "fA",    "electric_current", 1.e-15 * CLHEP::ampere));
      registration(unit("kiloampere",  "kA",    "electric_current", 1.e3   * CLHEP::ampere));
      registration(unit("megaampere",  "MA",    "electric_current", 1.e6   * CLHEP::ampere));

      // Temperature [theta]:
      registration(unit("kelvin",        "K",   "temperature;[theta]", CLHEP::kelvin, true));
      registration(unit("millikelvin",  "mK",   "temperature", 1.e-3 * CLHEP::kelvin));

      // Amount [N]
      registration(unit("mole",        "mol",   "amount;[N]",          CLHEP::mole, true));

      // https://en.wikipedia.org/wiki/Candela#SI_photometric_light_units

      // Luminous intensity [J]:
      registration(unit("candela",     "cd",   "luminous_intensity;[J]", CLHEP::candela, true));

      // Luminous flux/luminous power [J]:
      registration(unit("lumen",       "lm",   "luminous_flux", CLHEP::lumen, true));

      // Luminous energy [T⋅J]:
      registration(unit("lm.s",    "luminous_energy", CLHEP::lumen * CLHEP::second, true));

      // Luminance [L−2⋅J]:
      registration(unit("cd/m2", "nits",   "luminance;[J][L-2]", CLHEP::candela / CLHEP::meter2, true));

      // Illuminance/luminous emittance [L−2⋅J]:
      registration(unit("lux", "lx", "illuminance", CLHEP::lumen / CLHEP::meter2, true));

      // Luminous exposure [L−2⋅T⋅J]:
      registration(unit("lx.s", "luminous_exposure", CLHEP::lumen * CLHEP::second, true));

      // Luminous energy density [L−3⋅T⋅J]:
      registration(unit("lx.s/m3", "luminous_energy_density",
                        CLHEP::lumen * CLHEP::second / CLHEP::meter3, true));

      // Surface/area:
      registration(unit("meter2",       "m2", "surface;[L2]",      CLHEP::meter2, true));
      registration(unit("centimeter2", "cm2", "surface",           CLHEP::centimeter2));
      registration(unit("millimeter2", "mm2", "surface",           CLHEP::millimeter2));
      registration(unit("micrometer2", "um2", "surface",   1.e-6 * CLHEP::millimeter2));
      registration(unit("femtometer2", "fm2",
                        /**/        "fermi2", "surface",  1.e-30 * CLHEP::meter2));
      registration(unit("kilometer2",  "km2", "surface",           CLHEP::kilometer2));
      registration(unit("inch2",       "in2", "surface",  6.4516 * CLHEP::centimeter2));
      registration(unit("foot2",       "ft2",
                        /**/         "feet2", "surface",  0.09290304 * CLHEP::meter2));
      registration(unit("yard2",       "yd2",
                        /**/           "vg2", "surface",  0.83612736 * CLHEP::meter2));


      // Surface density:
      registration(unit("kg/m2",         "surface_density;[M][L-2]", CLHEP::kg / CLHEP::meter2, true));
      registration(unit("g/cm2",         "surface_density",          CLHEP::gram / CLHEP::centimeter2));

      // Cross-section:
      registration(unit("barn",          "b", "cross_section;[L2]",      CLHEP::barn, true));
      registration(unit("megabarn",     "Mb", "cross_section",  1.e+6  * CLHEP::barn));
      registration(unit("kilobarn",     "kb", "cross_section",  1.e+3  * CLHEP::barn));
      registration(unit("millibarn",    "mb", "cross_section",  1.e-3  * CLHEP::barn));
      registration(unit("microbarn",    "ub", "cross_section",  1.e-6  * CLHEP::barn));
      registration(unit("nanobarn",     "nb", "cross_section",  1.e-9  * CLHEP::barn));
      registration(unit("picobarn",     "pb", "cross_section",  1.e-12 * CLHEP::barn));
      registration(unit("femtobarn",    "fb", "cross_section",  1.e-15 * CLHEP::barn));

      // Volume:
      registration(unit("meter3",       "m3",     "volume;[L3]",             CLHEP::meter3, true));
      registration(unit("centimeter3", "cm3",     "volume",                  CLHEP::centimeter3));
      registration(unit("decimeter3",  "dm3",     "volume",           1.e3 * CLHEP::centimeter3));
      registration(unit("millimeter3", "mm3",     "volume",                  CLHEP::millimeter3));
      registration(unit("kilometer3",  "km3",     "volume",                  CLHEP::kilometer3));
      const double litre = 1.e3 * CLHEP::centimeter3;
      const double millilitre = 1.e-3 * litre;
      registration(unit("litre",        "l",
                        /**/            "L",      "volume",                litre));
      registration(unit("decalitre",  "dal",
                        /**/          "daL",      "volume",          10. * litre));
      registration(unit("hectolitre",  "hl",
                        /**/           "hL",      "volume",         100. * litre));
      registration(unit("kilolitre",   "kl",
                        /**/           "kL",      "volume",         1.e3 * litre));
      registration(unit("megalitre",   "Ml",
                        /**/           "ML",      "volume",         1.e6 * litre));
      registration(unit("gigalitre",   "Gl",
                        /**/           "GL",      "volume",         1.e9 * litre));
      registration(unit("teralitre",   "Tl",
                        /**/           "TL",      "volume",        1.e12 * litre));
      registration(unit("centilitre",  "cl",
                        /**/           "cL",      "volume",        1.e-2 * litre));
      registration(unit("millilitre",  "ml",
                        /**/           "mL",      "volume",           millilitre));
      registration(unit("microlitre",  "ul",
                        /**/           "uL",      "volume",        1.e-6 * litre));
      registration(unit("imperial_gallon",
                        /**/      "imp_gal",      "volume", 4.54609      * litre));
      registration(unit("us_gallon",  "gal",
                        /**/       "gal_us",      "volume", 3.785411784  * litre));
      registration(unit("inch3",      "in3",      "volume", 0.016387064  * litre));
      registration(unit("foot3",      "ft3",
                        /**/        "feet3",      "volume",  8.316846592 * litre));


      // Angle/plane angle:
      registration(unit("radian",         "rad", "angle;[1]",        CLHEP::radian, true));
      registration(unit("milliradian",   "mrad", "angle",            CLHEP::milliradian));
      registration(unit("microradian",   "urad", "angle",     1e-3 * CLHEP::milliradian));
      registration(unit("degree",         "deg", "angle",            CLHEP::degree));
      registration(unit("arcminute",   "arcmin",
                        /**/               "am", "angle",            CLHEP::degree / 60));
      registration(unit("arcsecond",   "arcsec",
                        /**/               "as", "angle",            CLHEP::degree / 3600));
      registration(unit("milliarcsecond", "mas", "angle",    1.e-3 * CLHEP::degree / 3600));
      registration(unit("microarcsecond", "uas", "angle",    1.e-6 * CLHEP::degree / 3600));
      registration(unit("grade",          "gon", "angle",     M_PI * CLHEP::radian / 200));
      registration(unit("turn",          "turn", "angle", 2 * M_PI * CLHEP::radian));

      // Solid angle:
      registration(unit("steradian",       "sr",  "solid_angle;[1]",     CLHEP::steradian, true));
      registration(unit("decasteradian", "dasr",  "solid_angle",   10. * CLHEP::steradian));
      registration(unit("decisteradian",  "dsr",  "solid_angle",   0.1 * CLHEP::steradian));
      registration(unit("centisteradian", "csr",  "solid_angle",  1e-2 * CLHEP::steradian));
      registration(unit("millisteradian", "msr",  "solid_angle",  1e-3 * CLHEP::steradian));
      registration(unit("microsteradian", "usr",  "solid_angle",  1e-6 * CLHEP::steradian));
      registration(unit("nanosteradian",  "nsr",  "solid_angle",  1e-9 * CLHEP::steradian));
      registration(unit("picosteradian",  "psr",  "solid_angle", 1e-12 * CLHEP::steradian));
      registration(unit("femtosteradian", "fsr",  "solid_angle", 1e-15 * CLHEP::steradian));

      // Energy:
      registration(unit("joule",               "J", "energy;[M][L2][T-2]", CLHEP::joule, true));
      registration(unit("nanoelectronvolt",  "neV", "energy",  1e-9 * CLHEP::electronvolt));
      registration(unit("microelectronvolt", "ueV", "energy",  1e-6 * CLHEP::electronvolt));
      registration(unit("millielectronvolt", "meV", "energy",  1e-3 * CLHEP::electronvolt));
      registration(unit("electronvolt",       "eV", "energy",         CLHEP::electronvolt));
      registration(unit("kiloelectronvolt", "keV",  "energy",         CLHEP::kiloelectronvolt));
      registration(unit("megaelectronvolt", "MeV",  "energy",         CLHEP::megaelectronvolt));
      registration(unit("gigaelectronvolt", "GeV",  "energy",         CLHEP::gigaelectronvolt));
      registration(unit("teraelectronvolt", "TeV",  "energy",         CLHEP::teraelectronvolt));
      registration(unit("petaelectronvolt", "PeV",  "energy",         CLHEP::petaelectronvolt));

      // Force (1 N = 1 kg⋅m/s2) :
      registration(unit("newton",      "N", "force;[M][L][T-2]",        CLHEP::newton, true));
      registration(unit("kilonewton", "kN", "force",             1.e3 * CLHEP::newton));
      const double dyne = 1.e-5 * CLHEP::newton;
      registration(unit("dyne",       "dyn",   "force",  dyne));

      // Surface tension (1 N/m = 1 kg/s2) :
      registration(unit("N/m",    "surface_tension;[M][T-2]", CLHEP::newton / CLHEP::meter, true));
      registration(unit("dyn/cm", "surface_tension",          dyne / CLHEP::cm));

      // Pressure (1 N/m2) :
      registration(unit("pascal",       "Pa",   "pressure;[M][L-1][T-2]", CLHEP::hep_pascal, true));
      registration(unit("decapascal", "daPa",   "pressure",     10  * CLHEP::hep_pascal));
      registration(unit("hectopascal", "hPa",   "pressure",    100  * CLHEP::hep_pascal));
      registration(unit("kilopascal",  "kPa",   "pressure",   1.e3  * CLHEP::hep_pascal));
      registration(unit("megapascal",  "MPa",   "pressure",   1.e6  * CLHEP::hep_pascal));
      registration(unit("gigapascal",  "GPa",   "pressure",   1.e9  * CLHEP::hep_pascal));
      registration(unit("bar",         "bar",   "pressure",           CLHEP::bar));
      registration(unit("decibar",    "dbar",   "pressure",   1.e-1 * CLHEP::bar));
      registration(unit("centibar",   "cbar",   "pressure",   1.e-2 * CLHEP::bar));
      registration(unit("millibar",   "mbar",   "pressure",   1.e-3 * CLHEP::bar));
      registration(unit("kilobar",    "kbar",   "pressure",   1.e+3 * CLHEP::bar));
      registration(unit("megabar",    "Mbar",   "pressure",   1.e+6 * CLHEP::bar));
      registration(unit("atmosphere",  "atm",   "pressure",           CLHEP::atmosphere));
      const double torr = 133.322 * CLHEP::hep_pascal;
      registration(unit("torr",       "Torr",
                        /**/          "mmHg",   "pressure", torr));
      const double cmHg = 1333.22 * CLHEP::hep_pascal;
      registration(unit("cmHg",       "pressure", cmHg));

      // Density/volumetric mass density:
      registration(unit("kg/m3",             "density;[M][L-3]", CLHEP::kilogram / CLHEP::m3, true));
      registration(unit("kg/L",    "kg/l",
                        /**/     "kg/dm3",   "density",         CLHEP::kilogram / litre));
      registration(unit("g/mL",    "g/mL",   "density",         CLHEP::gram / millilitre));
      registration(unit("t/m3",              "density",  1.e3 * CLHEP::kilogram / CLHEP::m3));
      registration(unit("g/cm3",             "density",         CLHEP::gram / CLHEP::cm3));
      registration(unit("mg/cm3",            "density",         CLHEP::milligram / CLHEP::cm3));

      // Frequency:
      registration(unit("hertz",        "Hz",   "frequency;[T-1]",    CLHEP::hertz, true));
      registration(unit("millihertz",  "mHz",   "frequency",  1.e-3 * CLHEP::hertz));
      registration(unit("kilohertz",   "kHz",   "frequency",          CLHEP::kilohertz));
      registration(unit("megahertz",   "MHz",   "frequency",          CLHEP::megahertz));
      registration(unit("gigahertz",   "GHz",   "frequency",   1.e3 * CLHEP::megahertz));

      // Angular velocity/frequency:
      registration(unit("rad/s",       "angular_frequency;[T-1]",   CLHEP::radian / CLHEP::second, true));

      // Activity:
      registration(unit("becquerel",       "Bq",    "activity;[T-1]",    CLHEP::becquerel, true));
      registration(unit("millibecquerel", "mBq",    "activity", 1.e-3  * CLHEP::becquerel));
      registration(unit("microbecquerel", "uBq",    "activity", 1.e-6  * CLHEP::becquerel));
      registration(unit("kilobecquerel",  "kBq",    "activity", 1.e+3  * CLHEP::becquerel));
      registration(unit("megabecquerel",  "MBq",    "activity", 1.e+6  * CLHEP::becquerel));
      registration(unit("gigabecquerel",  "GBq",    "activity", 1.e+9  * CLHEP::becquerel));
      registration(unit("curie",          "Ci",     "activity",          CLHEP::curie));
      registration(unit("millicurie",    "mCi",     "activity", 1.e-3  * CLHEP::curie));
      registration(unit("microcurie",    "uCi",     "activity", 1.e-6  * CLHEP::curie));
      registration(unit("nanocurie",     "nCi",     "activity", 1.e-9  * CLHEP::curie));
      registration(unit("picocurie",     "pCi",     "activity", 1.e-12 * CLHEP::curie));
      registration(unit("kilocurie",     "kCi",     "activity", 1.e+3  * CLHEP::curie));
      registration(unit("megacurie",     "MCi",     "activity", 1.e+6  * CLHEP::curie));
      const double dpm = CLHEP::becquerel / 60.0;
      registration(unit("dpm",                      "activity", dpm));

      // Volume activity:
      const double Bq_per_m3 = CLHEP::becquerel / CLHEP::m3;
      const double dpm_per_m3 = dpm / CLHEP::m3;
      registration(unit("Bq/m3",     "volume_activity;[L-3][T-1]", Bq_per_m3, true));
      registration(unit("mBq/m3",    "volume_activity", 1.e-3 * Bq_per_m3));
      registration(unit("uBq/m3",    "volume_activity", 1.e-6 * Bq_per_m3));
      registration(unit("kBq/m3",    "volume_activity", 1.e+3 * Bq_per_m3));
      registration(unit("MBq/m3",    "volume_activity", 1.e+6 * Bq_per_m3));
      registration(unit("GBq/m3",    "volume_activity", 1.e+9 * Bq_per_m3));
      registration(unit("dpm/m3",    "volume_activity", dpm_per_m3));

      // Surface activity:
      const double Bq_per_m2 = CLHEP::becquerel / CLHEP::m2;
      const double dpm_per_m2 = dpm / CLHEP::m2;
      registration(unit("Bq/m2",     "surface_activity;[L-2][T-1]", Bq_per_m2, true));
      registration(unit("mBq/m2",    "surface_activity", 1.e-3 * Bq_per_m2));
      registration(unit("uBq/m2",    "surface_activity", 1.e-6 * Bq_per_m2));
      registration(unit("kBq/m2",    "surface_activity", 1.e+3 * Bq_per_m2));
      registration(unit("MBq/m2",    "surface_activity", 1.e+6 * Bq_per_m2));
      registration(unit("GBq/m2",    "surface_activity", 1.e+9 * Bq_per_m2));
      registration(unit("dpm/m2",    "surface_activity", dpm_per_m2));

      // Mass activity:
      const double Bq_per_kg = CLHEP::becquerel / CLHEP::kg;
      const double dpm_per_kg = dpm / CLHEP::kg;
      registration(unit("Bq/kg",     "mass_activity;[M-1][T-1]", Bq_per_kg, true));
      registration(unit("mBq/kg",    "mass_activity", 1.e-3 * Bq_per_kg));
      registration(unit("uBq/kg",    "mass_activity", 1.e-6 * Bq_per_kg));
      registration(unit("kBq/kg",    "mass_activity", 1.e+3 * Bq_per_kg));
      registration(unit("MBq/kg",    "mass_activity", 1.e+6 * Bq_per_kg));
      registration(unit("GBq/kg",    "mass_activity", 1.e+9 * Bq_per_kg));
      registration(unit("dpm/kg",    "mass_activity", dpm_per_kg));

      // Absorbed dose [L^2][T^-2]:
      registration(unit("gray",       "Gy", "absorbed_dose;[L2][T-2]",  CLHEP::gray, true));
      registration(unit("milligray", "mGy", "absorbed_dose",  CLHEP::milligray));
      registration(unit("microgray", "uGy", "absorbed_dose",  CLHEP::microgray));
      registration(unit("kilogray",  "kGy", "absorbed_dose",  CLHEP::kilogray));
      registration(unit("_rad",             "absorbed_dose",  0.01 * CLHEP::gray));

      // Equivalent dose [L^2][T^-2]:
      const double sievert = 1.0 * CLHEP::gray;
      registration(unit("sievert",       "Sv", "equivalent_dose;[L2][T-2]", sievert, true));
      registration(unit("millisievert", "mSv", "equivalent_dose",  1.e-3 * sievert));
      registration(unit("microsievert", "uSv", "equivalent_dose",  1.e-6 * sievert));
      registration(unit("nanosievert",  "nSv", "equivalent_dose",  1.e-9 * sievert));
      registration(unit("picosievert",  "pSv", "equivalent_dose",  1.e-12 * sievert));
      registration(unit("rem",                 "equivalent_dose", 0.01 * sievert));

      // Fraction:
      registration(unit("percent",          "%",
                        /**/            "1/100",    "fraction;[1]", CLHEP::perCent, true));
      registration(unit("perthousand", "1/1000",    "fraction", CLHEP::perThousand));
      registration(unit("ppm",                      "fraction", CLHEP::perMillion));
      registration(unit("ppb",                      "fraction", 1.e-3 * CLHEP::perMillion));
      registration(unit("ppt",                      "fraction", 1.e-6 * CLHEP::perMillion));
      registration(unit("ppq",                      "fraction", 1.e-9 * CLHEP::perMillion));

      // Level:
      const double decibel = 1.0;
      registration(unit("decibel", "dB", "level;[1]",        decibel, true));
      registration(unit("bel",      "B", "level", 10.0 * decibel));
      registration(unit("neper",   "Np", "level", (20. / std::log(10.0)) * decibel));

      // Velocity:
      registration(unit("m/s",      "velocity;[L][T-1]",  CLHEP::meter      / CLHEP::second, true));
      registration(unit("m/h",      "velocity",  CLHEP::meter      / hour));
      registration(unit("mm/s",     "velocity",  CLHEP::millimeter / CLHEP::second));
      registration(unit("cm/s",     "velocity",  CLHEP::centimeter / CLHEP::second));
      registration(unit("cm/h",     "velocity",  CLHEP::centimeter / hour));
      registration(unit("km/s",     "velocity",  CLHEP::kilometer  / CLHEP::second));
      registration(unit("km/h",     "velocity",  CLHEP::kilometer  / hour));
      registration(unit("m/ms",     "velocity",  CLHEP::meter      / CLHEP::millisecond));
      registration(unit("mm/ms",    "velocity",  CLHEP::millimeter / CLHEP::millisecond));
      registration(unit("cm/ms",    "velocity",  CLHEP::centimeter / CLHEP::millisecond));
      registration(unit("m/us",     "velocity",  CLHEP::meter      / CLHEP::microsecond));
      registration(unit("mm/us",    "velocity",  CLHEP::millimeter / CLHEP::microsecond));
      registration(unit("cm/us",    "velocity",  CLHEP::centimeter / CLHEP::microsecond));
      registration(unit("m/ns",     "velocity",  CLHEP::meter      / CLHEP::nanosecond));
      registration(unit("mm/ns",    "velocity",  CLHEP::millimeter / CLHEP::nanosecond));
      registration(unit("cm/ns",    "velocity",  CLHEP::centimeter / CLHEP::nanosecond));
      registration(unit("mph",      "velocity",  mile / hour));

      // Acceleration:
      registration(unit("m/s2",     "acceleration;[L][T-2]",  CLHEP::meter / CLHEP::second / CLHEP::second, true));

      // Wave number:
      registration(unit("/m", "m-1", "wave_number;[L-1]",  1.0 / CLHEP::meter, true));

      // Electric charge (C = A·s) :
      registration(unit("coulomb",      "C",    "electric_charge;[T][I]",  CLHEP::coulomb, true));
      registration(unit("millicoulomb", "mC",   "electric_charge", 1e-3  * CLHEP::coulomb));
      registration(unit("microcoulomb", "uC",   "electric_charge", 1e-6  * CLHEP::coulomb));
      registration(unit("nanocoulomb",  "nC",   "electric_charge", 1e-9  * CLHEP::coulomb));
      registration(unit("picocoulomb",  "pC",   "electric_charge", 1e-12 * CLHEP::coulomb));
      registration(unit("femtocoulomb", "fC",   "electric_charge", 1e-15 * CLHEP::coulomb));

      // Electric potential (V = J/C = kg·m2·s−3·A−1) :
      registration(unit("volt",       "V",    "electric_potential;[M][L2][T-3][I-1]", CLHEP::volt, true));
      registration(unit("millivolt", "mV",    "electric_potential", 1e-3  * CLHEP::volt));
      registration(unit("kilovolt",  "kV",    "electric_potential",         CLHEP::kilovolt));
      registration(unit("megavolt",  "MV",    "electric_potential",         CLHEP::megavolt));

      // Electric resistance ( Ω = V/A = kg·m2·s−3·A−2) :
      registration(unit("ohm",       "O",   "electric_resistance;[M][L2][T-3][I-2]", CLHEP::ohm, true));
      registration(unit("kiloohm",  "kO",   "electric_resistance", 1.e3 * CLHEP::ohm));
      registration(unit("megaohm",  "MO",   "electric_resistance", 1.e6 * CLHEP::ohm));

      // Resistivity (Ω·m = kg·m3·s−3·A−2 ):
      registration(unit("ohm_meter", "ohm.m;O.m", "resistivity;[M][L3][T-3][I-2]",   CLHEP::ohm * CLHEP::meter, true));

      // Power (W = V·A = kg·m2·s−3) :
      registration(unit("watt",       "W",   "power;[M][L2][T-3]", CLHEP::watt, true));
      registration(unit("milliwatt", "mW",   "power", 1.e-3 * CLHEP::watt));
      registration(unit("microwatt", "uW",   "power", 1.e-6 * CLHEP::watt));
      registration(unit("nanowatt",  "nW",   "power", 1.e-9 * CLHEP::watt));
      registration(unit("kilowatt",  "kW",   "power", 1.e+3 * CLHEP::watt));
      registration(unit("megawatt",  "MW",   "power", 1.e+6 * CLHEP::watt));
      registration(unit("gigawatt",  "GW",   "power", 1.e+9 * CLHEP::watt));

      // Capacitance (F = C/V = kg−1·m−2·A2·s4) :
      registration(unit("farad",       "F",   "capacitance;[M-1][L-2][T4][I2]", CLHEP::farad, true));
      registration(unit("millifarad", "mF",   "capacitance",   1.e-3  * CLHEP::farad));
      registration(unit("microfarad", "uF",   "capacitance",   1.e-6  * CLHEP::farad));
      registration(unit("nanofarad",  "nF",   "capacitance",   1.e-9  * CLHEP::farad));
      registration(unit("picofarad",  "pF",   "capacitance",   1.e-12 * CLHEP::farad));
      registration(unit("femtofarad", "fF",   "capacitance",   1.e-15 * CLHEP::farad));

      // Electric flux (V·m = kg·m3·s−3·A−1 = L3MT–3I–1) :
      registration(unit("V.m",       "V.m",    "electric_flux;[M][L3][T-3][I-1]", CLHEP::volt * CLHEP::meter, true));

      // Electric field V/m = N/C = kg·m·A−1·s−3) :
      registration(unit("V/m",       "V/m",    "electric_field;[M][L][T-3][I-1]", CLHEP::volt / CLHEP::meter, true));
      registration(unit("V/cm",     "V/cm",    "electric_field", CLHEP::volt / CLHEP::centimeter));
      registration(unit("kV/m",     "kV/m",    "electric_field", CLHEP::kilovolt / CLHEP::meter));
      registration(unit("kV/cm",   "kV/cm",    "electric_field", CLHEP::kilovolt / CLHEP::centimeter));

      // Electric displacement (C/m2 = A·s·m−2) :
      registration(unit("C/m2",                "electric_displacement_field;[L-2][T][I]", CLHEP::coulomb / CLHEP::meter2, true));

      // Permittivity (F/m = kg−1·m−3·A2·s4) :
      registration(unit("F/m",                 "permittivity;[M-1][L-3][T4][I2]", CLHEP::farad / CLHEP::meter, true));

      // Conductance (S = Ω−1 = kg−1·m−2·s3·A2):
      const double siemens = CLHEP::ampere * CLHEP::ampere * CLHEP::second * CLHEP::second * CLHEP::second / CLHEP::kg / CLHEP::meter2;
      registration(unit("siemens",    "S",   "conductance;[M-1][L-2][T3][I2]", siemens, true));

      // Conductivity:
      const double siemens_per_meter = siemens / CLHEP::meter2;
      registration(unit("S/m",               "conductivity;[M-2][L-2][T3][I2]", siemens_per_meter , true));

      // Magnetic flux density/magnetic induction (T =  Wb/m2 = kg·s−2·A−1 = N·A−1·m−1) :
      registration(unit("tesla",       "T",
                        "magnetic_flux_density;[M][T-2][I-1]",
                        CLHEP::tesla, true));
      registration(unit("millitesla", "mT",     "magnetic_flux_density", 1.e-3 * CLHEP::tesla));
      registration(unit("microtesla", "uT",     "magnetic_flux_density", 1.e-6 * CLHEP::tesla));
      registration(unit("nanotesla",  "nT",     "magnetic_flux_density", 1.e-9 * CLHEP::tesla));
      registration(unit("gauss",       "G",
                        /**/           "Gs",    "magnetic_flux_density",         CLHEP::gauss));
      registration(unit("milligauss", "mG",     "magnetic_flux_density", 1.e-3 * CLHEP::gauss));
      registration(unit("kilogauss",  "kG",     "magnetic_flux_density",         CLHEP::kilogauss));

      // Magnetic flux (Wb = V·s = kg·m2·s−2·A−1) :
      registration(unit("weber",       "Wb",     "magnetic_flux;[M][L2][T-2][I-1]", CLHEP::weber, true));
      registration(unit("milliweber", "mWb",     "magnetic_flux", 1.e-3 * CLHEP::weber));
      registration(unit("microweber", "uWb",     "magnetic_flux", 1.e-6 * CLHEP::weber));
      registration(unit("nanoweber",  "nWb",     "magnetic_flux", 1.e-9 * CLHEP::weber));
      registration(unit("maxwell",     "Mx",     "magnetic_flux", 1.e-8 * CLHEP::weber));

      // Magnetic field strength (A/m = A·m−1) :
      registration(unit("A/m",                   "magnetic_field_strength;[L-1][I]",  CLHEP::ampere / CLHEP::meter, true));

      // Inductance (H = Wb/A = V·s/A = kg·m2·s−2·A−2):
      registration(unit("henry", "H", "inductance;[M][L2][T-2][I-2]", CLHEP::henry, true));

      // Permeability (H/m = kg·m·s−2·A−2) :
      registration(unit("H/m",        "permeability;[M][L][T-2][I-2]", CLHEP::henry / CLHEP::meter, true));

      return;
    }

  } // end of namespace units

} // end of namespace datatools
