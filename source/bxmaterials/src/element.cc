/* element.cc
 */
// - Ourselves
#include <materials/element.h>

// Standard Library
#include <stdexcept>
#include <sstream>
#include <fstream>

// Third Party
// - datatools
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>

// This Project
#include <materials/resource.h>
#include <materials/materials_config.h>
#include <materials/isotope.h>
#include <materials/chemical_symbol.h>

namespace materials {

  // static
  const int element::Z_UNDEFINED;

  iso_entry::iso_entry()
  {
    _owned_ = false;
    _iso_ptr_ = 0;
    datatools::invalidate(_weight_);
    return;
  }

  void iso_entry::reset_iso()
  {
    if (_iso_ptr_ != 0) {
      if (_owned_) delete _iso_ptr_;
      _iso_ptr_ = 0;
    }
    _owned_ = false;
    return;
  }

  iso_entry::~iso_entry()
  {
    reset_iso();
    return;
  }

  void iso_entry::reset()
  {
    reset_iso();
    datatools::invalidate(_weight_);
    return;
  }

  bool iso_entry::is_valid() const
  {
    return has_isotope() && datatools::is_valid(_weight_);
  }

  bool iso_entry::is_owned() const
  {
    return _owned_;
  }

  double iso_entry::get_weight() const
  {
    return _weight_;
  }

  bool iso_entry::has_weight() const
  {
    return datatools::is_valid(_weight_);
  }

  void iso_entry::set_weight(double val_)
  {
    DT_THROW_IF(val_ < 0., std::range_error, "Invalid negative weight !");
    _weight_ = val_;
    return;
  }

  void iso_entry::set_isotope(const isotope & iso_, bool owned_)
  {
    reset_iso();
    _iso_ptr_ = &iso_;
    _owned_ = owned_;
    return;
  }

  void iso_entry::set_isotope(const isotope * iso_, bool owned_)
  {
    reset_iso();
    _iso_ptr_ = iso_;
    _owned_ = owned_;
    return;
  }

  bool iso_entry::has_isotope() const
  {
    return _iso_ptr_ != 0;
  }

  const isotope & iso_entry::get_isotope() const
  {
    DT_THROW_IF (!has_isotope(),std::logic_error, "Not a compound isotope !");
    return * _iso_ptr_;
  }

  bool element::is_built_by_isotopic_composition() const
  {
    return (is_locked() && _composition_.size() > 0);
  }

  void element::_set_defaults()
  {
    _locked_ = false;
    _name_   = "?";
    _symbol_ = "?";
    _z_ = Z_UNDEFINED;
    datatools::invalidate(_molar_mass_);
    return;
  }

  element::element()
  {
    _set_defaults();
    return;
  }

  element::element(const std::string & name_, int z_, unsigned int build_flags_)
  {
    _set_defaults();
    set_name(name_);
    set_z(z_);
    build(build_flags_);
    return;
  }

  element::element(const char * name_, int z_, unsigned int build_flags_)
  {
    _set_defaults();
    set_name(std::string(name_));
    set_z(z_);
    build(build_flags_);
    return;
  }

  element::element(const std::string & name_, int z_, double molar_mass_, unsigned int build_flags_)
  {
    _set_defaults();
    set_name(name_);
    set_z(z_);
    set_molar_mass(molar_mass_);
    build(build_flags_);
    return;
  }

  element::element(const char * name_, int z_, double molar_mass_, unsigned int build_flags_)
  {
    _set_defaults();
    set_name(std::string(name_));
    set_z(z_);
    set_molar_mass(molar_mass_);
    build(build_flags_);
    return;
  }

  element::element(const std::string & name_, const std::string & symbol_, unsigned int build_flags_)
  {
    _set_defaults();
    set_name(name_);
    set_z(symbol_);
    build(build_flags_);
    return;
  }

  element::element(const char * name_, const char * symbol_, unsigned int build_flags_)
  {
    _set_defaults();
    set_name(std::string(name_));
    set_z(std::string(symbol_));
    build(build_flags_);
    return;
  }

  const isotope_weight_map_type & element::get_composition () const
  {
    return _composition_;
  }

  element::~element()
  {
    _composition_.clear ();
    return;
  }

  void element::reset()
  {
    _composition_.clear ();
    _set_defaults();
    return;
  }

  bool element::is_locked() const
  {
    return _locked_;
  }

  const std::string & element::get_name() const
  {
    return _name_;
  }

  const std::string & element::get_symbol() const
  {
    return _symbol_;
  }

  int element::get_z() const
  {
    return _z_;
  }

  double element::get_molar_mass() const
  {
    return _molar_mass_;
  }

  const datatools::properties & element::get_properties() const
  {
    return _properties_;
  }

  datatools::properties & element::grab_properties()
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");
    return _properties_;
  }

  void element::lock()
  {
    _locked_ = true;
    return;
  }

  void element::unlock()
  {
    _locked_ = false;
    return;
  }

  void element::set_name(const std::string & name_)
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");
    _name_ = name_;
    return;
  }

  void element::set_z(const std::string & symbol_)
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");
    DT_THROW_IF(! chemical_symbol::symbol_is_tabulated(symbol_),
                std::logic_error,
                "Symbol '" << symbol_<< "' not found !");
    int z = chemical_symbol::z_from_symbol(symbol_);
    DT_THROW_IF(z == 0, std::logic_error, "Neutron is not possible !");
    _z_ = z;
    _symbol_ = symbol_;
    return;
  }

  void element::set_z(const int z_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Operation not allowed ! Object is locked !");
    DT_THROW_IF(! chemical_symbol::z_is_tabulated(z_) || z_ == 0, std::logic_error, "Invalid Z value : '" << z_ << "' !");
    _z_ = z_;
    _symbol_ = chemical_symbol::symbol_from_z(_z_);
    return;
  }

  void element::add_isotope(const isotope & iso_ref_, double weight_, bool owned_)
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");

    // 2010-10-11 by FM: not compatible with molar mass manual setting:
    DT_THROW_IF ((_molar_mass_ > 0.0) && (_composition_.size () == 0),
                 std::logic_error,
                 "Operation not allowed ! Molar mass is set by hand !");

    // Set Z and symbol when add a new isotope:
    if ( _composition_.size () == 0) {
      if (_z_ != Z_UNDEFINED) {
        DT_THROW_IF (_z_ != iso_ref_.get_z(),
                     std::logic_error,
                     "Unmatching mother element/daughter isotope Z values !");
      }
      set_z(iso_ref_.get_z ());
    }

    // Check if the isotope is locked (which mean valid):
    DT_THROW_IF (! iso_ref_.is_locked (),
                 std::logic_error,
                 "Isotope '" << iso_ref_.get_name () << "' is not locked !'");

    // Check if Z value of isotope to be added is consistant with Z value of the element:
    DT_THROW_IF ( iso_ref_.get_z () != _z_,
                  std::logic_error,
                  "The Z value '" << iso_ref_.get_z () <<"' of isotope '" << iso_ref_.get_name ()
                  << "' to be added is not consistant with the Z value '"
                  << _z_ << "' of the element '" << get_name () << "' !");

    // Check the positive value of weight:
    DT_THROW_IF (weight_ <= 0.,
                 std::logic_error,
                 "weight value '" << weight_ << "' is not positive !");

    // Check if isotope is already in the composition of the element:
    DT_THROW_IF (_composition_.find(iso_ref_.get_name()) != _composition_.end(),
                 std::logic_error,
                 "Adding the isotope '" << iso_ref_.get_name()
                 << "' which is already in the composition of element '" << _name_ <<"' !");

    // Insert new isotope entry in the composition map:
    {
      iso_entry dummy;
      _composition_[iso_ref_.get_name()] = dummy;
    }
    _composition_[iso_ref_.get_name()].set_isotope(iso_ref_, owned_);
    _composition_[iso_ref_.get_name()].set_weight(weight_);
    return;
  }

  void element::add_isotope(const isotope * iso_ptr_, double weight_)
  {
    add_isotope(*iso_ptr_, weight_, true);
    return;
  }


  void element::build(unsigned int build_flags_)
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");
    if ( build_flags_ & BF_NIST_COMPOSITION) {
      build_from_nist();
    }
    if (_composition_.size() > 0) {
      _norm_weights_();
      _compute_molar_mass_();
    } else if (datatools::is_valid(_molar_mass_)) {
      // ok
    } else {
      // DT_THROW_IF(true, std::logic_error, "Missing explicit isotope(s) in the current element ! Build from NIST database is not implemented yet !");
    }
    if (build_flags_ & BF_LOCK) {
      lock();
    }
    return;
  }

  void element::build_from_nist()
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");
    //-----------------  Open an ifstream from file atomic_weight_nist.dat  ----------------------------
    std::string tape_name = materials::get_resource("data/isotopic_compositions_nist.dat", true);
    std::ifstream ifstr_tape;
    ifstr_tape.open (tape_name.c_str ());
    DT_THROW_IF(! ifstr_tape.is_open (),
                std::logic_error,
                "Cannot open file '" << tape_name << "' !");

    //----------------------------  Read the ifstream  ----------------------------

    bool is_z_found  = false;
    _composition_.clear();
    _molar_mass_ = -1.;

    std::string nist_line;

    for (int i = 0 ; i <= 74 ; i++) {
      std::getline(ifstr_tape, nist_line);
    }
    /*
      bool stop = false;
      while (! stop)
      {
      std::string tmp;
      getline (ifstr_tape, tmp);
      istringstream iss (tmp);
      std::string word;
      iss >> word;
      if (word.empty ()) continue;
      else if (word[0] != '#') continue;
      else stop = true;
      }
    */

    while ( !is_z_found && getline(ifstr_tape,nist_line)) {
      if( atoi((nist_line.substr(0, 4)).c_str()) == _z_
          && nist_line.find(chemical_symbol::symbol_from_z(_z_)) != std::string::npos) {
        // std::clog << nist_line.substr(4, 4) << std::endl;
        is_z_found = true ;
      }
    }

    if (! is_z_found) {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                       "Z values  '" << _z_
                       << "' not found in file 'isotopic_compositions_nist.dat' !");
        // throw logic_error (message.str ());
    }
    return;
  }

  void element::_norm_weights_()
  {
    double norm = 0.;
    for (isotope_weight_map_type::const_iterator i = _composition_.begin();
         i != _composition_.end();
         i++) {
      const iso_entry & e = i->second;
      norm += e.get_weight();
    }

    DT_THROW_IF (norm <= 0.,
                 std::logic_error,
                 "Sum of weights for isotope '" << get_name() <<"' is negative !'");
    if (norm != 1.0) {
      for (isotope_weight_map_type::iterator i = _composition_.begin();
           i != _composition_.end();
           i++) {
        iso_entry & e = i->second;
        e.set_weight(e.get_weight() / norm);
      }
    }
    return;
  }

  void element::set_molar_mass(double molar_mass_)
  {
    DT_THROW_IF(is_locked(),std::logic_error,"Operation not allowed ! Object is locked !");
    DT_THROW_IF (_composition_.size() > 0,
                 std::logic_error,
                 "Operation not allowed when build mode uses isotopic composition !");
    DT_THROW_IF (molar_mass_ < 0.0,
                 std::logic_error,
                 "Invalid mass value : '" << molar_mass_ << "' !");
    _molar_mass_ = molar_mass_;
    return;
  }

  void element::_set_molar_mass_(const double molar_mass_)
  {
    DT_THROW_IF (molar_mass_ < 0,
                 std::logic_error,
                 "Invalid mass value : '" << molar_mass_ << "' !");
    _molar_mass_ = molar_mass_;
    return;
  }

  void element::_compute_molar_mass_()
  {
    double molar_mass =0.;

    for (isotope_weight_map_type::const_iterator i = _composition_.begin();
         i != _composition_.end();
         i++) {
        const iso_entry & entry = i->second;
        molar_mass += (entry.get_weight() * entry.get_isotope().get_atomic_mass());
      }
    _set_molar_mass_(molar_mass);
    return;
  }

  void element::tree_dump(std::ostream & out_,
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
         << "Name         : '"<< get_name() << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Chem. symbol : '"<< get_symbol() << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z            : " << get_z() << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Composition  : ";
    if ( _composition_.size() == 0) {
      out_ << "<empty>" << std::endl;
    } else {
      out_ << std::endl;
      for (isotope_weight_map_type::const_iterator i = _composition_.begin ();
           i !=  _composition_.end();
           i++) {
        const iso_entry & entry = i->second;
        isotope_weight_map_type::const_iterator j = i;
        j++;
        std::string atag = datatools::i_tree_dumpable::tags::item();
        if (j == _composition_.end()) {
          atag = datatools::i_tree_dumpable::tags::last_item();
        }
        out_  << indent
              << i_tree_dumpable::skip_tag
              << atag
              << "Isotope '" << entry.get_isotope().get_name () << "'" << " : "
              << entry.get_weight() << " "
              << (entry.is_owned()? "(owned)": "(not owned)") << std::endl;
      }
    }

    out_ << indent << i_tree_dumpable::tag
         << "Molar mass (A) : ";
    if (datatools::is_valid(_molar_mass_)) {
      out_ << _molar_mass_ << " [g/mol]";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

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
        _properties_.tree_dump (out_, "", indent_oss.str());
      }
    }

    out_ << indent << i_tree_dumpable::last_tag
         << "Locked       : " << (is_locked() ? "Yes": "No") << std::endl;

    return;
  }

} // end of namespace materials
