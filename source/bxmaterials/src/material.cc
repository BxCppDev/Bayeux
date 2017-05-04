/// material.cc

// Ourselves:
#include <materials/material.h>

// Standard Library:
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

// Third Party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/units.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>

// This Project:
#include <materials/materials_config.h>
#include <materials/isotope.h>
#include <materials/element.h>

namespace materials {

  // static
  std::string material::state_to_label(state_type state_)
  {
    switch(state_) {
    case STATE_GAS    : return "gas";
    case STATE_LIQUID : return "liquid";
    case STATE_SOLID  : return "solid";
    default: break;
    }
    return "";
  }

  // static
  material::state_type material::label_to_state(const std::string & label_)
  {
    if (label_ == state_to_label(STATE_GAS)) return STATE_GAS;
    if (label_ == state_to_label(STATE_LIQUID)) return STATE_LIQUID;
    if (label_ == state_to_label(STATE_SOLID)) return STATE_SOLID;
    return STATE_UNKNOWN;
  }

  double material::g_per_cm3()
  {
    return CLHEP::g / CLHEP::cm3;
  }

  compound_entry::compound_entry()
  {
    _owned_ = false;
    _elt_ptr_ = 0;
    _mat_ptr_ = 0;
    datatools::invalidate(_weight_);
    _nb_of_atoms_ = -1;
    return;
  }

  void compound_entry::reset_compound()
  {
    if (_elt_ptr_ != 0) {
      if (_owned_) delete _elt_ptr_;
      _elt_ptr_ = 0;
    }
    if (_mat_ptr_ != 0) {
      if (_owned_) delete _mat_ptr_;
      _mat_ptr_ = 0;
    }
    _owned_ = false;
    return;
  }

  void compound_entry::reset()
  {
    reset_compound();
    datatools::invalidate(_weight_);
    _nb_of_atoms_ = -1;
    return;
  }

  compound_entry::~compound_entry()
  {
    reset_compound();
    return;
  }

  bool compound_entry::is_valid() const
  {
    return (has_element() && (datatools::is_valid(_weight_) || (_nb_of_atoms_ != -1)))
      || (has_material() && datatools::is_valid(_weight_));
  }

  bool compound_entry::has_weight() const
  {
    return datatools::is_valid(_weight_);
  }

  bool compound_entry::has_number_of_atoms() const
  {
    return _nb_of_atoms_ != -1;
  }

  bool compound_entry::has_element() const
  {
    return _elt_ptr_ != 0;
  }

  bool compound_entry::has_material() const
  {
    return _mat_ptr_ != 0;
  }

  bool compound_entry::is_owned() const
  {
    return _owned_;
  }

  double compound_entry::get_weight() const
  {
    return _weight_;
  }

  void compound_entry::set_weight(double val_)
  {
    DT_THROW_IF(val_ < 0., std::range_error, "Invalid negative weight !");
    _weight_ = val_;
    return;
  }

  void compound_entry::set_number_of_atoms(int val_)
  {
    DT_THROW_IF(val_ < 0, std::range_error, "Invalid negative number of atoms !");
    _nb_of_atoms_ = val_;
    return;
  }

  int compound_entry::get_number_of_atoms() const
  {
    return _nb_of_atoms_;
  }

  void compound_entry::set_element(const element & elt_, bool owned_)
  {
    reset_compound();
    _elt_ptr_ = &elt_;
    _owned_ = owned_;
    return;
  }

  void compound_entry::set_material(const material & mat_, bool owned_)
  {
    reset_compound();
    _mat_ptr_ = &mat_;
    _owned_ = owned_;
    return;
  }

  void compound_entry::set_element(const element * elt_, bool owned_)
  {
    reset_compound();
    _elt_ptr_ = elt_;
    _owned_ = owned_;
    return;
  }

  void compound_entry::set_material(const material * mat_, bool owned_)
  {
    reset_compound();
    _mat_ptr_ = mat_;
    _owned_ = owned_;
    return;
  }

  const element & compound_entry::get_element() const
  {
    DT_THROW_IF (!has_element(),std::logic_error, "Not a compound element !");
    return * _elt_ptr_;
  }

  const material & compound_entry::get_material() const
  {
    DT_THROW_IF (!has_material(),std::logic_error, "Not a compound material !");
    return * _mat_ptr_;
  }

  void material::_set_defaults()
  {
    _name_   = "";
    datatools::invalidate(_density_);
    _state_ = STATE_UNKNOWN;
    datatools::invalidate(_pressure_);
    datatools::invalidate(_temperature_);
    datatools::invalidate(_mean_z_);
    datatools::invalidate(_mean_a_);
    _proportion_unit_ = KP_UNDEFINED;
    return;
  }

  bool material::is_composed_by_mean_z_a() const
  {
    return _proportion_unit_ == MEAN_ZA;
  }

  bool material::is_composed_by_number_of_atoms() const
  {
    return _proportion_unit_ == NUMBER_OF_ATOMS;
  }

  bool material::is_composed_by_fraction_mass() const
  {
    return _proportion_unit_ == FRACTION_MASS;
  }

  const composition_map_type & material::get_composition() const
  {
    return _composition_;
  }

  material::material()
  {
    _locked_ = false;
    _set_defaults();
    return;
  }

  material::material(const std::string & name_)
  {
    _locked_ = false;
    _set_defaults();
    set_name(name_);
    return;
  }

  material::material(const char * name_)
  {
    _locked_ = false;
    _set_defaults();
    set_name(std::string(name_));
    return;
  }

  material::~material()
  {
    reset();
    return;
  }

  const std::string & material::get_name() const
  {
    return _name_;
  }

  void material::set_name(const std::string & name_)
  {
    _name_ = name_;
    return;
  }

  double material::get_mean_z() const
  {
    return _mean_z_;
  }

  void material::_set_mean_z_(double z_)
  {
    if (datatools::is_valid(z_)) {
      DT_THROW_IF (z_ <= 0.0,
                   std::logic_error,
                   "Invalid value for mean atomic number !");
    }
    _mean_z_ = z_;
    return;
  }

  double material::get_mean_a() const
  {
    return _mean_a_;
  }

  void  material::_set_mean_a_(double a_)
  {
    if (datatools::is_valid(a_)) {
      DT_THROW_IF (a_ <= 0.0,
                   std::logic_error,
                   "Invalid value for mean mass number !");
    }
    _mean_a_ = a_;
    return;
  }

  void material::set_mean_z_a (double z_, double a_)
  {
    DT_THROW_IF (is_locked (),std::logic_error, "Operation not allowed ! Object is locked !");
    if (_proportion_unit_ == KP_UNDEFINED) {
      _proportion_unit_ = KP_ZA;
    } else {
      DT_THROW_IF (_proportion_unit_ != KP_ZA,
                   std::logic_error,
                   "Setting Z/A is not allowed in this mode !");
    }
    _set_mean_z_(z_);
    _set_mean_a_(a_);
    return;
  }

  bool material::has_state() const
  {
    return _state_ != STATE_UNKNOWN;
  }

  void material::set_state(state_type state_)
  {
    _state_ = state_;
    return;
  }

  material::state_type material::get_state() const
  {
    return _state_;
  }

  bool material::has_density() const
  {
    return datatools::is_valid(_density_);
  }

  double material::get_density() const
  {
    return _density_;
  }

  void material::set_density(double density_)
  {
    if (datatools::is_valid(density_)) {
      DT_THROW_IF (density_ <= 0.0,
                   std::logic_error,
                   "Invalid value for density !");
    }
    _density_ = density_;
    return;
  }

  bool material::has_pressure() const
  {
    return datatools::is_valid(_pressure_);
  }

  double material::get_pressure() const
  {
    return _pressure_;
  }

  void material::set_pressure(double pressure_)
  {
    if (datatools::is_valid(pressure_)) {
      DT_THROW_IF (pressure_ <= 0.0,
                   std::logic_error,
                   "Invalid value for pressure !");
    }
    _pressure_ = pressure_;
    return;
  }

  bool material::has_temperature() const
  {
    return datatools::is_valid(_temperature_);
  }

  double material::get_temperature() const
  {
    return _temperature_;
  }

  void material::set_temperature(double temperature_)
  {
    if (datatools::is_valid(temperature_)) {
      DT_THROW_IF (temperature_ <= 0.0,
                   std::logic_error,
                   "Invalid value for temperature !");
    }
    _temperature_ = temperature_;
    return;
  }

  bool material::is_locked() const
  {
    return _locked_;
  }

  void material::add_element_by_mass(const element & elt_ref_, double weight_, bool owned_)
  {
    DT_THROW_IF (is_locked(),std::logic_error, "Operation not allowed ! Object is locked !");

    // check proportion_unit_type when add a new element
    // if you are adding the first element then set the proportion_unit

    if ( _composition_.size() == 0) {
      _proportion_unit_ = KP_MASS;
    } else {
      DT_THROW_IF(_proportion_unit_!=KP_MASS,
                  std::logic_error,
                  "Unmatching proportion unit  !");
    }

    // check if the element is locked (which mean valid)

    DT_THROW_IF (! elt_ref_.is_locked(),
                 std::logic_error,
                 "Element '" << elt_ref_.get_name () << "' is not locked !'");

    // check the positive value of weight

    DT_THROW_IF ( weight_ <= 0.,
                  std::logic_error,
                  "Weight value '" << weight_  << "' is not positive !");

    {
      compound_entry entry;
      // entry.set_element(elt_ref_, false);
      // entry.set_weight(weight_);
      _composition_[elt_ref_.get_name()] = entry;
    }
    _composition_[elt_ref_.get_name()].set_element(elt_ref_, owned_);
    _composition_[elt_ref_.get_name()].set_weight(weight_);
    return;
  }

  void material::add_element_by_mass(const element * elt_ptr_, double weight_)
  {
    add_element_by_mass(*elt_ptr_, weight_);
    return;
  }

  void material::add_element_by_nb_of_atoms(const element & elt_ref_,
                                            int nb_of_atoms_,
                                            bool owned_)
  {
    DT_THROW_IF (is_locked(),std::logic_error, "Operation not allowed ! Object is locked !");

    // check proportion_unit_type when add a new element
    // if you are adding the first element then set the proportion_unit

    if (_composition_.size() == 0) {
      _proportion_unit_ = KP_ATOM;
    } else {
      DT_THROW_IF (_proportion_unit_ != KP_ATOM,
                   std::logic_error,
                   "Unmatching proportion unit  !");
    }

    // check if the element is locked (which mean valid)

    DT_THROW_IF (! elt_ref_.is_locked(),
                 std::logic_error,
                 "Element '" << elt_ref_.get_name() << "' is not locked !'");

    // check the positive value of weight

    DT_THROW_IF ( nb_of_atoms_ <= 0.,
                  std::logic_error,
                  "Nb of atoms value '" << nb_of_atoms_ << "' is not positive !");

    {
      compound_entry entry;
      _composition_[elt_ref_.get_name()] = entry;
    }
    /*
    entry.owned = false;
    entry.elt_ptr = &elt_ref_;
    entry.nb_of_atoms = nb_of_atoms_;
    */
    _composition_[elt_ref_.get_name()].set_element(elt_ref_, owned_);
    _composition_[elt_ref_.get_name()].set_number_of_atoms(nb_of_atoms_);
    return;
  }

  void material::add_element_by_nb_of_atoms(const element * elt_ptr_, int nb_of_atoms_)
  {
    add_element_by_nb_of_atoms(*elt_ptr_, nb_of_atoms_);
    return;
  }

  void material::add_material_by_mass(const material & mat_ref_, double weight_, bool owned_)
  {
    DT_THROW_IF (is_locked(),std::logic_error, "Operation not allowed ! Object is locked !");

    // Check proportion_unit_type when add a new element
    // if you are adding the first element then set the proportion_unit

    if ( _composition_.size () == 0) {
      _proportion_unit_ = KP_MASS;
    } else {
      DT_THROW_IF(_proportion_unit_ != KP_MASS,
                  std::logic_error,
                  "Unmatching proportion unit  !");
    }

    // Check if the element is locked (which mean valid)

    DT_THROW_IF (! mat_ref_.is_locked(),
                 std::logic_error,
                 "Material '" << mat_ref_.get_name() << "' is not locked !'");

    // Check the positive value of weight

    DT_THROW_IF ( weight_ <= 0.,
                  std::logic_error,
                  "Material weight value '" << weight_ << "' is not positive !");

    {
      compound_entry entry;
      _composition_[mat_ref_.get_name()] = entry;
    }
    _composition_[mat_ref_.get_name()].set_material(mat_ref_, owned_);
    _composition_[mat_ref_.get_name()].set_weight(weight_);

    /*
    owned = false;
    entry.mat_ptr = &mat_ref_;
    entry.weight = weight_;
    _composition_[entry.mat_ptr->get_name()] = entry;
    _composition_[entry.mat_ptr->get_name()].owned = owned_;
    */
    return;
  }

  void material::add_material_by_mass(const material * mat_ptr_ , double weight_)
  {
    add_material_by_mass(*mat_ptr_, weight_);
    return;
  }

  void material::build(unsigned int build_flags_)
  {
    bool devel = false;
    //devel = true;
    if (devel) std::cerr << "DEVEL: material::build: Entering..." << std::endl;
    DT_THROW_IF (is_locked(),std::logic_error, "Operation not allowed ! Object is locked !");
    DT_THROW_IF (! datatools::is_valid(_density_), std::logic_error, "Invalid density !");
    if (is_composed_by_mean_z_a()) {
      if (devel) std::cerr << "DEVEL: material::build: by mean Z/A..." << std::endl;
      DT_THROW_IF (! datatools::is_valid(_mean_z_), std::logic_error, "Invalid mean atomic number (Z) !");
      DT_THROW_IF (! datatools::is_valid(_mean_a_), std::logic_error, "Invalid mean mass number (A) !");
    } else {
      if (devel) std::cerr << "DEVEL: material::build: by composition: " << _composition_.size () << std::endl;
      DT_THROW_IF(_composition_.size() == 0,
                  std::logic_error,
                  "Missing compound(s) in the current material ! Not implemented yet !");
      if (is_composed_by_fraction_mass()) {
        if (devel) std::cerr << "DEVEL: material::build: by fraction mass..." << std::endl;
        _normalize_weights_();
      }
      _compute_molar_mass_();
    }
    if (build_flags_ & BF_LOCK) {
      lock();
    }
    if (devel) std::cerr << "DEVEL: material::build: Exiting." << std::endl;
    return;
  }

  void material::_normalize_weights_()
  {
    DT_THROW_IF (! is_composed_by_fraction_mass(),
                 std::logic_error, "Invalid composition mode !");
    DT_THROW_IF (_composition_.size() == 0,
                 std::logic_error,
                 "Empty composition map !");
    // Compute the sum of weight (fraction of mass):
    double sum_weight = 0.0;
    for (composition_map_type::const_iterator i
           = _composition_.begin();
         i != _composition_.end();
         i++) {
      const compound_entry & entry = i->second;
      sum_weight += entry.get_weight();
    }
    // Normalize all weights (fraction of mass):
    for (composition_map_type::iterator i = _composition_.begin();
         i != _composition_.end();
         i++) {
      compound_entry & entry = i->second;
      entry.set_weight(entry.get_weight() / sum_weight);
    }
    return;
  }

  void material::_compute_molar_mass_()
  {
    // Not implemented yet
    /*  double molar_mass =0.;

        for (isotope_weight_map_t::const_iterator i = _composition_.begin ();
        i != _composition_.end ();
        i++)
        {
        const iso_entry & entry = i->second;
        molar_mass += (entry.weight) * (entry.iso_ptr->get_mass ());
        }
        __set_molar_mass (molar_mass);
    */
    return;
  }

  void material::lock()
  {
    _locked_ = true;
    return;
  }

  void material::unlock()
  {
    _locked_ = false;
    return;
  }

  void material::reset()
  {
    _properties_.clear();
    _composition_.clear();
    _set_defaults();
    return;
  }

  const datatools::properties & material::get_properties() const
  {
    return _properties_;
  }

  datatools::properties & material::grab_properties()
  {
    return _properties_;
  }

  void material::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool /*inherit_*/) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Name         : '" << get_name() << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Density      : " << get_density() / g_per_cm3() << " g/cm3" << std::endl;

    if (has_state()) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "State        : '" << state_to_label(get_state()) << "'" << std::endl;
    }

    if (has_pressure()) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Pressure     : " << get_pressure() / CLHEP::pascal << " Pa" << std::endl;
    }

    if (has_temperature()) {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Temperature  : " << get_temperature() / CLHEP::kelvin << " K" << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Composition  : ";
    if (is_composed_by_mean_z_a()) {
      out_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::tag << "Mean Z : " <<_mean_z_ << std::endl;
      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::last_tag << "Mean A : "
           << _mean_a_ << " g/mol" << std::endl;
    } else {
      if ( _composition_.size() == 0) {
        out_ << " <empty>" << std::endl;
      } else {
        if (_proportion_unit_ == KP_ATOM) {
          out_ << "(# atoms) "<< std::endl;
          for (composition_map_type::const_iterator i
                 = _composition_.begin();
               i != _composition_.end();
               i++) {
            const compound_entry & entry = i->second;
            composition_map_type::const_iterator j = i;
            j++;
            std::string atag = datatools::i_tree_dumpable::tags::item();
            if (j == _composition_.end()) {
              atag = datatools::i_tree_dumpable::tags::last_item();
            }
            out_ << indent << datatools::i_tree_dumpable::skip_tag << atag << "Element '"
                 << entry.get_element().get_name() << "'" << " : "
                 << entry.get_number_of_atoms() << " "
                 << (entry.is_owned()? "(owned)": "(not owned)") << std::endl;
          }
        }

        if (_proportion_unit_ == KP_MASS) {
          out_ << "(% mass)  "<< std::endl;
          for (composition_map_type::const_iterator i
                 = _composition_.begin();
               i != _composition_.end();
               i++) {
            const compound_entry & entry = i->second;
            composition_map_type::const_iterator j = i;
            j++;
            std::string atag = datatools::i_tree_dumpable::tags::item();
            if (j == _composition_.end ()) atag = datatools::i_tree_dumpable::tags::last_item();
            out_ << indent << datatools::i_tree_dumpable::skip_tag << atag;
            if (entry.is_valid()) {
              if (entry.has_element()) {
                out_  << "Element  : '" << entry.get_element().get_name ();
              } else if (entry.has_material()) {
                out_  << "Material : '" << entry.get_material().get_name ();
              }
              out_ << "'" << " : " << entry.get_weight() << " "
                   << (entry.is_owned()? "(owned)": "(not owned)");
            } else {
              out_ << "Invalid compound entry.";
            }
            out_ << std::endl;
          }
        }
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Properties   : ";
      if ( _properties_.size() == 0) {
        out_ << "<empty>" << std::endl;
      } else {
        out_ << std::endl;
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _properties_.tree_dump(out_,"",indent_oss.str ());
      }
    }

    {
      out_ << indent << i_tree_dumpable::last_tag
           << "Locked       : " << (is_locked()? "Yes": "No") << std::endl;
    }

    return;
  }

} // end of namespace materials
