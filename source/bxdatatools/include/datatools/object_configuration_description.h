// -*- mode: c++; -*-
/* object_configuration_description.h */
/* 
 * Description :
 *
 *  Classes describing configuration properties for complex objects
 *  that uses a deferred initilization mechanism using 'datatools::properties'
 *  container.
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_OBJECT_CONFIGURATION_DESCRIPTION_H_
#define DATATOOLS_OBJECT_CONFIGURATION_DESCRIPTION_H_

#include <iostream>
#include <string>
#include <vector>

#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>

#include <datatools/properties.h>
#include <datatools/types.h>

namespace datatools {

class object_configuration_description;

class configuration_property_description
{
public:

  static const bool SCALAR = false;
  static const bool ARRAY  = true;
  static const bool IMPLICIT_UNIT = false;
  static const bool EXPLICIT_UNIT = true;
  static const bool MUTABLE = false;
  static const bool CONST = true;

  struct dependency_entry {
    dependency_entry();
    bool is_valid() const;
    bool has_name() const;
    bool has_address() const;
    const std::string & get_name() const;
    const configuration_property_description & ref() const;
    std::string name;
    const configuration_property_description * address;
  };

  configuration_property_description();

  configuration_property_description & set_name_pattern(const std::string &np_);
  configuration_property_description & set_terse_description(const std::string &desc_);
  configuration_property_description & set_long_description(const std::string &desc_);
  configuration_property_description & set_mandatory(bool m_ = true);
  configuration_property_description & set_traits(int type_, 
                                                  bool array_ = false, 
                                                  int fixed_size_ = -1);
  configuration_property_description & set_triggered_by_flag(const std::string &prop_name_);
  configuration_property_description & set_complex_triggering_conditions(bool = true);
  configuration_property_description & set_complex_dependencies(bool = true);
  configuration_property_description & set_explicit_unit(bool = true);
  configuration_property_description & set_unit_symbol(const std::string &);
  configuration_property_description & set_unit_label(const std::string &);
  configuration_property_description & set_const(bool = true);
  configuration_property_description & set_path(bool = true);

  bool is_valid() const;
  bool has_type() const;
  bool is_const() const;
  bool is_mutable() const;
  bool is_boolean() const;
  bool is_integer() const;
  bool is_real() const;
  bool is_string() const;
  bool is_path() const;
  bool is_static() const;
  bool is_dynamic() const;
  bool is_trigger() const;
  bool is_triggered_by_flag() const;
  const std::string & get_name_pattern() const;
  bool has_terse_description() const;
  const std::string & get_terse_description() const;
  bool has_long_description() const;
  const std::string & get_long_description() const;
  bool is_mandatory() const;
  int  get_type() const;
  bool is_scalar() const;
  bool is_array() const;
  bool has_explicit_unit() const;
  bool has_unit_label() const;
  const std::string & get_unit_label() const;
  bool has_unit_symbol() const;
  const std::string & get_unit_symbol() const;
  bool is_fixed_sized_array() const;
  int get_array_fixed_size() const;
  void print(std::ostream &out_, const std::string & indent_ = "") const;
  bool has_complex_triggering_conditions() const;
  bool has_complex_dependencies() const;
  const dependency_entry & get_triggered_by_flag() const;
  const dependency_entry & get_dynamic_dependee() const;
  bool has_dynamic_dependers() const;
  unsigned int get_number_of_dynamic_dependers() const;
  const dependency_entry & get_dynamic_depender(int i_) const;
 
  void dump(std::ostream & out_ = std::clog, 
            const std::string & title_ = "",
            const std::string & indent_ = "") const;

private:

  std::string  _name_pattern_;
  std::string  _terse_description_;
  std::string  _long_description_;
  int          _type_;
  bool         _const_;
  bool         _path_;
  bool         _explicit_unit_;
  std::string  _unit_label_;
  std::string  _unit_symbol_;
  bool         _array_;
  int          _array_fixed_size_;
  bool         _mandatory_;
  bool         _complex_triggering_conditions_;
  bool         _complex_dependencies_;
  std::vector<dependency_entry> _dynamic_dependers_;
  dependency_entry              _dynamic_dependee_;
  std::vector<dependency_entry> _triggering_;
  dependency_entry              _triggered_by_flag_;

  friend class object_configuration_description;
};


class object_configuration_description
{
 public:

  typedef std::vector<configuration_property_description> cpd_col_type;

  bool is_available() const;

  object_configuration_description();

  object_configuration_description & 
    set_class_name(const std::string &);

  object_configuration_description &
    set_class_description(const std::string &);

  object_configuration_description &
    set_configuration_hints(const std::string &);

  object_configuration_description &
    set_validation_support(bool);

  const std::string & get_class_name() const;

  const std::string & get_class_description() const;
 
  bool has_class_description() const;
 
  const std::string & get_configuration_hints() const;

  bool has_configuration_hints() const;

  bool has_validation_support() const;

  void print(std::ostream & out_, const std::string & indent_ = "") const;

  unsigned int get_number_of_documented_properties() const;

  const configuration_property_description & get_property_info(int i_) const;
  
  configuration_property_description & add_property_info();

  bool validate(const datatools::properties & config_,
                std::string & error_message_) const;

  void generate_sample_configuration(std::ostream & out_, 
                                     const std::string & topic_ = "") const;

  bool is_locked() const;

  void lock();

  void dump(std::ostream & out_ = std::clog, 
            const std::string & title_ = "",
            const std::string & indent_ = "") const;

protected:

  void _generate_static_property(const configuration_property_description & cpd_,
                                 properties & props_) const;

  bool _validate_static(const configuration_property_description & cpd_,
                        const datatools::properties & config_,
                        std::string & error_message_) const;

  bool _validate_traits(const configuration_property_description & cpd_,
                        const datatools::properties & config_,
                        std::string & error_message_) const;

private:

  void _at_lock_();
  
private:

  bool        _locked_; /// Lock flag
  std::string _class_name_; /// The name of the class to be documented
  std::string _class_description_; /// An optional description string
  cpd_col_type _properties_infos_; /// Collection of documented properties
  std::string _configuration_hints_; /// Some embeded plain text documentation
  bool        _validation_support_; /// Flag to support validation
};

} // namespace datatools;

#endif // DATATOOLS_OBJECT_CONFIGURATION_DESCRIPTION_H_

// end of object_configuration_description.h
